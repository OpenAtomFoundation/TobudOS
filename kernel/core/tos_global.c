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

#include <tos.h>

k_nesting_t         k_irq_nest_cnt              = (k_nesting_t)0;
k_nesting_t         k_sched_lock_nest_cnt       = (k_nesting_t)0;
knl_state_t         k_knl_state                 = KNL_STATE_STOPPED;

readyqueue_t        k_rdyq;

k_tick_t            k_tick_count                = (k_tick_t)0u;
k_task_t           *k_curr_task                 = K_NULL;
k_task_t           *k_next_task                 = K_NULL;

k_task_t            k_idle_task;
k_stack_t           k_idle_task_stk[TOS_CFG_IDLE_TASK_STK_SIZE];
k_stack_t          *const k_idle_task_stk_addr  = &k_idle_task_stk[0];
size_t              const k_idle_task_stk_size  = TOS_CFG_IDLE_TASK_STK_SIZE;

k_tick_t            k_cpu_tick_per_second       = TOS_CFG_CPU_TICK_PER_SECOND;

k_cycle_t           k_cpu_cycle_per_tick        = (k_cycle_t)0u;

#if TOS_CFG_TASK_DYNAMIC_CREATE_EN > 0u
TOS_LIST_DEFINE(k_dead_task_list);
#endif

TOS_LIST_DEFINE(k_stat_list);

TOS_LIST_DEFINE(k_tick_list);

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
k_fault_log_writer_t    k_fault_log_writer = fault_default_log_writer;
#endif

#if TOS_CFG_MMHEAP_EN > 0u
#if TOS_CFG_MMHEAP_DEFAULT_POOL_EN > 0u
uint8_t             k_mmheap_default_pool[TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE] __ALIGNED__(4);
#endif
k_mmheap_ctl_t      k_mmheap_ctl;
#endif

#if TOS_CFG_ROUND_ROBIN_EN > 0u
k_timeslice_t       k_robin_default_timeslice       = TOS_CFG_CPU_TICK_PER_SECOND / 10;
#endif

#if TOS_CFG_TIMER_EN > 0u
timer_ctl_t         k_timer_ctl = { TOS_TIME_FOREVER, TOS_LIST_NODE(k_timer_ctl.list) };

#if TOS_CFG_TIMER_AS_PROC == 0u
k_task_t            k_timer_task;
k_stack_t           k_timer_task_stk[TOS_CFG_TIMER_TASK_STK_SIZE];
k_prio_t            const k_timer_task_prio         = TOS_CFG_TIMER_TASK_PRIO;
k_stack_t          *const k_timer_task_stk_addr     = &k_timer_task_stk[0];
size_t              const k_timer_task_stk_size     = TOS_CFG_TIMER_TASK_STK_SIZE;
#endif /* TOS_CFG_TIMER_AS_PROC == 0u */

#endif

#if TOS_CFG_PWR_MGR_EN > 0u
pm_device_ctl_t     k_pm_device_ctl             = { 0u };

/* default idle power manager mode is SLEEP */
idle_pwrmgr_mode_t  k_idle_pwr_mgr_mode         = IDLE_POWER_MANAGER_MODE_SLEEP;

/* default low power mode is SLEEP */
k_cpu_lpwr_mode_t   k_cpu_lpwr_mode             = TOS_LOW_POWER_MODE_SLEEP;
#endif

#if TOS_CFG_TICKLESS_EN > 0u
k_tickless_wkup_alarm_t *k_tickless_wkup_alarm[__LOW_POWER_MODE_DUMMY] = { K_NULL };
#endif

