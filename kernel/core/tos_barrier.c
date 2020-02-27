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

__API__ k_err_t tos_barrier_create(k_barrier_t *barrier, k_barrier_cnt_t count)
{
    TOS_PTR_SANITY_CHECK(barrier);

    if (count == 0u) {
        return K_ERR_BARRIER_COUNT_INVALID;
    }

    barrier->count = count;
    pend_object_init(&barrier->pend_obj);
    TOS_OBJ_INIT(barrier, KNL_OBJ_TYPE_BARRIER);

    return K_ERR_NONE;
}

__API__ k_err_t tos_barrier_destroy(k_barrier_t *barrier)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(barrier);
    TOS_OBJ_VERIFY(barrier, KNL_OBJ_TYPE_BARRIER);

    TOS_CPU_INT_DISABLE();

    if (!pend_is_nopending(&barrier->pend_obj)) {
        pend_wakeup_all(&barrier->pend_obj, PEND_STATE_DESTROY);
    }

    pend_object_deinit(&barrier->pend_obj);

    TOS_OBJ_DEINIT(barrier);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_barrier_pend(k_barrier_t *barrier)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(barrier);
    TOS_OBJ_VERIFY(barrier, KNL_OBJ_TYPE_BARRIER);

    TOS_CPU_INT_DISABLE();

    if (barrier->count == 0u) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_BARRIER_OVERFLOW;
    }

    if (barrier->count == (k_barrier_cnt_t)1u) {
        barrier->count = (k_barrier_cnt_t)0u;

        if (!pend_is_nopending(&barrier->pend_obj)) {
            pend_wakeup_all(&barrier->pend_obj, PEND_STATE_POST);
        }

        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    --barrier->count;
    pend_task_block(k_curr_task, &barrier->pend_obj, TOS_TIME_FOREVER);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return pend_state2errno(k_curr_task->pend_state);
}

__API__ k_err_t tos_barrier_reset(k_barrier_t *barrier, k_barrier_cnt_t count)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(barrier);
    TOS_OBJ_VERIFY(barrier, KNL_OBJ_TYPE_BARRIER);

    TOS_CPU_INT_DISABLE();
    barrier->count = count;
    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

