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

#include "tos.h"

#if TOS_CFG_ROUND_ROBIN_EN > 0u

__API__ void tos_robin_default_timeslice_config(k_timeslice_t default_timeslice)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_CPU_INT_DISABLE();

    if (default_timeslice > (k_timeslice_t)0u) {
        k_robin_default_timeslice = default_timeslice;
    } else {
        k_robin_default_timeslice = TOS_CFG_CPU_TICK_PER_SECOND / 10;
    }

    TOS_CPU_INT_ENABLE();
}

__API__ void tos_robin_timeslice_set(k_task_t *task, k_timeslice_t timeslice)
{
    TOS_CPU_CPSR_ALLOC();

    if (!task) {
        task = k_curr_task;
    }

    TOS_CPU_INT_DISABLE();

    if (timeslice == (k_timeslice_t)0u) {
        task->timeslice_reload = k_robin_default_timeslice;
    } else {
        task->timeslice_reload = timeslice;
    }

    if (task->timeslice_reload > task->timeslice) {
        task->timeslice = task->timeslice_reload;
    }
    TOS_CPU_INT_ENABLE();
}

__KERNEL__ void robin_sched(k_prio_t prio)
{
    TOS_CPU_CPSR_ALLOC();
    k_task_t *task;

    TOS_CPU_INT_DISABLE();

    task = readyqueue_first_task_get(prio);
    if (!task || knl_is_idle(task)) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    if (readyqueue_is_prio_onlyone(prio)) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    if (task->timeslice > (k_timeslice_t)0u) {
        --task->timeslice;
    }

    if (task->timeslice > (k_timeslice_t)0u) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    readyqueue_move_head_to_tail(k_curr_task->prio);

    task = readyqueue_first_task_get(prio);
    if (task->timeslice_reload == (k_timeslice_t)0u) {
        task->timeslice = k_robin_default_timeslice;
    } else {
        task->timeslice = task->timeslice_reload;
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();
}

#endif

