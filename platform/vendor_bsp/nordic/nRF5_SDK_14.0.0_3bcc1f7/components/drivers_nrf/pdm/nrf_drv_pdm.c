/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
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
#if NRF_MODULE_ENABLED(PDM)
#include "nrf_drv_pdm.h"
#include "nrf_assert.h"
#include "nrf_drv_common.h"
#include "nrf_gpio.h"

#define NRF_LOG_MODULE_NAME pdm

#if PDM_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL       PDM_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  PDM_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR PDM_CONFIG_DEBUG_COLOR
#define EVT_TO_STR(event)   (event == NRF_PDM_EVENT_STARTED ? "NRF_PDM_EVENT_STARTED" :                \
                            (event == NRF_PDM_EVENT_STOPPED ? "NRF_PDM_EVENT_STOPPED" :                  \
                            (event == NRF_PDM_EVENT_END ? "NRF_PDM_EVENT_END" : "UNKNOWN EVENT")))
#else //PDM_CONFIG_LOG_ENABLED
#define EVT_TO_STR(event)   ""
#define NRF_LOG_LEVEL       0
#endif //PDM_CONFIG_LOG_ENABLED
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();


/** @brief PDM interface status. */
typedef enum
{
    NRF_PDM_STATE_IDLE,
    NRF_PDM_STATE_RUNNING,
    NRF_PDM_STATE_STARTING,
    NRF_PDM_STATE_STOPPING
} nrf_drv_pdm_state_t;


/** @brief PDM interface control block.*/
typedef struct
{
    nrf_drv_pdm_event_handler_t  event_handler;    ///< Event handler function pointer.
    int16_t *                    buff_address[2];  ///< Sample buffers.
    uint16_t                     buff_length[2];   ///< Length of the sample buffers.
    nrf_drv_state_t              drv_state;        ///< Driver state.
    volatile nrf_drv_pdm_state_t op_state;         ///< PDM peripheral operation state.
    uint8_t                      active_buffer;    ///< Number of currently active buffer.
    uint8_t                      error;            ///< Driver error flag.
    uint8_t                      irq_priority;     ///< Interrupt priority.
    volatile uint8_t             irq_buff_request; ///< Request the next buffer in the ISR.
} nrf_drv_pdm_cb_t;


static nrf_drv_pdm_cb_t m_cb;


void PDM_IRQHandler(void)
{
    if (nrf_pdm_event_check(NRF_PDM_EVENT_STARTED))
    {
        nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
        NRF_LOG_DEBUG("Event: %s.", (uint32_t)EVT_TO_STR(NRF_PDM_EVENT_STARTED));

        nrf_drv_pdm_evt_t evt;
        uint8_t finished_buffer = m_cb.active_buffer;

        // Check if the next buffer was set before.
        uint8_t next_buffer = (~m_cb.active_buffer) & 0x01;
        if (m_cb.buff_address[next_buffer] || m_cb.op_state == NRF_PDM_STATE_STARTING)
        {
            evt.error = PDM_NO_ERROR;
            m_cb.error = 0;

            // Release the full buffer if ready and request the next one.
            if (m_cb.op_state == NRF_PDM_STATE_STARTING)
            {
                evt.buffer_released = 0;
                m_cb.op_state = NRF_PDM_STATE_RUNNING;
            }
            else
            {
                evt.buffer_released = m_cb.buff_address[finished_buffer];
                m_cb.buff_address[finished_buffer] = 0;
                m_cb.active_buffer = next_buffer;
            }
            evt.buffer_requested = true;
            m_cb.event_handler(&evt);
        }
        else
        {
            // No next buffer available. Report an error.
            // Do not request the new buffer as it was already done.
            if (m_cb.error == 0)
            {
                evt.error = PDM_ERROR_OVERFLOW;
                evt.buffer_released = 0;
                evt.buffer_requested = false;
                m_cb.error = 1;
                m_cb.event_handler(&evt);
            }
        }

        if (m_cb.op_state == NRF_PDM_STATE_STARTING)
        {
            m_cb.op_state = NRF_PDM_STATE_RUNNING;
        }
    }
    else if (nrf_pdm_event_check(NRF_PDM_EVENT_STOPPED))
    {
        nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
        NRF_LOG_DEBUG("Event: %s.", (uint32_t)EVT_TO_STR(NRF_PDM_EVENT_STOPPED));
        nrf_pdm_disable();
        m_cb.op_state = NRF_PDM_STATE_IDLE;

        // Release the buffers.
        nrf_drv_pdm_evt_t evt;
        evt.error = PDM_NO_ERROR;
        evt.buffer_requested = false;
        if (m_cb.buff_address[m_cb.active_buffer])
        {
            evt.buffer_released = m_cb.buff_address[m_cb.active_buffer];
            m_cb.buff_address[m_cb.active_buffer] = 0;
            m_cb.event_handler(&evt);
        }

        uint8_t second_buffer = (~m_cb.active_buffer) & 0x01;
        if (m_cb.buff_address[second_buffer])
        {
            evt.buffer_released = m_cb.buff_address[second_buffer];
            m_cb.buff_address[second_buffer] = 0;
            m_cb.event_handler(&evt);
        }
        m_cb.active_buffer = 0;


    }

    if (m_cb.irq_buff_request)
    {
        nrf_drv_pdm_evt_t evt;
        evt.buffer_released = 0;
        evt.error = PDM_NO_ERROR;
        evt.buffer_requested = true;
        m_cb.irq_buff_request = 0;
        m_cb.event_handler(&evt);
    }
}


ret_code_t nrf_drv_pdm_init(nrf_drv_pdm_config_t const * p_config,
                              nrf_drv_pdm_event_handler_t event_handler)
{
    ret_code_t err_code;

    if (m_cb.drv_state != NRF_DRV_STATE_UNINITIALIZED)
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_WARNING("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
        return err_code;
    }
    if ((p_config == NULL)
        || (event_handler == NULL))
    {
        err_code = NRF_ERROR_INVALID_PARAM;
        NRF_LOG_WARNING("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
        return err_code;
    }
    if (p_config->gain_l > NRF_PDM_GAIN_MAXIMUM
        || p_config->gain_r > NRF_PDM_GAIN_MAXIMUM)
    {
        err_code = NRF_ERROR_INVALID_PARAM;
        NRF_LOG_WARNING("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
        return err_code;
    }


    m_cb.buff_address[0] = 0;
    m_cb.buff_address[1] = 0;
    m_cb.active_buffer = 0;
    m_cb.error = 0;
    m_cb.event_handler = event_handler;
    m_cb.op_state = NRF_PDM_STATE_IDLE;

    nrf_pdm_clock_set(p_config->clock_freq);
    nrf_pdm_mode_set(p_config->mode, p_config->edge);
    nrf_pdm_gain_set(p_config->gain_l, p_config->gain_r);

    nrf_gpio_cfg_output(p_config->pin_clk);
    nrf_gpio_pin_clear(p_config->pin_clk);
    nrf_gpio_cfg_input(p_config->pin_din, NRF_GPIO_PIN_NOPULL);
    nrf_pdm_psel_connect(p_config->pin_clk, p_config->pin_din);

    nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
    nrf_pdm_event_clear(NRF_PDM_EVENT_END);
    nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
    nrf_pdm_int_enable(NRF_PDM_INT_STARTED | NRF_PDM_INT_STOPPED);
    m_cb.irq_priority = p_config->interrupt_priority;
    nrf_drv_common_irq_enable(PDM_IRQn, m_cb.irq_priority);
    m_cb.drv_state = NRF_DRV_STATE_INITIALIZED;

    err_code = NRF_SUCCESS;
    NRF_LOG_INFO("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
    return err_code;
}


void nrf_drv_pdm_uninit(void)
{
    nrf_pdm_disable();
    nrf_pdm_psel_disconnect();
    m_cb.drv_state = NRF_DRV_STATE_UNINITIALIZED;
    NRF_LOG_INFO("Uninitialized.");
}

static void pdm_start()
{
    m_cb.drv_state = NRF_DRV_STATE_POWERED_ON;
    nrf_pdm_enable();
    nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
    nrf_pdm_task_trigger(NRF_PDM_TASK_START);
}

static void pdm_buf_request()
{
    m_cb.irq_buff_request = 1;
    NVIC_SetPendingIRQ(PDM_IRQn);
}

ret_code_t nrf_drv_pdm_start(void)
{
    ASSERT(m_cb.drv_state != NRF_DRV_STATE_UNINITIALIZED);
    ret_code_t err_code;

    if (m_cb.op_state != NRF_PDM_STATE_IDLE)
    {
        if (m_cb.op_state == NRF_PDM_STATE_RUNNING)
        {
            err_code = NRF_SUCCESS;
            NRF_LOG_INFO("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
            return err_code;
        }
        err_code = NRF_ERROR_BUSY;
        NRF_LOG_WARNING("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
        return err_code;
    }

    m_cb.op_state = NRF_PDM_STATE_STARTING;
    pdm_buf_request();

    err_code = NRF_SUCCESS;
    NRF_LOG_INFO("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
    return err_code;
}

ret_code_t nrf_drv_pdm_buffer_set(int16_t * buffer, uint16_t buffer_length)
{
    if (m_cb.drv_state == NRF_DRV_STATE_UNINITIALIZED)
    {
        return NRF_ERROR_INVALID_STATE;
    }
    if (m_cb.op_state == NRF_PDM_STATE_STOPPING)
    {
        return NRF_ERROR_BUSY;
    }
    if ((!buffer) || (buffer_length > NRF_PDM_MAX_BUFFER_SIZE))
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    ret_code_t err_code = NRF_SUCCESS;

    // Enter the PDM critical section.
    nrf_drv_common_irq_disable(PDM_IRQn);

    uint8_t next_buffer = (~m_cb.active_buffer) & 0x01;
    if (m_cb.op_state == NRF_PDM_STATE_STARTING)
    {
        next_buffer = 0;
    }

    if (m_cb.buff_address[next_buffer])
    {
        // Buffer already set.
        err_code = NRF_ERROR_BUSY;
    }
    else
    {
        m_cb.buff_address[next_buffer] = buffer;
        m_cb.buff_length[next_buffer] = buffer_length;
        nrf_pdm_buffer_set((uint32_t *)buffer, buffer_length);

        if (m_cb.drv_state != NRF_DRV_STATE_POWERED_ON)
        {
                pdm_start();
        }
    }

    nrf_drv_common_irq_enable(PDM_IRQn, m_cb.irq_priority);
    return err_code;
}

ret_code_t nrf_drv_pdm_stop(void)
{
    ASSERT(m_cb.drv_state != NRF_DRV_STATE_UNINITIALIZED);
    ret_code_t err_code;

    if (m_cb.op_state != NRF_PDM_STATE_RUNNING)
    {
        if (m_cb.op_state == NRF_PDM_STATE_IDLE || m_cb.op_state == NRF_PDM_STATE_STARTING)
        {
            nrf_pdm_disable();
            m_cb.op_state = NRF_PDM_STATE_IDLE;
            err_code = NRF_SUCCESS;
            NRF_LOG_INFO("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
            return err_code;
        }
        err_code = NRF_ERROR_BUSY;
        NRF_LOG_WARNING("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
        return err_code;
    }
    m_cb.drv_state = NRF_DRV_STATE_INITIALIZED;
    m_cb.op_state = NRF_PDM_STATE_STOPPING;

    nrf_pdm_task_trigger(NRF_PDM_TASK_STOP);
    err_code = NRF_SUCCESS;
    NRF_LOG_INFO("Function: %s, error code: %s.", (uint32_t)__func__, (uint32_t)NRF_LOG_ERROR_STRING_GET(err_code));
    return err_code;
}

#endif //NRF_MODULE_ENABLED(PDM)
