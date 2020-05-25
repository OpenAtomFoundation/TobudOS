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
#include "embARC.h"

void tos_cpu_tick_handler(void)
{
    arc_timer_int_clear(BOARD_OS_TIMER_ID);
    tos_tick_handler();
}

__API__ uint32_t tos_cpu_clz(uint32_t val)
{
#if defined(TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT) && (TOS_CFG_CPU_LEAD_ZEROS_ASM_PRESENT == 0u)
    uint32_t nbr_lead_zeros = 0;

    if (!(val & 0XFFFF0000)) {
        val <<= 16;
        nbr_lead_zeros += 16;
    }

    if (!(val & 0XFF000000)) {
        val <<= 8;
        nbr_lead_zeros += 8;
    }

    if (!(val & 0XF0000000)) {
        val <<= 4;
        nbr_lead_zeros += 4;
    }

    if (!(val & 0XC0000000)) {
        val <<= 2;
        nbr_lead_zeros += 2;
    }

    if (!(val & 0X80000000)) {
        nbr_lead_zeros += 1;
    }

    if (!val) {
        nbr_lead_zeros += 1;
    }

    return (nbr_lead_zeros);
#else
    return (32 - port_cpu_clz(val));
#endif
}

__API__ void tos_cpu_int_disable(void)
{
    arc_lock();
}

__API__ void tos_cpu_int_enable(void)
{
    arc_unlock();
}

__API__ cpu_cpsr_t tos_cpu_cpsr_save(void)
{
    return (cpu_cpsr_t)arc_lock_save();
}

__API__ void tos_cpu_cpsr_restore(cpu_cpsr_t cpsr)
{
    arc_unlock_restore(cpsr);
}

__KNL__ void cpu_init(void)
{
    k_cpu_cycle_per_tick = TOS_CFG_CPU_CLOCK / k_cpu_tick_per_second;
    cpu_systick_init(k_cpu_cycle_per_tick);

#if (TOS_CFG_CPU_HRTIMER_EN > 0)
    tos_cpu_hrtimer_init();
#endif
}

__KNL__ void cpu_reset(void)
{
    port_cpu_reset();
}

__KNL__ void cpu_sched_start(void)
{
    port_sched_start();
}

__KNL__ void cpu_context_switch(void)
{
    port_context_switch();
}

__KNL__ void cpu_irq_context_switch(void)
{
    port_irq_context_switch();
}

__KNL__ void cpu_systick_init(k_cycle_t cycle_per_tick)
{
    int_disable(BOARD_OS_TIMER_INTNO); /* disable os timer interrupt */
    arc_timer_stop(BOARD_OS_TIMER_ID);
    arc_timer_start(BOARD_OS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cycle_per_tick);

    int_handler_install(BOARD_OS_TIMER_INTNO, (INT_HANDLER_T)tos_cpu_tick_handler);
    int_pri_set(BOARD_OS_TIMER_INTNO, INT_PRI_MIN + 1);
    int_enable(BOARD_OS_TIMER_INTNO);
}

#if TOS_CFG_TICKLESS_EN > 0u

/**
 * @brief Set value to systick reload value register
 *
 * @param cycles The value set to register
 *
 * @return None
 */
__STATIC_INLINE__ void cpu_systick_reload(k_cycle_t cycle_per_tick)
{
    port_systick_reload(cycle_per_tick);
}

/**
 * @brief Enable systick interrupt
 *
 * @return None
 */
__KNL__ void cpu_systick_resume(void)
{
    port_systick_resume();
}

/**
 * @brief Disable systick interrupt
 *
 * @return None
 */
__KNL__ void cpu_systick_suspend(void)
{
    port_systick_suspend();
}

__KNL__ k_time_t cpu_systick_max_delay_millisecond(void)
{
    return port_systick_max_delay_millisecond();
}

__KNL__ void cpu_systick_expires_set(k_time_t millisecond)
{
    k_cycle_t cycles;

    cycles = (k_cycle_t)((uint64_t)millisecond * TOS_CFG_CPU_CLOCK / K_TIME_MILLISEC_PER_SEC); /* CLOCK means cycle per second */

    cpu_systick_reload(cycles - 12); /* interrupt delay */
}

__KNL__ void cpu_systick_pending_reset(void)
{
    port_systick_pending_reset();
}

__KNL__ void cpu_systick_reset(void)
{
    cpu_systick_reload(k_cpu_cycle_per_tick);
}

#endif /* TOS_CFG_TICKLESS_EN */

#if TOS_CFG_PWR_MGR_EN > 0u

__KNL__ void cpu_sleep_mode_enter(void)
{
    port_sleep_mode_enter();
}

__KNL__ void cpu_stop_mode_enter(void)
{
    port_stop_mode_enter();
}

__KNL__ void cpu_standby_mode_enter(void)
{
    port_standby_mode_enter();
}

#endif /* TOS_CFG_PWR_MGR_EN */

uint32_t g_context_switch_reqflg;
uint32_t g_exc_nest_count;
extern void start_r(void);

#if ARC_FEATURE_STACK_CHECK
#define ARC_INIT_STATUS ((1 << AUX_STATUS_BIT_SC) | AUX_STATUS_MASK_IE | ((-1 - INT_PRI_MIN) << 1) | STATUS32_RESET_VALUE)
#else
#define ARC_INIT_STATUS (AUX_STATUS_MASK_IE | ((-1 - INT_PRI_MIN) << 1) | STATUS32_RESET_VALUE)
#endif

__KNL__ k_stack_t *cpu_task_stk_init(void *entry,
                                              void *arg,
                                              void *exit,
                                              k_stack_t *stk_base,
                                              size_t stk_size)
{
    cpu_data_t *sp;
    cpu_context_t *regs = 0;

    sp = (cpu_data_t *)&stk_base[stk_size];
    sp = (cpu_data_t *)((cpu_addr_t)sp & 0xFFFFFFFC);
    sp  -= (sizeof(cpu_context_t)/sizeof(cpu_data_t));
    regs = (cpu_context_t*) sp;

    regs->pc = (cpu_data_t)start_r;
    regs->blink = (cpu_data_t)exit;
    regs->task = (cpu_data_t)entry;
    regs->status32   = ARC_INIT_STATUS;
    regs->r0    = (cpu_data_t)arg;

    return (k_stack_t *)sp;
}

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u

__KNL__ k_err_t cpu_task_stack_draught_depth(k_stack_t *stk_base, size_t stk_size, int *depth)
{
    uint8_t *slot;
    uint8_t *sp, *bp;
    int the_depth = 0;

    bp = (uint8_t *)&stk_base[0];

    sp = &stk_base[stk_size];
    sp = (uint8_t *)((cpu_addr_t)sp & 0xFFFFFFF8);

    for (slot = sp - 1; slot >= bp; --slot) {
        if (*slot != 0xCC) {
            the_depth = sp - slot;
        }
    }

    *depth = the_depth;
    if (the_depth == stk_size) {
        return K_ERR_TASK_STK_OVERFLOW;
    }

    return K_ERR_NONE;
}

#endif
