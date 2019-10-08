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
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_error.h"
#include "nrf_qdec.h"
#include "qenc_sim.h"


/**
 * @enum nrf_qdec_task_t
 * @brief enumeration of QDEC tasks.
 * coding is [A|B] i.e. B is LSB
 */

#define _static

typedef enum {
  state_0 = 0,      /**< AB=00 */
  state_1 = 1,      /**< AB=01 */
  state_2 = 2,      /**< AB=10 */
  state_3 = 3       /**< AB=11 */
} qenc_state_t;

const qenc_state_t next_pos[4] = { state_1, state_3, state_0, state_2 }; // change of state to get phase increment
const qenc_state_t next_neg[4] = { state_2, state_0, state_3, state_1 }; // change of state to get phase decrement
const qenc_state_t next_dbl[4] = { state_3, state_2, state_1, state_0 }; // change of state to get double transition

_static volatile qenc_state_t qenc_state = state_0;
_static volatile int32_t qenc_count = 0;
_static volatile uint32_t qenc_dbl_count = 0;
_static volatile bool  qenc_enable_flag = false;

static void gpiote_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if ((qenc_count > 0) && qenc_enable_flag)
    {
        qenc_count--;
        qenc_state = next_pos[qenc_state];

    }
    else if ((qenc_count < 0 ) && qenc_enable_flag )
    {
        qenc_count++;
        qenc_state = next_neg[qenc_state];
    }
    else if ((qenc_dbl_count > 0 ) && qenc_enable_flag)
    {
        qenc_dbl_count--;
        qenc_state = next_dbl[qenc_state];
    }
    else
    {
        qenc_enable_flag = false;
    }

    if (qenc_state & 0x01)
    {
        nrf_drv_gpiote_out_set(QENC_CONFIG_PIO_B);
    }
    else
    {
        nrf_drv_gpiote_out_clear(QENC_CONFIG_PIO_B);
    }

    if (qenc_state & 0x02)
    {
        nrf_drv_gpiote_out_set(QENC_CONFIG_PIO_A);
    }
    else
    {
        nrf_drv_gpiote_out_clear(QENC_CONFIG_PIO_A);
    }
}

static void qenc_init_gpiote(nrf_qdec_ledpol_t led_pol)
{
    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    config.pull = NRF_GPIO_PIN_PULLUP;

    if (!nrf_drv_gpiote_is_init())
    {
        (void)nrf_drv_gpiote_init();
    }

    // change state on inactive edge of led pulse
    if (led_pol == NRF_QDEC_LEPOL_ACTIVE_LOW)
    {
        config.sense = NRF_GPIOTE_POLARITY_HITOLO;
    }

    (void)nrf_drv_gpiote_in_init(QENC_CONFIG_PIO_LED,&config,gpiote_event_handler);
    nrf_drv_gpiote_in_event_enable(QENC_CONFIG_PIO_LED, true);

    //Configure output pins.
    (void)nrf_drv_gpiote_out_init(QENC_CONFIG_PIO_A, &out_config);
    (void)nrf_drv_gpiote_out_init(QENC_CONFIG_PIO_B, &out_config);
}

void qenc_init(nrf_qdec_ledpol_t led_pol)
{
    qenc_init_gpiote(led_pol);
}

// this function is used mainly in tests
void qenc_pulse_dblpulse_count_set(int32_t pulse_count, uint32_t dble_pulse_count)
{
    qenc_count          = pulse_count;
    qenc_dbl_count      = dble_pulse_count;
    qenc_enable_flag    = true;
}

// this function is used mainly in a example
void qenc_pulse_count_set(int32_t pulse_count)
{
    qenc_pulse_dblpulse_count_set(pulse_count, 0);
}

