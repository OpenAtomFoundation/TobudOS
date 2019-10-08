/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements the platform abstraction layer APIs for os event/scheduler.
*
* \addtogroup  grPAL
* @{
*/

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include <stdbool.h>
#include "stdlib.h"
#include "stdio.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal_os_timer.h"
#include "nrf_rtc.h"
#include "nrf_drv_rtc.h"
#include "nrf_delay.h"
#include "nrf_pwr_mgmt.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/
/// @cond hidden
/// Callback function when timer elapses
static volatile register_callback callback_registered = NULL;
/// Pointer to store upper layer callback context (For example: Ifx i2c context)
static void * callback_ctx;
/// Flag to indicate to the delay function when the timer has elapsed
static volatile bool timer_elapsed = false;

/// Flag to indicate if the the RTC was already initialized, re-init. causes an NRF_ERROR
static bool m_rtc2_is_initialized = false;
static const nrf_drv_rtc_t rtc2 = NRF_DRV_RTC_INSTANCE(2);
static nrf_drv_rtc_config_t m_rtc2_config = NRF_DRV_RTC_DEFAULT_CONFIG;
#define RTC_CLOCK_FREQUENCY 32768
// Set the prescaler to approximately get 0.25 ms intervals
// 32768Hz/8 = 4096 Hz -> 0.2441us
// it's a 24bit counter, so it will overflow every ~68min
#define RTC_PRESCALER 8
#define RTC_TICK_FREQ (RTC_CLOCK_FREQUENCY/RTC_PRESCALER)

/**
*  Timer callback handler.
*
*  This get called from the TIMER elapse event.<br>
*  Once the timer expires, the registered callback funtion gets called from the timer event handler, if
*  the call back is not NULL.<br>
*
*\param[in] args Callback argument
*
*/
static void ifx_rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    volatile register_callback callback;
    (void)nrf_drv_rtc_cc_disable(&rtc2, int_type);
    switch(int_type)
    {
        case NRF_DRV_RTC_INT_COMPARE0:
            // handler for register_callback_oneshot
            if (callback_registered != NULL)
            {
                callback = callback_registered;
                callback_registered = NULL;
                callback(callback_ctx);
            }

            break;
        case NRF_DRV_RTC_INT_COMPARE1:
            // handler for delay_in_milliseconds
            timer_elapsed = true;
            break;
        default:
            break;
    }
}

/// @endcond

void pal_os_event_init()
{
   if (m_rtc2_is_initialized == true)
   {
      return;
   }

    // set prescaler so that a tick is approximately 1ms
    m_rtc2_config.prescaler = RTC_PRESCALER;

    // Initialize the RTC2 driver instance
    APP_ERROR_CHECK(nrf_drv_rtc_init(&rtc2, &m_rtc2_config, ifx_rtc_handler));

    // Power on RTC instance
    nrf_drv_rtc_enable(&rtc2);

    m_rtc2_is_initialized = true;
}

void pal_os_event_register_callback_oneshot(register_callback callback,
                                            void* callback_args,
                                            uint32_t time_us)
{
    callback_registered = callback;
    callback_ctx = callback_args;

    // parentheses are set this way to avoid overflow when multiplying time_us with something
    uint32_t future_ticks = (time_us/(1000*1000/RTC_TICK_FREQ));

    // we can't reliably set an interrupt less than two ticks ahead, as per NRF52832 datasheet, p. 245
    // do busy waiting instead
    if(future_ticks < 2) {
        nrf_delay_us(time_us);
        ifx_rtc_handler(NRF_DRV_RTC_INT_COMPARE0);
        return;
    }

    // add current tick value
    future_ticks += nrf_drv_rtc_counter_get(&rtc2);

    // Set the compare register to trigger approximately at time_us
    APP_ERROR_CHECK(nrf_drv_rtc_cc_set(&rtc2, NRF_DRV_RTC_INT_COMPARE0, future_ticks, true));
}

uint32_t pal_os_timer_get_time_in_milliseconds(void)
{
    return nrf_drv_rtc_counter_get(&rtc2)*1000/RTC_TICK_FREQ;
}

void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds)
{
    timer_elapsed = false;
    uint32_t future_ticks = milliseconds*(RTC_TICK_FREQ/1000);

    // we can't reliably set an interrupt less than two ticks ahead, as per NRF52832 datasheet, p. 245
    // do busy waiting instead
    if(future_ticks < 2) {
        nrf_delay_ms(milliseconds);
        return;
    }

    // add current tick value
    future_ticks += nrf_drv_rtc_counter_get(&rtc2);

    // Set the compare register to trigger in approximately milliseconds
    APP_ERROR_CHECK(nrf_drv_rtc_cc_set(&rtc2, NRF_DRV_RTC_INT_COMPARE1, future_ticks, true));
    while(!timer_elapsed)
    {
        nrf_pwr_mgmt_run();
    }
    timer_elapsed = false;
}

/**
* @}
*/

