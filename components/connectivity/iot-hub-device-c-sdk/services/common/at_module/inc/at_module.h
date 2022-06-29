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
 * @file at_mqtt_client.h
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

#ifndef IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_AT_MODULE_INC_AT_MODULE_H_
#define IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_AT_MODULE_INC_AT_MODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_mqtt_client.h"

/**
 * @brief Max number of topic subscribed
 *
 */
#define MAX_MESSAGE_HANDLERS (10)

/**
 * @brief Max number of ota fw version
 *
 */
#define MAX_FW_VERSION_STRING (32)

/**
 * @brief Max publish length
 *
 */
#define MAX_PUB_SHORT_LEN 128

/**
 * @brief Subscribe status of topic.
 *
 */
typedef enum {
    SUB_ACK_NOT_RECEIVED = 0,
    SUB_ACK_RECEIVED     = 1,
} SubStatus;

/**
 * @brief data structure for topic subscription handle
 *
 */
typedef struct {
    char           *topic_filter; /**< topic name, wildcard filter is supported */
    SubStatus       status;       /**< status of sub handle */
    SubscribeParams params;       /**< params needed to subscribe */
} SubTopicHandle;

/**
 * @brief
 *
 */
typedef enum {
    TLS_MODE_NO_TLS   = 0,
    TLS_MODE_TLS_PSK  = 1,
    TLS_MODE_TLS_CERT = 2,
} QcloudIotTlsMode;

/**
 * @brief
 *
 */
typedef enum {
    MQTT_CONNECT_STATE_CONNECTED = 0,
    MQTT_CONNECT_STATE_RECONNECTING,
    MQTT_CONNECT_STATE_RECONNECTED,
    MQTT_CONNECT_STATE_DISCONNECT,
} MQTTConnectState;

/**
 * @brief MQTT QCloud IoT Client structure
 *
 */
typedef struct {
    DeviceInfo      *device_info;                          /**< device info needed to connect server */
    uint32_t         command_timeout_ms;                   /**< MQTT command timeout, unit:ms */
    MQTTEventHandler event_handle;                         /**< callback for MQTT event */
    uint8_t          default_subscribe;                    /**< no subscribe packet send, only add subhandle */
    void            *lock_generic;                         /**< mutex/lock for this client struture */
    SubTopicHandle   sub_handles[MAX_MESSAGE_HANDLERS];    /**< subscription handle array */
    uint8_t          read_buf[QCLOUD_IOT_MQTT_RX_BUF_LEN]; /**< MQTT read buffer */
    size_t           read_buf_size;                        /**< size of MQTT read buffer */
    MQTTConnectState connect_state;                        /**< connect state */

    // connect options using in module
    QcloudIotTlsMode tls_mode;            /**< 0:no tls; 1:tls+psk; 2:tls+cert */
    uint8_t          auto_connect_enable; /**< enable auto connection or not */
    uint8_t          clean_session;       /**< clean session option */
    uint16_t         keep_alive_interval; /**< keepalive option */
} QcloudIotClient;

/**
 * @brief Cls state.
 *
 */
typedef enum {
    IOT_OTA_CLS_STATE_NO_REPORT = 0,
    IOT_OTA_CLS_STATE_REPORT_MCU,
    IOT_OTA_CLS_STATE_REPORT_MODULE, /**< only support in at set 1.8.0 */
    IOT_OTA_CLS_STATE_REPORT_BOTH    /**< only support in at set 1.8.0 */
} QcloudIotOtaClsState;

/**
 * @brief OTA fw info.
 *
 */
typedef struct {
    char     version[MAX_FW_VERSION_STRING];
    uint32_t fw_size;
    uint32_t fw_max_size;
    char     md5[33];
    void    *get_fw_info_sem;
} QcloudIotOtaFwInfo;

// ----------------------------------------------------------------------------
// cmd
// ----------------------------------------------------------------------------

/**
 * @brief Set device info.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_device_info_set(const QcloudIotClient *client);

/**
 * @brief Connect mqtt server.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_mqtt_connect(const QcloudIotClient *client);

/**
 * @brief Disconnect mqtt server.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_mqtt_disconnect(const QcloudIotClient *client);

/**
 * @brief Publish mqtt message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_name topic name to publish
 * @param[in] params params for publish
 * @return 0 for success
 */
int module_mqtt_publish(const QcloudIotClient *client, const char *topic_name, const PublishParams *params);

/**
 * @brief Subscribe mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to subscribe
 * @param[in] qos max qos for subscribe
 * @return 0 for success
 */
int module_mqtt_subscribe(const QcloudIotClient *client, const char *topic_filter, int qos);

/**
 * @brief Unsubscribe mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter to unsubscribe
 * @return 0 for success
 */
int module_mqtt_unsubscribe(const QcloudIotClient *client, const char *topic_filter);

/**
 * @brief Get mqtt connect state
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success
 */
int module_mqtt_get_connect_state(const QcloudIotClient *client);

/**
 * @brief Set fw version and report.
 *
 * @param[in] cls_state 0: off;1: only report mcu version;2: only report module version;3: report all
 * @param[in] version mcu fw version
 * @return 0 for success
 */
int module_ota_set_fw_version(QcloudIotOtaClsState cls_state, const char *version);

/**
 * @brief Get fw info.
 *
 * @return 0 for success
 */
int module_ota_get_fw_info(void);

/**
 * @brief Read fw data.
 *
 * @param[out] fw_data_buf data buffer
 * @param[out] fw_data_len data recv length
 * @param[in] timeout read timeout
 * @return 0 for success
 */
int module_ota_read_fw_data(void *fw_data_buf, uint32_t *fw_data_len, uint32_t timeout);

// ----------------------------------------------------------------------------
// urc
// ----------------------------------------------------------------------------

/**
 * @brief Set mqtt urc to at module.
 *
 * @return 0 means successful.
 */
int module_set_mqtt_urc(void);

/**
 * @brief Set ota urc to at module.
 *
 * @return 0 means successful.
 */
int module_set_ota_urc(void);

// ----------------------------------------------------------------------------
// mqtt
// ----------------------------------------------------------------------------

/**
 * @brief Get pointer to mqtt client.
 *
 * @return pointer to mqtt client.
 */
QcloudIotClient *qcloud_iot_get_mqtt_client(void);

/**
 * @brief Serialize and send subscribe packet.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic to subscribe
 * @param[in] params subscribe params
 * @return >=0 for packet id, < 0 for failed @see IotReturnCode
 */
int qcloud_iot_mqtt_subscribe(QcloudIotClient *client, const char *topic_filter, const SubscribeParams *params);

/**
 * @brief Serialize and send unsubscribe packet.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic to unsubscribe
 * @return >=0 packet id, < 0 for failed @see IotReturnCode
 */
int qcloud_iot_mqtt_unsubscribe(QcloudIotClient *client, const char *topic_filter);

/**
 * @brief Return if topic is sub ready.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter
 * @return true for ready
 * @return false for not ready
 */
bool qcloud_iot_mqtt_is_sub_ready(QcloudIotClient *client, const char *topic_filter);

/**
 * @brief Get usr data, usr should handle lock/unlock usrdata itself in callback and caller.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_filter topic filter
 * @return NULL or user data
 */
void *qcloud_iot_mqtt_get_subscribe_usr_data(QcloudIotClient *client, const char *topic_filter);

/**
 * @brief Clear sub handle array.
 *
 * @param[in,out] client pointer to mqtt client
 */
void qcloud_iot_mqtt_sub_handle_array_clear(QcloudIotClient *client);

/**
 * @brief Deliver message to subscribe message handler.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic_name topic name, no wildcard
 * @param[in] payload publish packet payload
 * @param[in] payload_len payload length
 */
void qcloud_iot_deliver_message(QcloudIotClient *client, char *topic_name, char *payload, int payload_len);

/**
 * @brief Get ota info handle.
 *
 * @return @see QcloudIotOtaFwInfo
 */
QcloudIotOtaFwInfo *qcloud_iot_get_ota_info_handle(void);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_AT_MODULE_INC_AT_MODULE_H_
