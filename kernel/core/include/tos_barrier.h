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

#ifndef _TOS_BARRIER_H_
#define  _TOS_BARRIER_H_

__CDECLS_BEGIN

typedef struct k_barrier_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t               knl_obj;
#endif

    pend_obj_t              pend_obj;
    k_barrier_cnt_t         count;
} k_barrier_t;

/**
 * @brief Create a thread barrier.
 *
 * @attention the count must be greater then zero.
 *
 * @param[in]   barrier         the barrier.
 * @param[in]   count           the number of threads(task) must call tos_barrier_pend before any of them successfully return from the call.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_BARRIER_COUNT_INVALID    the count is equals to zero.
 */
__API__ k_err_t tos_barrier_create(k_barrier_t *barrier, k_barrier_cnt_t count);

/**
 * @brief Create a thread barrier.
 *
 * @attention the count must be greater then zero.
 *
 * @param[in]   barrier         the barrier.
 * @param[in]   count           the number of threads(task) must call tos_barrier_pend before any of them successfully return from the call.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_BARRIER_COUNT_INVALID    the count is equals to zero.
 */
__API__ k_err_t tos_barrier_destroy(k_barrier_t *barrier);

/**
 * @brief Pend on a barrier.
 *
 * @attention until (countdownlatch->count) of tasks have called the pend, the pender would wake up.
 *
 * @param[in]   barrier         the barrier.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_BARRIER_OVERFLOW         the barrier is pended too many times.
 */
__API__ k_err_t tos_barrier_pend(k_barrier_t *barrier);

/**
 * @brief Reset a barrier.
 *
 * @attention
 *
 * @param[in]   barrier         the barrier.
 * @param[in]   count           the count of the barrier to be reset.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_barrier_reset(k_barrier_t *barrier, k_barrier_cnt_t count);

__CDECLS_END

#endif /* _TOS_BARRIER_H_ */

