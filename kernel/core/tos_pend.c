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

__STATIC__ void pend_list_add(k_task_t *task, pend_obj_t *pend_obj)
{
    k_task_t *iter;

    /* keep priority in descending order, the boss(task with highest priority,
       numerically smallest) always comes first
    */
    TOS_LIST_FOR_EACH_ENTRY(iter, k_task_t, pend_list, &pend_obj->list) {
        if (task->prio < iter->prio) {
            break;
        }
    }
    tos_list_add_tail(&task->pend_list, &iter->pend_list);

    // remember me, you may use me someday
    task->pending_obj = pend_obj;
    task_state_set_pend(task);
}

__KNL__ k_prio_t pend_highest_pending_prio_get(pend_obj_t *object)
{
    k_task_t *task;

    // we keep the task priority in descending order, so the first one is just fine.
    task = TOS_LIST_FIRST_ENTRY_OR_NULL(&object->list, k_task_t, pend_list);
    return task ? task->prio : K_TASK_PRIO_INVALID;
}

__KNL__ k_task_t *pend_highest_pending_task_get(pend_obj_t *object)
{
    return TOS_LIST_FIRST_ENTRY(&object->list, k_task_t, pend_list);
}

__KNL__ void pend_list_remove(k_task_t *task)
{
    tos_list_del(&task->pend_list);

    task->pending_obj = (pend_obj_t *)K_NULL;
    task_state_reset_pending(task);
}

__KNL__ void pend_object_init(pend_obj_t *object)
{
    tos_list_init(&object->list);
}

__KNL__ void pend_object_deinit(pend_obj_t *object)
{
    tos_list_init(&object->list);
}

__KNL__ int pend_is_nopending(pend_obj_t *object)
{
    return tos_list_empty(&object->list);
}

__KNL__ void pend_list_adjust(k_task_t *task)
{
    // we may be the boss, so re-enter the pend list
    tos_list_del(&task->pend_list);
    // the "someday" comes
    pend_list_add(task, task->pending_obj);
}

__KNL__ k_err_t pend_state2errno(pend_state_t state)
{
    if (state == PEND_STATE_POST) {
        return K_ERR_NONE;
    } else if (state == PEND_STATE_TIMEOUT) {
        return K_ERR_PEND_TIMEOUT;
    } else if (state == PEND_STATE_DESTROY) {
        return K_ERR_PEND_DESTROY;
    } else if (state == PEND_STATE_OWNER_DIE) {
        return K_ERR_PEND_OWNER_DIE;
    } else {
        return K_ERR_PEND_ABNORMAL;
    }
}

__KNL__ void pend_task_wakeup(k_task_t *task, pend_state_t state)
{
    if (task_state_is_pending(task)) {
        // mark why we wakeup
        task->pend_state = state;
        pend_list_remove(task);
    }

    if (task_state_is_sleeping(task)) {
        tick_list_remove(task);
    }

    if (task_state_is_suspended(task)) {
        return;
    }

    readyqueue_add(task);
}

__KNL__ void pend_task_block(k_task_t *task, pend_obj_t *object, k_tick_t timeout)
{
    readyqueue_remove(task);

    task->pend_state = PEND_STATE_NONE;
    pend_list_add(task, object);

    if (timeout != TOS_TIME_FOREVER) {
        tick_list_add(task, timeout);
    }
}

__KNL__ void pend_wakeup_one(pend_obj_t *object, pend_state_t state)
{
    pend_task_wakeup(TOS_LIST_FIRST_ENTRY(&object->list, k_task_t, pend_list), state);
}

__KNL__ void pend_wakeup_all(pend_obj_t *object, pend_state_t state)
{
    k_task_t *task, *tmp;

    TOS_LIST_FOR_EACH_ENTRY_SAFE(task, tmp, k_task_t, pend_list, &object->list) {
        pend_task_wakeup(task, state);
    }
}

__KNL__ void pend_wakeup(pend_obj_t *object, pend_state_t state, opt_post_t opt)
{
    if (opt == OPT_POST_ONE) {
        pend_wakeup_one(object, state);
    } else {
        pend_wakeup_all(object, state);
    }
}

