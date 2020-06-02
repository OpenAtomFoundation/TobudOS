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

#ifndef _HAL_TCP_H_
#define  _HAL_TCP_H_

int hal_tcp_connect(const char *host, uint16_t port);

int hal_tcp_disconnect(int fd);

int hal_tcp_write(int fd, const unsigned char *buf, uint32_t len, uint32_t timeout_ms);

int hal_tcp_read(uintptr_t fd, unsigned char *buf, uint32_t len, uint32_t timeout_ms);

#endif /* _HAL_TCP_H_ */

