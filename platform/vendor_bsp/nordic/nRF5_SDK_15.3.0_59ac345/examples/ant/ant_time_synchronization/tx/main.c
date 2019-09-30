/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2016
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

#include <string.h>
#include "boards.h"
#include "app_error.h"
#include "nrf_drv_rtc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "nrf_pwr_mgmt.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "ant_channel_config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Arbitrary Page Numbers
#define TIME_SYNC_PAGE                  0x01
#define INVALID_PAGE                    0xFF

// Miscellaneous defines.
#define ANT_RTC_CHANNEL                 0
#define APP_ANT_OBSERVER_PRIO           1       /**< Application's ANT observer priority. You shouldn't need to modify this value. */

const nrf_drv_rtc_t m_rtc = NRF_DRV_RTC_INSTANCE(1); /**< Declaring an instance of nrf_drv_rtc for RTC1. */

static uint8_t m_led_status      = 0; /**< Current status of LEDs. */
static uint8_t m_led_invert_next = 0; /**< Index of the next LED to be inverted. */

/**@brief Function for setting payload for ANT message and sending it.
 */
void ant_time_sync_message_send()
{
    uint32_t err_code;
    uint8_t  message_payload[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    uint32_t counter;

    // Get the current RTC counter value
    counter = nrf_drv_rtc_counter_get(&m_rtc);

    // Set the RTC channel to interrupt again after the LED_INVERT_PERIOD
    err_code = nrf_drv_rtc_cc_set(&m_rtc, ANT_RTC_CHANNEL, counter + LED_INVERT_PERIOD, true);
    APP_ERROR_CHECK(err_code);

    // Assign a new value to the broadcast data.
    memset(message_payload, 0, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    message_payload[0] = TIME_SYNC_PAGE;
    message_payload[4] = m_led_invert_next;
    message_payload[5] = m_led_status;
    message_payload[6] = (uint8_t) LSB_32(counter);        // 2-byte counter LSB
    message_payload[7] = (uint8_t) LSB_32(counter >> 8);   // 2-byte counter MSB

    // Broadcast the data.
    err_code = sd_ant_time_sync_broadcast_tx(BROADCAST_CHANNEL_NUMBER,
                                             ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                             message_payload);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    nrf_pwr_mgmt_feed();
    if (p_ant_evt->channel == BROADCAST_CHANNEL_NUMBER)
    {
        switch (p_ant_evt->event)
        {
            case EVENT_TX:
                // Event called when message is sent
                break;

            default:
                break;
        }
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);


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


/**@brief Function for handling the LED's when RTC interrupt occurs.
 *
 */
static void led_event(void)
{
    bsp_board_led_invert(m_led_invert_next);

    m_led_status      ^= 1 << (m_led_invert_next++); // Update current status of LEDs
    m_led_invert_next %= LEDS_NUMBER;                // Update next LED to be inverted
}


/** @brief: Function for handling the RTC1 interrupts.
 * Triggered on COMPARE0 match.
 */
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        led_event();
        ant_time_sync_message_send();
    }
}


/**@brief Function for setting up the ANT module to be ready for TX broadcast.
 */
static void ant_channel_tx_broadcast_setup(void)
{
    uint32_t             err_code;
    ANT_TIME_SYNC_CONFIG syncConfig;

    // Configure ANT Time Synchronization using RTC1
    syncConfig.ucTimeBase           = ANT_TIME_BASE_ALT1; // RTC1
    syncConfig.bInvalidationEnabled = true;
    syncConfig.ucInvalidationByte   = INVALID_PAGE;       // Invalid page number set to 0xFF
    err_code                        = sd_ant_time_sync_config_set(&syncConfig);
    APP_ERROR_CHECK(err_code);

    // Configure ANT Channel
    ant_channel_config_t broadcast_channel_config =
    {
        .channel_number    = BROADCAST_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_MASTER,
        .ext_assign        = 0x00,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = CHAN_ID_DEV_NUM,
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUM,
    };

    err_code = ant_channel_init(&broadcast_channel_config);
    APP_ERROR_CHECK(err_code);

    // Open channel.
    err_code = sd_ant_channel_open(BROADCAST_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function initialization and configuration of RTC driver instance.
 */
static void rtc_config(void)
{
    uint32_t err_code;

    // Initialize RTC instance
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;

    err_code = nrf_drv_rtc_init(&m_rtc, &config, rtc_handler);
    APP_ERROR_CHECK(err_code);

    // Enable tick event & interrupt
    nrf_drv_rtc_tick_enable(&m_rtc, true);

    // Set compare channel to trigger interrupt after LED_INVERT_PERIOD
    err_code = nrf_drv_rtc_cc_set(&m_rtc, ANT_RTC_CHANNEL, LED_INVERT_PERIOD, true);
    APP_ERROR_CHECK(err_code);

    // Power on RTC instance
    nrf_drv_rtc_enable(&m_rtc);
}


/**@brief Function for the Power Manager initialization.
 */
static void utils_setup(void)
{
    ret_code_t err_code = nrf_pwr_mgmt_init();
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


/**@brief Function for application main entry. Does not return.
 */
int main(void)
{
    log_init();
    utils_setup();
    softdevice_setup();
    rtc_config();
    bsp_board_init(BSP_INIT_LEDS);
    ant_channel_tx_broadcast_setup();

    NRF_LOG_INFO("ANT Time Synchronization TX example started.");

    // Main loop.
    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}
