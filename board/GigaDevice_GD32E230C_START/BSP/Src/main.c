/*!
    \file    main.c
    \brief   running LED
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "gd32e23x.h"
#include "gd32e230c_start.h"
#include "systick.h"
#include <stdio.h>
#include "cmsis_os.h"


#define TASK1_STK_SIZE		512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 2, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

int fputc(int ch, FILE *f)
{
  return ch;
}

void board_init(void)
{
    systick_config();
    
    /* enable the LED1 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure LED1 GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    /* reset LED1 GPIO pin */
    gpio_bit_reset(GPIOA,GPIO_PIN_7);

    /* enable the LED2 GPIO clock */
    /* configure LED2 GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* reset LED2 GPIO pin */
    gpio_bit_reset(GPIOA,GPIO_PIN_8);
    
    /* enable the LED3 GPIO clock */
    /* configure LED3 GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    /* reset LED3 GPIO pin */
    gpio_bit_reset(GPIOA,GPIO_PIN_11);
    
    /* enable the LED4 GPIO clock */
    /* configure LED4 GPIO port */ 
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    /* reset LED4 GPIO pin */
    gpio_bit_reset(GPIOA,GPIO_PIN_12);

//    while(1){
//        /* turn on LED1, turn off LED4 */
//        gpio_bit_set(GPIOA,GPIO_PIN_7);
//        gpio_bit_reset(GPIOA,GPIO_PIN_12);
//        delay_1ms(1000);
//        /* turn on LED2, turn off LED1 */
//        gpio_bit_set(GPIOA,GPIO_PIN_8);
//        gpio_bit_reset(GPIOA,GPIO_PIN_7);
//        delay_1ms(1000);
//        /* turn on LED3, turn off LED2 */
//        gpio_bit_set(GPIOA,GPIO_PIN_11);
//        gpio_bit_reset(GPIOA,GPIO_PIN_8);
//        delay_1ms(1000);
//        /* turn on LED4, turn off LED3 */
//        gpio_bit_set(GPIOA,GPIO_PIN_12);
//        gpio_bit_reset(GPIOA,GPIO_PIN_11);
//        delay_1ms(1000);
//    }
}

void task1(void *arg)
{
    int count = 1;
    while (1) {
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
        //printf("###This is task1, %d\r\n", count++);
        osDelay(2000);
    }
}

void task2(void *arg)
{
    int count = 1;
    while (1) {      
        //printf("***This is task2, %d\r\n", count++);
        gd_eval_led_toggle(LED3);
        gd_eval_led_toggle(LED4);
        osDelay(1000);
    }
}



int main(void)
{
    board_init();
    //printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
    osKernelStart();
}

