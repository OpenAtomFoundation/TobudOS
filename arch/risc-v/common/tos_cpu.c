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
#include <riscv_port.h>

#ifndef TOS_CFG_IRQ_STK_SIZE
#warning "did not specify the irq stack size, use default value"
#define TOS_CFG_IRQ_STK_SIZE 128
#endif

k_stack_t k_irq_stk[TOS_CFG_IRQ_STK_SIZE];
k_stack_t *k_irq_stk_top = k_irq_stk + TOS_CFG_IRQ_STK_SIZE;

__KERNEL__ void cpu_systick_init(k_cycle_t cycle_per_tick)
{
    port_systick_priority_set(TOS_CFG_CPU_SYSTICK_PRIO);
    port_systick_config(cycle_per_tick);
}

__KERNEL__ void cpu_init(void) {

    // reserve storage space for sp registers
    k_irq_stk_top = (k_stack_t *)(((cpu_addr_t) k_irq_stk_top) - sizeof(cpu_data_t));

    k_irq_stk_top = (k_stack_t *)(((cpu_addr_t) k_irq_stk_top) & 0xFFFFFFFC);

    k_cpu_cycle_per_tick = TOS_CFG_CPU_CLOCK / k_cpu_tick_per_second;

    cpu_systick_init(k_cpu_cycle_per_tick);

    port_cpu_init();
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


__KERNEL__ void cpu_context_switch(void)
{
    port_context_switch();
}

__KERNEL__ void cpu_irq_context_switch(void)
{
    // DO NOTHING
}

__KERNEL__ void cpu_sched_start(void)
{
    port_sched_start();
}

/*

Inx Offset Register
31    124    x31        t6
32    120    x30        t5
29    116    x29        t4
28    112    x28        t3
27    108    x27        s11
26    104    x26        s10
25    100    x25        s9
24    096    x24        s8
23    092    x23        s7
22    088    x22        s6
21    084    x21        s5
20    080    x20        s4
19    076    x19        s3
18    072    x18        s2
17    068    x17        a7
16    064    x16        a6
15    060    x15        a5
14    056    x14        a4
13    052    x13        a3
12    048    x12        a2
11    044    x11        a1
10    040    x10        a0
09    036    x9         s1
08    032    x8         s0/fp
07    028    x7         t2
06    024    x6         t1
05    020    x5         t0
04    016    x4         tp
03    012    x3         gp
02    008    x1         ra
01    004    mstatus
00    000    mepc

*/

__KERNEL__ k_stack_t *cpu_task_stk_init(void *entry,
                                              void *arg,
                                              void *exit,
                                              k_stack_t *stk_base,
                                              size_t stk_size)
{
    cpu_data_t *sp      = 0;
    cpu_context_t *regs = 0;

    sp = (cpu_data_t *)&stk_base[stk_size];
    sp = (cpu_data_t *)((cpu_addr_t)(sp) & 0xFFFFFFFC);

    sp  -= (sizeof(cpu_context_t)/sizeof(cpu_data_t));

    regs = (cpu_context_t*) sp;

    for(int i=1; i<(sizeof(cpu_context_t)/sizeof(cpu_data_t)); i++) {
        *(sp + i) = 0xACEADD00 | ((i / 10) << 4) | (i % 10);
    }

    cpu_data_t gp = 0;
    __ASM__ __VOLATILE__ ("mv %0, gp":"=r"(gp));

    regs->gp        = (cpu_data_t)gp;           // global pointer
    regs->a0        = (cpu_data_t)arg;          // argument
    regs->ra        = (cpu_data_t)exit;         // return address
    regs->mstatus   = (cpu_data_t)0x00001880;   // return to machine mode and enable interrupt
    regs->mepc      = (cpu_data_t)entry;        // task entry

    return (k_stack_t*)sp;
}

void cpu_trap_entry(cpu_data_t cause, cpu_context_t *regs)
{
    while (1) {
        // TODO
    }
}

void eclic_mtip_handler();
void cpu_irq_entry(cpu_data_t irq)
{
    typedef void (*irq_handler_t)();


    irq_handler_t *irq_handler_base = port_get_irq_vector_table();

    irq_handler_t irq_handler = irq_handler_base[irq];

    if(irq_handler == 0) {
        return;
    }


    (*irq_handler)();
}

__API__ uint32_t tos_cpu_clz(uint32_t val)
{
    uint32_t nbr_lead_zeros = 0;

    if (!(val & 0xFFFF0000)) {
        val <<= 16;
        nbr_lead_zeros += 16;
    }

    if (!(val & 0xFF000000)) {
        val <<= 8;
        nbr_lead_zeros += 8;
    }

    if (!(val & 0xF0000000)) {
        val <<= 4;
        nbr_lead_zeros += 4;
    }

    if (!(val & 0xC0000000)) {
        val <<= 2;
        nbr_lead_zeros += 2;
    }

    if (!(val & 0x80000000)) {
        nbr_lead_zeros += 1;
    }

    if (!val) {
        nbr_lead_zeros += 1;
    }

    return (nbr_lead_zeros);
}

