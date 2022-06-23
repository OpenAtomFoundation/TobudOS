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
 * @file mqtt_client_yield.c
 * @brief
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

#include "mqtt_client.h"

/**
 * @brief Remain waiting time after MQTT data is received (unit: ms)
 *
 */
#define QCLOUD_IOT_MQTT_MAX_REMAIN_WAIT_MS (100)

/**
 * @brief Read one byte from network for mqtt packet header except remaining length.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] timeout_ms timeout to read
 * @param[out] packet_type packet type to get
 * @param[in,out] pptr pointer to buf pointer
 * @return @see IotReturnCode
 */
static int _read_packet_header(QcloudIotClient *client, uint32_t timeout_ms, uint8_t *packet_type, uint8_t **pptr)
{
    int    rc       = 0;
    size_t read_len = 0;

    // refresh timeout
    timeout_ms = timeout_ms <= 0 ? 1 : timeout_ms;

    rc = client->network_stack.read(&(client->network_stack), *pptr, 1, timeout_ms, &read_len);
    if (rc == QCLOUD_ERR_SSL_NOTHING_TO_READ || rc == QCLOUD_ERR_TCP_NOTHING_TO_READ) {
        return QCLOUD_ERR_MQTT_NOTHING_TO_READ;
    }

    // get packet type
    *packet_type = (**pptr & 0xf0) >> 4;
    (*pptr)++;
    return rc;
}

/**
 * @brief Read 1~4 bytes from network for mqtt packet header remaining length.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] timeout_ms timeout to read
 * @param[out] plen read len
 * @param[in,out] pptr pointer to buf pointer
 * @return int
 */
static int _read_packet_remaining_len(QcloudIotClient *client, uint32_t timeout_ms, uint32_t *plen, uint8_t **pptr)
{
    IOT_FUNC_ENTRY;

    uint8_t *buf        = *pptr;
    uint8_t  c          = 0;
    uint32_t multiplier = 1;
    uint32_t count      = 0;
    uint32_t len        = 0;
    size_t   rlen       = 0;

    // refresh timeout
    timeout_ms = timeout_ms <= 0 ? 1 : timeout_ms;
    timeout_ms += QCLOUD_IOT_MQTT_MAX_REMAIN_WAIT_MS;

    do {
        if (++count > MAX_NO_OF_REMAINING_LENGTH_BYTES) {
            /* bad data */
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_PACKET_READ)
        }

        if (client->network_stack.read(&(client->network_stack), buf, 1, timeout_ms, &rlen)) {
            /* The value argument is the important value. len is just used temporarily
             * and never used by the calling function for anything else */
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
        }

        c = *buf++;
        len += (c & 127) * multiplier;
        multiplier *= 128;
    } while (c & 128);

    if (plen) {
        *plen = len;
    }

    *pptr += count;
    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Discard the packet for mqtt read buf not enough.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] timeout_ms timeout to read
 * @param[in] rem_len remaining length to read
 */
static void _discard_packet_for_short_buf(QcloudIotClient *client, uint32_t timeout_ms, uint32_t rem_len)
{
    int    rc;
    size_t bytes_to_be_read, total_bytes_read = 0;
    size_t read_len;

    timeout_ms = timeout_ms <= 0 ? 1 : timeout_ms;
    timeout_ms += QCLOUD_IOT_MQTT_MAX_REMAIN_WAIT_MS;

    bytes_to_be_read = client->read_buf_size;
    do {
        rc = client->network_stack.read(&(client->network_stack), client->read_buf, bytes_to_be_read, timeout_ms,
                                        &read_len);
        if (rc) {
            break;
        }

        total_bytes_read += read_len;
        bytes_to_be_read =
            (rem_len - total_bytes_read) >= client->read_buf_size ? client->read_buf_size : rem_len - total_bytes_read;
    } while (total_bytes_read < rem_len);
}

/**
 * @brief Read rem_len bytes from network for mqtt packet payload.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] timeout_ms timeout to read
 * @param[in] rem_len length to read
 * @param[out] buf buffer to save payload
 * @return @see IotReturnCode
 */
static int _read_packet_payload(QcloudIotClient *client, uint32_t timeout_ms, uint32_t rem_len, uint8_t *buf)
{
    int    rc;
    size_t read_len = 0;

    timeout_ms = timeout_ms <= 0 ? 1 : timeout_ms;
    timeout_ms += QCLOUD_IOT_MQTT_MAX_REMAIN_WAIT_MS;

    rc = client->network_stack.read(&(client->network_stack), buf, rem_len, timeout_ms, &read_len);
    if (rc) {
        return rc;
    }

    if (read_len != rem_len) {
#ifdef AUTH_WITH_NO_TLS
        return QCLOUD_ERR_TCP_READ_TIMEOUT;
#else
        return QCLOUD_ERR_SSL_READ_TIMEOUT;
#endif
    }

    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Read MQTT packet from network stack
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in,out] timer timeout timer
 * @param[out] packet_type MQTT packet type
 * @return @see IotReturnCode
 *
 * @note
 * 1. read 1st byte in fixed header and check if valid
 * 2. read the remaining length
 * 3. read payload according to remaining length
 */
static int _read_mqtt_packet(QcloudIotClient *client, QcloudIotTimer *timer, uint8_t *packet_type)
{
    IOT_FUNC_ENTRY;
    int      rc      = 0;
    uint32_t rem_len = 0, packet_len = 0;
    uint8_t *packet_read_buf = client->read_buf;

    // 1. read 1st byte in fixed header and check if valid
    rc = _read_packet_header(client, IOT_Timer_Remain(timer), packet_type, &packet_read_buf);
    if (rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    // 2. read the remaining length
    rc = _read_packet_remaining_len(client, IOT_Timer_Remain(timer), &rem_len, &packet_read_buf);
    if (rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    // if read buffer is not enough to read the remaining length, discard the packet
    packet_len = packet_read_buf - client->read_buf + rem_len;
    if (packet_len >= client->read_buf_size) {
        _discard_packet_for_short_buf(client, IOT_Timer_Remain(timer), rem_len);
        Log_e("MQTT Recv buffer not enough: %lu < %d", client->read_buf_size, rem_len);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    // 3. read payload according to remaining length
    if (rem_len > 0) {
        rc = _read_packet_payload(client, IOT_Timer_Remain(timer), rem_len, packet_read_buf);
    }
    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Reset ping counter & ping timer.
 *
 * @param[in,out] client pointer to mqtt_client
 */
static void _handle_pingresp_packet(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;

    HAL_MutexLock(client->lock_generic);
    client->is_ping_outstanding = 0;
    IOT_Timer_Countdown(&client->ping_timer, client->options.keep_alive_interval);
    HAL_MutexUnlock(client->lock_generic);

    IOT_FUNC_EXIT;
}

/**
 * @brief Read a mqtt packet from network and handle it.
 *
 * @param[in,out] client pointer to mqtt_client
 * @param[in] timer timer for operation
 * @param[out] packet_type packet type of packet read
 * @return @see IotReturnCode
 */
static int _cycle_for_read(QcloudIotClient *client, QcloudIotTimer *timer, uint8_t *packet_type)
{
    IOT_FUNC_ENTRY;

    int rc;

    /* read the socket, see what work is due */
    rc = _read_mqtt_packet(client, timer, packet_type);
    if (QCLOUD_ERR_MQTT_NOTHING_TO_READ == rc) {
        /* Nothing to read, not a cycle failure */
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
    }

    if (rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    switch (*packet_type) {
        case CONNACK:
        case PUBREC:
        case PUBREL:
        case PUBCOMP:
        case PINGRESP:
            break;
        case PUBACK:
            rc = qcloud_iot_mqtt_handle_puback(client);
            break;
        case SUBACK:
            rc = qcloud_iot_mqtt_handle_suback(client);
            break;
        case UNSUBACK:
            rc = qcloud_iot_mqtt_handle_unsuback(client);
            break;
        case PUBLISH:
            rc = qcloud_iot_mqtt_handle_publish(client);
            break;
        default:
            // Either unknown packet type or failure occurred should not happen
            IOT_FUNC_EXIT_RC(QCLOUD_ERR_RX_MESSAGE_INVAL);
            break;
    }

    switch (*packet_type) {
        // Recv below msgs are all considered as PING OK
        case PUBACK:
        case SUBACK:
        case UNSUBACK:
        case PINGRESP:
            _handle_pingresp_packet(client);
            break;

        // Recv downlink pub means link is OK but we still need to send PING request
        case PUBLISH:
            HAL_MutexLock(client->lock_generic);
            client->is_ping_outstanding = 0;
            HAL_MutexUnlock(client->lock_generic);
            break;
    }

    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Set reconnect wait interval if auto conect is enable.
 *
 * @param[in,out] client pointer to mqtt client
 */
static void _set_reconnect_wait_interval(QcloudIotClient *client)
{
    client->counter_network_disconnected++;
    if (client->auto_connect_enable) {
        srand(IOT_Timer_CurrentSec());
        // range: 1000 - 2000 ms, in 10ms unit
        client->current_reconnect_wait_interval = (rand() % 100 + 100) * 10;
        IOT_Timer_CountdownMs(&(client->reconnect_delay_timer), client->current_reconnect_wait_interval);
    }
}

/**
 * @brief Handle disconnect if connection is error.
 *
 * @param[in,out] client pointer to mqtt client
 */
static void _handle_disconnect(QcloudIotClient *client)
{
    int          rc;
    MQTTEventMsg msg;

    if (!get_client_conn_state(client)) {
        _set_reconnect_wait_interval(client);
        return;
    }

    rc = qcloud_iot_mqtt_disconnect(client);
    // disconnect network stack by force
    if (rc) {
        client->network_stack.disconnect(&(client->network_stack));
        set_client_conn_state(client, NOTCONNECTED);
    }

    Log_e("disconnect MQTT for some reasons..");

    // notify event
    if (client->event_handle.h_fp) {
        msg.event_type = MQTT_EVENT_DISCONNECT;
        msg.msg        = NULL;
        client->event_handle.h_fp(client, client->event_handle.context, &msg);
    }

    // exceptional disconnection
    client->was_manually_disconnected = 0;
    _set_reconnect_wait_interval(client);
}

/**
 * @brief Handle reconnect.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
static int _handle_reconnect(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;

    int rc = QCLOUD_RET_MQTT_RECONNECTED;

    MQTTEventMsg msg;

    // reconnect control by delay timer (increase interval exponentially )
    if (!IOT_Timer_Expired(&(client->reconnect_delay_timer))) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT);
    }

    rc = qcloud_iot_mqtt_attempt_reconnect(client);
    if (QCLOUD_RET_MQTT_RECONNECTED == rc) {
        Log_e("attempt to reconnect success.");
        // notify event
        if (client->event_handle.h_fp) {
            msg.event_type = MQTT_EVENT_RECONNECT;
            msg.msg        = NULL;
            client->event_handle.h_fp(client, client->event_handle.context, &msg);
        }
        IOT_FUNC_EXIT_RC(rc);
    }

    Log_e("attempt to reconnect failed, errCode: %d", rc);

    client->current_reconnect_wait_interval *= 2;

    if (MAX_RECONNECT_WAIT_INTERVAL < client->current_reconnect_wait_interval) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT);
    }

    IOT_Timer_CountdownMs(&(client->reconnect_delay_timer), client->current_reconnect_wait_interval);
    IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT);
}

/**
 * @brief Handle MQTT keep alive (hearbeat with server).
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
static int _mqtt_keep_alive(QcloudIotClient *client)
{
#define MQTT_PING_RETRY_TIMES      2
#define MQTT_PING_SEND_RETRY_TIMES 3

    IOT_FUNC_ENTRY;
    int rc = 0;

    if (0 == client->options.keep_alive_interval) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
    }

    if (!IOT_Timer_Expired(&client->ping_timer)) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
    }

    if (client->is_ping_outstanding >= MQTT_PING_RETRY_TIMES) {
        // reaching here means we haven't received any MQTT packet for a long time (keep_alive_interval)
        Log_e("Fail to recv MQTT msg. Something wrong with the connection.");
        _handle_disconnect(client);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    // there is no ping outstanding - send one
    rc = qcloud_iot_mqtt_pingreq(client, MQTT_PING_SEND_RETRY_TIMES);
    if (rc) {
        // if sending a PING fails, propably the connection is not OK and we decide to disconnect and begin reconnection
        // attempts
        Log_e("Fail to send PING request. Something wrong with the connection.");
        _handle_disconnect(client);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    // start a timer to wait for PINGRESP from server
    HAL_MutexLock(client->lock_generic);
    client->is_ping_outstanding++;
    IOT_Timer_CountdownMs(&client->ping_timer, client->command_timeout_ms);
    HAL_MutexUnlock(client->lock_generic);
    Log_d("PING request %u has been sent...", client->is_ping_outstanding);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Check connection and keep alive state, read/handle MQTT message in synchronized way.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] timeout_ms timeout value (unit: ms) for this operation
 *
 * @return QCLOUD_RET_SUCCESS when success, QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT when try reconnecting, others @see
 * IotReturnCode
 */
int qcloud_iot_mqtt_yield(QcloudIotClient *client, uint32_t timeout_ms)
{
    IOT_FUNC_ENTRY;

    int            rc = QCLOUD_RET_SUCCESS;
    uint8_t        packet_type;
    QcloudIotTimer timer;

    // 1. check if manually disconnect
    if (!get_client_conn_state(client) && client->was_manually_disconnected == 1) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_MQTT_MANUALLY_DISCONNECTED);
    }

    // 2. check connection state and if auto reconnect is enabled
    if (!get_client_conn_state(client) && client->auto_connect_enable == 0) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    // 3. main loop for packet reading/handling and keep alive maintainance
    IOT_Timer_CountdownMs(&timer, timeout_ms);

    while (!IOT_Timer_Expired(&timer)) {
        // handle reconnect
        if (!get_client_conn_state(client)) {
            if (client->current_reconnect_wait_interval > MAX_RECONNECT_WAIT_INTERVAL) {
                rc = QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT;
                break;
            }
            rc = _handle_reconnect(client);
            continue;
        }

        // read and handle packet
        rc = _cycle_for_read(client, &timer, &packet_type);
        switch (rc) {
            case QCLOUD_RET_SUCCESS:
                // check list of wait publish ACK to remove node that is ACKED or timeout
                qcloud_iot_mqtt_check_pub_timeout(client);
                // check list of wait subscribe(or unsubscribe) ACK to remove node that is ACKED or timeout
                qcloud_iot_mqtt_check_sub_timeout(client);

                rc = _mqtt_keep_alive(client);
                if (rc) {
                    IOT_FUNC_EXIT_RC(client->auto_connect_enable ? QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT : rc);
                }
                break;
            case QCLOUD_ERR_SSL_READ_TIMEOUT:
            case QCLOUD_ERR_SSL_READ:
            case QCLOUD_ERR_TCP_PEER_SHUTDOWN:
            case QCLOUD_ERR_TCP_READ_TIMEOUT:
            case QCLOUD_ERR_TCP_READ_FAIL:
                Log_e("network read failed, rc: %d. MQTT Disconnect.", rc);
                _handle_disconnect(client);
                IOT_FUNC_EXIT_RC(client->auto_connect_enable ? QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT
                                                             : QCLOUD_ERR_MQTT_NO_CONN);
                break;
            default:  // others, just return
                IOT_FUNC_EXIT_RC(rc);
        }
    }
    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief Wait read specific mqtt packet, such as connack.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] packet_type packet type except to read
 * @return @see IotReturnCode
 */
int qcloud_iot_mqtt_wait_for_read(QcloudIotClient *client, uint8_t packet_type)
{
    IOT_FUNC_ENTRY;

    int            rc;
    uint8_t        read_packet_type = 0;
    QcloudIotTimer timer;
    IOT_Timer_CountdownMs(&timer, client->command_timeout_ms);

    do {
        if (IOT_Timer_Expired(&timer)) {
            rc = QCLOUD_ERR_MQTT_REQUEST_TIMEOUT;
            break;
        }
        rc = _cycle_for_read(client, &timer, &read_packet_type);
    } while (QCLOUD_RET_SUCCESS == rc && read_packet_type != packet_type);

    IOT_FUNC_EXIT_RC(rc);
}
