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
 * @defgroup capacitive_sensor_example_main main.c
 * @{
 * @ingroup capacitive_sensor_example
 * @brief Capacitive Sensor Example main file.
 *
 * This file contains the source code for a sample application that uses a capacitive sensor.
 *
 */

#include <stdio.h>
#include <string.h>
#include "boards.h"
#include "nrf_csense.h"
#include "app_error.h"
#include "nrf_drv_clock.h"
#include "app_timer.h"

#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* Time between RTC interrupts. */
#define APP_TIMER_TICKS_TIMEOUT APP_TIMER_TICKS(50)

/* Scale range. */
#define RANGE                   50

/* Analog inputs. */
#define AIN1                    1
#define AIN2                    2
#define AIN3                    3
#define AIN4                    4
#define AIN7                    7

/* Definition which pads use which analog inputs. */
#define BUTTON                  AIN7
#define PAD1                    AIN4
#define PAD4                    AIN4
#ifdef NRF51
#define PAD2                    AIN2
#define PAD3                    AIN3
#else
#define PAD2                    AIN1
#define PAD3                    AIN2
#endif

/* Threshold values for pads and button. */
#define THRESHOLD_PAD_1         400
#define THRESHOLD_PAD_2         400
#define THRESHOLD_PAD_3         400
#define THRESHOLD_PAD_4         400
#define THRESHOLD_BUTTON        400

/*lint -e19 -save */
NRF_CSENSE_BUTTON_DEF(m_button, (BUTTON, THRESHOLD_BUTTON));
NRF_CSENSE_SLIDER_4_DEF(m_slider,
                        RANGE,
                        (PAD1, THRESHOLD_PAD_1),
                        (PAD2, THRESHOLD_PAD_2),
                        (PAD3, THRESHOLD_PAD_3),
                        (PAD4, THRESHOLD_PAD_4));
/*lint -restore*/


/**
 * @brief Function for starting the internal LFCLK XTAL oscillator.
 *
 * Note that when using a SoftDevice, LFCLK is always on.
 *
 * @return Values returned by @ref nrf_drv_clock_init.
 */
static ret_code_t clock_config(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_clock_init();
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    nrf_drv_clock_lfclk_request(NULL);

    return NRF_SUCCESS;
}

/**
 * @brief Function for handling slider interrupts.
 *
 * @param[in] step                          Detected step.
 */
static void slider_handler(uint16_t step)
{
    static uint16_t slider_val;
    if (slider_val != step)
    {
        NRF_LOG_INFO("Slider value: %03d.", step);
        slider_val = step;
    }
}

/**
 * @brief Event handler for Capacitive Sensor High module.
 *
 * @param [in] p_evt_type                    Pointer to event data structure.
 */
void nrf_csense_handler(nrf_csense_evt_t * p_evt)
{
    switch (p_evt->nrf_csense_evt_type)
    {
        case NRF_CSENSE_BTN_EVT_PRESSED:
            break;
        case NRF_CSENSE_BTN_EVT_RELEASED:
            if (p_evt->p_instance == (&m_button))
            {
                uint16_t * btn_cnt = ((uint16_t *)p_evt->p_instance->p_context);
                (*btn_cnt)++;
                NRF_LOG_INFO("Button touched %03d times.", (*btn_cnt));
            }
            break;
        case NRF_CSENSE_SLIDER_EVT_PRESSED:
        case NRF_CSENSE_SLIDER_EVT_RELEASED:
            break;
        case NRF_CSENSE_SLIDER_EVT_DRAGGED:
            if ((p_evt->p_instance == (&m_slider)) && (p_evt->params.slider.step != UINT16_MAX))
            {
                /*lint -e611 -save */
                ((void(*)(uint16_t, uint8_t))p_evt->p_instance->p_context)(p_evt->params.slider.step, 2);
                /*lint -restore*/
            }
            break;
        default:
            NRF_LOG_WARNING("Unknown event.");
            break;
    }
}

/**
 * @brief Function for starting Capacitive Sensor High module.
 *
 * Function enables one slider and one button.
 */
static void csense_start(void)
{
    ret_code_t err_code;

    static uint16_t touched_counter = 0;

    err_code = nrf_csense_init(nrf_csense_handler, APP_TIMER_TICKS_TIMEOUT);
    APP_ERROR_CHECK(err_code);

    nrf_csense_instance_context_set(&m_button, (void*)&touched_counter);
    nrf_csense_instance_context_set(&m_slider, (void*)slider_handler);

    err_code = nrf_csense_add(&m_button);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_csense_add(&m_slider);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    ret_code_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = clock_config();
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Capacitive sensing library example started.");

    csense_start();

    while (1)
    {
        __WFI();
        NRF_LOG_FLUSH();
    }
}

/** @} */
