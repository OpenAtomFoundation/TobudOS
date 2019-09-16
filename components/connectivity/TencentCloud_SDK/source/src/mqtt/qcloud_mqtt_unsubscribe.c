#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud.h"

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_unsubscribe(qcloud_mqtt_client_t *client, const char *topic_filter)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_STRING_PTR_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    osal_timer_t timer;
    size_t topic_len = 0;
    uint32_t len = 0;
    uint16_t packet_id = 0;
    int is_subscribed = QCLOUD_FALSE;
    char *topic_filter_mutable = NULL;

    topic_len = strlen(topic_filter);
    if (topic_len > QCLOUD_MQTT_TOPIC_SIZE_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
    }

    mqtt_glue_msg_handler_uninstall(client, topic_filter, &is_subscribed);

    if (!is_subscribed) {
        QCLOUD_LOG_E("not subscribed: %s", topic_filter);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_UNSUB_FAIL);
    }

    if (!qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    /* topic filter should be valid in the whole sub life */
    topic_filter_mutable = mqtt_glue_string_const2mutable(topic_filter, topic_len);
    if (!topic_filter_mutable) {
        QCLOUD_LOG_E("malloc failed");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, client->command_timeout);

    osal_mutex_lock(client->tx_lock);

    packet_id = mqtt_glue_packet_id_generate(client);
    rc = mqtt_common_serialize_unsubscribe_packet(client->tx_buffer, sizeof(client->tx_buffer),
                                                0, packet_id, 1, &topic_filter_mutable, &len);

    mqtt_glue_string_mutable_free(topic_filter_mutable);

    if (QCLOUD_ERR_SUCCESS != rc) {
    	osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    /* send the unsubscribe packet */
    rc = mqtt_glue_packet_send(client, len, &timer);
    if (QCLOUD_ERR_SUCCESS != rc) {
    	osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    rc = mqtt_glue_ack_list_record(client, QCLOUD_MQTT_ACK_TYPE_UNSUBACK, NULL, packet_id, len);
    if (QCLOUD_ERR_SUCCESS != rc) {
        QCLOUD_LOG_E("push publish into to pubInfolist failed: %d", rc);
        osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    osal_mutex_unlock(client->tx_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

#ifdef __cplusplus
}
#endif

