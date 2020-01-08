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

#include "tos_k.h"

#if TOS_CFG_MUTEX_EN > 0u

__STATIC_INLINE__ void mutex_old_owner_release(k_mutex_t *mutex)
{
    k_task_t *owner;

    owner = mutex->owner;

    tos_list_del(&mutex->owner_anchor);
    mutex->owner        = K_NULL;
    mutex->pend_nesting = (k_nesting_t)0u;

    // the right time comes! let's do it!
    if (owner->prio_pending != K_TASK_PRIO_INVALID) {
        tos_task_prio_change(owner, owner->prio_pending);
        owner->prio_pending = K_TASK_PRIO_INVALID;
    } else if (owner->prio != mutex->owner_orig_prio) {
        tos_task_prio_change(owner, mutex->owner_orig_prio);
        mutex->owner_orig_prio = K_TASK_PRIO_INVALID;
    }
}

__STATIC_INLINE__ void mutex_fresh_owner_mark(k_mutex_t *mutex, k_task_t *task)
{
    mutex->owner            = task;
    mutex->owner_orig_prio  = task->prio;
    mutex->pend_nesting     = (k_nesting_t)1u;

    tos_list_add(&mutex->owner_anchor, &task->mutex_own_list);
}

__STATIC_INLINE__ void mutex_new_owner_mark(k_mutex_t *mutex, k_task_t *task)
{
    k_prio_t highest_pending_prio;

    mutex_fresh_owner_mark(mutex, task);

    // we own the mutex now, make sure our priority is higher than any one in the pend list.
    highest_pending_prio = pend_highest_pending_prio_get(&mutex->pend_obj);
    if (task->prio > highest_pending_prio) {
        tos_task_prio_change(task, highest_pending_prio);
    }
}

__KERNEL__ void mutex_release(k_mutex_t *mutex)
{
    mutex_old_owner_release(mutex);
    pend_wakeup_all(&mutex->pend_obj, PEND_STATE_OWNER_DIE);
}

__API__ k_err_t tos_mutex_create(k_mutex_t *mutex)
{
    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(mutex);

    pend_object_init(&mutex->pend_obj);
    mutex->pend_nesting     = (k_nesting_t)0u;
    mutex->owner            = K_NULL;
    mutex->owner_orig_prio  = K_TASK_PRIO_INVALID;
    tos_list_init(&mutex->owner_anchor);

    TOS_OBJ_INIT(mutex, KNL_OBJ_TYPE_MUTEX);

    return K_ERR_NONE;
}

__API__ k_err_t tos_mutex_destroy(k_mutex_t *mutex)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(mutex);
    TOS_OBJ_VERIFY(mutex, KNL_OBJ_TYPE_MUTEX);

    TOS_CPU_INT_DISABLE();

    if (!pend_is_nopending(&mutex->pend_obj)) {
        pend_wakeup_all(&mutex->pend_obj, PEND_STATE_DESTROY);
    }

    if (mutex->owner) {
        mutex_old_owner_release(mutex);
    }

    pend_object_deinit(&mutex->pend_obj);

    TOS_OBJ_DEINIT(mutex);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_mutex_pend_timed(k_mutex_t *mutex, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(mutex);
    TOS_OBJ_VERIFY(mutex, KNL_OBJ_TYPE_MUTEX);

    TOS_CPU_INT_DISABLE();
    if (mutex->pend_nesting == (k_nesting_t)0u) { // first come
        mutex_fresh_owner_mark(mutex, k_curr_task);
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (knl_is_self(mutex->owner)) { // come again
        if (mutex->pend_nesting == (k_nesting_t)-1) {
            TOS_CPU_INT_ENABLE();
            return K_ERR_MUTEX_NESTING_OVERFLOW;
        }
        ++mutex->pend_nesting;
        TOS_CPU_INT_ENABLE();
        return K_ERR_MUTEX_NESTING;
    }

    if (timeout == TOS_TIME_NOWAIT) { // no wait, return immediately
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_NOWAIT;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    if (mutex->owner->prio > k_curr_task->prio) {
        // PRIORITY INVERSION:
        // we are declaring a mutex, which's owner has a lower(numerically bigger) priority.
        // make owner the same priority with us.
        tos_task_prio_change(mutex->owner, k_curr_task->prio);
    }

    pend_task_block(k_curr_task, &mutex->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return pend_state2errno(k_curr_task->pend_state);
}

__API__ k_err_t tos_mutex_pend(k_mutex_t *mutex)
{
    return tos_mutex_pend_timed(mutex, TOS_TIME_FOREVER);
}

__API__ k_err_t tos_mutex_post(k_mutex_t *mutex)
{
    TOS_CPU_CPSR_ALLOC();
    k_task_t *pending_task;

    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(mutex);
    TOS_OBJ_VERIFY(mutex, KNL_OBJ_TYPE_MUTEX);

    TOS_CPU_INT_DISABLE();
    if (!knl_is_self(mutex->owner)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_MUTEX_NOT_OWNER;
    }

    --mutex->pend_nesting;
    if (mutex->pend_nesting > (k_nesting_t)0u) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_MUTEX_NESTING;
    }

    mutex_old_owner_release(mutex);

    if (pend_is_nopending(&mutex->pend_obj)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    /* must do the mutex owner switch right here
       if the pender don't get a chance to schedule, the poster(old owner) may obtain the mutex immediately again
       but the pender already get ready(already in the critical section).
       we switch the owner right here to avoid the old owner obtain the mutex again
     */
    pending_task = pend_highest_pending_task_get(&mutex->pend_obj);
    mutex_new_owner_mark(mutex, pending_task);

    pend_wakeup_one(&mutex->pend_obj, PEND_STATE_POST);
    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

#endif

