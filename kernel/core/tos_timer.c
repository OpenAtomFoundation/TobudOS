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

#if TOS_CFG_TIMER_EN > 0u

__STATIC__ void timer_place(k_timer_t *tmr)
{
    TOS_CPU_CPSR_ALLOC();
    k_timer_t *iter = K_NULL;

    TOS_CPU_INT_DISABLE();

    tmr->expires += k_tick_count;

    TOS_LIST_FOR_EACH_ENTRY(iter, k_timer_t, list, &k_timer_ctl.list) {
        if (tmr->expires < iter->expires) {
            break;
        }
    }
    tos_list_add_tail(&tmr->list, &iter->list);

    if (k_timer_ctl.list.next == &tmr->list) {
        // we are the first guy now
        k_timer_ctl.next_expires = tmr->expires;

#if TOS_CFG_TIMER_AS_PROC == 0u
        if (task_state_is_sleeping(&k_timer_task)) {
            tos_task_delay_abort(&k_timer_task);
        }
#endif
    }

#if TOS_CFG_TIMER_AS_PROC == 0u
    if (task_state_is_suspended(&k_timer_task)) {
        tos_task_resume(&k_timer_task);
    }
#endif

    TOS_CPU_INT_ENABLE();
}

__STATIC__ void timer_takeoff(k_timer_t *tmr)
{
    TOS_CPU_CPSR_ALLOC();
    k_timer_t *first, *next;

    TOS_CPU_INT_DISABLE();

    first = TOS_LIST_FIRST_ENTRY(&k_timer_ctl.list, k_timer_t, list);

    tos_list_del(&tmr->list);

    if (first == tmr) {
        // if the first guy removed, we need to refresh k_timer_ctl.next_expires
        next = TOS_LIST_FIRST_ENTRY_OR_NULL(&tmr->list, k_timer_t, list);
        if (!next) {
            // the only guy removed
            k_timer_ctl.next_expires = TOS_TIME_FOREVER;
        } else {
            k_timer_ctl.next_expires = next->expires;
        }
    }

    TOS_CPU_INT_ENABLE();
}

__STATIC_INLINE__ void timer_reset(k_timer_t *tmr)
{
    tmr->state          = TIMER_STATE_UNUSED;
    tmr->delay          = (k_tick_t)0u;
    tmr->expires        = (k_tick_t)0u;
    tmr->period         = (k_tick_t)0u;
    tmr->opt            = (k_opt_t)0u;
    tmr->cb             = K_NULL;
    tmr->cb_arg         = K_NULL;
    tos_list_init(&tmr->list);

    TOS_OBJ_DEINIT(tmr);
}

__API__ k_err_t tos_timer_create(k_timer_t *tmr,
                                        k_tick_t delay,
                                        k_tick_t period,
                                        k_timer_callback_t callback,
                                        void *cb_arg,
                                        k_opt_t opt)
{
    TOS_PTR_SANITY_CHECK(tmr);
    TOS_PTR_SANITY_CHECK(callback);

    if (opt == TOS_OPT_TIMER_PERIODIC && period == (k_tick_t)0u) {
        return K_ERR_TIMER_INVALID_PERIOD;
    }

    if (opt == TOS_OPT_TIMER_ONESHOT && delay == (k_tick_t)0u) {
        // if you create a oneshot timer and delay 0 to trigger, why don't just call the timer_callback?
        return K_ERR_TIMER_INVALID_DELAY;
    }

    if (opt != TOS_OPT_TIMER_ONESHOT && opt != TOS_OPT_TIMER_PERIODIC) {
        return K_ERR_TIMER_INVALID_OPT;
    }

    if (delay == TOS_TIME_FOREVER) {
        return K_ERR_TIMER_DELAY_FOREVER;
    }

    if (period == TOS_TIME_FOREVER) {
        return K_ERR_TIMER_PERIOD_FOREVER;
    }

    tmr->state          = TIMER_STATE_STOPPED;
    tmr->delay          = delay;
    tmr->expires        = (k_tick_t)0u;
    tmr->period         = period;
    tmr->opt            = opt;
    tmr->cb             = callback;
    tmr->cb_arg         = cb_arg;
    tos_list_init(&tmr->list);

    TOS_OBJ_INIT(tmr, KNL_OBJ_TYPE_TIMER);

    return K_ERR_NONE;
}

__API__ k_err_t tos_timer_destroy(k_timer_t *tmr)
{
    TOS_PTR_SANITY_CHECK(tmr);
    TOS_OBJ_VERIFY(tmr, KNL_OBJ_TYPE_TIMER);

    if (tmr->state == TIMER_STATE_UNUSED) {
        return K_ERR_TIMER_INACTIVE;
    }

    if (tmr->state == TIMER_STATE_RUNNING) {
        timer_takeoff(tmr);
    }

    timer_reset(tmr);
    return K_ERR_NONE;
}

__API__ k_err_t tos_timer_start(k_timer_t *tmr)
{
    TOS_PTR_SANITY_CHECK(tmr);
    TOS_OBJ_VERIFY(tmr, KNL_OBJ_TYPE_TIMER);

    if (tmr->state == TIMER_STATE_UNUSED) {
        return K_ERR_TIMER_INACTIVE;
    }

    if (tmr->state == TIMER_STATE_RUNNING) {
        timer_takeoff(tmr);
        tmr->expires = tmr->delay;
        timer_place(tmr);
        return K_ERR_NONE;
    }

    if (tmr->state == TIMER_STATE_STOPPED ||
        tmr->state == TIMER_STATE_COMPLETED) {
        tmr->state = TIMER_STATE_RUNNING;
        if (tmr->delay == (k_tick_t)0u) {
            tmr->expires = tmr->period;
        } else {
            tmr->expires = tmr->delay;
        }
        timer_place(tmr);
        return K_ERR_NONE;
    }

    return K_ERR_TIMER_INVALID_STATE;
}

__API__ k_err_t tos_timer_stop(k_timer_t *tmr)
{
    TOS_PTR_SANITY_CHECK(tmr);
    TOS_OBJ_VERIFY(tmr, KNL_OBJ_TYPE_TIMER);

    if (tmr->state == TIMER_STATE_UNUSED) {
        return K_ERR_TIMER_INACTIVE;
    }

    if (tmr->state == TIMER_STATE_COMPLETED ||
        tmr->state == TIMER_STATE_STOPPED) {
        return K_ERR_TIMER_STOPPED;
    }

    if (tmr->state == TIMER_STATE_RUNNING) {
        tmr->state = TIMER_STATE_STOPPED;
        timer_takeoff(tmr);
    }

    return K_ERR_NONE;
}

__STATIC__ k_err_t timer_change(k_timer_t *tmr, k_tick_t new_val, timer_change_type_t change_type)
{
    TOS_PTR_SANITY_CHECK(tmr);
    TOS_OBJ_VERIFY(tmr, KNL_OBJ_TYPE_TIMER);

    if (tmr->state == TIMER_STATE_UNUSED) {
        return K_ERR_TIMER_INACTIVE;
    }

    if (tmr->state == TIMER_STATE_RUNNING) {
        return K_ERR_TIMER_RUNNING;
    }

    if (tmr->opt == TOS_OPT_TIMER_ONESHOT &&
        change_type == TIMER_CHANGE_TYPE_DELAY &&
        new_val == (k_tick_t)0u) {
        return K_ERR_TIMER_INVALID_DELAY;
    }

    if (tmr->opt == TOS_OPT_TIMER_PERIODIC &&
        change_type == TIMER_CHANGE_TYPE_PERIOD &&
        new_val == (k_tick_t)0u) {
        return K_ERR_TIMER_INVALID_PERIOD;
    }

    if (change_type == TIMER_CHANGE_TYPE_DELAY) {
        tmr->delay  = new_val;
    } else {
        tmr->period = new_val;
    }

    return K_ERR_NONE;
}

__API__ k_err_t tos_timer_delay_change(k_timer_t *tmr, k_tick_t delay)
{
    return timer_change(tmr, delay, TIMER_CHANGE_TYPE_DELAY);
}

__API__ k_err_t tos_timer_period_change(k_timer_t *tmr, k_tick_t period)
{
    return timer_change(tmr, period, TIMER_CHANGE_TYPE_PERIOD);
}

__KERNEL__ k_tick_t timer_next_expires_get(void)
{
    TOS_CPU_CPSR_ALLOC();
    k_tick_t next_expires;

    TOS_CPU_INT_DISABLE();

    if (k_timer_ctl.next_expires == TOS_TIME_FOREVER) {
        next_expires = TOS_TIME_FOREVER;
    } else if (k_timer_ctl.next_expires <= k_tick_count) {
        next_expires = (k_tick_t)0u;
    } else {
        next_expires = k_timer_ctl.next_expires - k_tick_count;
    }

    TOS_CPU_INT_ENABLE();
    return next_expires;
}

#if TOS_CFG_TIMER_AS_PROC > 0u

__KERNEL__ void timer_update(void)
{
    k_timer_t *tmr, *tmp;

    if (k_timer_ctl.next_expires < k_tick_count) {
        return;
    }

    tos_knl_sched_lock();

    TOS_LIST_FOR_EACH_ENTRY_SAFE(tmr, tmp, k_timer_t, list, &k_timer_ctl.list) {
        if (tmr->expires > k_tick_count) {
            break;
        }

        // time's up
        timer_takeoff(tmr);

        if (tmr->opt == TOS_OPT_TIMER_PERIODIC) {
            tmr->expires = tmr->period;
            timer_place(tmr);
        } else {
            tmr->state = TIMER_STATE_COMPLETED;
        }

        (*tmr->cb)(tmr->cb_arg);
    }

    tos_knl_sched_unlock();
}

#else /* TOS_CFG_TIMER_AS_PROC > 0u */

__STATIC__ void timer_task_entry(void *arg)
{
    k_timer_t *tmr, *tmp;
    k_tick_t next_expires;

    arg = arg; // make compiler happy
    while (K_TRUE) {
        next_expires = timer_next_expires_get();
        if (next_expires == TOS_TIME_FOREVER) {
            tos_task_suspend(K_NULL);
        } else if (next_expires > (k_tick_t)0u) {
            tos_task_delay(next_expires);
        }

        tos_knl_sched_lock();

        TOS_LIST_FOR_EACH_ENTRY_SAFE(tmr, tmp, k_timer_t, list, &k_timer_ctl.list) {
            if (tmr->expires > k_tick_count) { // not yet
                break;
            }

            // time's up
            timer_takeoff(tmr);

            if (tmr->opt == TOS_OPT_TIMER_PERIODIC) {
                tmr->expires = tmr->period;
                timer_place(tmr);
            } else {
                tmr->state = TIMER_STATE_COMPLETED;
            }

            (*tmr->cb)(tmr->cb_arg);
        }

        tos_knl_sched_unlock();
    }
}

#endif

__KERNEL__ k_err_t timer_init(void)
{
#if TOS_CFG_TIMER_AS_PROC > 0u
    return K_ERR_NONE;
#else
    return tos_task_create(&k_timer_task,
            "timer",
            timer_task_entry,
            K_NULL,
            k_timer_task_prio,
            k_timer_task_stk_addr,
            k_timer_task_stk_size,
            0);
#endif
}

#endif

