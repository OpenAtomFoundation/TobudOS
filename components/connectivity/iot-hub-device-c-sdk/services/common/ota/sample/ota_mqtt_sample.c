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

// ----------------------------------------------------------------------------
// OTA function
// ----------------------------------------------------------------------------

int _firmware_download(void *client, char *buf, int buf_len, UtilsJsonValue url, UtilsJsonValue md5sum,
                       uint32_t file_size, const char *version)
{
    Log_i("downloading firmware:url=%.*s|md5sum=%.*s|file_size=%u", url.value_len, url.value, md5sum.value_len,
          md5sum.value, file_size);
    IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOADING, 0, version);
    IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOADING, 50, version);
    IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOADING, 100, version);
    return 0;
}

void _firmware_burn(void *client, char *buf, int buf_len, const char *version)
{
    IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_BEGIN, 0, version);
    IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_SUCCESS, 0, version);
    IOT_OTA_ReportVersion(client, buf, buf_len, version);
}

// ----------------------------------------------------------------------------
// OTA callback
// ----------------------------------------------------------------------------

void _update_firmware_callback(UtilsJsonValue version, UtilsJsonValue url, UtilsJsonValue md5sum, uint32_t file_size,
                               void *usr_data)
{
    char firmware_version[64] = {0};
    char buf[256];
    int  buf_len = sizeof(buf);

    Log_i("recv firmware: version=%.*s|url=%.*s|md5sum=%.*s|file_size=%u", version.value_len, version.value,
          url.value_len, url.value, md5sum.value_len, md5sum.value, file_size);
    strncpy(firmware_version, version.value, version.value_len);

    int rc = _firmware_download(usr_data, buf, buf_len, url, md5sum, file_size, firmware_version);
    if (rc) {
        IOT_OTA_ReportProgress(usr_data, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_FAIL, 0, firmware_version);
        return;
    }
    _firmware_burn(usr_data, buf, buf_len, firmware_version);
}

void _report_version_reply_callback(int result_code, void *usr_data)
{
    Log_i("recv code=%d", result_code);
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
    LogHandleFunc func = DEFAULT_LOG_HANDLE_FUNCS;
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
        return QCLOUD_ERR_FAILURE;
    }

    IotOTAUpdateCallback ota_callback = {
        .update_firmware_callback      = _update_firmware_callback,
        .report_version_reply_callback = _report_version_reply_callback,
    };
    rc = IOT_OTA_Init(client, ota_callback, client);
    if (rc) {
        Log_e("OTA init failed!, rc=%d", rc);
        return rc;
    }

    rc = IOT_OTA_ReportVersion(client, buf, sizeof(buf), QCLOUD_IOT_DEVICE_SDK_VERSION);
    if (rc) {
        Log_e("OTA report version failed!, rc=%d", rc);
        return rc;
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
    IOT_OTA_Deinit(client);
    rc = IOT_MQTT_Destroy(&client);
    utils_log_deinit();
    return rc;
}
