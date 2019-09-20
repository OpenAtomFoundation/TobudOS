#include <tos.h>
#include <riscv_encoding.h>


__KERNEL__ void cpu_systick_init(k_cycle_t cycle_per_tick)
{
    port_systick_priority_set(TOS_CFG_CPU_SYSTICK_PRIO);
    port_systick_config(cycle_per_tick);
}

__KERNEL__ void cpu_init(void) {
    k_cpu_cycle_per_tick = TOS_CFG_CPU_CLOCK / k_cpu_tick_per_second;
    cpu_systick_init(k_cpu_cycle_per_tick);
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
    port_irq_context_switch();
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
00    000    epc

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
    sp = (cpu_data_t *)((cpu_addr_t)(sp) & 0xFFFFFFF8);

    sp  -= (sizeof(cpu_context_t)/sizeof(cpu_data_t));

    regs = (cpu_context_t*) sp;

#if 1
    for(int i=0; i<(sizeof(cpu_context_t)/sizeof(cpu_data_t)); i++) {
        #define _V(v) ((unsigned int)((v/10) << 4 | (v % 10)))
        *(sp + i) = (_V(i) << 24) | (_V(i) << 16) | (_V(i) << 8) | _V(i);
        #undef _V
    }
#endif

    regs->a0        = (cpu_data_t)arg;                          // a0: argument
    regs->ra        = (cpu_data_t)0xACE00ACE;                   // ra: return address
    regs->mstatus   = (cpu_data_t)(MSTATUS_MPP | MSTATUS_MPIE); // return to machine mode and enable interrupt
    regs->epc       = (cpu_data_t)entry;


    return (k_stack_t*)sp;
}

void cpu_trap_entry(cpu_data_t cause, cpu_context_t *regs)
{
    while(1) {
        // TODO
    }
}

void SysTick_IRQHandler() {
    port_systick_config(k_cpu_cycle_per_tick);
    if(tos_knl_is_running()) {
        tos_knl_irq_enter();
        tos_tick_handler();
        tos_knl_irq_leave();
    }
}

void cpu_irq_entry(cpu_data_t irq, cpu_context_t *regs)
{
#if 1
    if(irq != 7) {
        return;
    }

    SysTick_IRQHandler();
#else
    void (*irq_handler)();
    extern void (*handler_vector_table[])();

    irq_handler = handler_vector_table[irq];
    if((*irq_handler) == 0) {
        return;
    }

    (*irq_handler)();
#endif
}

__API__ uint32_t tos_cpu_clz(uint32_t val)
{
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
}
