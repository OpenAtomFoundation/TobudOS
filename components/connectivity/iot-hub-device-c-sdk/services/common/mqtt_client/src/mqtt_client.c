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
 * @file mqtt_client.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-07 <td>1.1     <td>fancyxu   <td>support user host for unittest
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotReturnCode and QcloudIotClient
 * </table>
 */

#include "mqtt_client.h"

#define MQTT_VERSION_3_1_1 4
// 20 for timestamp length & delimiter
#define MAX_MQTT_CONNECT_USR_NAME_LEN (MAX_SIZE_OF_CLIENT_ID + QCLOUD_IOT_DEVICE_SDK_APPID_LEN + MAX_CONN_ID_LEN + 20)
#define MAX_MQTT_CONNECT_PASSWORD_LEN (51)

/**************************************************************************************
 * static method
 **************************************************************************************/

/**
 * @brief Get random packet id, when start.
 *
 * @return packet id
 */
static uint16_t _get_random_start_packet_id(void)
{
    srand(HAL_Timer_CurrentSec());
    return rand() % 65536 + 1;
}

/**
 * @brief Init list_pub_wait_ack and list_sub_wait_ack
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
static int _mqtt_client_list_init(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;

    UtilsListFunc func = {
        .list_malloc      = HAL_Malloc,
        .list_free        = HAL_Free,
        .list_lock_init   = HAL_MutexCreate,
        .list_lock_deinit = HAL_MutexDestroy,
        .list_lock        = HAL_MutexLock,
        .list_unlock      = HAL_MutexUnlock,
    };

    client->list_pub_wait_ack = utils_list_create(func, MAX_REPUB_NUM);
    if (!client->list_pub_wait_ack) {
        Log_e("create pub wait list failed.");
        goto error;
    }

    client->list_sub_wait_ack = utils_list_create(func, MAX_MESSAGE_HANDLERS);
    if (!client->list_sub_wait_ack) {
        Log_e("create sub wait list failed.");
        goto error;
    }
    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
error:
    utils_list_destroy(client->list_pub_wait_ack);
    client->list_pub_wait_ack = NULL;
    utils_list_destroy(client->list_sub_wait_ack);
    client->list_sub_wait_ack = NULL;
    IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
}

/**
 * @brief Init network, tcp(with AUTH_WITH_NO_TLS) or tls.
 *
 * @param[in,out] client pointer to mqtt client
 */
static void _mqtt_client_network_init(QcloudIotClient *client, const MQTTInitParams *params)
{
    HAL_Snprintf(client->host_addr, HOST_STR_LENGTH, "%s.%s", client->device_info->product_id,
                 params->host ? params->host : QCLOUD_IOT_MQTT_DIRECT_DOMAIN);
    client->main_host        = params->host;
    client->backup_host      = params->backup_host;
    client->get_next_host_ip = params->get_next_host_ip;
#ifndef AUTH_WITH_NO_TLS
    // device param for TLS connection
#ifdef AUTH_MODE_CERT
    Log_d("cert file: %s", STRING_PTR_PRINT_SANITY_CHECK(client->device_info->cert_file));
    Log_d("key file: %s", STRING_PTR_PRINT_SANITY_CHECK(client->device_info->key_file));

    client->network_stack.ssl_connect_params.cert_file  = client->device_info->cert_file;
    client->network_stack.ssl_connect_params.key_file   = client->device_info->key_file;
    client->network_stack.ssl_connect_params.ca_crt     = iot_ca_get();
    client->network_stack.ssl_connect_params.ca_crt_len = strlen(client->network_stack.ssl_connect_params.ca_crt);
#else
    client->network_stack.ssl_connect_params.psk        = (char *)client->device_info->device_secret_decode;
    client->network_stack.ssl_connect_params.psk_length = client->device_info->device_secret_decode_len;
    client->network_stack.ssl_connect_params.psk_id     = client->device_info->client_id;
    client->network_stack.ssl_connect_params.ca_crt     = NULL;
    client->network_stack.ssl_connect_params.ca_crt_len = 0;
#endif
    client->network_stack.host                          = client->host_addr;
    client->network_stack.port                          = MQTT_SERVER_PORT_TLS;
    client->network_stack.ssl_connect_params.timeout_ms = client->command_timeout_ms > QCLOUD_IOT_TLS_HANDSHAKE_TIMEOUT
                                                              ? client->command_timeout_ms
                                                              : QCLOUD_IOT_TLS_HANDSHAKE_TIMEOUT;
    client->network_stack.type                          = IOT_NETWORK_TYPE_TLS;
#else
    client->network_stack.host = client->host_addr;
    client->network_stack.port = MQTT_SERVER_PORT_NO_TLS;
    client->network_stack.type = IOT_NETWORK_TYPE_TCP;
#endif
    qcloud_iot_network_init(&(client->network_stack));
}

/**
 * @brief Init mqtt connection options.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] params mqtt init params, @see MQTTInitParams
 * @return int
 */
static int _mqtt_client_connect_option_init(QcloudIotClient *client, const MQTTInitParams *params)
{
    IOT_FUNC_ENTRY;

    int  rc          = 0;
    long cur_timesec = 0;

    client->options.mqtt_version = MQTT_VERSION_3_1_1;
    // Upper limit of keep alive interval is (11.5 * 60) seconds
    client->options.client_id = client->device_info->client_id;
    client->options.keep_alive_interval =
        params->keep_alive_interval_ms / 1000 > 690 ? 690 : params->keep_alive_interval_ms / 1000;
    client->options.clean_session = params->clean_session;

    // calculate user name & password
    client->options.username = (char *)HAL_Malloc(MAX_MQTT_CONNECT_USR_NAME_LEN);
    if (!client->options.username) {
        Log_e("malloc username failed!");
        rc = QCLOUD_ERR_MALLOC;
        goto error;
    }

    cur_timesec =
        (MAX_ACCESS_EXPIRE_TIMEOUT <= 0) ? 0x7fffffffL : (HAL_Timer_CurrentSec() + MAX_ACCESS_EXPIRE_TIMEOUT / 1000);
    get_next_conn_id(client->conn_id);
    HAL_Snprintf(client->options.username, MAX_MQTT_CONNECT_USR_NAME_LEN, "%s;%s;%s;%ld", client->options.client_id,
                 QCLOUD_IOT_DEVICE_SDK_APPID, client->conn_id, cur_timesec);

#if defined(AUTH_WITH_NO_TLS) && defined(AUTH_MODE_KEY)
    char sign[41]            = {0};
    client->options.password = (char *)HAL_Malloc(MAX_MQTT_CONNECT_PASSWORD_LEN);
    if (!client->options.password) {
        Log_e("malloc password failed!");
        rc = QCLOUD_ERR_MALLOC;
        goto error;
    }
    utils_hmac_sha1(client->options.username, strlen(client->options.username),
                    client->device_info->device_secret_decode, client->device_info->device_secret_decode_len, sign);
    HAL_Snprintf(client->options.password, MAX_MQTT_CONNECT_PASSWORD_LEN, "%s;hmacsha1", sign);
#endif
    IOT_FUNC_EXIT_RC(rc);
error:
    HAL_Free(client->options.username);
    client->options.username = NULL;
    HAL_Free(client->options.password);
    client->options.password = NULL;
    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Init mqtt client.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] params mqtt init params, @see MQTTInitParams
 * @return @see IotReturnCode
 *
 * @note
 * 1. init device info.
 * 2. init mutex and var
 * 3. init list @see _mqtt_client_list_init
 * 4. init connect option @see _mqtt_client_connect_option_init
 * 5. init network @see _mqtt_client_network_init
 */
static int _qcloud_iot_mqtt_client_init(QcloudIotClient *client, const MQTTInitParams *params)
{
    IOT_FUNC_ENTRY;
    int rc = 0;
    memset(client, 0x0, sizeof(QcloudIotClient));

    // set device info
    client->device_info = params->device_info;
    rc = HAL_Snprintf(client->device_info->client_id, MAX_SIZE_OF_CLIENT_ID, "%s%s", client->device_info->product_id,
                      client->device_info->device_name);
#ifdef AUTH_MODE_KEY
    utils_base64decode(client->device_info->device_secret_decode, MAX_SIZE_OF_DECODE_PSK_LENGTH,
                       &client->device_info->device_secret_decode_len, (uint8_t *)client->device_info->device_secret,
                       strlen(client->device_info->device_secret));
#endif
    Log_i("SDK_Ver: %s, Product_ID: %s, Device_Name: %s", QCLOUD_IOT_DEVICE_SDK_VERSION,
          client->device_info->product_id, client->device_info->device_name);

    // command timeout should be in range [MIN_COMMAND_TIMEOUT, MAX_COMMAND_TIMEOUT]
    client->command_timeout_ms =
        (params->command_timeout < MIN_COMMAND_TIMEOUT)
            ? MIN_COMMAND_TIMEOUT
            : (params->command_timeout > MAX_COMMAND_TIMEOUT ? MAX_COMMAND_TIMEOUT : params->command_timeout);

    // packet id, random from [1 - 65536]
    client->next_packet_id      = _get_random_start_packet_id();
    client->write_buf_size      = QCLOUD_IOT_MQTT_TX_BUF_LEN;
    client->read_buf_size       = QCLOUD_IOT_MQTT_RX_BUF_LEN;
    client->event_handle        = params->event_handle;
    client->auto_connect_enable = params->auto_connect_enable;
    client->default_subscribe   = params->default_subscribe;

    client->lock_generic = HAL_MutexCreate();
    if (!client->lock_generic) {
        goto error;
    }

    client->lock_write_buf = HAL_MutexCreate();
    if (!client->lock_write_buf) {
        goto error;
    }

    rc = _mqtt_client_list_init(client);
    if (rc) {
        goto error;
    }

    rc = _mqtt_client_connect_option_init(client, params);
    if (rc) {
        goto error;
    }

    _mqtt_client_network_init(client, params);
    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);

error:
    HAL_MutexDestroy(client->lock_generic);
    client->lock_generic = NULL;
    HAL_MutexDestroy(client->lock_write_buf);
    client->lock_write_buf = NULL;
    IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
}

/**
 * @brief Deinit mqtt client.
 *
 * @param[in,out] client pointer to mqtt client
 */
static void _qcloud_iot_mqtt_client_deinit(QcloudIotClient *client)
{
    HAL_Free(client->options.username);
    HAL_Free(client->options.password);
    HAL_MutexDestroy(client->lock_generic);
    HAL_MutexDestroy(client->lock_write_buf);
    qcloud_iot_mqtt_sub_handle_array_clear(client);
    qcloud_iot_mqtt_suback_wait_list_clear(client);
    utils_list_destroy(client->list_pub_wait_ack);
    utils_list_destroy(client->list_sub_wait_ack);
    Log_i("release mqtt client resources");
}

/**************************************************************************************
 * API
 **************************************************************************************/

/**
 * @brief Create MQTT client and connect to MQTT server.
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

    rc = qcloud_iot_mqtt_connect(client, FIRST_CONNECT);
    if (rc) {
        Log_e("mqtt connect with id: %s failed: %d", STRING_PTR_PRINT_SANITY_CHECK(client->conn_id), rc);
        goto exit;
    }

    Log_i("mqtt connect with id: %s success", client->conn_id);
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
    if (!get_client_conn_state(client)) {
        IOT_FUNC_EXIT_RC(qcloud_iot_mqtt_connect(client, FIRST_CONNECT));
    }
    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
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

    int rc = qcloud_iot_mqtt_disconnect(mqtt_client);
    if (rc) {
        // disconnect network stack by force
        mqtt_client->network_stack.disconnect(&(mqtt_client->network_stack));
        set_client_conn_state(mqtt_client, NOTCONNECTED);
    }

    _qcloud_iot_mqtt_client_deinit(mqtt_client);

    HAL_Free(*client);
    *client = NULL;
    Log_i("mqtt release!");
    return rc;
}

/**
 * @brief Check connection and keep alive state, read/handle MQTT packet in synchronized way.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] timeout_ms timeout value (unit: ms) for this operation
 * @return QCLOUD_RET_SUCCESS when success, QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT when try reconnecting, others @see
 * IotReturnCode
 */
int IOT_MQTT_Yield(void *client, uint32_t timeout_ms)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;
    return qcloud_iot_mqtt_yield(mqtt_client, timeout_ms);
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

    if (!get_client_conn_state(client)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    if (strlen(topic_name) > MAX_SIZE_OF_CLOUD_TOPIC) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
    }

    if (QOS2 == params->qos) {
        Log_e("QoS2 is not supported currently");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT);
    }

    return qcloud_iot_mqtt_publish(mqtt_client, topic_name, params);
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

    if (!get_client_conn_state(client) && !mqtt_client->default_subscribe) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    if (strlen(topic_filter) > MAX_SIZE_OF_CLOUD_TOPIC) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
    }

    if (QOS2 == params->qos) {
        Log_e("QoS2 is not supported currently");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT);
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

    if (!get_client_conn_state(client)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    if (strlen(topic_filter) > MAX_SIZE_OF_CLOUD_TOPIC) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
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

    int rc;

    QcloudIotClient *mqtt_client = (QcloudIotClient *)client;

    int cnt_sub = mqtt_client->command_timeout_ms / QCLOUD_IOT_MQTT_YIELD_TIMEOUT;

    if (IOT_MQTT_IsSubReady(client, topic_filter)) {
        // if already sub, free the user data
        if (params->user_data_free) {
            params->user_data_free(params->user_data);
        }
        return QCLOUD_RET_SUCCESS;
    }

    rc = IOT_MQTT_Subscribe(client, topic_filter, params);
    if (rc < 0) {
        Log_e("topic subscribe failed: %d, cnt: %d", rc, cnt_sub);
        return rc;
    }

    while (cnt_sub-- >= 0 && rc >= 0 && !IOT_MQTT_IsSubReady(client, topic_filter)) {
        /**
         * @brief wait for subscription result
         *
         */
        rc = IOT_MQTT_Yield(client, QCLOUD_IOT_MQTT_YIELD_TIMEOUT);
    }
    return IOT_MQTT_IsSubReady(client, topic_filter) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_FAILURE;
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
    return get_client_conn_state(mqtt_client);
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
