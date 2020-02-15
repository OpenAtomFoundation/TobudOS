#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_touchscreen.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 触摸屏驱动头文件,触摸芯片为FT5426
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/21 左忠凯创建
***************************************************************/
#include "bsp.h"
#include "bsp_gpio.h"

/* 宏定义 */
#define FT5426_ADDR		0X38			/* FT5426设备地址 		*/

#define FT5426_DEVICE_MODE		0X00 	/* 模式寄存器 			*/
#define FT5426_IDGLIB_VERSION	0XA1 	/* 固件版本寄存器 			*/
#define FT5426_IDG_MODE			0XA4	/* 模式寄存器 			*/
#define FT5426_TD_STATUS		0X02	/* 触摸状态寄存器 			*/
#define FT5426_TOUCH1_XH		0X03	/* 触摸点坐标寄存器,
										 * 一个触摸点用6个寄存器存储坐标数据
										 */

#define FT5426_XYCOORDREG_NUM	30		/* 触摸点坐标寄存器数量 */
#define FT5426_INIT_FINISHED	1		/* 触摸屏初始化完成 			*/
#define FT5426_INIT_NOTFINISHED	0		/* 触摸屏初始化未完成 			*/

#define FT5426_TOUCH_EVENT_DOWN			0x00	/* 按下 		*/
#define FT5426_TOUCH_EVENT_UP			0x01	/* 释放 		*/
#define FT5426_TOUCH_EVENT_ON			0x02	/* 接触 		*/
#define FT5426_TOUCH_EVENT_RESERVED		0x03	/* 没有事件 */


/* 触摸屏结构体 */
struct ft5426_dev_struc
{
	unsigned char initfalg;		/* 触摸屏初始化状态 */
	unsigned char intflag;		/* 标记中断有没有发生 */
	unsigned char point_num;	/* 触摸点 		*/
	unsigned short x[5];		/* X轴坐标 	*/
	unsigned short y[5];		/* Y轴坐标 	*/

};

extern struct ft5426_dev_struc ft5426_dev;

/* 函数声明 */
void touchscreen_init(void);

void gpio1_io9_irqhandler(void);
unsigned char ft5426_write_byte(unsigned char addr,unsigned char reg, unsigned char data);
unsigned char ft5426_read_byte(unsigned char addr,unsigned char reg);
void ft426_read_len(unsigned char addr,unsigned char reg,unsigned char len,unsigned char *buf);
void ft5426_read_tpnum(void);
void ft5426_read_tpcoord(void);


#endif

