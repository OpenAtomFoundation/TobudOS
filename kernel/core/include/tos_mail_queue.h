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

#ifndef _TOS_MAIL_QUEUE_H_
#define  _TOS_MAIL_QUEUE_H_

#if TOS_CFG_MAIL_QUEUE_EN > 0u

typedef struct k_mail_queue_st {
    knl_obj_t   knl_obj;

    pend_obj_t  pend_obj;
    k_ring_q_t  ring_q;
} k_mail_q_t;

/**
 * @brief Create a mail queue.
 * create a mail queue.
 *
 * @attention a MAIL is a buffer with a certain size.
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 * @param[in]   pool        pool buffer of the mail queue.
 * @param[in]   mail_cnt    mail count of the mail queue.
 * @param[in]   mail_size   size of each mail in the mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_mail_q_create(k_mail_q_t *mail_q, void *pool, size_t mail_cnt, size_t mail_size);

/**
 * @brief Destroy a mail queue.
 * destroy a mail queue.
 *
 * @attention None
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_mail_q_destroy(k_mail_q_t *mail_q);

#if TOS_CFG_MMHEAP_EN > 0u

/**
 * @brief Create a mail queue with dynamic allocated pool.
 * create a mail queue with dynamic allocated pool.
 *
 * @attention a MAIL is a buffer with a certain size.
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 * @param[in]   mail_cnt    mail count of the mail queue.
 * @param[in]   mail_size   size of each mail in the mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_mail_q_create_dyn(k_mail_q_t *mail_q, size_t mail_cnt, size_t mail_size);

/**
 * @brief Destroy a mail queue with dynamic allocated pool.
 * destroy a mail queue with dynamic allocated pool.
 *
 * @attention None
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_mail_q_destroy_dyn(k_mail_q_t *mail_q);

#endif

/**
 * @brief Flush the mail queue.
 * flush the mail queue.
 *
 * @attention None
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_mail_q_flush(k_mail_q_t *mail_q);

/**
 * @brief Pend a mail queue.
 * pend a mail queue.
 *
 * @attention we WILL perform a memcpy when mail_buf is received(a MAIL is a buffer with a certain size).
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 * @param[OUT]  mail_buf    a pointer to the mail buffer we wanna receive.
 * @param[OUT]  mail_size   size of the mail buffer received(should be consistent with the mail_size passed to tos_mail_q_create).
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_PEND_DESTROY               the queue we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_mail_q_pend(k_mail_q_t *mail_q, void *mail_buf, size_t *mail_size, k_tick_t timeout);

/**
 * @brief Post a mail queue.
 * post a mail queue and wakeup one pending task.
 *
 * @attention when tos_mail_q_post return successfully, only one task who are waitting for the mail queue will be woken up.
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 * @param[in]   mail_buf    the mail to post(a MAIL is a buffer).
 * @param[in]   mail_size   the size of the mail to post(a MAIL is just a buffer).
 *
 * @return  errcode
 * @retval  #K_ERR_RING_Q_FULL      the mail queue is full.
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_mail_q_post(k_mail_q_t *mail_q, void *mail_buf, size_t mail_size);

/**
 * @brief Post a mail queue.
 * post a mail queue and wakeup all the pending task.
 *
 * @attention when tos_mail_q_post_all return successfully, all of the tasks who are waitting for the message queue will be woken up.
 *
 * @param[in]   mail_q      pointer to the handler of the mail queue.
 * @param[in]   mail_buf    the mail to post(a MAIL is a buffer).
 * @param[in]   mail_size   the size of the mail to post(a MAIL is just a buffer).
 *
 * @return  errcode
 * @retval  #K_ERR_RING_Q_FULL      the mail queue is full.
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_mail_q_post_all(k_mail_q_t *mail_q, void *mail_buf, size_t mail_size);

#endif

#endif /* _TOS_MAIL_QUEUE_H_ */

