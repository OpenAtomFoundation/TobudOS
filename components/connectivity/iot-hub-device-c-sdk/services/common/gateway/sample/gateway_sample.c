/**
 * @file gateway_sample.c
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-05-25
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
 * 2022-05-25		1.0			hubertxxu		first commit
 * </table>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_common.h"
#include "qcloud_iot_explorer.h"

#include "utils_log.h"

#include "gateway_subdev_handle.h"

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
// Data template callback
// ----------------------------------------------------------------------------
#if 0
static void _method_control_callback(UtilsJsonValue client_token, UtilsJsonValue params, void *usr_data)
{
    char buf[256];
    Log_i("recv msg[%.*s]: params=%.*s", client_token.value_len, client_token.value, params.value_len, params.value);
    IOT_DataTemplate_PropertyControlReply(usr_data, buf, sizeof(buf), 0, client_token);
}

static void _method_report_reply_callback(UtilsJsonValue client_token, int code, void *usr_data)
{
    Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_get_status_reply_callback(UtilsJsonValue client_token, int code, UtilsJsonValue reported,
                                              UtilsJsonValue control, void *usr_data)
{
    char buf[256];
    Log_i("recv msg[%.*s]: code=%d|reported=%.*s|control=%.*s", client_token.value_len, client_token.value, code,
          reported.value_len, STRING_PTR_PRINT_SANITY_CHECK(reported.value), control.value_len,
          STRING_PTR_PRINT_SANITY_CHECK(control.value));
    IOT_DataTemplate_PropertyClearControl(usr_data, buf, sizeof(buf));
}

static void _method_report_info_reply_callback(UtilsJsonValue client_token, int code, void *usr_data)
{
    Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_clear_control_reply_callback(UtilsJsonValue client_token, int code, void *usr_data)
{
    Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_event_reply_callback(UtilsJsonValue client_token, int code, void *usr_data)
{
    Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_action_callback(UtilsJsonValue client_token, UtilsJsonValue action_id, UtilsJsonValue params,
                                    void *usr_data)
{
    char buf[256];

    Log_i("recv msg[%.*s]: action_id=%.*s|params=%.*s", client_token.value_len, client_token.value, action_id.value_len,
          action_id.value, params.value_len, params.value);
    IotDataTemplateActionReply reply = {
        .code         = 0,
        .client_token = client_token,
        .response     = "{\"err_code\":0}",
    };
    IOT_DataTemplate_ActionReply(usr_data, buf, sizeof(buf), reply);
}

// ----------------------------------------------------------------------------
// Data template upstream
// ----------------------------------------------------------------------------

static void _cycle_report(void *client)
{
    char        buf[256];
    const char *report_property = "{\"power_switch\":0}";

    IotDataTemplateEventData event_data = {
        .event_id = "status_report",
        .type     = IOT_DATA_TEMPLATE_EVENT_TYPE_INFO,
        .params   = "{\"status\":0,\"message\":\"ok\"}",

    };

    static QcloudIotTimer sg_cycle_report_timer;
    if (IOT_Timer_Expired(&sg_cycle_report_timer)) {
        IOT_DataTemplate_PropertyReport(client, buf, sizeof(buf), report_property);
        IOT_DataTemplate_EventPost(client, buf, sizeof(buf), event_data);
        IOT_Timer_Countdown(&sg_cycle_report_timer, 500);
    }
}
#endif
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
#if 0
    // subscribe normal topics and wait result
    IotDataTemplateCallback callback = {
        .property_callback = {.method_control_callback             = _method_control_callback,
                              .method_clear_control_reply_callback = _method_clear_control_reply_callback,
                              .method_get_status_reply_callback    = _method_get_status_reply_callback,
                              .method_report_info_reply_callback   = _method_report_info_reply_callback,
                              .method_report_reply_callback        = _method_report_reply_callback},
        .event_callback    = {.method_event_reply_callback = _method_event_reply_callback},
        .action_callback   = {.method_action_callback = _method_action_callback},
    };

    rc = IOT_DataTemplate_Init(client, callback, client);
    if (rc) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        return rc;
    }

    const char *report_info =
        "{\"module_hardinfo\":\"ESP8266\",\"module_softinfo\":\"V1.0\", \"fw_ver\":\"4.0.0\", "
        "\"imei\":\"11-22-33-44\",\"lat\":\"22.546015\",\"lon\":\"113.941125\",\"mac\":\"11:22:33:44:55:66\",\"device_"
        "label\":{\"append_"
        "info\":\"your self defined info\"}}";

    IOT_DataTemplate_PropertyReportInfo(client, buf, sizeof(buf), report_info);
    IOT_DataTemplate_PropertyGetStatus(client, buf, sizeof(buf));
#endif
    // gateway init

    iot_gateway_init(client, 32);

    do {
        rc = IOT_MQTT_Yield(client, QCLOUD_IOT_MQTT_YIELD_TIMEOUT);
        switch (rc) {
            case QCLOUD_RET_SUCCESS:
                break;
            case QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT:
                continue;
            case QCLOUD_RET_MQTT_RECONNECTED:
                IOT_DataTemplate_PropertyGetStatus(client, buf, sizeof(buf));
                break;
            default:
                Log_e("Exit loop caused of errCode:%d", rc);
                goto exit;
        }
        // _cycle_report(client);
    } while (!sg_main_exit);

exit:
    iot_gateway_deinit(client);
    // IOT_DataTemplate_Deinit(client);
    rc |= IOT_MQTT_Destroy(&client);
    utils_log_deinit();
    return rc;
}
