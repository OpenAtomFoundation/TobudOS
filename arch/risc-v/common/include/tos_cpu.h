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

#ifndef _TOS_CPU_H_
#define _TOS_CPU_H_

typedef struct cpu_context_st {
    cpu_data_t          mepc;
    cpu_data_t          mstatus;
    union { cpu_data_t  x1,  ra; };
    union { cpu_data_t  x3,  gp; };
    union { cpu_data_t  x4,  tp; };
    union { cpu_data_t  x5,  t0; };
    union { cpu_data_t  x6,  t1; };
    union { cpu_data_t  x7,  t2; };
    union { cpu_data_t  x8,  s0, fp; };
    union { cpu_data_t  x9,  s1; };
    union { cpu_data_t x10,  a0; };
    union { cpu_data_t x11,  a1; };
    union { cpu_data_t x12,  a2; };
    union { cpu_data_t x13,  a3; };
    union { cpu_data_t x14,  a4; };
    union { cpu_data_t x15,  a5; };
    union { cpu_data_t x16,  a6; };
    union { cpu_data_t x17,  a7; };
    union { cpu_data_t x18,  s2; };
    union { cpu_data_t x19,  s3; };
    union { cpu_data_t x20,  s4; };
    union { cpu_data_t x21,  s5; };
    union { cpu_data_t x22,  s6; };
    union { cpu_data_t x23,  s7; };
    union { cpu_data_t x24,  s8; };
    union { cpu_data_t x25,  s9; };
    union { cpu_data_t x26, s10; };
    union { cpu_data_t x27, s11; };
    union { cpu_data_t x28,  t3; };
    union { cpu_data_t x29,  t4; };
    union { cpu_data_t x30,  t5; };
    union { cpu_data_t x31,  t6; };
} cpu_context_t;


__API__ uint32_t        tos_cpu_clz(uint32_t val);

__API__ void            tos_cpu_int_disable(void);

__API__ void            tos_cpu_int_enable(void);

__API__ cpu_cpsr_t      tos_cpu_cpsr_save(void);

__API__ void            tos_cpu_cpsr_restore(cpu_cpsr_t cpsr);


__KNL__ void            cpu_init(void);

__KNL__ void            cpu_reset(void);

__KNL__ void            cpu_systick_init(k_cycle_t cycle_per_tick);

__KNL__ void            cpu_sched_start(void);

__KNL__ void            cpu_context_switch(void);

__KNL__ void            cpu_irq_context_switch(void);

__KNL__ k_stack_t      *cpu_task_stk_init(void *entry,
                                                          void *arg,
                                                          void *exit,
                                                          k_stack_t *stk_base,
                                                          size_t stk_size);

#if TOS_CFG_TICKLESS_EN > 0u

__KNL__ void            cpu_systick_resume(void);

__KNL__ void            cpu_systick_suspend(void);

__KNL__ void            cpu_systick_reload_reset(void);

__KNL__ void            cpu_systick_pending_reset(void);

__KNL__ k_time_t        cpu_systick_max_delay_millisecond(void);

__KNL__ void            cpu_systick_expires_set(k_time_t millisecond);

__KNL__ void            cpu_systick_reset(void);

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__KNL__ void            cpu_sleep_mode_enter(void);

__KNL__ void            cpu_stop_mode_enter(void);

__KNL__ void            cpu_standby_mode_enter(void);

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
