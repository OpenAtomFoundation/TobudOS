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

#ifndef _TOS_MUTEX_H_
#define  _TOS_MUTEX_H_

#if TOS_CFG_MUTEX_EN > 0u

typedef struct k_mutex_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t       knl_obj;
#endif

    pend_obj_t      pend_obj;
    k_nesting_t     pend_nesting;
    k_task_t       *owner;
    k_prio_t        owner_orig_prio;
    k_list_t        owner_anchor;
} k_mutex_t;

/**
 * @brief Create a mutex.
 * create a mutex.
 *
 * @attention None
 *
 * @param[in]   mutex       pointer to the handler of the mutex.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_mutex_create(k_mutex_t *mutex);

/**
 * @brief Destroy a mutex.
 * destroy a mutex.
 *
 * @attention None
 *
 * @param[in]   mutex       pointer to the handler of the mutex.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_mutex_destroy(k_mutex_t *mutex);

/**
 * @brief Pend a mutex.
 * pend a mutex.
 *
 * @attention The task will keep blocked until the mutex is obtained or a timeout comes.
 *
 * @param[in]   mutex       pointer to the handler of the mutex.
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_MUTEX_NESTING_OVERFLOW     we are the owner of the mutex, and we are nesting pend too much on this mutex.
 * @retval  #K_ERR_MUTEX_NESTING              we are the owner of the mutex, and we are nesting pend on it.
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_PEND_DESTROY               the mutex we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_mutex_pend_timed(k_mutex_t *mutex, k_tick_t timeout);

/**
 * @brief Pend a mutex.
 * pend a mutex.
 *
 * @attention The task will keep blocked until the mutex is obtained.
 *
 * @param[in]   mutex       pointer to the handler of the mutex.
 *
 * @return  errcode
 * @retval  #K_ERR_MUTEX_NESTING_OVERFLOW     we are the owner of the mutex, and we are nesting pend too much on this mutex.
 * @retval  #K_ERR_MUTEX_NESTING              we are the owner of the mutex, and we are nesting pend on it.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_DESTROY               the mutex we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_mutex_pend(k_mutex_t *mutex);

/**
 * @brief Post a mutex.
 * post a mutex.
 *
 * @attention None
 *
 * @param[in]   mutex       pointer to the handler of the mutex.
 *
 * @return  errcode
 * @retval  #K_ERR_MUTEX_NOT_OWNER            we are posting a mutex of which the owner is not us.
 * @retval  #K_ERR_MUTEX_NESTING              we are posting a mutex owned by us, and we are still in a nesting.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_mutex_post(k_mutex_t *mutex);

__KERNEL__ void mutex_release(k_mutex_t *mutex);

#endif

#endif /* _TOS_MUTEX_H_ */

