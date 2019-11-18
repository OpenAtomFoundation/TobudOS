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

#ifndef _TOS_COUNTDOWNLATCH_H_
#define  _TOS_COUNTDOWNLATCH_H_

#if TOS_CFG_COUNTDOWNLATCH_EN > 0u

typedef struct k_countdownlatch_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t               knl_obj;
#endif

    pend_obj_t              pend_obj;
    k_countdownlatch_cnt_t  count;
} k_countdownlatch_t;

/**
 * @brief Create a countdown-latch.
 * create a countdown latch.
 *
 * @attention the count is how many posts have been done the pender would wakeup.
 *
 * @param[in]   countdownlatch  pointer to the handler of the countdown-latch.
 * @param[in]   count           the count to wait of the countdown-latch.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_countdownlatch_create(k_countdownlatch_t *countdownlatch, k_countdownlatch_cnt_t count);

/**
 * @brief Destroy a countdown-latch.
 * destroy a countdown-latch.
 *
 * @attention None
 *
 * @param[in]   countdownlatch  pointer to the handler of the countdown-latch.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_countdownlatch_destroy(k_countdownlatch_t *countdownlatch);

/**
 * @brief Pend a countdown-latch.
 * pend a countdown-latch.
 *
 * @attention The task will keep blocked until the countdown-latch is obtained or a timeout comes.
 *
 * @param[in]   countdownlatch  pointer to the handler of the countdown-latch.
 * @param[in]   timeout         how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_countdownlatch_pend_timed(k_countdownlatch_t *countdownlatch, k_tick_t timeout);

/**
 * @brief Pend a countdown-latch.
 * pend a countdown latch.
 *
 * @attention until (countdownlatch->count) of tasks have done the post, the pender would wake up.
 *
 * @param[in]   countdownlatch  pointer to the handler of the countdown-latch.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_SCHED_LOCKED    the schedule is locked.
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_countdownlatch_pend(k_countdownlatch_t *countdownlatch);

/**
 * @brief Post a countdown-latch.
 * post a countdown-latch.
 *
 * @attention until (countdownlatch->count) of tasks have done the post, the pender would wake up.
 *
 * @param[in]   countdownlatch  pointer to the handler of the countdown-latch.
 *
 * @return  errcode
 * @retval  #K_ERR_COUNTDOWNLATCH_OVERFLOW  we are posting the countdown-latch too much.
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_countdownlatch_post(k_countdownlatch_t *countdownlatch);

/**
 * @brief Reset a countdown-latch.
 * reset a countdown-latch's count.
 *
 * @attention None.
 *
 * @param[in]   countdownlatch  pointer to the handler of the countdown-latch.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_countdownlatch_reset(k_countdownlatch_t *countdownlatch, k_countdownlatch_cnt_t count);

#endif

#endif

