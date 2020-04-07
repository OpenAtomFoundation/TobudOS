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

#if TOS_CFG_MESSAGE_QUEUE_EN > 0u

__API__ k_err_t tos_msg_q_create(k_msg_q_t *msg_q, void *pool, size_t msg_cnt)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(msg_q);
    TOS_PTR_SANITY_CHECK(pool);

    err = tos_ring_q_create(&msg_q->ring_q, pool, msg_cnt, sizeof(void *));
    if (err != K_ERR_NONE) {
        return err;
    }

    pend_object_init(&msg_q->pend_obj);

    TOS_OBJ_INIT(msg_q, KNL_OBJ_TYPE_MESSAGE_QUEUE);
#if TOS_CFG_MMHEAP_EN > 0u
    knl_object_alloc_set_static(&msg_q->knl_obj);
#endif

    return K_ERR_NONE;
}

__API__ k_err_t tos_msg_q_destroy(k_msg_q_t *msg_q)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(msg_q);
    TOS_OBJ_VERIFY(msg_q, KNL_OBJ_TYPE_MESSAGE_QUEUE);

#if TOS_CFG_MMHEAP_EN > 0u
    if (!knl_object_alloc_is_static(&msg_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }
#endif

    TOS_CPU_INT_DISABLE();

    err = tos_ring_q_destroy(&msg_q->ring_q);
    if (err != K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return err;
    }

    pend_wakeup_all(&msg_q->pend_obj, PEND_STATE_DESTROY);

    pend_object_deinit(&msg_q->pend_obj);

    TOS_OBJ_DEINIT(msg_q);
#if TOS_CFG_MMHEAP_EN > 0u
    knl_object_alloc_reset(&msg_q->knl_obj);
#endif

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

#if TOS_CFG_MMHEAP_EN > 0u

__API__ k_err_t tos_msg_q_create_dyn(k_msg_q_t *msg_q, size_t msg_cnt)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(msg_q);

    err = tos_ring_q_create_dyn(&msg_q->ring_q, msg_cnt, sizeof(void *));
    if (err != K_ERR_NONE) {
        return err;
    }

    pend_object_init(&msg_q->pend_obj);

    TOS_OBJ_INIT(msg_q, KNL_OBJ_TYPE_MESSAGE_QUEUE);
    knl_object_alloc_set_dynamic(&msg_q->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_msg_q_destroy_dyn(k_msg_q_t *msg_q)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(msg_q);
    TOS_OBJ_VERIFY(msg_q, KNL_OBJ_TYPE_MESSAGE_QUEUE);

    if (!knl_object_alloc_is_dynamic(&msg_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }

    TOS_CPU_INT_DISABLE();

    err = tos_ring_q_destroy_dyn(&msg_q->ring_q);
    if (err != K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return err;
    }

    pend_wakeup_all(&msg_q->pend_obj, PEND_STATE_DESTROY);

    pend_object_deinit(&msg_q->pend_obj);

    TOS_OBJ_DEINIT(msg_q);
    knl_object_alloc_reset(&msg_q->knl_obj);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

#endif

__API__ k_err_t tos_msg_q_flush(k_msg_q_t *msg_q)
{
    TOS_PTR_SANITY_CHECK(msg_q);
    TOS_OBJ_VERIFY(msg_q, KNL_OBJ_TYPE_MESSAGE_QUEUE);

    return tos_ring_q_flush(&msg_q->ring_q);
}

__API__ k_err_t tos_msg_q_pend(k_msg_q_t *msg_q, void **msg_ptr, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(msg_q);
    TOS_PTR_SANITY_CHECK(msg_ptr);
    TOS_OBJ_VERIFY(msg_q, KNL_OBJ_TYPE_MESSAGE_QUEUE);

    TOS_CPU_INT_DISABLE();

    if (tos_ring_q_dequeue(&msg_q->ring_q, msg_ptr, K_NULL) == K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (timeout == TOS_TIME_NOWAIT) {
        *msg_ptr = K_NULL;
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_NOWAIT;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    pend_task_block(k_curr_task, &msg_q->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    err = pend_state2errno(k_curr_task->pend_state);
    if (err == K_ERR_NONE) {
        *msg_ptr = k_curr_task->msg;
        k_curr_task->msg = K_NULL;
    }

    return err;
}

__STATIC__ void msg_q_task_recv(k_task_t *task, void *msg_ptr)
{
    task->msg = msg_ptr;
    pend_task_wakeup(task, PEND_STATE_POST);
}

__STATIC__ k_err_t msg_q_do_post(k_msg_q_t *msg_q, void *msg_ptr, opt_post_t opt)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;
    k_task_t *task, *tmp;

    TOS_PTR_SANITY_CHECK(msg_q);
    TOS_OBJ_VERIFY(msg_q, KNL_OBJ_TYPE_MESSAGE_QUEUE);

    TOS_CPU_INT_DISABLE();

    if (pend_is_nopending(&msg_q->pend_obj)) {
        err = tos_ring_q_enqueue(&msg_q->ring_q, &msg_ptr, sizeof(void*));
        if (err != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return err;
        }
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (opt == OPT_POST_ONE) {
        msg_q_task_recv(TOS_LIST_FIRST_ENTRY(&msg_q->pend_obj.list, k_task_t, pend_list), msg_ptr);
    } else { // OPT_POST_ALL
        TOS_LIST_FOR_EACH_ENTRY_SAFE(task, tmp, k_task_t, pend_list, &msg_q->pend_obj.list) {
            msg_q_task_recv(task, msg_ptr);
        }
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_msg_q_post(k_msg_q_t *msg_q, void *msg_ptr)
{
    return msg_q_do_post(msg_q, msg_ptr, OPT_POST_ONE);
}

__API__ k_err_t tos_msg_q_post_all(k_msg_q_t *msg_q, void *msg_ptr)
{
    return msg_q_do_post(msg_q, msg_ptr, OPT_POST_ALL);
}

#endif

