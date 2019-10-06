#ifndef _TOS_CPU_H_
#define _TOS_CPU_H_

typedef struct cpu_context_st {
    union { cpu_data_t  s0, x8,  fp; };
    union { cpu_data_t  s1, x9;  };
    union { cpu_data_t  s2, x18; };
    union { cpu_data_t  s3, x19; };
    union { cpu_data_t  s4, x20; };
    union { cpu_data_t  s5, x21; };
    union { cpu_data_t  s6, x22; };
    union { cpu_data_t  s7, x23; };
    union { cpu_data_t  s8, x24; };
    union { cpu_data_t  s9, x25; };
    union { cpu_data_t  s10,x26; };
    union { cpu_data_t  s11,x27; };

    // caller save
    union { cpu_data_t  ra, x1;  };

    union { cpu_data_t  gp, x3;  };
    union { cpu_data_t  tp, x4;  };

    union { cpu_data_t  t0, x5;  };
    union { cpu_data_t  t1, x6;  };
    union { cpu_data_t  t2, x7;  };
    union { cpu_data_t  t3, x28; };
    union { cpu_data_t  t4, x29; };
    union { cpu_data_t  t5, x30; };
    union { cpu_data_t  t6, x31; };

    union { cpu_data_t  a0, x10; };
    union { cpu_data_t  a1, x11; };

    union { cpu_data_t  a2, x12; };
    union { cpu_data_t  a3, x13; };
    union { cpu_data_t  a4, x14; };
    union { cpu_data_t  a5, x15; };
    union { cpu_data_t  a6, x16; };
    union { cpu_data_t  a7, x17; };

    cpu_data_t          epc;
    cpu_data_t          mstatus;
} cpu_context_t;

__API__ uint32_t        tos_cpu_clz(uint32_t val);

__API__ void            tos_cpu_int_disable(void);

__API__ void            tos_cpu_int_enable(void);

__API__ cpu_cpsr_t      tos_cpu_cpsr_save(void);

__API__ void            tos_cpu_cpsr_restore(cpu_cpsr_t cpsr);


__KERNEL__ void         cpu_init(void);

__KERNEL__ void         cpu_reset(void);

__KERNEL__ void         cpu_systick_init(k_cycle_t cycle_per_tick);

__KERNEL__ void         cpu_sched_start(void);

__KERNEL__ void         cpu_context_switch(void);

__KERNEL__ void         cpu_irq_context_switch(void);

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

#error "unsupport now"

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
