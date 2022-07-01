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
 * @file at_module_mqtt_client.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-25
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-04-25 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "at_module.h"

/**
 * @brief Init mqtt client.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] params mqtt init params, @see MQTTInitParams
 * @return @see IotReturnCode
 *
 * @note
 * 1. init device info.
 * 2. init params.
 * 3. set mqtt urc to at module.
 * 4. set device info to at module.
 */
static int _qcloud_iot_mqtt_client_init(QcloudIotClient *client, const MQTTInitParams *params)
{
    memset(client, 0x0, sizeof(QcloudIotClient));
    client->device_info        = params->device_info;
    client->command_timeout_ms = params->command_timeout;
    client->event_handle       = params->event_handle;
    client->default_subscribe  = params->default_subscribe;
    client->connect_state      = MQTT_CONNECT_STATE_DISCONNECT;

    client->tls_mode            = TLS_MODE_TLS_PSK;
    client->auto_connect_enable = params->auto_connect_enable;
    client->clean_session       = params->clean_session;
    client->keep_alive_interval = params->keep_alive_interval > 690 ? 690 : params->keep_alive_interval;

    client->lock_generic = HAL_MutexCreate();
    if (!client->lock_generic) {
        return QCLOUD_ERR_FAILURE;
    }

    // 1. detect module && close echo
    int rc = HAL_Module_Init();
    if (rc) {
        Log_e("module init fail %d", rc);
        return rc;
    }

    // 2. set urc function
    rc = module_set_mqtt_urc();
    if (rc) {
        Log_e("set mqtt urc fail %d", rc);
        return rc;
    }

    // 3. set device info
    rc = module_device_info_set(client);
    if (rc) {
        Log_e("module init fail %d", rc);
        return rc;
    }

    // 4. connect network
    return HAL_Module_ConnectNetwork();
}

/**
 * @brief Deinit mqtt client.
 *
 * @param[in,out] client pointer to mqtt client
 */
static void _qcloud_iot_mqtt_client_deinit(QcloudIotClient *client)
{
    qcloud_iot_mqtt_sub_handle_array_clear(client);
    HAL_MutexDestroy(client->lock_generic);
    HAL_Module_Deinit();
    Log_i("release mqtt client resources");
}

// ----------------------------------------------------------------------------
// API
// ----------------------------------------------------------------------------

/**
 * @brief Only support one mqtt client when using at module.
 *
 */
static QcloudIotClient *sg_mqtt_client;

/**
 * @brief Get pointer to mqtt client.
 *
 * @return pointer to mqtt client.
 */
QcloudIotClient *qcloud_iot_get_mqtt_client(void)
{
    return sg_mqtt_client;
}

/**
 * @brief Create MQTT client and connect to MQTT server. Only one client supportted.
 *
 * @param[in] params MQTT init parameters
 * @return a valid MQTT client handle when success, or NULL otherwise
 */
void *IOT_MQTT_Construct(const MQTTInitParams *params)
{
    POINTER_SANITY_CHECK(params, NULL);
    POINTER_SANITY_CHECK(params->device_info, NULL);
    STRING_PTR_SANITY_CHECK(params->device_info->product_id, NULL);
    STRING_PTR_SANITY_CHECK(params->device_info->device_name, NULL);
#ifdef AUTH_MODE_CERT
    STRING_PTR_SANITY_CHECK(params->device_info->dev_cert_file_name, NULL);
    STRING_PTR_SANITY_CHECK(params->device_info->dev_key_file_name, NULL);
#else
    STRING_PTR_SANITY_CHECK(params->device_info->device_secret, NULL);
#endif
    if (sg_mqtt_client) {
        Log_e("only support one mqtt client in at module!");
        return NULL;
    }

    int rc = 0;

    QcloudIotClient *client = NULL;

    // create and init MQTTClient
    client = (QcloudIotClient *)HAL_Malloc(sizeof(QcloudIotClient));
    if (!client) {
        Log_e("malloc MQTTClient failed");
        return NULL;
    }

    rc = _qcloud_iot_mqtt_client_init(client, params);
    if (rc) {
        Log_e("mqtt init failed: %d", rc);
        goto exit;
    }

    if (!params->connect_when_construct) {
        return client;
    }

    rc = IOT_MQTT_Connect(client);
    if (rc) {
        goto exit;
    }

    client->connect_state = MQTT_CONNECT_STATE_CONNECTED;
    Log_i("mqtt connect with success");
    sg_mqtt_client = client;
    return client;
exit:
    _qcloud_iot_mqtt_client_deinit(client);
    HAL_Free(client);
    return NULL;
}

/**
 * @brief Connect Mqtt server if not connect.
 *
 * @param[in,out] client pointer to mqtt client pointer, should using the pointer of IOT_MQTT_Construct return.
 * @return @see IotReturnCode
 */
int IOT_MQTT_Connect(void *client)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    int rc = 0;

    QcloudIotTimer connect_timer;

    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;
    if (mqtt_client->connect_state == MQTT_CONNECT_STATE_DISCONNECT) {
        IOT_Timer_CountdownMs(&connect_timer, MAX_RECONNECT_WAIT_INTERVAL);

        do {  // wait for wifi config
            rc = module_mqtt_connect(client);
            if (rc) {
                Log_e("mqtt connect with failed: %d", rc);
                HAL_SleepMs(QCLOUD_IOT_MQTT_WAIT_ACK_TIMEOUT);
            }
            if (IOT_Timer_Expired(&connect_timer)) {
                return QCLOUD_ERR_FAILURE;
            }
        } while (rc);
    }
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Close connection and destroy MQTT client.
 *
 * @param[in,out] client pointer to mqtt client pointer, should using the pointer of IOT_MQTT_Construct return.
 * @return @see IotReturnCode
 */
int IOT_MQTT_Destroy(void **client)
{
    POINTER_SANITY_CHECK(*client, QCLOUD_ERR_INVAL);

    QcloudIotClient *mqtt_client = (QcloudIotClient *)(*client);

    int rc = module_mqtt_disconnect(mqtt_client);
    _qcloud_iot_mqtt_client_deinit(mqtt_client);
    HAL_Free(*client);
    *client = sg_mqtt_client = NULL;
    Log_i("mqtt release!");
    return rc;
}

/**
 * @brief Check connection state.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] timeout_ms timeout value (unit: ms) for this operation
 * @return QCLOUD_RET_SUCCESS when success, others @see IotReturnCode
 */
int IOT_MQTT_Yield(void *client, uint32_t timeout_ms)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;
    HAL_SleepMs(timeout_ms);
    module_mqtt_get_connect_state(mqtt_client);
    switch (mqtt_client->connect_state) {
        case MQTT_CONNECT_STATE_CONNECTED:
            return QCLOUD_RET_SUCCESS;
        case MQTT_CONNECT_STATE_RECONNECTING:
            return QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT;
        case MQTT_CONNECT_STATE_RECONNECTED:
            return QCLOUD_RET_MQTT_RECONNECTED;
        case MQTT_CONNECT_STATE_DISCONNECT:
            return QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT;
    }
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Publish MQTT message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_name topic to publish
 * @param[in] params @see PublishParams
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_MQTT_Publish(void *client, const char *topic_name, const PublishParams *params)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(params, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(topic_name, QCLOUD_ERR_INVAL);

    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;

    if (strlen(topic_name) > MAX_SIZE_OF_CLOUD_TOPIC) {
        return QCLOUD_ERR_MAX_TOPIC_LENGTH;
    }

    if (QOS2 == params->qos) {
        Log_e("QoS2 is not supported currently");
        return QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT;
    }

    return module_mqtt_publish(mqtt_client, topic_name, params);
}

/**
 * @brief Subscribe MQTT topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to subscribe
 * @param[in] params @see SubscribeParams
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_MQTT_Subscribe(void *client, const char *topic_filter, const SubscribeParams *params)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(params, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);

    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;

    if (strlen(topic_filter) > MAX_SIZE_OF_CLOUD_TOPIC) {
        return QCLOUD_ERR_MAX_TOPIC_LENGTH;
    }

    if (QOS2 == params->qos) {
        Log_e("QoS2 is not supported currently");
        return QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT;
    }

    return qcloud_iot_mqtt_subscribe(mqtt_client, topic_filter, params);
}

/**
 * @brief Unsubscribe MQTT topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to unsubscribe
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_MQTT_Unsubscribe(void *client, const char *topic_filter)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);

    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;

    if (strlen(topic_filter) > MAX_SIZE_OF_CLOUD_TOPIC) {
        return QCLOUD_ERR_MAX_TOPIC_LENGTH;
    }

    return qcloud_iot_mqtt_unsubscribe(mqtt_client, topic_filter);
}

/**
 * @brief Check if MQTT topic has been subscribed or not
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to subscribe
 * @return true already subscribed
 * @return false not ready
 */
bool IOT_MQTT_IsSubReady(void *client, const char *topic_filter)
{
    POINTER_SANITY_CHECK(client, false);
    STRING_PTR_SANITY_CHECK(topic_filter, false);
    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;
    return qcloud_iot_mqtt_is_sub_ready(mqtt_client, topic_filter);
}

/**
 * @brief Get user data in subscribe.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to subscribe
 * @return NULL or user data
 */
void *IOT_MQTT_GetSubUsrData(void *client, const char *topic_filter)
{
    POINTER_SANITY_CHECK(client, NULL);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL);
    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;
    return qcloud_iot_mqtt_get_subscribe_usr_data(mqtt_client, topic_filter);
}

/**
 * @brief Subscribe and wait sub ready.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to subscribe
 * @param[in] params @see SubscribeParams
 * @return @see IotReturnCode
 */
int IOT_MQTT_SubscribeSync(void *client, const char *topic_filter, const SubscribeParams *params)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(params, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);
    return IOT_MQTT_Subscribe(client, topic_filter, params);
}

/**
 * @brief Check if MQTT is connected.
 *
 * @param[in,out] client pointer to mqtt client
 * @return true connected
 * @return false no connected
 */
bool IOT_MQTT_IsConnected(void *client)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;
    module_mqtt_get_connect_state(mqtt_client);
    return mqtt_client->connect_state != MQTT_CONNECT_STATE_DISCONNECT;
}

/**
 * @brief Get device info using to connect mqtt server.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see DeviceInfo
 */
DeviceInfo *IOT_MQTT_GetDeviceInfo(void *client)
{
    POINTER_SANITY_CHECK(client, NULL);
    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;
    return mqtt_client->device_info;
}
