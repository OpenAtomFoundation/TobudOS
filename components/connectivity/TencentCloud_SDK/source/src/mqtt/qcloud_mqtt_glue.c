#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud.h"

__QCLOUD_STATIC__ mqtt_event_type_t mqtt_glue_event_type_get(qcloud_mqtt_ack_type_t ack_type, int is_nack, int is_timer_expired)
{
    mqtt_event_type_t event_type;

    if (is_nack) {
        switch (ack_type) {
            case QCLOUD_MQTT_ACK_TYPE_PUBACK:
                event_type = MQTT_EVENT_PUBLISH_NACK;
                break;

            case QCLOUD_MQTT_ACK_TYPE_SUBACK:
                event_type = MQTT_EVENT_SUBCRIBE_NACK;
                break;

            case QCLOUD_MQTT_ACK_TYPE_UNSUBACK:
                event_type = MQTT_EVENT_UNSUBCRIBE_NACK;
                break;
        }
    } else if (is_timer_expired) {
        switch (ack_type) {
            case QCLOUD_MQTT_ACK_TYPE_PUBACK:
                event_type = MQTT_EVENT_PUBLISH_TIMEOUT;
                break;

            case QCLOUD_MQTT_ACK_TYPE_SUBACK:
                event_type = MQTT_EVENT_SUBCRIBE_TIMEOUT;
                break;

            case QCLOUD_MQTT_ACK_TYPE_UNSUBACK:
                event_type = MQTT_EVENT_UNSUBCRIBE_TIMEOUT;
                break;
        }
    } else {
        switch (ack_type) {
        case QCLOUD_MQTT_ACK_TYPE_PUBACK:
            event_type = MQTT_EVENT_PUBLISH_SUCCESS;
            break;

        case QCLOUD_MQTT_ACK_TYPE_SUBACK:
            event_type = MQTT_EVENT_SUBCRIBE_SUCCESS;
            break;

        case QCLOUD_MQTT_ACK_TYPE_UNSUBACK:
            event_type = MQTT_EVENT_UNSUBCRIBE_SUCCESS;
            break;
        }
    }

    return event_type;
}

__QCLOUD_INTERNAL__ char *mqtt_glue_string_const2mutable(const char *orig_str, size_t orig_str_len)
{
    char *str_mutable = NULL;

    str_mutable = osal_malloc(orig_str_len + 1);
    if (!str_mutable) {
        return NULL;
    }

    strncpy(str_mutable, orig_str, orig_str_len);
    str_mutable[orig_str_len] = 0;
    return str_mutable;
}

__QCLOUD_INTERNAL__ void mqtt_glue_string_mutable_free(char *str_mutable)
{
    osal_free(str_mutable);
}

__QCLOUD_INTERNAL__ uint16_t mqtt_glue_packet_id_generate(qcloud_mqtt_client_t *client)
{
#define PACKET_ID_MAX   (65535)
    uint16_t packet_id = client->packet_id;

    osal_mutex_lock(client->global_lock);
    client->packet_id = (packet_id == PACKET_ID_MAX ? 1 : (packet_id + 1));
    osal_mutex_unlock(client->global_lock);

    return client->packet_id;
}

__QCLOUD_INTERNAL__ void mqtt_glue_connect_id_generate(char *conn_id)
{
	int i, flag;

	srand((unsigned)osal_timer_current_sec());

	for (i = 0; i < QCLOUD_MQTT_CONNECT_ID_MAX - 1; ++i) {
        flag = rand() % 3;

		switch (flag) {
			case 0:
				conn_id[i] = (rand() % 26) + 'a';
				break;

			case 1:
				conn_id[i] = (rand() % 26) + 'A';
				break;

			case 2:
				conn_id[i] = (rand() % 10) + '0';
				break;
		}
	}

	conn_id[QCLOUD_MQTT_CONNECT_ID_MAX - 1] = '\0';
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_packet_send(qcloud_mqtt_client_t *client, size_t length, osal_timer_t *timer)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(timer, QCLOUD_ERR_INVAL);

    qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    size_t cur_bytes_send = 0, total_bytes_send = 0;

    if (length >= sizeof(client->tx_buffer)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    while (total_bytes_send < length && !osal_timer_is_expired(timer)) {
        rc = client->network.write(&(client->network), &client->tx_buffer[total_bytes_send], length, osal_timer_remain(timer), &cur_bytes_send);
        if (rc != QCLOUD_ERR_SUCCESS) {
            /* there was an error writing the data */
            break;
        }
        total_bytes_send += cur_bytes_send;
    }

    if (total_bytes_send == length) {
        /* record the fact that we have successfully sent the packet */
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    QCLOUD_FUNC_EXIT_RC(rc);
}

/**
 * @brief 解析报文的剩余长度字段
 *
 * 每从网络中读取一个字节, 按照MQTT协议算法计算剩余长度
 *
 * @param client Client结构体
 * @param value   剩余长度
 * @param timeout 超时时间
 * @return
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_packet_decode_from_network(qcloud_mqtt_client_t *client, uint32_t *value, uint32_t timeout)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(value, QCLOUD_ERR_INVAL);

    uint8_t byte;
    uint32_t len = 0, multiplier = 1;
    size_t read_len = 0;

    /* The value argument is the important value. len is just used temporarily
     * and never used by the calling function for anything else */

    *value = 0;

    do {
#define MAX_NO_OF_REMAINING_LENGTH_BYTES    4

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES) {
            /* bad data */
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_PACKET_READ)
        }

        if ((client->network.read(&(client->network), &byte, 1, timeout, &read_len)) !=
            QCLOUD_ERR_SUCCESS) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
        }

        *value      += (byte & 127) * multiplier;
        multiplier  *= 128;
    } while ((byte & 128) != 0);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ void mqtt_glue_packet_drain(qcloud_mqtt_client_t *client, osal_timer_t *timer, uint32_t packet_len)
{
    qcloud_err_t rc;
    uint32_t time_remain = 0;
    size_t total_bytes_read = 0, read_len = 0, bytes2read = 0;

    time_remain = osal_timer_remain(timer) + QCLOUD_MQTT_REMAIN_WAIT_MS_MAX;

    if (packet_len < sizeof(client->rx_buffer)) {
        bytes2read = packet_len;
    } else {
        bytes2read = sizeof(client->rx_buffer);
    }

    do {
        rc = client->network.read(&(client->network), client->rx_buffer,
                                    bytes2read, time_remain, &read_len);
        if (rc == QCLOUD_ERR_SUCCESS) {
            total_bytes_read += read_len;
            if ((packet_len - total_bytes_read) >= sizeof(client->rx_buffer)) {
                bytes2read = sizeof(client->rx_buffer);
            } else {
                bytes2read = packet_len - total_bytes_read;
            }
        }
    } while (total_bytes_read < packet_len && rc == QCLOUD_ERR_SUCCESS);
}

/**
 * @brief 从底层SSL/TCP层读取报文数据
 *
 * 1. 读取第一个字节确定报文的类型;
 * 2. 读取剩余长度字段, 最大为四个字节; 剩余长度表示可变包头和有效负载的长度
 * 3. 根据剩余长度, 读取剩下的数据, 包括可变包头和有效负荷
 *
 * @param client        Client结构体
 * @param timer          定时器
 * @param packet_type    报文类型
 * @return
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_packet_read(qcloud_mqtt_client_t *client, osal_timer_t *timer, uint8_t *packet_type)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(timer, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(packet_type, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    size_t read_len = 0;
    mqtt_header_t header = {0};
    uint32_t len = 0, remain_len = 0, time_remain = 0;

	time_remain = osal_timer_remain(timer);
    if (time_remain == 0) {
        // QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_REQUEST_TIMEOUT);
        time_remain = 1;
    }

    // 1. 读取报文固定头部的第一个字节
    rc = client->network.read(&(client->network), client->rx_buffer, (len = 1), time_remain, &read_len);
    if (rc == QCLOUD_ERR_SSL_NOTHING_TO_READ || rc == QCLOUD_ERR_TCP_NOTHING_TO_READ) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NOTHING_TO_READ);
    }

    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    // 2. 读取报文固定头部剩余长度部分
	time_remain = osal_timer_remain(timer) + QCLOUD_MQTT_REMAIN_WAIT_MS_MAX; //确保一包MQTT报文接收完;

    rc = mqtt_glue_packet_decode_from_network(client, &remain_len, time_remain);
    if (QCLOUD_ERR_SUCCESS != rc) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    // 如果读缓冲区的大小小于报文的剩余长度, 报文会被丢弃
    if (remain_len >= sizeof(client->rx_buffer)) {
        mqtt_glue_packet_drain(client, timer, remain_len);

        QCLOUD_LOG_E("rx buffer insufficient[%d] < [%d]", sizeof(client->rx_buffer), remain_len);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    // 将剩余长度写入读缓冲区
    len += mqtt_common_packet_encode(client->rx_buffer + len, remain_len);

    // 3. 读取报文的剩余部分数据
    if ((len + remain_len) > sizeof(client->rx_buffer)) {
        mqtt_glue_packet_drain(client, timer, remain_len);

    	QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

	time_remain = osal_timer_remain(timer) + QCLOUD_MQTT_REMAIN_WAIT_MS_MAX;
	rc = client->network.read(&(client->network), client->rx_buffer + len, remain_len, time_remain, &read_len);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_FUNC_EXIT_RC(rc);
	}

    header.byte = client->rx_buffer[0];
    *packet_type = header.bits.type;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * @brief 消息主题是否相同
 *
 * @param topic_filter
 * @param topicName
 * @return
 */
__QCLOUD_INTERNAL__ int mqtt_glue_is_topic_equals(const char *topic_filter, char *topic)
{
    int topic_len = 0;

    topic_len = strlen(topic);
    if (strlen(topic_filter) != topic_len) {
        return QCLOUD_FALSE;
    }

    if (strncmp(topic_filter, topic, topic_len) == 0) {
        return QCLOUD_TRUE;
    }

    return QCLOUD_FALSE;
}

/**
 * @brief 消息主题匹配
 *
 * assume topic filter and name is in correct format
 * # can only be at end
 * + and # can only be next to separator
 *
 * @param topic_filter   订阅消息的主题名
 * @param topicName     收到消息的主题名, 不能包含通配符
 * @param topicNameLen  主题名的长度
 * @return
 */
__QCLOUD_INTERNAL__ int mqtt_glue_is_topic_matches(const char *topic_filter, char *topic, uint16_t topic_len)
{
    // it's ugly, but it works. keep it, optimize later

    char *curf, *curn, *curn_end, *nextpos;

    curf = (char *)topic_filter;
    curn = topic;
    curn_end = curn + topic_len;

    while (*curf && (curn < curn_end)) {
        if (*curf == '+' && *curn == '/') {
            curf++;
            continue;
        }

        if (*curn == '/' && *curf != '/') {
            break;
        }

        if (*curf != '+' && *curf != '#' && *curf != *curn) {
            break;
        }

        if (*curf == '+') {
            /* skip until we meet the next separator, or end of string */
            nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/') {
                nextpos = ++curn + 1;
            }
        } else if (*curf == '#') {
            /* skip until end of string */
            curn = curn_end - 1;
        }

        curf++;
        curn++;
    };

    if (*curf == '\0') {
        return curn == curn_end;
    } else {
        return (*curf == '#') || *(curf + 1) == '#' || (*curf == '+' && *(curn - 1) == '/');
    }
}

#if (QCLOUD_CFG_DUPLICATED_MSG_REMOVE_EN > 0u)

#define MQTT_MAX_REPEAT_BUF_LEN             50
static uint16_t sg_repeat_packet_id_buf[MQTT_MAX_REPEAT_BUF_LEN];

/**
 * @brief 判断packet_id缓存中是否已经存有传入的packet_id;
 */
__QCLOUD_STATIC__ int mqtt_glue_packet_id_is_exist(uint16_t packet_id)
{
    int i;

    for (i = 0; i < MQTT_MAX_REPEAT_BUF_LEN; ++i) {
        if (packet_id == sg_repeat_packet_id_buf[i]) {
            return QCLOUD_TRUE;
        }
    }
    return QCLOUD_FALSE;
}

__QCLOUD_STATIC__ static void mqtt_glue_packet_id_cache_add(uint16_t packet_id)
{
    static uint16_t current_packet_id_cnt = 0;

    if (mqtt_glue_packet_id_is_exist(packet_id)) {
        return;
    }

    sg_repeat_packet_id_buf[current_packet_id_cnt++] = packet_id;

    if (current_packet_id_cnt >= MQTT_MAX_REPEAT_BUF_LEN) {
        current_packet_id_cnt = current_packet_id_cnt % 50;
    }
}

__QCLOUD_INTERNAL__ void mqtt_glue_packet_id_cache_reset(void)
{
    int i = 0;

    for (i = 0; i < MQTT_MAX_REPEAT_BUF_LEN; ++i) {
        sg_repeat_packet_id_buf[i] = 0;
    }
}

#endif

__QCLOUD_STATIC__ qcloud_mqtt_msg_handler_t *mqtt_glue_msg_handler_get(qcloud_mqtt_client_t *client, char *topic, uint16_t topic_len)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    qcloud_mqtt_msg_handler_t *msg_handler;

    if (!client || !topic) {
        return NULL;
    }

    if (qcloud_list_empty(&client->msg_handler_list)) {
        return NULL;
    }

    osal_mutex_lock(client->msg_handler_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->msg_handler_list) {
        msg_handler = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_msg_handler_t, list);

        if (mqtt_glue_is_topic_equals(msg_handler->topic_filter_mutable, topic) ||
            mqtt_glue_is_topic_matches(msg_handler->topic_filter_mutable, topic, topic_len)) {
            osal_mutex_unlock(client->msg_handler_list_lock);
            return msg_handler;
        }
    }

    osal_mutex_unlock(client->msg_handler_list_lock);

    return NULL;
}

/**
 * @brief 终端收到服务器的的PUBLISH消息之后, 传递消息给消息回调处理函数
 *
 * @param client
 * @param topicName
 * @param message
 * @return
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_incoming_message_deliver(qcloud_mqtt_client_t *client,
                                                                                            char *topic,
                                                                                            uint16_t topic_len,
                                                                                            mqtt_incoming_msg_t *message)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(topic, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(message, QCLOUD_ERR_INVAL);

    qcloud_mqtt_msg_handler_t *msg_handler = NULL;

    message->topic      = topic;
    message->topic_len  = (size_t)topic_len;

    msg_handler = mqtt_glue_msg_handler_get(client, topic, topic_len);
    if (!msg_handler) {
        /* message handler not found for topic */
        /* call default handler */
        mqtt_glue_callback_involve(client, MQTT_EVENT_PUBLISH_RECVEIVED, message);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    msg_handler->handler(client, message, msg_handler->private_data);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ void mqtt_glue_ack_info_destroy(qcloud_mqtt_ack_info_t *ack_info)
{
    qcloud_list_del(&ack_info->list);

    if (ack_info->type == QCLOUD_MQTT_ACK_TYPE_SUBACK && ack_info->handler) {
        mqtt_glue_msg_handler_destory(ack_info->handler);
    }

    osal_free(ack_info);
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_ack_list_scan(qcloud_mqtt_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_list_t *curr, *next;
    mqtt_event_type_t event_type;
    qcloud_mqtt_ack_info_t *ack_info;

    if (qcloud_list_empty(&client->ack_pend_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    osal_mutex_lock(client->ack_pend_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->ack_pend_list) {
        ack_info = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_ack_info_t, list);

        if (!qcloud_mqtt_client_is_connected(client)) {
            continue;
        }

        // check whether the ack is timeout
        if (!osal_timer_is_expired(&ack_info->ack_timer)) {
            continue;
        }

        event_type = mqtt_glue_event_type_get(ack_info->type, QCLOUD_FALSE, QCLOUD_TRUE);

        /* 通知外部网络已经断开 */
        mqtt_glue_callback_involve(client, event_type, &ack_info->packet_id);

        mqtt_glue_ack_info_destroy(ack_info);
    }

    osal_mutex_unlock(client->ack_pend_list_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_ack_list_destroy(qcloud_mqtt_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_list_t *curr, *next;
    qcloud_mqtt_ack_info_t *ack_info;

    if (qcloud_list_empty(&client->ack_pend_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    osal_mutex_lock(client->ack_pend_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->ack_pend_list) {
        ack_info = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_ack_info_t, list);

        mqtt_glue_ack_info_destroy(ack_info);
    }

    osal_mutex_unlock(client->ack_pend_list_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_mqtt_ack_info_t *mqtt_glue_ack_info_create(qcloud_mqtt_ack_type_t type,
                                                                                        qcloud_mqtt_msg_handler_t *handler,
                                                                                        uint32_t timeout,
                                                                                        uint16_t packet_id,
                                                                                        uint16_t packet_len,
                                                                                        uint8_t *packet_body)
{
    qcloud_mqtt_ack_info_t *ack_info = NULL;

    ack_info = (qcloud_mqtt_ack_info_t *)osal_malloc(sizeof(qcloud_mqtt_ack_info_t) + packet_len);
    if (!ack_info) {
        QCLOUD_LOG_E("memory alloc failed!");
        return NULL;
    }

    qcloud_list_init(&ack_info->list);

    osal_timer_init(&ack_info->ack_timer);
    osal_timer_countdown_ms(&ack_info->ack_timer, timeout);

    ack_info->type          = type;
    ack_info->handler       = handler;

    ack_info->packet_id     = packet_id;
    ack_info->packet_len    = packet_len;
    ack_info->packet_body   = (uint8_t *)ack_info + sizeof(qcloud_mqtt_ack_info_t);
    memcpy(ack_info->packet_body, packet_body, packet_len);

    return ack_info;
}

__QCLOUD_STATIC__ void mqtt_glue_ack_list_do_unrecord(qcloud_mqtt_client_t *client, qcloud_mqtt_ack_info_t *ack_info)
{
    qcloud_list_del(&ack_info->list);
    osal_free(ack_info);
}

__QCLOUD_STATIC__ void mqtt_glue_ack_list_unrecord(qcloud_mqtt_client_t *client,
                                                                        qcloud_mqtt_ack_type_t type,
                                                                        uint16_t packet_id,
                                                                        int is_nack,
                                                                        qcloud_mqtt_msg_handler_t **handler)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    mqtt_event_type_t event_type;
    qcloud_mqtt_ack_info_t *ack_info;

    if (!client) {
        return;
    }

    if (qcloud_list_empty(&client->ack_pend_list) ||
        !qcloud_mqtt_client_is_connected(client)) {
        return;
    }

    osal_mutex_lock(client->ack_pend_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->ack_pend_list) {
        ack_info = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_ack_info_t, list);

        if (ack_info->packet_id != packet_id || ack_info->type != type) {
            continue;
        }

        event_type = mqtt_glue_event_type_get(type, is_nack, osal_timer_is_expired(&ack_info->ack_timer));

        mqtt_glue_callback_involve(client, event_type, &ack_info->packet_id);

        if (handler) {
            *handler = ack_info->handler;
        }

        mqtt_glue_ack_list_do_unrecord(client, ack_info);
    }

    osal_mutex_unlock(client->ack_pend_list_lock);
}

__QCLOUD_STATIC__ void mqtt_glue_ack_list_do_record(qcloud_mqtt_client_t *client, qcloud_mqtt_ack_info_t *ack_info)
{
    osal_mutex_lock(client->ack_pend_list_lock);
    qcloud_list_add_tail(&ack_info->list, &client->ack_pend_list);
    osal_mutex_unlock(client->ack_pend_list_lock);
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_ack_list_record(qcloud_mqtt_client_t *client,
                                                                                qcloud_mqtt_ack_type_t type,
                                                                                qcloud_mqtt_msg_handler_t *handler,
                                                                                uint16_t packet_id,
                                                                                uint16_t packet_len)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_mqtt_ack_info_t *ack_info = NULL;

    ack_info = mqtt_glue_ack_info_create(type, handler, client->command_timeout, packet_id, packet_len, client->tx_buffer);
    if (!ack_info) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    mqtt_glue_ack_list_do_record(client, ack_info);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_callback_involve(qcloud_mqtt_client_t *client, mqtt_event_type_t event_type, void *message)
{
    mqtt_event_t event;
    mqtt_event_handler_t *event_handler;

    event_handler = &client->event_handler;

    if (!event_handler) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    event.type      = event_type;
    event.message   = message;

    event_handler->handler(client, event_handler->context, &event);
    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * @brief 终端收到服务器的的PUBACK消息之后, 处理收到的PUBACK报文
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_puback_packet_handle(qcloud_mqtt_client_t *client, osal_timer_t *timer)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(timer, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    uint8_t dup, type;
    uint16_t packet_id;

    rc = mqtt_common_deserialize_ack_packet(&type, &dup, &packet_id, client->rx_buffer, sizeof(client->rx_buffer));
    if (QCLOUD_ERR_SUCCESS != rc) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    mqtt_glue_ack_list_unrecord(client, QCLOUD_MQTT_ACK_TYPE_PUBACK, packet_id, QCLOUD_FALSE, NULL);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_INTERNAL__ void mqtt_glue_msg_handler_destory(qcloud_mqtt_msg_handler_t *msg_handler)
{
    qcloud_list_del(&msg_handler->list);
    mqtt_glue_string_mutable_free(msg_handler->topic_filter_mutable);
    osal_free(msg_handler);
}

__QCLOUD_INTERNAL__ qcloud_mqtt_msg_handler_t * mqtt_glue_msg_handler_create(char *topic_filter_mutable, mqtt_subscribe_opt_t *subscribe_opt)
{
    qcloud_mqtt_msg_handler_t *msg_handler = NULL;

    msg_handler = (qcloud_mqtt_msg_handler_t *)osal_malloc(sizeof(qcloud_mqtt_msg_handler_t));
    if (!msg_handler) {
        QCLOUD_LOG_E("malloc failed");
        return NULL;
    }

    qcloud_list_init(&msg_handler->list);

    msg_handler->private_data           = subscribe_opt->private_data;
    msg_handler->handler                = subscribe_opt->message_handler;
    msg_handler->qos                    = subscribe_opt->qos;
    msg_handler->topic_filter_mutable   = topic_filter_mutable;

    return msg_handler;
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_msg_handler_list_destroy(qcloud_mqtt_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_list_t *curr, *next;
    qcloud_mqtt_msg_handler_t *msg_handler;

    if (qcloud_list_empty(&client->msg_handler_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    osal_mutex_lock(client->msg_handler_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->msg_handler_list) {
        msg_handler = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_msg_handler_t, list);

        mqtt_glue_msg_handler_destory(msg_handler);
    }

    osal_mutex_unlock(client->msg_handler_list_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ int mqtt_glue_handler_is_exist(qcloud_mqtt_client_t *client, qcloud_mqtt_msg_handler_t *that_handler)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    qcloud_mqtt_msg_handler_t *this_handler;

    if (!client || !that_handler) {
        return QCLOUD_FALSE;
    }

    if (qcloud_list_empty(&client->msg_handler_list)) {
        return QCLOUD_FALSE;
    }

    osal_mutex_lock(client->msg_handler_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->msg_handler_list) {
        this_handler = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_msg_handler_t, list);

        if (mqtt_glue_is_topic_equals(this_handler->topic_filter_mutable, that_handler->topic_filter_mutable) &&
            this_handler->handler == that_handler->handler) {
            return QCLOUD_TRUE;
        }
    }

    osal_mutex_unlock(client->msg_handler_list_lock);

    return QCLOUD_FALSE;
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_msg_handler_install(qcloud_mqtt_client_t *client, qcloud_mqtt_msg_handler_t *msg_handler)
{
    if (mqtt_glue_handler_is_exist(client, msg_handler)) {
        QCLOUD_LOG_W("duplicated handler");
        mqtt_glue_msg_handler_destory(msg_handler);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    qcloud_list_add(&msg_handler->list, &client->msg_handler_list);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_INTERNAL__ void mqtt_glue_msg_handler_uninstall(qcloud_mqtt_client_t *client, const char *topic_filter, int *is_subscribed)
{
    char *this_topic = NULL;
    int is_topic_filter_wildcard = QCLOUD_FALSE;
    qcloud_list_t *curr, *next;
    qcloud_mqtt_msg_handler_t *msg_handler = NULL;

    *is_subscribed = QCLOUD_FALSE;
    is_topic_filter_wildcard = (strstr(topic_filter, "/#") || strstr(topic_filter, "/+"));

    if (qcloud_list_empty(&client->msg_handler_list)) {
        return;
    }

    osal_mutex_lock(client->msg_handler_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->msg_handler_list) {
        msg_handler = QCLOUD_LIST_ENTRY(curr, qcloud_mqtt_msg_handler_t, list);

        if (mqtt_glue_is_topic_equals(topic_filter, this_topic) ||
            (is_topic_filter_wildcard &&
            mqtt_glue_is_topic_matches(topic_filter, this_topic, strlen(this_topic)))) {
            // completely the same, or match the wildcard
            mqtt_glue_msg_handler_destory(msg_handler);

            *is_subscribed = QCLOUD_TRUE;
        }
    }

    osal_mutex_unlock(client->msg_handler_list_lock);
}

/**
 * @brief 终端收到服务器的的 SUBACK 消息之后, 处理收到的 SUBACK 报文
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_suback_packet_handle(qcloud_mqtt_client_t *client, osal_timer_t *timer)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(timer, QCLOUD_ERR_INVAL);

    int qos = 0;
    qcloud_err_t rc;
    uint32_t count = 0;
    uint16_t packet_id = 0;
    int is_nack = QCLOUD_FALSE;
    qcloud_mqtt_msg_handler_t *msg_handler = NULL;

    // 反序列化SUBACK报文
    rc = mqtt_common_deserialize_suback_packet(&packet_id, 1, &count, &qos, client->rx_buffer, sizeof(client->rx_buffer));
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    /* check the return code of suback packet:
       0x00(QOS0, SUCCESS), 0x01(QOS1, SUCCESS), 0x02(QOS2, SUCCESS), 0x80(Failure)
     */
    is_nack = (qos == MQTT_PACKET_SUBACK_RC_FAILURE);
    mqtt_glue_ack_list_unrecord(client, QCLOUD_MQTT_ACK_TYPE_SUBACK, packet_id, is_nack, &msg_handler);

    if (!msg_handler) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    if (is_nack) {
        mqtt_glue_msg_handler_destory(msg_handler);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_SUB);
    }

    mqtt_glue_msg_handler_install(client, msg_handler);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * @brief 终端收到服务器的的 USUBACK 消息之后, 处理收到的 USUBACK 报文
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_unsuback_packet_handle(qcloud_mqtt_client_t *client, osal_timer_t *timer)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(timer, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    uint16_t packet_id = 0;

    rc = mqtt_common_deserialize_unsuback_packet(&packet_id, client->rx_buffer, sizeof(client->rx_buffer));
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    mqtt_glue_ack_list_unrecord(client, QCLOUD_MQTT_ACK_TYPE_UNSUBACK, packet_id, QCLOUD_FALSE, NULL);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * @brief 终端收到服务器的的PUBLISH消息之后, 处理收到的PUBLISH报文
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_publish_packet_handle(qcloud_mqtt_client_t *client, osal_timer_t *timer)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_err_t rc;
    char *topic_name;
    uint16_t topic_len;
    uint32_t len = 0;
    char topic_mutable[QCLOUD_MQTT_TOPIC_SIZE_MAX + 1] = {0};
    mqtt_incoming_msg_t msg;

    rc = mqtt_common_deserialize_publish_packet(&msg.dup, (int *)&msg.qos, &msg.retained, &msg.id,
                                            &topic_name, &topic_len,
                                            (uint8_t **) &msg.payload, &msg.payload_len,
                                            client->rx_buffer, sizeof(client->rx_buffer));
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

	if(topic_len > QCLOUD_MQTT_TOPIC_SIZE_MAX) {
		QCLOUD_LOG_E("incoming topic len overflow");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

    // 传过来的topicName没有截断，会把payload也带过来
    memset(topic_mutable, 0, sizeof(topic_mutable));
    memcpy(topic_mutable, topic_name, topic_len);

    if (msg.qos == MQTT_QOS0) {
        rc = mqtt_glue_incoming_message_deliver(client, topic_mutable, topic_len, &msg);
        /* no further processing required for QOS0 */
        QCLOUD_FUNC_EXIT_RC(rc);
    }

#if (QCLOUD_CFG_DUPLICATED_MSG_REMOVE_EN > 0u)
    // 执行订阅消息的回调函数
    if (!mqtt_glue_packet_id_is_exist(msg.id)) {
#endif
        rc = mqtt_glue_incoming_message_deliver(client, topic_mutable, topic_len, &msg);
        if (QCLOUD_ERR_SUCCESS != rc) {
            QCLOUD_FUNC_EXIT_RC(rc);
        }
#if (QCLOUD_CFG_DUPLICATED_MSG_REMOVE_EN > 0u)
    }
    mqtt_glue_packet_id_cache_add(msg.id);
#endif

    osal_mutex_lock(client->tx_lock);

    if (msg.qos == MQTT_QOS1) {
        rc = mqtt_common_serialize_puback_packet(client->tx_buffer, sizeof(client->tx_buffer), msg.id, &len);
    } else if (msg.qos == MQTT_QOS2) {
        rc = mqtt_common_serialize_pubrec_packet(client->tx_buffer, sizeof(client->tx_buffer), msg.id, &len);
    }

    if (QCLOUD_ERR_SUCCESS != rc) {
        osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    rc = mqtt_glue_packet_send(client, len, timer);

    osal_mutex_unlock(client->tx_lock);

    QCLOUD_FUNC_EXIT_RC(rc);
}

/**
 * @brief 处理PUBREC报文, 并发送PUBREL报文, PUBLISH报文为QOS2时
 *
 * @param client
 * @param timer
 * @return
 */
__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_pubrec_packet_handle(qcloud_mqtt_client_t *client, osal_timer_t *timer)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_err_t rc;
    uint32_t len;
    uint16_t packet_id;
    uint8_t dup, type;

    rc = mqtt_common_deserialize_ack_packet(&type, &dup, &packet_id, client->rx_buffer, sizeof(client->rx_buffer));
    if (QCLOUD_ERR_SUCCESS != rc) {
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    osal_mutex_lock(client->tx_lock);
    rc = mqtt_common_serialize_pubrel_packet(client->tx_buffer, sizeof(client->tx_buffer), 0, packet_id, &len);
    if (QCLOUD_ERR_SUCCESS != rc) {
        osal_mutex_unlock(client->tx_lock);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    /* send the PUBREL packet */
    rc = mqtt_glue_packet_send(client, len, timer);
    if (QCLOUD_ERR_SUCCESS != rc) {
        osal_mutex_unlock(client->tx_lock);
        /* there was a problem */
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    osal_mutex_unlock(client->tx_lock);
    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * @brief 处理服务器的心跳包回包
 *
 * @param client
 */
__QCLOUD_STATIC__ void mqtt_glue_pingresp_packet_handle(qcloud_mqtt_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    osal_mutex_lock(client->global_lock);
    client->ping_outstanding = 0;
    osal_timer_countdown(&client->ping_timer, client->keep_alive_interval);
    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT;
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_glue_packet_handle(qcloud_mqtt_client_t *client, osal_timer_t *timer, uint8_t packet_type)
{
    qcloud_err_t rc = QCLOUD_ERR_SUCCESS;

    switch (packet_type) {
        case MQTT_PACKET_TYPE_CONNACK:
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            rc = mqtt_glue_puback_packet_handle(client, timer);
            break;

        case MQTT_PACKET_TYPE_SUBACK:
            rc = mqtt_glue_suback_packet_handle(client, timer);
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            rc = mqtt_glue_unsuback_packet_handle(client, timer);
            break;

        case MQTT_PACKET_TYPE_PUBLISH:
            rc = mqtt_glue_publish_packet_handle(client, timer);
            break;

        case MQTT_PACKET_TYPE_PUBREC:
            rc = mqtt_glue_pubrec_packet_handle(client, timer);
            break;

        case MQTT_PACKET_TYPE_PUBREL:
            QCLOUD_LOG_E("PUBREL NOT handled!");
            break;

        case MQTT_PACKET_TYPE_PUBCOMP:
        case MQTT_PACKET_TYPE_PINGRESP:
            break;

        default:
            /* either unknown packet type or failure occurred should not happen */
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_RX_MESSAGE_INVAL);
    }

    switch (packet_type) {
        /* recv below msgs are all considered as PING OK */
        case MQTT_PACKET_TYPE_PUBACK:
        case MQTT_PACKET_TYPE_SUBACK:
        case MQTT_PACKET_TYPE_UNSUBACK:
        case MQTT_PACKET_TYPE_PINGRESP:
            mqtt_glue_pingresp_packet_handle(client);
            break;

        /* Recv downlink pub means link is OK but we still need to send PING request */
        case MQTT_PACKET_TYPE_PUBLISH:
            osal_mutex_lock(client->global_lock);
            client->ping_outstanding = 0;
            osal_mutex_unlock(client->global_lock);
            break;
    }

    QCLOUD_FUNC_EXIT_RC(rc);
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_spin(qcloud_mqtt_client_t *client, osal_timer_t *timer, uint8_t *packet_type)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(timer, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(packet_type, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;

    /* read the socket, see what work is done */
    rc = mqtt_glue_packet_read(client, timer, packet_type);

    /* nothing to read, not a failure */
    QCLOUD_FUNC_EXIT_RC_IF(rc, QCLOUD_ERR_MQTT_NOTHING_TO_READ, QCLOUD_ERR_SUCCESS);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    QCLOUD_FUNC_EXIT_RC(mqtt_glue_packet_handle(client, timer, *packet_type));
}

__QCLOUD_INTERNAL__ qcloud_err_t mqtt_glue_spin4ack(qcloud_mqtt_client_t *client, osal_timer_t *timer, uint8_t packet_type)
{
	QCLOUD_FUNC_ENTRY;
	qcloud_err_t rc;
	uint8_t ack_packet_type = 0;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(timer, QCLOUD_ERR_INVAL);

	do {
		if (osal_timer_is_expired(timer)) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_REQUEST_TIMEOUT);
		}
        rc = mqtt_glue_spin(client, timer, &ack_packet_type);
	} while (QCLOUD_ERR_SUCCESS == rc && ack_packet_type != packet_type);

	QCLOUD_FUNC_EXIT_RC(rc);
}

#ifdef __cplusplus
}
#endif

