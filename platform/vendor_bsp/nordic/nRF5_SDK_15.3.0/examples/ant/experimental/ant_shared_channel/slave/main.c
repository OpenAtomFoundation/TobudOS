/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2014
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
 * @defgroup ant_shared_channel_slave_demo ANT Auto Shared Slave Example
 * @{
 * @ingroup ant_shared_channel
 *
 * @brief Example of ANT Auto Shared Channel (ASC) Slave.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <stdint.h>
#include "ant_interface.h"
#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "ant_parameters.h"
#include "asc_events.h"
#include "asc_parameters.h"
#include "asc_slave.h"
#include "boards.h"
#include "hardfault.h"
#include "leds.h"
#include "nrf_delay.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Channel configuration.
#define ASCS_CHANNEL                    0x00u               /**< ANT Channel 0. */
#define ANT_PUBLIC_NETWORK_NUMBER       0x00u               /**< ANT Public/Default Network Key. */
#define ASCS_RF_FREQUENCY               0x42u               /**< 2400 + 66Mhz. */
#define ASCS_CHANNEL_PERIOD             MSG_PERIOD_4HZ

// Channel ID configuration.
#define ASCS_DEVICE_TYPE                0x02u               /**< Device type. */
#define ASCS_DEVICE_NUMBER              0x00u               /**< Device number. */
#define ASCS_TX_TYPE                    0x00u               /**< Transmission type. */

// Miscellaneous defines.
#define LED_ERROR_0                     BSP_LED_0
#define LED_ERROR_1                     BSP_LED_1
#define LED_ERROR_2                     LED_INVALID
#define LED_ERROR_3                     LED_INVALID

#define LIGHT                           BSP_LED_0
#define POLLED_LED                      LED_INVALID
#define CONNECTED_LED                   BSP_LED_1

// One-second tick timer defines
#define RTC_EVENT_INTERVAL_MS           1000                /**< Event interval in milliseconds. */

#define APP_ANT_OBSERVER_PRIO           1                   /**< Application's ANT observer priority. You shouldn't need to modify this value. */

// Static variables and buffers.us
static uint16_t                 m_device_number = 0x00;
static const asc_ant_params_t   m_asc_parameters = {

    ASCS_CHANNEL,
    ANT_PUBLIC_NETWORK_NUMBER,
    &m_device_number,
    ASCS_DEVICE_TYPE,
    ASCS_TX_TYPE,
    ASCS_CHANNEL_PERIOD,
    CHANNEL_TYPE_SHARED_SLAVE,
    ASCS_RF_FREQUENCY,
    RADIO_TX_POWER_LVL_3
}; /**< Structure containing setup parameters for an auto shared slave. */
APP_TIMER_DEF(m_timer_id);


/**@brief  Function for checking event flags and handling them.
 */
static void check_and_handle_asc_flags(void)
{
    uint32_t asc_flags = ascs_events_get();

    do
    {
        if (asc_flags & EVENT_ASC_STATE_CHANGED)
        {

            ascs_event_clear(EVENT_ASC_STATE_CHANGED);

            //Determine which state changed and react
            ascs_states_t state = ascs_state_get();
            switch (state)
            {
                case ASSIGNED:
                {
                    led_on(CONNECTED_LED);
                    break;
                }
                case SEARCHING:     //Intentional fallthrough
                case REQUESTING:    //Intentional fallthrough
                case WAITING:       //Intentional fallthrough
                case CONFIRMING:    //Intentional fallthrough
                case OFF:           //Intentional fallthrough
                default:
                {
                    led_off(CONNECTED_LED);
                    break;
                }
            }
        }

        if (asc_flags & EVENT_ASC_DEVICE_IN_WRONG_STATE)
        {
            ascs_event_clear(EVENT_ASC_DEVICE_IN_WRONG_STATE);
            //Do nothing
            for (;;)
            {
                led_toggle(LED_ERROR_0);
                led_toggle(LED_ERROR_1);
                led_toggle(LED_ERROR_2);
                led_toggle(LED_ERROR_3);
                nrf_delay_ms(200);
            }
        }

        if (asc_flags & EVENT_ASC_LIGHT_STATE_CHANGED)
        {
            ascs_event_clear(EVENT_ASC_LIGHT_STATE_CHANGED);
            asc_slave_states_t light_state = ascs_light_state_get();

            switch (light_state)
            {
                case SLAVE_STATE_ON:
                {
                    led_on(LIGHT);
                    break;
                }
                case SLAVE_STATE_OFF:
                {
                    led_off(LIGHT);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        if (asc_flags & EVENT_ASC_REQUEST_RECEIVED)
        {
            ascs_event_clear(EVENT_ASC_REQUEST_RECEIVED);

            uint8_t response_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};

            asc_request_data_t request = ascs_get_last_request();

            //encode the response buffer with whatever page was requested
            switch (request.page_id_requested)
            {
                default:
                {
                    break;
                }
            }

            //tell the slave to send the response
            ascs_send_response(response_buffer);
        }

        if (asc_flags & EVENT_ASC_DATA_TIMEOUT)
        {
            ascs_event_clear(EVENT_ASC_DATA_TIMEOUT);
            //Do nothing
        }

        asc_flags = ascs_events_get();
    }
    while (asc_flags != 0);
}


/**@brief Callback function for the one-second timer
 */
static void one_second_timer_callback(void * p_context)
{
    ascs_increment_timer();
}


/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
static void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    led_toggle(POLLED_LED);
    ascs_handle_ant_event(p_ant_evt->event, p_ant_evt->message.aucMessage);
    check_and_handle_asc_flags();
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);


/**@brief Function for the Timer and BSP initialization.
 */
static void utils_setup(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                one_second_timer_callback);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_timer_id,
                               APP_TIMER_TICKS(RTC_EVENT_INTERVAL_MS),
                               NULL);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for ANT stack initialization.
 */
static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
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
    led_init();
    utils_setup();
    softdevice_setup();

    //Initialise and start the auto shared channel module
    ascs_init(&m_asc_parameters);
    ascs_turn_on();
    check_and_handle_asc_flags();

    NRF_LOG_INFO("ANT Shared Channels Slave example started.");

    // Main loop.
    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}

/**
 *@}
 **/
