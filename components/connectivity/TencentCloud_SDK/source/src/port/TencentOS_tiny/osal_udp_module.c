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
#include "tos_k.h"
#include "sal_module_wrapper.h"
#include "qcloud.h"

#define PORT_BUFF_LEN	16

__QCLOUD_OSAL__ int osal_udp_connect(const char *host, uint16_t port)
{
    int fd;
	char port_str[PORT_BUFF_LEN];

	memset(port_str, 0, PORT_BUFF_LEN);
	snprintf(port_str, PORT_BUFF_LEN, "%u", port);
	QCLOUD_LOG_I("osal_udp_connect entry, host=%s port=%d(%s)", host , port, port_str);

	fd = tos_sal_module_connect("111.230.127.136", "5684", TOS_SAL_PROTO_UDP);
	if (fd < 0) {
		QCLOUD_LOG_I("net connect fail\n\r");
		if (QCLOUD_ERR_SUCCESS == tos_sal_module_init()) { /* 重新初始化模组 */
			QCLOUD_LOG_I("net reinit success\n\r");
			fd = tos_sal_module_connect(host, port_str, TOS_SAL_PROTO_UDP);
			if (fd < 0) {
				QCLOUD_LOG_I("net connect fail\n\r");
				return NULL;
			} else {
				QCLOUD_LOG_I("net connect success, fd=%d\n\r", fd);
			}
        } else {
			QCLOUD_LOG_I("net reinit fail\n\r");
			return NULL;
		}
	}

	return fd;
}

__QCLOUD_OSAL__ void osal_udp_disconnect(int sockfd)
{
    (void)tos_sal_module_close(sockfd);
}

__QCLOUD_OSAL__ qcloud_err_t osal_udp_write(int sockfd, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
    int ret;

    ret = tos_sal_module_sendto(sockfd, NULL, NULL, buf, len);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_WRITE_FAIL;
    }

    (*(int *)write_len) = ret;
    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_OSAL__ qcloud_err_t osal_udp_read(int sockfd, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
    int ret;

    QCLOUD_LOG_I("osal_udp_read len %d timeout %d\r\n", len);

    ret = tos_sal_module_recvfrom_timeout(sockfd, buf, len, timeout);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_READ_FAIL;
    }

    if (ret == 0) {
        return QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }

    *(int *)read_len = ret;
    return QCLOUD_ERR_SUCCESS;
}

