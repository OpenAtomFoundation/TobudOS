/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
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
/**@file
 * @defgroup nrf_dev_simple_timer_example_main.c
 * @{
 * @ingroup nrf_dev_simple_timer_example
 * @brief Timer example application main file.
 *
 * This file contains the source code for a sample application using timer library.
 * For a more detailed description of the functionality, see the SDK documentation.
 */

#include "app_simple_timer.h"
#include <stdio.h>
#include "boards.h"
#include "app_error.h"
#include "nrf_delay.h"

#define TIMEOUT_VALUE                    50000                          /**< 50 mseconds timer time-out value. */
#define TOGGLE_LED_COUNTER               (500 / (TIMEOUT_VALUE / 1000)) /**< Interval for toggling a LED. Yields to 500 mseconds. */
#define STATE_TRANSIT_COUNTER_INIT_VALUE (4 * TOGGLE_LED_COUNTER)       /**< Initial value for the state transition counter.  */
#define GENERIC_DELAY_TIME               1000                           /**< Generic delay time used by application. */

/**@brief Application states. */
typedef enum
{
    APP_STATE_SINGLE_SHOT,                                              /**< Application state where single shot timer mode is tested. */
    APP_STATE_REPEATED                                                  /**< Application state where repeated timer mode is tested. */
} state_t;

static volatile uint32_t m_state_transit_counter = 0;                            /**< State transition counter variable. */
static volatile uint32_t m_toggle_led_counter    = 0;                            /**< Led toggling counter variable. */
static volatile state_t  m_state;                                                /**< Current application state. */

void timeout_handler(void * p_context);


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    bsp_board_leds_off();

    for (;;)
    {
        nrf_delay_ms(GENERIC_DELAY_TIME);

        bsp_board_led_invert(BSP_BOARD_LED_0);
        bsp_board_led_invert(BSP_BOARD_LED_1);
    }
}


/**@brief Function for toggling a LED and starting a timer.
 *
 * @param[in] led_id     ID of the LED to toggle.
 * @param[in] timer_mode Timer mode @ref timer_mode_t.
 */
static void led_and_timer_control(uint32_t led_id, app_simple_timer_mode_t timer_mode)
{
    uint32_t err_code;

    bsp_board_led_invert(led_id);

    m_state_transit_counter = STATE_TRANSIT_COUNTER_INIT_VALUE;
    m_toggle_led_counter    = TOGGLE_LED_COUNTER;

    err_code = app_simple_timer_start(timer_mode, timeout_handler, TIMEOUT_VALUE, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for executing the state entry action.
 */
static __INLINE void state_entry_action_execute(void)
{
    switch (m_state)
    {
        case APP_STATE_SINGLE_SHOT:
            led_and_timer_control(BSP_BOARD_LED_0, APP_SIMPLE_TIMER_MODE_SINGLE_SHOT);
            break;

        case APP_STATE_REPEATED:
            led_and_timer_control(BSP_BOARD_LED_1, APP_SIMPLE_TIMER_MODE_REPEATED);
            break;

        default:
            APP_ERROR_HANDLER(m_state);
            break;
    }
}


/**@brief Function for changing the state of the state machine.
 *
 * @param[in] new_state  State to which the state machine transitions.
 */
static void state_machine_state_change(state_t new_state)
{
    m_state = new_state;
    state_entry_action_execute();
}


void timeout_handler(void * p_context)
{
    switch (m_state)
    {
        uint32_t err_code;

        case APP_STATE_SINGLE_SHOT:
            if (--m_state_transit_counter != 0)
            {
                if (--m_toggle_led_counter == 0)
                {
                    m_toggle_led_counter = TOGGLE_LED_COUNTER;
                    bsp_board_led_invert(BSP_BOARD_LED_0);
                }

                err_code = app_simple_timer_start(APP_SIMPLE_TIMER_MODE_SINGLE_SHOT,
                                       timeout_handler,
                                       TIMEOUT_VALUE,
                                       NULL);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                state_machine_state_change(APP_STATE_REPEATED);
            }
            break;

        case APP_STATE_REPEATED:
            if (--m_state_transit_counter != 0)
            {
                if (--m_toggle_led_counter == 0)
                {
                    m_toggle_led_counter = TOGGLE_LED_COUNTER;
                    bsp_board_led_invert(BSP_BOARD_LED_1);
                }
            }
            else
            {
                bsp_board_led_on(BSP_BOARD_LED_0);
                bsp_board_led_on(BSP_BOARD_LED_1);

                err_code = app_simple_timer_stop();
                APP_ERROR_CHECK(err_code);

                nrf_delay_ms(GENERIC_DELAY_TIME);

                state_machine_state_change(APP_STATE_SINGLE_SHOT);
            }
            break;

        default:
            APP_ERROR_HANDLER(m_state);
            break;
    }
}


/**@brief Function for the Power Management.
 */
static void power_manage(void)
{
    // Use directly __WFE and __SEV macros since the SoftDevice is not available.

    // Wait for event.
    __WFE();

    // Clear Event Register.
    __SEV();
    __WFE();
}


int main(void)
{
    uint32_t err_code = app_simple_timer_init();
    APP_ERROR_CHECK(err_code);

    bsp_board_init(BSP_INIT_LEDS);
    bsp_board_led_on(BSP_BOARD_LED_0);
    bsp_board_led_on(BSP_BOARD_LED_1);

    nrf_delay_ms(GENERIC_DELAY_TIME);

    state_machine_state_change(APP_STATE_SINGLE_SHOT);

    for (;;)
    {
        power_manage();
    }
}
