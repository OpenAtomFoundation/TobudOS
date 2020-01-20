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

#include "qcloud.h"

__QCLOUD_STATIC__ int qcloud_network_tcp_is_connected(qcloud_network_t *network)
{
    return network->fd >= 0;
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_network_tcp_read(qcloud_network_t *network, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(read_len, QCLOUD_ERR_INVAL);

    return osal_tcp_read(network->fd, buf, len, timeout, read_len);
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_network_tcp_write(qcloud_network_t *network, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(write_len, QCLOUD_ERR_INVAL);

    return osal_tcp_write(network->fd, buf, len, timeout, write_len);
}

__QCLOUD_STATIC__ void qcloud_network_tcp_disconnect(qcloud_network_t *network)
{
    if (!network || network->fd < 0) {
        return;
    }

    osal_tcp_disconnect(network->fd);
    network->fd = -1;
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_network_tcp_connect(qcloud_network_t *network)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    network->fd = osal_tcp_connect(network->host, network->port);
    if (network->fd >= 0) {
        return QCLOUD_ERR_SUCCESS;
    }
    return QCLOUD_ERR_FAILURE;
}

__QCLOUD_API__ qcloud_err_t qcloud_network_tcp_init(qcloud_network_t *network)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    network->connect            = qcloud_network_tcp_connect;
    network->read               = qcloud_network_tcp_read;
    network->write              = qcloud_network_tcp_write;
    network->disconnect         = qcloud_network_tcp_disconnect;

    network->is_connected       = qcloud_network_tcp_is_connected;
    network->fd                 = -1;

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_STATIC__ int qcloud_network_udp_is_connected(qcloud_network_t *network)
{
    return network->fd >= 0;
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_network_udp_read(qcloud_network_t *network, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(read_len, QCLOUD_ERR_INVAL);

    return osal_udp_read(network->fd, buf, len, timeout, read_len);
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_network_udp_write(qcloud_network_t *network, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(write_len, QCLOUD_ERR_INVAL);

    return osal_udp_write(network->fd, buf, len, timeout, write_len);
}

__QCLOUD_STATIC__ void qcloud_network_udp_disconnect(qcloud_network_t *network)
{
    if (!network || network->fd < 0) {
        return;
    }

    osal_udp_disconnect(network->fd);
    network->fd = -1;
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_network_udp_connect(qcloud_network_t *network)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    network->fd = osal_udp_connect(network->host, network->port);
    if (network->fd >= 0) {
        return QCLOUD_ERR_SUCCESS;
    }
    return QCLOUD_ERR_FAILURE;
}

__QCLOUD_API__ qcloud_err_t qcloud_network_udp_init(qcloud_network_t *network)
{
    QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    network->connect        = qcloud_network_udp_connect;
    network->read           = qcloud_network_udp_read;
    network->write          = qcloud_network_udp_write;
    network->disconnect     = qcloud_network_udp_disconnect;

    network->is_connected   = qcloud_network_udp_is_connected;
    network->fd             = -1;

    return QCLOUD_ERR_SUCCESS;
}

