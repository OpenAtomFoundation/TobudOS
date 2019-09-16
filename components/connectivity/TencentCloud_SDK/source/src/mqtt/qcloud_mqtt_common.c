/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - non-blocking packet read functions for stream transport
 *******************************************************************************/

#include "qcloud.h"

__QCLOUD_STATIC__ int mqtt_common_packet_length_get(int rem_len)
{
    rem_len += 1; /* header byte */

    /* now remaining_length field */
    if (rem_len < 128) {
        rem_len += 1;
    } else if (rem_len < 16384) {
        rem_len += 2;
    } else if (rem_len < 2097151) {
        rem_len += 3;
    } else {
        rem_len += 4;
    }

    return rem_len;
}

/**
  * Determines the length of the MQTT connect packet that would be produced using the supplied connect options.
  * @param options the options to be used to build the connect packet
  * @param the length of buffer needed to contain the serialized version of the packet
  * @return int indicating function execution status
  */
__QCLOUD_STATIC__ int mqtt_common_serialize_connect_packet_length(mqtt_connect_opt_t *connect_opt)
{
    int len = 0;

    /* variable depending on MQTT or MQIsdp */
    if (connect_opt->mqtt_version == 3) {
        len = 12;
    } else if (connect_opt->mqtt_version == 4) {
        len = 10;
    }

    len += strlen(connect_opt->client_id) + 2;

    if (connect_opt->username_len) {
        len += connect_opt->username_len + 2;
    }

    if (connect_opt->password_len) {
        len += connect_opt->password_len + 2;
    }

    return len;
}

/**
  * Determines the length of the MQTT publish packet that would be produced using the supplied parameters
  * @param qos the MQTT QoS of the publish (packetid is omitted for QoS 0)
  * @param topicName the topic name to be used in the publish
  * @param payload_len the length of the payload to be sent
  * @return the length of buffer needed to contain the serialized version of the packet
  */
__QCLOUD_STATIC__ int mqtt_common_serialize_publish_packet_length(int qos, char *topic, size_t payload_len)
{
    int len = 0;

    len += 2 + strlen(topic) + payload_len;
    if (qos > MQTT_QOS0) {
        len += 2; /* packet id */
    }
    return len;
}

/**
  * Determines the length of the MQTT subscribe packet that would be produced using the supplied parameters
  * @param count the number of topic filter strings in topicFilters
  * @param topicFilters the array of topic filter strings to be used in the publish
  * @return the length of buffer needed to contain the serialized version of the packet
  */
__QCLOUD_STATIC__ int mqtt_common_serialize_subscribe_packet_length(uint32_t count, char *topic_filters[])
{
    int i;
    int len = 2; /* packet id */

    for (i = 0; i < count; ++i) {
        len += 2 + strlen(topic_filters[i]) + 1; /* length + topic + req_qos */
    }

    return len;
}

/**
  * Determines the length of the MQTT unsubscribe packet that would be produced using the supplied parameters
  * @param count the number of topic filter strings in topicFilters
  * @param topicFilters the array of topic filter strings to be used in the publish
  * @return the length of buffer needed to contain the serialized version of the packet
  */
__QCLOUD_STATIC__ int mqtt_common_serialize_unsubscribe_packet_length(uint32_t count, char *topic_filters[])
{
    int i = 0;
    int len = 2; /* packet id */

    for (i = 0; i < count; ++i) {
        len += 2 + strlen(topic_filters[i]); /* length + topic*/
    }

    return len;
}

/**
 * Decodes the message length according to the MQTT algorithm
 * @param getcharfn pointer to function to read the next character from the data source
 * @param value the decoded length returned
 * @return the number of bytes read from the socket
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_common_packet_do_decode_from_buf(uint32_t (*getcharfn)(uint8_t *, uint32_t), uint32_t *value, uint32_t *read_bytes_len)
{
    QCLOUD_FUNC_ENTRY;

    uint8_t c;
    uint32_t multiplier = 1;
    uint32_t len = 0, get_len;

    *value = 0;

    do {
#define MAX_NO_OF_REMAINING_LENGTH_BYTES    4

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES) {
            /* bad data */
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_PACKET_READ);
        }

        get_len = 0;
        get_len = (*getcharfn)(&c, 1);
        if (1 != get_len) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
        }
        *value += (c & 127) * multiplier;
        multiplier *= 128;
    } while ((c & 128) != 0);

    *read_bytes_len = len;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

static uint8_t *bufptr;
__QCLOUD_STATIC__ uint32_t __bufchar(uint8_t *c, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; ++i) {
        *c = *bufptr++;
    }

    return count;
}

/**
 * Encodes the message length according to the MQTT algorithm
 * @param buf the buffer into which the encoded data is written
 * @param length the length to be encoded
 * @return the number of bytes written to buffer
 */
__QCLOUD_INTERNAL__ int mqtt_common_packet_encode(uint8_t *buf, int length)
{
    QCLOUD_FUNC_ENTRY;

    int rc = 0;
    uint8_t encode_byte;

    do {
        encode_byte = (uint8_t)(length % 128);
        length /= 128;
        /* if there are more digits to encode, set the top bit of this digit */
        if (length > 0) {
            encode_byte |= 0x80;
        }
        buf[rc++] = encode_byte;
    } while (length > 0);

    return rc;
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_common_packet_decode_from_buf(uint8_t *buf, uint32_t *value, uint32_t *read_bytes_len)
{
    bufptr = buf;
    return mqtt_common_packet_do_decode_from_buf(__bufchar, value, read_bytes_len);
}

/**
 * Calculates uint16 packet id from two bytes read from the input buffer
 * @param pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the value calculated
 */
__QCLOUD_STATIC__ uint16_t mqtt_common_packet_read_dbyte(uint8_t **pptr)
{
    uint8_t *ptr = *pptr;
    uint8_t firstByte = (uint8_t) (*ptr);
    uint8_t secondByte = (uint8_t) (*(ptr + 1));
    uint16_t len = (uint16_t) (secondByte + (256 * firstByte));
    *pptr += 2;
    return len;
}

/**
 * Reads one character from the input buffer.
 * @param pptr pointer to the input buffer - incremented by the number of bytes used & returned
 * @return the character read
 */
__QCLOUD_STATIC__ uint8_t mqtt_common_packet_read_byte(uint8_t **pptr)
{
    uint8_t c = **pptr;
    (*pptr)++;
    return c;
}

/**
 * @param mqttstring the MQTTString structure into which the data is to be read
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param enddata pointer to the end of the data: do not read beyond
 * @return SUCCESS if successful, FAILURE if not
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_common_packet_read_string(char **string, uint16_t *string_len, uint8_t **pptr, uint8_t *enddata)
{
    /* the first two bytes are the length of the string */
    if (enddata - (*pptr) <= 1) { /* enough length to read the integer? */
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    *string_len = mqtt_common_packet_read_dbyte(pptr); /* increments pptr to point past length */

    if (*string_len > QCLOUD_MQTT_CLIENT_RX_BUF_LEN){
        QCLOUD_LOG_E("string length overflow!");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    if (&(*pptr)[*string_len] > enddata) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    *string = (char *)*pptr;
    *pptr += *string_len;
    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * Writes one character to an output buffer.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param c the character to write
 */
__QCLOUD_STATIC__ void mqtt_common_packet_write_byte(uint8_t **pptr, uint8_t c)
{
    **pptr = c;
    (*pptr)++;
}

/**
 * Writes an integer as 2 bytes to an output buffer.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param anInt the integer to write
 */
__QCLOUD_STATIC__ void mqtt_common_packet_write_dbyte(uint8_t **pptr, uint16_t dbyte)
{
    **pptr = (uint8_t)(dbyte / 256);
    (*pptr)++;
    **pptr = (uint8_t)(dbyte % 256);
    (*pptr)++;
}

/**
 * Writes a "UTF" string to an output buffer.  Converts C string to length-delimited.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param string the C string to write
 */
__QCLOUD_STATIC__ void mqtt_common_packet_write_string(uint8_t **pptr, const char *string)
{
    size_t len = strlen(string);
    mqtt_common_packet_write_dbyte(pptr, (uint16_t)len);
    memcpy(*pptr, string, len);
    *pptr += len;
}

/**
  * Serializes the ack packet into the supplied buffer.
  * @param buf the buffer into which the packet will be serialized
  * @param buf_len the length in bytes of the supplied buffer
  * @param packet_type the MQTT packet type: 1.PUBACK; 2.PUBREL; 3.PUBCOMP
  * @param dup the MQTT dup flag
  * @param packet_id the MQTT packet identifier
  * @return serialized length, or error if 0
  */
__QCLOUD_STATIC__ qcloud_err_t mqtt_common_serialize_ack_packet(uint8_t *buf,
                                                                                        size_t buf_len,
                                                                                        mqtt_packet_t packet_type,
                                                                                        uint8_t dup,
                                                                                        uint16_t packet_id,
                                                                                        uint32_t *serialized_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    mqtt_header_t header = {0};
    uint8_t *ptr = buf;

    /* Minimum byte length required by ACK headers is
     * 2 for fixed and 2 for variable part */
    if (buf_len < 4) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header.bits.type = packet_type;
    header.bits.dup = dup;
    header.bits.qos = (packet_type == MQTT_PACKET_TYPE_PUBREL) ? 1 : 0;

    mqtt_common_packet_write_byte(&ptr, header.byte); /* write header */

    ptr += mqtt_common_packet_encode(ptr, 2); /* write remaining length */
    mqtt_common_packet_write_dbyte(&ptr, packet_id);

    *serialized_len = ptr - buf;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Deserializes the supplied (wire) buffer into an ack
  * @param packet_type returned integer - the MQTT packet type
  * @param dup returned integer - the MQTT dup flag
  * @param packet_id returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buf_len the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_ack_packet(uint8_t *packet_type,
                                                                                                uint8_t *dup,
                                                                                                uint16_t *packet_id,
                                                                                                uint8_t *buf,
                                                                                                size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(packet_type, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(dup, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(packet_id, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    mqtt_header_t header = {0};
    uint8_t ack_code;
    uint8_t *curdata = buf, *enddata = NULL;
    uint32_t decoded_len = 0, read_bytes_len = 0;

    /* PUBACK fixed header size is two bytes, variable header is 2 bytes, MQTT v3.1.1 Specification 3.4.1 */
    if (buf_len < 4) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header.byte = mqtt_common_packet_read_byte(&curdata);
    *dup = header.bits.dup;
    *packet_type = header.bits.type;

    /* read remaining length */
    rc = mqtt_common_packet_decode_from_buf(curdata, &decoded_len, &read_bytes_len);
    if (QCLOUD_ERR_SUCCESS != rc) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    curdata += read_bytes_len;
    enddata = curdata + decoded_len;

    if (enddata - curdata < 2) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    *packet_id = mqtt_common_packet_read_dbyte(&curdata);

    // 返回错误码处理
    if (enddata - curdata >= 1) {
        ack_code = mqtt_common_packet_read_byte(&curdata);
        if (ack_code != 0) {
            QCLOUD_LOG_E("deserialize ack packet failure! 0x%02x", ack_code);
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
        }
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Deserializes the supplied (wire) buffer into suback data
  * @param packet_id returned integer - the MQTT packet identifier
  * @param max_count - the maximum number of members allowed in the grantedQoSs array
  * @param count returned integer - number of members in the grantedQoSs array
  * @param grantedQoSs returned array of integers - the granted qualities of service
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buf_len the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success, 0 is failure
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_suback_packet(uint16_t *packet_id,
                                                                                                    uint32_t max_count,
                                                                                                    uint32_t *count,
                                                                                                    int granted_qoss[],
                                                                                                    uint8_t *buf,
                                                                                                    size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(packet_id, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(count, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(granted_qoss, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    mqtt_header_t header = {0};
    uint8_t *curdata = buf, *enddata = NULL;
    uint32_t decoded_len = 0, read_bytes_len = 0;

    // SUBACK头部大小为4字节, 负载部分至少为1字节QOS返回码
    if (buf_len < 5) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    // 读取报文固定头部的第一个字节
    header.byte = mqtt_common_packet_read_byte(&curdata);
    if (header.bits.type != MQTT_PACKET_TYPE_SUBACK) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // 读取报文固定头部的剩余长度
    rc = mqtt_common_packet_decode_from_buf(curdata, &decoded_len, &read_bytes_len);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    curdata += read_bytes_len;
    enddata = curdata + decoded_len;
    if (enddata - curdata < 2) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // 读取报文可变头部的报文标识符
    *packet_id = mqtt_common_packet_read_dbyte(&curdata);

    // 读取报文的负载部分
    *count = 0;
    while (curdata < enddata) {
        if (*count > max_count) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
        }
        granted_qoss[(*count)++] = mqtt_common_packet_read_byte(&curdata);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Deserializes the supplied (wire) buffer into unsuback data
  * @param packet_id returned integer - the MQTT packet identifier
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buf_len the length in bytes of the data in the supplied buffer
  * @return int indicating function execution status
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_unsuback_packet(uint16_t *packet_id,
                                                                                                        uint8_t *buf,
                                                                                                        size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(packet_id, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    uint8_t type = 0, dup = 0;

    rc = mqtt_common_deserialize_ack_packet(&type, &dup, packet_id, buf, buf_len);
    if (rc != QCLOUD_ERR_SUCCESS || type != MQTT_PACKET_TYPE_UNSUBACK) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Serializes the connect options into the buffer.
  * @param buf the buffer into which the packet will be serialized
  * @param len the length in bytes of the supplied buffer
  * @param options the options to be used to build the connect packet
  * @param serialized length
  * @return int indicating function execution status
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_connect_packet(uint8_t *buf,
                                                                                                    size_t buf_len,
                                                                                                    mqtt_connect_opt_t *connect_opt,
                                                                                                    uint32_t *serialized_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(connect_opt, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    int packet_len = 0;

    uint8_t *ptr = buf;
    mqtt_header_t header = {0};
    mqtt_connect_flag_t flags = {0};

    packet_len = mqtt_common_serialize_connect_packet_length(connect_opt);
    if (mqtt_common_packet_length_get(packet_len) > buf_len) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header.byte = 0;
    header.bits.type = MQTT_PACKET_TYPE_CONNECT;

    // 报文固定头部第一个字节
    mqtt_common_packet_write_byte(&ptr, header.byte);  /* write header */

    // 报文固定头部剩余长度字段
    ptr += mqtt_common_packet_encode(ptr, packet_len);  /* write remaining length */

    // 报文可变头部协议名 + 协议版本号
    if (connect_opt->mqtt_version == 4) {
        mqtt_common_packet_write_string(&ptr, "MQTT");
        mqtt_common_packet_write_byte(&ptr, (uint8_t)4);
    } else {
        mqtt_common_packet_write_string(&ptr, "MQIsdp");
        mqtt_common_packet_write_byte(&ptr, (uint8_t)3);
    }

    // 报文可变头部连接标识位
    flags.all = 0;
    flags.bits.cleansession = connect_opt->clean_session;

    if (connect_opt->username_len) {
        flags.bits.username = 1;
    }

    if (connect_opt->password_len) {
        flags.bits.password = 1;
    }

    mqtt_common_packet_write_byte(&ptr, flags.all);

    // 报文可变头部心跳周期/保持连接, 一个以秒为单位的时间间隔, 表示为一个16位的字
    mqtt_common_packet_write_dbyte(&ptr, connect_opt->keep_alive_interval);

    // 有效负载部分: 客户端标识符
    mqtt_common_packet_write_string(&ptr, connect_opt->client_id);

    // 用户名
    if (flags.bits.username) {
        mqtt_common_packet_write_string(&ptr, connect_opt->username);
    }

    if (flags.bits.password) {
    	mqtt_common_packet_write_string(&ptr, connect_opt->password);
    }

    *serialized_len = ptr - buf;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Deserializes the supplied (wire) buffer into connack data - return code
  * @param sessionPresent the session present flag returned (only for MQTT 3.1.1)
  * @param connack_rc returned integer value of the connack return code
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return int indicating function execution status
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_connack_packet(uint8_t *session_present,
                                                                                                    uint8_t *connack_rc,
                                                                                                    uint8_t *buf,
                                                                                                    size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(session_present, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(connack_rc, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    mqtt_header_t header = {0};
    mqtt_connack_flags_t flags = {0};
    uint8_t *curdata = buf, *enddata = NULL;
    uint32_t decoded_len = 0, read_bytes_len = 0;

    // CONNACK 头部大小是固定的2字节长度, 可变头部也是两个字节的长度, 无有效负载
    if (buf_len < 4) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    // 读取固定头部第一个字节
    header.byte = mqtt_common_packet_read_byte(&curdata);
    if (header.bits.type != MQTT_PACKET_TYPE_CONNACK) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // 读取固定头部剩余长度字段
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc = mqtt_common_packet_decode_from_buf(curdata, &decoded_len, &read_bytes_len), QCLOUD_ERR_SUCCESS, rc);

    curdata += read_bytes_len;
    enddata = curdata + decoded_len;
    if (enddata - curdata != 2) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // 读取可变头部-连接确认标志 参考MQTT协议说明文档3.2.2.1小结
    flags.all = mqtt_common_packet_read_byte(&curdata);
    *session_present = flags.bits.sessionpresent;

    // 读取可变头部-连接返回码 参考MQTT协议说明文档3.2.2.3小结
    *connack_rc = mqtt_common_packet_read_byte(&curdata);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Serializes a 0-length packet into the supplied buffer, ready for writing to a socket
  * @param buf the buffer into which the packet will be serialized
  * @param buf_len the length in bytes of the supplied buffer, to avoid overruns
  * @param packettype the message type
  * @param serialized length
  * @return int indicating function execution status
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_zero_payload_packet(uint8_t *buf,
                                                                                                            size_t buf_len,
                                                                                                            mqtt_packet_t packet_type,
                                                                                                            uint32_t *serialized_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    uint8_t *ptr = buf;
    mqtt_header_t header = {0};

    /* Buffer should have at least 2 bytes for the header */
    if (buf_len < 2) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header.byte         = 0;
    header.bits.type    = packet_type;

    /* write header */
    mqtt_common_packet_write_byte(&ptr, header.byte);

    /* write remaining length */
    ptr += mqtt_common_packet_encode(ptr, 0);
    *serialized_len = ptr - buf;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Deserializes the supplied (wire) buffer into publish data
  * @param dup returned integer - the MQTT dup flag
  * @param qos returned integer - the MQTT QoS value
  * @param retained returned integer - the MQTT retained flag
  * @param packet_id returned integer - the MQTT packet identifier
  * @param topicName returned MQTTString - the MQTT topic in the publish
  * @param payload returned byte buffer - the MQTT publish payload
  * @param payload_len returned integer - the length of the MQTT payload
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buf_len the length in bytes of the data in the supplied buffer
  * @return error code.  1 is success
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_deserialize_publish_packet(uint8_t *dup, int *qos,
                                                                                                    uint8_t *retained, uint16_t *packet_id,
                                                                                                    char **topic, uint16_t *topic_len,
                                                                                                    uint8_t **payload, size_t *payload_len,
                                                                                                    uint8_t *buf, size_t buf_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(dup, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(qos, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(retained, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(packet_id, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    uint32_t decoded_len = 0, read_bytes_len = 0;
    mqtt_header_t header = {0};
    uint8_t *curdata = buf, *enddata = NULL;

    /* Publish header size is at least four bytes.
     * Fixed header is two bytes.
     * Variable header size depends on QoS And Topic Name.
     * QoS level 0 doesn't have a message identifier (0 - 2 bytes)
     * Topic Name length fields decide size of topic name field (at least 2 bytes)
     * MQTT v3.1.1 Specification 3.3.1 */
    if (buf_len < 4) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header.byte = mqtt_common_packet_read_byte(&curdata);
    if (header.bits.type != MQTT_PACKET_TYPE_PUBLISH) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    *dup        = header.bits.dup;
    *qos        = header.bits.qos;
    *retained   = header.bits.retain;

    /* read remaining length */
    rc = mqtt_common_packet_decode_from_buf(curdata, &decoded_len, &read_bytes_len); /* read remaining length */
    if (QCLOUD_ERR_SUCCESS != rc) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    curdata += read_bytes_len;
    enddata = curdata + decoded_len;

    /* do we have enough data to read the protocol version byte? */
    if (mqtt_common_packet_read_string(topic, topic_len, &curdata, enddata) != QCLOUD_ERR_SUCCESS ||
        enddata - curdata < 0) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    if (*qos > MQTT_QOS0) {
        *packet_id = mqtt_common_packet_read_dbyte(&curdata);
    }

    *payload_len = enddata - curdata;
    *payload = curdata;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Serializes a puback packet into the supplied buffer.
  * @param buf the buffer into which the packet will be serialized
  * @param buflen the length in bytes of the supplied buffer
  * @param packetid integer - the MQTT packet identifier
  * @return serialized length, or error if 0
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_puback_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint16_t packet_id,
                                                                                                uint32_t *serialized_len)
{
    return mqtt_common_serialize_ack_packet(buf, buf_len, MQTT_PACKET_TYPE_PUBACK, 0, packet_id, serialized_len);
}

/**
  * Serializes a pubrel packet into the supplied buffer.
  * @param buf the buffer into which the packet will be serialized
  * @param buflen the length in bytes of the supplied buffer
  * @param dup integer - the MQTT dup flag
  * @param packetid integer - the MQTT packet identifier
  * @return serialized length, or error if 0
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_pubrel_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint8_t dup,
                                                                                                uint16_t packet_id,
                                                                                                uint32_t *serialized_len)
{
    return mqtt_common_serialize_ack_packet(buf, buf_len, MQTT_PACKET_TYPE_PUBREL, dup, packet_id, serialized_len);
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_pubrec_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint16_t packet_id,
                                                                                                uint32_t *serialized_len)
{
    return mqtt_common_serialize_ack_packet(buf, buf_len, MQTT_PACKET_TYPE_PUBREC, 0, packet_id, serialized_len);
}


/**
  * Serializes the supplied publish data into the supplied buffer, ready for sending
  * @param buf the buffer into which the packet will be serialized
  * @param buf_len the length in bytes of the supplied buffer
  * @param dup integer - the MQTT dup flag
  * @param qos integer - the MQTT QoS value
  * @param retained integer - the MQTT retained flag
  * @param packet_id integer - the MQTT packet identifier
  * @param topicName MQTTString - the MQTT topic in the publish
  * @param payload byte buffer - the MQTT publish payload
  * @param payload_len integer - the length of the MQTT payload
  * @return the length of the serialized data.  <= 0 indicates error
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_publish_packet(uint8_t *buf,
                                                                                                size_t buf_len,
                                                                                                uint8_t dup,
                                                                                                int qos,
                                                                                                uint8_t retained,
                                                                                                uint16_t packet_id,
                                                                                                char *topic,
                                                                                                uint8_t *payload,
                                                                                                size_t payload_len,
                                                                                                uint32_t *serialized_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(payload, QCLOUD_ERR_INVAL);

    uint8_t *ptr = buf;
    mqtt_header_t header = {0};
    int rem_len = 0;

    rem_len = mqtt_common_serialize_publish_packet_length(qos, topic, payload_len);
    if (mqtt_common_packet_length_get(rem_len) > buf_len) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header.bits.type    = MQTT_PACKET_TYPE_PUBLISH;
    header.bits.dup     = dup;
    header.bits.qos     = qos;
    header.bits.retain  = retained;

    mqtt_common_packet_write_byte(&ptr, header.byte); /* write header */

    ptr += mqtt_common_packet_encode(ptr, rem_len); /* write remaining length */;

    mqtt_common_packet_write_string(&ptr, topic);   /* Variable Header: Topic Name */

    if (qos > MQTT_QOS0) {
        mqtt_common_packet_write_dbyte(&ptr, packet_id);  /* Variable Header: Topic Name */
    }

    memcpy(ptr, payload, payload_len);
    ptr += payload_len;

    *serialized_len = ptr - buf;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Serializes the supplied subscribe data into the supplied buffer, ready for sending
  * @param buf the buffer into which the packet will be serialized
  * @param buf_len the length in bytes of the supplied bufferr
  * @param dup integer - the MQTT dup flag
  * @param packet_id integer - the MQTT packet identifier
  * @param count - number of members in the topicFilters and reqQos arrays
  * @param topicFilters - array of topic filter names
  * @param requestedQoSs - array of requested QoS
  * @return the length of the serialized data.  <= 0 indicates error
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_subscribe_packet(uint8_t *buf,
                                                                                                    size_t buf_len,
                                                                                                    uint8_t dup,
                                                                                                    uint16_t packet_id,
                                                                                                    uint32_t count,
                                                                                                    char *topic_filters[],
                                                                                                    int requested_qoss[],
                                                                                                    uint32_t *serialized_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    int i = 0;
    uint8_t *ptr = buf;
    mqtt_header_t header = {0};
    uint32_t rem_len = 0;

    // SUBSCRIBE报文的剩余长度 = 报文标识符(2 byte) + count * (长度字段(2 byte) + topicLen + qos(1 byte))
    rem_len = mqtt_common_serialize_subscribe_packet_length(count, topic_filters);
    if (mqtt_common_packet_length_get(rem_len) > buf_len) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    // 初始化报文头部
    header.byte         = 0;
    header.bits.type    = MQTT_PACKET_TYPE_SUBSCRIBE;
    header.bits.dup     = dup;
    header.bits.qos     = MQTT_QOS1;
    // 写报文固定头部第一个字节
    mqtt_common_packet_write_byte(&ptr, header.byte);

    // 写报文固定头部剩余长度字段
    ptr += mqtt_common_packet_encode(ptr, rem_len);

    // 写可变头部: 报文标识符
    mqtt_common_packet_write_dbyte(&ptr, packet_id);

    // 写报文的负载部分数据
    for (i = 0; i < count; ++i) {
        mqtt_common_packet_write_string(&ptr, topic_filters[i]);
        mqtt_common_packet_write_byte(&ptr, (uint8_t)requested_qoss[i]);
    }

    *serialized_len = ptr - buf;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Serializes the supplied unsubscribe data into the supplied buffer, ready for sending
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buf_len the length in bytes of the data in the supplied buffer
  * @param dup integer - the MQTT dup flag
  * @param packet_id integer - the MQTT packet identifier
  * @param count - number of members in the topicFilters array
  * @param topicFilters - array of topic filter names
  * @param serialized_len - the length of the serialized data
  * @return int indicating function execution status
  */
__QCLOUD_INTERNAL__ qcloud_err_t mqtt_common_serialize_unsubscribe_packet(uint8_t *buf, size_t buf_len,
                                                                                                        uint8_t dup, uint16_t packet_id,
                                                                                                        uint32_t count, char *topic_filters[],
                                                                                                        uint32_t *serialized_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    int i = 0;
    uint8_t *ptr = buf;
    mqtt_header_t header = {0};
    uint32_t rem_len = 0;

    rem_len = mqtt_common_serialize_unsubscribe_packet_length(count, topic_filters);
    if (mqtt_common_packet_length_get(rem_len) > buf_len) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header.byte         = 0;
    header.bits.type    = MQTT_PACKET_TYPE_UNSUBSCRIBE;
    header.bits.dup     = dup;
    header.bits.qos     = MQTT_QOS1;
    mqtt_common_packet_write_byte(&ptr, header.byte); /* write header */

    ptr += mqtt_common_packet_encode(ptr, rem_len); /* write remaining length */

    mqtt_common_packet_write_dbyte(&ptr, packet_id);

    for (i = 0; i < count; ++i) {
        mqtt_common_packet_write_string(&ptr, topic_filters[i]);
    }

    *serialized_len = ptr - buf;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

