#ifndef __CORE_DELAY_H
#define __CORE_DELAY_H

#include "core_delay.h"
#include "stdint.h"

#define USE_DWT_DELAY			1	/* 使用dwt内核精确延时 */

#if USE_DWT_DELAY
#define USE_TICK_DELAY		0		/* 不使用SysTick延时 */
#else
#define USE_TICK_DELAY		1		/* 使用SysTick延时 */
#endif

#if USE_DWT_DELAY

#define Delay_ms(ms)  	CPU_TS_Tmr_Delay_MS(ms)
#define Delay_us(us)  	CPU_TS_Tmr_Delay_US(us)
/* 最大延时 60s=2的32次方/72000000 */
#define Delay_s(s)  	  CPU_TS_Tmr_Delay_S(s)

/* 获取内核时钟频率 */            
#define GET_CPU_ClkFreq()       (180000000)
#define SysClockFreq            (180000000)
/* 为方便使用，在延时函数内部调用CPU_TS_TmrInit函数初始化时间戳寄存器，
   这样每次调用函数都会初始化一遍。
   把本宏值设置为0，然后在main函数刚运行时调用CPU_TS_TmrInit可避免每次都初始化 */  

#define CPU_TS_INIT_IN_DELAY_FUNCTION   0  


/*******************************************************************************
 * 							函数声明
 ******************************************************************************/
uint32_t CPU_TS_TmrRd(void);
void CPU_TS_TmrInit(void);

//使用以下函数前必须先调用CPU_TS_TmrInit函数使能计数器，或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
//最大延时值为60秒
void CPU_TS_Tmr_Delay_US(uint32_t us);
#define CPU_TS_Tmr_Delay_MS(ms)     CPU_TS_Tmr_Delay_US(ms*1000)
#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)

#endif

#endif /* __CORE_DELAY_H */
