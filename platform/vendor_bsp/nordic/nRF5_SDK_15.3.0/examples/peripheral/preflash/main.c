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
 * @defgroup led_softblink_example_main main.c
 * @{
 * @ingroup led_softblink_example
 * @brief LED Soft Blink Example Application main file.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "led_softblink.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "nfc_t2t_lib.h"
#include "nfc_uri_msg.h"
#include "nrf_delay.h"

#if defined(BOARD_PCA10040)
 //URL "nordicsemi.com/start52dk"
static const uint8_t m_url[] = {'n', 'o', 'r', 'd', 'i', 'c', 's', 'e', 'm', 'i', '.',
                                'c', 'o', 'm','/','s','t','a','r','t','5','2','d','k'};
#elif defined (BOARD_PCA10056)
//URL "nordicsemi.com/start52840dk"
static const uint8_t m_url[] = {'n', 'o', 'r', 'd', 'i', 'c', 's', 'e', 'm', 'i', '.',
                                'c', 'o', 'm','/','s','t','a','r','t','5','2','8','4','0','d','k'};
#else
#error "Board is not supported"
#endif

uint8_t             m_ndef_msg_buf[256];        ///< Buffer for the NFC NDEF message.
volatile uint32_t   m_active_led_mask;          ///< LED mask.
volatile bool       m_update_softblink = false; ///< Flag for signaling a change of the LED mask for the softbling engine.

/**
 * @brief Callback function for handling NFC events.
 */
static void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
    (void)p_context;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            (void)led_softblink_stop();
            bsp_board_leds_on();
            break;
        case NFC_T2T_EVENT_FIELD_OFF:
            bsp_board_leds_off();
            (void)led_softblink_start(m_active_led_mask);
            break;
        case NFC_T2T_EVENT_DATA_READ:
            break;
        default:
            break;
    }
}

static void nfc_init(void)
{
    ret_code_t err_code;
    uint32_t   len = sizeof(m_ndef_msg_buf);

    /* Set up NFC */
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    /* Provide information about available buffer size to encoding function */

    /* Encode URI message into buffer */
    err_code = nfc_uri_msg_encode(NFC_URI_HTTP_WWW,
                                  m_url,
                                  sizeof(m_url),
                                  m_ndef_msg_buf,
                                  &len);

    APP_ERROR_CHECK(err_code);

    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    err_code = nfc_t2t_emulation_start();
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for LEDs initialization.
 */
static void leds_init(void)
{
    ret_code_t err_code;

    led_sb_init_params_t led_sb_init_params = LED_SB_INIT_DEFAULT_PARAMS(LEDS_MASK);
    led_sb_init_params.off_time_ticks       = 32768;
    led_sb_init_params.on_time_ticks        = 16384;
    led_sb_init_params.duty_cycle_max       = 200;
    led_sb_init_params.duty_cycle_min       = 4;
    led_sb_init_params.duty_cycle_step      = 1;

    err_code = led_softblink_init(&led_sb_init_params);
    APP_ERROR_CHECK(err_code);
}

static void clock_init(void)
{
    uint32_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

/**@brief Function for handling bsp events.
 */
void bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            m_active_led_mask = BSP_LED_0_MASK;
            break;
        case BSP_EVENT_KEY_1:
            m_active_led_mask = BSP_LED_1_MASK;
            break;
        case BSP_EVENT_KEY_2:
            m_active_led_mask = BSP_LED_2_MASK;
            break;
        case BSP_EVENT_KEY_3:
            m_active_led_mask = BSP_LED_3_MASK;
            break;
        default:
            return; // no implementation needed
    }

    m_update_softblink = true; // request update of blinked LED
}

/**
 * @brief Function for updating LED to be softly blinking.
 */
static void softblink_led_update(void)
{
    uint32_t err_code;

    if (m_update_softblink == false)
    {
         // nothing to do
         return;
    }

    m_update_softblink = false;

    err_code = led_softblink_stop();
    APP_ERROR_CHECK(err_code);

    err_code = led_softblink_start(m_active_led_mask);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    clock_init();

    // Start APP_TIMER to generate timeouts.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    leds_init();

    err_code = bsp_init(BSP_INIT_BUTTONS, bsp_evt_handler);
    APP_ERROR_CHECK(err_code);

    nfc_init();

    m_active_led_mask = BSP_LED_0_MASK;
    err_code = led_softblink_start(m_active_led_mask);
    APP_ERROR_CHECK(err_code);

    while (true)
    {
        __WFE();
        softblink_led_update();
    }
}

/** @} */
