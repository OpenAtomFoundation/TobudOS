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

#include "tos_evtdrv.h"

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

__STATIC__ void evtdrv_timer_place(evtdrv_timer_t *tmr, evtdrv_tick_t timeout)
{
    TOS_CPU_CPSR_ALLOC();
    evtdrv_timer_t *curr_tmr;

    TOS_CPU_INT_DISABLE();
    tmr->expires = tos_evtdrv_systick_get() + timeout;

    TOS_LIST_FOR_EACH_ENTRY(curr_tmr, evtdrv_timer_t, list, &evtdrv_timer_list) {
        if (tmr->expires < curr_tmr->expires) {
            break;
        }
    }
    tos_list_add_tail(&tmr->list, &curr_tmr->list);

    TOS_CPU_INT_ENABLE();
}

__STATIC__ void evtdrv_timer_takeoff(evtdrv_timer_t *tmr)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_CPU_INT_DISABLE();
    tos_list_del(&tmr->list);
    TOS_CPU_INT_ENABLE();
}

__API__ evtdrv_err_t tos_evtdrv_timer_create(evtdrv_timer_t *tmr,
                                                                evtdrv_timer_callback_t callback,
                                                                void *cb_arg,
                                                                evtdrv_timer_opt_t opt)
{
    if (!tmr || !callback) {
        return EVTDRV_ERR_PTR_NULL;
    }

    tmr->cb = callback;
    tmr->opt = opt;
    tos_list_init(&tmr->list);

    return EVTDRV_ERR_NONE;
}

__API__ evtdrv_err_t tos_evtdrv_timer_start(evtdrv_timer_t *tmr, evtdrv_tick_t timeout)
{
    if (!tmr) {
        return EVTDRV_ERR_PTR_NULL;
    }

    if (!tos_list_empty(&tmr->list)) {
        return EVTDRV_ERR_TIMER_ALREADY_EXIST;
    }

    evtdrv_timer_place(tmr, timeout);
    if (tmr->opt == EVTDRV_TIMER_OPT_PERIODIC) {
        tmr->period = timeout;
    }

    return EVTDRV_ERR_NONE;
}

__API__ evtdrv_err_t tos_evtdrv_timer_stop(evtdrv_timer_t *tmr)
{
    if (!tmr) {
        return EVTDRV_ERR_PTR_NULL;
    }

    if (tos_list_empty(&tmr->list)) {
        return EVTDRV_ERR_TIMER_INACTIVE;
    }

    evtdrv_timer_takeoff(tmr);

    return EVTDRV_ERR_NONE;
}

__KNL__ void evtdrv_timer_update(void)
{
    TOS_CPU_CPSR_ALLOC();
    evtdrv_timer_t *tmr, *tmp;

    TOS_CPU_INT_DISABLE();

    TOS_LIST_FOR_EACH_ENTRY_SAFE(tmr, tmp, evtdrv_timer_t, list, &evtdrv_timer_list) {
        if (tmr->expires > tos_evtdrv_systick_get()) {
            break;
        }

        // time's up
        evtdrv_timer_takeoff(tmr);

        if (tmr->opt == EVTDRV_TIMER_OPT_PERIODIC) {
            evtdrv_timer_place(tmr, tmr->period);
        } else {
            evtdrv_timer_takeoff(tmr);
        }

        (*tmr->cb)(tmr->cb_arg);
    }

    TOS_CPU_INT_ENABLE();
}

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

