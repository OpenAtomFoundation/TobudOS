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

#if TOS_CFG_COMPLETION_EN > 0u

__API__ k_err_t tos_completion_create(k_completion_t *completion)
{
    TOS_PTR_SANITY_CHECK(completion);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&completion->knl_obj, KNL_OBJ_TYPE_COMPLETION);
#endif

    pend_object_init(&completion->pend_obj);
    completion->done = (completion_done_t)0u;

    return K_ERR_NONE;
}

__API__ k_err_t tos_completion_destroy(k_completion_t *completion)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(completion);
    TOS_OBJ_VERIFY(completion, KNL_OBJ_TYPE_COMPLETION);

    TOS_CPU_INT_DISABLE();

    if (!pend_is_nopending(&completion->pend_obj)) {
        pend_wakeup_all(&completion->pend_obj, PEND_STATE_DESTROY);
    }

    pend_object_deinit(&completion->pend_obj);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&completion->knl_obj);
#endif

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_completion_pend_timed(k_completion_t *completion, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(completion);
    TOS_OBJ_VERIFY(completion, KNL_OBJ_TYPE_COMPLETION);

    TOS_CPU_INT_DISABLE();

    if (completion->done > (completion_done_t)0u) {
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

    pend_task_block(k_curr_task, &completion->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return pend_state2errno(k_curr_task->pend_state);
}

__API__ k_err_t tos_completion_pend(k_completion_t *completion)
{
    return tos_completion_pend_timed(completion, TOS_TIME_FOREVER);
}

__STATIC__ k_err_t completion_do_post(k_completion_t *completion, opt_post_t opt)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(completion);
    TOS_OBJ_VERIFY(completion, KNL_OBJ_TYPE_COMPLETION);

    TOS_CPU_INT_DISABLE();

    if (completion->done == (completion_done_t)-1) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_COMPLETION_OVERFLOW;
    }

    ++completion->done;

    if (pend_is_nopending(&completion->pend_obj)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    pend_wakeup(&completion->pend_obj, PEND_STATE_POST, opt);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_completion_post(k_completion_t *completion)
{
    return completion_do_post(completion, OPT_POST_ONE);
}

__API__ k_err_t tos_completion_post_all(k_completion_t *completion)
{
    return completion_do_post(completion, OPT_POST_ALL);
}

__API__ k_err_t tos_completion_reset(k_completion_t *completion)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(completion);
    TOS_OBJ_VERIFY(completion, KNL_OBJ_TYPE_COMPLETION);

    TOS_CPU_INT_DISABLE();
    completion->done = (completion_done_t)0u;
    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

__API__ int tos_completion_is_done(k_completion_t *completion)
{
    TOS_CPU_CPSR_ALLOC();
    int is_done = K_FALSE;

    TOS_PTR_SANITY_CHECK_RC(completion, K_FALSE);
    TOS_OBJ_VERIFY_RC(completion, KNL_OBJ_TYPE_COMPLETION, K_FALSE);

    TOS_CPU_INT_DISABLE();
    is_done = (completion->done > (completion_done_t)0u ? K_TRUE : K_FALSE);
    TOS_CPU_INT_ENABLE();

    return is_done;
}

#endif

