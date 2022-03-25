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
 * @file data_template_mqtt.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-18
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-18 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_data_template.h"

#include "data_template.h"

/**
 * @brief Direction(upstream/downstream) for topic.
 *
 */
typedef enum {
    DATA_TEMPLATE_DIRECTION_UP = 0,
    DATA_TEMPLATE_DIRECTION_DOWN,
} DataTemplateDirection;

/**
 * @brief Generate topic string.
 *
 * @param[out] buf buffer for topic name
 * @param[in] buf_len buffer length
 * @param[in] direction @see DataTemplateDirection
 * @param[in] type @see DataTemplateType
 * @param[in] product_id product id of device
 * @param[in] device_name device name of device
 * @return > 0 for length of topic name, others for fail.
 */
static int _data_template_topic_generate(char *buf, int buf_len, DataTemplateDirection direction, DataTemplateType type,
                                         const char *product_id, const char *device_name)
{
    const char *topic_method[] = {"property", "event", "action"};
    // $thing/down/property/C283SMY3W3/test1
    return HAL_Snprintf(buf, buf_len, "$thing/%s/%s/%s/%s", direction ? "down" : "up", topic_method[type],
                        STRING_PTR_PRINT_SANITY_CHECK(product_id), STRING_PTR_PRINT_SANITY_CHECK(device_name));
}

/**
 * @brief Check if topic already subscribed, if not then subscribe.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see DataTemplateType
 * @param[in] on_message_handler message handle of topic
 * @param[in] context @see DataTemplateContext
 * @return @see IotReturnCode
 */
int data_template_topic_check_and_sub(void *client, DataTemplateType type, OnMessageHandler on_message_handler,
                                      DataTemplateContext context)
{
    char data_template_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    _data_template_topic_generate(data_template_topic, MAX_SIZE_OF_CLOUD_TOPIC, DATA_TEMPLATE_DIRECTION_DOWN, type,
                                  IOT_MQTT_GetDeviceInfo(client)->product_id,
                                  IOT_MQTT_GetDeviceInfo(client)->device_name);

    int rc = 0;

    DataTemplateContext *data_template_context = HAL_Malloc(sizeof(DataTemplateContext));
    if (!data_template_context) {
        return QCLOUD_ERR_MALLOC;
    }
    *data_template_context = context;

    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = on_message_handler;
    sub_params.qos                = QOS1;
    sub_params.user_data          = data_template_context;
    sub_params.user_data_free     = HAL_Free;

    rc = IOT_MQTT_SubscribeSync(client, data_template_topic, &sub_params);
    if (rc) {
        Log_e("subscribe topic %s failed!", data_template_topic);
        HAL_Free(data_template_context);
    }
    return rc;
}

/**
 * @brief Unsubscribe data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see DataTemplateType
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_topic_unsubscribe(void *client, DataTemplateType type)
{
    char data_template_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    _data_template_topic_generate(data_template_topic, MAX_SIZE_OF_CLOUD_TOPIC, DATA_TEMPLATE_DIRECTION_DOWN, type,
                                  IOT_MQTT_GetDeviceInfo(client)->product_id,
                                  IOT_MQTT_GetDeviceInfo(client)->device_name);
    return IOT_MQTT_Unsubscribe(client, data_template_topic);
}

/**
 * @brief Publish to data template topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see DataTemplateType
 * @param[in] qos @see QoS
 * @param[in] payload payload of mqtt packet
 * @param[in] payload_len payload len
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int data_template_publish(void *client, DataTemplateType type, QoS qos, const char *payload, int payload_len)
{
    NUMBERIC_SANITY_CHECK(payload_len, QCLOUD_ERR_BUF_TOO_SHORT);

    char data_template_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    _data_template_topic_generate(data_template_topic, MAX_SIZE_OF_CLOUD_TOPIC, DATA_TEMPLATE_DIRECTION_UP, type,
                                  IOT_MQTT_GetDeviceInfo(client)->product_id,
                                  IOT_MQTT_GetDeviceInfo(client)->device_name);

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = qos;
    pub_params.payload       = (void *)payload;
    pub_params.payload_len   = payload_len;
    return IOT_MQTT_Publish(client, data_template_topic, &pub_params);
}
