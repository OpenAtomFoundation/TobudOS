#ifndef __CORE_DELAY_H
#define __CORE_DELAY_H

#include "stm32f1xx.h"

/* 获取内核时钟频率 */
#define GET_CPU_ClkFreq()       HAL_RCC_GetSysClockFreq()
#define SysClockFreq            (72000000)

/* 为方便使用，在延时函数内部调用CPU_TS_TmrInit函数初始化时间戳寄存器，
   这样每次调用函数都会初始化一遍。
   把本宏值设置为0，然后在main函数刚运行时调用CPU_TS_TmrInit可避免每次都初始化 */  
#define CPU_TS_INIT_IN_DELAY_FUNCTION   0  

/*******************************************************************************
 * 函数声明
 ******************************************************************************/
uint32_t CPU_TS_TmrRd(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);

//使用以下函数前必须先调用CPU_TS_TmrInit函数使能计数器，或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
//最大延时值为8秒
void CPU_TS_Tmr_Delay_US(uint32_t us);
#define HAL_Delay(ms)     CPU_TS_Tmr_Delay_US(ms*1000)
#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)


#endif /* __CORE_DELAY_H */

