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

uint8_t irq_context_switch_flag = 0;

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

__KERNEL__ void cpu_init(void)
{

}

__KERNEL__ void cpu_reset(void)
{
    port_cpu_reset();
}

__KERNEL__ void cpu_sched_start(void)
{
    port_sched_start();
}

__KERNEL__ void cpu_context_switch(void)
{
    port_context_switch();
}

__KERNEL__ void cpu_irq_context_switch(void)
{
    irq_context_switch_flag = 1;//port_irq_context_switch();
}

__KERNEL__ void cpu_systick_init(k_cycle_t cycle_per_tick)
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
__KERNEL__ void cpu_systick_resume(void)
{
    port_systick_resume();
}

/**
 * @brief Disable systick interrupt
 *
 * @return None
 */
__KERNEL__ void cpu_systick_suspend(void)
{
    port_systick_suspend();
}

__KERNEL__ k_time_t cpu_systick_max_delay_millisecond(void)
{
    return port_systick_max_delay_millisecond();
}

__KERNEL__ void cpu_systick_expires_set(k_time_t millisecond)
{
    k_cycle_t cycles;

    cycles = (k_cycle_t)((uint64_t)millisecond * TOS_CFG_CPU_CLOCK / K_TIME_MILLISEC_PER_SEC); /* CLOCK means cycle per second */

    cpu_systick_reload(cycles - 12); /* interrupt delay */
}

__KERNEL__ void cpu_systick_pending_reset(void)
{
    port_systick_pending_reset();
}

__KERNEL__ void cpu_systick_reset(void)
{
    cpu_systick_reload(k_cpu_cycle_per_tick);
}

#endif /* TOS_CFG_TICKLESS_EN */

#if TOS_CFG_PWR_MGR_EN > 0u

__KERNEL__ void cpu_sleep_mode_enter(void)
{
    __bis_SR_register( LPM4_bits + GIE );
    __no_operation();
}

__KERNEL__ void cpu_stop_mode_enter(void)
{

}

__KERNEL__ void cpu_standby_mode_enter(void)
{

}

#endif /* TOS_CFG_PWR_MGR_EN */

__KERNEL__ k_stack_t *cpu_task_stk_init(void *entry,
                                              void *arg,
                                              void *exit,
                                              k_stack_t *stk_base,
                                              size_t stk_size)
{
    cpu_data_t *sp;
    uint16_t *pus_top_of_stack;
    uint32_t *pul_top_of_stack;

    #define PORT_BYTE_ALIGNMENT_MASK	( 0x0001 )

    /* The stack type changes depending on the data model. */

    sp = (cpu_data_t *)&( stk_base[ stk_size - ( uint32_t ) 1 ] );
    sp = ( cpu_data_t * ) ( ( ( cpu_data_t ) sp ) & ( ~( ( cpu_data_t ) PORT_BYTE_ALIGNMENT_MASK ) ) );

    /* cpu_data_t is either 16 bits or 32 bits depending on the data model.
    Some stacked items do not change size depending on the data model so have
    to be explicitly cast to the correct size so this function will work
    whichever data model is being used. */

    if( sizeof( cpu_data_t ) == sizeof( uint16_t ) )
    {
        /* Make room for a 20 bit value stored as a 32 bit value. */
        pus_top_of_stack = ( uint16_t * ) sp;
        pus_top_of_stack--;
        pul_top_of_stack = ( uint32_t * ) pus_top_of_stack;
    }
    else
    {
        pul_top_of_stack = ( uint32_t * ) sp;
    }

    /* PC - Interrupt return pointer */
    *pul_top_of_stack = ( uint32_t ) entry;

    pus_top_of_stack = ( uint16_t * ) pul_top_of_stack;
    pus_top_of_stack--;

    /* R2 - SR.GIE - bit8,golbal interrupt enable */
    *pus_top_of_stack = 0x08;
    /* SR size is 16-bits */
    pus_top_of_stack -= ( sizeof( cpu_data_t ) / 2 );


    /* From here on the size of stacked items depends on the memory model. */
    sp = ( cpu_data_t * ) pus_top_of_stack;

#if 0 // enable for debug
    *sp = ( cpu_data_t ) 0xffff;
    sp--;
    *sp = ( cpu_data_t ) 0xeeee;
    sp--;
    *sp = ( cpu_data_t ) 0xdddd;
    sp--;
    *sp = ( cpu_data_t ) arg;
    sp--;
    *sp = ( cpu_data_t ) 0xbbbb;
    sp--;
    *sp = ( cpu_data_t ) 0xaaaa;
    sp--;
    *sp = ( cpu_data_t ) 0x9999;
    sp--;
    *sp = ( cpu_data_t ) 0x8888;
    sp--;
    *sp = ( cpu_data_t ) 0x5555;
    sp--;
    *sp = ( cpu_data_t ) 0x6666;
    sp--;
    *sp = ( cpu_data_t ) 0x5555;
    sp--;
    *sp = ( cpu_data_t ) 0x4444;
#else
    sp -= 3;
    *sp = ( cpu_data_t ) arg;
    sp -= 8;// R11-R4
#endif

    return (k_stack_t *)sp;
}

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u

__KERNEL__ k_err_t cpu_task_stack_draught_depth(k_stack_t *stk_base, size_t stk_size, int *depth)
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

__KERNEL__ void cpu_fault_diagnosis(void)
{
    port_fault_diagnosis();
}

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

