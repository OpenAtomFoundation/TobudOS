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

#include "errno.h"
#include "sched.h"
#include "private/pthread.h"
#include "private/time.h"

__API__ int sched_get_priority_max(int policy)
{
    return K_TASK_PRIO_IDLE - 1;
}

__API__ int sched_get_priority_min(int policy)
{
    return 0;
}

__API__ int sched_getparam(pid_t pid, struct sched_param *param)
{
    pthread_ctl_t *the_ctl;

    TOS_PTR_SANITY_CHECK_RC(param, EINVAL);

    the_ctl = pthread_ctl_by_id(pid);
    if (!the_ctl) {
        return EINVAL;
    }

    *param = the_ctl->attr.schedparam;

    return 0;
}

__NOTSUPP__ int sched_getscheduler(pid_t pid)
{
    return EOPNOTSUPP;
}

__API__ int sched_rr_get_interval(pid_t pid, struct timespec *interval)
{
#if TOS_CFG_ROUND_ROBIN_EN > 0u
    k_tick_t ktick;
    pthread_ctl_t *the_ctl;

    TOS_PTR_SANITY_CHECK_RC(interval, EINVAL);

    the_ctl = pthread_ctl_by_id(pid);
    if (!the_ctl) {
        return EINVAL;
    }

    ktick = the_ctl->the_ktask->timeslice_reload;
    ktick_to_timespec(ktick, interval);

    return 0;
#else
    return EOPNOTSUPP;
#endif
}

__API__ int sched_setparam(pid_t pid, const struct sched_param *param)
{
    pthread_ctl_t *the_ctl;

    TOS_PTR_SANITY_CHECK_RC(param, EINVAL);

    the_ctl = pthread_ctl_by_id(pid);
    if (!the_ctl) {
        return EINVAL;
    }

    the_ctl->attr.schedparam = *param;

    return 0;
}

__NOTSUPP__ int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param)
{
    return EOPNOTSUPP;
}

__API__ int sched_yield(void)
{
    tos_task_yield();

    return 0;
}

