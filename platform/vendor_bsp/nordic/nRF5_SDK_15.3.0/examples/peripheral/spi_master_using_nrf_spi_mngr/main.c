/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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
 * @brief SPI Example Application main file.
 *
 * This file contains the source code for a sample application using nRF SPI manager and nRF GFX.
 */

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_clock.h"
#include "app_error.h"
#include "nrf_spi_mngr.h"
#include "st7565LCD.h"
#include "nrf_lcd.h"
#include "nrf_gfx.h"
#include "bsp.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#if defined( __GNUC__ ) && (__LINT__ == 0)
    // This is required if one wants to use floating-point values in 'printf'
    // (by default this feature is not linked together with newlib-nano).
    // Please note, however, that this adds about 13 kB code footprint...
    __ASM(".global _printf_float");
#endif

extern const nrf_gfx_font_desc_t orkney_8ptFontInfo;

#define FACE_LEFT   0
#define FACE_CENTER 1
#define FACE_RIGHT  2
#define FACE_SHOW   1
#define FACE_ERASE  0

APP_TIMER_DEF(m_timer);

/* Below structure is requested by NRF GFX library. Monochromatic LCD display used in this example
 * is based on ST7565 driver. It has width = 128 pixels and height = 32 pixels */
static lcd_cb_t m_lcd_cb =
{
    .state    = NRFX_DRV_STATE_UNINITIALIZED,
    .height   = ST7565_LCD_HEIGHT,
    .width    = ST7565_LCD_WIDTH,
    .rotation = NRF_LCD_ROTATE_0
};

/* Dummy function. Rotation is not used in this example but such function is required
 * by nrf_gfx module. */
static void dummy_lcd_rotation_set(nrf_lcd_rotation_t rotation)
{
    UNUSED_PARAMETER(rotation);
}

/* Below functions are used by NRF GFX library. Rotation is not needed in this example. */
static const nrf_lcd_t m_nrf_lcd =
{
    .lcd_init   = st7565_init,
    .lcd_uninit = st7565_uninit,
    .lcd_pixel_draw = st7565_pixel_draw,
    .lcd_rect_draw = st7565_rect_draw,
    .lcd_display = st7565_display_screen,
    .lcd_rotation_set = dummy_lcd_rotation_set,
    .lcd_display_invert = st7565_display_invert,
    .p_lcd_cb = &m_lcd_cb
};


static ret_code_t happy_face_draw(nrf_lcd_t const * p_instance, uint8_t idx, uint32_t color)
{
    ASSERT(p_instance != NULL);

    int8_t x_offset;

    if (idx == FACE_LEFT)
    {
        x_offset = -40;
    }
    else if (idx == FACE_CENTER)
    {
        x_offset = 0;
    }
    else if (idx == FACE_RIGHT)
    {
        x_offset = 40;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    nrf_gfx_line_t const nose =
    {
        .x_start = 64 + x_offset,
        .y_start = 20,
        .x_end = 64 + x_offset,
        .y_end = 22,
        .thickness = 1
    };
    nrf_gfx_line_t const mouth1 =
    {
        .x_start = 59 + x_offset,
        .y_start = 23,
        .x_end = 62 + x_offset,
        .y_end = 25,
        .thickness = 1
    };
    nrf_gfx_line_t const mouth2 =
    {
        .x_start = 62 + x_offset,
        .y_start = 25,
        .x_end = 66 + x_offset,
        .y_end = 25,
        .thickness = 1
    };
    nrf_gfx_line_t const mouth3 =
    {
        .x_start = 66 + x_offset,
        .y_start = 25,
        .x_end = 69 + x_offset,
        .y_end = 23,
        .thickness = 1
    };
    nrf_gfx_circle_t const head =
    {
        .x = 64 + x_offset,
        .y = 21,
        .r = 8
    };
    nrf_gfx_rect_t const left_eye =
    {
        .x = 60 + x_offset,
        .y = 19,
        .width = 2,
        .height= 2
    };
    nrf_gfx_rect_t const right_eye =
    {
        .x = 67 + x_offset,
        .y = 19,
        .width = 2,
        .height= 2
    };
    APP_ERROR_CHECK(nrf_gfx_line_draw(p_instance, &nose, color));
    APP_ERROR_CHECK(nrf_gfx_line_draw(p_instance, &mouth1, color));
    APP_ERROR_CHECK(nrf_gfx_line_draw(p_instance, &mouth2, color));
    APP_ERROR_CHECK(nrf_gfx_line_draw(p_instance, &mouth3, color));
    APP_ERROR_CHECK(nrf_gfx_circle_draw(p_instance, &head, color, false));
    APP_ERROR_CHECK(nrf_gfx_rect_draw(p_instance, &left_eye, 1, color, true));
    APP_ERROR_CHECK(nrf_gfx_rect_draw(p_instance, &right_eye, 1, color, true));

    return NRF_SUCCESS;
}


static void lfclk_config(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

static void bsp_event_handler(bsp_event_t event)
{
    // Each time the button 1 or 4 is pushed we start a transaction with invert command
    // to LCD display
    switch (event)
    {
    case BSP_EVENT_KEY_0: // Button 1 pushed.
        nrf_gfx_invert(&m_nrf_lcd, true);
        break;

    case BSP_EVENT_KEY_3: // Button 4 pushed.
        nrf_gfx_invert(&m_nrf_lcd, false);
        break;

    default:
        break;
    }
}


static void timer_handler(void * p_context)
{
    static uint8_t modifier = 0;

    switch (modifier)
    {
        case 0:
        {
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_LEFT, FACE_SHOW));
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_CENTER, FACE_ERASE));
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_RIGHT, FACE_ERASE));
            nrf_gfx_display(&m_nrf_lcd);
            break;
        }
        case 1:
        {
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_LEFT, FACE_ERASE));
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_CENTER, FACE_SHOW));
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_RIGHT, FACE_ERASE));
            nrf_gfx_display(&m_nrf_lcd);
            break;
        }
        case 2:
        default:
        {
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_LEFT, FACE_ERASE));
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_CENTER, FACE_ERASE));
            APP_ERROR_CHECK(happy_face_draw(&m_nrf_lcd, FACE_RIGHT, FACE_SHOW));
            nrf_gfx_display(&m_nrf_lcd);
            break;
        }
    }
    if (++modifier > 2)
    {
        modifier = 0;
    }
}


static void timer_interrupt_init(void)
{
    ret_code_t err_code;

    err_code = app_timer_create(&m_timer, APP_TIMER_MODE_REPEATED, timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_timer, APP_TIMER_TICKS(1000), NULL);
    APP_ERROR_CHECK(err_code);
}


static void text_display(void)
{
    nrf_gfx_point_t const text_point =
    {
        .x = 5,
        .y = 1
    };
    APP_ERROR_CHECK(nrf_gfx_print(&m_nrf_lcd, &text_point, 1,
                    "  NORDICSEMI", &orkney_8ptFontInfo, false));
}


int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    lfclk_config();
    APP_ERROR_CHECK(app_timer_init());
    timer_interrupt_init();
    APP_ERROR_CHECK(nrf_pwr_mgmt_init());
    APP_ERROR_CHECK(bsp_init(BSP_INIT_BUTTONS, bsp_event_handler));
    APP_ERROR_CHECK(nrf_gfx_init(&m_nrf_lcd));

    NRF_LOG_INFO("SPI transaction manager example started. \n\r");

    text_display();

    while (true)
    {
        nrf_pwr_mgmt_run();
        NRF_LOG_FLUSH();
    }
}

