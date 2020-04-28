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

#include "tos_k.h"

#if (TOS_CFG_SEM_EN > 0u) && (TOS_CFG_MUTEX_EN > 0u)

__API__ k_err_t tos_rwlock_create(k_rwlock_t *rwlock)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(rwlock);

    err = tos_sem_create(&rwlock->signal, 0u);
    if (err != K_ERR_NONE) {
        return err;
    }

    err = tos_mutex_create(&rwlock->lock);
    if (err != K_ERR_NONE) {
        tos_sem_destroy(&rwlock->signal);
        return err;
    }

    rwlock->n_readers       = (rw_cnt_t)0u;
    rwlock->n_writers       = (rw_cnt_t)0u;
    rwlock->is_writting     = K_FALSE;
    TOS_OBJ_INIT(rwlock, KNL_OBJ_TYPE_RWLOCK);

    return K_ERR_NONE;
}

__API__ k_err_t tos_rwlock_destroy(k_rwlock_t *rwlock)
{
    k_err_t err0, err1;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    err0 = tos_sem_destroy(&rwlock->signal);
    err1 = tos_mutex_destroy(&rwlock->lock);

    rwlock->n_readers       = (rw_cnt_t)0u;
    rwlock->n_writers       = (rw_cnt_t)0u;
    rwlock->is_writting     = K_FALSE;
    TOS_OBJ_DEINIT(rwlock);

    if (err0 != K_ERR_NONE) {
        return err0;
    }

    return err1;
}

__API__ k_err_t tos_rwlock_rpend_timed(k_rwlock_t *rwlock, k_tick_t timeout)
{
    k_err_t err;
    k_stopwatch_t stopwatch;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    if (timeout != TOS_TIME_FOREVER) {
        tos_stopwatch_create(&stopwatch);
        tos_stopwatch_countdown(&stopwatch, timeout);
    }

    err = tos_mutex_pend_timed(&rwlock->lock, timeout);
    if (err != K_ERR_NONE) {
        return err;
    }

    if (rwlock->n_readers == (rw_cnt_t)-1) {
        /* number of reader reachs limit */
        return K_ERR_RWLOCK_READERS_TO_MANY;
    }

    if (rwlock->n_writers == 0u && !rwlock->is_writting) {
        /* no writer is now holding or waiting to hold the lock */
        ++rwlock->n_readers;
        tos_mutex_post(&rwlock->lock);
        return K_ERR_NONE;
    }

    if (timeout != TOS_TIME_FOREVER) {
        timeout = tos_stopwatch_remain(&stopwatch);
        if (timeout == 0u) {
            timeout = 1u;
        }
    }

    while (rwlock->n_writers > 0u || rwlock->is_writting) {
        /* util no one is writting or waiting to hold the lock */
        err = tos_sem_pend(&rwlock->signal, timeout);
        if (err != K_ERR_NONE) {
            break;
        }

        if (timeout != TOS_TIME_FOREVER) {
            timeout = tos_stopwatch_remain(&stopwatch);
            if (timeout == 0u) {
                err = K_ERR_PEND_TIMEOUT;
                break;
            }
        }
    }

    tos_mutex_post(&rwlock->lock);
    return err;
}

__API__ k_err_t tos_rwlock_rpend(k_rwlock_t *rwlock)
{
    return tos_rwlock_rpend_timed(rwlock, TOS_TIME_FOREVER);
}

__API__ k_err_t tos_rwlock_rpend_try(k_rwlock_t *rwlock)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    err = tos_mutex_pend_timed(&rwlock->lock, TOS_TIME_NOWAIT);
    if (err != K_ERR_NONE) {
        return err;
    }

    if (rwlock->n_readers == (rw_cnt_t)-1) {
        /* number of reader reachs limit */
        return K_ERR_RWLOCK_READERS_TO_MANY;
    }

    if (rwlock->n_writers == 0u && !rwlock->is_writting) {
        /* no writer is holding or waiting to hold the lock */
        ++rwlock->n_readers;
        tos_mutex_post(&rwlock->lock);
        return K_ERR_NONE;
    }

    /* the rwlock is held by other writters */
    tos_mutex_post(&rwlock->lock);
    return K_ERR_RWLOCK_IS_WRITTING;
}

__API__ k_err_t tos_rwlock_wpend_timed(k_rwlock_t *rwlock, k_tick_t timeout)
{
    k_err_t err;
    k_stopwatch_t stopwatch;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    if (timeout != TOS_TIME_FOREVER) {
        tos_stopwatch_create(&stopwatch);
        tos_stopwatch_countdown(&stopwatch, timeout);
    }

    err = tos_mutex_pend_timed(&rwlock->lock, timeout);
    if (err != K_ERR_NONE) {
        return err;
    }

    if (rwlock->n_writers == (rw_cnt_t)-1) {
        /* number of waitting writer reachs limit */
        return K_ERR_RWLOCK_WAITING_WRITERS_TO_MANY;
    }

    ++rwlock->n_writers;

    if (timeout != TOS_TIME_FOREVER) {
        timeout = tos_stopwatch_remain(&stopwatch);
        if (timeout == 0u) {
            timeout = 1u;
        }
    }

    while (rwlock->n_readers > 0u || rwlock->is_writting) {
        /* until no one is writting or reading */
        err = tos_sem_pend(&rwlock->signal, timeout);
        if (err != K_ERR_NONE) {
            break;
        }

        if (timeout != TOS_TIME_FOREVER) {
            timeout = tos_stopwatch_remain(&stopwatch);
            if (timeout == 0u) {
                err = K_ERR_PEND_TIMEOUT;
                break;
            }
        }
    }

    if (err == K_ERR_NONE) {
        /* we hold the wlock now */
        rwlock->is_writting = K_TRUE;
    } else {
        tos_sem_post_all(&rwlock->signal);
    }

    --rwlock->n_writers;

    tos_mutex_post(&rwlock->lock);
    return err;
}

__API__ k_err_t tos_rwlock_wpend(k_rwlock_t *rwlock)
{
    return tos_rwlock_wpend_timed(rwlock, TOS_TIME_FOREVER);
}

__API__ k_err_t tos_rwlock_wpend_try(k_rwlock_t *rwlock)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    err = tos_mutex_pend_timed(&rwlock->lock, TOS_TIME_NOWAIT);
    if (err != K_ERR_NONE) {
        return err;
    }

    if (rwlock->n_readers > 0u) {
        err = K_ERR_RWLOCK_IS_READING;
    } else if (rwlock->is_writting) {
        err = K_ERR_RWLOCK_IS_WRITTING;
    } else {
        rwlock->is_writting = K_TRUE;
    }

    tos_mutex_post(&rwlock->lock);
    return err;
}

__API__ k_err_t tos_rwlock_rpost(k_rwlock_t *rwlock)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    err = tos_mutex_pend(&rwlock->lock);
    if (err != K_ERR_NONE) {
        return err;
    }

    if (rwlock->n_readers == 0u) {
        err = K_ERR_RWLOCK_NOT_READING;
    } else {
        --rwlock->n_readers;
        if (rwlock->n_readers == 0u) {
            err = tos_sem_post_all(&rwlock->signal);
        }
    }

    tos_mutex_post(&rwlock->lock);
    return err;
}

__API__ k_err_t tos_rwlock_wpost(k_rwlock_t *rwlock)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    err = tos_mutex_pend(&rwlock->lock);
    if (err != K_ERR_NONE) {
        return err;
    }

    if (!rwlock->is_writting) {
        err = K_ERR_RWLOCK_NOT_WRITTING;
    } else {
        rwlock->is_writting = K_FALSE;
        err = tos_sem_post_all(&rwlock->signal);
    }

    tos_mutex_post(&rwlock->lock);
    return err;
}

__API__ k_err_t tos_rwlock_post(k_rwlock_t *rwlock)
{
    k_err_t err;

    TOS_PTR_SANITY_CHECK(rwlock);
    TOS_OBJ_VERIFY(rwlock, KNL_OBJ_TYPE_RWLOCK);

    err = tos_mutex_pend(&rwlock->lock);
    if (err != K_ERR_NONE) {
        return err;
    }

    if (rwlock->n_readers > 0u) {
        --rwlock->n_readers;
        if (rwlock->n_readers == 0u) {
            err = tos_sem_post_all(&rwlock->signal);
        }
    } else if (rwlock->is_writting) {
        rwlock->is_writting = K_FALSE;
        err = tos_sem_post_all(&rwlock->signal);
    } else {
        err = K_ERR_RWLOCK_NOT_TAKEN;
    }

    tos_mutex_post(&rwlock->lock);
    return err;
}

#endif

