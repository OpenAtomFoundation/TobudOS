/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include <tos_k.h>
#include "ch32v30x.h"
#include "core_riscv.h"

__PORT__ void port_cpu_reset(void)
{
    NVIC_SystemReset();
}

/* clear soft interrupt */
__PORT__ void sw_clearpend(void)
{
    SysTick->CTLR &= ~(1<<31);
}

/* trigger software interrupt */
__PORT__ void port_context_switch(void)
{
    SysTick->CTLR |= (1<<31);
}

/* trigger software interrupt */
__PORT__ void port_irq_context_switch(void)
{
    SysTick->CTLR |= (1<<31);
}

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{
    SysTick->CTLR=0;
    SysTick->SR=0;
    SysTick->CNT=0;
    SysTick->CMP=cycle_per_tick-1;
    SysTick->CTLR=0xF;
}

__PORT__ void port_systick_priority_set(uint32_t prio)
{
    NVIC_SetPriority(SysTicK_IRQn, prio);
}

__PORT__ void port_cpu_init()
{
    NVIC_SetPriority(Software_IRQn,0xf0);
    NVIC_EnableIRQ(SysTicK_IRQn);
    NVIC_EnableIRQ(Software_IRQn);
}


void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler(void)
{
    GET_INT_SP();  /* 切换中断栈 */
    if (tos_knl_is_running())
    {
      tos_knl_irq_enter();
      SysTick->SR=0;
      tos_tick_handler();
      tos_knl_irq_leave();
    }
    FREE_INT_SP(); /* 释放中断栈 */
}



#if TOS_CFG_TICKLESS_EN > 0u
__PORT__ k_time_t port_systick_max_delay_millisecond(void)
{
    k_time_t max_millisecond;
    uint32_t max_cycle;

    max_cycle = 0xffffffff; // systick 是64位，这里用低32位
    max_millisecond = (k_time_t)((uint64_t)max_cycle * K_TIME_MILLISEC_PER_SEC / TOS_CFG_CPU_CLOCK); // CLOCK: cycle per second
    return max_millisecond;
}

__PORT__ void port_systick_resume(void)
{
    SysTick->CTLR |= (3<<0);
}

__PORT__ void port_systick_suspend(void)
{
    SysTick->CTLR &= ~(3<<0);
}

__PORT__ k_cycle_t port_systick_max_reload_cycle(void)
{
    return 0xffffffff;
}

__PORT__ void port_systick_reload(uint32_t cycle_per_tick)
{
    port_systick_config(cycle_per_tick);
}

__PORT__ void port_systick_pending_reset(void)
{
    PFIC->IPRR[0] |= (1<<12); //clear pend
}

#endif


#if TOS_CFG_PWR_MGR_EN > 0u
__PORT__ void port_sleep_mode_enter(void)
{
    /* only CPU sleep */
    PFIC->SCTLR |= (1<<2);
    __WFI();
    PFIC->SCTLR &= ~(1<<2);
}

__PORT__ void port_stop_mode_enter(void)
{
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
}

__PORT__ void port_standby_mode_enter(void)
{
    PWR_EnterSTANDBYMode();
}

#endif







