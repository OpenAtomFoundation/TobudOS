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

#ifndef _st7565LCD_H__
#define _st7565LCD_H__

#include "sdk_errors.h"
#include "nrf_lcd.h"

// Details concerning used LCD display. The one used in the example is 128x32 pixels.
#define ST7565_LCD_WIDTH    ((uint16_t)128)
#define ST7565_LCD_HEIGHT   ((uint16_t)32)


// Functions below are required by nrf_gfx library

/**
 * @brief Function for initializing an SPI transaction manager and the LCD controller.
 */
ret_code_t st7565_init(void);


/**
 * @brief Function for uninitializing an SPI transaction manager instance and for clearing
 * the LCD display buffer
 *
 */
void st7565_uninit(void);


/**
 * @brief Function for drawing a single pixel in the LCD display buffer.
 *
 * @param[in] x             Horizontal coordinate of the pixel.
 * @param[in] y             Vertical coordinate of the pixel.
 * @param[in] color         Color of the pixel in LCD accepted format.
 */
void st7565_pixel_draw(uint16_t x, uint16_t y, uint32_t color);


/**
 * @brief Function for drawing a filled rectangle in the LCD display buffer.
 *
 * @param[in] x             Horizontal coordinate of the point where to start drawing the rectangle.
 * @param[in] y             Vertical coordinate of the point where to start drawing the rectangle.
 * @param[in] width         Width of the image.
 * @param[in] height        Height of the image.
 * @param[in] color         Color with which to fill the rectangle in LCD accepted format.
 */
void st7565_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);


/**
 * @brief Function for displaying data from an LCD display buffer
 *
 */
void st7565_display_screen(void);


/**
 * @brief Function for setting inversion of colors on the screen.
 *
 * @param[in] invert        If true, inversion will be set.
 */
void st7565_display_invert(bool invert);


/**
 * @brief Function for clearing the LCD display buffer and the LCD screen
 *
 */
void st7565_clear_display(void);

#endif

