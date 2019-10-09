/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
 
#include "syscfg/syscfg.h"

#if (MYNEWT_VAL(USE_NEOPIXEL))

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "sysinit/sysinit.h"
#include "os/os.h"
#include "bsp/bsp.h"
#include "pwm/pwm.h"
#include "nrfx.h"
#include "nrfx_pwm.h"
#include "ws2812.h"

#define BITS_PER_SEQ    (24)
#define BIT0            (0x8000 | 6)
#define BIT1            (0x8000 | 11)

static const nrfx_pwm_t pwm = NRFX_PWM_INSTANCE(WS2812_PWM);

static const nrfx_pwm_config_t pwm_config = {
    .output_pins = { WS2812_GPIO, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED },
    .irq_priority = 3,
    .base_clock = NRF_PWM_CLK_16MHz,
    .count_mode = NRF_PWM_MODE_UP,
    .top_value = 20,
    .load_mode = NRF_PWM_LOAD_COMMON,
    .step_mode = NRF_PWM_STEP_AUTO,
};

static uint16_t pwm_seq_values[2][BITS_PER_SEQ];

static const nrf_pwm_sequence_t pwm_seq[2] = {
        {
            .values.p_raw = pwm_seq_values[0],
            .length = BITS_PER_SEQ,
            .repeats = 0,
            .end_delay = 0,
        }, {
            .values.p_raw = pwm_seq_values[1],
            .length = BITS_PER_SEQ,
            .repeats = 0,
            .end_delay = 0,
        },
};

static uint32_t led_color[WS2812_NUM_LED];
static int led_idx;

static void
load_pixel(void)
{
    uint16_t *seq_values;
    uint32_t grb;
    int i;

    seq_values = pwm_seq_values[led_idx & 1];
    grb = led_color[led_idx];

    for (i = 0; i < BITS_PER_SEQ; i++) {
        *seq_values = grb & 0x800000 ? BIT1 : BIT0;
        grb <<= 1;
        seq_values++;
    }

    led_idx++;
}

static void
pwm_handler_func(nrfx_pwm_evt_type_t event_type)
{
    switch (event_type) {
    case NRFX_PWM_EVT_END_SEQ0:
    case NRFX_PWM_EVT_END_SEQ1:
        load_pixel();
        break;
    default:
        break;
    }
}

int
ws2812_init(void)
{
    nrfx_err_t err;

    err = nrfx_pwm_init(&pwm, &pwm_config, pwm_handler_func);

    return err != NRFX_SUCCESS;
}

int
ws2812_write(const uint32_t *rgb)
{
    uint32_t grb;
    int i;

    for (i = 0; i < WS2812_NUM_LED; i++) {
        grb = 0;
        grb |= (rgb[i] & 0x00FF00) << 8;
        grb |= (rgb[i] & 0xFF0000) >> 8;
        grb |= (rgb[i] & 0x0000FF);

        led_color[i] = grb;
    }

    led_idx = 0;

    load_pixel();
    load_pixel();
    nrfx_pwm_complex_playback(&pwm, &pwm_seq[0], &pwm_seq[1], WS2812_NUM_LED,
                              NRFX_PWM_FLAG_SIGNAL_END_SEQ0 |
                              NRFX_PWM_FLAG_SIGNAL_END_SEQ1 |
                              NRFX_PWM_FLAG_STOP);

    return 0;
}
#endif
