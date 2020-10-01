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

#ifndef __AIR724_H__
#define __AIR724_H__

#include "tos_hal.h"

typedef enum air724_send_mode {
    AIR724_SEND_MODE_NORMAL,
    AIR724_SEND_MODE_TRANSPARENT,
} air724_send_mode_t;

typedef enum air724_multilink_state {
    AIR724_MULTILINK_STATE_ENABLE,
    AIR724_MULTILINK_STATE_DISABLE,
} air724_multilink_state_t;

/* PWRKEY GPIO */
#define AIR724_USE_PWRKEY_GPIO   0
#define AIR724_PWR_GPIO_PORT     GPIOB
#define AIR724_PWR_GPIO_PIN      GPIO_PIN_5

int air724_sal_init(hal_uart_port_t uart_port);

#endif /* __AIR724_H__ */

