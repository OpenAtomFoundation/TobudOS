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
 * @brief TFT Example Application main file.
 *
 * This file contains the source code for a sample application using the
 * GFX library based on the ILI9341 controller.
 *
 */

#include "nrf_gfx.h"
#include "nrf52_dk.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define GRAY            0xC618
#define RED             0xF800
#define BLUE            0x001F

#define LINE_STEP       10

#define CIRCLE_RADIUS   10
#define CIRCLE_STEP     ((2 * CIRCLE_RADIUS) + 1)

#define BORDER          2

static const char * test_text = "nRF52 family";

extern const nrf_gfx_font_desc_t orkney_24ptFontInfo;
extern const nrf_lcd_t nrf_lcd_ili9341;
extern const nrf_lcd_t nrf_lcd_st7735;

static const nrf_gfx_font_desc_t * p_font = &orkney_24ptFontInfo;
static const nrf_lcd_t * p_lcd = &nrf_lcd_ili9341;

static void gfx_initialization(void)
{
    APP_ERROR_CHECK(nrf_gfx_init(p_lcd));
}

static void brackground_set(void)
{
    nrf_gfx_invert(p_lcd, true);
    nrf_gfx_background_set(p_lcd, nrf52);
    nrf_gfx_invert(p_lcd, false);
}

static void text_print(void)
{
    nrf_gfx_point_t text_start = NRF_GFX_POINT(5, nrf_gfx_height_get(p_lcd) - 50);
    APP_ERROR_CHECK(nrf_gfx_print(p_lcd, &text_start, 0, test_text, p_font, true));
}

static void screen_clear(void)
{
    nrf_gfx_screen_fill(p_lcd, GRAY);
}

static void line_draw(void)
{
    nrf_gfx_line_t my_line = NRF_GFX_LINE(0, 0, 0, nrf_gfx_height_get(p_lcd), 2);
    nrf_gfx_line_t my_line_2 = NRF_GFX_LINE(nrf_gfx_width_get(p_lcd), nrf_gfx_height_get(p_lcd), 0, nrf_gfx_height_get(p_lcd), 1);

    for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += LINE_STEP)
    {
        my_line.x_end = i;
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line, RED));
    }

    my_line.x_end = nrf_gfx_width_get(p_lcd);

    for (uint16_t i = 0; i <= nrf_gfx_height_get(p_lcd); i += LINE_STEP)
    {
        my_line.y_end = (nrf_gfx_height_get(p_lcd) - i);
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line, RED));
    }

    for (uint16_t i = 0; i <= nrf_gfx_height_get(p_lcd); i += LINE_STEP)
    {
        my_line_2.y_end = (nrf_gfx_height_get(p_lcd) - i);
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line_2, BLUE));
    }

    my_line_2.y_end = 0;

    for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += LINE_STEP)
    {
        my_line_2.x_end = i;
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line_2, BLUE));
    }
}

static void circle_draw(void)
{
    nrf_gfx_circle_t my_circle = NRF_GFX_CIRCLE(0, 0, CIRCLE_RADIUS);

    for (uint16_t j = 0; j <= nrf_gfx_height_get(p_lcd); j += CIRCLE_STEP)
    {
        my_circle.y = j;
        for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += CIRCLE_STEP)
        {
            my_circle.x = i;
            APP_ERROR_CHECK(nrf_gfx_circle_draw(p_lcd, &my_circle, BLUE, true));
        }
    }

    for (uint16_t j = CIRCLE_RADIUS; j <= nrf_gfx_height_get(p_lcd) + CIRCLE_RADIUS; j += CIRCLE_STEP)
    {
        my_circle.y = j;
        for (uint16_t i = CIRCLE_RADIUS; i <= nrf_gfx_width_get(p_lcd) + CIRCLE_RADIUS; i += CIRCLE_STEP)
        {
            my_circle.x = i;
            APP_ERROR_CHECK(nrf_gfx_circle_draw(p_lcd, &my_circle, RED, false));
        }
    }
}

static void rect_draw(void)
{
    nrf_gfx_rect_t my_rect = NRF_GFX_RECT(nrf_gfx_width_get(p_lcd) / 2,
                             nrf_gfx_height_get(p_lcd) / nrf_gfx_width_get(p_lcd),
                             nrf_gfx_height_get(p_lcd),
                             BORDER);
    nrf_gfx_rect_t my_rect_fill = NRF_GFX_RECT(nrf_gfx_width_get(p_lcd) / 2,
                                  nrf_gfx_height_get(p_lcd) / nrf_gfx_width_get(p_lcd),
                                  nrf_gfx_height_get(p_lcd),
                                  BORDER);

    nrf_gfx_rotation_set(p_lcd, NRF_LCD_ROTATE_90);

    for (uint16_t i = 0, j = 0;
        i <= (nrf_gfx_width_get(p_lcd) - (2 * BORDER)) / 2 &&
        j <= (nrf_gfx_height_get(p_lcd) - (2 * BORDER)) / 2;
        i += 6, j += 8)
    {
        my_rect.x = i;
        my_rect.y = j;
        my_rect_fill.x = i + BORDER;
        my_rect_fill.y = j + BORDER;
        my_rect.width = nrf_gfx_width_get(p_lcd) - i * 2;
        my_rect.height = nrf_gfx_height_get(p_lcd) - j * 2;
        my_rect_fill.width = nrf_gfx_width_get(p_lcd) - i * 2 - (2 * BORDER);
        my_rect_fill.height = nrf_gfx_height_get(p_lcd) - j * 2 - (2 * BORDER);

        // Draw using pseudo-random colors.
        APP_ERROR_CHECK(nrf_gfx_rect_draw(p_lcd, &my_rect, 2, ((i + j) * 10), false));
        APP_ERROR_CHECK(nrf_gfx_rect_draw(p_lcd, &my_rect_fill, 2, (UINT16_MAX - (i + j) * 10), true));
    }

    nrf_gfx_rotation_set(p_lcd, NRF_LCD_ROTATE_0);

}

int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("GFX usage example application started.")
    NRF_LOG_FLUSH();

    gfx_initialization();

    while (1)
    {
       brackground_set();
       text_print();
       nrf_delay_ms(1000);
       screen_clear();
       line_draw();
       nrf_delay_ms(1000);
       screen_clear();
       circle_draw();
       nrf_delay_ms(1000);
       screen_clear();
       rect_draw();
       nrf_delay_ms(1000);
    }
}

