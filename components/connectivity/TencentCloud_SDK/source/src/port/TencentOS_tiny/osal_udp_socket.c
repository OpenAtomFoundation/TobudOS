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
#include "qcloud.h"

#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

__QCLOUD_OSAL__ int osal_udp_connect(const char *host, uint16_t port)
{
#define NETWORK_ADDR_LEN    (16)

    int ret;
    int fd = 0;
    char port_str[6] = {0};
    struct addrinfo hints, *addr_list, *cur;

    osal_snprintf(port_str, 6, "%d", port);

    memset((char *)&hints, 0x00, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;

    QCLOUD_LOG_D("establish tcp connection with server(host=%s port=%s)", host, port_str);

    if (getaddrinfo(host, port_str, &hints, &addr_list) != 0) {
        QCLOUD_LOG_E("getaddrinfo error,errno:%s", strerror(errno));
        return 0;
    }

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            ret = 0;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            ret = fd;
            break;
        }

        close(fd);
        ret = 0;
    }

    if (ret == 0) {
        QCLOUD_LOG_E("fail to establish udp");
    } else {
        QCLOUD_LOG_D("success to establish udp, fd=%d", ret);
    }

    freeaddrinfo(addr_list);

    return fd;
}

__QCLOUD_OSAL__ void osal_udp_disconnect(int sockfd)
{
    close(sockfd);
}

__QCLOUD_OSAL__ qcloud_err_t osal_udp_write(int sockfd, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
    int ret;

    ret = send(sockfd, (char *)buf, (int)len, 0);
    if (ret < 0) {
        return QCLOUD_ERR_UDP_WRITE_FAIL;
    }

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_OSAL__ qcloud_err_t osal_udp_read(int sockfd, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;

    if (sockfd < 0) {
        return QCLOUD_ERR_TCP_READ_FAIL;
    }

    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    ret = select(sockfd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);

    /* Zero fds ready means we timed out */
    if (ret == 0) {
        return QCLOUD_ERR_UDP_READ_TIMEOUT;    /* receive timeout */
    } else if (ret < 0) {
        if (errno == EINTR) {
            return QCLOUD_ERR_UDP_READ_FAIL;    /* want read */
        }

        return QCLOUD_ERR_UDP_READ_TIMEOUT; /* receive failed */
    }

    ret = read(sockfd, buf, len);
    if (ret > 0) {
        return QCLOUD_ERR_SUCCESS;
    } else {
        return QCLOUD_ERR_UDP_READ_FAIL;
    }
}

