#ifndef _BSP_KEYFILTER_H
#define _BSP_KEYFILTER_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_keyfilter.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 定时器按键消抖驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/5 左忠凯创建
***************************************************************/


/* 函数声明 */
void filterkey_init(void);
void filtertimer_init(unsigned int value);
void filtertimer_stop(void);
void filtertimer_restart(unsigned int value);
void filtertimer_irqhandler(void);
void gpio1_16_31_irqhandler(void);

#endif
