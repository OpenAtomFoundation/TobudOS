#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud.h"

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_subscribe(qcloud_mqtt_client_t *client, const char *topic_filter, mqtt_subscribe_opt_t *subscribe_opt)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(subscribe_opt, QCLOUD_ERR_INVAL);
    QCLOUD_STRING_PTR_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    osal_timer_t timer;
    size_t topic_len;
    uint32_t len = 0;
    uint16_t packet_id = 0;
    char *topic_filter_mutable = NULL;
    qcloud_mqtt_msg_handler_t *msg_handler = NULL;

    topic_len = strlen(topic_filter);
    if (topic_len > QCLOUD_MQTT_TOPIC_SIZE_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
    }

    if (subscribe_opt->qos == MQTT_QOS2) {
        QCLOUD_LOG_E("QoS2 not supported yet");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT);
    }

    if (!qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN)
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
    // 序列化SUBSCRIBE报文
    packet_id = mqtt_glue_packet_id_generate(client);
    QCLOUD_LOG_D("topic name=%s|packet id=%d|private data=%s", topic_filter_mutable, packet_id, (char *)subscribe_opt->private_data);

    rc = mqtt_common_serialize_subscribe_packet(client->tx_buffer, sizeof(client->tx_buffer),
                                            0, packet_id, 1, &topic_filter_mutable,
                                            (int *)&subscribe_opt->qos, &len);
    if (QCLOUD_ERR_SUCCESS != rc) {
    	osal_mutex_unlock(client->tx_lock);
    	mqtt_glue_string_mutable_free(topic_filter_mutable);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    // 发送SUBSCRIBE报文
    rc = mqtt_glue_packet_send(client, len, &timer);
    if (QCLOUD_ERR_SUCCESS != rc) {
    	osal_mutex_unlock(client->tx_lock);
    	mqtt_glue_string_mutable_free(topic_filter_mutable);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    msg_handler = mqtt_glue_msg_handler_create(topic_filter_mutable, subscribe_opt);
    if (!msg_handler) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    rc = mqtt_glue_ack_list_record(client, QCLOUD_MQTT_ACK_TYPE_SUBACK, msg_handler, packet_id, len);
    if (QCLOUD_ERR_SUCCESS != rc) {
        osal_mutex_unlock(client->tx_lock);
        mqtt_glue_msg_handler_destory(msg_handler);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    osal_mutex_unlock(client->tx_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

#ifdef __cplusplus
}
#endif

