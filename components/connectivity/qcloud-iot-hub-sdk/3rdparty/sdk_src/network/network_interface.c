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

#ifdef __cplusplus
extern "C" {
#endif

#include "network_interface.h"

#include "qcloud_iot_export_error.h"
#include "utils_param_check.h"

int is_network_connected(Network *pNetwork)
{
    return pNetwork->handle;
}

#ifdef AT_TCP_ENABLED
int is_network_at_connected(Network *pNetwork)
{
    return pNetwork->handle == AT_NO_CONNECTED_FD ? 0 : pNetwork->handle == AT_NO_CONNECTED_FD;
}
#endif

int network_init(Network *pNetwork)
{
    POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);

    // to avoid process crash when writing to a broken socket
#if defined(__linux__)
    signal(SIGPIPE, SIG_IGN);
#endif

    switch (pNetwork->type) {
        case NETWORK_TCP:
#ifdef AT_TCP_ENABLED
            pNetwork->init         = network_at_tcp_init;
            pNetwork->connect      = network_at_tcp_connect;
            pNetwork->read         = network_at_tcp_read;
            pNetwork->write        = network_at_tcp_write;
            pNetwork->disconnect   = network_at_tcp_disconnect;
            pNetwork->is_connected = is_network_at_connected;
            pNetwork->handle       = AT_NO_CONNECTED_FD;
#else
            pNetwork->init         = network_tcp_init;
            pNetwork->connect      = network_tcp_connect;
            pNetwork->read         = network_tcp_read;
            pNetwork->write        = network_tcp_write;
            pNetwork->disconnect   = network_tcp_disconnect;
            pNetwork->is_connected = is_network_connected;
            pNetwork->handle       = 0;
#endif
            break;

#ifndef AUTH_WITH_NOTLS
        case NETWORK_TLS:
            pNetwork->init         = network_tls_init;
            pNetwork->connect      = network_tls_connect;
            pNetwork->read         = network_tls_read;
            pNetwork->write        = network_tls_write;
            pNetwork->disconnect   = network_tls_disconnect;
            pNetwork->is_connected = is_network_connected;
            pNetwork->handle       = 0;
            break;
#endif

#ifdef COAP_COMM_ENABLED
#ifdef AUTH_WITH_NOTLS
        case NETWORK_UDP:
            pNetwork->init         = network_udp_init;
            pNetwork->connect      = network_udp_connect;
            pNetwork->read         = network_udp_read;
            pNetwork->write        = network_udp_write;
            pNetwork->disconnect   = network_udp_disconnect;
            pNetwork->is_connected = is_network_connected;
            pNetwork->handle       = 0;
            break;
#else
        case NETWORK_DTLS:
            pNetwork->init         = network_dtls_init;
            pNetwork->connect      = network_dtls_connect;
            pNetwork->read         = network_dtls_read;
            pNetwork->write        = network_dtls_write;
            pNetwork->disconnect   = network_dtls_disconnect;
            pNetwork->is_connected = is_network_connected;
            pNetwork->handle       = 0;
            break;
#endif
#endif
        default:
            Log_e("unknown network type: %d", pNetwork->type);
            return QCLOUD_ERR_INVAL;
    }
    return pNetwork->init(pNetwork);
}

#ifdef __cplusplus
}
#endif
