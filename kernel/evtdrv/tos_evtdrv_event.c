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

__API__ evtdrv_err_t tos_evtdrv_event_set(evtdrv_task_id_t task_id, evtdrv_event_flag_t event_flags)
{
    TOS_CPU_CPSR_ALLOC();
    uint8_t i = 0;
    evtdrv_event_t *event;
    evtdrv_event_flag_t flag;

    if (evtdrv_task_id_is_invalid(task_id)) {
        return EVTDRV_ERR_TASK_INVALID;
    }

    TOS_CPU_INT_DISABLE();

    event = &evtdrv_events[task_id];
    for (i = 0; i < sizeof(evtdrv_event_flag_t) * 8; ++i) {
        flag = (1 << i);

        if (event_flags & flag) {
            if (event->nesting[i] == (evtdrv_event_nesting_t)-1) {
                continue;
            }

            ++event->nesting[i];
            event->flags |= flag;
        }
    }

    TOS_CPU_INT_ENABLE();

    return EVTDRV_ERR_NONE;
}

__API__ evtdrv_err_t tos_evtdrv_event_reset(evtdrv_task_id_t task_id, evtdrv_event_flag_t event_flags)
{
    TOS_CPU_CPSR_ALLOC();
    uint8_t i = 0;
    evtdrv_event_t *event;
    evtdrv_event_flag_t flag;

    if (evtdrv_task_id_is_invalid(task_id)) {
        return EVTDRV_ERR_TASK_INVALID;
    }

    TOS_CPU_INT_DISABLE();

    event = &evtdrv_events[task_id];
    for (i = 0; i < sizeof(evtdrv_event_flag_t) * 8; ++i) {
        flag = (1 << i);

        if (event_flags & flag) {
            if (!(event->flags & flag)) {
                continue;
            }

            if (event->nesting[i] > (evtdrv_event_nesting_t)0u) {
                --event->nesting[i];
            }

            if (event->nesting[i] == (evtdrv_event_nesting_t)0u) {
                event->flags &= ~flag;
            }
        }
    }

    TOS_CPU_INT_ENABLE();

    return EVTDRV_ERR_NONE;
}

__KERNEL__ evtdrv_err_t evtdrv_event_init(void)
{
    evtdrv_task_id_t i;
    evtdrv_event_t *event;

    evtdrv_events = (evtdrv_event_t *)tos_mmheap_calloc(evtdrv_task_table_size, sizeof(evtdrv_event_t));
    if (!evtdrv_events) {
        return EVTDRV_ERR_MEM_ALLOC_FAILED;
    }

    for (i = 0; i < evtdrv_task_table_size; ++i) {
        event = &evtdrv_events[i];
        event->flags = TOS_EVTDRV_EVENT_NONE;
    }

    return EVTDRV_ERR_NONE;
}

__KERNEL__ evtdrv_event_flag_t evtdrv_event_fetch(evtdrv_task_id_t *task_id)
{
    evtdrv_task_id_t i;
    evtdrv_event_t *event;

    for (i = 0; i < evtdrv_task_table_size; ++i) {
        event = &evtdrv_events[i];
        if (event->flags != TOS_EVTDRV_EVENT_NONE) {
            if (task_id) {
                *task_id = i;
            }
            return event->flags;
        }
    }

    return TOS_EVTDRV_EVENT_NONE;
}

#endif

