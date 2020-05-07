/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "network_interface.h"
#include "qcloud_iot_export_error.h"
#include "qcloud_iot_import.h"
#include "utils_param_check.h"

/*
 * TCP/UDP socket API
 */

int network_tcp_init(Network *pNetwork)
{
    return QCLOUD_RET_SUCCESS;
}

int network_tcp_connect(Network *pNetwork)
{
    POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);

    pNetwork->handle = HAL_TCP_Connect(pNetwork->host, pNetwork->port);
    if (0 == pNetwork->handle) {
        return -1;
    }

    return 0;
}

int network_tcp_read(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *read_len)
{
    POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);

    int rc = 0;

    rc = HAL_TCP_Read(pNetwork->handle, data, (uint32_t)datalen, timeout_ms, read_len);

    return rc;
}

int network_tcp_write(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *written_len)
{
    POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);

    int rc = 0;

    rc = HAL_TCP_Write(pNetwork->handle, data, datalen, timeout_ms, written_len);

    return rc;
}

void network_tcp_disconnect(Network *pNetwork)
{
    POINTER_SANITY_CHECK_RTN(pNetwork);

    if (0 == pNetwork->handle) {
        return;
    }

    HAL_TCP_Disconnect(pNetwork->handle);
    pNetwork->handle = 0;
    return;
}

#if (defined COAP_COMM_ENABLED) && (defined AUTH_WITH_NOTLS)

int network_udp_init(Network *pNetwork)
{
    return QCLOUD_RET_SUCCESS;
}

int network_udp_read(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *read_len)
{
    POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);

    int ret = HAL_UDP_ReadTimeout(pNetwork->handle, data, datalen, timeout_ms);
    if (ret > 0) {
        *read_len = ret;
        ret       = 0;
    }

    return ret;
}

int network_udp_write(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *written_len)
{
    POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);

    int ret = HAL_UDP_Write(pNetwork->handle, data, datalen);
    if (ret > 0) {
        *written_len = ret;
        ret          = 0;
    }

    return ret;
}

void network_udp_disconnect(Network *pNetwork)
{
    POINTER_SANITY_CHECK_RTN(pNetwork);

    HAL_UDP_Disconnect(pNetwork->handle);
    pNetwork->handle = 0;

    return;
}

int network_udp_connect(Network *pNetwork)
{
    POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);

    pNetwork->handle = HAL_UDP_Connect(pNetwork->host, pNetwork->port);
    if (0 == pNetwork->handle) {
        return -1;
    }
    return 0;
}

#endif
