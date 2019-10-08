/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2012
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
/*
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <stdint.h>
#include <stdio.h>
#include "mem.h"
#include "bsp.h"
#include "antfs.h"
#include "app_error.h"
#include "app_timer.h"
#include "hardfault.h"
#include "ant_parameters.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_TIMER_OP_QUEUE_SIZE     0x04                            /**< Size of timer operation queues. */
#define APP_ANT_OBSERVER_PRIO       1                               /**< Application's ANT observer priority. You shouldn't need to modify this value. */

static const uint8_t m_friendly_name[] = "Ref Design";              /**< Client's friendly name. */
static const uint8_t m_pass_key[]      = {0x01, 0x02, 0x03, 0x04,\
                                          0x05, 0x06, 0x07, 0x08,\
                                          0x09, 0x0A, 0x0B, 0x0C,\
                                          0x0D, 0x0E, 0x0F, 0x10};  /**< Authentication string (passkey). */

static antfs_dir_struct_t       m_temp_dir_structure;               /**< Current directory file structure. */
static antfs_request_info_t     m_response_info;                    /**< Parameters for response to a download and upload request. */
static uint16_t                 m_file_index;                       /**< Index of the current file downloaded/uploaded. */
static uint32_t                 m_file_offset;                      /**< Current offset. */
static uint16_t                 m_current_crc;                      /**< Current CRC. */
static bool                     m_upload_success;                   /**< Upload response. */


/**@brief Function for processing ANTFS pairing request event.
 */
static void event_pairing_request_handle(void)
{
    const char * p_name = antfs_hostname_get();
    if (p_name != NULL)
    {
        NRF_LOG_INFO("host name: %s", (uint32_t)p_name);
    }
}


/**@brief Function for processing ANTFS download request event.
 *
 * @param[in] p_event The event extracted from the queue to be processed.
 */
static void event_download_request_handle(const antfs_event_return_t * p_event)
{
    uint8_t response = RESPONSE_MESSAGE_OK;

    // Grab request info.
    m_file_index = p_event->file_index;

    // Read file information from directory
    if (mem_file_info_get(m_file_index, &m_temp_dir_structure))
    {
        // Check permissions.
        if (!(m_temp_dir_structure.general_flags & ANTFS_DIR_READ_MASK))
        {
            response = RESPONSE_MESSAGE_NOT_AVAILABLE;
            NRF_LOG_INFO("Download request denied: file n/a for reading");
        }

        // Set response parameters.
        m_response_info.file_index.data           = m_file_index;
        // File size (per directory).
        m_response_info.file_size.data            = m_temp_dir_structure.file_size_in_bytes;
        // File is being read, so maximum size is the file size.
        m_response_info.max_file_size             = m_temp_dir_structure.file_size_in_bytes;
        // Send the entire file in a single block if possible.
        m_response_info.max_burst_block_size.data = m_temp_dir_structure.file_size_in_bytes;
    }
    else // Index not found.
    {
        response                                  = RESPONSE_MESSAGE_NOT_EXIST;
        m_response_info.file_index.data           = 0;
        m_response_info.file_size.data            = 0;
        m_response_info.max_file_size             = 0;
        m_response_info.max_burst_block_size.data = 0;
        NRF_LOG_INFO("Download request denied: file does not exist");
    }

    antfs_download_req_resp_prepare(response, &m_response_info);
}


/**@brief Function for processing ANTFS download data event.
 *
 * @param[in] p_event The event extracted from the queue to be processed.
 */
static void event_download_data_handle(const antfs_event_return_t * p_event)
{
    // This example does not interact with a file system, and it does not account for latency for
    // reading or writing a file from EEPROM/flash. Prefetching the file might be useful to feed the
    // data to download in order to maintain the burst timing.
    if (m_file_index == p_event->file_index)
    {
        // Only send data for a file index matching the download request.

        // Burst data block size * 8 bytes per burst packet.
        uint8_t buffer[ANTFS_BURST_BLOCK_SIZE * 8];
        // Offset specified by client.
        const uint32_t offset     = p_event->offset;
        // Size of requested block of data.
        const uint32_t data_bytes = p_event->bytes;

        // Read block of data from memory.
        mem_file_read(m_file_index, offset, buffer, data_bytes);

        // @note: Suppress return value as no use case for handling it exists.
        UNUSED_RETURN_VALUE(antfs_input_data_download(m_file_index, offset, data_bytes, buffer));
    }
}


/**@brief Function for processing ANTFS upload request data event.
 *
 * @param[in] p_event The event extracted from the queue to be processed.
 */
static void event_upload_request_handle(const antfs_event_return_t * p_event)
{
    uint8_t response = RESPONSE_MESSAGE_OK;

    if ((p_event->offset == MAX_ULONG))
    {
        // Requesting to resume an upload.

        if (m_file_index != p_event->file_index)
        {
            // We do not have a save point for this file.
            m_file_offset = 0;
            m_current_crc = 0;
        }
    }
    else
    {
        // This is a new upload.

        // Use requested offset and reset CRC.
        m_file_offset = p_event->offset;
        m_current_crc = 0;
    }

    m_file_index = p_event->file_index;

    // Read file information from directory.
    if (mem_file_info_get(m_file_index, &m_temp_dir_structure))
    {
        // Check permissions.
        if (!(m_temp_dir_structure.general_flags & ANTFS_DIR_WRITE_MASK))
        {
            response = RESPONSE_MESSAGE_NOT_AVAILABLE;
            NRF_LOG_INFO("Upload request denied: file n/a for writing");
        }

        // Set response parameters.
        m_response_info.file_index.data           = m_file_index;
        // Current valid file size is the last offset written to the file.
        m_response_info.file_size.data            = m_file_offset;
        // Space available for writing is the file size, as specified on directory.
        m_response_info.max_file_size             = m_temp_dir_structure.file_size_in_bytes;
        // Get the entire file in a single burst if possible.
        m_response_info.max_burst_block_size.data = m_temp_dir_structure.file_size_in_bytes;
        // Last valid CRC.
        m_response_info.file_crc                  = m_current_crc;
    }
    else
    {
        // Index not found.
        response                                  = RESPONSE_MESSAGE_NOT_EXIST;
        m_response_info.file_index.data           = m_file_index;
        m_response_info.file_size.data            = 0;
        m_response_info.max_file_size             = 0;
        m_response_info.max_burst_block_size.data = 0;
        m_response_info.file_crc                  = 0;
        NRF_LOG_INFO("Upload request denied: file does not exist");
    }

    m_upload_success = true;

    // @note: Suppress return value as no use case for handling it exists.
    UNUSED_RETURN_VALUE(antfs_upload_req_resp_transmit(response, &m_response_info));
}


/**@brief Function for processing ANTFS upload data event.
 *
 * @param[in] p_event The event extracted from the queue to be processed.
 */
static void event_upload_data_handle(const antfs_event_return_t * p_event)
{
    // This example does not interact with a file system, and it does not account for latency for
    // reading or writing a file from EEPROM/flash. Buffering and other strategies might be useful
    // to handle a received upload, while maintaining the burst timing.
    if (m_upload_success && (m_file_index == p_event->file_index))
    {
        // Offset requested for upload.
        const uint32_t offset     = p_event->offset;
        // Size of current block of data.
        const uint32_t data_bytes = p_event->bytes;

        // Write data to file.
        if (!mem_file_write(m_file_index, offset, p_event->data, data_bytes))
        {
            // Failed to write the data to system; do not attempt to write any more data after this,
            // and set upload response as FAIL.
            m_upload_success = false;
            NRF_LOG_INFO("Failed to write file to system");
            NRF_LOG_INFO("Current offset %u, ", offset);
        }
        else
        {
            // Data was written successfully:
            // - update offset
            // - update current CRC.
            m_file_offset = offset + data_bytes;
            m_current_crc = p_event->crc;
        }
    }
}


/**@brief Function for processing ANTFS upload complete event.
 */
static void event_upload_complete_handle(void)
{
    NRF_LOG_INFO("ANTFS_EVENT_UPLOAD_COMPLETE");

    // @note: Suppress return value as no use case for handling it exists.
    UNUSED_RETURN_VALUE(antfs_upload_data_resp_transmit(m_upload_success));
    if (m_upload_success)
    {
        m_file_offset = 0;
    }
}


/**@brief Function for processing ANTFS erase request event.
 *
 * @param[in] p_event The event extracted from the queue to be processed.
 */
static void event_erase_request_handle(const antfs_event_return_t * p_event)
{
    uint8_t response = RESPONSE_MESSAGE_OK;
    m_file_index     = p_event->file_index;

    if (m_file_index != 0)
    {
        // Read file information from directory.
        if (mem_file_info_get(m_file_index, &m_temp_dir_structure))
        {
            // Check permissions.
            if (!(m_temp_dir_structure.general_flags & ANTFS_DIR_ERASE_MASK))
            {
                response = RESPONSE_MESSAGE_FAIL;
                NRF_LOG_INFO("Erase request denied: file n/a for erasing");
            }
            else
            {
                // Erase file.
                if (!mem_file_erase(m_file_index))
                {
                    response = RESPONSE_MESSAGE_FAIL;
                }
            }
        }
        else
        {
            // Index not found.
            response = RESPONSE_MESSAGE_FAIL;
            NRF_LOG_INFO("Erase request denied: file does not exist");
        }
    }
    else
    {
        // Should not delete the directory.
        response = RESPONSE_MESSAGE_FAIL;
        NRF_LOG_INFO("Erase request denied: can not delete directory");
    }

    antfs_erase_req_resp_transmit(response);
}


/**@brief Function for extracting and processing a ANTFS events.
 *
 */
static void antfs_event_extract_and_process(void)
{
    antfs_event_return_t antfs_event;

    while (antfs_event_extract(&antfs_event))
    {
        switch (antfs_event.event)
        {
            case ANTFS_EVENT_OPEN_COMPLETE:
                NRF_LOG_INFO("ANTFS_EVENT_OPEN_COMPLETE");
                break;

            case ANTFS_EVENT_CLOSE_COMPLETE:
                NRF_LOG_INFO("ANTFS_EVENT_CLOSE_COMPLETE");
                break;

            case ANTFS_EVENT_LINK:
                NRF_LOG_INFO("ANTFS_EVENT_LINK");
                break;

            case ANTFS_EVENT_AUTH:
                NRF_LOG_INFO("ANTFS_EVENT_AUTH");
                break;

            case ANTFS_EVENT_TRANS:
                NRF_LOG_INFO("ANTFS_EVENT_TRANS");
                break;

            case ANTFS_EVENT_PAIRING_REQUEST:
                NRF_LOG_INFO("ANTFS_EVENT_PAIRING_REQUEST");
                event_pairing_request_handle();
                break;

            case ANTFS_EVENT_PAIRING_TIMEOUT:
                NRF_LOG_INFO("ANTFS_EVENT_PAIRING_TIMEOUT");
                break;

            case ANTFS_EVENT_DOWNLOAD_REQUEST:
                NRF_LOG_INFO("ANTFS_EVENT_DOWNLOAD_REQUEST");
                event_download_request_handle(&antfs_event);
                break;

            case ANTFS_EVENT_DOWNLOAD_START:
                NRF_LOG_INFO("ANTFS_EVENT_DOWNLOAD_START");
                break;

            case ANTFS_EVENT_DOWNLOAD_REQUEST_DATA:
                event_download_data_handle(&antfs_event);
                break;

            case ANTFS_EVENT_DOWNLOAD_COMPLETE:
                NRF_LOG_INFO("ANTFS_EVENT_DOWNLOAD_COMPLETE");
                break;

            case ANTFS_EVENT_DOWNLOAD_FAIL:
                NRF_LOG_INFO("ANTFS_EVENT_DOWNLOAD_FAIL");
                break;

            case ANTFS_EVENT_UPLOAD_REQUEST:
                NRF_LOG_INFO("ANTFS_EVENT_UPLOAD_REQUEST");
                event_upload_request_handle(&antfs_event);
                break;

            case ANTFS_EVENT_UPLOAD_START:
                NRF_LOG_INFO("ANTFS_EVENT_UPLOAD_START");
                break;

            case ANTFS_EVENT_UPLOAD_DATA:
                NRF_LOG_INFO("ANTFS_EVENT_UPLOAD_DATA");
                event_upload_data_handle(&antfs_event);
                break;

            case ANTFS_EVENT_UPLOAD_FAIL:
                NRF_LOG_INFO("ANTFS_EVENT_UPLOAD_FAIL");
                // @note: Suppress return value as no use case for handling it exists.
                UNUSED_RETURN_VALUE(antfs_upload_data_resp_transmit(false));
                break;

            case ANTFS_EVENT_UPLOAD_COMPLETE:
                NRF_LOG_INFO("ANTFS_EVENT_UPLOAD_COMPLETE");
                event_upload_complete_handle();
                break;

            case ANTFS_EVENT_ERASE_REQUEST:
                NRF_LOG_INFO("ANTFS_EVENT_ERASE_REQUEST");
                event_erase_request_handle(&antfs_event);
                break;

            default:
                break;
        }
    }
}


/**@brief Function for handling bsp events.
 */
static void bsp_evt_handler(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_KEY_0:
            if (antfs_pairing_resp_transmit(true))
            {
                // @note: If pairing is supported by the implementation the only reason this code gets
                // executed would be if the protocol is in incorrect state, which would imply an error
                // either in the host or the client implementation.
            }
            break;

        case BSP_EVENT_KEY_1:
            UNUSED_RETURN_VALUE(antfs_pairing_resp_transmit(false));
            break;

        default:
            return;
    }

    // Process ANT-FS event queue.
    antfs_event_extract_and_process();
}


/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
static void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    antfs_message_process(p_ant_evt->message.aucMessage);
    antfs_event_extract_and_process();
}


NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);


/**@brief Function for configuring and setting up the SoftDevice.
 */
static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for setup all things not directly associated with ANT stack/protocol.
 * @desc Initialization of: @n
 *         - app_tarce for debug.
 *         - app_timer, presetup for bsp.
 */
static void utils_setup(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS,
                        bsp_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for application main entry, does not return.
 */
int main(void)
{
    log_init();
    utils_setup();
    softdevice_setup();

    const antfs_params_t params =
    {
        CLIENT_SERIAL_NUMBER,
        CLIENT_DEV_TYPE,
        CLIENT_MANUF_ID,
        ANTFS_LINK_FREQ,
        {ANTFS_DEFAULT_BEACON | DATA_AVAILABLE_FLAG_MASK},
        m_pass_key,
        m_friendly_name
    };

    antfs_init(&params, nrf_pwr_mgmt_run);
    antfs_channel_setup();

    NRF_LOG_INFO("ANT-FS Client example started.");

    for (;; )
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}
