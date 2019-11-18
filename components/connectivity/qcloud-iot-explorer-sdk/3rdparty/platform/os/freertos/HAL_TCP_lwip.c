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

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"

#include "qcloud_iot_import.h"
#include "qcloud_iot_export_log.h"
#include "qcloud_iot_export_error.h"
#include "qcloud_iot_common.h"

/* lwIP socket handle start from 0 */
#define LWIP_SOCKET_FD_SHIFT 3


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
	int ret;
	struct addrinfo hints, *addr_list, *cur;
    int fd = 0;

    char port_str[6];
    HAL_Snprintf(port_str, 6, "%d", port);

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;    

    ret = getaddrinfo(host, port_str, &hints, &addr_list);
    if (ret) {
        Log_e("getaddrinfo(%s:%s) error", host, port_str);
        return 0;
    }

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
    	fd = (int) socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
		if( fd < 0 )
		{
			ret = 0;
			continue;
		}

		if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0)
		{
			ret = fd + LWIP_SOCKET_FD_SHIFT;
			break;
		}

		close( fd );
		ret = 0;
    }

    if (ret == 0) {
        Log_e("failed to connect with TCP server: %s:%s", host, port_str);
    } else {
        /* reduce log print due to frequent log server connect/disconnect */
        if (0 == strncmp(host, LOG_UPLOAD_SERVER_DOMAIN, HOST_STR_LENGTH))
            UPLOAD_DBG("connected with TCP server: %s:%s", host, port_str);
        else
            Log_i("connected with TCP server: %s:%s", host, port_str);
    }

    freeaddrinfo(addr_list);

    return (uintptr_t)ret;
}


int HAL_TCP_Disconnect(uintptr_t fd)
{
    int rc;

    fd -= LWIP_SOCKET_FD_SHIFT;

    /* Shutdown both send and receive operations. */
    rc = shutdown((int) fd, 2);
    if (0 != rc) {       
		Log_e("shutdown error: %s", strerror(errno));
        return -1;
    }

    rc = close((int) fd);
    if (0 != rc) {
		Log_e("closesocket error: %s", strerror(errno));
        return -1;
    }

    return 0;
}


int HAL_TCP_Write(uintptr_t fd, const unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *written_len)
{
    int ret;
    uint32_t len_sent;
    uint32_t t_end, t_left;
    fd_set sets;

    fd -= LWIP_SOCKET_FD_SHIFT;

    t_end = HAL_GetTimeMs() + timeout_ms;
    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    do {
        t_left = _time_left(t_end, HAL_GetTimeMs());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = select(fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(fd, &sets)) {
                    Log_e("Should NOT arrive");
                    /* If timeout in next loop, it will not sent any data */
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                ret = QCLOUD_ERR_TCP_WRITE_TIMEOUT;
                Log_e("select-write timeout %d", (int)fd);
                break;
            } else {
                if (EINTR == errno) {
                    Log_e("EINTR be caught");
                    continue;
                }

                ret = QCLOUD_ERR_TCP_WRITE_FAIL;               
				Log_e("select-write fail: %s", strerror(errno));
                break;
            }
        }
        else {
        	ret = QCLOUD_ERR_TCP_WRITE_TIMEOUT;
        }

        if (ret > 0) {
            ret = send(fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                Log_e("No data be sent. Should NOT arrive");
            } else {
                if (EINTR == errno) {
                    Log_e("EINTR be caught");
                    continue;
                }

                ret = QCLOUD_ERR_TCP_WRITE_FAIL;                
				Log_e("send fail: %s", strerror(errno));
                break;
            }
        }
    } while ((len_sent < len) && (_time_left(t_end, HAL_GetTimeMs()) > 0));

    *written_len = (size_t)len_sent;

    return len_sent > 0 ? QCLOUD_RET_SUCCESS : ret;
}


int HAL_TCP_Read(uintptr_t fd, unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *read_len)
{
    int ret, err_code;
    uint32_t len_recv;
    uint32_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;

    
    fd -= LWIP_SOCKET_FD_SHIFT;
    t_end = HAL_GetTimeMs() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    do {
        t_left = _time_left(t_end, HAL_GetTimeMs());
        if (0 == t_left) {
        	err_code = QCLOUD_ERR_TCP_READ_TIMEOUT;
            break;
        }

        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeout.tv_sec = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0) {
            ret = recv(fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (0 == ret) {
                struct sockaddr_in peer;
                socklen_t sLen = sizeof(peer);
                int peer_port = 0;
                getpeername(fd, (struct sockaddr*)&peer, &sLen);
                peer_port = ntohs(peer.sin_port);

                /* reduce log print due to frequent log server connect/disconnect */
                if (peer_port == LOG_UPLOAD_SERVER_PORT)
    				UPLOAD_DBG("connection is closed by server: %s:%d", inet_ntoa(peer.sin_addr), peer_port);
                else
                    Log_e("connection is closed by server: %s:%d", inet_ntoa(peer.sin_addr), peer_port);
                
                err_code = QCLOUD_ERR_TCP_PEER_SHUTDOWN;
                break;
            } else {
                if (EINTR == errno) {
                    Log_e("EINTR be caught");
                    continue;
                }              
				Log_e("recv error: %s", strerror(errno));
                err_code = QCLOUD_ERR_TCP_READ_FAIL;
                break;
            }
        } else if (0 == ret) {
            err_code = QCLOUD_ERR_TCP_READ_TIMEOUT;
            break;
        } else {
			Log_e("select-recv error: %s", strerror(errno));
            err_code = QCLOUD_ERR_TCP_READ_FAIL;
            break;
        }
    } while ((len_recv < len));

    *read_len = (size_t)len_recv;

    if (err_code == QCLOUD_ERR_TCP_READ_TIMEOUT && len_recv == 0)
        err_code = QCLOUD_ERR_TCP_NOTHING_TO_READ;

    return (len == len_recv) ? QCLOUD_RET_SUCCESS : err_code;
}
