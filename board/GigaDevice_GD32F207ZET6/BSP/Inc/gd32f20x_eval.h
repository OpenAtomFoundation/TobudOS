/*!
    \file  gd32f207i_eval.h
    \brief definitions for GD32F207i_EVAL's leds, keys and COM ports hardware resources

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
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

#ifndef GD32F20X_EVAL_H
#define GD32F20X_EVAL_H

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32f20x.h"
     
/* exported types */
typedef enum 
{
    LED2 = 0,
    LED3 = 1,
    LED4 = 2,
    LED5 = 3
} led_typedef_enum;

typedef enum 
{
    KEY_WAKEUP = 0,
    KEY_TAMPER = 1,
    KEY_USER = 2,
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                             3U

#define LED2_PIN                         GPIO_PIN_11
#define LED2_GPIO_PORT                   GPIOD
#define LED2_GPIO_CLK                    RCU_GPIOD
  
#define LED3_PIN                         GPIO_PIN_12
#define LED3_GPIO_PORT                   GPIOD
#define LED3_GPIO_CLK                    RCU_GPIOD
  
#define LED4_PIN                         GPIO_PIN_13
#define LED4_GPIO_PORT                   GPIOD
#define LED4_GPIO_CLK                    RCU_GPIOD


#define COMn                             1U

#define EVAL_COM0                        USART1
#define EVAL_COM0_CLK                    RCU_USART1
#define EVAL_COM0_TX_PIN                 GPIO_PIN_6
#define EVAL_COM0_RX_PIN                 GPIO_PIN_7
#define EVAL_COM0_GPIO_PORT              GPIOB
#define EVAL_COM0_GPIO_CLK               RCU_GPIOB

#define KEYn                             2U

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_14
#define WAKEUP_KEY_GPIO_PORT             GPIOD
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOD
#define WAKEUP_KEY_EXTI_LINE             EXTI_14
#define WAKEUP_KEY_EXTI_PORT_SOURCE      GPIO_EVENT_PORT_GPIOD
#define WAKEUP_KEY_EXTI_PIN_SOURCE       GPIO_EVENT_PIN_14
#define WAKEUP_KEY_EXTI_IRQn             EXTI10_15_IRQn  

/* user push-button */
#define USER_KEY_PIN                     GPIO_PIN_15
#define USER_KEY_GPIO_PORT               GPIOD
#define USER_KEY_GPIO_CLK                RCU_GPIOD
#define USER_KEY_EXTI_LINE               EXTI_15
#define USER_KEY_EXTI_PORT_SOURCE        GPIO_EVENT_PORT_GPIOD
#define USER_KEY_EXTI_PIN_SOURCE         GPIO_EVENT_PIN_15
#define USER_KEY_EXTI_IRQn               EXTI10_15_IRQn

/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum key);
/* configure COM port */
void gd_eval_com_init(uint32_t com);

#ifdef cplusplus
}
#endif

#endif /* GD32F20X_EVAL_H */
