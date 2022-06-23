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

#ifndef __ESP8266_TENCENT_FIRMWARE_H__
#define __ESP8266_TENCENT_FIRMWARE_H__

#include "tos_hal.h"
#include "tos_at.h"

typedef enum
{
    SAL_NET_MODE_STA,
    SAL_NET_MODE_AP,
    SAL_NET_MODE_STA_AP,
} sal_net_mode_t;

typedef enum
{
    SAL_SEND_MODE_NORMAL,
    SAL_SEND_MODE_TRANSPARENT,
} sal_send_mode_t;

int esp8266_tencent_firmware_sal_init(hal_uart_port_t uart_port);
void esp8266_tencent_firmware_sal_deinit(void);
int esp8266_tencent_firmware_join_ap(const char *ssid, const char *pwd);
int esp8266_tencent_firmware_set_urc(const char *urc, at_urc_callback_t urc_handle);
int esp8266_tencent_firmware_raw_data_send(const void *data, int data_len);
int esp8266_tencent_firmware_at_cmd_exec_until(const char *at_cmd, const char *at_expect, uint32_t timeout_ms);
int esp8266_tencent_firmware_at_cmd_exec_recv(const char *at_cmd, const char *at_expect, void *recv_buffer, uint32_t *read_len, uint32_t timeout_ms);

#endif /* __ESP8266_TENCENT_FIRMWARE_H__ */
