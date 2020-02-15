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

    interrupt_init();
    chip_init();
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
    port_systick_priority_set(TOS_CFG_CPU_SYSTICK_PRIO);
    port_systick_config(cycle_per_tick);
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
    cpu_context_t *context;

    sp = (cpu_data_t *)&stk_base[stk_size];
    sp = (cpu_data_t *)((cpu_addr_t)sp & 0xFFFFFFF8);

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u
    uint8_t *slot = (uint8_t *)&stk_base[0];
    for (; slot < (uint8_t *)sp; ++slot) {
        *slot = 0xCC;
    }
#endif

    context = (cpu_context_t *)sp;
    --context;

#define CPSR_SVC_MODE   0x13
#define CPSR_BIT_T      0x20
    /* we donnot set the irq bit, so first time context switch should enable irq */
    context->cpsr   = CPSR_SVC_MODE;
    if ((cpu_addr_t)entry & 1) {
        context->cpsr |= CPSR_BIT_T;
    }

    context->pc     = (cpu_data_t)entry;
    context->lr     = (cpu_data_t)exit;

    context->r12    = (cpu_data_t)0x12121212u;
    context->r11    = (cpu_data_t)0x11111111u;
    context->r10    = (cpu_data_t)0x10101010u;
    context->r9     = (cpu_data_t)0x09090909u;
    context->r8     = (cpu_data_t)0x08080808u;
    context->r7     = (cpu_data_t)0x07070707u;
    context->r6     = (cpu_data_t)0x06060606u;
    context->r5     = (cpu_data_t)0x05050505u;
    context->r4     = (cpu_data_t)0x04040404u;
    context->r3     = (cpu_data_t)0x03030303u;
    context->r2     = (cpu_data_t)0x02020202u;
    context->r1     = (cpu_data_t)0x01010101u;
    context->r0     = (cpu_data_t)arg;

    return (k_stack_t *)context;
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

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u

#if defined (TOS_CFG_CPU_ARM_FPU_EN) && (TOS_CFG_CPU_ARM_FPU_EN == 1U)
__KNL__ void cpu_flush_fpu(void)
{
    (void)__get_FPSCR();
}
#endif

__KNL__ void cpu_fault_diagnosis(void)
{
    port_fault_diagnosis();
}

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

