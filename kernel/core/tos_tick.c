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

__STATIC__ void tick_task_place(k_task_t *task, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();
    k_list_t *curr;
    k_task_t *curr_task = K_NULL;
    k_tick_t curr_expires, prev_expires = (k_tick_t)0u;

    TOS_CPU_INT_DISABLE();

    task->tick_expires = timeout;

    TOS_LIST_FOR_EACH(curr, &k_tick_list) {
        curr_task = TOS_LIST_ENTRY(curr, k_task_t, tick_list);
        curr_expires = prev_expires + curr_task->tick_expires;

        if (task->tick_expires < curr_expires) {
            break;
        }
        if (task->tick_expires == curr_expires &&
            task->prio < curr_task->prio) {
            break;
        }
        prev_expires = curr_expires;
    }
    task->tick_expires -= prev_expires;
    if (curr != &k_tick_list && curr_task) {
        curr_task->tick_expires -= task->tick_expires;
    }
    tos_list_add_tail(&task->tick_list, curr);

    TOS_CPU_INT_ENABLE();
}

__STATIC__ void tick_task_takeoff(k_task_t *task)
{
    TOS_CPU_CPSR_ALLOC();
    k_task_t *next;

    TOS_CPU_INT_DISABLE();

    next = TOS_LIST_FIRST_ENTRY_OR_NULL(&task->tick_list, k_task_t, tick_list);
    if (next && task->tick_list.next != &k_tick_list) { // not the only one
        if (next->tick_expires <= K_TIME_MAX - task->tick_expires) {
            next->tick_expires += task->tick_expires;
        } else {
            next->tick_expires = K_TIME_MAX;
        }
    }

    tos_list_del(&task->tick_list);

    TOS_CPU_INT_ENABLE();
}

__KERNEL__ void tick_list_add(k_task_t *task, k_tick_t timeout)
{
    tick_task_place(task, timeout);
    task_state_set_sleeping(task);
}

__KERNEL__ void tick_list_remove(k_task_t *task)
{
    tick_task_takeoff(task);
    task_state_reset_sleeping(task);
}

__KERNEL__ void tick_update(k_tick_t tick)
{
    TOS_CPU_CPSR_ALLOC();
    k_task_t *first, *task;
    k_list_t *curr, *next;

    TOS_CPU_INT_DISABLE();
    k_tick_count += tick;

    if (tos_list_empty(&k_tick_list)) {
        TOS_CPU_INT_ENABLE();
        return;
    }

    first = TOS_LIST_FIRST_ENTRY(&k_tick_list, k_task_t, tick_list);
    if (first->tick_expires <= tick) {
        first->tick_expires = (k_tick_t)0u;
    } else {
        first->tick_expires -= tick;
        TOS_CPU_INT_ENABLE();
        return;
    }

    TOS_LIST_FOR_EACH_SAFE(curr, next, &k_tick_list) {
        task = TOS_LIST_ENTRY(curr, k_task_t, tick_list);
        if (task->tick_expires > (k_tick_t)0u) {
            break;
        }

        // we are pending on something, but tick's up, no longer waitting
        pend_task_wakeup(task, PEND_STATE_TIMEOUT);
    }

    TOS_CPU_INT_ENABLE();
}

__KERNEL__ k_tick_t tick_next_expires_get(void)
{
    TOS_CPU_CPSR_ALLOC();
    k_tick_t next_expires;
    k_task_t *first;

    TOS_CPU_INT_DISABLE();

    first = TOS_LIST_FIRST_ENTRY_OR_NULL(&k_tick_list, k_task_t, tick_list);
    next_expires = first ? first->tick_expires : TOS_TIME_FOREVER;

    TOS_CPU_INT_ENABLE();
    return next_expires;
}

__API__ void tos_tick_handler(void)
{
    if (unlikely(!tos_knl_is_running())) {
        return;
    }

    tick_update((k_tick_t)1u);

#if TOS_CFG_TIMER_EN > 0u && TOS_CFG_TIMER_AS_PROC > 0u
    timer_update();
#endif

#if TOS_CFG_ROUND_ROBIN_EN > 0u
    robin_sched(k_curr_task->prio);
#endif
}

