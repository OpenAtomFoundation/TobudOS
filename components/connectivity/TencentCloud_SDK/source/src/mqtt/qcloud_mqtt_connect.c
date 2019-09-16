#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud.h"

__QCLOUD_STATIC__ qcloud_err_t mqtt_connack2errno(uint8_t connack_rc)
{
    switch (connack_rc) {
        case MQTT_CONNACK_CONNECTION_ACCEPTED:
            return QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED;

        case MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR:
            return QCLOUD_ERR_MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION;

        case MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR:
            return QCLOUD_ERR_MQTT_CONNACK_IDENTIFIER_REJECTED;

        case MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR:
            return QCLOUD_ERR_MQTT_CONNACK_SERVER_UNAVAILABLE;

        case MQTT_CONNACK_BAD_USERDATA_ERROR:
            return QCLOUD_ERR_MQTT_CONNACK_BAD_USERDATA;

        case MQTT_CONNACK_NOT_AUTHORIZED_ERROR:
            return QCLOUD_ERR_MQTT_CONNACK_NOT_AUTHORIZED;

        default:
            return QCLOUD_ERR_MQTT_CONNACK_UNKNOWN;
    }
}
__QCLOUD_API__ qcloud_err_t qcloud_mqtt_connect(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(connect_opt, QCLOUD_ERR_INVAL);

    osal_timer_t timer;
    uint32_t serialized_len = 0;
    uint8_t connack_rc = 0, session_present = 0;
    qcloud_err_t rc = QCLOUD_ERR_FAILURE;

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, client->command_timeout);

    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc = client->network.connect(&(client->network)), QCLOUD_ERR_SUCCESS, rc);

    osal_mutex_lock(client->tx_lock);

    // 序列化CONNECT报文
    rc = mqtt_common_serialize_connect_packet(client->tx_buffer, sizeof(client->tx_buffer), connect_opt, &serialized_len);
    if (rc != QCLOUD_ERR_SUCCESS || serialized_len == 0) {
        osal_mutex_unlock(client->tx_lock);
        goto errout;
    }

    // 发送CONNECT报文
    rc = mqtt_glue_packet_send(client, serialized_len, &timer);
    if (QCLOUD_ERR_SUCCESS != rc) {
        osal_mutex_unlock(client->tx_lock);
        goto errout;
    }

    osal_mutex_unlock(client->tx_lock);

    // 阻塞等待CONNACK的报文,
    rc = mqtt_glue_spin4ack(client, &timer, (uint8_t)MQTT_PACKET_TYPE_CONNACK);
    if (QCLOUD_ERR_SUCCESS != rc) {
        goto errout;
    }

    // 反序列化CONNACK包, 检查返回码
    rc = mqtt_common_deserialize_connack_packet(&session_present, &connack_rc, client->rx_buffer, sizeof(client->rx_buffer));
    if (QCLOUD_ERR_SUCCESS != rc) {
        goto errout;
    }

    rc = mqtt_connack2errno(connack_rc);
    if (rc != QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED) {
        goto errout;
    }

    mqtt_client_connection_state_set(client, QCLOUD_MQTT_CONNECTION_STATE_CONNECTED);

    client->keep_alive_interval         = connect_opt->keep_alive_interval;

    osal_mutex_lock(client->global_lock);
    client->is_manually_disconnected    = QCLOUD_FALSE;
    client->ping_outstanding            = 0;
    osal_timer_countdown(&client->ping_timer, client->keep_alive_interval);
    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);

errout:
    client->network.disconnect(&client->network);

    QCLOUD_FUNC_EXIT_RC(rc);
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_disconnect(qcloud_mqtt_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    osal_timer_t timer;
    uint32_t serialized_len = 0;

    if (!qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    mqtt_glue_msg_handler_list_destroy(client);

    // 1. 组disconnect包
    osal_mutex_lock(client->tx_lock);

    rc = mqtt_common_serialize_zero_payload_packet(client->tx_buffer, sizeof(client->tx_buffer), MQTT_PACKET_TYPE_DISCONNECT, &serialized_len);
    if (rc != QCLOUD_ERR_SUCCESS) {
        osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, client->command_timeout);

    // 2. 发送disconnect包
    if (serialized_len > 0) {
        rc = mqtt_glue_packet_send(client, serialized_len, &timer);
        if (rc != QCLOUD_ERR_SUCCESS) {
            osal_mutex_unlock(client->tx_lock);
            QCLOUD_FUNC_EXIT_RC(rc);
        }
    }
    osal_mutex_unlock(client->tx_lock);

    // 3. 断开底层TCP连接, 并修改相关标识位
    client->network.disconnect(&(client->network));
    mqtt_client_connection_state_set(client, QCLOUD_MQTT_CONNECTION_STATE_DISCONNECTED);
    client->is_manually_disconnected = QCLOUD_TRUE;

    QCLOUD_LOG_I("mqtt disconnect!");

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

#ifdef __cplusplus
}
#endif

