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
 * @file gateway_mqtt.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-05-10
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-05-10 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_gateway.h"

#include "utils_hmac.h"
#include "utils_base64.h"

// ----------------------------------------------------------------------------
// downstream message
// ----------------------------------------------------------------------------

/**
 * @brief Context of gateway callback and user data.
 *
 */
typedef struct {
    IotGatewayMessageCallback callback;
    void                     *usr_data;
} GatewayContext;

/**
 * @brief Gateway message type.
 *
 */
typedef enum {
    TYPE_BIND = 0,
    TYPE_UNBIND,
    TYPE_UNBIND_ALL,
    TYPE_ONLINE,
    TYPE_OFFLINE,
    TYPE_SEARCH_DEVICES,
    TYPE_DESCRIBE_SUB_DEVICES,
    TYPE_CHANGE,
} GatewayMessageType;

/**
 * @brief Get message type.
 *
 * @param[in] json_buf
 * @param[in] buf_len
 * @return < 0 for fail, others @see GatewayMessageType
 */
static int _parse_gateway_message_type(const char *json_buf, int buf_len)
{
    UtilsJsonValue type;
    // parse type
    int rc = utils_json_value_get("type", sizeof("type") - 1, json_buf, buf_len, &type);
    if (rc) {
        Log_e("parse gateway msg type error");
        return rc;
    }

    /**
     * @brief Order @see GatewayMessageType
     *
     */
    const char *gateway_type_str[] = {
        [TYPE_BIND]                 = "bind",
        [TYPE_UNBIND]               = "unbind",
        [TYPE_UNBIND_ALL]           = "unbind_all",
        [TYPE_ONLINE]               = "online",
        [TYPE_OFFLINE]              = "offline",
        [TYPE_SEARCH_DEVICES]       = "search_devices",
        [TYPE_DESCRIBE_SUB_DEVICES] = "describe_sub_devices",
        [TYPE_CHANGE]               = "change",
    };

    for (int i = 0; i < sizeof(gateway_type_str) / sizeof(const char *); i++) {
        if (!strncmp(type.value, gateway_type_str[i], type.value_len)) {
            return i;
        }
    }
    return QCLOUD_ERR_FAILURE;
}

static void _callback_gateway(GatewayMessageType msg_type, IotGatewayMessageCallback *callback,
                              UtilsJsonValue sub_devices, int status, void *usr_data)
{
    switch (msg_type) {
        case TYPE_BIND:
        case TYPE_UNBIND:
            if (callback->bind_unbind_reply_callback) {
                callback->bind_unbind_reply_callback(sub_devices, msg_type == TYPE_BIND, usr_data);
            }
            break;
        case TYPE_UNBIND_ALL:
            if (callback->unbind_all_callback) {
                callback->unbind_all_callback(usr_data);
            }
            break;
        case TYPE_ONLINE:
        case TYPE_OFFLINE:
            if (callback->online_offline_reply_callback) {
                callback->online_offline_reply_callback(sub_devices, msg_type == TYPE_ONLINE, usr_data);
            }
            break;
        case TYPE_SEARCH_DEVICES:
            if (callback->search_device_callback) {
                callback->search_device_callback(status, usr_data);
            }
            break;
        case TYPE_DESCRIBE_SUB_DEVICES:
            if (callback->describe_subdevices_reply_callback) {
                callback->describe_subdevices_reply_callback(sub_devices, usr_data);
            }
            break;
        case TYPE_CHANGE:
            // status : 0:unbind 1:bind
            if (callback->change_subdevices_status_callback) {
                callback->change_subdevices_status_callback(sub_devices, status, usr_data);
            }
            break;
        default:
            Log_e("unsupported now");
            break;
    }
}

/**
 * @brief Gateway downstream messsage handle.
 *
 * @param[in] client pointer to mqtt client
 * @param[in] message @see MQTTMessage
 * @param[in] user_data
 */
static void _gateway_mqtt_message_handler(void *client, const MQTTMessage *message, void *user_data)
{
    int rc = 0;
    Log_d("receive gateway message %.*s", message->payload_len, message->payload_str);

    int msg_type = _parse_gateway_message_type(message->payload_str, message->payload_len);
    if (msg_type < 0) {
        return;
    }
    GatewayContext *gateway_context = user_data;
    Log_i("get type %d from server", msg_type);

    UtilsJsonValue sub_devices;
    int32_t        status;

    if (msg_type != TYPE_SEARCH_DEVICES && msg_type != TYPE_UNBIND_ALL) {
        rc = utils_json_value_get("payload.devices", sizeof("payload.devices") - 1, message->payload_str,
                                  message->payload_len, &sub_devices);
        if (rc) {
            Log_e("invalid gateway payload.");
            return;
        }
    }

    if (msg_type == TYPE_SEARCH_DEVICES || msg_type == TYPE_CHANGE) {
        rc = utils_json_get_int32("payload.status", sizeof("payload.status") - 1, message->payload_str,
                                  message->payload_len, &status);
        if (rc) {
            return;
        }
    }

    _callback_gateway(msg_type, &gateway_context->callback, sub_devices, status, gateway_context->usr_data);
}

/**
 * @brief Subscribe gateway result topic.
 *
 * @param[in] client pointer to mqtt client
 * @param[in] callback @see IotGatewayMessageCallback
 * @param[in] usr_data
 * @return @see IotReturnCode
 */
static int _gateway_mqtt_subscribe(void *client, IotGatewayMessageCallback callback, void *usr_data)
{
    GatewayContext *gateway_context = HAL_Malloc(sizeof(GatewayContext));
    if (!gateway_context) {
        return QCLOUD_ERR_MALLOC;
    }
    gateway_context->callback = callback;
    gateway_context->usr_data = usr_data;

    char gateway_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(gateway_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$gateway/operation/result/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    SubscribeParams sub_param    = DEFAULT_SUB_PARAMS;
    sub_param.on_message_handler = _gateway_mqtt_message_handler;
    sub_param.user_data          = gateway_context;
    sub_param.user_data_free     = HAL_Free;

    int rc = IOT_MQTT_SubscribeSync(client, gateway_topic, &sub_param);
    if (rc) {
        HAL_Free(gateway_context);
    }
    return rc;
}

// ----------------------------------------------------------------------------
// server mqtt context
// ----------------------------------------------------------------------------

/**
 * @brief Calculate bind signature
 *
 * @param[in] subdev_info subdev info
 * @param[in] nonce random
 * @param[in] timestamp timestamp
 * @param[out] signature signature
 * @return 0 for success
 */
static int _subdev_bind_hmac_sha1_cal(const DeviceInfo *subdev_info, int nonce, uint64_t timestamp, char *signature)
{
#define MAX_SIGN_BUF_SIZE (MAX_SIZE_OF_CLIENT_ID + 48)
    size_t olen = 0;

    char sign_buf[MAX_SIGN_BUF_SIZE] = {0};
    int  sign_buf_len;
    char sign[SHA1_DIGEST_SIZE] = {0};

    /* 1. sign fmt : ${product_id}${device_name};${random};${expiration_time} */
    sign_buf_len = HAL_Snprintf(sign_buf, sizeof(sign_buf), "%s%s;%d;%llu", subdev_info->product_id,
                                subdev_info->device_name, nonce, timestamp);
    /* 2. cal hmac sha1 */
    utils_hmac_sha1(sign_buf, sign_buf_len, (const uint8_t *)subdev_info->device_secret,
                    strlen(subdev_info->device_secret), sign);
    /* 3. base64 encode */
    return utils_base64encode((uint8_t *)signature, 31, &olen, (const uint8_t *)sign, SHA1_DIGEST_SIZE);
}

/**
 * @brief Publish gateway message.
 *
 * @param[in] client pointer to mqtt client
 * @param[in] qos @see QoS
 * @param[in] payload message payload
 * @param[in] payload_len payload length
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
static int _gateway_mqtt_publish(void *client, QoS qos, const char *payload, int payload_len)
{
    char gateway_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(gateway_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$gateway/operation/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = qos;
    pub_params.payload       = (void *)payload;
    pub_params.payload_len   = payload_len;
    return IOT_MQTT_Publish(client, gateway_topic, &pub_params);
}

// ----------------------------------------------------------------------------
// API
// ----------------------------------------------------------------------------

/**
 * @brief Subscribe gateway topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotGatewayMessageCallback
 * @param[in] usr_data usr data using in callback
 * @return @see IotReturnCode
 */
int IOT_Gateway_Init(void *client, IotGatewayMessageCallback callback, void *usr_data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    return _gateway_mqtt_subscribe(client, callback, usr_data);
}

/**
 * @brief Unsubscribe gateway topic.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_Gateway_Deinit(void *client)
{
    POINTER_SANITY_CHECK_RTN(client);

    char gateway_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(gateway_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$gateway/operation/result/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));
    IOT_MQTT_Unsubscribe(client, gateway_topic);
}

/**
 * @brief Get gateway init usr data.
 *
 * @param [in,out] client pointer to mqtt client
 * @return usr data or NULL
 */
void *IOT_Gateway_GetUsrData(void *client)
{
    POINTER_SANITY_CHECK(client, NULL);
    char gateway_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(gateway_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$gateway/operation/result/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));
    GatewayContext *gateway_context = IOT_MQTT_GetSubUsrData(client, gateway_topic);
    if (gateway_context && gateway_context->usr_data) {
        return gateway_context->usr_data;
    }
    return NULL;
}

/**
 * @brief Publish subdevice online/offline message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to hold publish message
 * @param[in] buf_len buffer length
 * @param[in] sub_dev_list subdevice list
 * @param[in] num num of subdevice, @note only one subdevice is supported now.
 * @param[in] is_online 1: online; 0: offline
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_SubOnOffLine(void *client, char *buf, int buf_len, const DeviceInfo *sub_dev_list[], int num,
                             bool is_online)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(sub_dev_list, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(*sub_dev_list, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(num, QCLOUD_ERR_INVAL);
    if (num > 1) {
        Log_e("Only one subdevice is supported!");
        return QCLOUD_ERR_INVAL;
    }

    int len;
    len = HAL_Snprintf(buf, buf_len, "{\"type\":\"%s\",\"payload\":{\"devices\":[", is_online ? "online" : "offline");
    for (int i = 0; i < num; i++) {
        len += HAL_Snprintf(buf + len, buf_len - len, "{\"product_id\":\"%s\",\"device_name\":\"%s\"},",
                            sub_dev_list[i]->product_id, sub_dev_list[i]->device_name);
    }
    len += HAL_Snprintf(buf + len - 1, buf_len - len + 1, "]}}");  // -1 remove ','
    return _gateway_mqtt_publish(client, QOS0, buf, len);
}

/**
 * @brief Publish subdevice bind/unbind message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to hold publish message
 * @param[in] buf_len buffer length
 * @param[in] sub_dev_list subdevice list
 * @param[in] num num of subdevice, @note only one subdevice is supported now.
 * @param[in] is_bind 1: bind; 0: unbind
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_BindUnbind(void *client, char *buf, int buf_len, const DeviceInfo *sub_dev_list[], int num,
                           bool is_bind)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(sub_dev_list, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(num, QCLOUD_ERR_INVAL);

    if (num > 1) {
        Log_e("Only one subdevice is supported!");
        return QCLOUD_ERR_INVAL;
    }

    srand(IOT_Timer_CurrentSec());
    int      nonce     = rand();
    uint64_t timestamp = IOT_Timer_CurrentSec();

    char signature[31] = {0};
    int  rc, len;
    len = HAL_Snprintf(buf, buf_len, "{\"type\":\"%s\",\"payload\":{\"devices\":[", is_bind ? "bind" : "unbind");
    for (int i = 0; i < num; i++) {
        if (is_bind) {
            rc = _subdev_bind_hmac_sha1_cal(sub_dev_list[i] + i, nonce, timestamp, signature);
            if (rc) {
                Log_e("calculate signature fail");
                return QCLOUD_ERR_FAILURE;
            }
            len += HAL_Snprintf(buf + len, buf_len - len,
                                "{\"product_id\":\"%s\",\"device_name\":\"%s\",\"signature\":\"%s\",\"random\":%d,"
                                "\"timestamp\":%lld,\"signmethod\":\"hmacsha1\",\"authtype\":\"psk\"},",
                                sub_dev_list[i]->product_id, sub_dev_list[i]->device_name, signature, nonce, timestamp);
        } else {
            len += HAL_Snprintf(buf + len, buf_len - len, "{\"product_id\":\"%s\",\"device_name\":\"%s\"},",
                                sub_dev_list[i]->product_id, sub_dev_list[i]->device_name);
        }
    }
    len += HAL_Snprintf(buf + len - 1, buf_len - len + 1, "]}}");  // -1 remove ','
    return _gateway_mqtt_publish(client, QOS0, buf, len);
}

/**
 * @brief Publish subdevice describe message.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_Describe(void *client)
{
#define GATEWAY_UP_DESCRIBE "{\"type\":\"describe_sub_devices\"}"
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    return _gateway_mqtt_publish(client, QOS0, GATEWAY_UP_DESCRIBE, sizeof(GATEWAY_UP_DESCRIBE) - 1);
}

/**
 * @brief Publish search device reply message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] is_on 1: on; 0: off
 * @param[in] result 0: success; 1: fail
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_SearchDeviceReply(void *client, bool is_on, int result)
{
    char buf[64];
    int  len = HAL_Snprintf(buf, sizeof(buf), "{\"type\":\"search_devices\",\"payload\":{\"status\":%d,\"result\":%d}}",
                            is_on, result);
    return _gateway_mqtt_publish(client, QOS0, buf, len);
}

/**
 * @brief Publish unbind all reply message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] result 0: success; 1: fail
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_UnbindAllReply(void *client, int result)
{
    char buf[64];
    int  len = HAL_Snprintf(buf, sizeof(buf), "{\"type\":\"unbind_all\",\"payload\":{\"result\":%d}}", result);
    return _gateway_mqtt_publish(client, QOS0, buf, len);
}

#if 0  // TODO: no need now for useless
int IOT_Gateway_ChangeReply(void *client, char *buf, int buf_len, DeviceInfo *sub_dev_list, int num, int status)
{
    char buf[64];
    int  len = HAL_Snprintf(buf, sizeof(buf), "{\"type\":\"change\",\"payload\":{\"status\":%d,\"devices\":[", status);
    for (int i = 0; i < num; i++) {
        len += HAL_Snprintf(buf + len, buf_len - len, "{\"product_id\":\"%s\",\"device_name\":\"%s\"},",
                            sub_dev_list[i].product_id, sub_dev_list[i].device_name);
    }
    len += HAL_Snprintf(buf + len - 1, buf_len - len + 1, "]}}");  // -1 remove ','
    return _gateway_mqtt_publish(client, QOS0, buf, len);
}
#endif
