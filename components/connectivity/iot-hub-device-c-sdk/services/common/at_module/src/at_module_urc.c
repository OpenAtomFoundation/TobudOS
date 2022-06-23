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
 * @file at_module_urc.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-04-24 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "at_module.h"

/**
 * @brief URC for iot hub
 * 1. urc for at command
 *      +TCMQTTRCVPUB
 *      +TCMQTTDISCON
 *      +TCMQTTRECONNECTING
 *      +TCMQTTRECONNECTED
 *      +TCOTASTATUS
 * 2. urc cmd response
 *      +TCMQTTSTATE
 * 3. urc for ota
 *      +TCREADFWDATA
 *      +TCFWINFO
 *      +TCOTASTATUS
 *
 */

// ----------------------------------------------------------------------------
// mqtt urc handle
// ----------------------------------------------------------------------------

/**
 * @brief Discconect by mqtt server.
 *
 * @param[in] data data recv from at module
 * @param[in] data_len data len
 */
static void _urc_mqtt_disconnect_handle(const char *data, size_t data_len)
{
    // +TCMQTTSTATE:-3
    int error_code = 0;

    QcloudIotClient *mqtt_client = qcloud_iot_get_mqtt_client();
    if (!mqtt_client) {
        return;
    }
    mqtt_client->connect_state = MQTT_CONNECT_STATE_DISCONNECT;

    sscanf(data, "+TCMQTTDISCON,%d", &error_code);
    Log_e("disconnect %d", error_code);
}

/**
 * @brief Reconnecting.
 *
 * @param[in] data data recv from at module
 * @param[in] data_len data len
 */
static void _urc_mqtt_reconnect_handle(const char *data, size_t data_len)
{
    // +TCMQTTRECONNECTING
    QcloudIotClient *mqtt_client = qcloud_iot_get_mqtt_client();
    if (!mqtt_client) {
        return;
    }
    mqtt_client->connect_state = MQTT_CONNECT_STATE_RECONNECTING;
}

/**
 * @brief Reconnected.
 *
 * @param[in] data data recv from at module
 * @param[in] data_len data len
 */
static void _urc_mqtt_reconnected_handle(const char *data, size_t data_len)
{
    // +TCMQTTRECONNECTED
    QcloudIotClient *mqtt_client = qcloud_iot_get_mqtt_client();
    if (!mqtt_client) {
        return;
    }
    mqtt_client->connect_state = MQTT_CONNECT_STATE_RECONNECTED;
}

/**
 * @brief Connect state.
 *
 * @param[in] data data recv from at module
 * @param[in] data_len data len
 */
static void _urc_mqtt_state_handle(const char *data, size_t data_len)
{
    // +TCMQTTSTATE:1
    int connect_state = 1;

    QcloudIotClient *mqtt_client = qcloud_iot_get_mqtt_client();
    if (!mqtt_client) {
        return;
    }
    sscanf(data, "+TCMQTTSTATE:%d", &connect_state);
    Log_d("mqtt state %d", connect_state);
    if (connect_state) {
        mqtt_client->connect_state = MQTT_CONNECT_STATE_CONNECTED;
    }
}

/**
 * @brief Recv publish packet.
 *
 * @param[in] data data recv from at module
 * @param[in] data_len data len
 */
static void _urc_mqtt_publish_handle(const char *data, size_t data_len)
{
    // +TCMQTTRCVPUB:"$thing/down/property/Q5NNWVC2Z8/test1",81,"{"method":"report_reply","clientToken":"property-69","code":0,"status":"success"}"

    QcloudIotClient *mqtt_client = qcloud_iot_get_mqtt_client();
    if (!mqtt_client) {
        return;
    }

    char *head, *topic_name, *payload;
    char *src = (char *)data;

    src[data_len - 1] = '\0';

    head = strtok((char *)data, "\"");
    if (!head) {
        return;
    }

    topic_name = strtok(NULL, "\"");
    if (!topic_name) {
        return;
    }

    strtok(NULL, "\"");
    payload = strtok(NULL, "");
    if (!payload) {
        return;
    }
    qcloud_iot_deliver_message(mqtt_client, topic_name, payload, strlen(payload));
}

// ----------------------------------------------------------------------------
// ota urc handle
// ----------------------------------------------------------------------------

static void _cancel_char(char *str, char ch)
{
    char *p;
    for (p = str; *p != '\0'; p++)
        if (*p != ch)
            *str++ = *p;
    *str = '\0';
}

/**
 * @brief Recv fw info.
 *
 * @param[in] data data recv from at module
 * @param[in] data_len data len
 */
static void _urc_ota_fw_info_handle(const char *data, size_t data_len)
{
    // +TCFWINFO:"1.2.0",17300,"a2aa3c261ebfc1322edafd37edb6b183",262144
    QcloudIotOtaFwInfo *ota_info = qcloud_iot_get_ota_info_handle();
    _cancel_char((char *)data, '"');
    sscanf((char *)data, "+TCFWINFO:%[^,],%u,%[^,],%u", ota_info->version, &(ota_info->fw_size), ota_info->md5,
           &ota_info->fw_max_size);
    HAL_SemaphorePost(ota_info->get_fw_info_sem);
}

static void _urc_ota_state_handle(const char *data, size_t data_len)
{
    // Todo
}

// ----------------------------------------------------------------------------
// API
// ----------------------------------------------------------------------------

/**
 * @brief Set mqtt urc to at module.
 *
 * @return 0 means successful.
 */
int module_set_mqtt_urc(void)
{
    int rc;
    rc = HAL_Module_SetUrc("+TCMQTTRCVPUB", _urc_mqtt_publish_handle);
    rc |= HAL_Module_SetUrc("+TCMQTTDISCON", _urc_mqtt_disconnect_handle);
    rc |= HAL_Module_SetUrc("+TCMQTTRECONNECTING", _urc_mqtt_reconnect_handle);
    rc |= HAL_Module_SetUrc("+TCMQTTRECONNECTED", _urc_mqtt_reconnected_handle);
    rc |= HAL_Module_SetUrc("+TCMQTTSTATE", _urc_mqtt_state_handle);
    return rc;
}

/**
 * @brief Set ota urc to at module.
 *
 * @return 0 means successful.
 */
int module_set_ota_urc(void)
{
    int rc;
    rc = HAL_Module_SetUrc("+TCFWINFO", _urc_ota_fw_info_handle);
    rc |= HAL_Module_SetUrc("+TCOTASTATUS", _urc_ota_state_handle);
    return rc;
}
