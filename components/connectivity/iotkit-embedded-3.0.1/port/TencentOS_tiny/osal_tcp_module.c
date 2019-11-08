/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <string.h>
#include "tos.h"
#include "sal_module_wrapper.h"

#define PORT_BUFF_LEN	16

int HAL_TCP_Establish(const char *host, uint16_t port)
{
    int fd;
    char port_str[PORT_BUFF_LEN];

    memset(port_str, 0, PORT_BUFF_LEN);
    snprintf(port_str, PORT_BUFF_LEN, "%u", port);
    printf("osal_tcp_connect entry, host=%s port=%d(%s)", host , port, port_str);

    fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_TCP);
    if (fd < 0) {
        printf("net connect fail\n\r");
        if (0 == tos_sal_module_init()) {
            printf("net reinit success\n\r");
            fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_TCP);
            if (fd < 0) {
                printf("net connect fail\n\r");
                return NULL;
            } else {
                printf("net connect success, fd=%d\n\r", fd);
            }
        } else {
            printf("net reinit fail\n\r");
            return NULL;
        }
    }

    return fd;
}

int HAL_TCP_Destroy(int fd)
{
    (void)tos_sal_module_close(fd);
    return 0;
}

int32_t HAL_TCP_Write(int fd, const void *buf, size_t len, uint32_t timeout)
{
    return tos_sal_module_send(fd, buf, len);
}

int32_t HAL_TCP_Read(int fd, void *buf, size_t len, uint32_t timeout)
{
    return tos_sal_module_recv_timeout(fd, buf, len, timeout);
}

