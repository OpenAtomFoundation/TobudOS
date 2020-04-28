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
#define  _TOS_CPU_H_

typedef struct cpu_context_st {
    cpu_data_t      R31;
    cpu_data_t      R30;
    cpu_data_t      R29;
    cpu_data_t      R28;
    cpu_data_t      R27;
    cpu_data_t      R26;
    cpu_data_t      R25;
    cpu_data_t      R24;
    cpu_data_t      R23;
    cpu_data_t      R22;
    cpu_data_t      R21;
    cpu_data_t      R20;
    cpu_data_t      R19;
    cpu_data_t      R18;
    cpu_data_t      R17;
    cpu_data_t      R16;
    cpu_data_t      R15;
    cpu_data_t      R14;
    cpu_data_t      R13;
    cpu_data_t      R12;
    cpu_data_t      R11;
    cpu_data_t      R10;
    cpu_data_t      R9;
    cpu_data_t      R8;
    cpu_data_t      R7;
    cpu_data_t      R6;
    cpu_data_t      R5;
    cpu_data_t      R4;
    cpu_data_t      R3;
    cpu_data_t      R2;
    cpu_data_t      R1;

    cpu_data_t      SREG;
    cpu_data_t      R0;

    cpu_data_t      PCH;
    cpu_data_t      PCL;
    cpu_data_t      LRH;
    cpu_data_t      LRL;
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

__KNL__ void            cpu_init(void);

__KNL__ void            cpu_reset(void);

__KNL__ void            cpu_systick_init(k_cycle_t cycle_per_tick);

__KNL__ void            cpu_sched_start(void);

__KNL__ void            cpu_context_switch(void);

__KNL__ void            cpu_irq_context_switch(void);

#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u

__KNL__ k_err_t         cpu_task_stack_draught_depth(k_stack_t *stk_base, size_t stk_size, int *depth);

#endif

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

__KNL__ void            cpu_fault_diagnosis(void);

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

