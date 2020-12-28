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

#ifndef __M26_H__
#define __M26_H__

#include "tos_hal.h"

typedef enum m26_send_mode {
    M26_SEND_MODE_NORMAL,
    M26_SEND_MODE_TRANSPARENT,
} m26_send_mode_t;

typedef enum m26_multilink_state {
    M26_MULTILINK_STATE_ENABLE,
    M26_MULTILINK_STATE_DISABLE,
} m26_multilink_state_t;

int m26_sal_init(hal_uart_port_t uart_port);

#endif /* __M26_H__ */

