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

__API__ evtdrv_msg_body_t tos_evtdrv_msg_alloc(evtdrv_msg_len_t msg_len)
{
    evtdrv_msg_hdr_t *msg_hdr = K_NULL;

    if (!msg_len) {
        return K_NULL;
    }

    msg_hdr = (evtdrv_msg_hdr_t *)tos_mmheap_alloc(EVTDRV_MSG_LEN(msg_len));
    if (!msg_hdr) {
        return K_NULL;
    }

    tos_list_init(&msg_hdr->list);
    msg_hdr->len            = msg_len;
    msg_hdr->dst_task_id    = TOS_EVTDRV_TASK_ID_NONE;

    return EVTDRV_MSG_HDR2BODY(msg_hdr);
}

__API__ evtdrv_err_t tos_evtdrv_msg_free(evtdrv_msg_body_t msg_body)
{
    evtdrv_msg_hdr_t *msg_hdr = K_NULL;

    if (!msg_body) {
        return EVTDRV_ERR_PTR_NULL;
    }

    msg_hdr = EVTDRV_MSG_BODY2HDR(msg_body);
    if (msg_hdr->dst_task_id != TOS_EVTDRV_TASK_ID_NONE ||
        !tos_list_empty(&msg_hdr->list)) {
        return EVTDRV_ERR_MSG_BUSY;
    }

    tos_mmheap_free(msg_hdr);

    return EVTDRV_ERR_NONE;
}

__API__ evtdrv_err_t tos_evtdrv_msg_send(evtdrv_task_id_t dst_task_id, evtdrv_msg_body_t msg_body)
{
    evtdrv_msg_hdr_t *msg_hdr = K_NULL;

    if (!msg_body) {
        return EVTDRV_ERR_PTR_NULL;
    }

    if (evtdrv_task_id_is_invalid(dst_task_id)) {
        tos_evtdrv_msg_free(msg_body);
        return EVTDRV_ERR_TASK_INVALID;
    }

    msg_hdr = EVTDRV_MSG_BODY2HDR(msg_body);
    msg_hdr->dst_task_id = dst_task_id;
    tos_list_add(&msg_hdr->list, &evtdrv_msg_list);

    tos_evtdrv_event_set(dst_task_id, TOS_EVTDRV_SYS_EVENT_MSG);

    return EVTDRV_ERR_NONE;
}

__STATIC_INLINE__ void evtdrv_msg_prepare4use(evtdrv_msg_hdr_t *msg_hdr)
{
    tos_list_del_init(&msg_hdr->list);
    msg_hdr->dst_task_id = TOS_EVTDRV_TASK_ID_NONE;
}

__API__ evtdrv_msg_body_t tos_evtdrv_msg_recv(void)
{
    TOS_CPU_CPSR_ALLOC();
    k_list_t *curr, *next;
    evtdrv_msg_hdr_t *msg_hdr = K_NULL;

    TOS_CPU_INT_DISABLE();

    TOS_LIST_FOR_EACH_SAFE(curr, next, &evtdrv_msg_list) {
        msg_hdr = TOS_LIST_ENTRY(curr, evtdrv_msg_hdr_t, list);
        if (!evtdrv_task_is_self(msg_hdr->dst_task_id)) {
            continue;
        }

        evtdrv_msg_prepare4use(msg_hdr);
        tos_evtdrv_event_set(tos_evtdrv_task_self(), TOS_EVTDRV_SYS_EVENT_MSG);

        TOS_CPU_INT_ENABLE();
        return EVTDRV_MSG_HDR2BODY(msg_hdr);
    }

    TOS_CPU_INT_ENABLE();

    tos_evtdrv_event_reset(tos_evtdrv_task_self(), TOS_EVTDRV_SYS_EVENT_MSG);

    return K_NULL;
}

__KERNEL__ void evtdrv_msg_init(void)
{
    tos_list_init(&evtdrv_msg_list);
}

#endif

