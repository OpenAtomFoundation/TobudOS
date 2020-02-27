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

#ifndef _TOS_SEM_H_
#define  _TOS_SEM_H_

__CDECLS_BEGIN

#if TOS_CFG_SEM_EN > 0u

typedef struct k_sem_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t       knl_obj;
#endif

    pend_obj_t      pend_obj;
    k_sem_cnt_t     count;
    k_sem_cnt_t     count_max;
} k_sem_t;

/**
 * @brief Create a semaphore with a limitation of maximum count.
 * create a semaphore with a limitation of maximum count.
 *
 * @attention None
 *
 * @param[in]   sem         pointer to the handler of the semaphore.
 * @param[in]   init_count  initial count of the semaphore.
 * @param[in]   max_count   maximum count of the semaphore.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_sem_create_max(k_sem_t *sem, k_sem_cnt_t init_count, k_sem_cnt_t max_count);

/**
 * @brief Create a semaphore.
 * create a semaphore.
 *
 * @attention None
 *
 * @param[in]   sem         pointer to the handler of the semaphore.
 * @param[in]   init_count  initial count of the semaphore.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_sem_create(k_sem_t *sem, k_sem_cnt_t init_count);

/**
 * @brief Destroy a semaphore.
 * destroy a semaphore.
 *
 * @attention None
 *
 * @param[in]   semaphore   pointer to the handler of the semaphore.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_sem_destroy(k_sem_t *sem);

/**
 * @brief Pend a semaphore.
 * pend a semaphore.
 *
 * @attention None
 *
 * @param[in]   sem         pointer to the handler of the semaphore.
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_PEND_DESTROY               the semaphore we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_sem_pend(k_sem_t *sem, k_tick_t timeout);

/**
 * @brief Post a semaphore.
 * post a semaphore and wakeup one pending task.
 *
 * @attention when tos_sem_post return successfully, only one task who are waitting for the semaphore will be woken up.
 *
 * @param[in]   sem     pointer to the handler of the semaphore.
 *
 * @return  errcode
 * @retval  #K_ERR_SEM_OVERFLOW               we are nesting post a semaphore too much.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_sem_post(k_sem_t *sem);

/**
 * @brief Post a semaphore.
 * post a semaphore and wakeup all the pending task.
 *
 * @attention when tos_sem_post_all return successfully, all of the tasks who are waitting for the semaphore will be woken up.
 *
 * @param[in]   sem     pointer to the handler of the semaphore.
 *
 * @return  errcode
 * @retval  #K_ERR_SEM_OVERFLOW               we are nesting post a semaphore too much.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_sem_post_all(k_sem_t *sem);

#endif

__CDECLS_END

#endif /* _TOS_SEM_H_ */

