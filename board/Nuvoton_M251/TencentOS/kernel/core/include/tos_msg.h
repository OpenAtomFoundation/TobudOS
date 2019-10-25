#ifndef _TOS_MSG_H_
#define  _TOS_MSG_H_

#define TOS_OPT_MSG_PUT_LIFO        (k_opt_t)0x0001
#define TOS_OPT_MSG_PUT_FIFO        (k_opt_t)0x0002

typedef struct k_message_st {
    k_list_t        list;
    void           *msg_addr;
    size_t          msg_size;
} k_msg_t;

typedef struct k_msg_queue_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t       knl_obj;
#endif

    k_list_t        queue_head;
} k_msg_queue_t;

/**
 * @brief Create a message queue.
 * Initialize a message queue.
 *
 * @attention None
 *
 * @param[IN]   msg_queue       the pointer to the handler of the message queue.
 *
 * @return  errcode.
 * @retval  #K_ERR_OBJ_PTR_NULL msg_queue is a null pointer
 * @retval  #K_ERR_NONE         return successfully
 */
__API__ k_err_t tos_msg_queue_create(k_msg_queue_t *msg_queue);

/**
 * @brief Destroy a message queue.
 *
 * @attention None
 *
 * @param[IN]   msg_queue       the pointer to the handler of the message queue.
 *
 * @return  errcode.
 * @retval  #K_ERR_OBJ_PTR_NULL msg_queue is a null pointer
 * @retval  #K_ERR_OBJ_INVALID  msg_queue is not a valid pointer to a message queue
 * @retval  #K_ERR_NONE         return successfully
 */
__API__ k_err_t tos_msg_queue_destroy(k_msg_queue_t *msg_queue);

/**
 * @brief Get a message.
 * Get a message from the queue.
 *
 * @attention None
 *
 * @param[IN]   msg_queue       the pointer to the handler of the message queue.
 * @param[OUT]  msg_body        the pointer to the body of the message.
 * @param[OUT]  msg_size        the pointer to the size of the message.
 *
 * @return  errcode.
 * @retval  #K_ERR_QUEUE_EMPTY  the queue is empty.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_msg_queue_get(k_msg_queue_t *msg_queue, void **msg_addr, size_t *msg_size);

/**
 * @brief Put a message.
 * Put a message to the queue.
 *
 * @attention None
 *
 * @param[IN]   msg_queue       the pointer to the handler of the message queue.
 * @param[IN]   msg_body        the pointer to the body of the message.
 * @param[IN]   msg_size        the pointer to the size of the message.
 * @param[IN]   opt             option for the function call.
 *
 * @return  errcode.
 * @retval  #K_ERR_QUEUE_FULL   the queue is full.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_msg_queue_put(k_msg_queue_t *msg_queue, void *msg_addr, size_t msg_size, k_opt_t opt);

/**
 * @brief Flush all of the messages.
 * Flush all of the messages in the queue.
 *
 * @attention None
 *
 * @param[IN]   msg_queue       the pointer to the handler of the message queue.
 *
 * @return  None.
 */
__API__ void    tos_msg_queue_flush(k_msg_queue_t *msg_queue);

__KERNEL__ void msgpool_init(void);

#endif /* _TOS_MSG_H_ */

