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
 * @file qcloud_iot_error.h
 * @brief error code of sdk
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotReturnCode and QcloudIotClient
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_ERROR_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IOT SDK return/error code.
 * Enumeration of return code in QCloud IoT C-SDK.
 * Values less than 0 are specific error codes
 * Value of 0 is successful return
 * Values greater than 0 are specific non-error return codes
 *
 */
typedef enum {
    QCLOUD_RET_SUCCESS                          = 0,    /**< Successful return */
    QCLOUD_ERR_FAILURE                          = -1,   /**< Generic failure return */
    QCLOUD_ERR_INVAL                            = -2,   /**< Invalid parameter */
    QCLOUD_ERR_DEV_INFO                         = -3,   /**< Fail to get device info */
    QCLOUD_ERR_MALLOC                           = -4,   /**< Fail to malloc memory */
    QCLOUD_RET_MQTT_ALREADY_CONNECTED           = 4,    /**< Already connected with MQTT server */
    QCLOUD_RET_MQTT_CONNACK_CONNECTION_ACCEPTED = 3,    /**< MQTT connection accepted by server */
    QCLOUD_RET_MQTT_MANUALLY_DISCONNECTED       = 2,    /**< Manually disconnected with MQTT server */
    QCLOUD_RET_MQTT_RECONNECTED                 = 1,    /**< Reconnected with MQTT server successfully */
    QCLOUD_ERR_MQTT_NO_CONN                     = -101, /**< Not connected with MQTT server */
    QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT        = -102, /**< Reconnecting with MQTT server */
    QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT           = -103, /**< MQTT reconnect timeout */
    QCLOUD_ERR_MQTT_SUB                         = -104, /**< MQTT topic subscription fail */
    QCLOUD_ERR_MQTT_NOTHING_TO_READ             = -105, /**< MQTT nothing to read */
    QCLOUD_ERR_MQTT_PACKET_READ                 = -106, /**< Something wrong when reading MQTT packet */
    QCLOUD_ERR_MQTT_REQUEST_TIMEOUT             = -107, /**< MQTT request timeout */
    QCLOUD_ERR_RX_MESSAGE_INVAL                 = -108, /**< MQTT received invalid msg */
    QCLOUD_ERR_BUF_TOO_SHORT                    = -109, /**< MQTT recv buffer not enough */
    QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT             = -110, /**< MQTT QoS level not supported */
    QCLOUD_ERR_MQTT_UNSUB_FAIL                  = -111, /**< MQTT unsubscribe failed */
    QCLOUD_ERR_MAX_TOPIC_LENGTH                 = -112, /**< Topic length oversize */
    QCLOUD_ERR_HTTP                             = -201, /**< HTTP unknown error */
    QCLOUD_ERR_HTTP_AUTH                        = -202, /**< HTTP auth failed */
    QCLOUD_ERR_HTTP_NOT_FOUND                   = -203, /**< HTTP 404 */
    QCLOUD_ERR_HTTP_TIMEOUT                     = -204, /**< HTTP timeout */
    QCLOUD_ERR_HTTP_PARSE                       = -205, /**< HTTP URL parse failed */
    QCLOUD_ERR_JSON_PARSE                       = -300, /**< JSON parsing error */
    QCLOUD_ERR_TCP_SOCKET_FAILED                = -401, /**< TLS TCP socket connect fail */
    QCLOUD_ERR_TCP_UNKNOWN_HOST                 = -402, /**< TCP unknown host (DNS fail) */
    QCLOUD_ERR_TCP_CONNECT                      = -403, /**< TCP/UDP socket connect fail */
    QCLOUD_ERR_TCP_READ_TIMEOUT                 = -404, /**< TCP read timeout */
    QCLOUD_ERR_TCP_WRITE_TIMEOUT                = -405, /**< TCP write timeout */
    QCLOUD_ERR_TCP_READ_FAIL                    = -406, /**< TCP read error */
    QCLOUD_ERR_TCP_WRITE_FAIL                   = -407, /**< TCP write error */
    QCLOUD_ERR_TCP_PEER_SHUTDOWN                = -408, /**< TCP server close connection */
    QCLOUD_ERR_TCP_NOTHING_TO_READ              = -409, /**< TCP socket nothing to read */
    QCLOUD_ERR_SSL_INIT                         = -501, /**< TLS/SSL init fail */
    QCLOUD_ERR_SSL_CERT                         = -502, /**< TLS/SSL certificate issue */
    QCLOUD_ERR_SSL_CONNECT                      = -503, /**< TLS/SSL connect fail */
    QCLOUD_ERR_SSL_CONNECT_TIMEOUT              = -504, /**< TLS/SSL connect timeout */
    QCLOUD_ERR_SSL_WRITE_TIMEOUT                = -505, /**< TLS/SSL write timeout */
    QCLOUD_ERR_SSL_WRITE                        = -506, /**< TLS/SSL write error */
    QCLOUD_ERR_SSL_READ_TIMEOUT                 = -507, /**< TLS/SSL read timeout */
    QCLOUD_ERR_SSL_READ                         = -508, /**< TLS/SSL read error */
    QCLOUD_ERR_SSL_NOTHING_TO_READ              = -509, /**< TLS/SSL nothing to read */
} IotReturnCode;

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_ERROR_H_
