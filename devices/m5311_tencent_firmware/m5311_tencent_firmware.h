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

#ifndef __m5311_TENCENT_FIRMWARE_H__
#define __m5311_TENCENT_FIRMWARE_H__

#include "tos_hal.h"


int m5311_tencent_firmware_sal_init(hal_uart_port_t uart_port);
int m5311_tencent_firmware_join_ap(const char *ssid, const char *pwd);

#endif /* __m5311_TENCENT_FIRMWARE_H__ */

