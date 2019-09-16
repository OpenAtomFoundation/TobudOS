#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud.h"

__QCLOUD_STATIC__ qcloud_err_t mqtt_yield_resubscribe(qcloud_mqtt_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    char *topic = NULL;
    mqtt_subscribe_opt_t subscribe_opt;
    qcloud_list_t *curr, *next;
    qcloud_mqtt_msg_handler_t *msg_handler;

    if (!qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    if (qcloud_list_empty(&client->msg_handler_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    osal_mutex_lock(client->msg_handler_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->msg_handler_list) {
        msg_handler = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_msg_handler_t, list);

        subscribe_opt.message_handler   = msg_handler->handler;
        subscribe_opt.qos               = msg_handler->qos;
        subscribe_opt.private_data      = msg_handler->private_data;

        rc = qcloud_mqtt_subscribe(client, msg_handler->topic_filter_mutable, &subscribe_opt);
        if (rc != QCLOUD_ERR_SUCCESS) {
            osal_mutex_unlock(client->msg_handler_list_lock);
        	QCLOUD_LOG_E("resubscribe failed %d, topic: %s", rc, topic);
            QCLOUD_FUNC_EXIT_RC(rc);
        }
    }

    osal_mutex_unlock(client->msg_handler_list_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_yield_do_reconnect(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(connect_opt, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;

    QCLOUD_LOG_I("attempt to reconnect...");

    if (qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_ALREADY_CONNECTED);
    }

    rc = qcloud_mqtt_connect(client, connect_opt);

    if (!qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    rc = mqtt_yield_resubscribe(client);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_RECONNECTED);
}

/**
 * @brief 处理非手动断开连接的情况
 *
 * @param client
 * @return
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_yield_try_disconnect(qcloud_mqtt_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_err_t rc;

    if (!qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    rc = qcloud_mqtt_disconnect(client);
    // 若断开连接失败, 强制断开底层TLS层连接
    if (rc != QCLOUD_ERR_SUCCESS) {
        client->network.disconnect(&(client->network));
        mqtt_client_connection_state_set(client, QCLOUD_MQTT_CONNECTION_STATE_DISCONNECTED);
    }

    mqtt_glue_callback_involve(client, MQTT_EVENT_DISCONNECT, NULL);

    // 非手动断开连接
    client->is_manually_disconnected = QCLOUD_FALSE;
    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
}

/**
 * @brief 处理自动重连的相关逻辑
 *
 * @param client
 * @return
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_yield_try_reconnect(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_err_t rc = QCLOUD_ERR_MQTT_RECONNECTED;

    // 自动重连等待时间还未过期, 还未到重连的时候, 返回正在进行重连
    if (!osal_timer_is_expired(&client->reconnect_timer)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT);
    }

    rc = mqtt_yield_do_reconnect(client, connect_opt);
    if (rc == QCLOUD_ERR_MQTT_RECONNECTED) {
        QCLOUD_LOG_E("reconnect success");
        mqtt_glue_callback_involve(client, MQTT_EVENT_RECONNECT, NULL);

        QCLOUD_FUNC_EXIT_RC(rc);
    } else {
        QCLOUD_LOG_E("attempt reconnect failed %d", rc);
        rc = QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT;
    }

    client->reconnect_try_duration *= 2;

    if (client->reconnect_try_duration > QCLOUD_MQTT_RECONNECT_TRY_THRESHOLD) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT);
    }
    osal_timer_countdown_ms(&client->reconnect_timer, client->reconnect_try_duration);

    QCLOUD_FUNC_EXIT_RC(rc);
}

/**
 * @brief 处理与服务器维持心跳的相关逻辑
 *
 * @param client
 * @return
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_yield_keep_alive(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt)
{
#define MQTT_PING_RETRY_TIMES   2

    QCLOUD_FUNC_ENTRY;

    int try = 0;
    qcloud_err_t rc;
    osal_timer_t timer;
    uint32_t serialized_len = 0;

    if (!connect_opt->keep_alive_interval) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    if (!osal_timer_is_expired(&client->ping_timer)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    if (client->ping_outstanding >= MQTT_PING_RETRY_TIMES) {
        // reaching here means we haven't received any MQTT packet for a long time(keep_alive_interval)
        QCLOUD_LOG_E("fail to recv MQTT msg.");
        QCLOUD_FUNC_EXIT_RC(mqtt_yield_try_disconnect(client));
    }

    /* there is no ping outstanding - send one */
    osal_mutex_lock(client->tx_lock);
    rc = mqtt_common_serialize_zero_payload_packet(client->tx_buffer, sizeof(client->tx_buffer), MQTT_PACKET_TYPE_PINGREQ, &serialized_len);
    if (QCLOUD_ERR_SUCCESS != rc) {
        osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    /* send the ping packet */
    osal_timer_init(&timer);
    do {
        osal_timer_countdown_ms(&timer, client->command_timeout);
        rc = mqtt_glue_packet_send(client, serialized_len, &timer);
    } while (QCLOUD_ERR_SUCCESS != rc && (try++ < 3));

    if (QCLOUD_ERR_SUCCESS != rc) {
        osal_mutex_unlock(client->tx_lock);
        // if send a PING fails, propably the connection is not OK and we decide to disconnect and begin reconnection attempts
        QCLOUD_LOG_E("fail to send PING request.");
        rc = mqtt_yield_try_disconnect(client);
        QCLOUD_FUNC_EXIT_RC(rc);
    }
    osal_mutex_unlock(client->tx_lock);

    osal_mutex_lock(client->global_lock);
    ++client->ping_outstanding;

    /* start a timer to wait for PINGRESP from server */
    osal_timer_countdown(&client->ping_timer, QCLOUD_MIN(5, connect_opt->keep_alive_interval / 2));
    osal_mutex_unlock(client->global_lock);

    QCLOUD_LOG_D("PING request %u sent.", client->ping_outstanding);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_yield(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt, uint32_t timeout_ms)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

    qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    osal_timer_t timer;
    uint8_t packet_type;

    // 1. 检查连接是否已经手动断开
    if (!qcloud_mqtt_client_is_connected(client)) {
        if (client->is_manually_disconnected) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_MANUALLY_DISCONNECTED);
        } else if (client->auto_connect_state != QCLOUD_AUTO_CONN_STATE_ENABLED) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
        }
    }

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, timeout_ms);

    // 3. 循环读取消息以及心跳包管理
    while (!osal_timer_is_expired(&timer)) {
        if (!qcloud_mqtt_client_is_connected(client)) {
            if (client->reconnect_try_duration > QCLOUD_MQTT_RECONNECT_TRY_THRESHOLD) {
                rc = QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT;
                break;
            }

            rc = mqtt_yield_try_reconnect(client, connect_opt);
            continue;
        }

        rc = mqtt_glue_spin(client, &timer, &packet_type);

        if (rc == QCLOUD_ERR_SUCCESS) {
            /* check list of ACK pend list to remove node that is timeout */
            mqtt_glue_ack_list_scan(client);

            rc = mqtt_yield_keep_alive(client, connect_opt);
        } else if (rc == QCLOUD_ERR_SSL_READ_TIMEOUT ||
                    rc == QCLOUD_ERR_SSL_READ ||
                    rc == QCLOUD_ERR_TCP_PEER_SHUTDOWN ||
                    rc == QCLOUD_ERR_TCP_READ_FAIL) {
        	QCLOUD_LOG_E("network failed, MQTT disconnect %d", rc);
        	rc = mqtt_yield_try_disconnect(client);
        }

        if (rc == QCLOUD_ERR_MQTT_NO_CONN) {
            ++client->network_disconnect_counter;

            if (client->auto_connect_state != QCLOUD_AUTO_CONN_STATE_ENABLED) {
                break;
            }

            client->reconnect_try_duration = QCLOUD_MQTT_RECONNECT_TRY_INITIAL;
            osal_timer_countdown_ms(&client->reconnect_timer, client->reconnect_try_duration);

            // 如果超时时间到了,则会直接返回
            rc = QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT;
        } else if (rc != QCLOUD_ERR_SUCCESS) {
            break;
        }
    }

    QCLOUD_FUNC_EXIT_RC(rc);
}

#ifdef __cplusplus
}
#endif

