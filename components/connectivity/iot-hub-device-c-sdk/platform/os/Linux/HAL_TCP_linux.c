/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file HAL_TCP_linux.c
 * @brief Linux tcp api
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-09 <td>1.1     <td>fancyxu   <td>refactor for support tls, change port to str format
 * </table>
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "qcloud_iot_platform.h"

/**
 * @brief TCP connect in linux
 *
 * @param[in] host host to connect
 * @param[out] port port to connect
 * @return socket fd
 */
int HAL_TCP_Connect(const char *host, const char *port)
{
    // to avoid process crash when writing to a broken socket
    signal(SIGPIPE, SIG_IGN);

    int rc;
    int fd = 0;

    struct addrinfo hints, *addr_list, *cur;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    rc = getaddrinfo(host, port, &hints, &addr_list);
    if (rc) {
        Log_e("getaddrinfo(%s:%s) error: %s", STRING_PTR_PRINT_SANITY_CHECK(host), STRING_PTR_PRINT_SANITY_CHECK(port),
              rc == EAI_SYSTEM ? strerror(errno) : gai_strerror(rc));
        return QCLOUD_ERR_TCP_UNKNOWN_HOST;
    }

    for (cur = addr_list; cur; cur = cur->ai_next) {
        fd = (int)socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            rc = QCLOUD_ERR_TCP_SOCKET_FAILED;
            continue;
        }

        rc = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
        if (rc) {
            Log_e("set socket non block faliled %d", rc);
            close(fd);
            rc = QCLOUD_ERR_TCP_SOCKET_FAILED;
            continue;
        }

        rc = connect(fd, cur->ai_addr, cur->ai_addrlen);
        if (!rc) {
            rc = fd;
            break;
        }

        if (errno == EINPROGRESS) {
            // IO select to wait for connect result
            struct timeval timeout;
            timeout.tv_sec  = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT / 1000;
            timeout.tv_usec = 0;

            fd_set sets;
            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            rc = select(fd + 1, NULL, &sets, NULL, &timeout);
            if (rc > 0) {
                int       so_error;
                socklen_t len = sizeof(so_error);
                getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
                if (FD_ISSET(fd, &sets) && so_error == 0) {
                    rc = fd;
                    break;
                }
            }
        }

        close(fd);
        rc = QCLOUD_ERR_TCP_CONNECT;
    }

    freeaddrinfo(addr_list);
    return rc;
}

/**
 * @brief TCP disconnect
 *
 * @param[in] fd socket fd
 * @return 0 for success
 */
int HAL_TCP_Disconnect(int fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = shutdown(fd, 2);
    if (rc) {
        Log_e("shutdown error: %s", strerror(errno));
    }

    rc = close(fd);
    if (rc) {
        Log_e("closesocket error: %s", strerror(errno));
        return -1;
    }

    return 0;
}

/**
 * @brief TCP write
 *
 * @param[in] fd socket fd
 * @param[in] buf buf to write
 * @param[in] len buf len
 * @param[in] timeout_ms timeout
 * @param[out] written_len data written length
 * @return @see IotReturnCode
 */
int HAL_TCP_Write(int fd, const uint8_t *buf, uint32_t len, uint32_t timeout_ms, size_t *written_len)
{
    int            rc = 0;
    uint32_t       len_sent;
    Timer          timer_send;
    fd_set         sets;
    struct timeval timeout;

    HAL_Timer_CountdownMs(&timer_send, timeout_ms);
    len_sent = 0;

    /* send one time if timeout_ms is value 0 */
    while ((len_sent < len) && !HAL_Timer_Expired(&timer_send)) {
        timeout.tv_sec  = HAL_Timer_Remain(&timer_send) / 1000;
        timeout.tv_usec = HAL_Timer_Remain(&timer_send) % 1000 * 1000;

        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        rc = select(fd + 1, NULL, &sets, NULL, &timeout);
        if (!rc) {
            rc = QCLOUD_ERR_TCP_WRITE_TIMEOUT;
            Log_e("select-write timeout %d", (int)fd);
            break;
        }

        if (rc < 0) {
            if (EINTR != errno) {
                rc = QCLOUD_ERR_TCP_WRITE_FAIL;
                Log_e("select-write fail: %s", strerror(errno));
                break;
            }
            Log_e("EINTR be caught");
            continue;
        }

        rc = send(fd, buf + len_sent, len - len_sent, 0);
        if (rc < 0) {
            if (EINTR == errno) {
                Log_e("EINTR be caught");
                continue;
            }
            rc = (EPIPE == errno || ECONNRESET == errno) ? QCLOUD_ERR_TCP_PEER_SHUTDOWN : QCLOUD_ERR_TCP_WRITE_FAIL;
            Log_e("send fail: %s", strerror(errno));
            break;
        }

        len_sent += rc;
    }

    *written_len = (size_t)len_sent;
    // We always know hom much should write.
    return len_sent == len ? QCLOUD_RET_SUCCESS : rc;
}

/**
 * @brief TCP read.
 *
 * @param[in] fd socket fd
 * @param[out] buf buffer to save read data
 * @param[in] len buffer len
 * @param[in] timeout_ms timeout
 * @param[out] read_len length of data read
 * @return @see IotReturnCode
 */
int HAL_TCP_Read(int fd, uint8_t *buf, uint32_t len, uint32_t timeout_ms, size_t *read_len)
{
    int            rc;
    uint32_t       len_recv;
    Timer          timer_recv;
    fd_set         sets;
    struct timeval timeout;

    HAL_Timer_CountdownMs(&timer_recv, timeout_ms);
    len_recv = 0;

    do {
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeout.tv_sec  = HAL_Timer_Remain(&timer_recv) / 1000;
        timeout.tv_usec = HAL_Timer_Remain(&timer_recv) % 1000 * 1000;

        rc = select(fd + 1, &sets, NULL, NULL, &timeout);
        if (!rc) {
            rc = QCLOUD_ERR_TCP_READ_TIMEOUT;
            break;
        }

        if (rc < 0) {
            if (EINTR != errno) {
                rc = QCLOUD_ERR_TCP_READ_FAIL;
                Log_e("select-recv fail: %s", strerror(errno));
                break;
            }
            Log_e("EINTR be caught");
            continue;
        }

        rc = recv(fd, buf + len_recv, len - len_recv, 0);
        if (rc <= 0) {
            if (!rc) {
                Log_e("connection is closed by server");
                rc = QCLOUD_ERR_TCP_PEER_SHUTDOWN;
                break;
            }

            if (EINTR == errno) {
                Log_e("EINTR be caught");
                continue;
            }
            Log_e("recv error: %s", strerror(errno));
            rc = (EPIPE == errno || ECONNRESET == errno) ? QCLOUD_ERR_TCP_PEER_SHUTDOWN : QCLOUD_ERR_TCP_READ_FAIL;
            break;
        }
        len_recv += rc;
    } while (len_recv < len);

    *read_len = (size_t)len_recv;

    if (rc == QCLOUD_ERR_TCP_READ_TIMEOUT && len_recv == 0) {
        rc = QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }
    // We always don't know hom much should read.
    return (len_recv > 0) ? QCLOUD_RET_SUCCESS : rc;
}
