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
 * @file at_mqtt_cmd.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-19
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-04-19 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "at_module.h"

/**
 * @brief AT Command for Iot Hub
 * 1. network_register:
 *      AT+TCREGNET=?
 *      AT+TCREGNET?
 *      AT+TCREGNET=<module_type>,<action>[,<ssid>,<pw>][,<apn>]
 * 2. device_info:
 *      AT+TCDEVINFOSET=?
 *      AT+TCDEVINFOSET?
 *      AT+TCDEVINFOSET=<tlsmode>,<productId>,<devicename>[,<devicesecret>][,<certname>]
 * 3. cert:
 *      AT+TCCERTADD=?
 *      AT+TCCERTADD?
 *      AT+TCCERTADD=<cert_name>,<cert_size>
 *      AT+TCCERTCHECK=?
 *      AT+TCCERTCHECK?
 *      AT+TCCERTCHECK=<cert_name>
 *      AT+TCCERTDEL=?
 *      AT+TCCERTDEL?
 *      AT+TCCERTDEL=<cert_name>
 * 4. dynamic register:
 *      AT+TCPRDINFOSET=?
 *      AT+TCPRDINFOSET?
 *      AT+TCPRDINFOSET=<tls_mode>,<product_ID>,<product_secret>,<device_name>
 *      AT+TCDEVREG=?
 *      AT+TCDEVREG
 * 5. module info
 *      AT+TCMODULE
 *      AT+TCRESTORE=?
 * 6. mqtt
 *      AT+TCMQTTCONN=?
 *      AT+TCMQTTCONN?
 *      AT+TCMQTTCONN=<tlsmode>,<cmdtimeout>,<keepalive>,<clean_session>,<reconnect>
 *      AT+TCMQTTDISCONN=?
 *      AT+TCMQTTDISCONN
 *      AT+TCMQTTPUB=?
 *      AT+TCMQTTPUB=<topic>,<qos>,<message>
 *      AT+TCMQTTPUBL=?
 *      AT+TCMQTTPUBL= <topic>,<qos>,<msg_length>
 *      AT+TCMQTTSUB=?
 *      AT+TCMQTTSUB?
 *      AT+TCMQTTSUB=<topic>,<qos>
 *      AT+TCMQTTUNSUB=?
 *      AT+TCMQTTUNSUB?
 *      AT+TCMQTTUNSUB=<topic>
 *      AT+TCMQTTSTATE=?
 *      AT+TCMQTTSTATE?
 * 7. ota
 *      AT+TCOTASET=?
 *      AT+TCOTASET?
 *      AT+TCOTASET=<ctlstate>,<fw_ver>
 *      AT+TCFWINFO=?
 *      AT+TCFWINFO?
 *      AT+TCREADFWDATA=?
 *      AT+TCREADFWDATA=<len>
 *
 */

/**
 * @brief AT module cmd.
 *
 */
typedef enum {
    MODULE_CMD_SET_DEVICE_INFO,     // AT+TCDEVINFOSET=<tlsmode>,<productId>,<devicename>[,<devicesecret>][,<certname>]
    MODULE_CMD_CONNECT,             // AT+TCMQTTCONN=<tlsmode>,<cmdtimeout>,<keepalive>,<clean_session>,<reconnect>
    MODULE_CMD_DISCONNECT,          // AT+TCMQTTDISCONN
    MODULE_CMD_PUBLISH,             // AT+TCMQTTPUB=<topic>,<qos>,<message>
    MODULE_CMD_PUBLISHL,            // AT+TCMQTTPUBL=<topic>,<qos>,<msg_length>
    MODULE_CMD_SUBSCRIBE,           // AT+TCMQTTSUB=<topic>,<qos>
    MODULE_CMD_UNSUBSCRIBE,         // AT+TCMQTTUNSUB=<topic>
    MODULE_CMD_GET_CONNECT_STATUS,  // AT+TCMQTTSTATE?
    MODULE_CMD_SET_OTA_VERSION,     // AT+TCOTASET=<ctlstate>,<fw_ver>
    MODULE_CMD_GET_OTA_FW_INFO,     // AT+TCFWINFO?
    MODULE_CMD_READ_OTA_FW_DATA     // AT+TCREADFWDATA=<len>
} ModuleCmd;

/**
 * @brief Publish cmd params.
 *
 */
typedef struct {
    const char          *topic_name;
    const PublishParams *params;
} ModuleCmdPublishParams;

/**
 * @brief Subscribe cmd params.
 *
 */
typedef struct {
    const char *topic_filter;
    int         qos;
} ModuleCmdSubscribeParams;

/**
 * @brief Unsubscribe cmd params.
 *
 */
typedef struct {
    const char *topic_filter;
} ModuleCmdUnsubscribeParams;

/**
 * @brief Set ota version to report.
 *
 */
typedef struct {
    /**
     * @brief Control module report
     * 0: off
     * 1: only report mcu version
     * 2: only report module version
     * 3: report all
     */
    QcloudIotOtaClsState cls_state;
    const char          *version; /* ota version */
} ModuleCmdSetOTAVersionParams;

/**
 * @brief Read fw data.
 *
 */
typedef struct {
    void     *fw_data_buf;
    uint32_t *fw_data_len;
    uint32_t  timeout;
} ModuleCmdReadOTFwDataParams;

/**
 * @brief Module cmd params.
 *
 */
typedef struct {
    const QcloudIotClient *client;
    union {
        ModuleCmdPublishParams       publish_params;
        ModuleCmdSubscribeParams     subscribe_params;
        ModuleCmdUnsubscribeParams   unsubscribe_params;
        ModuleCmdSetOTAVersionParams set_ota_version_params;
        ModuleCmdReadOTFwDataParams  read_ota_fw_data_params;
    };
} ModuleCmdParams;

/**
 * @brief Transfer '\"' and ','.
 *
 * @param[in,out] payload payload to transfer, reuse memory
 * @return length of payload
 */
static int _transfer_payload(char *payload, char **payload_transferred)
{
    char *tmp, *src = payload;
    int   count = 0;
    char  c     = '\0';

    // 1. calculate '\"' and '.'
    while ((c = *src++) != '\0') {
        if (c == '\"' || c == ',') {
            count++;
        }
    }

    // 2. malloc for payload
    int tmp_len = src - payload + count;
    tmp         = HAL_Malloc(tmp_len);
    if (!tmp) {
        return 0;
    }
    memset(tmp, 0, tmp_len);

    // 3. replace
    for (src = payload, count = 0; (c = *src) != '\0'; src++) {
        if (c == '\"' || c == ',') {
            tmp[count++] = '\\';
        }
        tmp[count++] = c;
    }

    tmp[count]           = '\0';
    *payload_transferred = tmp;
    return count;
}

/**
 * @brief Send at cmd to at module.
 *
 * @param[in] cmd @see ModuleCmd
 * @param[in] params @see ModuleCmdParams
 * @return 0 for success
 */
static int _module_send_cmd(ModuleCmd cmd, ModuleCmdParams *params)
{
#define DEFAULT_TIMEOUT_MS 5000
    int   rc, len = 0;
    char  at_cmd[1250]        = {0};
    char  at_resp[32]         = {0};
    char *payload_transferred = NULL;

    const QcloudIotClient *client = params->client;

    switch (cmd) {
        case MODULE_CMD_SET_DEVICE_INFO:
            strncpy(at_resp, "+TCDEVINFOSET:OK", sizeof(at_resp));
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCDEVINFOSET=%d,\"%s\",\"%s\",\"%s\"\r\n", client->tls_mode,
                               client->device_info->product_id, client->device_info->device_name,
                               client->device_info->device_secret);
            break;
        case MODULE_CMD_CONNECT:
            strncpy(at_resp, "+TCMQTTCONN:OK", sizeof(at_resp));
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCMQTTCONN=%d,%d,%d,%d,%d\r\n", client->tls_mode,
                               client->command_timeout_ms, client->keep_alive_interval, client->clean_session,
                               client->auto_connect_enable);
            break;
        case MODULE_CMD_DISCONNECT:
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCMQTTDISCONN\r\n");
            break;
        case MODULE_CMD_PUBLISHL:
            len = _transfer_payload(params->publish_params.params->payload, &payload_transferred);
            if (!len) {
                return QCLOUD_ERR_MALLOC;
            }

            strncpy(at_resp, ">", sizeof(at_resp));
            HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCMQTTPUBL=\"%s\",%d,%d\r\n", params->publish_params.topic_name,
                         params->publish_params.params->qos, len);
            rc = HAL_Module_SendAtCmdWaitResp(at_cmd, at_resp, client->command_timeout_ms);
            if (rc) {
                HAL_Free(payload_transferred);
                return rc;
            }

            strncpy(at_resp, "+TCMQTTPUBL:OK", sizeof(at_resp));
            rc = HAL_Module_SendAtData(payload_transferred, len);
            HAL_Free(payload_transferred);
            if (rc) {
                return rc;
            }
            return HAL_Module_SendAtCmdWaitResp("\r\n", at_resp, client->command_timeout_ms);
        case MODULE_CMD_PUBLISH:
            strncpy(at_resp, "+TCMQTTPUB:OK", sizeof(at_resp));
            len = _transfer_payload(params->publish_params.params->payload, &payload_transferred);
            if (!len) {
                return QCLOUD_ERR_MALLOC;
            }
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCMQTTPUB=\"%s\",%d,\"%s\"\r\n",
                               params->publish_params.topic_name, params->publish_params.params->qos,
                               payload_transferred);
            HAL_Free(payload_transferred);
            break;
        case MODULE_CMD_SUBSCRIBE:
            strncpy(at_resp, "+TCMQTTSUB:OK", sizeof(at_resp));
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCMQTTSUB=\"%s\",%d\r\n",
                               params->subscribe_params.topic_filter, params->subscribe_params.qos);
            break;
        case MODULE_CMD_UNSUBSCRIBE:
            strncpy(at_resp, "+TCMQTTUNSUB:OK", sizeof(at_resp));
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCMQTTUNSUB=\"%s\"\r\n",
                               params->subscribe_params.topic_filter);
            break;
        case MODULE_CMD_GET_CONNECT_STATUS:
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCMQTTSTATE?\r\n");
            break;
        case MODULE_CMD_SET_OTA_VERSION:
            strncpy(at_resp, "+TCOTASET:OK", sizeof(at_resp));
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCOTASET=%d,\"%s\"\r\n",
                               params->set_ota_version_params.cls_state, params->set_ota_version_params.version);
            break;
        case MODULE_CMD_GET_OTA_FW_INFO:
            len = HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCFWINFO?\r\n");
            break;
        case MODULE_CMD_READ_OTA_FW_DATA:
            strncpy(at_resp, "+TCREADFWDATA:%u,", sizeof(at_resp));
            HAL_Snprintf(at_cmd, sizeof(at_cmd), "AT+TCREADFWDATA=%d\r\n",
                         *(params->read_ota_fw_data_params.fw_data_len));
            return HAL_Module_SendAtCmdWaitRespWithData(at_cmd, at_resp, params->read_ota_fw_data_params.fw_data_buf,
                                                        params->read_ota_fw_data_params.fw_data_len,
                                                        params->read_ota_fw_data_params.timeout);
        default:
            return QCLOUD_ERR_INVAL;
    }
    return len ? HAL_Module_SendAtCmdWaitResp(at_cmd, at_resp[0] ? at_resp : NULL,
                                              client ? client->command_timeout_ms : DEFAULT_TIMEOUT_MS)
               : QCLOUD_ERR_BUF_TOO_SHORT;
}

// ----------------------------------------------------------------------------
// device info
// ----------------------------------------------------------------------------

/**
 * @brief Set device info.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_device_info_set(const QcloudIotClient *client)
{
    ModuleCmdParams cmd_params = {0};
    cmd_params.client          = client;
    return _module_send_cmd(MODULE_CMD_SET_DEVICE_INFO, &cmd_params);
}

// ----------------------------------------------------------------------------
// mqtt
// ----------------------------------------------------------------------------

/**
 * @brief Connect mqtt server.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_mqtt_connect(const QcloudIotClient *client)
{
    ModuleCmdParams cmd_params = {0};
    cmd_params.client          = client;
    return _module_send_cmd(MODULE_CMD_CONNECT, &cmd_params);
}

/**
 * @brief Disconnect mqtt server.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_mqtt_disconnect(const QcloudIotClient *client)
{
    ModuleCmdParams cmd_params = {0};
    cmd_params.client          = client;
    return _module_send_cmd(MODULE_CMD_DISCONNECT, &cmd_params);
}

/**
 * @brief Publish mqtt message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_name topic name to publish
 * @param[in] params params for publish
 * @return 0 for success
 */
int module_mqtt_publish(const QcloudIotClient *client, const char *topic_name, const PublishParams *params)
{
    ModuleCmdParams cmd_params           = {0};
    cmd_params.client                    = client;
    cmd_params.publish_params.params     = params;
    cmd_params.publish_params.topic_name = topic_name;
    return params->payload_len > MAX_PUB_SHORT_LEN ? _module_send_cmd(MODULE_CMD_PUBLISHL, &cmd_params)
                                                   : _module_send_cmd(MODULE_CMD_PUBLISH, &cmd_params);
}

/**
 * @brief Subscribe mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to subscribe
 * @param[in] qos max qos for subscribe
 * @return 0 for success
 */
int module_mqtt_subscribe(const QcloudIotClient *client, const char *topic_filter, int qos)
{
    ModuleCmdParams cmd_params               = {0};
    cmd_params.client                        = client;
    cmd_params.subscribe_params.topic_filter = topic_filter;
    cmd_params.subscribe_params.qos          = qos;
    return _module_send_cmd(MODULE_CMD_SUBSCRIBE, &cmd_params);
}

/**
 * @brief Unsubscribe mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to unsubscribe
 * @return 0 for success
 */
int module_mqtt_unsubscribe(const QcloudIotClient *client, const char *topic_filter)
{
    ModuleCmdParams cmd_params                 = {0};
    cmd_params.client                          = client;
    cmd_params.unsubscribe_params.topic_filter = topic_filter;
    return _module_send_cmd(MODULE_CMD_UNSUBSCRIBE, &cmd_params);
}

/**
 * @brief Get mqtt connect state
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_mqtt_get_connect_state(const QcloudIotClient *client)
{
    ModuleCmdParams cmd_params = {0};
    cmd_params.client          = client;
    return _module_send_cmd(MODULE_CMD_GET_CONNECT_STATUS, &cmd_params);
}

// ----------------------------------------------------------------------------
// ota
// ----------------------------------------------------------------------------

/**
 * @brief Set fw version and report.
 *
 * @param[in] cls_state 0: off;1: only report mcu version;2: only report module version;3: report all
 * @param[in] version mcu fw version
 * @return 0 for success
 */
int module_ota_set_fw_version(QcloudIotOtaClsState cls_state, const char *version)
{
    ModuleCmdParams cmd_params                  = {0};
    cmd_params.client                           = NULL;
    cmd_params.set_ota_version_params.cls_state = cls_state;
    cmd_params.set_ota_version_params.version   = version;
    return _module_send_cmd(MODULE_CMD_SET_OTA_VERSION, &cmd_params);
}

/**
 * @brief Get fw info.
 *
 * @return 0 for success
 */
int module_ota_get_fw_info(void)
{
    ModuleCmdParams cmd_params = {0};
    cmd_params.client          = NULL;
    return _module_send_cmd(MODULE_CMD_GET_OTA_FW_INFO, &cmd_params);
}

/**
 * @brief Read fw data.
 *
 * @param[out] fw_data_buf data buffer
 * @param[out] fw_data_len data recv length
 * @param[in] timeout read timeout
 * @return 0 for success
 */
int module_ota_read_fw_data(void *fw_data_buf, uint32_t *fw_data_len, uint32_t timeout)
{
    ModuleCmdParams cmd_params                     = {0};
    cmd_params.client                              = NULL;
    cmd_params.read_ota_fw_data_params.fw_data_buf = fw_data_buf;
    cmd_params.read_ota_fw_data_params.fw_data_len = fw_data_len;
    cmd_params.read_ota_fw_data_params.timeout     = timeout;
    return _module_send_cmd(MODULE_CMD_READ_OTA_FW_DATA, &cmd_params);
}
