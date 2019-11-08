/*!
    \file    lcd_log.c
    \brief   LCD log driver

    \version 2018-03-26, V1.0.0, demo for GD32E10x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "lcd_log.h"
#include "string.h"
#include "gd32e103v_lcd_eval.h"

uint16_t LINE;

char_format_struct charform = {
    .font = CHAR_FONT_8_16,
    .direction = CHAR_DIRECTION_HORIZONTAL,
    .char_color = RED,
    .bk_color = BLACK
};

/*!
    \brief      initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_clear(BLACK);
}

/*!
    \brief      de-initialize the LCD log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit (void)
{
}

/*!
    \brief      display the application header (title) on the LCD screen 
    \param[in]  p_title: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_header_set (uint8_t *p_title, uint16_t start_x)
{
    uint16_t i = 0;
    uint16_t str_len = strlen((const char *)p_title);

    lcd_rectangle_fill(210, 0, 240, 320, BLUE);

    charform.bk_color = BLUE;

    for (i = 0; i < str_len; i++) {
        lcd_char_display (230, (start_x + 8 * i), p_title[i], charform);
    }

}

/*!
    \brief      display the application footer (status) on the LCD screen 
    \param[in]  p_status: pointer to the string to be displayed
    \param[in]  start_x: the start x position
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *p_status, uint16_t start_x)
{
    uint16_t i = 0;
    uint16_t str_len = strlen((const char *)p_status);

    lcd_rectangle_fill(0, 0, 30, 320, BLUE);


    charform.bk_color = BLUE;

    for (i = 0; i < str_len; i++) {
        lcd_char_display (20, (start_x + 8 * i), p_status[i], charform);
    }

}

/*!
    \brief      clear the text zone 
    \param[in]  start_x: the start x position
    \param[in]  start_y: the start y position
    \param[in]  width: the width to clear text zone
    \param[in]  height: the heitht to clear text zone
    \param[out] none
    \retval     none
*/
void lcd_log_text_zone_clear(uint16_t start_x,
                             uint16_t start_y,
                             uint16_t width,
                             uint16_t height)
{
    lcd_rectangle_fill(start_x, start_y, width, height, BLACK);
}

/*!
    \brief      redirect the printf to the lcd 
    \param[in]  p_str: pointer to string to be displayed
    \param[in]  offset: the offset to set
    \param[in]  char_color: the clar color to set
    \param[out] none
    \retval     none
*/
void lcd_log_print (uint8_t *p_str, uint16_t offset, uint16_t char_color)
{
    uint16_t i;

#if defined(USE_HOST_MODE) && defined(USE_DEVICE_MODE)
    if(LINE <= 60)
    {
        LINE = 190;

        lcd_rectangle_fill(60, 0, 210, 320, BLACK);
    }
#else
    if(LINE <= 30)
    {
        LINE = 190;

        lcd_rectangle_fill(30, 0, 210, 320, BLACK);
    }
#endif

    charform.bk_color = BLACK;
    charform.char_color = char_color;

    for (i = 0; i < offset; i++) {
        lcd_char_display(LINE, (10 + 8 * i), *p_str++, charform);
    }

    LINE -= 20;
}
