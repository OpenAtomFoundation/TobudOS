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
 * @file mqtt_packet_deserialize.c
 * @brief implements mqtt packet deserialize. Reference paho.mqtt.embedded-c &
 * http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.pdf
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-21
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-21 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "mqtt_packet.h"

/**
 * @brief Reads one character from the input buffer. See 1.5.1.
 *
 * @param[in,out] pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the character read
 */
static inline char _read_char(uint8_t** pptr)
{
    char c = **pptr;
    (*pptr)++;
    return c;
}

/**
 * @brief Calculates an integer from two bytes read from the input buffer. See 1.5.2.
 *
 * @param[in,out] pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the integer value calculated
 */
static inline uint16_t _read_int(uint8_t** pptr)
{
    uint8_t* ptr = *pptr;
    int      len = 256 * (*ptr) + (*(ptr + 1));
    *pptr += 2;
    return (uint16_t)len;
}

/**
 * @brief Read a UTF-8 encoded string pointer from the input buffer. See 1.5.3.
 *
 * @param[out] pstring the pointer of the C string to read.
 * @param[in,out] pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the length of string
 */
static inline int _read_string(char** pstring, uint8_t** pptr)
{
    int str_len = _read_int(pptr);
    if (str_len > 0) {
        *pstring = (char*)*pptr;
        *pptr += str_len;
    }

    return str_len;
}

/**
 * @brief Check the mqtt type and get publish flags. See 2.2.
 *
 * @param[in] packet_type type of mqtt control packet, See 2.2.1
 * @param[in] pflags pointer to the flags of mqtt control packet fixed header. See 2.2.2
 * @param[in,out] pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return @see MQTTPacketErrCode
 */
static int _mqtt_packet_type_check(MQTTPacketType packet_type, MQTTPublishFlags* pflags, uint8_t** pptr)
{
    MQTTHeader header = {0};

    header.byte = _read_char(pptr);

    switch (header.bits.type) {
        case PUBLISH:
            pflags->dup    = header.bits.dup;
            pflags->qos    = header.bits.qos;
            pflags->retain = header.bits.retain;
            break;
        case SUBSCRIBE:
        case UNSUBSCRIBE:
            if ((header.byte & 0xf0) != 0x10) {
                return MQTT_ERR_INVALID_PACKET_TYPE;
            }
            break;
        default:
            break;
    }

    return packet_type != header.bits.type ? MQTT_ERR_INVALID_PACKET_TYPE : MQTT_RET_PACKET_OK;
}

/**
 * @brief Deserialize remaining length. See 2.2.3.
 *
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[in] plen the pointer to the remaining length
 * @param[in,out] pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return @see MQTTPacketErrCode
 */
static int _mqtt_remaining_length_deserialize(int buf_len, int* plen, uint8_t** pptr)
{
    uint8_t* buf        = *pptr;
    uint8_t  c          = 0;
    uint32_t multiplier = 1;
    uint32_t count      = 0;
    uint32_t len        = 0;

    do {
        if (++count > MAX_NO_OF_REMAINING_LENGTH_BYTES) {
            return MQTT_ERR_INVALID_PACKET_TYPE;
        }
        c = *buf++;
        len += (c & 127) * multiplier;
        multiplier *= 128;

        SHORT_BUFFER_CHECK(buf_len, count + 1 + len);
    } while (c & 128);

    if (plen) {
        *plen = len;
    }

    *pptr += count;
    return MQTT_RET_PACKET_OK;
}

/**
 * @brief Deserialize the supplied (wire) buffer into connack data. See 3.2.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] session_present the session present flag returned (only for MQTT 3.1.1)
 * @param[out] connack_rc returned integer value of the connack return code
 * @return @see MQTTPacketErrCode
 */
int mqtt_connack_packet_deserialize(uint8_t* buf, int buf_len, uint8_t* session_present, uint8_t* connack_rc)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t*         ptr   = buf;
    int              rc    = 0;
    MQTTConnackFlags flags = {0};

    rc = _mqtt_packet_type_check(CONNACK, NULL, &ptr);
    if (rc) {
        goto exit;
    }

    rc = _mqtt_remaining_length_deserialize(buf_len, NULL, &ptr);
    if (rc < 0) {
        goto exit;
    }

    flags.all        = _read_char(&ptr);
    *session_present = flags.bits.sessionpresent;
    *connack_rc      = _read_char(&ptr);
exit:
    return rc;
}

/**
 * @brief Deserialize the supplied (wire) buffer into pingresp data. See 3.13.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @return @see MQTTPacketErrCode
 */
int mqtt_pingresp_packet_deserialize(uint8_t* buf, int buf_len)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t* ptr = buf;
    int      rc  = 0;

    rc = _mqtt_packet_type_check(PINGRESP, NULL, &ptr);
    if (rc) {
        goto exit;
    }

    rc = _mqtt_remaining_length_deserialize(buf_len, NULL, &ptr);
    if (rc < 0) {
        goto exit;
    }
exit:
    return rc;
}

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
                                    char** topic_name, int* topic_len, uint8_t** payload, int* payload_len)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t* ptr        = buf;
    uint8_t* ptr_remain = NULL;
    int      rc         = 0;
    int      len        = 0;

    rc = _mqtt_packet_type_check(PUBLISH, flags, &ptr);
    if (rc) {
        goto exit;
    }

    rc = _mqtt_remaining_length_deserialize(buf_len, &len, &ptr);
    if (rc) {
        goto exit;
    }

    ptr_remain = ptr;

    rc = _read_string(topic_name, &ptr);
    if (rc <= 0) {
        rc = MQTT_ERR_INVALID_PACKET_TYPE;
        goto exit;
    }
    *topic_len = rc;

    if (flags->qos > 0) {
        *packet_id = _read_int(&ptr);
    }

    *payload_len = len - (ptr - ptr_remain);
    *payload     = ptr;

    rc = MQTT_RET_PACKET_OK;
exit:
    return rc;
}

/**
 * @brief Deserialize the supplied (wire) buffer into an ack. See 3.4.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] packet_id returned integer - the MQTT packet identifier
 * @return @see MQTTPacketErrCode
 */
int mqtt_puback_packet_deserialize(uint8_t* buf, int buf_len, uint16_t* packet_id)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t* ptr = buf;
    int      rc  = 0;

    rc = _mqtt_packet_type_check(PUBACK, 0, &ptr);
    if (rc) {
        goto exit;
    }

    rc = _mqtt_remaining_length_deserialize(buf_len, NULL, &ptr);
    if (rc) {
        goto exit;
    }

    *packet_id = _read_int(&ptr);
exit:
    return rc;
}

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
                                   int granted_qos[])
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t* ptr = buf;
    int      rc  = 0;
    int      len = 0;

    rc = _mqtt_packet_type_check(SUBACK, NULL, &ptr);
    if (rc) {
        goto exit;
    }

    rc = _mqtt_remaining_length_deserialize(buf_len, &len, &ptr);
    if (rc) {
        goto exit;
    }

    *packet_id = _read_int(&ptr);
    len -= 2;

    *count = 0;
    while (len--) {
        if (*count > maxcount) {
            rc = MQTT_ERR_SUB_COUNT_EXCEED;
            goto exit;
        }
        granted_qos[(*count)++] = _read_char(&ptr);
    }

exit:
    return rc;
}

/**
 * @brief Deserialize the supplied (wire) buffer into unsuback data. See 3.11.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] packet_id returned integer - the MQTT packet identifier
 * @return @see MQTTPacketErrCode
 */
int mqtt_unsuback_packet_deserialize(uint8_t* buf, int buf_len, uint16_t* packet_id)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t* ptr = buf;
    int      rc  = 0;

    rc = _mqtt_packet_type_check(UNSUBACK, NULL, &ptr);
    if (rc) {
        goto exit;
    }

    rc = _mqtt_remaining_length_deserialize(buf_len, NULL, &ptr);
    if (rc) {
        goto exit;
    }

    *packet_id = _read_int(&ptr);
exit:
    return rc;
}
