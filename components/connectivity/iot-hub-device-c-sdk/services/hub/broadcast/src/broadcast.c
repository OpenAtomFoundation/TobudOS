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
 * @file broadcast.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-18
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-18 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_broadcast.h"

/**
 * @brief Context of broadcast message.
 *
 */
typedef struct {
    OnBroadcastArrivedCallback callback; /**< callback to handle message */
    void *                     usr_data; /**< usr data using in callback */
} QcloudIotBroadcastContext;

/**
 * @brief Callback for broadcast topic message
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message broadcast message @see MQTTMessage
 * @param[in,out] context @see QcloudIotBroadcastContext
 */
static void _broadcast_message_cb(void *client, const MQTTMessage *message, void *context)
{
    QcloudIotBroadcastContext *broadcast_context = (QcloudIotBroadcastContext *)context;

    Log_d("topic=%.*s", message->topic_len, STRING_PTR_PRINT_SANITY_CHECK(message->topic_name));
    Log_i("len=%u, topic_msg=%.*s", message->payload_len, message->payload_len,
          STRING_PTR_PRINT_SANITY_CHECK(message->payload_str));

    if (broadcast_context->callback) {
        broadcast_context->callback(client, message->payload_str, message->payload_len, broadcast_context->usr_data);
    }
}

/**
 * @brief Subscribe broadcast topic with callback.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback callback to handle message
 * @param[in] usr_data usr data using in callback
 * @return @see IotReturnCode
 */
int IOT_Broadcast_Init(void *client, OnBroadcastArrivedCallback callback, void *usr_data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(callback, QCLOUD_ERR_INVAL);

    int             rc = 0;
    char            broadcast_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;

    /**
     * @brief Using static for only one broardcast topic can be subscribed to.
     *
     */
    QcloudIotBroadcastContext *broadcast_context =
        (QcloudIotBroadcastContext *)HAL_Malloc(sizeof(QcloudIotBroadcastContext));
    if (!broadcast_context) {
        return QCLOUD_ERR_MALLOC;
    }

    broadcast_context->callback = callback;
    broadcast_context->usr_data = usr_data;

    HAL_Snprintf(broadcast_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$broadcast/rxd/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    sub_params.on_message_handler = _broadcast_message_cb;
    sub_params.qos                = QOS1;
    sub_params.user_data          = broadcast_context;
    sub_params.user_data_free     = HAL_Free;

    rc = IOT_MQTT_SubscribeSync(client, broadcast_topic, &sub_params);
    if (rc) {
        HAL_Free(broadcast_context);
    }
    return rc;
}

/**
 * @brief Unsubscribe broadcast topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int IOT_Broadcast_Deinit(void *client)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    char broadcast_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(broadcast_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$broadcast/rxd/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));
    return IOT_MQTT_Unsubscribe(client, broadcast_topic) > 0 ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}
