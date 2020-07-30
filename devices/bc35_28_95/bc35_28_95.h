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

#ifndef __BC35_28_95_H__
#define __BC35_28_95_H__

#include "tos_at.h"

typedef enum sal_net_mode {
    SAL_NET_MODE_STA,
    SAL_NET_MODE_AP,
    SAL_NET_MODE_STA_AP,
} sal_net_mode_t;

typedef enum sal_send_mode {
    SAL_SEND_MODE_NORMAL,
    SAL_SEND_MODE_TRANSPARENT,
} sal_send_mode_t;

typedef enum sal_multilink_state {
    SAL_MULTILINK_STATE_ENABLE,
    SAL_MULTILINK_STATE_DISABLE,
} sal_multilink_state_t;

int bc35_28_95_sal_init(hal_uart_port_t uart_port);

#endif /* __BC35_28_95_H__*/

