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
 * @file log_mqtt.c
 * @brief
 * @author hubertxxu (hubertxxu@tencent.com)
 * @version 1.0
 * @date 2022-01-05
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-05 <td>1.0     <td>hubertxxu   <td>first commit
 * </table>
 */

#include "log_upload.h"

/**
 * @brief Parse log level.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see LogLevelResultInfo
 */
static void _log_mqtt_message_callback(void *client, const MQTTMessage *message, void *usr_data)
{
    int      rc;
    uint32_t log_level;

    Log_d("Receive log result message:%.*s", message->payload_len, message->payload_str);

    // get log level
    rc = utils_json_get_uint32("log_level", sizeof("log_level") - 1, message->payload_str, message->payload_len,
                               &log_level);
    if (rc) {
        return;
    }

    switch (log_level) {
        case LOG_LEVEL_DISABLE:
            log_upload_buffer_clear();
            log_upload_set_upload_log_in_comm_err(true);
            log_upload_set_log_upload_level(LOG_LEVEL_ERROR);
            UPLOAD_DBG("Upload log level change to: %d", LOG_LEVEL_ERROR);
            break;
        case LOG_LEVEL_ERROR:
        case LOG_LEVEL_WARN:
        case LOG_LEVEL_INFO:
        case LOG_LEVEL_DEBUG:
            if (log_level < log_upload_get_log_upload_level()) {
                log_upload_buffer_clear();
            }
            log_upload_set_upload_log_in_comm_err(false);
            log_upload_set_log_upload_level((LogLevel)log_level);
            UPLOAD_DBG("Upload log level change to: %d", log_level);
            break;
        default:
            UPLOAD_ERR("Invalid log level: %d", log_level);
            break;
    }
}

/**
 * @brief Check and subscribe log result topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
static int _log_mqtt_result_topic_check_and_sub(void *client)
{
    char log_upload_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    /* subscribe the log topic: "$log/operation/result/${productId}/${deviceName}" */
    HAL_Snprintf(log_upload_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$log/operation/result/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));
    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _log_mqtt_message_callback;
    sub_params.qos                = QOS1;
    return IOT_MQTT_SubscribeSync(client, log_upload_topic, &sub_params);
}

/**
 * @brief Publish get log level message to log topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
static int _log_mqtt_get_log_level_publish(void *client)
{
    static int sg_get_log_level_token_count = 0;
    char       topic_name[MAX_SIZE_OF_CLOUD_TOPIC];
    char       payload[128] = {0};

    HAL_Snprintf(topic_name, sizeof(topic_name), "$log/operation/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));
    HAL_Snprintf(payload, sizeof(payload), "{\"type\": \"get_log_level\",\"clientToken\": \"get_log_level-%u\"}",
                 sg_get_log_level_token_count++);

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = QOS0;
    pub_params.payload       = payload;
    pub_params.payload_len   = strlen(payload);
    return IOT_MQTT_Publish(client, topic_name, &pub_params) >= 0 ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
}

/**
 * @brief Init log upload module.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int log_mqtt_init(void *client)
{
    int rc = _log_mqtt_result_topic_check_and_sub(client);
    if (rc) {
        return rc;
    }
    return _log_mqtt_get_log_level_publish(client);
}
