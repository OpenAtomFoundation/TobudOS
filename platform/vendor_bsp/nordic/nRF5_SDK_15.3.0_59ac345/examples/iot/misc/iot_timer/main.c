/**
 * Copyright (c) 2013 - 2019, Nordic Semiconductor ASA
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
/** @file
 *
 * @defgroup iot_sdk_app_iot_timer main.c
 * @{
 * @ingroup iot_sdk_app_misc
 *
 * @brief This file contains the source code for IoT Timer sample application.
 *
 * @details The example will create one IoT Timer with 4 different clients.
 *          The clients will be called for every:
 *          - 100 ms
 *          - 200 ms
 *          - 400 ms, and
 *          - 800 ms.
 *
 *          The example uses UART to print a log of the timer events.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "bsp.h"
#include "sdk_config.h"
#include "app_error.h"
#include "nordic_common.h"
#include "app_timer.h"
#include "iot_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define MAX_LENGTH_FILENAME                 80

#define IOT_TIMER_CLIENT_ONE_CB_INTERVAL    100                                                     /**< Interval in milliseconds between callbacks for one of the IoT Timer clients. */
#define IOT_TIMER_CLIENT_TWO_CB_INTERVAL    200                                                     /**< Interval in milliseconds between callbacks for one of the IoT Timer clients. */
#define IOT_TIMER_CLIENT_THREE_CB_INTERVAL  400                                                     /**< Interval in milliseconds between callbacks for one of the IoT Timer clients. */
#define IOT_TIMER_CLIENT_FOUR_CB_INTERVAL   800                                                     /**< Interval in milliseconds between callbacks for one of the IoT Timer clients. */

#define DISPLAY_LED_0                       BSP_LED_0_MASK                                          /**< LED used for displaying IoT Timer callbacks. */
#define DISPLAY_LED_1                       BSP_LED_1_MASK                                          /**< LED used for displaying IoT Timer callbacks. */
#define DISPLAY_LED_2                       BSP_LED_2_MASK                                          /**< LED used for displaying IoT Timer callbacks. */
#define DISPLAY_LED_3                       BSP_LED_3_MASK                                          /**< LED used for displaying IoT Timer callbacks. */
#define ALL_APP_LED                        (BSP_LED_0_MASK | BSP_LED_1_MASK | \
                                            BSP_LED_2_MASK | BSP_LED_3_MASK)                        /**< Define used for simultaneous operation of all application LEDs. */

#define APP_ENABLE_LOGS                     1                                                       /**< Enable logs in the application. */

#if (APP_ENABLE_LOGS == 1)

#define APPL_LOG  NRF_LOG_INFO

#else // APP_ENABLE_LOGS

#define APPL_LOG(...)

#endif // APP_ENABLE_LOGS

APP_TIMER_DEF(m_iot_timer_tick_src_id);                                                             /**< App timer instance used to update the IoT timer wall clock. */


/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    // Configure application LED pins.
    LEDS_CONFIGURE(ALL_APP_LED);

    // Turn off all LED on initialization.
    LEDS_OFF(ALL_APP_LED);
}


/**@brief Function for updating the wall clock of the IoT Timer module.
 */
static void iot_timer_tick_callback(void * p_context)
{
    UNUSED_VARIABLE(p_context);
    uint32_t err_code = iot_timer_update();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the App Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    APP_ERROR_CHECK(app_timer_init());

    // Initialize timer instance as a tick source for IoT timer.
    err_code = app_timer_create(&m_iot_timer_tick_src_id,
                                APP_TIMER_MODE_REPEATED,
                                iot_timer_tick_callback);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting the low frequency clock.
 */
static void low_freq_clock_start(void)
{
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        //No implementation needed.
    }

    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}


/**@brief Timer callback used for periodic changing of LED state.
 */
static void iot_timer_client_one_callback(iot_timer_time_in_ms_t elapsed_time)
{
    UNUSED_PARAMETER(elapsed_time);
    LEDS_INVERT(DISPLAY_LED_0);
    APPL_LOG("---- %4d ms", IOT_TIMER_CLIENT_ONE_CB_INTERVAL);
}


/**@brief Timer callback used for periodic changing of LED state.
 */
static void iot_timer_client_two_callback(iot_timer_time_in_ms_t elapsed_time)
{
    UNUSED_PARAMETER(elapsed_time);
    LEDS_INVERT(DISPLAY_LED_1);
    APPL_LOG("----------- %4d ms", IOT_TIMER_CLIENT_TWO_CB_INTERVAL);
}


/**@brief Timer callback used for periodic changing of LED state.
 */
static void iot_timer_client_three_callback(iot_timer_time_in_ms_t elapsed_time)
{
    UNUSED_PARAMETER(elapsed_time);
    LEDS_INVERT(DISPLAY_LED_2);
    APPL_LOG("------------------ %4d ms", IOT_TIMER_CLIENT_THREE_CB_INTERVAL);
}


/**@brief Timer callback used for periodic changing of LED state.
 */
static void iot_timer_client_four_callback(iot_timer_time_in_ms_t elapsed_time)
{
    UNUSED_PARAMETER(elapsed_time);
    LEDS_INVERT(DISPLAY_LED_3);
    nrf_gpio_pin_toggle(13);
    APPL_LOG("------------------------- %4d ms", IOT_TIMER_CLIENT_FOUR_CB_INTERVAL);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    //Initialize.
    log_init();
    leds_init();
    low_freq_clock_start();
    timers_init();

    static const iot_timer_client_t list_of_clients[] =
    {
        {iot_timer_client_one_callback,   IOT_TIMER_CLIENT_ONE_CB_INTERVAL},
        {iot_timer_client_two_callback,   IOT_TIMER_CLIENT_TWO_CB_INTERVAL},
        {iot_timer_client_three_callback, IOT_TIMER_CLIENT_THREE_CB_INTERVAL},
        {iot_timer_client_four_callback,  IOT_TIMER_CLIENT_FOUR_CB_INTERVAL},
    };

    //The list of IoT Timer clients is declared as a constant.
    static const iot_timer_clients_list_t iot_timer_clients =
    {
        (sizeof(list_of_clients) / sizeof(iot_timer_client_t)),
        &(list_of_clients[0]),
    };

    //Passing the list of clients to the IoT Timer module.
    err_code = iot_timer_client_list_set(&iot_timer_clients);
    APP_ERROR_CHECK(err_code);

    //Starting the app timer instance that is the tick source for the IoT Timer.
    err_code = app_timer_start(m_iot_timer_tick_src_id,
                               APP_TIMER_TICKS(IOT_TIMER_RESOLUTION_IN_MS),
                               NULL);
    APP_ERROR_CHECK(err_code);

    APPL_LOG("Application started.");

    //Enter main loop.
    for (;;)
    {
        (void)NRF_LOG_PROCESS();
    }
}

/** @} */
