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
 * @file ota_mqtt.c
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

#include "qcloud_iot_ota.h"

/**
 * @brief Context of OTA update topic, callback and user data.
 *
 */
typedef struct {
    IotOTAUpdateCallback callback;
    void *               usr_data;
} OTAUpdateContext;

/**
 * @brief OTA update type.
 *
 */
typedef enum {
    OTA_UPDATE_TYPE_REPORT_VERSION_RSP = 0,
    OTA_UPDATE_TYPE_UPDATE_FIRMWARE,
} OTAUpdateType;

/**
 * @brief Parse payload and callback.
 *
 * @param[in] type @see OTAUpdateType
 * @param[in] message message from cloud
 * @param[in] callback callback for user
 * @param[in,out] usr_data user data used in callback
 */
static void _parse_update_payload_and_callback(OTAUpdateType type, const MQTTMessage *message,
                                               const IotOTAUpdateCallback *callback, void *usr_data)
{
    int            rc, result_code;
    uint32_t       file_size = 0;
    UtilsJsonValue version, md5sum, url, value_file_size, value_result_code;

    // callback
    switch (type) {
        case OTA_UPDATE_TYPE_REPORT_VERSION_RSP:
            if (!callback->report_version_reply_callback) {
                return;
            }

            rc = utils_json_value_get("result_code", strlen("result_code"), message->payload_str, message->payload_len,
                                      &value_result_code);
            if (rc) {
                goto error;
            }
            rc = utils_json_value_data_get(value_result_code, UTILS_JSON_VALUE_TYPE_INT32, &result_code);
            if (rc) {
                goto error;
            }
            callback->report_version_reply_callback(result_code, usr_data);
            break;
        case OTA_UPDATE_TYPE_UPDATE_FIRMWARE:
            if (!callback->update_firmware_callback) {
                return;
            }

            rc = utils_json_value_get("version", strlen("version"), message->payload_str, message->payload_len,
                                      &version);
            rc |= utils_json_value_get("url", strlen("url"), message->payload_str, message->payload_len, &url);
            rc |= utils_json_value_get("md5sum", strlen("md5sum"), message->payload_str, message->payload_len, &md5sum);
            rc |= utils_json_value_get("file_size", strlen("file_size"), message->payload_str, message->payload_len,
                                       &value_file_size);
            rc |= utils_json_value_data_get(value_file_size, UTILS_JSON_VALUE_TYPE_UINT32, &file_size);
            if (rc) {
                goto error;
            }
            callback->update_firmware_callback(version, url, md5sum, file_size, usr_data);
            break;
        default:
            break;
    }
    return;
error:
    Log_e("invalid format of payload!");
    return;
}

/**
 * @brief Mqtt message callback for property topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see OTAUpdateContext
 */
static void _ota_mqtt_message_callback(void *client, const MQTTMessage *message, void *usr_data)
{
    const char *ota_update_str[] = {
        "report_version_rsp",  // OTA_UPDATE_TYPE_REPORT_VERSION_RSP
        "update_firmware",     // OTA_UPDATE_TYPE_UPDATE_FIRMWARE
    };

    int rc, i = 0;

    OTAUpdateContext *ota_update_context = (OTAUpdateContext *)usr_data;
    UtilsJsonValue    update_type;

    Log_d("receive ota message:%.*s", message->payload_len, message->payload_str);

    rc = utils_json_value_get("type", strlen("type"), message->payload_str, message->payload_len, &update_type);
    if (rc) {
        Log_e("invalid ota message!");
        return;
    }

    for (i = OTA_UPDATE_TYPE_REPORT_VERSION_RSP; i <= OTA_UPDATE_TYPE_UPDATE_FIRMWARE; i++) {
        if (!strncmp(update_type.value, ota_update_str[i], update_type.value_len)) {
            Log_d("callback ota message!");
            _parse_update_payload_and_callback(i, message, &ota_update_context->callback, ota_update_context->usr_data);
        }
    }
}

/**
 * @brief Check if topic already subscribed, if not then subscribe.
 *
 * @param[in,out] client pointer to mqtt client
 * @param callback @see IotOTAUpdateCallback
 * @param usr_data usr data in callback
 * @return @see IotReturnCode
 */
static int _ota_mqtt_update_topic_check_and_sub(void *client, IotOTAUpdateCallback callback, void *usr_data)
{
    int  rc = 0;
    char ota_result_topic[MAX_SIZE_OF_CLOUD_TOPIC];

    HAL_Snprintf(ota_result_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$ota/update/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    OTAUpdateContext *ota_update_context = (OTAUpdateContext *)HAL_Malloc(sizeof(OTAUpdateContext));
    if (!ota_update_context) {
        return QCLOUD_ERR_MALLOC;
    }
    ota_update_context->callback = callback;
    ota_update_context->usr_data = usr_data;

    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _ota_mqtt_message_callback;
    sub_params.qos                = QOS1;
    sub_params.user_data          = ota_update_context;
    sub_params.user_data_free     = HAL_Free;

    rc = IOT_MQTT_SubscribeSync(client, ota_result_topic, &sub_params);
    if (rc) {
        HAL_Free(ota_update_context);
    }
    return rc;
}

/**
 * @brief Unsubscribe topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
static int _ota_mqtt_update_topic_unsubscribe(void *client)
{
    char ota_result_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(ota_result_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$ota/update/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));
    return IOT_MQTT_Unsubscribe(client, ota_result_topic);
}

/**
 * @brief Publish to OTA report topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] qos @see QoS
 * @param[in] payload payload of mqtt packet
 * @param[in] payload_len payload len
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
static int _ota_mqtt_publish(void *client, QoS qos, const char *payload, int payload_len)
{
    char ota_report_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(ota_report_topic, sizeof(ota_report_topic), "$ota/report/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = qos;
    pub_params.payload       = (void *)payload;
    pub_params.payload_len   = payload_len;
    return IOT_MQTT_Publish(client, ota_report_topic, &pub_params);
}

/**
 * @brief OTA init, subscribe update topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotOTAUpdateCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_OTA_Init(void *client, IotOTAUpdateCallback callback, void *usr_data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    return _ota_mqtt_update_topic_check_and_sub(client, callback, usr_data);
}

/**
 * @brief OTA deinit, unsubscribe update topic.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_OTA_Deinit(void *client)
{
    POINTER_SANITY_CHECK_RTN(client);
    _ota_mqtt_update_topic_unsubscribe(client);
}

/**
 * @brief Report upgrade progress.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] report_type @see IotOTAReportType
 * @param[in] progress progress using in IOT_OTA_REPORT_TYPE_DOWNLOADING
 * @param[in] version update firmware version
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_OTA_ReportProgress(void *client, char *buf, int buf_len, IotOTAReportType report_type, int progress,
                           const char *version)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(buf_len, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(version, QCLOUD_ERR_INVAL);

    /**
     * @brief order @see IotOTAReportType
     *
     */
    const char *ota_state[]   = {"downloading", "burning", "done", "fail", "fail", "fail", "fail", "fail"};
    int         result_code[] = {0, 0, 0, -1, -2, -3, -4, -5};
    const char *result_msg[]  = {"", "", "", "timeout", "file not exit", "auth fail", "md5 not match", "upgrade fail"};

    int len;
    switch (report_type) {
        case IOT_OTA_REPORT_TYPE_DOWNLOADING:
            len = HAL_Snprintf(buf, buf_len,
                               "{\"type\":\"report_progress\",\"report\":{\"progress\":{\"state\":\"%s\",\"percent\":"
                               "\"%d\",\"result_code\":\"%d\",\"result_msg\":\"\"},\"version\":\"%s\"}}",
                               ota_state[report_type], progress, result_code[report_type], version);
            break;
        case IOT_OTA_REPORT_TYPE_UPGRADE_BEGIN:
        case IOT_OTA_REPORT_TYPE_UPGRADE_SUCCESS:
        case IOT_OTA_REPORT_TYPE_DOWNLOAD_TIMEOUT:
        case IOT_OTA_REPORT_TYPE_FILE_NOT_EXIST:
        case IOT_OTA_REPORT_TYPE_AUTH_FAIL:
        case IOT_OTA_REPORT_TYPE_MD5_NOT_MATCH:
        case IOT_OTA_REPORT_TYPE_UPGRADE_FAIL:
            len = HAL_Snprintf(buf, buf_len,
                               "{\"type\":\"report_progress\",\"report\":{\"progress\":{\"state\":\"%s\",\"result_"
                               "code\":\"%d\",\"result_msg\":\"%s\"},\"version\":\"%s\"}}",
                               ota_state[report_type], result_code[report_type], result_msg[report_type], version);
            break;
        default:
            return QCLOUD_ERR_INVAL;
    }
    return _ota_mqtt_publish(client, QOS0, buf, len);
}

/**
 * @brief Report current firmware version.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] version current firmware version
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_OTA_ReportVersion(void *client, char *buf, int buf_len, const char *version)
{
    int len = HAL_Snprintf(buf, buf_len, "{\"type\":\"report_version\",\"report\":{\"version\":\"%s\"}}",
                           STRING_PTR_PRINT_SANITY_CHECK(version));
    return _ota_mqtt_publish(client, QOS0, buf, len);
}
