/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2018-11-22
* | Info        :

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "spi.h"
#include "tim.h"
#include "main.h"

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
 * GPIO config
**/
#define DC_Pin GPIO_PIN_12
#define DC_GPIO_Port GPIOB
#define RST_Pin GPIO_PIN_11
#define RST_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_12
#define CS_GPIO_Port GPIOA

#define DEV_RST_PIN     RST_GPIO_Port,RST_Pin		//PA11
#define DEV_DC_PIN      DC_GPIO_Port,DC_Pin			//PB12
#define DEV_CS_PIN			CS_GPIO_Port,CS_Pin			//PA12
#define DEV_BL_PIN			TIM4->CCR1							//PB6

/**
 * GPIO read and write
**/
#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
#define DEV_Digital_Read(_pin) HAL_GPIO_ReadPin(_pin)

/**
 * SPI
**/
#define DEV_SPI_WRITE(_dat)  DEV_SPI_WRite(_dat);

/**
 * delay x ms
**/
#define DEV_Delay_ms(__xms) HAL_Delay(__xms)

/**
 * PWM_BL
**/

#define DEV_Set_PWM(_Value)     DEV_BL_PIN= _Value

/*-----------------------------------------------------------------------------*/
void DEV_SPI_WRite(UBYTE _dat);
int DEV_Module_Init(void);
void DEV_Module_Exit(void);
#endif
