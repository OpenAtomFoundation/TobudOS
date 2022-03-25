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
 * @file mqtt_packet_serialize.c
 * @brief implements mqtt packet serialize. Reference paho.mqtt.embedded-c &
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
 * @brief Writes one character to an output buffer. See 1.5.1.
 *
 * @param[in] c the character to write
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static inline void _write_char(char c, uint8_t** pptr)
{
    **pptr = c;
    (*pptr)++;
}

/**
 * @brief Writes an integer as 2 bytes to an output buffer. See 1.5.2.
 *
 * @param[in] value the integer to write.
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned.
 */
static inline void _write_int(int value, uint8_t** pptr)
{
    **pptr = (uint8_t)(value / 256);
    (*pptr)++;
    **pptr = (uint8_t)(value % 256);
    (*pptr)++;
}

/**
 * @brief Writes a UTF-8 encoded string to an output buffer.  Converts C string to
 * length-delimited. See 1.5.3.
 *
 * @param[in] string the C string to write
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static inline void _write_string(const char* string, uint8_t** pptr)
{
    int len = strlen(string);
    _write_int(len, pptr);
    memcpy(*pptr, string, len);
    *pptr += len;
}

/**
 * @brief Serialize mqtt fixed header except remaining length. See 2.2.
 *
 * @param[in] packet_type type of mqtt control packet, See 2.2.1 MQTT Control Packet type
 * @param[in] pflags pointer to the flags of mqtt control packet fixed header. See 2.2.2 Flags
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static void _mqtt_header_serialize(MQTTPacketType packet_type, const MQTTPublishFlags* pflags, uint8_t** pptr)
{
    MQTTHeader header = {0};

    switch (packet_type) {
        case PUBLISH:
            header.bits.dup    = pflags->dup;
            header.bits.qos    = pflags->qos;
            header.bits.retain = pflags->retain;
            break;
        case SUBSCRIBE:
        case UNSUBSCRIBE:
            header.byte = 0x02;
            break;
        default:
            break;
    }

    header.bits.type |= packet_type;
    _write_char(header.byte, pptr); /* write header */
}

/**
 * @brief Serialize remaining length. See 2.2.3.
 *
 * @param[in] length the length to be encoded to remaining length
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static void _mqtt_remaining_length_serialize(int length, uint8_t** pptr)
{
    char d;
    do {
        d = length % 128;
        length /= 128;

        // if there are more digits to encode, set the top bit of this digit
        if (length > 0) {
            d |= 0x80;
        }

        _write_char(d, pptr);
    } while (length > 0);
}

/**
 * @brief Serialize the remaining length of the MQTT connect packet that would be produced using the
 * supplied connect options. See 3.1.1.
 *
 * @param[in] options the options to be used to build the connect packet
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static int _mqtt_connect_remaining_length_serialize(const MQTTPacketConnectOption* options, int buf_len, uint8_t** pptr)
{
    if (options->mqtt_version != 3 && options->mqtt_version != 4) {
        return MQTT_ERR_VERSION_UNSUPPORTED;
    }

    int len = 0;

    len = options->mqtt_version == 3 ? 12 : 10;

    len += strlen(options->client_id) + 2;

    if (options->will_flag) {
        len += strlen(options->will.topic_name) + 2 + strlen(options->will.message) + 2;
    }

    if (options->username) {
        len += strlen(options->username) + 2;
    }

    if (options->password) {
        len += strlen(options->password) + 2;
    }

    SHORT_BUFFER_CHECK(buf_len, len);

    _mqtt_remaining_length_serialize(len, pptr);

    return MQTT_RET_PACKET_OK;
}

/**
 * @brief Serialize the mqtt connect packet variable header. See 3.1.2.
 *
 * @param[in] options the options to be used to build the connect packet
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static int _mqtt_connect_variable_header_serialize(const MQTTPacketConnectOption* options, uint8_t** pptr)
{
    if (options->password && !options->username) {
        return MQTT_ERR_INVALID_PACKET_TYPE;
    }

    // 3.1.2.1 protocol name & 3.1.2.2 protocol level
    switch (options->mqtt_version) {
        case 3:
            _write_string("MQIsdp", pptr);
            _write_char((char)3, pptr);
            break;
        case 4:
            _write_string("MQTT", pptr);
            _write_char((char)4, pptr);
            break;
    }

    // 3.1.2.3 Connect Flags
    MQTTConnectFlags flags = {
        .bits.clean_session = options->clean_session,
        .bits.will          = options->will_flag,
        .bits.will_qos      = options->will_flag ? options->will.qos : 0,
        .bits.will_retain   = options->will_flag ? options->will.retained : 0,
        .bits.username      = options->username ? 1 : 0,
        .bits.password      = options->password ? 1 : 0,
    };
    _write_char(flags.all, pptr);

    return MQTT_RET_PACKET_OK;
}

/**
 * @brief Serialize the remaining length of the MQTT publish packet that would be produced using the supplied
 * parameters. See 3.3.2 & 3.3.3.
 *
 * @param[in] qos the MQTT QoS of the publish (packet id is omitted for QoS 0)
 * @param[in] topic_name the topic name to be used in the publish
 * @param[in] payload_len the length of the payload to be sent
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static int _mqtt_publish_remaining_length_serialize(uint8_t qos, const char* topic_name, int payload_len, int buf_len,
                                                    uint8_t** pptr)
{
    int len = 0;

    len += 2 + strlen(topic_name) + payload_len;
    if (qos > 0) {
        len += 2; /* packet id */
    }

    SHORT_BUFFER_CHECK(buf_len, len + MAX_MQTT_FIXED_HEADER_LEN);

    _mqtt_remaining_length_serialize(len, pptr);
    return MQTT_RET_PACKET_OK;
}

/**
 * @brief Serialize the length of the MQTT subscribe/unsubscribe packet that would be produced using the
 * supplied parameters. See 3.8.1 & 3.10.1.
 *
 * @param[in] count the number of topic filter strings in topicFilters
 * @param[in] topic_filters the array of topic filter strings to be used in the publish
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[out] pptr pointer to the output buffer - incremented by the number of bytes used & returned
 */
static int _mqtt_sub_unsub_remaining_length_serialize(uint32_t count, char* topic_filters[], int buf_len,
                                                      uint8_t** pptr, uint8_t is_sub)
{
    int i   = 0;
    int len = 2; /* packet id */

    for (i = 0; i < count; ++i) {
        len += 2 + strlen(topic_filters[i]) + is_sub; /* length + topic */
    }

    SHORT_BUFFER_CHECK(buf_len, len + MAX_MQTT_FIXED_HEADER_LEN);

    _mqtt_remaining_length_serialize(len, pptr);
    return MQTT_RET_PACKET_OK;
}

/**
 * @brief Serialize the connect options into the buffer. See 3.1.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[in] options the options to be used to build the connect packet
 * @return serialized length, or error if <= 0
 */
int mqtt_connect_packet_serialize(uint8_t* buf, int buf_len, const MQTTPacketConnectOption* options)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);
    int      rc;
    uint8_t* ptr = buf;

    // 3.1.1 Fixed header
    _mqtt_header_serialize(CONNECT, NULL, &ptr);

    rc = _mqtt_connect_remaining_length_serialize(options, buf_len, &ptr);
    if (rc) {
        return rc;
    }

    // 3.1.2 Variable header
    rc = _mqtt_connect_variable_header_serialize(options, &ptr);
    if (rc) {
        return rc;
    }

    // 3.1.3 Payload
    _write_int(options->keep_alive_interval, &ptr);
    _write_string(options->client_id, &ptr);

    if (options->will_flag) {
        _write_string(options->will.topic_name, &ptr);
        _write_string(options->will.message, &ptr);
    }

    if (options->username) {
        _write_string(options->username, &ptr);
    }

    if (options->password) {
        _write_string(options->password, &ptr);
    }

    return ptr - buf;
}

/**
 * @brief Serialize a disconnect packet into the supplied buffer, ready for writing to a socket. See 3.14.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer, to avoid overruns
 * @return serialized length, or error if <= 0
 */
int mqtt_disconnect_packet_serialize(uint8_t* buf, int buf_len)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t* ptr = buf;
    _mqtt_header_serialize(DISCONNECT, 0, &ptr);
    _mqtt_remaining_length_serialize(0, &ptr);
    return ptr - buf;
}

/**
 * @brief Serialize a disconnect packet into the supplied buffer, ready for writing to a socket. See 3.12.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer, to avoid overruns
 * @return serialized length, or error if <= 0
 */
int mqtt_pingreq_packet_serialize(uint8_t* buf, int buf_len)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    uint8_t* ptr = buf;
    _mqtt_header_serialize(PINGREQ, 0, &ptr);
    _mqtt_remaining_length_serialize(0, &ptr);
    return ptr - buf;
}

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
                                  const char* topic_name, const uint8_t* payload, int payload_len)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    int      rc;
    uint8_t* ptr = buf;

    _mqtt_header_serialize(PUBLISH, flags, &ptr);

    rc = _mqtt_publish_remaining_length_serialize(flags->qos, topic_name, payload_len, buf_len, &ptr);
    if (rc) {
        return rc;
    }

    _write_string(topic_name, &ptr);

    if (flags->qos > 0) {
        _write_int(packet_id, &ptr);
    }

    memcpy(ptr, payload, payload_len);

    ptr += payload_len;
    return ptr - buf;
}

/**
 * @brief Serialize a puback packet into the supplied buffer. See 3.4.
 *
 * @param[out] buf the buffer into which the packet will be serialized
 * @param[in] buf_len the length in bytes of the supplied buffer
 * @param[in] packet_id integer - the MQTT packet identifier
 * @return serialized length, or error if <= 0
 */
int mqtt_puback_packet_serialize(uint8_t* buf, int buf_len, uint16_t packet_id)
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN + 2);

    uint8_t* ptr = buf;

    _mqtt_header_serialize(PUBACK, 0, &ptr);
    _mqtt_remaining_length_serialize(2, &ptr);
    _write_int(packet_id, &ptr);
    return ptr - buf;
}

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
                                    char* topic_filters[], const int requested_qos[])
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    int      rc, i = 0;
    uint8_t* ptr = buf;

    _mqtt_header_serialize(SUBSCRIBE, 0, &ptr);
    rc = _mqtt_sub_unsub_remaining_length_serialize(count, topic_filters, buf_len, &ptr, 1);
    if (rc) {
        return rc;
    }

    _write_int(packet_id, &ptr);

    for (i = 0; i < count; ++i) {
        _write_string(topic_filters[i], &ptr);
        _write_char(requested_qos[i], &ptr);
    }

    return ptr - buf;
}

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
int mqtt_unsubscribe_packet_serialize(uint8_t* buf, int buf_len, uint16_t packet_id, int count, char* topic_filters[])
{
    SHORT_BUFFER_CHECK(buf_len, MIN_MQTT_FIXED_HEADER_LEN);

    int      rc, i = 0;
    uint8_t* ptr = buf;

    _mqtt_header_serialize(UNSUBSCRIBE, 0, &ptr);
    rc = _mqtt_sub_unsub_remaining_length_serialize(count, topic_filters, buf_len, &ptr, 0);
    if (rc) {
        return rc;
    }

    _write_int(packet_id, &ptr);

    for (i = 0; i < count; ++i) {
        _write_string(topic_filters[i], &ptr);
    }

    return ptr - buf;
}
