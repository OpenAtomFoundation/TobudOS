#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "bsp.h"
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_led.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : LED驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/

#define LED0	0

/* 函数声明 */
void led_init(void);
void led_switch(int led, int status);
#endif

