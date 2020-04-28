/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-15 20:51:38
 * @LastEditTime: 2020-02-16 00:07:34
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _BSP_DWT_H
#define _BSP_DWT_H

#include "stdint.h"

#define USE_DWT_DELAY			1	/* 使用dwt内核精确延时 */

#if USE_DWT_DELAY
#define USE_TICK_DELAY		0		/* 不使用SysTick延时 */
#else
#define USE_TICK_DELAY		1		/* 使用SysTick延时 */
#endif

#if USE_DWT_DELAY

/* 获取内核时钟频率 */            
#define get_cpu_clk_freq()          (72000000U)
#define system_clk_freq             (72000000U)
/* 为方便使用，在延时函数内部调用dwt_init函数初始化时间戳寄存器，
   这样每次调用函数都会初始化一遍。
   把本宏值设置为0，然后在main函数刚运行时调用dwt_init可避免每次都初始化 */  

#define CPU_TS_INIT_IN_DELAY_FUNCTION   0  


/*******************************************************************************
 * 							函数声明
 ******************************************************************************/
uint32_t dwt_read(void);
void dwt_init(void);

//使用以下函数前必须先调用dwt_init函数使能计数器，或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
//最大延时值为60秒
void dwt_delay_us(uint32_t us);
#define dwt_delay_ms(ms)     dwt_delay_us(ms*1000)
#define dwt_delay_s(s)       dwt_delay_ms(s*1000)

/* 最大延时 60s=2的32次方/72000000 */
#define delay_ms(ms)  	dwt_delay_ms(ms)
#define delay_us(us)  	dwt_delay_us(us)
#define delay_s(s)  	dwt_delay_s(s)

#endif

#endif /* _BSP_DWT_H */
