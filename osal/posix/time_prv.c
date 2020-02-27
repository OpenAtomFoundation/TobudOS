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

#include "time.h"
#include "private/time.h"

__KNL__ k_tick_t timespec_to_ktick(const struct timespec *tp)
{
    int nsecond, second;
    struct timespec curr_tp;

    TOS_PTR_SANITY_CHECK_RC(tp, (k_tick_t)-1);

    clock_gettime(CLOCK_REALTIME, &curr_tp);

    if (tp->tv_nsec - curr_tp.tv_nsec < 0) {
        nsecond = NANOSECOND_PER_SECOND - (curr_tp.tv_nsec - tp->tv_nsec);
        second  = tp->tv_sec - curr_tp.tv_sec - 1;
    } else {
        nsecond = tp->tv_nsec - curr_tp.tv_nsec;
        second  = tp->tv_sec - curr_tp.tv_sec;
    }

    if (second < 0) {
        return (k_tick_t)0u;
    }

    return (k_tick_t)(second * TOS_CFG_CPU_TICK_PER_SECOND + nsecond * TOS_CFG_CPU_TICK_PER_SECOND / NANOSECOND_PER_SECOND);
}

__KNL__ void ktick_to_timespec(k_tick_t ktick, struct timespec *tp)
{
    if (!tp) {
        return;
    }

    tp->tv_sec  = ktick / TOS_CFG_CPU_TICK_PER_SECOND;
    tp->tv_nsec = (ktick % TOS_CFG_CPU_TICK_PER_SECOND) * ((long)1000000000 / TOS_CFG_CPU_TICK_PER_SECOND);
}

