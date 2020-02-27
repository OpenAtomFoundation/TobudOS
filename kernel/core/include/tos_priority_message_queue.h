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

#ifndef _TOS_PRIORITY_MESSAGE_QUEUE_H_
#define  _TOS_PRIORITY_MESSAGE_QUEUE_H_

__CDECLS_BEGIN

#if TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN > 0u

typedef struct k_priority_message_queue_st {
    knl_obj_t   knl_obj;

    pend_obj_t  pend_obj;

    void       *prio_q_mgr_array;
    k_prio_q_t  prio_q;
} k_prio_msg_q_t;

/**
 * @brief Create a priority message queue.
 * create a priority message queue.
 *
 * @attention a MESSAGE is a "void *" pointer.
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 * @param[in]   pool        pool buffer of the priority message queue.
 * @param[in]   msg_cnt     message count of the priority message queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_msg_q_create(k_prio_msg_q_t *prio_msg_q, void *pool, size_t msg_cnt);

/**
 * @brief Destroy a priority message queue.
 * destroy a priority message queue.
 *
 * @attention None
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_msg_q_destroy(k_prio_msg_q_t *prio_msg_q);

/**
 * @brief Create a priority message queue with dynamic allocated pool.
 * create a priority message queue with dynamic allocated pool.
 *
 * @attention a MESSAGE is a "void *" pointer.
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 * @param[in]   pool        pool buffer of the priority message queue.
 * @param[in]   msg_cnt     message count of the priority message queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_msg_q_create_dyn(k_prio_msg_q_t *prio_msg_q, size_t msg_cnt);

/**
 * @brief Destroy a priority message queue with dynamic allocated pool.
 * destroy a priority message queue with dynamic allocated pool.
 *
 * @attention None
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_msg_q_destroy_dyn(k_prio_msg_q_t *prio_msg_q);

/**
 * @brief Flush the priority message queue.
 * flush the priority message queue.
 *
 * @attention None
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_prio_msg_q_flush(k_prio_msg_q_t *prio_msg_q);

/**
 * @brief Pend a priority message queue.
 * pend a priority message queue.
 *
 * @attentio
 * 1. we DONNOT perform a memcpy when msg_ptr is received(a MESSAGE is just a pointer).
 * 2. With priority message queue, if the poster has post several message with different priority, the pender will receive
 *    the message in priority order(numerically bigger, actually smaller, means the message with highest priority(numerically
 *    smallest) will be received first, then the second highest priority message, and so on).
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 * @param[OUT]  msg_ptr     a pointer to the message we wanna receive.
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_PEND_DESTROY               the queue we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_prio_msg_q_pend(k_prio_msg_q_t *prio_msg_q, void **msg_ptr, k_tick_t timeout);

/**
 * @brief Post a priority message queue.
 * post a priority message queue and wakeup one pending task.
 *
 * @attention when tos_prio_msg_q_post return successfully, only one task who are waitting for the priority message queue will be woken up.
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 * @param[in]   msg_ptr     the message to post(a MESSAGE is just a pointer).
 * @param[in]   prio        the priority of the message.
 *
 * @return  errcode
 * @retval  #K_ERR_PRIO_Q_FULL      the priority message queue is full.
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_prio_msg_q_post(k_prio_msg_q_t *prio_msg_q, void *msg_ptr, k_prio_t prio);

/**
 * @brief Post a priority message queue.
 * post a priority message queue and wakeup all the pending task.
 *
 * @attention when tos_prio_msg_q_post_all return successfully, all of the tasks who are waitting for the priority message queue will be woken up.
 *
 * @param[in]   prio_msg_q  pointer to the handler of the priority message queue.
 * @param[in]   msg_ptr     the priority message to post(a MESSAGE is just a pointer).
 * @param[in]   prio        the priority of the message.
 *
 * @return  errcode
 * @retval  #K_ERR_PRIO_Q_FULL      the priority message queue is full.
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_prio_msg_q_post_all(k_prio_msg_q_t *prio_msg_q, void *msg_ptr, k_prio_t prio);

#endif

__CDECLS_END

#endif /* _TOS_PRIORITY_MESSAGE_QUEUE_H_ */

