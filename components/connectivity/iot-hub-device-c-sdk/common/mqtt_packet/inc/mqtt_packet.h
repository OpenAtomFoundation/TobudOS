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
 * @file mqtt_packet.h
 * @brief header file for mqtt packet
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-24 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_COMMON_MQTT_PACKET_INC_MQTT_PACKET_H_
#define IOT_HUB_DEVICE_C_SDK_COMMON_MQTT_PACKET_INC_MQTT_PACKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

/**
 * @brief header 1 byte + remaining length 1~4 byte(s).
 *
 */
#define MAX_NO_OF_REMAINING_LENGTH_BYTES 4
#define MAX_MQTT_FIXED_HEADER_LEN        (1 + MAX_NO_OF_REMAINING_LENGTH_BYTES)
#define MIN_MQTT_FIXED_HEADER_LEN        (1 + 1)

/**
 * @brief Check if short buffer.
 *
 */
#define SHORT_BUFFER_CHECK(buf_len, len) \
    if (buf_len < len) {                 \
        return MQTT_ERR_SHORT_BUFFER;    \
    }

/**
 * @brief Error code for mqtt packet。
 *
 */
typedef enum {
    MQTT_RET_PACKET_OK           = 0,
    MQTT_ERR_INVALID_PACKET_TYPE = -1,
    MQTT_ERR_SHORT_BUFFER        = -2,
    MQTT_ERR_VERSION_UNSUPPORTED = -3,
    MQTT_ERR_SUB_COUNT_EXCEED    = -4,
} MQTTPacketErrCode;

/**
 * @brief MQTT packet type.
 *
 */
typedef enum {
    CONNECT = 1,
    CONNACK,
    PUBLISH,
    PUBACK,
    PUBREC,
    PUBREL,
    PUBCOMP,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PINGREQ,
    PINGRESP,
    DISCONNECT
} MQTTPacketType;

/**
 * @brief Defines the MQTT "Last Will and Testament" (LWT) settings for the connect packet.
 *
 */
typedef struct {
    const char* topic_name; /**< The LWT topic to which the LWT message will be published */
    const char* message;    /**< The LWT payload */
    uint8_t     retained;   /**< The retained flag for the LWT message */
    uint8_t     qos;        /**< The quality of service setting for the LWT message */
} MQTTPacketWillOptions;

/**
 * @brief MQTT packet connect option.
 *
 */
typedef struct {
    uint8_t               mqtt_version; /**< Version of MQTT to be used. 3 = 3.1 4 = 3.1.1 */
    const char*           client_id;
    uint16_t              keep_alive_interval;
    uint8_t               clean_session;
    uint8_t               will_flag;
    MQTTPacketWillOptions will;
    char*                 username;
    char*                 password;
} MQTTPacketConnectOption;

/**
 * @brief Bitfields for the MQTT header byte.
 *
 */
typedef union {
    uint8_t byte; /**< the whole byte */
    struct {
        uint8_t retain : 1; /**< retained flag bit */
        uint8_t qos : 2;    /**< QoS value, 0, 1 or 2 */
        uint8_t dup : 1;    /**< DUP flag bit */
        uint8_t type : 4;   /**< message type nibble */
    } bits;
} MQTTHeader;

/**
 * @brief Connect flags byte.
 *
 */
typedef union {
    uint8_t all; /**< all connect flags */

    struct {
        uint8_t : 1;               /**< unused */
        uint8_t clean_session : 1; /**< cleansession flag */
        uint8_t will : 1;          /**< will flag */
        uint8_t will_qos : 2;      /**< will QoS value */
        uint8_t will_retain : 1;   /**< will retain setting */
        uint8_t password : 1;      /**< 3.1 password */
        uint8_t username : 1;      /**< 3.1 user name */
    } bits;
} MQTTConnectFlags;

/**
 * @brief connack flags byte.
 *
 */
typedef union {
    unsigned char all; /**< all connack flags */
    struct {
        unsigned int sessionpresent : 1; /**< session present flag */
        unsigned int reserved : 7;       /**< unused */
    } bits;
} MQTTConnackFlags;

/**
 * @brief Flags for publish.
 *
 */
typedef struct {
    uint8_t dup;
    uint8_t qos;
    uint8_t retain;
} MQTTPublishFlags;

/**
 * @brief Connect return code.
 *
 */
typedef enum {
    CONNACK_CONNECTION_ACCEPTED                 = 0, /**< connection accepted */
    CONNACK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = 1, /**< connection refused: unaccepted protocol version */
    CONNACK_IDENTIFIER_REJECTED_ERROR           = 2, /**< connection refused: identifier rejected */
    CONNACK_SERVER_UNAVAILABLE_ERROR            = 3, /**< connection refused: server unavailable */
    CONNACK_BAD_USERDATA_ERROR                  = 4, /**< connection refused: bad user name or password */
    CONNACK_NOT_AUTHORIZED_ERROR                = 5  /**< connection refused: not authorized */
} MQTTConnackReturnCodes;

/**
 * @brief Serialize the connect options into the buffer. See 3.1.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[in] options the options to be used to build the connect packet
 * @return serialized length, or error if <= 0
 */
int mqtt_connect_packet_serialize(uint8_t* buf, int buf_len, const MQTTPacketConnectOption* options);

/**
 * @brief Serialize a disconnect packet into the supplied buffer, ready for writing to a socket. See 3.14.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer, to avoid overruns
 * @return serialized length, or error if <= 0
 */
int mqtt_disconnect_packet_serialize(uint8_t* buf, int buf_len);

/**
 * @brief Serialize a disconnect packet into the supplied buffer, ready for writing to a socket. See 3.12.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer, to avoid overruns
 * @return serialized length, or error if <= 0
 */
int mqtt_pingreq_packet_serialize(uint8_t* buf, int buf_len);

/**
 * @brief Serialize the supplied publish data into the supplied buffer, ready for sending. See 3.3.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[in] flags the MQTT dup, qos, retained flag
 * @param[in] packet_id integer - the MQTT packet identifier
 * @param[in] topic_name char * - the MQTT topic in the publish
 * @param[in] payload byte buffer - the MQTT publish payload
 * @param[in] payload_len integer - the length of the MQTT payload
 * @return serialized length, or error if <= 0
 */
int mqtt_publish_packet_serialize(uint8_t* buf, int buf_len, const MQTTPublishFlags* flags, uint16_t packet_id,
                                  const char* topic_name, const uint8_t* payload, int payload_len);

/**
 * @brief Serialize a puback packet into the supplied buffer. See 3.4.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[in] packet_id integer - the MQTT packet identifier
 * @return serialized length, or error if <= 0
 */
int mqtt_puback_packet_serialize(uint8_t* buf, int buf_len, uint16_t packet_id);

/**
 * @brief Serialize the supplied subscribe data into the supplied buffer, ready for sending. See 3.8.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied bufferr
 * @param[in] packet_id integer - the MQTT packet identifier
 * @param[in] count - number of members in the topicFilters and reqQos arrays
 * @param[in] topic_filters - array of topic filter names
 * @param[in] requested_qos - array of requested QoS
 * @return serialized length, or error if <= 0
 */
int mqtt_subscribe_packet_serialize(uint8_t* buf, int buf_len, uint16_t packet_id, uint32_t count,
                                    char* topic_filters[], const int requested_qos[]);

/**
 * @brief Serialize the supplied unsubscribe data into the supplied buffer, ready for sending. See 3.10.
 *
 * @param[out] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[in] packet_id integer - the MQTT packet identifier
 * @param[in] count - number of members in the topicFilters array
 * @param[in] topic_filters - array of topic filter names
 * @return serialized length, or error if <= 0
 */
int mqtt_unsubscribe_packet_serialize(uint8_t* buf, int buf_len, uint16_t packet_id, int count, char* topic_filters[]);

/**
 * @brief Deserialize the supplied (wire) buffer into connack data. See 3.2.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] session_present the session present flag returned (only for MQTT 3.1.1)
 * @param[out] connack_rc returned integer value of the connack return code
 * @return @see MQTTPacketErrCode
 */
int mqtt_connack_packet_deserialize(uint8_t* buf, int buf_len, uint8_t* session_present, uint8_t* connack_rc);

/**
 * @brief Deserialize the supplied (wire) buffer into pingresp data. See 3.13.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @return @see MQTTPacketErrCode
 */
int mqtt_pingresp_packet_deserialize(uint8_t* buf, int buf_len);

/**
 * @brief Deserialize the supplied (wire) buffer into publish data. See 3.3.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] flags the MQTT dup, qos, retained flag
 * @param[out] packet_id returned integer - the MQTT packet identifier
 * @param[out] topic_name returned string - the MQTT topic in the publish
 * @param[out] topic_len returned integer - the length of the MQTT topic
 * @param[out] payload returned byte buffer - the MQTT publish payload
 * @param[out] payload_len returned integer - the length of the MQTT payload
 * @return @see MQTTPacketErrCode
 */
int mqtt_publish_packet_deserialize(uint8_t* buf, int buf_len, MQTTPublishFlags* flags, uint16_t* packet_id,
                                    char** topic_name, int* topic_len, uint8_t** payload, int* payload_len);

/**
 * @brief Deserialize the supplied (wire) buffer into an ack. See 3.4.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] packet_id returned integer - the MQTT packet identifier
 * @return @see MQTTPacketErrCode
 */
int mqtt_puback_packet_deserialize(uint8_t* buf, int buf_len, uint16_t* packet_id);

/**
 * @brief Deserialize the supplied (wire) buffer into suback data. See 3.9.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[in] maxcount - the maximum number of members allowed in the grantedQoSs array
 * @param[out] count returned integer - number of members in the grantedQoSs array
 * @param[out] packet_id returned integer - the MQTT packet identifier
 * @param[out] granted_qos returned array of integers - the granted qualities of service
 * @return @see MQTTPacketErrCode
 */
int mqtt_suback_packet_deserialize(uint8_t* buf, int buf_len, int maxcount, int* count, uint16_t* packet_id,
                                   int granted_qos[]);

/**
 * @brief Deserialize the supplied (wire) buffer into unsuback data. See 3.11.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] packet_id returned integer - the MQTT packet identifier
 * @return @see MQTTPacketErrCode
 */
int mqtt_unsuback_packet_deserialize(uint8_t* buf, int buf_len, uint16_t* packet_id);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_COMMON_MQTT_PACKET_INC_MQTT_PACKET_H_
