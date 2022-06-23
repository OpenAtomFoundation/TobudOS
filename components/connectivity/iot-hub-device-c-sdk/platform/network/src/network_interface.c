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
 * @file network_interface.c
 * @brief network interface for tcp/tls
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-09 <td>1.1     <td>fancyxu   <td>support tls
 * </table>
 */

#include "network_interface.h"

/**
 * @brief TCP init, do nothing.
 *
 * @param[in,out] network pointer to network handle
 * @return always ok
 */
static int _network_tcp_init(IotNetwork *network)
{
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief TCP connect.
 *
 * @param[in,out] network pointer to network handle
 * @return @see IotReturnCode
 */
static int _network_tcp_connect(IotNetwork *network)
{
    POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    network->fd = HAL_TCP_Connect(network->host, network->port);

    if (network->fd < 0) {
        Log_e("fail to connect with TCP server: %s:%s", STRING_PTR_PRINT_SANITY_CHECK(network->host),
              STRING_PTR_PRINT_SANITY_CHECK(network->port));
        return network->fd;
    }

    if (!strstr(network->host, "gateway.tencentdevices.com") &&
        !strstr(network->host, "devicelog.iot.cloud.tencent.com")) {  // reduce log info
        Log_i("connected with TCP server: %s:%s", STRING_PTR_PRINT_SANITY_CHECK(network->host),
              STRING_PTR_PRINT_SANITY_CHECK(network->port));
    }

    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief TCP read.
 *
 * @param[in,out] network pointer to network handle
 * @param[out] data data buffer to be read in
 * @param[in] datalen data buffer len
 * @param[in] timeout_ms read timeout
 * @param[out] read_len read data len
 * @return @see IotReturnCode
 */
static int _network_tcp_read(IotNetwork *network, unsigned char *data, size_t datalen, uint32_t timeout_ms,
                             size_t *read_len)
{
    POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    return HAL_TCP_Read(network->fd, data, (uint32_t)datalen, timeout_ms, read_len);
}

/**
 * @brief TCP write.
 *
 * @param[in,out] network pointer to network handle
 * @param[out] data data buffer to write
 * @param[in] datalen data buffer len
 * @param[in] timeout_ms write timeout
 * @param[out] written_len len of written data
 * @return @see IotReturnCode
 */
static int _network_tcp_write(IotNetwork *network, unsigned char *data, size_t datalen, uint32_t timeout_ms,
                              size_t *written_len)
{
    POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    return HAL_TCP_Write(network->fd, data, datalen, timeout_ms, written_len);
}

/**
 * @brief TCP disconnect
 *
 * @param[in,out] network pointer to network handle
 */
static void _network_tcp_disconnect(IotNetwork *network)
{
    POINTER_SANITY_CHECK_RTN(network);

    if (network->fd < 0) {
        return;
    }

    HAL_TCP_Disconnect(network->fd);
    network->fd = -1;
    return;
}

/**
 * @brief Return handle.
 *
 * @param[in] network pointer to network
 * @return handle of network
 */
static int _is_network_tcp_connected(IotNetwork *network)
{
    return network->fd > 0;
}

#ifndef AUTH_WITH_NO_TLS

/**
 * @brief TLS init, do nothing.
 *
 * @param[in,out] network pointer to network handle
 * @return always ok
 */
static int _network_tls_init(IotNetwork *network)
{
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief TLS connect.
 *
 * @param[in,out] network pointer to network handle
 * @return @see IotReturnCode
 */
static int _network_tls_connect(IotNetwork *network)
{
    POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);
    network->handle = qcloud_iot_tls_client_connect(&network->ssl_connect_params, network->host, network->port);
    return network->handle ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_SSL_CONNECT;
}

/**
 * @brief TLS read.
 *
 * @param[in,out] network pointer to network handle
 * @param[out] data data buffer to be read in
 * @param[in] datalen data buffer len
 * @param[in] timeout_ms read timeout
 * @param[out] read_len read data len
 * @return @see IotReturnCode
 */
static int _network_tls_read(IotNetwork *network, unsigned char *data, size_t datalen, uint32_t timeout_ms,
                             size_t *read_len)
{
    POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);
    return qcloud_iot_tls_client_read(network->handle, data, datalen, timeout_ms, read_len);
}

/**
 * @brief TLS write.
 *
 * @param[in,out] network pointer to network handle
 * @param[out] data data buffer to write
 * @param[in] datalen data buffer len
 * @param[in] timeout_ms write timeout
 * @param[out] written_len len of written data
 * @return @see IotReturnCode
 */
static int _network_tls_write(IotNetwork *network, unsigned char *data, size_t datalen, uint32_t timeout_ms,
                              size_t *written_len)
{
    POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);
    return qcloud_iot_tls_client_write(network->handle, data, datalen, timeout_ms, written_len);
}

/**
 * @brief TLS disconnect
 *
 * @param[in,out] network pointer to network handle
 */
static void _network_tls_disconnect(IotNetwork *network)
{
    POINTER_SANITY_CHECK_RTN(network);

    qcloud_iot_tls_client_disconnect(network->handle);
    network->handle = 0;
}

/**
 * @brief Return handle.
 *
 * @param[in] network pointer to network
 * @return handle of network
 */
static int _is_network_tls_connected(IotNetwork *network)
{
    return network->handle;
}

#endif

/**
 * @brief Init network, support tcp, tls(if AUTH_WITH_NO_TLS defined).
 *
 * @param[in,out] network pointer to network
 * @return @see IotReturnCode
 */
int qcloud_iot_network_init(IotNetwork *network)
{
    POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    switch (network->type) {
        case IOT_NETWORK_TYPE_TCP:
            network->init         = _network_tcp_init;
            network->connect      = _network_tcp_connect;
            network->read         = _network_tcp_read;
            network->write        = _network_tcp_write;
            network->disconnect   = _network_tcp_disconnect;
            network->is_connected = _is_network_tcp_connected;
            network->fd           = -1;
            break;

#ifndef AUTH_WITH_NO_TLS
        case IOT_NETWORK_TYPE_TLS:
            network->init         = _network_tls_init;
            network->connect      = _network_tls_connect;
            network->read         = _network_tls_read;
            network->write        = _network_tls_write;
            network->disconnect   = _network_tls_disconnect;
            network->is_connected = _is_network_tls_connected;
            network->handle       = 0;
            break;
#endif

        default:
            Log_e("unknown network type: %d", network->type);
            return QCLOUD_ERR_INVAL;
    }
    return network->init(network);
}
