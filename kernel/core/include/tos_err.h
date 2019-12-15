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

#ifndef _TOS_ERR_H_
#define  _TOS_ERR_H_

typedef enum k_err_en {
    K_ERR_NONE                                  = 0u,

    K_ERR_BIN_HEAP_FULL                     = 10u,
    K_ERR_BIN_HEAP_EMPTY,
    K_ERR_BIN_HEAP_ITEM_SIZE_NOT_MATCH,

    K_ERR_COMPLETION_OVERFLOW               = 25u,

    K_ERR_COUNTDOWNLATCH_OVERFLOW           = 50u,

    K_ERR_DELAY_ZERO                            = 100u,
    K_ERR_DELAY_FOREVER,

    K_ERR_EVENT_PEND_OPT_INVALID                = 200u,

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
    K_ERR_MMHEAP_POOL_OVERFLOW,
    K_ERR_MMHEAP_POOL_ALREADY_EXIST,
    K_ERR_MMHEAP_POOL_NOT_EXIST,

    K_ERR_MUTEX_NOT_OWNER                       = 1000u,
    K_ERR_MUTEX_NESTING,
    K_ERR_MUTEX_NESTING_OVERFLOW,

    K_ERR_OBJ_PTR_NULL                          = 1100u,
    K_ERR_OBJ_INVALID,
    K_ERR_OBJ_INVALID_ALLOC_TYPE,

    K_ERR_OUT_OF_MEMORY                         = 1150u,

    K_ERR_PEND_NOWAIT                           = 1200u,
    K_ERR_PEND_SCHED_LOCKED,
    K_ERR_PEND_IN_IRQ,
    K_ERR_PEND_ABNORMAL,
    K_ERR_PEND_TIMEOUT,
    K_ERR_PEND_DESTROY,
    K_ERR_PEND_OWNER_DIE,

    K_ERR_PM_DEVICE_ALREADY_REG                 = 1300u,
    K_ERR_PM_DEVICE_OVERFLOW,
    K_ERR_PM_WKUP_SOURCE_NOT_INSTALL,

    K_ERR_PRIO_Q_EMPTY                      = 1400u,
    K_ERR_PRIO_Q_FULL,
    K_ERR_PRIO_Q_SLOT_NOT_TAKEN,
    K_ERR_PRIO_Q_ITEM_SIZE_NOT_MATCH,

    K_ERR_RING_Q_FULL                           = 1600u,
    K_ERR_RING_Q_EMPTY,
    K_ERR_RING_Q_ITEM_SIZE_NOT_MATCH,

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
    K_ERR_TASK_OUT_OF_MEMORY,

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
    K_ERR_TIMER_RUNNING,
} k_err_t;

#endif /* _TOS_ERR_H_ */

