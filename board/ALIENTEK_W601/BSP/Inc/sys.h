#ifndef __SYS_H
#define __SYS_H

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK W601开发板
 *	系统时钟初始化
 *	包括时钟设置/中断管理等
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2019/7/10
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2019-2029
 *	All rights reserved
 *	******************************************************************************
 *	版本修改说明
 *	无
 *	******************************************************************************/


#include "misc.h"
#include "wm_type_def.h"
#include "wm_cpu.h"
#include "wm_ram_config.h"
#include "wm_gpio.h"
#include "wm_regs.h"
#include "wm_gpio_afsel.h"
#include "wm_uart_new.h"
#include "wm_uart.h"
#include "wm_pmu.h"




//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))


//IO口操作,只对单一的IO口!
//确保n的值小于32!
#define PAout(n)   BIT_ADDR(HR_GPIOA_DATA,n)  //输出 
#define PAin(n)    BIT_ADDR(HR_GPIOA_DATA,n)  //输入 

#define PBout(n)   BIT_ADDR(HR_GPIOB_DATA,n)  //输出 
#define PBin(n)    BIT_ADDR(HR_GPIOB_DATA,n)  //输入 


//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址

#endif











