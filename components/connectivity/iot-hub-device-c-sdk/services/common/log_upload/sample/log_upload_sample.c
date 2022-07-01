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

static void _setup_log_upload_init_params(LogUploadInitParams *init_params, DeviceInfo *device_info)
{
    init_params->device_name = device_info->device_name;
    init_params->product_id  = device_info->product_id;
    init_params->sign_key    = device_info->device_secret;

    init_params->log_buffer_size   = LOG_UPLOAD_BUFFER_SIZE;
    init_params->save_log_filename = "./tmp/upload-fail-save.log";
    init_params->read_func         = HAL_File_Read;
    init_params->save_func         = HAL_File_Write;
    init_params->del_func          = HAL_File_Del;
    init_params->get_size_func     = HAL_File_GetSize;
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
    int loop_cnt = 5;
    // init log level
    LogHandleFunc func = DEFAULT_LOG_HANDLE_FUNCS;
    func.log_upload    = IOT_Log_Upload_AppendToUploadBuffer;

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
    // init log upload
    LogUploadInitParams log_upload_init_params = DEFAULT_LOG_UPLOAD_INIT_PARAMS;

    _setup_log_upload_init_params(&log_upload_init_params, &device_info);

    IOT_Log_Upload_InitPre(&log_upload_init_params);
    // create MQTT client and connect with server
    void *client = IOT_MQTT_Construct(&init_params);
    if (client) {
        Log_i("Cloud Device Construct Success");
        rc = IOT_Log_Upload_Init(client);
        if (rc) {
            Log_e("Log upload init failed: %d", rc);
        }
        IOT_Log_Upload(true);
    } else {
        Log_e("MQTT Construct failed!");
        return QCLOUD_ERR_FAILURE;
    }

    do {
        Log_d("log upload test debug %d...", loop_cnt);
        Log_i("log upload test info %d...", loop_cnt);
        Log_w("log upload test waring %d...", loop_cnt);
        Log_e("log upload test error %d...", loop_cnt);
        rc = IOT_MQTT_Yield(client, 200);
        IOT_Log_Upload(rc ? true : false);
    } while (loop_cnt--);
    // If you have to report some urgent messages, you can call this function
    IOT_Log_Upload(true);
    rc = IOT_MQTT_Destroy(&client);
    IOT_Log_Upload_Deinit();
    utils_log_deinit();
    return rc;
}
