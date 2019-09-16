#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud.h"

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_publish(qcloud_mqtt_client_t *client, char *topic, mqtt_publish_opt_t *publish_opt)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(publish_opt, QCLOUD_ERR_INVAL);
    QCLOUD_STRING_PTR_SANITY_CHECK(topic, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    osal_timer_t timer;
    uint32_t len = 0;
    size_t topic_len = 0;

    topic_len = strlen(topic);
    if (topic_len > QCLOUD_MQTT_TOPIC_SIZE_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
    }

    if (publish_opt->qos == MQTT_QOS2) {
        QCLOUD_LOG_E("QoS2 isn't supported yet");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT);
    }

    if (!qcloud_mqtt_client_is_connected(client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    osal_timer_init(&timer);
    osal_timer_countdown_ms(&timer, client->command_timeout);

    osal_mutex_lock(client->tx_lock);

    if (publish_opt->qos == MQTT_QOS1) {
        publish_opt->id = mqtt_glue_packet_id_generate(client);
    }
    QCLOUD_LOG_D("publish topic seq=%d|name=%s|payload=%s", publish_opt->id, topic, (char *)publish_opt->payload);

    rc = mqtt_common_serialize_publish_packet(client->tx_buffer, sizeof(client->tx_buffer),
                                            0, publish_opt->qos, publish_opt->retained, publish_opt->id,
                                            topic, (uint8_t *) publish_opt->payload, publish_opt->payload_len, &len);
    if (QCLOUD_ERR_SUCCESS != rc) {
    	osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    rc = mqtt_glue_packet_send(client, len, &timer);
    if (rc == QCLOUD_ERR_SUCCESS && publish_opt->qos > MQTT_QOS0) {
        mqtt_glue_ack_list_record(client, QCLOUD_MQTT_ACK_TYPE_PUBACK, NULL, publish_opt->id, len);
    }

    osal_mutex_unlock(client->tx_lock);

    QCLOUD_FUNC_EXIT_RC(rc);
}

#ifdef __cplusplus
}
#endif

