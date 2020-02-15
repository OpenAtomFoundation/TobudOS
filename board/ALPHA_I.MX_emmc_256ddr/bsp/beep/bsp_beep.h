#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_beep.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : 蜂鸣器驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/
#include "bsp.h"


/* 函数声明 */
void beep_init(void);
void beep_switch(int status);
#endif

