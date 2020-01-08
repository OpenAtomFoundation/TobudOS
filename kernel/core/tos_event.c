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

#if TOS_CFG_EVENT_EN > 0

__API__ k_err_t tos_event_create(k_event_t *event, k_event_flag_t init_flag)
{
    TOS_PTR_SANITY_CHECK(event);

    event->flag = init_flag;
    pend_object_init(&event->pend_obj);
    TOS_OBJ_INIT(event, KNL_OBJ_TYPE_EVENT);

    return K_ERR_NONE;
}

__API__ k_err_t tos_event_destroy(k_event_t *event)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(event);
    TOS_OBJ_VERIFY(event, KNL_OBJ_TYPE_EVENT);

    TOS_CPU_INT_DISABLE();

    if (!pend_is_nopending(&event->pend_obj)) {
        pend_wakeup_all(&event->pend_obj, PEND_STATE_DESTROY);
    }

    event->flag = (k_event_flag_t)0u;

    pend_object_deinit(&event->pend_obj);

    TOS_OBJ_DEINIT(event);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__STATIC__ int event_is_match(k_event_flag_t event, k_event_flag_t flag_expect, k_event_flag_t *flag_match, k_opt_t opt_pend)
{
    if (opt_pend & TOS_OPT_EVENT_PEND_ALL) {
        if ((event & flag_expect) == flag_expect) {
            *flag_match = flag_expect;
            return K_TRUE;
        }
    } else if (opt_pend & TOS_OPT_EVENT_PEND_ANY) {
        if (event & flag_expect) {
            *flag_match = event & flag_expect;
            return K_TRUE;
        }
    }
    return K_FALSE;
}

__API__ k_err_t tos_event_pend(k_event_t *event, k_event_flag_t flag_expect, k_event_flag_t *flag_match, k_tick_t timeout, k_opt_t opt_pend)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_PTR_SANITY_CHECK(event);
    TOS_PTR_SANITY_CHECK(flag_match);
    TOS_OBJ_VERIFY(event, KNL_OBJ_TYPE_EVENT);

    if (!(opt_pend & TOS_OPT_EVENT_PEND_ALL) && !(opt_pend & TOS_OPT_EVENT_PEND_ANY)) {
        return K_ERR_EVENT_PEND_OPT_INVALID;
    }

    if ((opt_pend & TOS_OPT_EVENT_PEND_ALL) && (opt_pend & TOS_OPT_EVENT_PEND_ANY)) {
        return K_ERR_EVENT_PEND_OPT_INVALID;
    }

    TOS_CPU_INT_DISABLE();

    if (event_is_match(event->flag, flag_expect, flag_match, opt_pend)) {
        if (opt_pend & TOS_OPT_EVENT_PEND_CLR) { // destroy the bridge after get across the river
            event->flag = (k_event_flag_t)0u;
        }
        TOS_CPU_INT_ENABLE();
        return K_ERR_NONE;
    }

    if (timeout == TOS_TIME_NOWAIT) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_NOWAIT;
    }

    if (knl_is_inirq()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_IN_IRQ;
    }

    if (knl_is_sched_locked()) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_PEND_SCHED_LOCKED;
    }

    k_curr_task->flag_expect      = flag_expect;
    k_curr_task->flag_match       = flag_match;
    k_curr_task->opt_event_pend   = opt_pend;

    pend_task_block(k_curr_task, &event->pend_obj, timeout);

    TOS_CPU_INT_ENABLE();
    knl_sched();

    k_curr_task->flag_expect      = (k_event_flag_t)0u;
    k_curr_task->flag_match       = (k_event_flag_t *)K_NULL;
    k_curr_task->opt_event_pend   = (k_opt_t)0u;

    return pend_state2errno(k_curr_task->pend_state);
}

__STATIC__ k_err_t event_do_post(k_event_t *event, k_event_flag_t flag, opt_event_post_t opt_post)
{
    TOS_CPU_CPSR_ALLOC();
    k_task_t *task, *tmp;

    TOS_PTR_SANITY_CHECK(event);
    TOS_OBJ_VERIFY(event, KNL_OBJ_TYPE_EVENT);

    if (opt_post == OPT_EVENT_POST_KEP) {
        event->flag |= flag;
    } else {
        event->flag = flag;
    }

    TOS_CPU_INT_DISABLE();

    TOS_LIST_FOR_EACH_ENTRY_SAFE(task, tmp, k_task_t, pend_list, &event->pend_obj.list) {
        if (event_is_match(event->flag, task->flag_expect, task->flag_match, task->opt_event_pend)) {
            pend_task_wakeup(task, PEND_STATE_POST);

            // if anyone pending the event has set the TOS_OPT_EVENT_PEND_CLR, then no wakeup for the others pendig for the event.
            if (task->opt_event_pend & TOS_OPT_EVENT_PEND_CLR) {
                event->flag = (k_event_flag_t)0u;
                break;
            }
        }
    }

    TOS_CPU_INT_ENABLE();
    knl_sched();

    return K_ERR_NONE;
}

__API__ k_err_t tos_event_post(k_event_t *event, k_event_flag_t flag)
{
    return event_do_post(event, flag, OPT_EVENT_POST_CLR);
}

__API__ k_err_t tos_event_post_keep(k_event_t *event, k_event_flag_t flag)
{
    return event_do_post(event, flag, OPT_EVENT_POST_KEP);
}

#endif

