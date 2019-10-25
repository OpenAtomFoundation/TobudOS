#include "tos.h"

#if TOS_CFG_QUEUE_EN > 0u

__API__ k_err_t tos_queue_create(k_queue_t *queue)
{
    TOS_PTR_SANITY_CHECK(queue);

    pend_object_init(&queue->pend_obj, PEND_TYPE_QUEUE);
    tos_msg_queue_create(&queue->msg_queue);
    return K_ERR_NONE;
}

__API__ k_err_t tos_queue_destroy(k_queue_t *queue)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(queue);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!pend_object_verify(&queue->pend_obj, PEND_TYPE_QUEUE)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (!pend_is_nopending(&queue->pend_obj)) {
        pend_wakeup_all(&queue->pend_obj, PEND_STATE_DESTROY);
    }

    pend_object_deinit(&queue->pend_obj);
    tos_msg_queue_flush(&queue->msg_queue);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_queue_flush(k_queue_t *queue)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(queue);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!pend_object_verify(&queue->pend_obj, PEND_TYPE_QUEUE)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();
    tos_msg_queue_flush(&queue->msg_queue);
    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

__API__ k_err_t tos_queue_pend(k_queue_t *queue, void **msg_addr, size_t *msg_size, k_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();
    k_err_t err;

    TOS_PTR_SANITY_CHECK(queue);
    TOS_PTR_SANITY_CHECK(msg_addr);
    TOS_PTR_SANITY_CHECK(msg_size);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!pend_object_verify(&queue->pend_obj, PEND_TYPE_QUEUE)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (tos_msg_queue_get(&queue->msg_queue, msg_addr, msg_size) == K_ERR_NONE) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (timeout == TOS_TIME_NOWAIT) {
        *msg_addr = K_NULL;
        *msg_size = 0;
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_NOWAIT;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    pend_task_block(k_curr_task, &queue->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    err = pend_state2errno(k_curr_task->pend_state);

    if (err == K_ERR_NONE) {
        *msg_addr = k_curr_task->msg_addr;
        *msg_size = k_curr_task->msg_size;
        k_curr_task->msg_addr = K_NULL;
        k_curr_task->msg_size = 0;
    }

    return err;
}

__STATIC__ void queue_task_msg_recv(k_task_t *task, void *msg_addr, size_t msg_size)
{
    task->msg_addr = msg_addr;
    task->msg_size = msg_size;
    pend_task_wakeup(task, PEND_STATE_POST);
}

__STATIC__ k_err_t queue_do_post(k_queue_t *queue, void *msg_addr, size_t msg_size, opt_post_t opt)
{
    TOS_CPU_CPSR_ALLOC();
    k_list_t *curr, *next;

    TOS_PTR_SANITY_CHECK(queue);
    TOS_PTR_SANITY_CHECK(msg_addr);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!pend_object_verify(&queue->pend_obj, PEND_TYPE_QUEUE)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (pend_is_nopending(&queue->pend_obj)) {
        if (tos_msg_queue_put(&queue->msg_queue, msg_addr, msg_size, TOS_OPT_MSG_PUT_FIFO) != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return K_ERR_QUEUE_FULL;
        }
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (opt == OPT_POST_ONE) {
        queue_task_msg_recv(TOS_LIST_FIRST_ENTRY(&queue->pend_obj.list, k_task_t, pend_list),
                                msg_addr, msg_size);
    } else { // OPT_QUEUE_POST_ALL
        TOS_LIST_FOR_EACH_SAFE(curr, next, &queue->pend_obj.list) {
            queue_task_msg_recv(TOS_LIST_ENTRY(curr, k_task_t, pend_list),
                                msg_addr, msg_size);
        }
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_queue_post(k_queue_t *queue, void *msg_addr, size_t msg_size)
{
    return queue_do_post(queue, msg_addr, msg_size, OPT_POST_ONE);
}

__API__ k_err_t tos_queue_post_all(k_queue_t *queue, void *msg_addr, size_t msg_size)
{
    return queue_do_post(queue, msg_addr, msg_size, OPT_POST_ALL);
}

#endif

