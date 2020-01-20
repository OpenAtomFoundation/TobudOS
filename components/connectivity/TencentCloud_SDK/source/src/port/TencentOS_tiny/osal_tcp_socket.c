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

__QCLOUD_OSAL__ int osal_tcp_connect(const char *host, uint16_t port)
{
    int ret;
    int fd = 0;
    char port_str[6];
    struct addrinfo hints, *addr_list, *cur;

    osal_snprintf(port_str, 6, "%d", port);

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    ret = getaddrinfo(host, port_str, &hints, &addr_list);
    if (ret) {
        QCLOUD_LOG_E("getaddrinfo(%s:%s) error: %s", host, port_str, strerror(errno));
        return -1;
    }

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            ret = -1;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            ret = fd;
            break;
        }

        close(fd);
        ret = -1;
    }

    if (ret == -1) {
        QCLOUD_LOG_E("fail to connect: %s:%s", host, port_str);
    }

    freeaddrinfo(addr_list);

    return ret;
}

__QCLOUD_OSAL__ int osal_tcp_disconnect(int fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = shutdown(fd, 2);
    if (rc != 0) {
		QCLOUD_LOG_E("shutdown error: %s", strerror(errno));
        return -1;
    }

    rc = close(fd);
    if (rc != 0) {
		QCLOUD_LOG_E("closesocket error: %s", strerror(errno));
        return -1;
    }

    return 0;
}

__QCLOUD_OSAL__ qcloud_err_t osal_tcp_write(int fd, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
    int ret;
    qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    uint32_t len_sent = 0;
    fd_set sets;
    uint32_t time_remain;
    osal_timer_t timer;
    struct timeval tv;

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, timeout);

    do {
        if (osal_timer_is_expired(&timer)) {
            rc = QCLOUD_ERR_TCP_WRITE_TIMEOUT;
            break;
        }

        time_remain = osal_timer_remain(&timer);

        tv.tv_sec = time_remain / 1000;
        tv.tv_usec = (time_remain % 1000) * 1000;

        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        ret = select(fd + 1, NULL, &sets, NULL, &tv);
        if (ret > 0) {
            if (FD_ISSET(fd, &sets) == 0) {
                QCLOUD_LOG_E("Should NOT arrive");
                /* If timeout in next loop, it will not sent any data */
                rc = QCLOUD_ERR_TCP_NOTHING_TO_READ;
                continue;
            }
        } else if (ret == 0) {
            rc = QCLOUD_ERR_TCP_WRITE_TIMEOUT;
            QCLOUD_LOG_E("select-write timeout %d", fd);
            break;
        } else {
            if (errno == EINTR) {
                QCLOUD_LOG_E("EINTR be caught");
                continue;
            }

            rc = QCLOUD_ERR_TCP_WRITE_FAIL;
			QCLOUD_LOG_E("select-write fail: %s", strerror(errno));
            break;
        }

        ret = send(fd, (uint8_t *)buf + len_sent, len - len_sent, 0);
        if (ret > 0) {
            len_sent += ret;
        } else if (ret == 0) {
            QCLOUD_LOG_E("No data be sent. Should NOT arrive");
        } else {
            if (errno == EINTR) {
                QCLOUD_LOG_E("EINTR be caught");
                continue;
            }

            ret = QCLOUD_ERR_TCP_WRITE_FAIL;
			QCLOUD_LOG_E("send fail: %s", strerror(errno));
            break;
        }
    } while (len_sent < len);

    *write_len = (size_t)len_sent;

    return len_sent > 0 ? QCLOUD_ERR_SUCCESS : rc;
}

__QCLOUD_OSAL__ qcloud_err_t osal_tcp_read(int fd, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
    int ret;
    qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    uint32_t len_recv = 0;
    uint32_t time_remain;
    fd_set sets;
    struct timeval tv;
    osal_timer_t timer;
    struct sockaddr_in peer;
    socklen_t sLen = sizeof(peer);
    int peer_port = 0;

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, timeout);

    do {
        if (osal_timer_is_expired(&timer)) {
        	rc = QCLOUD_ERR_TCP_READ_TIMEOUT;
            break;
        }

        time_remain = osal_timer_remain(&timer);

        tv.tv_sec = time_remain / 1000;
        tv.tv_usec = (time_remain % 1000) * 1000;

        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        ret = select(fd + 1, &sets, NULL, NULL, &tv);
        if (ret > 0) {
            ret = recv(fd, (uint8_t *)buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (ret == 0) {
                getpeername(fd, (struct sockaddr*)&peer, &sLen);
                peer_port = ntohs(peer.sin_port);
                QCLOUD_LOG_E("connection is closed by server: %s:%d", inet_ntoa(peer.sin_addr), peer_port);

                rc = QCLOUD_ERR_TCP_PEER_SHUTDOWN;
                break;
            } else {
                if (EINTR == errno) {
                    QCLOUD_LOG_E("EINTR be caught");
                    continue;
                }

				QCLOUD_LOG_E("recv error: %s", strerror(errno));
                rc = QCLOUD_ERR_TCP_READ_FAIL;
                break;
            }
        } else if (ret == 0) {
            rc = QCLOUD_ERR_TCP_READ_TIMEOUT;
            break;
        } else {
			QCLOUD_LOG_E("select-recv error: %s", strerror(errno));
            rc = QCLOUD_ERR_TCP_READ_FAIL;
            break;
        }
    } while ((len_recv < len));

    *read_len = (size_t)len_recv;

    if (rc == QCLOUD_ERR_TCP_READ_TIMEOUT && len_recv == 0) {
        rc = QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }

    return (len == len_recv) ? QCLOUD_ERR_SUCCESS : rc;
}

