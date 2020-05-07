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
#include <errno.h>

#include "qcloud_iot_import.h"
#include "qcloud_iot_export_log.h"
#include "qcloud_iot_export_error.h"
#include "qcloud_iot_common.h"

#include "tos_k.h"
#include "sal_module_wrapper.h"

#define PORT_BUFF_LEN	16

static uint32_t _time_left(uint32_t t_end, uint32_t t_now)
{
    uint32_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

uintptr_t HAL_TCP_Connect(const char *host, uint16_t port)
{
    int fd;
    char port_str[PORT_BUFF_LEN] = {0};
    
    snprintf(port_str, PORT_BUFF_LEN, "%u", port);
    Log_i("osal_tcp_connect entry, host=%s port=%d(%s)", host , port, port_str);

    fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_TCP);
    if (fd < 0) {
        Log_i("net connect fail\n\r");
        if (QCLOUD_RET_SUCCESS == tos_sal_module_init()) {
            Log_i("net reinit success\n\r");
            fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_TCP);
            if (fd < 0) {
                Log_i("net connect fail\n\r");
                return NULL;
            } else {
                Log_i("net connect success, fd=%d\n\r", fd);
            }
        } else {
            Log_i("net reinit fail\n\r");
            return NULL;
        }
    }

    return fd;
}

int HAL_TCP_Disconnect(uintptr_t fd)
{
    (void)tos_sal_module_close(fd);
    return QCLOUD_RET_SUCCESS;
}

int HAL_TCP_Write(uintptr_t fd, const unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *written_len)
{
    int ret;

    ret = tos_sal_module_send(fd, buf, len);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_WRITE_FAIL;
    }

    (*(int *)written_len) = ret;
    return QCLOUD_RET_SUCCESS;
}

int HAL_TCP_Read(uintptr_t fd, unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *read_len)
{
    int ret;

    ret = tos_sal_module_recv_timeout(fd, buf, len, timeout_ms);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_READ_FAIL;
    }

    if (ret == 0) {
        return QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }

    (*(int *)read_len) = ret;
    return QCLOUD_RET_SUCCESS;
}
