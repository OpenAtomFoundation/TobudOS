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
 * @file data_template_sample.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-09-27
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-09-27 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_common.h"
#include "qcloud_iot_explorer.h"
#include "data_template_config.h"

#include "utils_log.h"
#include "utils_md5.h"

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
static void _handle_property_callback(void *client, int is_get_status)
{
    for (UsrPropertyIndex i = USR_PROPERTY_INDEX_POWER_SWITCH; i <= USR_PROPERTY_INDEX_POWER; i++) {
        if (usr_data_template_property_status_get(i)) {
            DataTemplatePropertyValue value;
            switch (i) {
                case USR_PROPERTY_INDEX_POWER_SWITCH:
                case USR_PROPERTY_INDEX_COLOR:
                case USR_PROPERTY_INDEX_BRIGHTNESS:  // read only, just for example
                case USR_PROPERTY_INDEX_POWER:       // read only, just for example
                    value = usr_data_template_property_value_get(i);
                    Log_d("recv %s:%d", usr_data_template_property_key_get(i), value.value_int);
                    break;
                case USR_PROPERTY_INDEX_NAME:  // read only, just for example
                    value = usr_data_template_property_value_get(i);
                    Log_d("recv %s:%s", usr_data_template_property_key_get(i), value.value_string);
                    break;
                case USR_PROPERTY_INDEX_POSITION:  // read only, just for example
                    for (UsrPropertyPositionIndex j = USR_PROPERTY_POSITION_INDEX_LONGITUDE;
                         j <= USR_PROPERTY_POSITION_INDEX_LATITUDE; j++) {
                        value = usr_data_template_property_struct_value_get(i, j);
                        Log_d("recv %s:%d", usr_data_template_property_struct_key_get(i, j), value.value_int);
                    }
                    break;
            }
            usr_data_template_property_status_reset(i);
        }
    }
}

static void _method_control_callback(UtilsJsonValue client_token, UtilsJsonValue params, void *usr_data)
{
    char buf[256];
    Log_i("recv msg[%.*s]: params=%.*s", client_token.value_len, client_token.value, params.value_len, params.value);
    IOT_DataTemplate_PropertyControlReply(usr_data, buf, sizeof(buf), 0, client_token);
    usr_data_template_property_parse(params);
    _handle_property_callback(usr_data, 0);
}

static void _method_get_status_reply_callback(UtilsJsonValue client_token, int code, UtilsJsonValue reported,
                                              UtilsJsonValue control, void *usr_data)
{
    char buf[256];
    Log_i("recv msg[%.*s]: code=%d|reported=%.*s|control=%.*s", client_token.value_len, client_token.value, code,
          reported.value_len, STRING_PTR_PRINT_SANITY_CHECK(reported.value), control.value_len,
          STRING_PTR_PRINT_SANITY_CHECK(control.value));
    usr_data_template_property_parse(control);
    IOT_DataTemplate_PropertyClearControl(usr_data, buf, sizeof(buf));
    _handle_property_callback(usr_data, 1);
}

static void _method_action_callback(UtilsJsonValue client_token, UtilsJsonValue action_id, UtilsJsonValue params,
                                    void *usr_data)
{
    UsrActionIndex            index;
    int                       rc;
    DataTemplatePropertyValue value_time, value_color, value_total_time;

    char buf[256];

    Log_i("recv msg[%.*s]: action_id=%.*s|params=%.*s", client_token.value_len, client_token.value, action_id.value_len,
          action_id.value, params.value_len, params.value);

    rc = usr_data_template_action_parse(action_id, params, &index);
    if (rc) {
        return;
    }

    switch (index) {
        case USR_ACTION_INDEX_LIGHT_BLINK:
            value_time       = usr_data_template_action_input_value_get(USR_ACTION_INDEX_LIGHT_BLINK,
                                                                        USR_ACTION_LIGHT_BLINK_INPUT_INDEX_TIME);
            value_color      = usr_data_template_action_input_value_get(USR_ACTION_INDEX_LIGHT_BLINK,
                                                                        USR_ACTION_LIGHT_BLINK_INPUT_INDEX_COLOR);
            value_total_time = usr_data_template_action_input_value_get(USR_ACTION_INDEX_LIGHT_BLINK,
                                                                        USR_ACTION_LIGHT_BLINK_INPUT_INDEX_TOTAL_TIME);
            Log_i("light[%d] blink %d every %d s ", value_color.value_enum, value_time.value_int,
                  value_total_time.value_int);
            usr_data_template_action_reply(usr_data, buf, sizeof(buf), index, client_token, 0, "{\"err_code\":0}");
            break;
        default:
            break;
    }
}

// ----------------------------------------------------------------------------
// Data template upstream
// ----------------------------------------------------------------------------

/**
 * @brief Report status.
 *
 * @param[in,out] client pointer to mqtt client
 */
static void _cycle_report(void *client)
{
    char                  buf[256];
    static QcloudIotTimer sg_cycle_report_timer;
    if (IOT_Timer_Expired(&sg_cycle_report_timer)) {
        usr_data_template_event_post(client, buf, sizeof(buf), USR_EVENT_INDEX_STATUS_REPORT,
                                     "{\"status\":0,\"message\":\"ok\"}");
        IOT_Timer_Countdown(&sg_cycle_report_timer, 60);
    }
}

/**
 * @brief Init usr data template and data.
 *
 */
static void _usr_init(void)
{
    usr_data_template_init();

    DataTemplatePropertyValue value;
    value.value_int = 0;
    usr_data_template_property_value_set(USR_PROPERTY_INDEX_POWER_SWITCH, value);
    value.value_enum = 0;
    usr_data_template_property_value_set(USR_PROPERTY_INDEX_COLOR, value);
    value.value_int = 10;
    usr_data_template_property_value_set(USR_PROPERTY_INDEX_BRIGHTNESS, value);
    value.value_string = "light";
    usr_data_template_property_value_set(USR_PROPERTY_INDEX_NAME, value);
    value.value_int = 30;
    usr_data_template_property_struct_value_set(USR_PROPERTY_INDEX_POSITION, USR_PROPERTY_POSITION_INDEX_LONGITUDE,
                                                value);
    value.value_int = 30;
    usr_data_template_property_struct_value_set(USR_PROPERTY_INDEX_POSITION, USR_PROPERTY_POSITION_INDEX_LATITUDE,
                                                value);
    value.value_string = "high";
    usr_data_template_property_value_set(USR_PROPERTY_INDEX_POWER, value);
}

// ----------------------------------------------------------------------------
// OTA
// ----------------------------------------------------------------------------

#define SIGNAL_DOWNLOAD_OTA_SIZE (1024)

static int _qcloud_iot_check_ota(void)
{
    char    *remote_ver = NULL, *local_ver = "1.0.0";
    uint32_t fw_size;
    char    *md5 = NULL;
    int      rc  = 0;

    rc = IOT_OTA_Init(local_ver);
    if (rc) {
        Log_e("OTA Init Failed: %d", rc);
        return rc;
    }

    // 1. get remote fw info
    rc = IOT_OTA_ReadFwInfo(&remote_ver, &fw_size, &md5, 5000);
    if (rc) {
        Log_e("read ota fw info fail:%d", rc);
        return rc;
    }
    Log_w("fw version : %s, size : %u, md5:%s", remote_ver, fw_size, md5);

    // 2. check if need ota
    if (!strcmp(local_ver, remote_ver)) {
        Log_w("local version(%s) == remote version(%s)", local_ver, remote_ver);
        return QCLOUD_RET_SUCCESS;
    }
    // 3. do ota
    char         *fw_data = HAL_Malloc(SIGNAL_DOWNLOAD_OTA_SIZE);
    uint32_t      read_len = SIGNAL_DOWNLOAD_OTA_SIZE;
    IotMd5Context md5ctx;
    uint32_t      download_size = 0;

    utils_md5_reset(&md5ctx);
    do {
        rc = IOT_OTA_ReadFWData(fw_data, &read_len, 5000);
        if (rc) {
            break;
        }
        download_size += read_len;
        Log_i("read len : %d(%d%%)", read_len, download_size*100 / fw_size);
        if (read_len > 0) {
            utils_md5_update(&md5ctx, fw_data, read_len);
            // todo: save to flash
        }

    } while (read_len == SIGNAL_DOWNLOAD_OTA_SIZE);
    HAL_Free(fw_data);

    if (download_size != fw_size) {
        Log_e("download fail downloaded(%u) != fw size(%u)", download_size, fw_size);
        return QCLOUD_ERR_FAILURE;
    }
    // 4. check md5
    utils_md5_finish(&md5ctx);
    if (utils_md5_compare(&md5ctx, md5)) {
        Log_e("md5 check fail cal(%s) != fw(%s)", md5ctx.md5sum, md5);
        return QCLOUD_ERR_FAILURE;
    }
    // todo: reboot or not
    Log_d("ota success.");
    return QCLOUD_RET_SUCCESS;
}

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
static int sg_main_exit = 0;

int data_template_app(void)
{
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

    _usr_init();

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

    rc = _qcloud_iot_check_ota();
    if (rc) {
        Log_e("OTA Check failed!");
        return rc;
    }

    // subscribe normal topics and wait result
    IotDataTemplateCallback callback                            = DEFAULT_DATA_TEMPLATE_CALLBACK;
    callback.property_callback.method_control_callback          = _method_control_callback;
    callback.property_callback.method_get_status_reply_callback = _method_get_status_reply_callback;
    callback.action_callback.method_action_callback             = _method_action_callback;

    rc = IOT_DataTemplate_Init(client, callback, client);
    if (rc) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        return rc;
    }

    IOT_DataTemplate_PropertyGetStatus(client, buf, sizeof(buf));

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
        _cycle_report(client);
        usr_data_template_property_report(client, buf, sizeof(buf));
    } while (!sg_main_exit);

exit:
    IOT_DataTemplate_Deinit(client);
    rc |= IOT_MQTT_Destroy(&client);
    utils_log_deinit();
    return rc;
}

void application_entry(void *arg)
{   
    data_template_app();
}
