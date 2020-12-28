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

#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "tos_hal.h"

typedef enum esp8266_net_mode {
    ESP8266_NET_MODE_STA,
    ESP8266_NET_MODE_AP,
    ESP8266_NET_MODE_STA_AP,
} esp8266_net_mode_t ;

typedef enum esp8266_send_mode {
    ESP8266_SEND_MODE_NORMAL,
    ESP8266_SEND_MODE_TRANSPARENT,
} esp8266_send_mode_t;

typedef enum esp8266_multilink_state {
    ESP8266_MULTILINK_STATE_ENABLE,
    ESP8266_MULTILINK_STATE_DISABLE,
} esp8266_multilink_state_t;

int esp8266_sal_init(hal_uart_port_t uart_port);

int esp8266_join_ap(const char *ssid, const char *pwd);

int esp8266_sal_deinit(void);

#endif /* __ESP8266_H__ */

