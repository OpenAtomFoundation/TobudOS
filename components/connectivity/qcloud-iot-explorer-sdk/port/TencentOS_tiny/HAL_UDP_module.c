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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "qcloud_iot_import.h"
#include "qcloud_iot_export_log.h"
#include "qcloud_iot_export_error.h"

#include "tos.h"
#include "sal_module_wrapper.h"

#define PORT_BUFF_LEN	16

#ifdef COAP_COMM_ENABLED

uintptr_t HAL_UDP_Connect(const char *host, unsigned short port)
{
    int fd;
	char port_str[PORT_BUFF_LEN];

	memset(port_str, 0, PORT_BUFF_LEN);
	snprintf(port_str, PORT_BUFF_LEN, "%u", port);
	Log_i("osal_udp_connect entry, host=%s port=%d(%s)", host , port, port_str);

	fd = tos_sal_module_connect("111.230.127.136", "5684", TOS_SAL_PROTO_UDP);
	if (fd < 0) {
		Log_i("net connect fail\n\r");
		if (QCLOUD_RET_SUCCESS == tos_sal_module_init()) { /* 重新初始化模组 */
			Log_i("net reinit success\n\r");
			fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_UDP);
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

void HAL_UDP_Disconnect(uintptr_t fd)
{
    (void)tos_sal_module_close(fd);
}

int HAL_UDP_Write(uintptr_t fd, const unsigned char *p_data, unsigned int datalen)
{
    int ret;

    ret = tos_sal_module_sendto(fd, NULL, NULL, p_data, datalen);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_WRITE_FAIL;
    }

    return QCLOUD_RET_SUCCESS;
}

int HAL_UDP_Read(uintptr_t fd, unsigned char *p_data, unsigned int datalen)
{
    int ret;

    Log_i("osal_udp_read len %d timeout %d\r\n", datalen);

    ret = tos_sal_module_recvfrom(fd, p_data, datalen);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_READ_FAIL;
    }

    if (ret == 0) {
        return QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }

    return QCLOUD_RET_SUCCESS;
}

int HAL_UDP_ReadTimeout(uintptr_t fd, unsigned char *p_data, unsigned int datalen, unsigned int timeout_ms)
{
    int ret;

    Log_i("osal_udp_read len %d timeout %d\r\n", datalen);

    ret = tos_sal_module_recvfrom_timeout(fd, p_data, datalen, timeout_ms);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_READ_FAIL;
    }

    if (ret == 0) {
        return QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }

    return QCLOUD_RET_SUCCESS;
}

#endif
