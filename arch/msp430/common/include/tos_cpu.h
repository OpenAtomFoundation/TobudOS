#ifndef _TOS_CPU_H_
#define  _TOS_CPU_H_

typedef struct cpu_context_st {
    cpu_data_t      _R0; //PC
    //cpu_data_t      _R1; //SP
    cpu_data_t      _R2; //SR
    //cpu_data_t      _R3; 
    cpu_data_t      _R4;
    cpu_data_t      _R5;
    cpu_data_t      _R6;
    cpu_data_t      _R7;
    cpu_data_t      _R8;
    cpu_data_t      _R9;
    cpu_data_t      _R10;
    cpu_data_t      _R11;
    cpu_data_t      _R12;
    cpu_data_t      _R13;
    cpu_data_t      _R14;
    cpu_data_t      _R15;
} cpu_context_t;

__API__ uint32_t        tos_cpu_clz(uint32_t val);

__API__ void            tos_cpu_int_disable(void);

__API__ void            tos_cpu_int_enable(void);

__API__ cpu_cpsr_t      tos_cpu_cpsr_save(void);

__API__ void            tos_cpu_cpsr_restore(cpu_cpsr_t cpsr);

#if (TOS_CFG_CPU_HRTIMER_EN > 0u)

__API__ void            tos_cpu_hrtimer_init(void);

__API__ cpu_hrtimer_t   tos_cpu_hrtimer_read(void);

#endif

__KERNEL__ void         cpu_init(void);

__KERNEL__ void         cpu_reset(void);

__KERNEL__ void         cpu_systick_init(k_cycle_t cycle_per_tick);

__KERNEL__ void         cpu_sched_start(void);

__KERNEL__ void         cpu_context_switch(void);

__KERNEL__ void         cpu_irq_context_switch(void);

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u

__KERNEL__ k_err_t cpu_task_stack_draught_depth(k_stack_t *stk_base, size_t stk_size, int *depth);

#endif

__KERNEL__ k_stack_t   *cpu_task_stk_init(void *entry,
                                                          void *arg,
                                                          void *exit,
                                                          k_stack_t *stk_base,
                                                          size_t stk_size);

#if TOS_CFG_TICKLESS_EN > 0u

__KERNEL__ void         cpu_systick_resume(void);

__KERNEL__ void         cpu_systick_suspend(void);

__KERNEL__ void         cpu_systick_reload_reset(void);

__KERNEL__ void         cpu_systick_pending_reset(void);

__KERNEL__ k_time_t     cpu_systick_max_delay_millisecond(void);

__KERNEL__ void         cpu_systick_expires_set(k_time_t millisecond);

__KERNEL__ void         cpu_systick_reset(void);

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__KERNEL__ void cpu_sleep_mode_enter(void);

__KERNEL__ void cpu_stop_mode_enter(void);

__KERNEL__ void cpu_standby_mode_enter(void);

#endif

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u

#if defined (TOS_CFG_CPU_ARM_FPU_EN) && (TOS_CFG_CPU_ARM_FPU_EN == 1U)

__KERNEL__ void         cpu_flush_fpu(void);

#endif /* TOS_CFG_CPU_ARM_FPU_EN */

__KERNEL__ void         cpu_fault_diagnosis(void);

#endif

/* Allocates CPU status register word. */
#define TOS_CPU_CPSR_ALLOC()    cpu_cpsr_t cpu_cpsr = (cpu_cpsr_t)0u

/* Save CPU status word & disable interrupts.*/
#define TOS_CPU_INT_DISABLE() \
    do { \
        cpu_cpsr = tos_cpu_cpsr_save(); \
    } while (0)

/* Restore CPU status word.                     */
#define  TOS_CPU_INT_ENABLE() \
    do { \
        tos_cpu_cpsr_restore(cpu_cpsr); \
    } while (0)

#endif /* _TOS_CPU_H_ */

