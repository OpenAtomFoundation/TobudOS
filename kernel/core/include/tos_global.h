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

#ifndef _TOS_GLOBAL_H_
#define  _TOS_GLOBAL_H_

/* interrupt nesting count                      */
extern k_nesting_t          k_irq_nest_cnt;

/* schedule lock nesting count                  */
extern k_nesting_t          k_sched_lock_nest_cnt;

/* kernel running state                         */
extern knl_state_t          k_knl_state;

/* ready queue of tasks                         */
extern readyqueue_t         k_rdyq;

/* ticks since boot up                          */
extern k_tick_t             k_tick_count;

/* current task                                 */
extern k_task_t            *k_curr_task;
/* next task to run                             */
extern k_task_t            *k_next_task;

/* idle task related stuff                      */
extern k_task_t             k_idle_task;
extern k_stack_t            k_idle_task_stk[];
extern k_stack_t           *const k_idle_task_stk_addr;
extern size_t               const k_idle_task_stk_size;

#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
/* list to hold all the destroyed dynamic created tasks */
extern k_list_t             k_dead_task_list;
#endif

/* list to hold all the tasks for statistics */
extern k_list_t             k_stat_list;

/* list to hold all the tasks delayed or pend for timeout */
extern k_list_t             k_tick_list;

/* how many ticks will be triggered in a second */
extern k_tick_t             k_cpu_tick_per_second;

/* how many cycle per tick                      */
extern k_cycle_t            k_cpu_cycle_per_tick;

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u

extern k_fault_log_writer_t k_fault_log_writer;

#endif

#if TOS_CFG_MMHEAP_EN > 0u
#if TOS_CFG_MMHEAP_DEFAULT_POOL_EN > 0u
extern uint8_t              k_mmheap_default_pool[] __ALIGNED__(4);
#endif
extern k_mmheap_ctl_t       k_mmheap_ctl;
#endif

#if TOS_CFG_ROUND_ROBIN_EN > 0u
extern k_timeslice_t        k_robin_default_timeslice;
#endif

#if TOS_CFG_TIMER_EN > 0u
/* list holding all the timer */
extern timer_ctl_t          k_timer_ctl;
#if TOS_CFG_TIMER_AS_PROC == 0u
extern k_task_t             k_timer_task;
extern k_stack_t            k_timer_task_stk[];
extern k_prio_t             const k_timer_task_prio;
extern k_stack_t           *const k_timer_task_stk_addr;
extern size_t               const k_timer_task_stk_size;
#endif
#endif

#if TOS_CFG_PWR_MGR_EN > 0u
extern pm_device_ctl_t      k_pm_device_ctl;

extern idle_pwrmgr_mode_t   k_idle_pwr_mgr_mode;

extern k_cpu_lpwr_mode_t    k_cpu_lpwr_mode;
#endif

#if TOS_CFG_TICKLESS_EN > 0u
extern k_tickless_wkup_alarm_t  *k_tickless_wkup_alarm[__LOW_POWER_MODE_DUMMY];
#endif

#endif /* _TOS_GLOBAL_H_ */

