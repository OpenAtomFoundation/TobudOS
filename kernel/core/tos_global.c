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

TOS_LIST_DEFINE(k_tick_list);

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
k_fault_log_writer_t    k_fault_log_writer = fault_default_log_writer;
#endif

#if TOS_CFG_MMHEAP_EN > 0u
uint8_t             k_mmheap_pool[TOS_CFG_MMHEAP_POOL_SIZE] __ALIGNED__(4);
k_mmheap_ctl_t      k_mmheap_ctl;
#endif

#if TOS_CFG_ROUND_ROBIN_EN > 0u
k_timeslice_t       k_robin_default_timeslice       = TOS_CFG_CPU_TICK_PER_SECOND / 10;
k_robin_state_t     k_robin_state                   = TOS_ROBIN_STATE_DISABLED;
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

#if TOS_CFG_MSG_EN > 0u
TOS_LIST_DEFINE(k_msg_freelist);
k_msg_t             k_msg_pool[TOS_CFG_MSG_POOL_SIZE];
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

