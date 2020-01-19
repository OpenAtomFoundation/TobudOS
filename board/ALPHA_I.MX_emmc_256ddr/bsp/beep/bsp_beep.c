#include "bsp_beep.h"
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: bsp_beep.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 蜂鸣器驱动文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/

/*
 * @description	: 初始化蜂鸣器对应的IO
 * @param 		: 无
 * @return 		: 无
 */
void beep_init(void)
{
	/* 1、初始化IO复用，复用为GPIO5_IO01 */
	IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0);		
	
	
	/* 2、、配置GPIO1_IO03的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
	 *bit [13]: 0 kepper功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 110 R0/6驱动能力
	 *bit [0]: 0 低转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0X10B0);
	
	/* 3、初始化GPIO,GPIO5_IO01设置为输出 */
	GPIO5->GDIR |= (1 << 1);	 

	/* 4、设置GPIO5_IO01输出高电平，关闭蜂鸣器 */
	GPIO5->DR |= (1 << 1);		
}

/*
 * @description		: 蜂鸣器控制函数，控制蜂鸣器打开还是关闭
 * @param - status	: 0，关闭蜂鸣器，1 打开蜂鸣器
 * @return 			: 无
 */
void beep_switch(int status)
{	
	if(status == ON)
		GPIO5->DR &= ~(1 << 1);	/* 打开蜂鸣器 */
	else if(status == OFF)
		GPIO5->DR |= (1 << 1);	/* 关闭蜂鸣器 */
}
