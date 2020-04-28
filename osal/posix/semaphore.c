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

#include "private/posix_config.h"

#include "errno.h"
#include "semaphore.h"
#include "private/time.h"

#if POSIX_CFG_SEM_EN > 0u

__NOTSUPP__ int sem_close(sem_t *sem)
{
    return EOPNOTSUPP;
}

__API__ int sem_destroy(sem_t *sem)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(sem, EINVAL);

    kerr = tos_sem_destroy((k_sem_t *)sem);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int sem_getvalue(sem_t *sem, int *sval)
{
    TOS_PTR_SANITY_CHECK_RC(sem, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(sval, EINVAL);

    *sval = ((k_sem_t *)sem)->count;
    return 0;
}

__API__ int sem_init(sem_t *sem, int pshared, unsigned value)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(sem, EINVAL);

    kerr = tos_sem_create((k_sem_t *)sem, value);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__NOTSUPP__ sem_t *sem_open(const char *name, int oflag, ...)
{
    return K_NULL;
}

__API__ int sem_post(sem_t *sem)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(sem, EINVAL);

    kerr = tos_sem_post((k_sem_t *)sem);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int sem_timedwait(sem_t *sem, const struct timespec *abstime)
{
    k_err_t kerr;
    k_tick_t ktick;

    TOS_PTR_SANITY_CHECK_RC(sem, EINVAL);


    ktick   = timespec_to_ktick(abstime);
    kerr    = tos_sem_pend((k_sem_t *)sem, ktick);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__API__ int sem_trywait(sem_t *sem)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(sem, EINVAL);

    kerr = tos_sem_pend((k_sem_t *)sem, TOS_TIME_NOWAIT);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__NOTSUPP__ int sem_unlink(const char *name)
{
    return EOPNOTSUPP;
}

__API__ int sem_wait(sem_t *sem)
{
    k_err_t kerr;

    TOS_PTR_SANITY_CHECK_RC(sem, EINVAL);

    kerr = tos_sem_pend((k_sem_t *)sem, TOS_TIME_FOREVER);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

#endif /* POSIX_CFG_SEM_EN */
