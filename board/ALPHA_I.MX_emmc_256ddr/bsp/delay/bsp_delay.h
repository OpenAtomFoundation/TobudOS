#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_delay.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : 延时头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建

		 V2.0 2019/1/15 左忠凯修改
		 添加了一些函数声明。
***************************************************************/
#include "bsp.h"


/* 函数声明 */
void delay_init(void);
void delayus(unsigned    int usdelay);
void delayms(unsigned	 int msdelay);
void delay(volatile unsigned int n);
void gpt1_irqhandler(void);

#endif

