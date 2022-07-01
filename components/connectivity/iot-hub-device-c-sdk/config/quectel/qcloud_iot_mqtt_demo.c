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
 * @file mqtt_sample.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-03-08
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-03-08 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_common.h"

#include "utils_log.h"

/**
 * @brief MQTT event callback, @see MQTTEventHandleFun
 *
 * @param[in] client pointer to mqtt client
 * @param[in] handle_context context
 * @param[in] msg msg
 */
static void _mqtt_event_handler(void *client, void *handle_context, MQTTEventMsg *msg)
{
    MQTTMessage *mqtt_message = (MQTTMessage *)msg->msg;
    uintptr_t    packet_id    = (uintptr_t)msg->msg;

    switch (msg->event_type) {
        case MQTT_EVENT_UNDEF:
            Log_i("undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            Log_i("MQTT disconnect.");
            break;

        case MQTT_EVENT_RECONNECT:
            Log_i("MQTT reconnect.");
            break;

        case MQTT_EVENT_PUBLISH_RECEIVED:
            Log_i("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                  mqtt_message->topic_len, STRING_PTR_PRINT_SANITY_CHECK(mqtt_message->topic_name),
                  mqtt_message->payload_len, STRING_PTR_PRINT_SANITY_CHECK((char *)mqtt_message->payload));
            break;
        case MQTT_EVENT_SUBSCRIBE_SUCCESS:
            Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBSCRIBE_TIMEOUT:
            Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_SUBSCRIBE_NACK:
            Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBE_SUCCESS:
            Log_i("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBE_TIMEOUT:
            Log_i("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBE_NACK:
            Log_i("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;
        default:
            Log_i("Should NOT arrive here.");
            break;
    }
}

/**
 * @brief Setup MQTT construct parameters.
 *
 * @param[in,out] initParams @see MQTTInitParams
 * @param[in] device_info @see DeviceInfo
 */
static void _setup_connect_init_params(MQTTInitParams *init_params, DeviceInfo *device_info)
{
    init_params->device_info       = device_info;
    init_params->event_handle.h_fp = _mqtt_event_handler;
}

/**
 * @brief Publish a test mqtt message.
 *
 * @param[in, out] client pointer to mqtt client
 * @param[in] topic_keyword topic keyword
 * @param[in] qos qos to publish
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
static int _publish_test_msg(void *client, const char *topic_keyword, QoS qos)
{
    char topic_name[MAX_SIZE_OF_CLOUD_TOPIC] = {0};

    DeviceInfo *dev_info = IOT_MQTT_GetDeviceInfo(client);

    HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", STRING_PTR_PRINT_SANITY_CHECK(dev_info->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(dev_info->device_name), STRING_PTR_PRINT_SANITY_CHECK(topic_keyword));

    static int    test_count = 0;
    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = qos;

    char publish_content[128] = {0};
    HAL_Snprintf(publish_content, sizeof(publish_content), "{\"action\": \"publish_test\", \"count\": \"%d\"}",
                 test_count++);
    pub_params.payload     = publish_content;
    pub_params.payload_len = strlen(publish_content);
    return IOT_MQTT_Publish(client, topic_name, &pub_params);
}

/**
 * @brief Callback when MQTT msg arrives @see OnMessageHandler
 *
 * @param[in, out] client pointer to mqtt client
 * @param[in] message publish message from server
 * @param[in] usr_data user data of SubscribeParams, @see SubscribeParams
 */
static void _on_message_callback(void *client, const MQTTMessage *message, void *user_data)
{
    if (!message) {
        return;
    }

    Log_i("Receive Message With topicName:%.*s, payload:%.*s", message->topic_len,
          STRING_PTR_PRINT_SANITY_CHECK(message->topic_name), message->payload_len,
          STRING_PTR_PRINT_SANITY_CHECK(message->payload_str));
}

/**
 * @brief Subscribe MQTT topic and wait for sub result.
 *
 * @param[in, out] client pointer to mqtt client
 * @param[in] topic_keyword topic keyword
 * @param[in] qos qos to subscribe
 * @return @see IotReturnCode
 */
static int _subscribe_topic_wait_result(void *client, char *topic_keyword, QoS qos)
{
    char topic_name[MAX_SIZE_OF_CLOUD_TOPIC] = {0};

    DeviceInfo *dev_info = IOT_MQTT_GetDeviceInfo(client);

    HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", STRING_PTR_PRINT_SANITY_CHECK(dev_info->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(dev_info->device_name), STRING_PTR_PRINT_SANITY_CHECK(topic_keyword));

    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.qos                = qos;
    sub_params.on_message_handler = _on_message_callback;

    return IOT_MQTT_SubscribeSync(client, topic_name, &sub_params);
}

/**
 * @brief Unsubscribe MQTT topic.
 *
 * @param[in, out] client pointer to mqtt client
 * @param[in] topic_keyword topic keyword
 * @return @see IotReturnCode
 */
static int _unsubscribe_topic(void *client, char *topic_keyword)
{
    char topic_name[MAX_SIZE_OF_CLOUD_TOPIC] = {0};

    DeviceInfo *dev_info = IOT_MQTT_GetDeviceInfo(client);

    HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", STRING_PTR_PRINT_SANITY_CHECK(dev_info->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(dev_info->device_name), STRING_PTR_PRINT_SANITY_CHECK(topic_keyword));

    int rc = IOT_MQTT_Unsubscribe(client, topic_name);
    if (rc < 0) {
        Log_e("MQTT unsubscribe FAILED: %d", rc);
        return rc;
    }

    return IOT_MQTT_Yield(client, 500);  // wait for unsuback
}

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int qcloud_iot_mqtt_demo(int argc, char **argv)
{
    int rc;

    // init log level
    LogHandleFunc func = DEFAULT_LOG_HANDLE_FUNCS;
    utils_log_init(func, LOG_LEVEL_DEBUG, 2048);

    DeviceInfo device_info;

    rc = HAL_GetDevInfo(&device_info);
    if (rc) {
        Log_e("get device info failed: %d", rc);
        return rc;
    }

    // init connection
    MQTTInitParams init_params = DEFAULT_MQTT_INIT_PARAMS;
    _setup_connect_init_params(&init_params, &device_info);

    // create MQTT client and connect with server
    void *client = IOT_MQTT_Construct(&init_params);
    if (client) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("MQTT Construct failed!");
        return QCLOUD_ERR_FAILURE;
    }

    // subscribe normal topics and wait result
    rc = _subscribe_topic_wait_result(client, "data", QOS0);
    if (rc) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        return rc;
    }

    do {
        rc = _publish_test_msg(client, "data", QOS1);
        if (rc < 0) {
            Log_e("client publish topic failed :%d.", rc);
        }

        rc = IOT_MQTT_Yield(client, 2000);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("exit with error: %d", rc);
            break;
        }
    } while (1);

    rc = _unsubscribe_topic(client, "data");
    rc |= IOT_MQTT_Destroy(&client);

    utils_log_deinit();
    return rc;
}
