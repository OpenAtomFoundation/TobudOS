/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
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
 * @defgroup low_power_pwm_example_main main.c
 * @{
 * @ingroup low_power_pwm_example
 * @brief Low Power PWM Example Application main file.
 *
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "app_util_platform.h"
#include "low_power_pwm.h"
#include "nordic_common.h"

/*Ticks before change duty cycle of each LED*/
#define TICKS_BEFORE_CHANGE_0   500
#define TICKS_BEFORE_CHANGE_1   400

static low_power_pwm_t low_power_pwm_0;
static low_power_pwm_t low_power_pwm_1;
static low_power_pwm_t low_power_pwm_2;

/**
 * @brief Function to be called in timer interrupt.
 *
 * @param[in] p_context     General purpose pointer (unused).
 */
static void pwm_handler(void * p_context)
{
    uint8_t new_duty_cycle;
    static uint16_t led_0, led_1;
    uint32_t err_code;
    UNUSED_PARAMETER(p_context);

    low_power_pwm_t * pwm_instance = (low_power_pwm_t*)p_context;

    if (pwm_instance->bit_mask == BSP_LED_0_MASK)
    {
        led_0++;

        if (led_0 > TICKS_BEFORE_CHANGE_0)
        {
            new_duty_cycle = pwm_instance->period - pwm_instance->duty_cycle;
            err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
            led_0 = 0;
            APP_ERROR_CHECK(err_code);
        }
    }
    else if (pwm_instance->bit_mask == BSP_LED_1_MASK)
    {
        led_1++;

        if (led_1 > TICKS_BEFORE_CHANGE_1)
        {
            new_duty_cycle = pwm_instance->period - pwm_instance->duty_cycle;
            err_code = low_power_pwm_duty_set(pwm_instance, new_duty_cycle);
            led_1 = 0;
            APP_ERROR_CHECK(err_code);
        }
    }
    else
    {
        /*empty else*/
    }
}
/**
 * @brief Function to initalize low_power_pwm instances.
 *
 */

static void pwm_init(void)
{
    uint32_t err_code;
    low_power_pwm_config_t low_power_pwm_config;

    APP_TIMER_DEF(lpp_timer_0);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 220;
    low_power_pwm_config.bit_mask       = BSP_LED_0_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_0;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm_0), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_0, 20);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_1);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 200;
    low_power_pwm_config.bit_mask       = BSP_LED_1_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_1;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm_1), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_1, 150);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_2);
    low_power_pwm_config.active_high    = false;
    low_power_pwm_config.period         = 100;
    low_power_pwm_config.bit_mask       = BSP_LED_2_MASK;
    low_power_pwm_config.p_timer_id     = &lpp_timer_2;
    low_power_pwm_config.p_port         = NRF_GPIO;

    err_code = low_power_pwm_init((&low_power_pwm_2), &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&low_power_pwm_2, 20);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start((&low_power_pwm_0), low_power_pwm_0.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start((&low_power_pwm_1), low_power_pwm_1.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start((&low_power_pwm_2), low_power_pwm_2.bit_mask);
    APP_ERROR_CHECK(err_code);
}

static void lfclk_init(void)
{
    uint32_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint8_t new_duty_cycle;
    uint32_t err_code;

    lfclk_init();

    // Start APP_TIMER to generate timeouts.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    /*Initialize low power PWM for all 3  channels of RGB or 3 channels of leds on pca10028*/
    pwm_init();

    while (true)
    {
        /* Duty cycle can also be changed from main context. */
        new_duty_cycle = low_power_pwm_2.period - low_power_pwm_2.duty_cycle;
        err_code = low_power_pwm_duty_set(&low_power_pwm_2, new_duty_cycle);
        APP_ERROR_CHECK(err_code);
        nrf_delay_ms(500);

        new_duty_cycle = low_power_pwm_2.period - low_power_pwm_2.duty_cycle;
        err_code = low_power_pwm_duty_set(&low_power_pwm_2, new_duty_cycle);
        APP_ERROR_CHECK(err_code);
        nrf_delay_ms(500);
    }
}

/** @} */
