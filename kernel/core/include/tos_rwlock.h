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

#ifndef _TOS_RWLOCK_H_
#define  _TOS_RWLOCK_H_

__CDECLS_BEGIN

#if (TOS_CFG_SEM_EN > 0u) && (TOS_CFG_MUTEX_EN > 0u)

typedef uint16_t    rw_cnt_t;

typedef struct k_rwlock_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t       knl_obj;
#endif

    k_mutex_t       lock;
    k_sem_t         signal;

    rw_cnt_t        n_readers;  /* how many readers are reading? */
    rw_cnt_t        n_writers;  /* how many writers are waiting to obtain the wlock? */
    int             is_writting;
} k_rwlock_t;

/**
 * @brief Create a read-write lock.
 *
 * @attention a read-write lock can be hold by multi-readers, that means simultaneously reading is allowed;
 *            but a read-write lock can only be hold by one writes, that means simultaneously writting or read while writting is not allowed.
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_rwlock_create(k_rwlock_t *rwlock);

/**
 * @brief Destroy a read-write lock.
 *
 * @attention
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_rwlock_destroy(k_rwlock_t *rwlock);

/**
 * @brief Pend on the read-lock of a read-write lock.
 *
 * @attention if one reader already hold the read-lock, other reader can hold the read-lock simultaneously.
 *              and no writers can hold the write-lock.
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_READERS_TO_MANY   too many reader are holding the read-lock
 */
__API__ k_err_t tos_rwlock_rpend_timed(k_rwlock_t *rwlock, k_tick_t timeout);

/**
 * @brief Pend on the read-lock of a read-write lock.
 *
 * @attention if one reader already hold the read-lock, other reader can hold the read-lock simultaneously.
 *              and no writers can hold the write-lock.
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_READERS_TO_MANY   too many reader are holding the read-lock
 */
__API__ k_err_t tos_rwlock_rpend(k_rwlock_t *rwlock);

/**
 * @brief Try pend on the read-lock of a read-write lock.
 *
 * @attention Try means just take a look, if can obtain the read-lock, then we obtain it; otherwise, just return with no-waiting.
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_IS_WRITTING       the read-write lock is hold by a writter(is writting).
 */
__API__ k_err_t tos_rwlock_rpend_try(k_rwlock_t *rwlock);

/**
 * @brief Pend on the write-lock of a read-write lock.
 *
 * @attention if one writer already hold the write-lock, other writer CANNOT hold the write-lock any more.
 *              and no readers can hold the read-lock.
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                             return successfully.
 * @retval  #K_ERR_RWLOCK_WAITING_WRITERS_TO_MANY   too many writers are waiting for the write-lock
 */
__API__ k_err_t tos_rwlock_wpend_timed(k_rwlock_t *rwlock, k_tick_t timeout);

/**
 * @brief Pend on the write-lock of a read-write lock.
 *
 * @attention if one writer already hold the write-lock, other writer CANNOT hold the write-lock any more.
 *              and no readers can hold the read-lock.
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_WAITING_WRITERS_TO_MANY   too many writers are waiting for the write-lock
 */
__API__ k_err_t tos_rwlock_wpend(k_rwlock_t *rwlock);

/**
 * @brief Pend on the write-lock of a read-write lock.
 *
 * @attention Try means just take a look, if can obtain the write-lock, then we obtain it; otherwise, just return with no-waiting.
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_IS_READING        the read-write lock is hold by other reader[s](is reading).
 * @retval  #K_ERR_RWLOCK_IS_WRITTING       the read-write lock is hold by another writter(is writting).
 */
__API__ k_err_t tos_rwlock_wpend_try(k_rwlock_t *rwlock);

/**
 * @brief Post the read-lock of a read-write lock.
 *
 * @attention
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_NOT_READING       the read-lock is not held by reader[s].
 */
__API__ k_err_t tos_rwlock_rpost(k_rwlock_t *rwlock);

/**
 * @brief Post the write-lock of a read-write lock.
 *
 * @attention
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_NOT_WRITTING      the write-lock is not held by a writter.
 */
__API__ k_err_t tos_rwlock_wpost(k_rwlock_t *rwlock);

/**
 * @brief Post the read&write-lock of a read-write lock.
 *
 * @attention
 *
 * @param[in]   rwlock              the read-write lock.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 * @retval  #K_ERR_RWLOCK_NOT_TAKEN         the read-write lock is neither held by reader[s] nor held by a writter.
 */
__API__ k_err_t tos_rwlock_post(k_rwlock_t *rwlock);

#endif

__CDECLS_END

#endif /* _TOS_RWLOCK_H_ */

