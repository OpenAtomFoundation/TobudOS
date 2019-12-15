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

#ifndef _TOS_EVTDRV_ERR_H_
#define  _TOS_EVTDRV_ERR_H_

typedef enum evtdrv_err_en {
    EVTDRV_ERR_NONE                 = 0x0u,
    EVTDRV_ERR_MEM_ALLOC_FAILED     = 0x1u,
    EVTDRV_ERR_PTR_NULL             = 0x2u,
    EVTDRV_ERR_MSG_BUSY             = 0x3u,
    EVTDRV_ERR_TASK_INVALID         = 0x4u,
    EVTDRV_ERR_MMHEAP_NOT_ENABLED   = 0x5u,

    EVTDRV_ERR_EVENT_INVALID         = 0x10u,
    EVTDRV_ERR_EVENT_OVERFLOW       = 0x11u,

    EVTDRV_ERR_TIMER_ALREADY_EXIST   = 0x20u,
    EVTDRV_ERR_TIMER_INACTIVE       = 0x21u,
} evtdrv_err_t;

#endif

