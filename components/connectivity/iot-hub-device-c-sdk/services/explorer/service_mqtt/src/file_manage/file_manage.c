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
 * @file file_manage.c
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

#include "qcloud_iot_file_manage.h"

#include "service_mqtt.h"

/**
 * @brief Context of file manage, callback and user data.
 *
 */
typedef struct {
    IotFileManageCallback callback;
    void                 *usr_data;
} FileManageContext;

/**
 * @brief File manage down stream message type.
 *
 */
typedef enum {
    FILE_MANAGE_DOWN_MESSAGE_TYPE_UPDATE = 0,
    FILE_MANAGE_DOWN_MESSAGE_TYPE_DEL,
    FILE_MANAGE_DOWN_MESSAGE_TYPE_REPORT_VERSION_RESPONSE,
    FILE_MANAGE_DOWN_MESSAGE_TYPE_POST_REQUEST_RESPONSE,
} FileManageDownMessageType;

/**
 * @brief Method string of down stream message. Order @see FileManageDownMessageType.
 *
 */
static const char *sg_file_manage_method_str[] = {
    "update_resource",     // FILE_MANAGE_DOWN_MESSAGE_TYPE_UPDATE
    "del_resource",        // FILE_MANAGE_DOWN_MESSAGE_TYPE_DEL
    "report_version_rsp",  // FILE_MANAGE_DOWN_MESSAGE_TYPE_REPORT_VERSION_RESPONSE
    "request_url_resp",    // FILE_MANAGE_DOWN_MESSAGE_TYPE_POST_REQUEST_RESPONSE
};

/**
 * @brief Method string of down stream message. Order @see IotFileManageFileType.
 *
 */
static const char *sg_file_manage_file_type_str[] = {
    "FILE",   // IOT_FILE_MANAGE_FILE_TYPE_FILE
    "AUDIO",  // IOT_FILE_MANAGE_FILE_TYPE_AUDIO
    "VOICE",  // IOT_FILE_MANAGE_FILE_TYPE_VOICE
    "VIDEO",  // IOT_FILE_MANAGE_FILE_TYPE_VIDEO
};

/**
 * @brief Handle file mange down stream message.
 *
 * @param[in] type @see FileManageDownMessageType
 * @param[in] message @see MQTTMessage
 * @param[in] callback @see IotFileManageCallback
 * @param[in,out] usr_data user define in IOT_FileManage_Init
 */
static void _parse_update_payload_and_callback(FileManageDownMessageType type, const MQTTMessage *message,
                                               const IotFileManageCallback *callback, void *usr_data)
{
    int            rc, result_code;
    uint32_t       file_size = 0;
    UtilsJsonValue file_name, file_type, version, md5sum, url, value_result_code, file_token, file_list = {0};

    // callback
    switch (type) {
        case FILE_MANAGE_DOWN_MESSAGE_TYPE_UPDATE:
            if (!callback->update_file_callback) {
                return;
            }
            rc = utils_json_value_get("resource_name", sizeof("resource_name") - 1, message->payload_str,
                                      message->payload_len, &file_name);
            rc |= utils_json_value_get("resource_type", sizeof("resource_type") - 1, message->payload_str,
                                       message->payload_len, &file_type);
            rc |= utils_json_value_get("version", sizeof("version") - 1, message->payload_str, message->payload_len,
                                       &version);
            rc |= utils_json_value_get("url", sizeof("url") - 1, message->payload_str, message->payload_len, &url);
            rc |= utils_json_value_get("md5sum", sizeof("md5sum") - 1, message->payload_str, message->payload_len,
                                       &md5sum);
            rc |= utils_json_get_uint32("file_size", sizeof("file_size") - 1, message->payload_str,
                                        message->payload_len, &file_size);
            if (rc) {
                goto error;
            }
            callback->update_file_callback(file_name, file_type, version, url, md5sum, file_size, usr_data);
            break;
        case FILE_MANAGE_DOWN_MESSAGE_TYPE_DEL:
            if (!callback->del_file_callback) {
                return;
            }
            rc = utils_json_value_get("resource_name", sizeof("resource_name") - 1, message->payload_str,
                                      message->payload_len, &file_name);
            rc |= utils_json_value_get("resource_type", sizeof("resource_type") - 1, message->payload_str,
                                       message->payload_len, &file_type);
            rc |= utils_json_value_get("version", sizeof("version") - 1, message->payload_str, message->payload_len,
                                       &version);
            if (rc) {
                goto error;
            }
            callback->del_file_callback(file_name, file_type, version, usr_data);
            break;
        case FILE_MANAGE_DOWN_MESSAGE_TYPE_REPORT_VERSION_RESPONSE:
            if (!callback->report_file_version_reponse_callback) {
                return;
            }
            // maybe no resource
            utils_json_value_get("resource_list", sizeof("resource_list") - 1, message->payload_str,
                                 message->payload_len, &file_list);
            rc = utils_json_value_get("result_code", sizeof("result_code") - 1, message->payload_str,
                                      message->payload_len, &value_result_code);
            rc |= utils_json_value_data_get(value_result_code, UTILS_JSON_VALUE_TYPE_INT32, &result_code);
            if (rc) {
                goto error;
            }
            callback->report_file_version_reponse_callback(file_list, result_code, usr_data);
            break;
        case FILE_MANAGE_DOWN_MESSAGE_TYPE_POST_REQUEST_RESPONSE:
            if (!callback->request_file_url_response_callback) {
                return;
            }
            rc = utils_json_value_get("resource_url", sizeof("resource_url") - 1, message->payload_str,
                                      message->payload_len, &url);
            rc |= utils_json_value_get("resource_token", sizeof("resource_token") - 1, message->payload_str,
                                       message->payload_len, &file_token);
            rc |= utils_json_value_get("result_code", sizeof("result_code") - 1, message->payload_str,
                                       message->payload_len, &value_result_code);
            rc |= utils_json_value_data_get(value_result_code, UTILS_JSON_VALUE_TYPE_INT32, &result_code);
            if (rc) {
                goto error;
            }
            callback->request_file_url_response_callback(file_list, file_token, result_code, usr_data);
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
 * @brief Mqtt message callback for file manage.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see FileManageContext
 */
static void _file_manage_message_callback(void *client, const MQTTMessage *message, void *usr_data)
{
    const char *file_manage_method_str[] = {
        "update_resource",     // FILE_MANAGE_DOWN_MESSAGE_TYPE_UPDATE
        "del_resource",        // FILE_MANAGE_DOWN_MESSAGE_TYPE_DEL
        "report_version_rsp",  // FILE_MANAGE_DOWN_MESSAGE_TYPE_REPORT_VERSION_RESPONSE
        "request_url_resp",    // FILE_MANAGE_DOWN_MESSAGE_TYPE_POST_REQUEST_RESPONSE
    };

    int rc, i = 0;

    FileManageContext *file_manage_context = (FileManageContext *)usr_data;
    UtilsJsonValue     method;

    Log_d("receive file manage message:%.*s", message->payload_len, message->payload_str);

    rc = utils_json_value_get("method", sizeof("method") - 1, message->payload_str, message->payload_len, &method);
    if (rc) {
        Log_e("invalid file manage message!");
        return;
    }

    for (i = FILE_MANAGE_DOWN_MESSAGE_TYPE_UPDATE; i <= FILE_MANAGE_DOWN_MESSAGE_TYPE_POST_REQUEST_RESPONSE; i++) {
        if (!strncmp(method.value, file_manage_method_str[i], method.value_len)) {
            Log_d("callback file manage message!");
            _parse_update_payload_and_callback(i, message, &file_manage_context->callback,
                                               file_manage_context->usr_data);
        }
    }
}

/**
 * @brief File manage init, register handler to server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotFileManageCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_Init(void *client, IotFileManageCallback callback, void *usr_data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    int rc = service_mqtt_init(client);
    if (rc) {
        return rc;
    }

    FileManageContext *file_manage_context = (FileManageContext *)HAL_Malloc(sizeof(FileManageContext));
    if (!file_manage_context) {
        return QCLOUD_ERR_MALLOC;
    }
    file_manage_context->callback = callback;
    file_manage_context->usr_data = usr_data;

    ServiceRegisterParams params = {
        .type           = SERVICE_TYPE_FILE_MANAGE,
        .method_list    = sg_file_manage_method_str,
        .method_num     = sizeof(sg_file_manage_method_str) / sizeof(sg_file_manage_method_str[0]),
        .message_handle = _file_manage_message_callback,
        .usr_data       = file_manage_context,
        .user_data_free = HAL_Free,
    };

    rc = service_mqtt_service_register(client, &params);
    if (rc) {
        HAL_Free(file_manage_context);
    }
    return rc;
}

/**
 * @brief File manage deinit, unregister handler from server list.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_FileManage_Deinit(void *client)
{
    POINTER_SANITY_CHECK_RTN(client);

    service_mqtt_service_unregister(client, SERVICE_TYPE_FILE_MANAGE);
    service_mqtt_deinit(client);
}

/**
 * @brief Report file manage message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] report_type @see IotFileManageReportType
 * @param[in] progress progress using in  IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING
 * @param[in] file_name_or_token token using in post event;file name using in other event
 * @param[in] version file version
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_Report(void *client, char *buf, int buf_len, IotFileManageReportType report_type, int progress,
                          const char *file_name_or_token, const char *version)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(buf_len, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(file_name_or_token, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(version, QCLOUD_ERR_INVAL);

    /**
     * @brief order @see IotOTAReportType
     *
     */
    const char *state_string[] = {
        "downloading", "burning", "done", "fail", "fail", "fail", "fail",
        "fail",        "fail",    "done", "fail", "done", "fail",
    };

    int result_code[] = {0, 0, 0, -1, -2, -3, -4, -5, -6, 0, -7, 0, -8};

    const char *result_msg[] = {
        "",
        "",
        "",
        "timeout",
        "file not exit",
        "auth fail",
        "md5 not match",
        "upgrade fail",
        "space not enough",
        "",
        "del fail",
        "",
        "post fail",
    };

    int len;
    switch (report_type) {
        case IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING:
            len = HAL_Snprintf(
                buf, buf_len,
                "{\"method\":\"report_progress\",\"report\":{\"progress\":{\"resource_name\":\"%s\",\"state\":\"%s\","
                "\"percent\":\"%d\",\"result_code\":\"%d\",\"result_msg\":\"\"},\"version\":\"%s\"}}",
                file_name_or_token, state_string[report_type], progress, result_code[report_type], version);
            break;
        case IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_BEGIN:
        case IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_SUCCESS:
        case IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOAD_TIMEOUT:
        case IOT_FILE_MANAGE_REPORT_TYPE_FILE_NOT_EXIST:
        case IOT_FILE_MANAGE_REPORT_TYPE_AUTH_FAIL:
        case IOT_FILE_MANAGE_REPORT_TYPE_MD5_NOT_MATCH:
        case IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_FAIL:
        case IOT_FILE_MANAGE_REPORT_TYPE_SPACE_NOT_ENOUGH:
            len = HAL_Snprintf(buf, buf_len,
                               "{\"method\":\"report_result\",\"report\":{\"progress\":{\"resource_name\":\"%s\","
                               "\"state\":\"%s\",\"result_code\":\"%d\",\"result_msg\":\"%s\"},\"version\":\"%s\"}}",
                               file_name_or_token, state_string[report_type], result_code[report_type],
                               result_msg[report_type], version);
            break;
        case IOT_FILE_MANAGE_REPORT_TYPE_DEL_SUCCESS:
        case IOT_FILE_MANAGE_REPORT_TYPE_DEL_FAIL:
            len = HAL_Snprintf(buf, buf_len,
                               "{\"method\":\"del_result\",\"report\":{\"progress\":{\"resource_name\":\"%s\","
                               "\"state\":\"%s\",\"result_code\":\"%d\", \"result_msg\":\"%s\"}, \"version\":\"%s\"}}",
                               file_name_or_token, state_string[report_type], result_code[report_type],
                               result_msg[report_type], version);
            break;
        case IOT_FILE_MANAGE_REPORT_TYPE_POST_SUCCESS:
        case IOT_FILE_MANAGE_REPORT_TYPE_POST_FAIL:
            len = HAL_Snprintf(buf, buf_len,
                               "{\"method\":\"report_post_result\",\"report\":{\"progress\":{\"resource_token\":\"%s\","
                               "\"state\":\"%s\",\"result_code\":\"%d\", \"result_msg\":\"%s\"}}}",
                               file_name_or_token, state_string[report_type], result_code[report_type],
                               result_msg[report_type]);
            break;
        default:
            return QCLOUD_ERR_INVAL;
    }
    return service_mqtt_publish(client, QOS0, buf, len);
}

/**
 * @brief Report file list to server mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] file_list file list of @see IotFileManageFileInfo; file name which is "\0" means invalid.
 * @param[in] max_num max num of file list.
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_ReportFileList(void *client, char *buf, int buf_len, const IotFileManageFileInfo file_list[],
                                  int max_num)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(buf_len, QCLOUD_ERR_INVAL);

    int len = HAL_Snprintf(buf, buf_len, "{\"method\":\"report_version\",\"report\":{\"resource_list\":[");
    for (int i = 0; i < max_num; i++) {
        if (file_list[i].file_type == IOT_FILE_MANAGE_FILE_TYPE_UNKOWN) {
            continue;
        }

        if (file_list[i].file_name[0] != '\0') {
            len += HAL_Snprintf(buf + len, buf_len - len,
                                "{\"resource_name\":\"%s\",\"version\":\"%s\",\"resource_type\":\"%s\"},",
                                file_list[i].file_name, file_list[i].file_version,
                                sg_file_manage_file_type_str[file_list[i].file_type]);
        }
    }
    if (buf[len - 1] != '[') {
        len--;  // remove the last ','
    }
    len += HAL_Snprintf(buf + len, buf_len - len, "]}}");
    return service_mqtt_publish(client, QOS0, buf, len);
}

/**
 * @brief Request url to upload.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] file_info file to upload, @see IotFileManageFileInfo
 * @param[in] request_id user defined, to keep unique.
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_PostRequest(void *client, char *buf, int buf_len, const IotFileManageFileInfo *file_info,
                               int request_id)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(buf_len, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(file_info, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(file_info->file_type, QCLOUD_ERR_INVAL);

    int len = HAL_Snprintf(buf, buf_len,
                           "{\"method\":\"request_url\",\"request_id\":\"%d\","
                           "\"report\":{\"resource_name\":\"%s\",\"version\":\"%s\",\"resource_type\":\"%s\"}}",
                           request_id, file_info->file_name, file_info->file_version,
                           sg_file_manage_file_type_str[file_info->file_type]);
    return service_mqtt_publish(client, QOS0, buf, len);
}

/**
 * @brief Get file type according to string. @see sg_file_manage_file_type_str.
 *
 * @param[in] file_type file type string
 * @param[in] len string length
 * @return @see IotFileManageFileType
 */
IotFileManageFileType IOT_FileManage_GetFileType(const char *file_type, int len)
{
    for (int i = 0; i < sizeof(sg_file_manage_file_type_str) / sizeof(sg_file_manage_file_type_str[0]); i++) {
        if (len != strlen(sg_file_manage_file_type_str[i])) {
            continue;
        }

        if (!strncmp(file_type, sg_file_manage_file_type_str[i], len)) {
            return i;
        }
    }

    return IOT_FILE_MANAGE_FILE_TYPE_UNKOWN;
}
