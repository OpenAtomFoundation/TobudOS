#ifndef _BSP_EXIT_H
#define _BSP_EXIT_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_exit.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 外部中断驱动头文件。
其他	   : 配置按键对应的GPIP为中断模式
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/
#include "bsp.h"

/* 函数声明 */
void exit_init(void);						/* 中断初始化 */
void gpio1_io18_irqhandler(void *arg); 			/* 中断处理函数 */

#endif
