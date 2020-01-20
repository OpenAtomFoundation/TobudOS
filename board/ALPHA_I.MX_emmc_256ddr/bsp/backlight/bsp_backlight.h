#ifndef _BACKLIGHT_H
#define _BACKLIGHT_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_backlight.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : LCD背光PWM驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/22 左忠凯创建
***************************************************************/
#include "bsp.h"

/* 背光PWM结构体 */
struct backlight_dev_struc
{
	unsigned char pwm_duty;		/* 占空比	*/
};


/* 函数声明 */
void backlight_init(void);
void pwm1_enable(void);
void pwm1_setsample_value(unsigned int value);
void pwm1_setperiod_value(unsigned int value);
void pwm1_setduty(unsigned char duty);
void pwm1_irqhandler(void);

#endif
