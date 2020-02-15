#ifndef _BSP_SPI_H
#define _BSP_SPI_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_spi.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : SPI驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/17 左忠凯创建
***************************************************************/
#include "bsp.h"

/* 函数声明 */
void spi_init(ECSPI_Type *base);
unsigned char spich0_readwrite_byte(ECSPI_Type *base, unsigned char txdata);

#endif

