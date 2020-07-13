/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-14 22:37:23
 * @LastEditTime : 2020-02-14 23:37:18
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _BSP_INIT_H_
#define _BSP_INIT_H_

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "bsp_led.h"
#include "bsp_dwt.h"
#include "bsp_usart.h"
#include "bsp_motor.h"

void bsp_init(void);
void SystemClock_Config(void);

#endif


