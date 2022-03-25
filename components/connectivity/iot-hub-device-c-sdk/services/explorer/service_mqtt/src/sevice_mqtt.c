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
 * @file sevice_mqtt.c
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

#include "service_mqtt.h"

/**
 * @brief Direction(upstream/downstream) for topic.
 *
 */
typedef enum {
    SERVICE_TOPIC_DIRECTION_UP = 0,
    SERVICE_TOPIC_DIRECTION_DOWN,
} ServiceTopicDirection;

/**
 * @brief Context of service mqtt, callback and user data.
 *
 */
typedef struct {
    void *service_list;
    void *usr_data;
} ServiceMqttContext;

/**
 * @brief Service list conext.
 *
 */
typedef struct {
    const char        *method;
    int                method_len;
    const MQTTMessage *message;
} ServiceListContext;

/**
 * @brief Generate topic string.
 *
 * @param[out] buf buffer for topic name
 * @param[in] buf_len buffer length
 * @param[in] direction @see ServiceTopicDirection
 * @param[in] product_id product id of device
 * @param[in] device_name device name of device
 * @return > 0 for length of topic name, others for fail.
 */
static int _service_mqtt_topic_generate(char *buf, int buf_len, ServiceTopicDirection direction, const char *product_id,
                                        const char *device_name)
{
    return HAL_Snprintf(buf, buf_len, "$thing/%s/service/%s/%s", direction ? "down" : "up",
                        STRING_PTR_PRINT_SANITY_CHECK(product_id), STRING_PTR_PRINT_SANITY_CHECK(device_name));
}

// ----------------------------------------------------------------------------
// server mqtt context
// ----------------------------------------------------------------------------

/**
 * @brief Malloc service context and service list.
 *
 * @return @see ServiceMqttContext
 */
static ServiceMqttContext *_service_mqtt_context_malloc(void)
{
    ServiceMqttContext *context = (ServiceMqttContext *)HAL_Malloc(sizeof(ServiceMqttContext));
    if (!context) {
        return NULL;
    }

    UtilsListFunc func = {
        .list_malloc      = HAL_Malloc,
        .list_free        = HAL_Free,
        .list_lock_init   = HAL_MutexCreate,
        .list_lock        = HAL_MutexLock,
        .list_unlock      = HAL_MutexUnlock,
        .list_lock_deinit = HAL_MutexDestroy,
    };
    context->service_list = utils_list_create(func, 10);
    if (!context->service_list) {
        HAL_Free(context);
        context = NULL;
    }
    return context;
}

/**
 * @brief Free service context and service list.
 *
 * @param usr_data @see ServiceMqttContext
 */
static void _service_mqtt_context_free(void *usr_data)
{
    ServiceMqttContext *context = (ServiceMqttContext *)usr_data;
    utils_list_destroy(context->service_list);
    HAL_Free(context);
}

/**
 * @brief Get service mqtt context from client.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see ServiceMqttContext
 */
static ServiceMqttContext *_service_mqtt_context_get(void *client)
{
    char service_mqtt_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    _service_mqtt_topic_generate(service_mqtt_topic, MAX_SIZE_OF_CLOUD_TOPIC, SERVICE_TOPIC_DIRECTION_DOWN,
                                 IOT_MQTT_GetDeviceInfo(client)->product_id,
                                 IOT_MQTT_GetDeviceInfo(client)->device_name);
    return (ServiceMqttContext *)IOT_MQTT_GetSubUsrData(client, service_mqtt_topic);
}

// ----------------------------------------------------------------------------
// server list
// ----------------------------------------------------------------------------

/**
 * @brief Unregister server from server list.
 *
 * @param[in] list pointer to server list
 * @param[in] node pointer tot server list node
 * @param[in] val @see ServiceRegisterParams
 * @param[in] usr_data @see ServiceType
 * @return @see UtilsListResult
 */
static UtilsListResult _server_list_unregister_callback(void *list, void *node, void *val, void *usr_data)
{
    ServiceRegisterParams *params = (ServiceRegisterParams *)val;

    ServiceType type = *(ServiceType *)usr_data;
    if (params->type == type) {
        if (params->user_data_free) {
            params->user_data_free(params->usr_data);
        }
        utils_list_remove(list, node);
        return LIST_TRAVERSE_BREAK;
    }
    return LIST_TRAVERSE_CONTINUE;
}

/**
 * @brief Callback server handler from server list.
 *
 * @param[in] list pointer to server list
 * @param[in] node pointer tot server list node
 * @param[in] val @see ServiceRegisterParams
 * @param[in] usr_data @see ServiceListContext
 * @return @see UtilsListResult
 */
static UtilsListResult _server_list_process_message_callback(void *list, void *node, void *val, void *usr_data)
{
    ServiceRegisterParams *params  = (ServiceRegisterParams *)val;
    ServiceListContext    *context = (ServiceListContext *)usr_data;

    for (int i = 0; i < params->method_num; i++) {
        int len = strlen(params->method_list[i]);
        if (len == context->method_len && !strncmp(context->method, params->method_list[i], len)) {
            params->message_handle(usr_data, context->message, params->usr_data);
            return LIST_TRAVERSE_BREAK;
        }
    }
    return LIST_TRAVERSE_CONTINUE;
}

// ----------------------------------------------------------------------------
// mqtt
// ----------------------------------------------------------------------------

/**
 * @brief Handle server mqtt topic down stream message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message @see MQTTMessage
 * @param[in] usr_data @see ServiceMqttContext
 */
static void _service_mqtt_message_handle(void *client, const MQTTMessage *message, void *usr_data)
{
    Log_d("receive service message:%.*s", message->payload_len, message->payload_str);

    UtilsJsonValue method;

    int rc = utils_json_value_get("method", sizeof("method") - 1, message->payload_str, message->payload_len, &method);
    if (rc) {
        return;
    }

    // process list
    ServiceListContext server_list_context = {
        .method     = method.value,
        .method_len = method.value_len,
        .message    = message,
    };
    void *list = ((ServiceMqttContext *)usr_data)->service_list;
    utils_list_process(list, LIST_HEAD, _server_list_process_message_callback, &server_list_context);
}

/**
 * @brief Check if topic already subscribed, if not then subscribe.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int _service_mqtt_topic_check_and_sub(void *client)
{
    char service_mqtt_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    _service_mqtt_topic_generate(service_mqtt_topic, MAX_SIZE_OF_CLOUD_TOPIC, SERVICE_TOPIC_DIRECTION_DOWN,
                                 IOT_MQTT_GetDeviceInfo(client)->product_id,
                                 IOT_MQTT_GetDeviceInfo(client)->device_name);

    int rc = 0;

    ServiceMqttContext *context = _service_mqtt_context_malloc();
    if (!context) {
        return QCLOUD_ERR_MALLOC;
    }

    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _service_mqtt_message_handle;
    sub_params.qos                = QOS1;
    sub_params.user_data          = context;
    sub_params.user_data_free     = _service_mqtt_context_free;

    rc = IOT_MQTT_SubscribeSync(client, service_mqtt_topic, &sub_params);
    if (rc) {
        _service_mqtt_context_free(context);
        Log_e("subscribe topic %s failed!", service_mqtt_topic);
    }
    return rc;
}

/**
 * @brief Unsubscribe service mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int _service_mqtt_unsubscribe(void *client)
{
    char service_mqtt_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    _service_mqtt_topic_generate(service_mqtt_topic, MAX_SIZE_OF_CLOUD_TOPIC, SERVICE_TOPIC_DIRECTION_DOWN,
                                 IOT_MQTT_GetDeviceInfo(client)->product_id,
                                 IOT_MQTT_GetDeviceInfo(client)->device_name);
    return IOT_MQTT_Unsubscribe(client, service_mqtt_topic);
}

// ----------------------------------------------------------------------------
// api
// ----------------------------------------------------------------------------

/**
 * @brief Subscribe service mqtt.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int service_mqtt_init(void *client)
{
    return _service_mqtt_topic_check_and_sub(client);
}

/**
 * @brief If no service in the service list, then unsubscribe.
 *
 * @param[in,out] client pointer to mqtt client
 */
void service_mqtt_deinit(void *client)
{
    ServiceMqttContext *context = _service_mqtt_context_get(client);
    if (!context) {
        return;
    }

    if (!utils_list_len_get(context->service_list)) {
        _service_mqtt_unsubscribe(client);
    }
}

/**
 * @brief Publish to service mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] qos @see QoS
 * @param[in] payload payload of mqtt packet
 * @param[in] payload_len payload len
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int service_mqtt_publish(void *client, QoS qos, const char *payload, int payload_len)
{
    char service_mqtt_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    _service_mqtt_topic_generate(service_mqtt_topic, MAX_SIZE_OF_CLOUD_TOPIC, SERVICE_TOPIC_DIRECTION_UP,
                                 IOT_MQTT_GetDeviceInfo(client)->product_id,
                                 IOT_MQTT_GetDeviceInfo(client)->device_name);

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = qos;
    pub_params.payload       = (void *)payload;
    pub_params.payload_len   = payload_len;
    return IOT_MQTT_Publish(client, service_mqtt_topic, &pub_params);
}

/**
 * @brief Register server handler to server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] params @see ServiceRegisterParams
 * @return @see IotReturnCode
 */
int service_mqtt_service_register(void *client, const ServiceRegisterParams *params)
{
    ServiceMqttContext *context = _service_mqtt_context_get(client);
    if (!context) {
        return QCLOUD_ERR_FAILURE;
    }

    ServiceRegisterParams *register_params = (ServiceRegisterParams *)HAL_Malloc(sizeof(ServiceRegisterParams));
    if (!register_params) {
        return QCLOUD_ERR_MALLOC;
    }
    memcpy(register_params, params, sizeof(ServiceRegisterParams));

    void *node = utils_list_push(context->service_list, register_params);
    if (!node) {
        HAL_Free(register_params);
        return QCLOUD_ERR_MALLOC;
    }
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Unregister server handler from server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] type @see ServiceType
 */
void service_mqtt_service_unregister(void *client, ServiceType type)
{
    ServiceMqttContext *context = _service_mqtt_context_get(client);
    if (!context) {
        return;
    }
    utils_list_process(context->service_list, LIST_HEAD, _server_list_unregister_callback, &type);
}
