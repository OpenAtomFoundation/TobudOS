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

#ifdef __cplusplus
extern "C" {
#endif

#include "mqtt_client_net.h"

//TODO: how to implement    
/**
 * @brief Check if TLS connection is valid
 *
 * @param pNetwork
 * @return
 */
int qcloud_iot_mqtt_tls_is_connected(Network *pNetwork) {
    return 1;
}

/**
 * @brief Init network stack
 *
 * @param pNetwork
 * @param pConnectParams
 * @return
 */
int qcloud_iot_mqtt_network_init(Network *pNetwork) {
    int rc;

    /* first choice: TLS */
    pNetwork->type = NETWORK_TLS;

#ifdef AUTH_WITH_NOTLS    
    pNetwork->type = NETWORK_TCP;
#endif

    rc = network_init(pNetwork);
    pNetwork->is_connected = qcloud_iot_mqtt_tls_is_connected;

    return rc;
}

#ifdef __cplusplus
}
#endif
