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

#include "private/posix_config.h"

#include "errno.h"
#include "time.h"
#include "private/time.h"
#include "private/timer.h"

__API__ clock_t clock(void)
{
    return tos_systick_get();
}

__API__ int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    k_tick_t ktick;

    TOS_PTR_SANITY_CHECK_RC(tp, EINVAL);

    if (clock_id != CLOCK_REALTIME) {
        return EINVAL;
    }

    /* use systick rather than rtc now */
    ktick = tos_systick_get();

    ktick_to_timespec(ktick, tp);

    return 0;
}

#if POSIX_CFG_TIMER_EN > 0u

__STATIC__ void timer_callback(void *arg)
{
    ptimer_ctl_t *the_ctl;

    the_ctl = (ptimer_ctl_t *)arg;

    the_ctl->sigev_notify_function(the_ctl->sigev_value);
}

__API__ int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid)
{
    k_err_t kerr;
    timer_t id;
    ptimer_ctl_t *the_ctl;

    TOS_PTR_SANITY_CHECK_RC(timerid, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(evp, EINVAL);
    TOS_PTR_SANITY_CHECK_RC(evp->sigev_notify_function, EINVAL);

    id = timer_id_alloc();
    if (id == -1) {
        return ENOMEM;
    }

    clockid = clockid; /* make compiler happy */

    the_ctl = (ptimer_ctl_t *)tos_mmheap_alloc(sizeof(ptimer_ctl_t));
    if (!the_ctl) {
        return ENOMEM;
    }

    the_ctl->sigev_notify_function      = evp->sigev_notify_function;
    the_ctl->sigev_value                = evp->sigev_value;

    kerr = tos_timer_create((k_timer_t *)&the_ctl->ktimer, 1u, 1u,
                                timer_callback, the_ctl,
                                TOS_OPT_TIMER_PERIODIC);
    if (kerr != K_ERR_NONE) {
        tos_mmheap_free(the_ctl);
        return EBUSY;
    }


    the_ctl->id = id;
    timer_id_add(id, the_ctl);

    return 0;
}

__API__ int timer_delete(timer_t timerid)
{
    k_err_t kerr;
    ptimer_ctl_t *the_ctl;

    TOS_PTR_SANITY_CHECK_RC(timerid, EINVAL);

    the_ctl = timer_by_id(timerid);
    if (!the_ctl) {
        return EINVAL;
    }

    kerr = tos_timer_destroy(&the_ctl->ktimer);
    timer_id_free(timerid);
    tos_mmheap_free(the_ctl);

    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__NOTSUPP__ int timer_getoverrun(timer_t timerid)
{
    return EOPNOTSUPP;
}

__API__ int timer_gettime(timer_t timerid, struct itimerspec *value)
{
    k_tick_t expires, period;
    ptimer_ctl_t *the_ctl;

    the_ctl = timer_by_id(timerid);
    if (!the_ctl) {
        return EINVAL;
    }

    if (!value) {
        return 0;
    }

    expires = the_ctl->ktimer.expires;
    period  = the_ctl->ktimer.period;

    ktick_to_timespec(expires, &value->it_value);
    ktick_to_timespec(period, &value->it_interval);

    return 0;
}

__API__ int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue)
{
    k_tick_t delay, period;
    ptimer_ctl_t *the_ctl;

    TOS_PTR_SANITY_CHECK_RC(value, EINVAL);

    the_ctl = timer_by_id(timerid);
    if (!the_ctl) {
        return EINVAL;
    }

    if (ovalue) {
        timer_gettime(timerid, ovalue);
    }

    delay   = timespec_to_ktick(&value->it_value);
    period  = timespec_to_ktick(&value->it_interval);

    tos_timer_stop(&the_ctl->ktimer);
    tos_timer_delay_change(&the_ctl->ktimer, delay);
    tos_timer_period_change(&the_ctl->ktimer, period);
    tos_timer_start(&the_ctl->ktimer);

    return 0;
}

#endif /* POSIX_CFG_TIMER_EN */

