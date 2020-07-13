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

#ifndef _TOS_PRIORITY_MAIL_QUEUE_H_
#define  _TOS_PRIORITY_MAIL_QUEUE_H_

__CDECLS_BEGIN

#if TOS_CFG_PRIORITY_MAIL_QUEUE_EN > 0u

typedef struct k_priority_mail_queue_st {
    knl_obj_t   knl_obj;

    pend_obj_t  pend_obj;

    void       *prio_q_mgr_array;
    k_prio_q_t  prio_q;
} k_prio_mail_q_t;

/**
 * @brief Create a priority mail queue.
 * create a priority mail queue.
 *
 * @attention a MAIL is a buffer with a certain size.
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 * @param[in]   pool        pool buffer of the priority mail queue.
 * @param[in]   mail_cnt    mail count of the priority mail queue.
 * @param[in]   mail_size   size of each mail in the priority mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_mail_q_create(k_prio_mail_q_t *prio_mail_q, void *pool, size_t mail_cnt, size_t mail_size);

/**
 * @brief Destroy a priority mail queue.
 * destroy a priority mail queue.
 *
 * @attention None
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_mail_q_destroy(k_prio_mail_q_t *prio_mail_q);

/**
 * @brief Create a priority mail queue with dynamic allocated pool.
 * create a priority mail queue with dynamic allocated pool.
 *
 * @attention a MAIL is a buffer with a certain size.
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 * @param[in]   mail_cnt    mail count of the priority mail queue.
 * @param[in]   mail_size   size of each mail in the priority mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_mail_q_create_dyn(k_prio_mail_q_t *prio_mail_q, size_t mail_cnt, size_t mail_size);

/**
 * @brief Destroy a priority mail queue with dynamic allocated pool.
 * destroy a priority mail queue with dynamic allocated pool.
 *
 * @attention None
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_prio_mail_q_destroy_dyn(k_prio_mail_q_t *prio_mail_q);

/**
 * @brief Flush the priority mail queue.
 * flush the priority mail queue.
 *
 * @attention None
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_prio_mail_q_flush(k_prio_mail_q_t *prio_mail_q);

/**
 * @brief Pend a priority mail queue.
 * pend a priority mail queue.
 *
 * @attention
 * 1. we WILL perform a memcpy when mail_buf is received(a MAIL is a buffer with a certain size).
 * 2. With priority mail queue, if the poster has post several mail with different priority, the pender will receive
 *    the mail in priority order(numerically bigger, actually smaller, means the mail with highest priority(numerically
 *    smallest) will be received first, then the second highest priority mail, and so on).
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 * @param[OUT]  mail_buf    a pointer to the mail buffer we wanna receive.
 * @param[OUT]  mail_size   size of the mail buffer received(should be consistent with the mail_size passed to tos_prio_mail_q_create).
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_PEND_DESTROY               the queue we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_prio_mail_q_pend(k_prio_mail_q_t *prio_mail_q, void *mail_buf, size_t *mail_size, k_tick_t timeout);

/**
 * @brief Post a priority mail queue.
 * post a priority mail queue and wakeup one pending task.
 *
 * @attention when tos_prio_mail_q_post return successfully, only one task who are waitting for the mail queue will be woken up.
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 * @param[in]   mail_buf    the mail to post(a MAIL is a buffer).
 * @param[in]   mail_size   the size of the mail to post(a MAIL is just a buffer).
 * @param[in]   prio        the priority of the mail.
 *
 * @return  errcode
 * @retval  #K_ERR_PRIO_Q_FULL      the mail queue is full.
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_prio_mail_q_post(k_prio_mail_q_t *prio_mail_q, void *mail_buf, size_t mail_size, k_prio_t prio);

/**
 * @brief Post a priority mail queue.
 * post a priority mail queue and wakeup all the pending task.
 *
 * @attention when tos_prio_mail_q_post_all return successfully, all of the tasks who are waitting for the message queue will be woken up.
 *
 * @param[in]   prio_mail_q pointer to the handler of the priority mail queue.
 * @param[in]   mail_buf    the mail to post(a MAIL is a buffer).
 * @param[in]   mail_size   the size of the mail to post(a MAIL is just a buffer).
 * @param[in]   prio        the priority of the mail.
 *
 * @return  errcode
 * @retval  #K_ERR_PRIO_Q_FULL      the mail queue is full.
 * @retval  #K_ERR_NONE             return successfully.
 */
__API__ k_err_t tos_prio_mail_q_post_all(k_prio_mail_q_t *prio_mail_q, void *mail_buf, size_t mail_size, k_prio_t prio);

#endif /* TOS_CFG_PRIORITY_MAIL_QUEUE_EN */

__CDECLS_END

#endif /* _TOS_PRIORITY_MAIL_QUEUE_H_ */

