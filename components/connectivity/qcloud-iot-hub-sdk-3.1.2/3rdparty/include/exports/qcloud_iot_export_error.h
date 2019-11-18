/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_EXPORT_ERROR_H_
#define QCLOUD_IOT_EXPORT_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * IOT SDK return/error code
 * Enumeration of return code in QCloud IoT C-SDK.
 * Values less than 0 are specific error codes
 * Value of 0 is successful return
 * Values greater than 0 are specific non-error return codes
 */
typedef enum {
    QCLOUD_RET_MQTT_ALREADY_CONNECTED                        = 4,       // Already connected with MQTT server
    QCLOUD_RET_MQTT_CONNACK_CONNECTION_ACCEPTED              = 3,       // MQTT connection accepted by server
    QCLOUD_RET_MQTT_MANUALLY_DISCONNECTED                    = 2,       // Manually disconnected with MQTT server
    QCLOUD_RET_MQTT_RECONNECTED                              = 1,       // Reconnected with MQTT server successfully

    QCLOUD_RET_SUCCESS                                       = 0,       // Successful return
    
    QCLOUD_ERR_FAILURE                                       = -1001,   // Generic failure return
    QCLOUD_ERR_INVAL                                         = -1002,   // Invalid parameter
    QCLOUD_ERR_DEV_INFO                                      = -1003,   // Fail to get device info
    QCLOUD_ERR_MALLOC                                        = -1004,   // Fail to malloc memory

    QCLOUD_ERR_HTTP_CLOSED                                   = -3,      // HTTP server close the connection
    QCLOUD_ERR_HTTP                                          = -4,      // HTTP unknown error
    QCLOUD_ERR_HTTP_PRTCL                                    = -5,      // HTTP protocol error
    QCLOUD_ERR_HTTP_UNRESOLVED_DNS                           = -6,      // HTTP DNS resolve failed
    QCLOUD_ERR_HTTP_PARSE                                    = -7,      // HTTP URL parse failed
    QCLOUD_ERR_HTTP_CONN                                     = -8,      // HTTP connect failed
    QCLOUD_ERR_HTTP_AUTH                                     = -9,      // HTTP auth failed
    QCLOUD_ERR_HTTP_NOT_FOUND                                = -10,     // HTTP 404
    QCLOUD_ERR_HTTP_TIMEOUT                                  = -11,     // HTTP timeout

    QCLOUD_ERR_MQTT_PUSH_TO_LIST_FAILED                      = -102,    // Fail to push node to MQTT waiting list
    QCLOUD_ERR_MQTT_NO_CONN                                  = -103,    // Not connected with MQTT server
    QCLOUD_ERR_MQTT_UNKNOWN                                  = -104,    // MQTT unknown error
    QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT                     = -105,    // Reconnecting with MQTT server
    QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT                        = -106,    // MQTT reconnect timeout
    QCLOUD_ERR_MQTT_MAX_SUBSCRIPTIONS                        = -107,    // MQTT topic subscription out of range
    QCLOUD_ERR_MQTT_SUB                                      = -108,    // MQTT topic subscription fail
    QCLOUD_ERR_MQTT_NOTHING_TO_READ                          = -109,    // MQTT nothing to read
    QCLOUD_ERR_MQTT_PACKET_READ                              = -110,    // Something wrong when reading MQTT packet
    QCLOUD_ERR_MQTT_REQUEST_TIMEOUT                          = -111,    // MQTT request timeout
    QCLOUD_ERR_MQTT_CONNACK_UNKNOWN                          = -112,    // MQTT connection refused: unknown error
    QCLOUD_ERR_MQTT_CONNACK_UNACCEPTABLE_PROTOCOL_VERSION    = -113,    // MQTT connection refused: protocol version invalid
    QCLOUD_ERR_MQTT_CONNACK_IDENTIFIER_REJECTED              = -114,    // MQTT connection refused: identifier rejected
    QCLOUD_ERR_MQTT_CONNACK_SERVER_UNAVAILABLE               = -115,    // MQTT connection refused: service not available
    QCLOUD_ERR_MQTT_CONNACK_BAD_USERDATA                     = -116,    // MQTT connection refused: bad user name or password
    QCLOUD_ERR_MQTT_CONNACK_NOT_AUTHORIZED                   = -117,    // MQTT connection refused: not authorized
    QCLOUD_ERR_RX_MESSAGE_INVAL                              = -118,    // MQTT received invalid msg
    QCLOUD_ERR_BUF_TOO_SHORT                                 = -119,    // MQTT recv buffer not enough
    QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT                          = -120,    // MQTT QoS level not supported
    QCLOUD_ERR_MQTT_UNSUB_FAIL                               = -121,    // MQTT unsubscribe failed

    QCLOUD_ERR_JSON_PARSE                                    = -132,    // JSON parsing error
    QCLOUD_ERR_JSON_BUFFER_TRUNCATED                         = -133,    // JSON buffer truncated
    QCLOUD_ERR_JSON_BUFFER_TOO_SMALL                         = -134,    // JSON parsing buffer not enough
    QCLOUD_ERR_JSON                                          = -135,    // JSON generation error
    QCLOUD_ERR_MAX_JSON_TOKEN                                = -136,    // JSON token out of range
    
    QCLOUD_ERR_MAX_APPENDING_REQUEST                         = -137,    // appending request out of range
    QCLOUD_ERR_MAX_TOPIC_LENGTH                              = -138,    // Topic length oversize

    QCLOUD_ERR_COAP_NULL                                     = -150,    // COAP null pointer
    QCLOUD_ERR_COAP_DATA_SIZE                                = -151,    // COAP data size out of range
    QCLOUD_ERR_COAP_INTERNAL                                 = -152,    // COAP interval error
    QCLOUD_ERR_COAP_BADMSG                                   = -153,    // COAP bad msg
    QCLOUD_ERR_DTLS_PEER_CLOSE_NOTIFY                        = -160,    // DTLS connection is closed
    
    QCLOUD_ERR_SHADOW_PROPERTY_EXIST                         = -201,    // Shadow property already exist
    QCLOUD_ERR_SHADOW_NOT_PROPERTY_EXIST                     = -202,    // Shadow property not exist
    QCLOUD_ERR_SHADOW_UPDATE_TIMEOUT                         = -203,    // Shadow update timeout
    QCLOUD_ERR_SHADOW_UPDATE_REJECTED                        = -204,    // Shadow update rejected by server
    QCLOUD_ERR_SHADOW_GET_TIMEOUT                            = -205,    // Shadow get timeout
    QCLOUD_ERR_SHADOW_GET_REJECTED                           = -206,    // Shadow get rejected by server

    QCLOUD_ERR_GATEWAY_CREATE_SESSION_FAIL                   = -221,    // Gateway fail to create sub-device session
    QCLOUD_ERR_GATEWAY_SESSION_NO_EXIST                      = -222,    // Gateway sub-device session not exist
    QCLOUD_ERR_GATEWAY_SESSION_TIMEOUT                       = -223,    // Gateway sub-device session timeout
    QCLOUD_ERR_GATEWAY_SUBDEV_ONLINE                         = -224,    // Gateway sub-device online
    QCLOUD_ERR_GATEWAY_SUBDEV_OFFLINE                        = -225,    // Gateway sub-device offline

    QCLOUD_ERR_TCP_SOCKET_FAILED                             = -601,    // TLS TCP socket connect fail
    QCLOUD_ERR_TCP_UNKNOWN_HOST                              = -602,    // TCP unknown host (DNS fail)
    QCLOUD_ERR_TCP_CONNECT                                   = -603,    // TCP/UDP socket connect fail 
    QCLOUD_ERR_TCP_READ_TIMEOUT                              = -604,    // TCP read timeout
    QCLOUD_ERR_TCP_WRITE_TIMEOUT                             = -605,    // TCP write timeout
    QCLOUD_ERR_TCP_READ_FAIL                                 = -606,    // TCP read error
    QCLOUD_ERR_TCP_WRITE_FAIL                                = -607,    // TCP write error
    QCLOUD_ERR_TCP_PEER_SHUTDOWN                             = -608,    // TCP server close connection
    QCLOUD_ERR_TCP_NOTHING_TO_READ                           = -609,    // TCP socket nothing to read

    QCLOUD_ERR_SSL_INIT                                      = -701,    // TLS/SSL init fail
    QCLOUD_ERR_SSL_CERT                                      = -702,    // TLS/SSL certificate issue
    QCLOUD_ERR_SSL_CONNECT                                   = -703,    // TLS/SSL connect fail
    QCLOUD_ERR_SSL_CONNECT_TIMEOUT                           = -704,    // TLS/SSL connect timeout
    QCLOUD_ERR_SSL_WRITE_TIMEOUT                             = -705,    // TLS/SSL write timeout
    QCLOUD_ERR_SSL_WRITE                                     = -706,    // TLS/SSL write error
    QCLOUD_ERR_SSL_READ_TIMEOUT                              = -707,    // TLS/SSL read timeout
    QCLOUD_ERR_SSL_READ                                      = -708,    // TLS/SSL read error
    QCLOUD_ERR_SSL_NOTHING_TO_READ                           = -709,    // TLS/SSL nothing to read
} IoT_Return_Code;

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_ERROR_H_ */
