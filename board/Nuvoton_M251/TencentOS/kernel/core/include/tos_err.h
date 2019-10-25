#ifndef _TOS_ERR_H_
#define  _TOS_ERR_H_

typedef enum k_err_en {
    K_ERR_NONE                                  = 0u,

    K_ERR_DELAY_ZERO                            = 100u,
    K_ERR_DELAY_FOREVER,

    K_ERR_EVENT_PEND_OPT_INVALID                = 200u,

    K_ERR_FIFO_FULL                             = 300u,
    K_ERR_FIFO_EMPTY,

    K_ERR_IN_IRQ                                    = 400u,

    K_ERR_KNL_NOT_RUNNING                       = 500u,
    K_ERR_KNL_RUNNING,

    K_ERR_LOCK_NESTING_OVERFLOW                 = 600u,

    K_ERR_MMBLK_POOL_FULL                       = 700u,
    K_ERR_MMBLK_POOL_EMPTY,
    K_ERR_MMBLK_INVALID_BLK_SIZE,
    K_ERR_MMBLK_INVALID_POOL_ADDR,

    K_ERR_MMHEAP_INVALID_POOL_ADDR              = 800u,
    K_ERR_MMHEAP_INVALID_POOL_SIZE,

    K_ERR_MSG_QUEUE_FULL                        = 900u,
    K_ERR_MSG_QUEUE_EMPTY,

    K_ERR_MUTEX_NOT_OWNER                       = 1000u,
    K_ERR_MUTEX_NESTING,
    K_ERR_MUTEX_NESTING_OVERFLOW,

    K_ERR_OBJ_PTR_NULL                          = 1100u,
    K_ERR_OBJ_INVALID,

    K_ERR_PM_DEVICE_ALREADY_REG                 = 1200u,
    K_ERR_PM_DEVICE_OVERFLOW                    = 1300u,
    K_ERR_PM_WKUP_SOURCE_NOT_INSTALL            = 1400u,

    K_ERR_QUEUE_EMPTY                           = 1500u,
    K_ERR_QUEUE_FULL,

    K_ERR_PEND_NOWAIT                           = 1600u,
    K_ERR_PEND_SCHED_LOCKED,
    K_ERR_PEND_ABNORMAL,
    K_ERR_PEND_TIMEOUT,
    K_ERR_PEND_DESTROY,
    K_ERR_PEND_OWNER_DIE,

    K_ERR_SCHED_LOCKED                          = 1700u,
    K_ERR_SCHED_NOT_LOCKED,

    K_ERR_SEM_OVERFLOW                          = 1800u,

    K_ERR_TASK_DESTROY_IDLE                     = 1900u,
    K_ERR_TASK_NOT_DELAY,
    K_ERR_TASK_PRIO_INVALID,
    K_ERR_TASK_RESUME_SELF,
    K_ERR_TASK_SUSPENDED,
    K_ERR_TASK_SUSPEND_IDLE,
    K_ERR_TASK_STK_OVERFLOW,
    K_ERR_TASK_STK_SIZE_INVALID,

    K_ERR_TICKLESS_WKUP_ALARM_NOT_INSTALLED     = 2000u,
    K_ERR_TICKLESS_WKUP_ALARM_NO_INIT,
    K_ERR_TICKLESS_WKUP_ALARM_INIT_FAILED,

    K_ERR_TIMER_INACTIVE                        = 2100u,
    K_ERR_TIMER_DELAY_FOREVER,
    K_ERR_TIMER_PERIOD_FOREVER,
    K_ERR_TIMER_INVALID_DELAY,
    K_ERR_TIMER_INVALID_PERIOD,
    K_ERR_TIMER_INVALID_STATE,
    K_ERR_TIMER_INVALID_OPT,
    K_ERR_TIMER_STOPPED,
} k_err_t;

#endif /* _TOS_ERR_H_ */

