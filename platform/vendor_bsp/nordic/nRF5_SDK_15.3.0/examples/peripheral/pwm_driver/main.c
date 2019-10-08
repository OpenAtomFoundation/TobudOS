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
 * @defgroup pwm_example_main main.c
 * @{
 * @ingroup pwm_example
 *
 * @brief PWM Example Application main file.
 *
 * This file contains the source code for a sample application using PWM.
 */

#include <stdio.h>
#include <string.h>
#include "nrf_drv_pwm.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);
static nrf_drv_pwm_t m_pwm2 = NRF_DRV_PWM_INSTANCE(2);

// This is for tracking PWM instances being used, so we can unintialize only
// the relevant ones when switching from one demo to another.
#define USED_PWM(idx) (1UL << idx)
static uint8_t m_used = 0;


static uint16_t const              m_demo1_top  = 10000;
static uint16_t const              m_demo1_step = 200;
static uint8_t                     m_demo1_phase;
static nrf_pwm_values_individual_t m_demo1_seq_values;
static nrf_pwm_sequence_t const    m_demo1_seq =
{
    .values.p_individual = &m_demo1_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_demo1_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static void demo1_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint8_t channel    = m_demo1_phase >> 1;
        bool    down       = m_demo1_phase & 1;
        bool    next_phase = false;

        uint16_t * p_channels = (uint16_t *)&m_demo1_seq_values;
        uint16_t value = p_channels[channel];
        if (down)
        {
            value -= m_demo1_step;
            if (value == 0)
            {
                next_phase = true;
            }
        }
        else
        {
            value += m_demo1_step;
            if (value >= m_demo1_top)
            {
                next_phase = true;
            }
        }
        p_channels[channel] = value;

        if (next_phase)
        {
            if (++m_demo1_phase >= 2 * NRF_PWM_CHANNEL_COUNT)
            {
                m_demo1_phase = 0;
            }
        }
    }
}
static void demo1(void)
{
    NRF_LOG_INFO("Demo 1");

    /*
     * This demo plays back a sequence with different values for individual
     * channels (LED 1 - LED 4). Only four values are used (one per channel).
     * Every time the values are loaded into the compare registers, they are
     * updated in the provided event handler. The values are updated in such
     * a way that increase and decrease of the light intensity can be observed
     * continuously on succeeding channels (one second per channel).
     */

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            BSP_LED_1 | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = m_demo1_top,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, demo1_handler));
    m_used |= USED_PWM(0);

    m_demo1_seq_values.channel_0 = 0;
    m_demo1_seq_values.channel_1 = 0;
    m_demo1_seq_values.channel_2 = 0;
    m_demo1_seq_values.channel_3 = 0;
    m_demo1_phase                = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &m_demo1_seq, 1,
                                      NRF_DRV_PWM_FLAG_LOOP);
}


static void demo2(void)
{
    NRF_LOG_INFO("Demo 2");

    /*
     * This demo plays back two concatenated sequences:
     * - Sequence 0: Light intensity is increased in 25 steps during one second.
     * - Sequence 1: LED blinks twice (100 ms off, 100 ms on), then stays off
     *   for 200 ms.
     * The same output is generated on all 4 channels (LED 1 - LED 4).
     * The playback is repeated in a loop.
     */

    enum { // [local constants]
        TOP        = 10000,
        STEP_COUNT = 25
    };

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            BSP_LED_1 | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_500kHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = TOP,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, NULL));
    m_used |= USED_PWM(0);

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM.
    static nrf_pwm_values_common_t seq0_values[STEP_COUNT];
    uint16_t value = 0;
    uint16_t step  = TOP / STEP_COUNT;
    uint8_t  i;
    for (i = 0; i < STEP_COUNT; ++i)
    {
        value         += step;
        seq0_values[i] = value;
    }

    nrf_pwm_sequence_t const seq0 =
    {
        .values.p_common = seq0_values,
        .length          = NRF_PWM_VALUES_LENGTH(seq0_values),
        .repeats         = 1,
        .end_delay       = 0
    };

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
    static nrf_pwm_values_common_t /*const*/ seq1_values[] =
    {
             0,
        0x8000,
             0,
        0x8000,
             0,
             0
    };
    nrf_pwm_sequence_t const seq1 =
    {
        .values.p_common = seq1_values,
        .length          = NRF_PWM_VALUES_LENGTH(seq1_values),
        .repeats         = 4,
        .end_delay       = 0
    };

    (void)nrf_drv_pwm_complex_playback(&m_pwm0, &seq0, &seq1, 1,
                                       NRF_DRV_PWM_FLAG_LOOP);
}


static void demo3(void)
{
    NRF_LOG_INFO("Demo 3");

    /*
     * This demo uses only one channel, which is reflected on LED 1.
     * The LED blinks three times (200 ms on, 200 ms off), then it stays off
     * for one second.
     * This scheme is performed three times before the peripheral is stopped.
     */

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 1
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 2
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_125kHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 25000,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, NULL));
    m_used |= USED_PWM(0);

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
    static uint16_t /*const*/ seq_values[] =
    {
        0x8000,
             0,
        0x8000,
             0,
        0x8000,
             0
    };
    nrf_pwm_sequence_t const seq =
    {
        .values.p_common = seq_values,
        .length          = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats         = 0,
        .end_delay       = 4
    };

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &seq, 3, NRF_DRV_PWM_FLAG_STOP);
}


static void demo4(void)
{
    NRF_LOG_INFO("Demo 4");

    /*
     * This demo uses all three PWM peripheral instances:
     * - PWM0 drives LED 1 and LED 2: Subsequent 2-bit binary values are
     *   presented every 500 ms.
     * - PWM1 drives LED 3: During 500 ms, the LED increases and decreases
     *   the light intensity, then it stays off for 1500 ms.
     * - PWM2 drives LED 4: For 500 ms, the LED stays off, then during 1500 ms
     *   it increases and decreases the light intensity.
     * Simple playback with grouped loading mode is used for PWM0, and complex
     * playback with common loading mode is used for both PWM1 and PWM2.
     */

    nrf_drv_pwm_config_t config =
    {
        // These are the common configuration options we use for all PWM
        // instances.
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .count_mode   = NRF_PWM_MODE_UP,
        .step_mode    = NRF_PWM_STEP_AUTO,
    };

    ////////////////////////////////////////////////////////////////////////////
    // PWM0 initialization.

    config.output_pins[0] = BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED;
    config.output_pins[1] = NRF_DRV_PWM_PIN_NOT_USED;
    config.output_pins[2] = BSP_LED_1 | NRF_DRV_PWM_PIN_INVERTED;
    config.output_pins[3] = NRF_DRV_PWM_PIN_NOT_USED;
    config.base_clock     = NRF_PWM_CLK_125kHz;
    config.top_value      = 31250; // 250ms period
    config.load_mode      = NRF_PWM_LOAD_GROUPED;
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config, NULL));
    m_used |= USED_PWM(0);

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
    static nrf_pwm_values_grouped_t /*const*/ pwm0_seq_values[] =
    {
        {      0,      0 },
        { 0x8000,      0 },
        {      0, 0x8000 },
        { 0x8000, 0x8000 }
    };
    nrf_pwm_sequence_t const pwm0_seq =
    {
        .values.p_grouped = pwm0_seq_values,
        .length           = NRF_PWM_VALUES_LENGTH(pwm0_seq_values),
        .repeats          = 1,
        .end_delay        = 0
    };

    ////////////////////////////////////////////////////////////////////////////
    // Common settings for PWM1 and PWM2.

    enum { // [local constants]
        TOP        = 5000,
        STEP_COUNT = 50
    };

    config.base_clock = NRF_PWM_CLK_1MHz;
    config.top_value  = TOP;
    config.load_mode  = NRF_PWM_LOAD_COMMON;

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM.
    static nrf_pwm_values_common_t fade_in_out_values[2 * STEP_COUNT];
    uint16_t value = 0;
    uint16_t step  = TOP / STEP_COUNT;
    uint8_t  i;
    for (i = 0; i < STEP_COUNT; ++i)
    {
        value                             += step;
        fade_in_out_values[i]              = value;
        fade_in_out_values[STEP_COUNT + i] = TOP - value;
    }

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
    static nrf_pwm_values_common_t /*const*/ stay_off_values[2] = { 0, 0 };

    ////////////////////////////////////////////////////////////////////////////
    // PWM1 initialization.

    config.output_pins[0] = NRF_DRV_PWM_PIN_NOT_USED;
    config.output_pins[1] = NRF_DRV_PWM_PIN_NOT_USED;
    config.output_pins[2] = BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED;
    config.output_pins[3] = NRF_DRV_PWM_PIN_NOT_USED;
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm1, &config, NULL));
    m_used |= USED_PWM(1);

    // Sequence 0 - fade-in/fade-out, duration: 500 ms.
    nrf_pwm_sequence_t const pwm1_seq0 =
    {
        .values.p_common = fade_in_out_values,
        .length          = NRF_PWM_VALUES_LENGTH(fade_in_out_values),
        .repeats         = 0,
        .end_delay       = 0
    };
    // Sequence 1 - off, duration: 1500 ms.
    nrf_pwm_sequence_t const pwm1_seq1 =
    {
        .values.p_common = stay_off_values,
        .length          = 2,
        .repeats         = 149,
        .end_delay       = 0
    };

    ////////////////////////////////////////////////////////////////////////////
    // PWM2 initialization.

    config.output_pins[0] = NRF_DRV_PWM_PIN_NOT_USED;
    config.output_pins[1] = NRF_DRV_PWM_PIN_NOT_USED;
    config.output_pins[2] = NRF_DRV_PWM_PIN_NOT_USED;
    config.output_pins[3] = BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED;
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm2, &config, NULL));
    m_used |= USED_PWM(2);

    // Sequence 0 - fade-in/fade-out, duration: 1500 ms.
    nrf_pwm_sequence_t const pwm2_seq0 =
    {
        .values.p_common = stay_off_values,
        .length          = 2,
        .repeats         = 49,
        .end_delay       = 0
    };
    // Sequence 1 - off, duration: 500 ms.
    nrf_pwm_sequence_t const pwm2_seq1 =
    {
        .values.p_common = fade_in_out_values,
        .length          = NRF_PWM_VALUES_LENGTH(fade_in_out_values),
        .repeats         = 2,
        .end_delay       = 0
    };

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &pwm0_seq, 1,
                                      NRF_DRV_PWM_FLAG_LOOP);
    (void)nrf_drv_pwm_complex_playback(&m_pwm1, &pwm1_seq0, &pwm1_seq1, 1,
                                       NRF_DRV_PWM_FLAG_LOOP);
    (void)nrf_drv_pwm_complex_playback(&m_pwm2, &pwm2_seq0, &pwm2_seq1, 1,
                                       NRF_DRV_PWM_FLAG_LOOP);
}


static void demo5(void)
{
    NRF_LOG_INFO("Demo 5");

    /*
     * This demo, similarly to demo1, plays back a sequence with different
     * values for individual channels. Unlike demo 1, however, it does not use
     * an event handler. Therefore, the PWM peripheral does not use interrupts
     * and the CPU can stay in sleep mode.
     * The LEDs (1-4) blink separately. They are turned on for 125 ms each,
     * in counterclockwise order (looking at the board).
     */

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            BSP_LED_1 | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_125kHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 15625,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, NULL));
    m_used |= USED_PWM(0);

    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
    static nrf_pwm_values_individual_t /*const*/ seq_values[] =
    {
        { 0x8000,      0,      0,      0 },
        {      0, 0x8000,      0,      0 },
        {      0,      0, 0x8000,      0 },
        {      0,      0,      0, 0x8000 }
    };
    nrf_pwm_sequence_t const seq =
    {
        .values.p_individual = seq_values,
        .length              = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats             = 0,
        .end_delay           = 0
    };

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}


static void bsp_evt_handler(bsp_event_t evt)
{
    void (* const demos[])(void) =
    {
        demo1,
        demo2,
        demo3,
        demo4,
        demo5
    };
    uint8_t const  demo_idx_max = (sizeof(demos) / sizeof(demos[0])) - 1;
    static uint8_t demo_idx     = 0;

    switch (evt)
    {
        // Button 1 - switch to the previous demo.
        case BSP_EVENT_KEY_0:
            if (demo_idx > 0)
            {
                --demo_idx;
            }
            else
            {
                demo_idx = demo_idx_max;
            }
            break;

        // Button 2 - switch to the next demo.
        case BSP_EVENT_KEY_1:
            if (demo_idx < demo_idx_max)
            {
                ++demo_idx;
            }
            else
            {
                demo_idx = 0;
            }
            break;

        default:
            return;
    }

    if (m_used & USED_PWM(0))
    {
        nrf_drv_pwm_uninit(&m_pwm0);
    }
    if (m_used & USED_PWM(1))
    {
        nrf_drv_pwm_uninit(&m_pwm1);
    }
    if (m_used & USED_PWM(2))
    {
        nrf_drv_pwm_uninit(&m_pwm2);
    }
    m_used = 0;

    demos[demo_idx]();
}
static void init_bsp()
{
    APP_ERROR_CHECK(nrf_drv_clock_init());
    nrf_drv_clock_lfclk_request(NULL);

    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(bsp_init(BSP_INIT_BUTTONS, bsp_evt_handler));
    APP_ERROR_CHECK(bsp_buttons_enable());
}


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    bsp_board_leds_on();
    app_error_save_and_stop(id, pc, info);
}


int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    init_bsp();

    NRF_LOG_INFO("PWM example started.");

    // Start with Demo 1, then switch to another one when the user presses
    // button 1 or button 2 (see the 'bsp_evt_handler' function).
    demo1();

    for (;;)
    {
        // Wait for an event.
        __WFE();

        // Clear the event register.
        __SEV();
        __WFE();

        NRF_LOG_FLUSH();
    }
}


/** @} */
