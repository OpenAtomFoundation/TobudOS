/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2013
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
/**@file
 * @defgroup nrf_ant_background_scanning_demo ANT Background Scanning Example
 * @{
 * @ingroup nrf_ant_background_scanning_demo
 *
 * @brief Example of ANT Background Scanning implementation.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <stdio.h>
#include <stdlib.h>
#include "nrf.h"
#include "bsp.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "nrf_pwr_mgmt.h"
#include "ant_channel_config.h"
#include "ant_search_config.h"
#include "app_timer.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "sdk_config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define ANT_BEACON_PAGE             ((uint8_t) 1)
#define APP_MS_ANT_OBSERVER_PRIO    1
#define APP_BS_ANT_OBSERVER_PRIO    1

static uint8_t  m_last_rssi      = 0;
static uint16_t m_last_device_id = 0;
static uint8_t  m_received       = 0;

/**@brief Function for setting payload for ANT message and sending it via
 *        ANT master beacon channel.
 *
 *
 * @details   ANT_BEACON_PAGE message is queued. The format is:
 *            byte[0]   = page (1 = ANT_BEACON_PAGE)
 *            byte[1]   = last RSSI value received
 *            byte[2-3] = channel ID of device corresponding to last RSSI value (little endian)
 *            byte[6]   = counter that increases with every message period
 *            byte[7]   = number of messages received on background scanning channel
 */
void ant_message_send()
{
    uint32_t       err_code;
    uint8_t        tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    static uint8_t counter = 0;

    tx_buffer[0] = ANT_BEACON_PAGE;
    tx_buffer[1] = m_last_rssi;
    tx_buffer[2] = (uint8_t) LSB_16(m_last_device_id); // LSB
    tx_buffer[3] = (uint8_t) MSB_16(m_last_device_id); // MSB
    tx_buffer[6] = counter++;
    tx_buffer[7] = m_received;

    err_code = sd_ant_broadcast_message_tx(ANT_MS_CHANNEL_NUMBER,
                                           ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                           tx_buffer);
    APP_ERROR_CHECK(err_code);
}

/**@brief Initialize application.
 */
static void application_initialize()
{
    /* Set library config to report RSSI and Device ID */
    uint32_t err_code = sd_ant_lib_config_set(ANT_LIB_CONFIG_MESG_OUT_INC_RSSI
                                              | ANT_LIB_CONFIG_MESG_OUT_INC_DEVICE_ID);

    APP_ERROR_CHECK(err_code);

    const uint16_t dev_num = (uint16_t) (NRF_FICR->DEVICEID[0]);

    const ant_channel_config_t ms_channel_config =
    {
        .channel_number    = ANT_MS_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_MASTER,
        .ext_assign        = 0x00,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = dev_num,
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUM,
    };

    const ant_channel_config_t bs_channel_config =
    {
        .channel_number    = ANT_BS_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_SLAVE,
        .ext_assign        = EXT_PARAM_ALWAYS_SEARCH,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = 0x00,              // Wild card
        .channel_period    = 0x00,              // This is not taken into account.
        .network_number    = ANT_NETWORK_NUM,
    };

    const ant_search_config_t bs_search_config =
    {
        .channel_number        = ANT_BS_CHANNEL_NUMBER,
        .low_priority_timeout  = ANT_LOW_PRIORITY_TIMEOUT_DISABLE,
        .high_priority_timeout = ANT_HIGH_PRIORITY_SEARCH_DISABLE,
        .search_sharing_cycles = ANT_SEARCH_SHARING_CYCLES_DISABLE,
        .search_priority       = ANT_SEARCH_PRIORITY_DEFAULT,
        // The ANT Search Waveform must be set to the default while using high duty search
        .waveform = ANT_WAVEFORM_DEFAULT,
    };

    // Configure high duty search
    // Can only be done with all channels closed
    const ANT_HIGH_DUTY_SEARCH_CONFIG hconfig =
    {
        HIGH_DUTY_SEARCH_ENABLE,
        HIGH_DUTY_SEARCH_SUPPRESSION_DEFAULT,
        HIGH_DUTY_SEARCH_RESTART_INTERVAL_DEFAULT
    };

    err_code = sd_ant_high_duty_search_config_set(&hconfig);
    APP_ERROR_CHECK(err_code);

    err_code = ant_channel_init(&ms_channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = ant_channel_init(&bs_channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = ant_search_init(&bs_search_config);
    APP_ERROR_CHECK(err_code);

    // Fill tx buffer for the first frame
    ant_message_send();

    err_code = sd_ant_channel_open(ANT_MS_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_open(ANT_BS_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Tracer initialization.
 */
static void utils_setup(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = bsp_init(BSP_INIT_LEDS,
                        NULL);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**
 *@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for ANT stack initialization.
 *
 * @details Initializes the SoftDevice and the ANT event interrupt.
 */
static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);
}

/**@brief Process ANT message on ANT background scanning channel.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void background_scanner_process(ant_evt_t * p_ant_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ant_evt->event)
    {
        case EVENT_RX:
            err_code = bsp_indication_set(BSP_INDICATE_RCV_OK);
            APP_ERROR_CHECK(err_code);

            if (p_ant_evt->message.ANT_MESSAGE_stExtMesgBF.bANTDeviceID)
            {
                m_last_device_id = uint16_decode(p_ant_evt->message.ANT_MESSAGE_aucExtData);
            }

            if (p_ant_evt->message.ANT_MESSAGE_stExtMesgBF.bANTRssi)
            {
                m_last_rssi = p_ant_evt->message.ANT_MESSAGE_aucExtData[5];
            }

            NRF_LOG_INFO("Message number %d", m_received);
            NRF_LOG_INFO("Device ID:     %d", m_last_device_id);
            NRF_LOG_INFO("RSSI:          %d\n\r", (int8_t)m_last_rssi);

            m_received++;
            break;

        default:
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_bs_ant_observer, APP_BS_ANT_OBSERVER_PRIO, background_scanner_process, NULL);

/**@brief Process ANT message on ANT master beacon channel.
 *
 *
 * @details   This function handles all events on the master beacon channel.
 *            On EVENT_TX an ANT_BEACON_PAGE message is queued.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void master_beacon_process(ant_evt_t * p_ant_evt, void * p_context)
{
    switch (p_ant_evt->event)
    {
        case EVENT_TX:
            ant_message_send();
            break;

        default:
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ms_ant_observer, APP_MS_ANT_OBSERVER_PRIO, master_beacon_process, NULL);


/* Main function */
int main(void)
{
    log_init();
    utils_setup();
    softdevice_setup();
    application_initialize();

    NRF_LOG_INFO("ANT High Duty Search and Background Scanning example started.");

    // Enter main loop
    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}


/**
 *@}
 **/
