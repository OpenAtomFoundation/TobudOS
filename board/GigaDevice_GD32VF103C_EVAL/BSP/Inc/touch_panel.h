/*!
    \file  touch_panel.h 
    \brief LCD touch panel driver header file

    \version 2019-06-05, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#ifndef TOUCH_PANEL_H
#define TOUCH_PANEL_H

#include "gd32vf103.h"

#define AD_Left     300
#define AD_Right    3850
#define AD_Top      220
#define AD_Bottom   3850

#define LCD_X       240
#define LCD_Y       320

#define CH_X        0XD2
#define CH_Y        0X92

/* SPI SCK pin */
#define  SPI_SCK_PIN               GPIO_PIN_5
#define  SPI_SCK_PORT              GPIOA
#define  SPI_SCK_LOW()             gpio_bit_reset(SPI_SCK_PORT, SPI_SCK_PIN)
#define  SPI_SCK_HIGH()            gpio_bit_set(SPI_SCK_PORT, SPI_SCK_PIN)

/* SPI MOSI pin */
#define  SPI_MOSI_PIN              GPIO_PIN_7
#define  SPI_MOSI_PORT             GPIOA
#define  SPI_MOSI_LOW()            gpio_bit_reset(SPI_MOSI_PORT, SPI_MOSI_PIN)
#define  SPI_MOSI_HIGH()           gpio_bit_set(SPI_MOSI_PORT, SPI_MOSI_PIN)

/* SPI MISO pin */
#define  SPI_MISO_PIN              GPIO_PIN_6
#define  SPI_MISO_PORT             GPIOA
#define  SPI_MISO_READ()           gpio_input_bit_get(SPI_MISO_PORT, SPI_MISO_PIN)

/* SPI Chip select pin */
#define  SPI_TOUCH_CS_PIN          GPIO_PIN_4
#define  SPI_TOUCH_CS_PORT         GPIOE
#define  SPI_TOUCH_CS_LOW()        gpio_bit_reset(SPI_TOUCH_CS_PORT, SPI_TOUCH_CS_PIN)
#define  SPI_TOUCH_CS_HIGH()       gpio_bit_set(SPI_TOUCH_CS_PORT, SPI_TOUCH_CS_PIN)

/* LCD touch interrupt request pin */
#define  TOUCH_PEN_INT_PIN         GPIO_PIN_5
#define  TOUCH_PEN_INT_PORT        GPIOE
#define  TOUCH_PEN_INT_READ()      gpio_input_bit_get(TOUCH_PEN_INT_PORT, TOUCH_PEN_INT_PIN)


/* touch panel gpio configure */
void touch_panel_gpio_configure(void);
/* touch start */
void touch_start(void);
/* write data to touch screen */
void touch_write(uint8_t d);
/* read the touch AD value */
uint16_t touch_read(void);
/* read the touch pen interrupt request signal */
FlagStatus touch_pen_irq(void);
/* get the AD sample value of touch location at X coordinate */
uint16_t touch_ad_x_get(void);
/* get the AD sample value of touch location at Y coordinate */
uint16_t touch_ad_y_get(void);
/* get channel X+ AD average sample value */
uint16_t touch_average_ad_x_get(void);
/* get channel Y+ AD average sample value */
uint16_t touch_average_ad_y_get(void);
/* get X coordinate value of touch point on LCD screen */
uint16_t touch_coordinate_x_get(uint16_t adx);
/* get Y coordinate value of touch point on LCD screen */
uint16_t touch_coordinate_y_get(uint16_t ady);

uint16_t touch_data_filter(uint8_t channel_select);
ErrStatus touch_ad_xy_get(uint16_t *ad_x, uint16_t *ad_y);
ErrStatus touch_scan(void);

#endif /* TOUCH_PANEL_H */
