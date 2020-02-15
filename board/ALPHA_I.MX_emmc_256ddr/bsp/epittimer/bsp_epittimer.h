#ifndef _BSP_EPITTIMER_H
#define _BSP_EPITTIMER_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_epittimer.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : EPIT定时器驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/5 左忠凯创建
***************************************************************/
#include "bsp.h"


/* 函数声明 */
void epit1_init(unsigned int frac, unsigned int value);
void epit1_irqhandler(void *arg);

#endif
