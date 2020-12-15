/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/
#include "tos_k.h"
#include "stdbool.h"
#include "e53_sc1.h"

extern void key1_handler_callback(void);

static k_timer_t key_check_tmr;
static uint32_t key_press_count = 0;
static uint32_t key_release_count = 0;
static bool key_pressed = false;

#define KEY_CHECK_SHAKING_COUNT		3

static void key_check_timer_cb(void *arg)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0)){
        key_press_count++;
        key_release_count = 0;
    } else {
        key_press_count = 0;
        key_release_count++;
    }

    // 支持按键消抖
    if (key_press_count > KEY_CHECK_SHAKING_COUNT) {
        // 按键按下时功能即生效，且只生效一次
        if (!key_pressed) {
            //printf("key 1 pressed\r\n");
            key_pressed = true;
            key1_handler_callback();
        }
    }
    if (key_release_count > KEY_CHECK_SHAKING_COUNT){
        if (key_pressed) {
            key_pressed = false;
            //printf("key 1 release\r\n");
        }
    }
}

void stm32g0xx_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	 /*Configure GPIO pin : PtPin */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	// 使用tencent-os tiny的定时器实现按键检测功能
	tos_timer_create(&key_check_tmr, 1u, 10u, key_check_timer_cb, K_NULL, TOS_OPT_TIMER_PERIODIC);
	tos_timer_start(&key_check_tmr);
}
