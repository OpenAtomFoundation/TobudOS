/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "sdk_common.h"

#if NRF_MODULE_ENABLED(NRF_FSTORAGE)

#include "nrf_fstorage_sd.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nordic_common.h"


#if (NRF_FSTORAGE_SD_MAX_WRITE_SIZE % 4)
    #error NRF_FSTORAGE_SD_MAX_WRITE_SIZE must be a multiple of the word size.
#endif


/**@brief   fstorage operation codes. */
typedef enum
{
    NRF_FSTORAGE_OP_WRITE,  //!< Write bytes to flash.
    NRF_FSTORAGE_OP_ERASE   //!< Erase flash pages.
} nrf_fstorage_sd_opcode_t;


ANON_UNIONS_ENABLE
/**@brief   fstorage operation queue element. */
typedef struct
{
    nrf_fstorage_t           const * p_fs;     //!< The fstorage instance that requested the operation.
    nrf_fstorage_sd_opcode_t         op_code;  //!< Requested operation.
    void                           * p_param;  //!< User-defined parameter passed to the event handler.
    union
    {
        struct
        {
            void     const * p_src;     //!< Data to be written to flash.
            uint32_t         dest;      //!< Destination of the data in flash.
            uint32_t         len;       //!< Length of the data to be written (in bytes).
            uint32_t         offset;    //!< Write offset.
        } write;
        struct
        {
            uint32_t page;              //!< Physical page number.
            uint32_t progress;          //!< Number of pages erased.
            uint32_t pages_to_erase;    //!< Total number of pages to erase.
        } erase;
    };
} nrf_fstorage_sd_op_t;
ANON_UNIONS_DISABLE


/**@brief   Status flags. */
typedef struct
{
    bool flash_operation_ongoing;    //!< An operation initiated by fstorage is ongoing.
    bool flash_operation_pending;    //!< fstorage is waiting for a flash operation that was initiated by another module to complete.
    bool paused;
    bool sd_enabled;
} nrf_fstorage_sd_flags_t;


typedef struct
{
    nrf_fstorage_sd_op_t op[NRF_FSTORAGE_SD_QUEUE_SIZE];
    uint32_t             rp;
    uint32_t             cnt;
} nrf_fstorage_sd_queue_t;


/* API function prototypes. */
static ret_code_t init(nrf_fstorage_t *, void *);
static ret_code_t uninit(nrf_fstorage_t *, void *);
static ret_code_t read(nrf_fstorage_t const *, uint32_t, void *, uint32_t);
static ret_code_t write(nrf_fstorage_t const *, uint32_t, void const *, uint32_t, void *);
static ret_code_t erase(nrf_fstorage_t const *, uint32_t, uint32_t, void *);
static bool is_busy(nrf_fstorage_t const *);


bool nrf_fstorage_sdh_req_handler(nrf_sdh_req_evt_t, void *);
void nrf_fstorage_sdh_state_handler(nrf_sdh_state_evt_t, void *);
void nrf_fstorage_sys_evt_handler(uint32_t, void *);

NRF_SDH_REQUEST_OBSERVER(m_req_obs, 0) =
{
    .handler   = nrf_fstorage_sdh_req_handler,
    .p_context = NULL,
};

NRF_SDH_STATE_OBSERVER(m_state_obs, 0) =
{
    .handler   = nrf_fstorage_sdh_state_handler,
    .p_context = NULL,
};

NRF_SDH_SOC_OBSERVER(m_sys_obs, 0, nrf_fstorage_sys_evt_handler, NULL);


/* Flash information. */
static nrf_fstorage_info_t m_flash_info =
{
#if   defined(NRF51)
    .erase_unit = 1024,
#elif defined(NRF52_SERIES)
    .erase_unit = 4096,
#else
    #error Family not defined.
#endif
    .program_unit = 4,
    // .erased_value = 0xFF,
    // .synchronous  = false,
};


/* API implementation. */
nrf_fstorage_api_t nrf_fstorage_sd =
{
    .init    = init,
    .uninit  = uninit,
    .read    = read,
    .write   = write,
    .erase   = erase,
    .is_busy = is_busy
};


static nrf_fstorage_sd_queue_t m_queue;
static nrf_fstorage_sd_flags_t m_flags;
static uint32_t                m_retry_cnt;


/* Sends events to the application. */
static void event_send(nrf_fstorage_sd_op_t const * p_op, ret_code_t result)
{
    if (p_op->p_fs->evt_handler == NULL)
    {
        /* Nothing to do. */
        return;
    }

    nrf_fstorage_evt_t evt;
    memset(&evt, 0x00, sizeof(evt));

    evt.result  = result;
    evt.p_param = p_op->p_param;

    switch (p_op->op_code)
    {
        case NRF_FSTORAGE_OP_WRITE:
            evt.id   = NRF_FSTORAGE_EVT_WRITE_RESULT;
            evt.addr = p_op->write.dest;
            evt.len  = p_op->write.len;
            break;

        case NRF_FSTORAGE_OP_ERASE:
            evt.id   = NRF_FSTORAGE_EVT_ERASE_RESULT;
            evt.addr = (p_op->erase.page * m_flash_info.erase_unit);
            evt.len  = p_op->erase.pages_to_erase;
            break;

        default:
            /* Should not happen. */
            break;
    }

    p_op->p_fs->evt_handler(&evt);
}


/* Write to flash. */
static uint32_t write_execute(nrf_fstorage_sd_op_t const * p_op)
{
    uint32_t chunk_len;

    chunk_len = MIN(p_op->write.len - p_op->write.offset, NRF_FSTORAGE_SD_MAX_WRITE_SIZE);
    chunk_len = MAX(1, chunk_len / m_flash_info.program_unit);

    /* Cast to p_src to uint32_t to perform arithmetic. */
    uint32_t       * p_dest = (uint32_t*)(p_op->write.dest + p_op->write.offset);
    uint32_t const * p_src  = (uint32_t*)((uint32_t)p_op->write.p_src + p_op->write.offset);

    return sd_flash_write(p_dest, p_src, chunk_len);
}


/* Erase flash page(s). */
static uint32_t erase_execute(nrf_fstorage_sd_op_t const * p_op)
{
    return sd_flash_page_erase(p_op->erase.page + p_op->erase.progress);
}


/* Advance the queue, wrapping around if necessary. */
static void queue_advance()
{
    m_queue.cnt--;
    m_queue.rp++;

    if (m_queue.rp == NRF_FSTORAGE_SD_QUEUE_SIZE)
    {
        m_queue.rp = 0;
    }
}


/* Execute the current operation in the queue. */
static void queue_process()
{
    if (m_queue.cnt == 0)
    {
        /* Nothing to do. */
        return;
    }

    /* Get the current operation from the queue and execute it. */
    nrf_fstorage_sd_op_t * const p_op = &m_queue.op[m_queue.rp];
    m_flags.flash_operation_ongoing = true;

    uint32_t rc;
    switch (p_op->op_code)
    {
        case NRF_FSTORAGE_OP_WRITE:
            rc = write_execute(p_op);
            break;

        case NRF_FSTORAGE_OP_ERASE:
            rc = erase_execute(p_op);
            break;

         default:
            rc = NRF_ERROR_INTERNAL;
            break;
    }

    switch (rc)
    {
        case NRF_ERROR_BUSY:
            /* There is a pending flash operation which was not initiated by this module.
             * Stop processing the queue until a system event is received. */
            m_flags.flash_operation_ongoing = false;
            m_flags.flash_operation_pending = true;
            break;

        case NRF_SUCCESS:
            /* The operation was accepted by the SoftDevice.
             * If the SOftDevice is enabled, wait for a system event.
             * Otherwise, the SoftDevice call is synchronous and will not send an event. */
            if (!m_flags.sd_enabled)
            {
                nrf_fstorage_sys_evt_handler(NRF_EVT_FLASH_OPERATION_SUCCESS, NULL);
            }
            break;

        default:
            /* Another error has occurred. */
            event_send(p_op, NRF_ERROR_INTERNAL);
            break;
    }
}


/* Start processing the queue if there are no pending flash operations,
 * both inside and outside this module. */
static void queue_start()
{
    if ((!m_flags.flash_operation_ongoing) &&
        (!m_flags.flash_operation_pending))
    {
        queue_process();
    }
}


/* Flash operation success callback. Keeps track of the progress of an operation.
 * If it has finished, advance the queue and send an event. */
static bool on_operation_success(nrf_fstorage_sd_op_t * p_op)
{
    /* Reset the retry counter on success. */
    m_retry_cnt = 0;

    switch (p_op->op_code)
    {
        case NRF_FSTORAGE_OP_WRITE:
        {
            /* The offset is updated only if the operation is successful
             * so that it can be retried in case it times out instead. */
            uint32_t const chunk_len = MIN(p_op->write.len - p_op->write.offset,
                                           NRF_FSTORAGE_SD_MAX_WRITE_SIZE);

            p_op->write.offset += chunk_len;

            if (p_op->write.offset == p_op->write.len)
            {
                return true;
            }
        }
        break;

        case NRF_FSTORAGE_OP_ERASE:
        {
            p_op->erase.progress++;

            if (p_op->erase.progress == p_op->erase.pages_to_erase)
            {
                return true;
            }
        }
        break;

        default:
            /* Should not happen. */
            break;
    }

    return false;
}


/* Flash operation failure callback. If the maximum number of retries has
 * been reached, send an event and advance the queue. */
static bool on_operation_failure(nrf_fstorage_sd_op_t const * p_op)
{
    if (++m_retry_cnt > NRF_FSTORAGE_SD_MAX_RETRIES)
    {
        m_retry_cnt = 0;
        return true;
    }

    return false;
}


/* Retrieves a pointer to the next free element in the queue. */
static bool queue_get_next_free(nrf_fstorage_sd_op_t ** p_op)
{
    uint32_t idx;

    if (m_queue.cnt == NRF_FSTORAGE_SD_QUEUE_SIZE)
    {
        return false;
    }

    idx = ((m_queue.rp + m_queue.cnt) < NRF_FSTORAGE_SD_QUEUE_SIZE) ?
           (m_queue.rp + m_queue.cnt) :
           (m_queue.rp + m_queue.cnt) - NRF_FSTORAGE_SD_QUEUE_SIZE;

    m_queue.cnt++;

    memset((void*)&m_queue.op[idx], 0x00, sizeof(nrf_fstorage_sd_op_t));
    *p_op = &m_queue.op[idx];

    return true;
}


static ret_code_t init(nrf_fstorage_t * p_fs, void * p_param)
{
    (void) p_param;

    p_fs->p_flash_info = &m_flash_info;

    memset(&m_flags, 0x00, sizeof(m_flags));

    #if (defined(NRF_SDH_ENABLED) && NRF_SDH_ENABLED)
        m_flags.sd_enabled = nrf_sdh_is_enabled();
    #endif

    /* Reset the queue. */
    memset(m_queue.op, 0x00, ARRAY_SIZE(m_queue.op));
    m_queue.rp  = 0;
    m_queue.cnt = 0;

    m_retry_cnt = 0;

    return NRF_SUCCESS;
}


static ret_code_t uninit(nrf_fstorage_t * p_fs, void * p_param)
{
    (void) p_param;

    /* The state is re-initialized upon init().
     * The common uninitialization code is run by the caller.
     * Nothing to do.
     */

    return NRF_SUCCESS;
}


static ret_code_t write(nrf_fstorage_t const * p_fs,
                        uint32_t               dest,
                        void           const * p_src,
                        uint32_t               len,
                        void                 * p_param)
{
    nrf_fstorage_sd_op_t * p_op;

    if (!queue_get_next_free(&p_op))
    {
        return NRF_ERROR_NO_MEM;
    }

    /* Initialize the operation. */
    p_op->op_code     = NRF_FSTORAGE_OP_WRITE;
    p_op->p_fs        = p_fs;
    p_op->p_param     = p_param;
    p_op->write.dest  = dest;
    p_op->write.p_src = p_src;
    p_op->write.len   = len;

    queue_start();

    return NRF_SUCCESS;
}


static ret_code_t read(nrf_fstorage_t const * p_fs, uint32_t src, void * p_dest, uint32_t len)
{
    memcpy(p_dest, (uint32_t*)src, len);

    return NRF_SUCCESS;
}


static ret_code_t erase(nrf_fstorage_t const * p_fs,
                        uint32_t               page_addr,
                        uint32_t               len,
                        void                 * p_param)
{
    nrf_fstorage_sd_op_t * p_op;

    if (m_flags.paused)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (!queue_get_next_free(&p_op))
    {
        return NRF_ERROR_NO_MEM;
    }

    /* Initialize the operation. */
    p_op->op_code              = NRF_FSTORAGE_OP_ERASE;
    p_op->p_fs                 = p_fs;
    p_op->p_param              = p_param;
    p_op->erase.page           = (page_addr / m_flash_info.erase_unit);
    p_op->erase.pages_to_erase = len;

    queue_start();

    return NRF_SUCCESS;
}


static bool is_busy(nrf_fstorage_t const * p_fs)
{
    return (m_queue.cnt != 0);
}


void nrf_fstorage_sys_evt_handler(uint32_t sys_evt, void * p_context)
{
    (void) p_context;

    if (m_flags.flash_operation_ongoing)
    {
        /* Handle the result of a flash operation initiated by this module. */
        bool operation_finished = false;
        nrf_fstorage_sd_op_t * p_op = &m_queue.op[m_queue.rp];

        switch (sys_evt)
        {
            case NRF_EVT_FLASH_OPERATION_SUCCESS:
                operation_finished = on_operation_success(p_op);
                break;

            case NRF_EVT_FLASH_OPERATION_ERROR:
                operation_finished = on_operation_failure(p_op);
                break;
        }

        if (operation_finished)
        {
            nrf_fstorage_sd_op_t op;

            /* Copy the current operation, to allow the queue element to be re-used. */
            memcpy(&op, p_op, sizeof(op));
            /* Free the queue element, so that new operations can be queued.*/
            queue_advance();

            m_flags.flash_operation_ongoing = false;

            event_send(&op, (sys_evt == NRF_EVT_FLASH_OPERATION_SUCCESS) ?
                            NRF_SUCCESS : NRF_ERROR_TIMEOUT);
        }
    }
    else if (m_flags.flash_operation_pending)
    {
        /* A flash operation was initiated outside this module.
         * A callback which indicates that it has finished was received. */
        m_flags.flash_operation_pending = false;
    }

    if (!m_flags.paused)
    {
        /* Resume processing the queue, if necessary. */
        queue_process();
    }
    else
    {
        /* Flash operation has completed.
         * Allow the SoftDevice to change state. */
         (void) nrf_sdh_request_continue();
    }
}


bool nrf_fstorage_sdh_req_handler(nrf_sdh_req_evt_t req, void * p_context)
{
    switch (req)
    {
        case NRF_SDH_EVT_ENABLE_REQUEST:
        case NRF_SDH_EVT_DISABLE_REQUEST:
        {
            m_flags.paused = true;

            /* If there are any operations ongoing, pause the SoftDevice state change.
             * Otherwise, go through with it. */
            if (m_flags.flash_operation_ongoing || m_flags.flash_operation_pending)
            {
                return false;
            }
        } break;

        default:
            break;
    }

    return true;
}


void nrf_fstorage_sdh_state_handler(nrf_sdh_state_evt_t state, void * p_context)
{
    switch (state)
    {
        case NRF_SDH_EVT_STATE_ENABLED:
            /* Fallthrough. */
        case NRF_SDH_EVT_STATE_DISABLED:
        {
            m_flags.paused     = false;
            m_flags.sd_enabled = (state == NRF_SDH_EVT_STATE_ENABLED);
            // Execute any operations still in the queue.
            queue_process();
        } break;

        default:
            break;
    }
}


#endif // NRF_FSTORAGE_ENABLED
