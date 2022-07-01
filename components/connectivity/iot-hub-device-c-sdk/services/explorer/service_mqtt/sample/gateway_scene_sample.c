/**
 * @file gateway_scene_sample.c
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-06-16
 *
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
 * @par Change Log:
 * <table>
 * Date				Version		Author			Description
 * 2022-06-16		1.0			hubertxxu		first commit
 * </table>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_common.h"
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
    init_params->device_info       = device_info;
    init_params->event_handle.h_fp = _mqtt_event_handler;
}

// ----------------------------------------------------------------------------
// Gateway scene callbacks
// ----------------------------------------------------------------------------

static int _gateway_scene_handles_callback(UtilsJsonValue scene_id, UtilsJsonValue scene_name,
                                    UtilsJsonValue scene_update_time, UtilsJsonValue scene_handles, void *usr_data)
{
    Log_d("recv scene handles :  id:%.*s| name:%.*s| update time:%.*s| handles:%.*s", scene_id.value_len,
          scene_id.value, scene_name.value_len, scene_name.value, scene_update_time.value_len, scene_update_time.value,
          scene_handles.value_len, scene_handles.value);
    return 0;
}

static int _gateway_run_scene_callback(UtilsJsonValue scene_id, void *usr_data)
{
    Log_d("run scene: id:%.*s", scene_id.value_len, scene_id.value);
    return 0;
}

static int _gateway_delete_scene_callback(UtilsJsonValue scene_id, void *usr_data)
{
    Log_d("delete scene: id:%.*s", scene_id.value_len, scene_id.value);
    return 0;
}

static int _gateway_reload_scene_reply_callback(int result_code, UtilsJsonValue status, UtilsJsonValue scene_result,
                                         void *usr_data)
{
    Log_d("reload scene reply: result code:%d|status:%.*s|result:%.*s", result_code, status.value_len, status.value,
          scene_result.value_len, scene_result.value);
    return 0;
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

    int  rc;
    char buf[4096];
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
    IoTGatewaySceneCallback cbs = {
        .gateway_delete_scene_callback       = _gateway_delete_scene_callback,
        .gateway_reload_scene_reply_callback = _gateway_reload_scene_reply_callback,
        .gateway_run_scene_callback          = _gateway_run_scene_callback,
        .gateway_scene_handles_callback      = _gateway_scene_handles_callback,
    };
    rc = IOT_GatewayScene_Init(client, cbs, NULL);

    rc = IOT_GatewayScene_Reload(client, buf, sizeof(buf));

    IoTGatewaySceneInnerList scene_inner_list[MAX_LENGTH_INNER_SCENE_LIST];
    for (int i = 0; i < MAX_LENGTH_INNER_SCENE_LIST; i++) {
        HAL_Snprintf(scene_inner_list[i].inner_scene_id, MAX_LENGTH_INNER_SCENE_ID, "inner_scene_id_%d", i);
        HAL_Snprintf(scene_inner_list[i].inner_scene_name, MAX_LENGTH_INNER_SCENE_NAME, "inner_scene_name_%d", i);
    }
    rc = IOT_GatewayScene_ReportInnerList(client, buf, sizeof(buf), scene_inner_list, MAX_LENGTH_INNER_SCENE_LIST);

    do {
        rc = IOT_MQTT_Yield(client, 2000);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            HAL_SleepMs(1000);
            continue;
        } else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("exit with error: %d", rc);
            break;
        }
    } while (!sg_main_exit);

    IOT_GatewayScene_Deinit(client);

    rc |= IOT_MQTT_Destroy(&client);

    utils_log_deinit();
    return rc;
}
