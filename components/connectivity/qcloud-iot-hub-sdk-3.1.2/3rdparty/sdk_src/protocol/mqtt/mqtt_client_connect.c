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
 *    Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <limits.h>

#include "qcloud_iot_common.h"
#include "mqtt_client.h"
#include "utils_hmac.h"


#define MQTT_CONNECT_FLAG_USERNAME      0x80
#define MQTT_CONNECT_FLAG_PASSWORD      0x40
#define MQTT_CONNECT_FLAG_WILL_RETAIN   0x20
#define MQTT_CONNECT_FLAG_WILL_QOS2     0x18
#define MQTT_CONNECT_FLAG_WILL_QOS1     0x08
#define MQTT_CONNECT_FLAG_WILL_QOS0     0x00
#define MQTT_CONNECT_FLAG_WILL_FLAG     0x04
#define MQTT_CONNECT_FLAG_CLEAN_SES     0x02

#define MQTT_CONNACK_FLAG_SES_PRE       0x01

/**
 * Connect return code
 */
typedef enum {
    CONNACK_CONNECTION_ACCEPTED = 0,    // connection accepted
    CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = 1,    // connection refused: unaccpeted protocol verison
    CONNACK_IDENTIFIER_REJECTED_ERROR = 2,    // connection refused: identifier rejected
    CONNACK_SERVER_UNAVAILABLE_ERROR = 3,    // connection refused: server unavailable
    CONNACK_BAD_USERDATA_ERROR = 4,    // connection refused: bad user name or password
    CONNACK_NOT_AUTHORIZED_ERROR = 5     // connection refused: not authorized
} MQTTConnackReturnCodes;

/**
  * Determines the length of the MQTT connect packet that would be produced using the supplied connect options.
  * @param options the options to be used to build the connect packet
  * @param the length of buffer needed to contain the serialized version of the packet
  * @return int indicating function execution status
  */
static uint32_t _get_packet_connect_rem_len(MQTTConnectParams *options) {
    size_t len = 0;
    /* variable depending on MQTT or MQIsdp */
    if (3 == options->mqtt_version) {
        len = 12;
    } else if (4 == options->mqtt_version) {
        len = 10;
    }

    len += strlen(options->client_id) + 2;

    if (options->username) {
        len += strlen(options->username) + 2;
    }

    if (options->password) {
        len += strlen(options->password) + 2;
    }

    return (uint32_t) len;
}

static void _copy_connect_params(MQTTConnectParams *destination, MQTTConnectParams *source) {

	POINTER_SANITY_CHECK_RTN(destination);
	POINTER_SANITY_CHECK_RTN(source);

    destination->mqtt_version = source->mqtt_version;
    destination->client_id = source->client_id;
    destination->username = source->username;
    destination->keep_alive_interval = source->keep_alive_interval;
    destination->clean_session = source->clean_session;
    destination->auto_connect_enable = source->auto_connect_enable;
#ifdef AUTH_WITH_NOTLS
    destination->device_secret = source->device_secret;
    destination->device_secret_len = source->device_secret_len;
#endif
}

/**
  * Serializes the connect options into the buffer.
  * @param buf the buffer into which the packet will be serialized
  * @param len the length in bytes of the supplied buffer
  * @param options the options to be used to build the connect packet
  * @param serialized length
  * @return int indicating function execution status
  */
static int _serialize_connect_packet(unsigned char *buf, size_t buf_len, MQTTConnectParams *options, uint32_t *serialized_len) {
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(options, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(options->client_id, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    unsigned char *ptr = buf;
    unsigned char header = 0;
    unsigned char flags = 0;
    uint32_t rem_len = 0;
    int rc;

    long cur_timesec = HAL_Timer_current_sec() + MAX_ACCESS_EXPIRE_TIMEOUT / 1000;
    if (cur_timesec <= 0 || MAX_ACCESS_EXPIRE_TIMEOUT <= 0) {
    	cur_timesec = LONG_MAX;
    }
    long cur_timesec_bak = cur_timesec;
    int cur_timesec_len = 0;
    while(cur_timesec_bak != 0) {
    	cur_timesec_bak /= 10;
		++cur_timesec_len;
	}

    int username_len = strlen(options->client_id) + strlen(QCLOUD_IOT_DEVICE_SDK_APPID) + MAX_CONN_ID_LEN + cur_timesec_len + 4;
    options->username = (char*)HAL_Malloc(username_len);
    get_next_conn_id(options->conn_id);
	HAL_Snprintf(options->username, username_len, "%s;%s;%s;%ld", options->client_id, QCLOUD_IOT_DEVICE_SDK_APPID, options->conn_id, cur_timesec);

#if defined(AUTH_WITH_NOTLS) && defined(AUTH_MODE_KEY)
     if (options->device_secret != NULL && options->username != NULL) {
    	 char                sign[41]   = {0};
    	 utils_hmac_sha1(options->username, strlen(options->username), sign, options->device_secret, options->device_secret_len);
    	 options->password = (char*) HAL_Malloc (51);
    	 if (options->password == NULL) IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
		 HAL_Snprintf(options->password, 51, "%s;hmacsha1", sign);
     }
#endif

    rem_len = _get_packet_connect_rem_len(options);
    if (get_mqtt_packet_len(rem_len) > buf_len) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    rc = mqtt_init_packet_header(&header, CONNECT, QOS0, 0, 0);
    if (QCLOUD_RET_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    // 1st byte in fixed header
    mqtt_write_char(&ptr, header);

    // remaining length
    ptr += mqtt_write_packet_rem_len(ptr, rem_len);

    // MQTT protocol name and version in variable header
    if (4 == options->mqtt_version) {
        mqtt_write_utf8_string(&ptr, "MQTT");
        mqtt_write_char(&ptr, (unsigned char) 4);
    } else {
        mqtt_write_utf8_string(&ptr, "MQIsdp");
        mqtt_write_char(&ptr, (unsigned char) 3);
    }

    // flags in variable header
    flags |= (options->clean_session) ? MQTT_CONNECT_FLAG_CLEAN_SES : 0;
    flags |= (options->username != NULL) ? MQTT_CONNECT_FLAG_USERNAME : 0;

#if defined(AUTH_WITH_NOTLS) && defined(AUTH_MODE_KEY)
	flags |= MQTT_CONNECT_FLAG_PASSWORD;
#endif

    mqtt_write_char(&ptr, flags);

    // keep alive interval (unit:ms) in variable header
    mqtt_write_uint_16(&ptr, options->keep_alive_interval);

    // client id
    mqtt_write_utf8_string(&ptr, options->client_id);

    if ((flags & MQTT_CONNECT_FLAG_USERNAME) && options->username != NULL) {
        mqtt_write_utf8_string(&ptr, options->username);
        HAL_Free(options->username);
        options->username = NULL;
    }

    if ((flags & MQTT_CONNECT_FLAG_PASSWORD) && options->password != NULL) {
    	mqtt_write_utf8_string(&ptr, options->password);
    	HAL_Free(options->password);
    	options->password = NULL;
    }

    *serialized_len = (uint32_t) (ptr - buf);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
  * Deserializes the supplied (wire) buffer into connack data - return code
  * @param sessionPresent the session present flag returned (only for MQTT 3.1.1)
  * @param connack_rc returned integer value of the connack return code
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return int indicating function execution status
  */
static int _deserialize_connack_packet(uint8_t *sessionPresent, int *connack_rc, unsigned char *buf, size_t buflen) {
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(sessionPresent, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(connack_rc, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    unsigned char header, type = 0;
    unsigned char *curdata = buf;
    unsigned char *enddata = NULL;
    int rc;
    uint32_t decodedLen = 0, readBytesLen = 0;
    unsigned char flags = 0;
    unsigned char connack_rc_char;

    // CONNACK: 2 bytes in fixed header and 2 bytes in variable header, no payload
    if (4 > buflen) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    header = mqtt_read_char(&curdata);
    type = (header&MQTT_HEADER_TYPE_MASK)>>MQTT_HEADER_TYPE_SHIFT;
    if (CONNACK != type) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    rc = mqtt_read_packet_rem_len_form_buf(curdata, &decodedLen, &readBytesLen);
    if (QCLOUD_RET_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }
    curdata += (readBytesLen);
    enddata = curdata + decodedLen;
    if (enddata - curdata != 2) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // variable header - connack flag, refer to MQTT spec 3.2.2.1
    flags = mqtt_read_char(&curdata);
    *sessionPresent = flags & MQTT_CONNACK_FLAG_SES_PRE;

    // variable header - return code, refer to MQTT spec 3.2.2.3
    connack_rc_char = mqtt_read_char(&curdata);
    switch (connack_rc_char) {
        case CONNACK_CONNECTION_ACCEPTED:
            *connack_rc = QCLOUD_RET_MQTT_CONNACK_CONNECTION_ACCEPTED;
            break;
        case CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_UNACCEPTABLE_PROTOCOL_VERSION;
            break;
        case CONNACK_IDENTIFIER_REJECTED_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_IDENTIFIER_REJECTED;
            break;
        case CONNACK_SERVER_UNAVAILABLE_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_SERVER_UNAVAILABLE;
            break;
        case CONNACK_BAD_USERDATA_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_BAD_USERDATA;
            break;
        case CONNACK_NOT_AUTHORIZED_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_NOT_AUTHORIZED;
            break;
        default:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_UNKNOWN;
            break;
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Setup connection with MQTT server
 *
 * @param pClient
 * @param options
 * @return
 */
static int _mqtt_connect(Qcloud_IoT_Client *pClient, MQTTConnectParams *options) {

    IOT_FUNC_ENTRY;

    Timer connect_timer;
    int connack_rc = QCLOUD_ERR_FAILURE, rc = QCLOUD_ERR_FAILURE;
    uint8_t sessionPresent = 0;
    uint32_t len = 0;

    InitTimer(&connect_timer);
    // if we have log in console, we need a longer timeout
    countdown_ms(&connect_timer, pClient->command_timeout_ms + 10 * 1000);

    if (NULL != options) {
        _copy_connect_params(&(pClient->options), options);
    }

    // TCP or TLS network connect
    rc = pClient->network_stack.connect(&(pClient->network_stack));
    if (QCLOUD_RET_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    HAL_MutexLock(pClient->lock_write_buf);
    // serialize CONNECT packet
    rc = _serialize_connect_packet(pClient->write_buf, pClient->write_buf_size, &(pClient->options), &len);
    if (QCLOUD_RET_SUCCESS != rc || 0 == len) {
    	HAL_MutexUnlock(pClient->lock_write_buf);
        IOT_FUNC_EXIT_RC(rc);
    }

    // send CONNECT packet
    rc = send_mqtt_packet(pClient, len, &connect_timer);
    if (QCLOUD_RET_SUCCESS != rc) {
    	HAL_MutexUnlock(pClient->lock_write_buf);
        IOT_FUNC_EXIT_RC(rc);
    }
    HAL_MutexUnlock(pClient->lock_write_buf);

    // wait for CONNACK
    rc = wait_for_read(pClient, CONNACK, &connect_timer, QOS0);
    if (QCLOUD_RET_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    // deserialize CONNACK and check reture code
    rc = _deserialize_connack_packet(&sessionPresent, &connack_rc, pClient->read_buf, pClient->read_buf_size);
    if (QCLOUD_RET_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    if (QCLOUD_RET_MQTT_CONNACK_CONNECTION_ACCEPTED != connack_rc) {
        IOT_FUNC_EXIT_RC(connack_rc);
    }

    set_client_conn_state(pClient, CONNECTED);
    HAL_MutexLock(pClient->lock_generic);
    pClient->was_manually_disconnected = 0;
    pClient->is_ping_outstanding = 0;
    countdown(&pClient->ping_timer, pClient->options.keep_alive_interval);
    HAL_MutexUnlock(pClient->lock_generic);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int qcloud_iot_mqtt_connect(Qcloud_IoT_Client *pClient, MQTTConnectParams *pParams) {

    IOT_FUNC_ENTRY;
    int rc;
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pParams, QCLOUD_ERR_INVAL);

    // check connection state first
    if (get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_MQTT_ALREADY_CONNECTED);
    }

    rc = _mqtt_connect(pClient, pParams);

    // disconnect network if connect fail
    if (rc != QCLOUD_RET_SUCCESS) {
        pClient->network_stack.disconnect(&(pClient->network_stack));
    }

    IOT_FUNC_EXIT_RC(rc);
}

int qcloud_iot_mqtt_attempt_reconnect(Qcloud_IoT_Client *pClient) {

    IOT_FUNC_ENTRY;

    int rc;
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    Log_i("attempt to reconnect...");

    if (get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_MQTT_ALREADY_CONNECTED);
    }

    rc = qcloud_iot_mqtt_connect(pClient, &pClient->options);

    if (!get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(rc);
    }

    rc = qcloud_iot_mqtt_resubscribe(pClient);
    if (rc != QCLOUD_RET_SUCCESS) {
        IOT_FUNC_EXIT_RC(rc);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_MQTT_RECONNECTED);
}

int qcloud_iot_mqtt_disconnect(Qcloud_IoT_Client *pClient) {

    IOT_FUNC_ENTRY;

    int rc;
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    Timer timer;
    uint32_t serialized_len = 0;

    if (get_client_conn_state(pClient) == 0) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    HAL_MutexLock(pClient->lock_write_buf);
    rc = serialize_packet_with_zero_payload(pClient->write_buf, pClient->write_buf_size, DISCONNECT, &serialized_len);
    if (rc != QCLOUD_RET_SUCCESS) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        IOT_FUNC_EXIT_RC(rc);
    }

    InitTimer(&timer);
    countdown_ms(&timer, pClient->command_timeout_ms);

    if (serialized_len > 0) {
        rc = send_mqtt_packet(pClient, serialized_len, &timer);
        if (rc != QCLOUD_RET_SUCCESS) {
            HAL_MutexUnlock(pClient->lock_write_buf);
            IOT_FUNC_EXIT_RC(rc);
        }
    }
    HAL_MutexUnlock(pClient->lock_write_buf);

    pClient->network_stack.disconnect(&(pClient->network_stack));
    set_client_conn_state(pClient, NOTCONNECTED);
    pClient->was_manually_disconnected = 1;

    Log_i("mqtt disconnect!");

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

#ifdef __cplusplus
}
#endif


