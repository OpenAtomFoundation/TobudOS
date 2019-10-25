#include <tos.h>

#if TOS_CFG_MSG_EN > 0u

__KERNEL__ void msgpool_init(void)
{
    uint32_t i;

    for (i = 0; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        tos_list_init(&k_msg_pool[i].list);
        tos_list_add(&k_msg_pool[i].list, &k_msg_freelist);
    }
}

__STATIC__ k_msg_t *msgpool_alloc(void)
{
    k_msg_t *msg = K_NULL;

    if (tos_list_empty(&k_msg_freelist)) {
        return K_NULL;
    }

    msg = TOS_LIST_FIRST_ENTRY(&k_msg_freelist, k_msg_t, list);
    tos_list_del(&msg->list);
    return msg;
}

__STATIC__ void msgpool_free(k_msg_t *msg)
{
    tos_list_del(&msg->list);
    tos_list_add(&msg->list, &k_msg_freelist);
}

__API__ void tos_msg_queue_flush(k_msg_queue_t *msg_queue)
{
    TOS_CPU_CPSR_ALLOC();
    k_list_t *curr, *next;

    TOS_CPU_INT_DISABLE();

    TOS_LIST_FOR_EACH_SAFE(curr, next, &msg_queue->queue_head) {
        msgpool_free(TOS_LIST_ENTRY(curr, k_msg_t, list));
    }

    TOS_CPU_INT_ENABLE();
}

__API__ k_err_t tos_msg_queue_create(k_msg_queue_t *msg_queue)
{
    TOS_PTR_SANITY_CHECK(msg_queue);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&msg_queue->knl_obj, KNL_OBJ_TYPE_MSG_QUEUE);
#endif

    tos_list_init(&msg_queue->queue_head);
    return K_ERR_NONE;
}

__API__ k_err_t tos_msg_queue_destroy(k_msg_queue_t *msg_queue)
{
    TOS_PTR_SANITY_CHECK(msg_queue);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&msg_queue->knl_obj, KNL_OBJ_TYPE_MSG_QUEUE)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    tos_msg_queue_flush(msg_queue);
    tos_list_init(&msg_queue->queue_head);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&msg_queue->knl_obj);
#endif

    return K_ERR_NONE;
}

__API__ k_err_t tos_msg_queue_get(k_msg_queue_t *msg_queue, void **msg_addr, size_t *msg_size)
{
    TOS_CPU_CPSR_ALLOC();
    k_msg_t *msg;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&msg_queue->knl_obj, KNL_OBJ_TYPE_MSG_QUEUE)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    msg = TOS_LIST_FIRST_ENTRY_OR_NULL(&msg_queue->queue_head, k_msg_t, list);
    if (!msg) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_MSG_QUEUE_EMPTY;
    }

    *msg_addr = msg->msg_addr;
    *msg_size = msg->msg_size;
    msgpool_free(msg);

    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

__API__ k_err_t tos_msg_queue_put(k_msg_queue_t *msg_queue, void *msg_addr, size_t msg_size, k_opt_t opt)
{
    TOS_CPU_CPSR_ALLOC();
    k_msg_t *msg;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&msg_queue->knl_obj, KNL_OBJ_TYPE_MSG_QUEUE)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    msg = msgpool_alloc();
    if (!msg) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_MSG_QUEUE_FULL;
    }

    msg->msg_addr = msg_addr;
    msg->msg_size = msg_size;

    if (opt & TOS_OPT_MSG_PUT_LIFO) {
        tos_list_add(&msg->list, &msg_queue->queue_head);
    } else {
        tos_list_add_tail(&msg->list, &msg_queue->queue_head);
    }

    TOS_CPU_INT_ENABLE();

    return K_ERR_NONE;
}

#endif

