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
 * @file qcloud_iot_net_socket.c
 * @brief implements mbedtls net socket api for tls client
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-09
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-09 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_platform.h"
#include "mbedtls/net_sockets.h"

/**
 * @brief Convert return code from IotReturnCode to mbedtls error code
 *
 * @param[in] iot_return_code @see IotReturnCode
 * @return mbedtls error code
 */
static int _return_code_convert(int iot_return_code)
{
    switch (iot_return_code) {
        case QCLOUD_ERR_TCP_UNKNOWN_HOST:
            return MBEDTLS_ERR_NET_UNKNOWN_HOST;
        case QCLOUD_ERR_TCP_SOCKET_FAILED:
            return MBEDTLS_ERR_NET_SOCKET_FAILED;
        case QCLOUD_ERR_TCP_CONNECT:
            return MBEDTLS_ERR_NET_CONNECT_FAILED;
        case QCLOUD_ERR_TCP_WRITE_TIMEOUT:
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        case QCLOUD_ERR_TCP_WRITE_FAIL:
            return MBEDTLS_ERR_NET_SEND_FAILED;
        case QCLOUD_ERR_TCP_PEER_SHUTDOWN:
            return MBEDTLS_ERR_NET_CONN_RESET;
        case QCLOUD_ERR_TCP_READ_TIMEOUT:
            return MBEDTLS_ERR_SSL_TIMEOUT;
        case QCLOUD_ERR_TCP_NOTHING_TO_READ:
            return MBEDTLS_ERR_SSL_WANT_READ;
        case QCLOUD_ERR_TCP_READ_FAIL:
            return MBEDTLS_ERR_NET_RECV_FAILED;
        default:
            return -1;
    }
}

/**
 * @brief Init net context
 *
 * @param[in,out] ctx mbedtls net context handle
 */
void mbedtls_net_init(mbedtls_net_context *ctx)
{
    ctx->fd = -1;
}

/**
 * @brief Initiate a TCP connection with host:port and the given protocol
 *
 * @param[in,out] ctx mbedtls net context handle
 * @param[in] host host to connect
 * @param[in] port port to connect
 * @param[in] proto no use for always tcp
 * @return fd > 0 for success, others for mbedtls error code
 */
int mbedtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto)
{
    ctx->fd = HAL_TCP_Connect(host, port);
    return ctx->fd < 0 ? _return_code_convert(ctx->fd) : ctx->fd;
}

/**
 * @brief Read at most 'len' characters
 *
 * @param[in] ctx mbedtls net context handle
 * @param[out] buf data buffer
 * @param[in] len data buffer len
 * @return > 0 for data received bytes, others for mbedtls error code
 */
int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
    int    rc;
    size_t rlen;
    int    fd = ((mbedtls_net_context *)ctx)->fd;

    rc = HAL_TCP_Read(fd, buf, len, INT_MAX, &rlen);

    return rc ? _return_code_convert(rc) : rlen;
}

/**
 * @brief Read at most 'len' characters, blocking for at most 'timeout' ms
 *
 * @param[in] ctx mbedtls net context handle
 * @param[out] buf data buffer
 * @param[in] len data buffer len
 * @param[in] timeout read timeout
 * @return > 0 for data received bytes, others for mbedtls error code
 */
int mbedtls_net_recv_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    int    rc;
    size_t rlen;
    int    fd = ((mbedtls_net_context *)ctx)->fd;

    rc = HAL_TCP_Read(fd, buf, len, timeout, &rlen);

    return rc ? _return_code_convert(rc) : rlen;
}

/**
 * @brief Write at most 'len' characters
 *
 * @param[in] ctx mbedtls net context handle
 * @param[in] buf data buffer
 * @param[in] len data buffer len
 * @return > 0 for data writtern bytes, others for mbedtls error code
 */
int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
    int    rc;
    size_t wlen;
    int    fd = ((mbedtls_net_context *)ctx)->fd;

    rc = HAL_TCP_Write(fd, buf, len, INT_MAX, &wlen);

    return rc ? _return_code_convert(rc) : wlen;
}

/**
 * @brief Gracefully close the connection
 *
 * @param ctx mbedtls net context handle
 */
void mbedtls_net_free(mbedtls_net_context *ctx)
{
    HAL_TCP_Disconnect(ctx->fd);
}
