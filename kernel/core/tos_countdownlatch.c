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

#if TOS_CFG_COUNTDOWNLATCH_EN > 0u

__API__ k_err_t tos_countdownlatch_create(k_countdownlatch_t *countdownlatch, k_countdownlatch_cnt_t count)
{
    TOS_PTR_SANITY_CHECK(countdownlatch);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&countdownlatch->knl_obj, KNL_OBJ_TYPE_COUNTDOWNLATCH);
#endif

    pend_object_init(&countdownlatch->pend_obj);
    countdownlatch->count = count;

    return K_ERR_NONE;
}

__API__ k_err_t tos_countdownlatch_destroy(k_countdownlatch_t *countdownlatch)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(countdownlatch);
    TOS_OBJ_VERIFY(countdownlatch, KNL_OBJ_TYPE_COUNTDOWNLATCH);

    TOS_CPU_INT_DISABLE();

    if (!pend_is_nopending(&countdownlatch->pend_obj)) {
        pend_wakeup_all(&countdownlatch->pend_obj, PEND_STATE_DESTROY);
    }

    pend_object_deinit(&countdownlatch->pend_obj);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&countdownlatch->knl_obj);
#endif

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_countdownlatch_pend_timed(k_countdownlatch_t *countdownlatch, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(countdownlatch);
    TOS_OBJ_VERIFY(countdownlatch, KNL_OBJ_TYPE_COUNTDOWNLATCH);

    TOS_CPU_INT_DISABLE();

    if (countdownlatch->count == (k_countdownlatch_cnt_t)0u) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (timeout == TOS_TIME_NOWAIT) { // no wait, return immediately
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_NOWAIT;
    }

    if (knl_is_inirq()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_IN_IRQ;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    pend_task_block(k_curr_task, &countdownlatch->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return pend_state2errno(k_curr_task->pend_state);
}

__API__ k_err_t tos_countdownlatch_pend(k_countdownlatch_t *countdownlatch)
{
    return tos_countdownlatch_pend_timed(countdownlatch, TOS_TIME_FOREVER);
}

__API__ k_err_t tos_countdownlatch_post(k_countdownlatch_t *countdownlatch)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(countdownlatch);
    TOS_OBJ_VERIFY(countdownlatch, KNL_OBJ_TYPE_COUNTDOWNLATCH);

    TOS_CPU_INT_DISABLE();

    if (countdownlatch->count == (k_countdownlatch_cnt_t)0) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_COUNTDOWNLATCH_OVERFLOW;
    }

    --countdownlatch->count;

    if (countdownlatch->count > (k_countdownlatch_cnt_t)0) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    pend_wakeup_one(&countdownlatch->pend_obj, PEND_STATE_POST);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_countdownlatch_reset(k_countdownlatch_t *countdownlatch, k_countdownlatch_cnt_t count)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(countdownlatch);
    TOS_OBJ_VERIFY(countdownlatch, KNL_OBJ_TYPE_COUNTDOWNLATCH);

    TOS_CPU_INT_DISABLE();
    countdownlatch->count = count;
    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

#endif

