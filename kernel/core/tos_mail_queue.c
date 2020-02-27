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

#if TOS_CFG_MAIL_QUEUE_EN > 0u

__API__ k_err_t tos_mail_q_create(k_mail_q_t *mail_q, void *pool, size_t mail_cnt, size_t mail_size)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(mail_q);

    err = tos_ring_q_create(&mail_q->ring_q, pool, mail_cnt, mail_size);
    if (err != K_ERR_NONE) {
        return err;
    }

    pend_object_init(&mail_q->pend_obj);

    TOS_OBJ_INIT(mail_q, KNL_OBJ_TYPE_MAIL_QUEUE);
#if TOS_CFG_MMHEAP_EN > 0u
    knl_object_alloc_set_static(&mail_q->knl_obj);
#endif

    return K_ERR_NONE;
}

__API__ k_err_t tos_mail_q_destroy(k_mail_q_t *mail_q)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(mail_q);
    TOS_OBJ_VERIFY(mail_q, KNL_OBJ_TYPE_MAIL_QUEUE);

#if TOS_CFG_MMHEAP_EN > 0u
    if (!knl_object_alloc_is_static(&mail_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }
#endif

    TOS_CPU_INT_DISABLE();

    err = tos_ring_q_destroy(&mail_q->ring_q);
    if (err != K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return err;
    }

    if (!pend_is_nopending(&mail_q->pend_obj)) {
        pend_wakeup_all(&mail_q->pend_obj, PEND_STATE_DESTROY);
    }

    pend_object_deinit(&mail_q->pend_obj);

    TOS_OBJ_DEINIT(mail_q);
#if TOS_CFG_MMHEAP_EN > 0u
    knl_object_alloc_reset(&mail_q->knl_obj);
#endif

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

#if TOS_CFG_MMHEAP_EN > 0u

__API__ k_err_t tos_mail_q_create_dyn(k_mail_q_t *mail_q, size_t mail_cnt, size_t mail_size)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(mail_q);

    err = tos_ring_q_create_dyn(&mail_q->ring_q, mail_cnt, mail_size);
    if (err != K_ERR_NONE) {
        return err;
    }

    pend_object_init(&mail_q->pend_obj);

    TOS_OBJ_INIT(mail_q, KNL_OBJ_TYPE_MAIL_QUEUE);
    knl_object_alloc_set_dynamic(&mail_q->knl_obj);

    return K_ERR_NONE;
}

__API__ k_err_t tos_mail_q_destroy_dyn(k_mail_q_t *mail_q)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(mail_q);
    TOS_OBJ_VERIFY(mail_q, KNL_OBJ_TYPE_MAIL_QUEUE);

    if (!knl_object_alloc_is_dynamic(&mail_q->knl_obj)) {
        return K_ERR_OBJ_INVALID_ALLOC_TYPE;
    }

    TOS_CPU_INT_DISABLE();

    err = tos_ring_q_destroy_dyn(&mail_q->ring_q);
    if (err != K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return err;
    }

    if (!pend_is_nopending(&mail_q->pend_obj)) {
        pend_wakeup_all(&mail_q->pend_obj, PEND_STATE_DESTROY);
    }

    pend_object_deinit(&mail_q->pend_obj);

    TOS_OBJ_DEINIT(mail_q);
    knl_object_alloc_reset(&mail_q->knl_obj);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

#endif

__API__ k_err_t tos_mail_q_flush(k_mail_q_t *mail_q)
{
    TOS_PTR_SANITY_CHECK(mail_q);
    TOS_OBJ_VERIFY(mail_q, KNL_OBJ_TYPE_MAIL_QUEUE);

    return tos_ring_q_flush(&mail_q->ring_q);
}

__API__ k_err_t tos_mail_q_pend(k_mail_q_t *mail_q, void *mail_buf, size_t *mail_size, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_IN_IRQ_CHECK();
    TOS_PTR_SANITY_CHECK(mail_q);
    TOS_PTR_SANITY_CHECK(mail_buf);
    TOS_OBJ_VERIFY(mail_q, KNL_OBJ_TYPE_MAIL_QUEUE);

    TOS_CPU_INT_DISABLE();

    if (tos_ring_q_dequeue(&mail_q->ring_q, mail_buf, mail_size) == K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (timeout == TOS_TIME_NOWAIT) {
        *mail_size = 0;
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_NOWAIT;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    k_curr_task->mail = mail_buf;
    pend_task_block(k_curr_task, &mail_q->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    err = pend_state2errno(k_curr_task->pend_state);
    if (err == K_ERR_NONE) {
        *mail_size              = k_curr_task->mail_size;
        k_curr_task->mail       = K_NULL;
        k_curr_task->mail_size  = 0;
    }

    return err;
}

__STATIC__ void mail_task_recv(k_task_t *task, void *mail_buf, size_t mail_size)
{
    memcpy(task->mail, mail_buf, mail_size);
    task->mail_size = mail_size;
    pend_task_wakeup(task, PEND_STATE_POST);
}

__STATIC__ k_err_t mail_q_do_post(k_mail_q_t *mail_q, void *mail_buf, size_t mail_size, opt_post_t opt)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;
    k_task_t *task, *tmp;

    TOS_PTR_SANITY_CHECK(mail_q);
    TOS_PTR_SANITY_CHECK(mail_buf);
    TOS_OBJ_VERIFY(mail_q, KNL_OBJ_TYPE_MAIL_QUEUE);

    TOS_CPU_INT_DISABLE();

    if (pend_is_nopending(&mail_q->pend_obj)) {
        err = tos_ring_q_enqueue(&mail_q->ring_q, mail_buf, mail_size);
        if (err != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return err;
        }
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (opt == OPT_POST_ONE) {
        mail_task_recv(TOS_LIST_FIRST_ENTRY(&mail_q->pend_obj.list, k_task_t, pend_list),
                            mail_buf, mail_size);
    } else { // OPT_POST_ALL
        TOS_LIST_FOR_EACH_ENTRY_SAFE(task, tmp, k_task_t, pend_list, &mail_q->pend_obj.list) {
            mail_task_recv(task, mail_buf, mail_size);
        }
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_mail_q_post(k_mail_q_t *mail_q, void *mail_buf, size_t mail_size)
{
    return mail_q_do_post(mail_q, mail_buf, mail_size, OPT_POST_ONE);
}

__API__ k_err_t tos_mail_q_post_all(k_mail_q_t *mail_q, void *mail_buf, size_t mail_size)
{
    return mail_q_do_post(mail_q, mail_buf, mail_size, OPT_POST_ALL);
}

#endif

