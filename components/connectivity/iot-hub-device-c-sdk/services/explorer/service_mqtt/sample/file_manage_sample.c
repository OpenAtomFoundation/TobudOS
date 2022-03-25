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
 * @file broadcast_sample.c
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_explorer.h"

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
    init_params->device_info            = device_info;
    init_params->command_timeout        = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
    init_params->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;
    init_params->auto_connect_enable    = 1;
    init_params->event_handle.h_fp      = _mqtt_event_handler;
    init_params->event_handle.context   = NULL;
}

// ----------------------------------------------------------------------------
// file list function
// ----------------------------------------------------------------------------
static IotFileManageFileInfo sg_file_list[5];

static void _file_info_init(IotFileManageFileInfo *file_info, UtilsJsonValue file_name, UtilsJsonValue file_type,
                            UtilsJsonValue version)
{
    strncpy(file_info->file_name, file_name.value, file_name.value_len);
    strncpy(file_info->file_version, version.value, version.value_len);
    file_info->file_type = IOT_FileManage_GetFileType(file_type.value, file_type.value_len);
}

static void _add_file_info_to_list(IotFileManageFileInfo *file_info)
{
    int i_free = -1;
    for (int i = 0; i < sizeof(sg_file_list) / sizeof(sg_file_list[0]); i++) {
        if (!strcmp(sg_file_list[i].file_name, file_info->file_name)) {
            memcpy(sg_file_list + i, file_info, sizeof(IotFileManageFileInfo));
            return;
        }

        if (sg_file_list[i].file_name[0] == '\0' && i_free == -1) {
            i_free = i;
        }
    }
    if (i_free != -1) {
        memcpy(sg_file_list + i_free, file_info, sizeof(IotFileManageFileInfo));
    }
}

static void _del_file_info_to_list(IotFileManageFileInfo *file_info)
{
    for (int i = 0; i < sizeof(sg_file_list) / sizeof(sg_file_list[0]); i++) {
        if (!strcmp(sg_file_list[i].file_name, file_info->file_name)) {
            sg_file_list[i].file_name[0] = '\0';
        }
    }
}

// ----------------------------------------------------------------------------
// file function
// ----------------------------------------------------------------------------

static int _file_download(void *client, char *buf, int buf_len, UtilsJsonValue url, UtilsJsonValue md5sum,
                          uint32_t file_size, IotFileManageFileInfo *file_info)
{
    Log_i("downloading file:url=%.*s|md5sum=%.*s|file_size=%u", url.value_len, url.value, md5sum.value_len,
          md5sum.value, file_size);
    IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING, 0, file_info->file_name,
                          file_info->file_version);
    IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING, 50, file_info->file_name,
                          file_info->file_version);
    IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING, 100, file_info->file_name,
                          file_info->file_version);
    return 0;
}

static int _file_upgrade(void *client, char *buf, int buf_len, IotFileManageFileInfo *file_info)
{
    IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_BEGIN, 0, file_info->file_name,
                          file_info->file_version);
    IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_SUCCESS, 0, file_info->file_name,
                          file_info->file_version);
    _add_file_info_to_list(file_info);
    return 0;
}

static int _file_del(void *client, char *buf, int buf_len, IotFileManageFileInfo *file_info)
{
    _del_file_info_to_list(file_info);
    IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DEL_SUCCESS, 0, file_info->file_name,
                          file_info->file_version);
    return 0;
}

// ----------------------------------------------------------------------------
// OTA callback
// ----------------------------------------------------------------------------

static void _file_manage_update_file_callback(UtilsJsonValue file_name, UtilsJsonValue file_type,
                                              UtilsJsonValue version, UtilsJsonValue url, UtilsJsonValue md5sum,
                                              uint32_t file_size, void *usr_data)
{
    IotFileManageFileInfo file_info = {0};

    char buf[256];
    int  buf_len = sizeof(buf);

    Log_i("recv file: file_name=%.*s|type=%.*s|version=%.*s|url=%.*s|md5sum=%.*s|file_size=%u", file_name.value_len,
          file_name.value, file_type.value_len, file_type.value, version.value_len, version.value, url.value_len,
          url.value, md5sum.value_len, md5sum.value, file_size);

    _file_info_init(&file_info, file_name, file_type, version);

    int rc = _file_download(usr_data, buf, buf_len, url, md5sum, file_size, &file_info);
    if (rc) {
        IOT_FileManage_Report(usr_data, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_FAIL, 0, file_info.file_name,
                              file_info.file_version);
        return;
    }
    _file_upgrade(usr_data, buf, buf_len, &file_info);
    IOT_FileManage_ReportFileList(usr_data, buf, buf_len, sg_file_list, sizeof(sg_file_list) / sizeof(sg_file_list[0]));
}

static void _file_manage_del_file_callback(UtilsJsonValue file_name, UtilsJsonValue file_type, UtilsJsonValue version,
                                           void *usr_data)
{
    char buf[256];
    int  buf_len = sizeof(buf);

    IotFileManageFileInfo file_info = {0};
    _file_info_init(&file_info, file_name, file_type, version);
    _file_del(usr_data, buf, buf_len, &file_info);
    IOT_FileManage_ReportFileList(usr_data, buf, buf_len, sg_file_list, sizeof(sg_file_list) / sizeof(sg_file_list[0]));
}

static void _file_manage_report_file_version_reponse_callback(UtilsJsonValue file_list, int result_code, void *usr_data)
{
    Log_i("recv file_list=%.*s|result_code=%d", file_list.value_len, file_list.value, result_code);
}

static void _file_manage_request_file_url_response_callback(UtilsJsonValue url, UtilsJsonValue file_token,
                                                            int result_code, void *usr_data)
{
    Log_i("recv response: url=%.*s|file_token=%.*s|result_code=%d", url.value_len, url.value, file_token.value_len,
          file_token.value, result_code);
}

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
static int sg_main_exit = 0;

#ifdef __linux__

#include <signal.h>
#include <pthread.h>
#include <unistd.h>

static void _main_exit(int sig)
{
    Log_e("demo exit by signal:%d\n", sig);
    sg_main_exit = 1;
}
#endif

int main(int argc, char **argv)
{
#ifdef __linux__
    signal(SIGINT, _main_exit);
#endif

    int rc;

    char buf[1024];

    // init log level
    LogHandleFunc func = {0};

    func.log_malloc               = HAL_Malloc;
    func.log_free                 = HAL_Free;
    func.log_get_current_time_str = HAL_Timer_Current;
    func.log_printf               = HAL_Printf;
    utils_log_init(func, LOG_LEVEL_DEBUG, 2048);

    DeviceInfo device_info;
    rc = HAL_GetDevInfo((void *)&device_info);
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
        goto exit;
    }

    IotFileManageCallback file_manage_callback = {
        .update_file_callback                 = _file_manage_update_file_callback,
        .del_file_callback                    = _file_manage_del_file_callback,
        .report_file_version_reponse_callback = _file_manage_report_file_version_reponse_callback,
        .request_file_url_response_callback   = _file_manage_request_file_url_response_callback,
    };
    rc = IOT_FileManage_Init(client, file_manage_callback, client);
    if (rc) {
        Log_e("OTA init failed!, rc=%d", rc);
        goto exit;
    }

    rc = IOT_FileManage_ReportFileList(client, buf, sizeof(buf), sg_file_list,
                                       sizeof(sg_file_list) / sizeof(sg_file_list[0]));
    if (rc) {
        Log_e("OTA report version failed!, rc=%d", rc);
        goto exit;
    }

    do {
        rc = IOT_MQTT_Yield(client, QCLOUD_IOT_MQTT_YIELD_TIMEOUT);
        switch (rc) {
            case QCLOUD_RET_SUCCESS:
                break;
            case QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT:
                continue;
            case QCLOUD_RET_MQTT_RECONNECTED:
                break;
            default:
                Log_e("Exit loop caused of errCode:%d", rc);
                goto exit;
        }
    } while (!sg_main_exit);
exit:
    IOT_FileManage_Deinit(client);
    rc = IOT_MQTT_Destroy(&client);
    utils_log_deinit();
    return rc;
}
