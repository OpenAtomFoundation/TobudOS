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

/* list to hold all the task delayed or pend for timeout */
extern k_list_t             k_tick_list;

/* how many ticks will be triggered in a second */
extern k_tick_t             k_cpu_tick_per_second;

/* how many cycle per tick                      */
extern k_cycle_t            k_cpu_cycle_per_tick;

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u

extern k_fault_log_writer_t k_fault_log_writer;

#endif

#if TOS_CFG_MMHEAP_EN > 0u
extern uint8_t              k_mmheap_pool[] __ALIGNED__(4);
extern k_mmheap_ctl_t       k_mmheap_ctl;
#endif

#if TOS_CFG_ROUND_ROBIN_EN > 0u
extern k_timeslice_t        k_robin_default_timeslice;
extern k_robin_state_t      k_robin_state;
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

#if (TOS_CFG_MSG_EN > 0u)
extern k_list_t             k_msg_freelist;
extern k_msg_t              k_msg_pool[];
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

