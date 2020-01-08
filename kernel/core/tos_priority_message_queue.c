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

#if TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN > 0u

__API__ k_err_t tos_prio_msg_q_create(k_prio_msg_q_t *prio_msg_q, void *pool, size_t msg_cnt)
{
    k_err_t err;
    void *prio_q_mgr_array = K_NULL;

    TOS_PTR_SANITY_CHECK(prio_msg_q);

    prio_q_mgr_array = tos_mmheap_alloc(TOS_PRIO_Q_MGR_ARRAY_SIZE(msg_cnt));
    if (!prio_q_mgr_array) {
        return K_ERR_OUT_OF_MEMORY;
    }

    err = tos_prio_q_create(&prio_msg_q->prio_q, prio_q_mgr_array, pool, msg_cnt, sizeof(void *));
    if (err != K_ERR_NONE) {
        tos_mmheap_free(prio_q_mgr_array);
        return err;
    }

    prio_msg_q->prio_q_mgr_array = prio_q_mgr_array;
    pend_object_init(&prio_msg_q->pend_obj);

    TOS_OBJ_INIT(prio_msg_q, KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE);
    knl_object_alloc_set_static(&prio_msg_q->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_msg_q_destroy(k_prio_msg_q_t *prio_msg_q)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(prio_msg_q);
    TOS_OBJ_VERIFY(prio_msg_q, KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE);

    if (!knl_object_alloc_is_static(&prio_msg_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }

    TOS_CPU_INT_DISABLE();

    err = tos_prio_q_destroy(&prio_msg_q->prio_q);
    if (err != K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return err;
    }

    if (!pend_is_nopending(&prio_msg_q->pend_obj)) {
        pend_wakeup_all(&prio_msg_q->pend_obj, PEND_STATE_DESTROY);
    }

    tos_mmheap_free(prio_msg_q->prio_q_mgr_array);
    prio_msg_q->prio_q_mgr_array = K_NULL;

    pend_object_deinit(&prio_msg_q->pend_obj);

    TOS_OBJ_DEINIT(prio_msg_q);
    knl_object_alloc_reset(&prio_msg_q->knl_obj);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_msg_q_create_dyn(k_prio_msg_q_t *prio_msg_q, size_t msg_cnt)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(prio_msg_q);

    err = tos_prio_q_create_dyn(&prio_msg_q->prio_q, msg_cnt, sizeof(void *));
    if (err != K_ERR_NONE) {
        return err;
    }

    pend_object_init(&prio_msg_q->pend_obj);

    TOS_OBJ_INIT(prio_msg_q, KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE);
    knl_object_alloc_set_dynamic(&prio_msg_q->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_msg_q_destroy_dyn(k_prio_msg_q_t *prio_msg_q)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(prio_msg_q);
    TOS_OBJ_VERIFY(prio_msg_q, KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE);

    if (!knl_object_alloc_is_dynamic(&prio_msg_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }

    TOS_CPU_INT_DISABLE();

    err = tos_prio_q_destroy_dyn(&prio_msg_q->prio_q);
    if (err != K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return err;
    }

    if (!pend_is_nopending(&prio_msg_q->pend_obj)) {
        pend_wakeup_all(&prio_msg_q->pend_obj, PEND_STATE_DESTROY);
    }

    tos_mmheap_free(prio_msg_q->prio_q_mgr_array);
    prio_msg_q->prio_q_mgr_array = K_NULL;

    pend_object_deinit(&prio_msg_q->pend_obj);

    TOS_OBJ_DEINIT(prio_msg_q);
    knl_object_alloc_reset(&prio_msg_q->knl_obj);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_msg_q_flush(k_prio_msg_q_t *prio_msg_q)
{
    TOS_PTR_SANITY_CHECK(prio_msg_q);
    TOS_OBJ_VERIFY(prio_msg_q, KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE);

    return tos_prio_q_flush(&prio_msg_q->prio_q);
}

__API__ k_err_t tos_prio_msg_q_pend(k_prio_msg_q_t *prio_msg_q, void **msg_ptr, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(prio_msg_q);
    TOS_PTR_SANITY_CHECK(msg_ptr);
    TOS_OBJ_VERIFY(prio_msg_q, KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE);

    TOS_CPU_INT_DISABLE();

    if (tos_prio_q_dequeue(&prio_msg_q->prio_q, msg_ptr, K_NULL, K_NULL) == K_ERR_NONE) {
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

    pend_task_block(k_curr_task, &prio_msg_q->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    err = pend_state2errno(k_curr_task->pend_state);
    if (err == K_ERR_NONE) {
        *msg_ptr            = k_curr_task->msg;
        k_curr_task->msg    = K_NULL;
    }

    return err;
}

__STATIC__ void prio_msg_q_task_recv(k_task_t *task, void *msg_ptr)
{
    task->msg = msg_ptr;
    pend_task_wakeup(task, PEND_STATE_POST);
}

__STATIC__ k_err_t prio_msg_q_do_post(k_prio_msg_q_t *prio_msg_q, void *msg_ptr, k_prio_t prio, opt_post_t opt)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;
    k_task_t *task, *tmp;

    TOS_PTR_SANITY_CHECK(prio_msg_q);
    TOS_OBJ_VERIFY(prio_msg_q, KNL_OBJ_TYPE_PRIORITY_MESSAGE_QUEUE);

    TOS_CPU_INT_DISABLE();

    if (pend_is_nopending(&prio_msg_q->pend_obj)) {
        err = tos_prio_q_enqueue(&prio_msg_q->prio_q, &msg_ptr, sizeof(void *), prio);
        if (err != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return err;
        }
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (opt == OPT_POST_ONE) {
        prio_msg_q_task_recv(TOS_LIST_FIRST_ENTRY(&prio_msg_q->pend_obj.list, k_task_t, pend_list), msg_ptr);
    } else { // OPT_POST_ALL
        TOS_LIST_FOR_EACH_ENTRY_SAFE(task, tmp, k_task_t, pend_list, &prio_msg_q->pend_obj.list) {
            prio_msg_q_task_recv(task, msg_ptr);
        }
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_prio_msg_q_post(k_prio_msg_q_t *prio_msg_q, void *msg_ptr, k_prio_t prio)
{
    return prio_msg_q_do_post(prio_msg_q, msg_ptr, prio, OPT_POST_ONE);
}

__API__ k_err_t tos_prio_msg_q_post_all(k_prio_msg_q_t *prio_msg_q, void *msg_ptr, k_prio_t prio)
{
    return prio_msg_q_do_post(prio_msg_q, msg_ptr, prio, OPT_POST_ALL);
}

#endif

