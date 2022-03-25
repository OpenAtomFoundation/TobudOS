/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @file service_mqtt.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-01-11
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-11 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_SERVICES_EXPLORER_SERVICE_MQTT_INC_SERVICE_MQTT_H_
#define IOT_HUB_DEVICE_C_SDK_SERVICES_EXPLORER_SERVICE_MQTT_INC_SERVICE_MQTT_H_

#include "qcloud_iot_common.h"
#include "utils_list.h"

/**
 * @brief Service type, only file manage supportted now.
 *
 */
typedef enum {
    SERVICE_TYPE_FILE_MANAGE = 0,
} ServiceType;

/**
 * @brief Register to server list.
 *
 */
typedef struct {
    ServiceType      type;
    const char **    method_list;
    int              method_num;
    OnMessageHandler message_handle;
    void *           usr_data;
    void (*user_data_free)(void *);
} ServiceRegisterParams;

/**
 * @brief Subscribe service mqtt.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int service_mqtt_init(void *client);

/**
 * @brief If no service in the service list, then unsubscribe.
 *
 * @param[in,out] client pointer to mqtt client
 */
void service_mqtt_deinit(void *client);

/**
 * @brief Publish to service mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] qos @see QoS
 * @param[in] payload payload of mqtt packet
 * @param[in] payload_len payload len
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int service_mqtt_publish(void *client, QoS qos, const char *payload, int payload_len);

/**
 * @brief Register server handler to server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] params @see ServiceRegisterParams
 * @return @see IotReturnCode
 */
int service_mqtt_service_register(void *client, const ServiceRegisterParams *params);

/**
 * @brief Unregister server handler from server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see ServiceType
 */
void service_mqtt_service_unregister(void *client, ServiceType type);

#endif  // IOT_HUB_DEVICE_C_SDK_SERVICES_EXPLORER_SERVICE_MQTT_INC_SERVICE_MQTT_H_
