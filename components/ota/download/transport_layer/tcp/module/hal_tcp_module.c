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

#include "sal_module_wrapper.h"

int hal_tcp_connect(const char *host, uint16_t port)
{
    int fd;
#define PORT_MAX         6
    char port_str[PORT_MAX] = { 0 };

#define IP_MAX          16
    char ip[IP_MAX] = { 0 };

    snprintf(port_str, PORT_MAX, "%u", port);

    if (tos_sal_module_parse_domain(host, ip, sizeof(ip)) != 0) {
        return -1;
    }

    fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_TCP);
    if (fd >= 0) {
        return fd;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_TCP);
    if (fd < 0) {
        return -1;
    }

    return fd;
}

int hal_tcp_disconnect(int fd)
{
    (void)tos_sal_module_close(fd);
    return 0;
}

int hal_tcp_write(int fd, const unsigned char *buf, uint32_t len, uint32_t timeout_ms)
{
    return tos_sal_module_send(fd, buf, len);
}

int hal_tcp_read(uintptr_t fd, unsigned char *buf, uint32_t len, uint32_t timeout_ms)
{
    return tos_sal_module_recv_timeout(fd, buf, len, timeout_ms);
}

