#ifndef _BSP_AP3216C_H
#define _BSP_AP3216C_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_ap3216c.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : AP3216C驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/3/26 左忠凯创建
***************************************************************/
#include "bsp.h"

#define AP3216C_ADDR    	0X1E	/* AP3216C器件地址 */

/* AP3316C寄存器 */
#define AP3216C_SYSTEMCONG	0x00	/* 配置寄存器 			*/
#define AP3216C_INTSTATUS	0X01	/* 中断状态寄存器 			*/
#define AP3216C_INTCLEAR	0X02	/* 中断清除寄存器 			*/
#define AP3216C_IRDATALOW	0x0A	/* IR数据低字节 			*/
#define AP3216C_IRDATAHIGH	0x0B	/* IR数据高字节 			*/
#define AP3216C_ALSDATALOW	0x0C	/* ALS数据低字节 		*/
#define AP3216C_ALSDATAHIGH	0X0D	/* ALS数据高字节			*/
#define AP3216C_PSDATALOW	0X0E	/* PS数据低字节 			*/
#define AP3216C_PSDATAHIGH	0X0F	/* PS数据高字节 			*/

/* 函数声明 */
unsigned char ap3216c_init(void);
unsigned char ap3216c_readonebyte(unsigned char addr,unsigned char reg);
unsigned char ap3216c_writeonebyte(unsigned char addr,unsigned char reg, unsigned char data);
void ap3216c_readdata(unsigned short *ir, unsigned short *ps, unsigned short *als);

#endif


