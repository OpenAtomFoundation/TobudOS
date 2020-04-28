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

#include "tos_k.h"

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
    return port_clz(val);
#endif
}

__API__ void tos_cpu_int_disable(void)
{
    port_int_disable();
}

__API__ void tos_cpu_int_enable(void)
{
    port_int_enable();
}

__API__ cpu_cpsr_t tos_cpu_cpsr_save(void)
{
    return port_cpsr_save();
}

__API__ void tos_cpu_cpsr_restore(cpu_cpsr_t cpsr)
{
    port_cpsr_restore(cpsr);
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
#if 0
    port_systick_priority_set(TOS_CFG_CPU_SYSTICK_PRIO);
    port_systick_config(cycle_per_tick);
#endif
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

__KNL__ k_stack_t *cpu_task_stk_init(void *entry,
                                              void *arg,
                                              void *exit,
                                              k_stack_t *stk_base,
                                              size_t stk_size)
{
    cpu_data_t *sp;

    sp = (cpu_data_t *)&stk_base[stk_size];

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u
    uint8_t *slot = (uint8_t *)&stk_base[0];
    for (; slot < (uint8_t *)sp; ++slot) {
        *slot = 0xCC;
    }
#endif

    *--sp = (cpu_data_t)((uint16_t)exit & 0xFF);           /* LRL */
    *--sp = (cpu_data_t)(((uint16_t)exit >> 8) & 0xFF);    /* LRH */

    *--sp = (cpu_data_t)((uint16_t)entry & 0xFF);           /* PCL */
    *--sp = (cpu_data_t)(((uint16_t)entry >> 8) & 0xFF);    /* PCH */

    *--sp = (cpu_data_t)0x59;                               /* YL */
    *--sp = (cpu_data_t)0x59;                               /* YH */

    *--sp = (cpu_data_t)((uint16_t)arg & 0xFF);             /* XL */
    *--sp = (cpu_data_t)(((uint16_t)arg >> 8) & 0xFF);      /* XH */

    *--sp = 0xAA;                                           /* A */

    /*
    -----------------------------------
    | 0 | 1 | 2 | 3  | 4 | 5  | 6 | 7 |
    -----------------------------------
    | C | Z | N | I0 | H | I1 | - | V |
    -----------------------------------

    Interruptability levels:
    ----------------------------------------------
    | Interruptability      | Priority | I1 | I0 |
    ----------------------------------------------
    | Interruptable Main    | Lowest   | 1  | 0  |
    ------------------------|          |----------
    | Interruptable Level 1 |    ^     | 0  | 1  |
    ------------------------|    |     |----------
    | Interruptable Level 2 |    |     | 0  | 0  |
    ------------------------|          |----------
    | Non Interruptable     | Highest  | 1  | 1  |
    ------------------------------------------------
     */
    *--sp = 0x00;                                           /* CC */

    *--sp = 0x08;                                           /* b8 */
    *--sp = 0x09;                                           /* b9 */
    *--sp = 0x10;                                           /* b10 */
    *--sp = 0x11;                                           /* b11 */
    *--sp = 0x12;                                           /* b12 */
    *--sp = 0x13;                                           /* b13 */
    *--sp = 0x14;                                           /* b14 */
    *--sp = 0x15;                                           /* b15 */

    /* ATTENTION:
       must do a sp decrease here in STM8
     */
    return (k_stack_t *)(--sp);
}

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u

__KNL__ k_err_t cpu_task_stack_draught_depth(k_stack_t *stk_base, size_t stk_size, int *depth)
{
    uint8_t *slot;
    uint8_t *sp, *bp;
    int the_depth = 0;

    bp = (uint8_t *)&stk_base[0];

    sp = &stk_base[stk_size];

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

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u

__KNL__ void cpu_fault_diagnosis(void)
{
    port_fault_diagnosis();
}

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

