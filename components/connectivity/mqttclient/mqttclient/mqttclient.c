/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime : 2020-02-16 02:50:32
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "mqttclient.h"

#define     MQTT_MIN_PAYLOAD_SIZE   2               
#define     MQTT_MAX_PAYLOAD_SIZE   268435455       // MQTT imposes a maximum payload size of 268435455 bytes.

static void default_msg_handler(void* client, message_data_t* msg)
{
    LOG_I("%s:%d %s()...\ntopic: %s, qos: %d, \nmessage:%s", __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name, msg->message->qos, (char*)msg->message->payload);
}

static client_state_t mqtt_get_client_state(mqtt_client_t* c)
{
    return c->client_state;
}

static void mqtt_set_client_state(mqtt_client_t* c, client_state_t state)
{
    platform_mutex_lock(&c->global_lock);
    c->client_state = state;
    platform_mutex_unlock(&c->global_lock);
}

static int mqtt_set_publish_dup(mqtt_client_t* c, unsigned char dup)
{
    unsigned char *read_data = c->write_buf;
    unsigned char *write_data = c->write_buf;
	MQTTHeader header = {0};

    if (NULL == c->write_buf)
        RETURN_ERROR(MQTT_SET_PUBLISH_DUP_FAILED);

	header.byte = readChar(&read_data); /* read header */

    if (header.bits.type != PUBLISH)
        RETURN_ERROR(MQTT_SET_PUBLISH_DUP_FAILED);
    
	header.bits.dup = dup;
	writeChar(&write_data, header.byte); /* write header */

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

static int mqtt_ack_handler_is_maximum(mqtt_client_t* c)
{
    return (c->ack_handler_number >= MQTT_ACK_HANDLER_NUM_MAX) ? 1 : 0;
}

static void mqtt_add_ack_handler_num(mqtt_client_t* c)
{
    platform_mutex_lock(&c->global_lock);
    c->ack_handler_number++;
    platform_mutex_unlock(&c->global_lock);
}

static int mqtt_subtract_ack_handler_num(mqtt_client_t* c)
{
    int rc = MQTT_SUCCESS_ERROR;
    platform_mutex_lock(&c->global_lock);
    if (c->ack_handler_number <= 0) {
        goto exit;
    }
    
    c->ack_handler_number--;
    
exit:
    platform_mutex_unlock(&c->global_lock);
    RETURN_ERROR(rc);
}

static unsigned short mqtt_get_next_packet_id(mqtt_client_t *c) 
{
    platform_mutex_lock(&c->global_lock);
    c->packet_id = (c->packet_id == MQTT_MAX_PACKET_ID) ? 1 : c->packet_id + 1;
    platform_mutex_unlock(&c->global_lock);
    return c->packet_id;
}

static int mqtt_decode_packet(mqtt_client_t* c, int* value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES) {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = c->network->read(c->network, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);
exit:
    return len;
}

static void mqtt_packet_drain(mqtt_client_t* c, platform_timer_t *timer, int packet_len)
{
    int total_bytes_read = 0, read_len = 0, bytes2read = 0;

    if (packet_len < c->read_buf_size) {
        bytes2read = packet_len;
    } else {
        bytes2read = c->read_buf_size;
    }

    do {
        read_len = c->network->read(c->network, c->read_buf, bytes2read, platform_timer_remain(timer));
        if (0 != read_len) {
            total_bytes_read += read_len;
            if ((packet_len - total_bytes_read) >= c->read_buf_size) {
                bytes2read = c->read_buf_size;
            } else {
                bytes2read = packet_len - total_bytes_read;
            }
        }
    } while ((total_bytes_read < packet_len) && (0 != read_len));
}

static int mqtt_read_packet(mqtt_client_t* c, int* packet_type, platform_timer_t* timer)
{
    MQTTHeader header = {0};
    int rc;
    int len = 1;
    int remain_len = 0;
    
    if (NULL == packet_type)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);

    /* 1. read the header byte.  This has the packet type in it */
    rc = c->network->read(c->network, c->read_buf, len, platform_timer_remain(timer));
    if (rc != len)
        RETURN_ERROR(MQTT_NOTHING_TO_READ_ERROR);

    /* 2. read the remaining length.  This is variable in itself */
    mqtt_decode_packet(c, &remain_len, platform_timer_remain(timer));

    /* put the original remaining length back into the buffer */
    len += MQTTPacket_encode(c->read_buf + len, remain_len); 

    if ((len + remain_len) > c->read_buf_size) {
        mqtt_packet_drain(c, timer, remain_len);
    	RETURN_ERROR(MQTT_BUFFER_TOO_SHORT_ERROR);
    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if ((remain_len > 0) && ((rc = c->network->read(c->network, c->read_buf + len, remain_len, platform_timer_remain(timer))) != remain_len))
        RETURN_ERROR(MQTT_NOTHING_TO_READ_ERROR);

    header.byte = c->read_buf[0];
    *packet_type = header.bits.type;
    
    platform_timer_cutdown(&c->last_received, (c->connect_params->keep_alive_interval * 1000)); 

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

static int mqtt_send_packet(mqtt_client_t* c, int length, platform_timer_t* timer)
{
    int len = 0;
    int sent = 0;

    while ((sent < length) && (!platform_timer_is_expired(timer))) {
        len = c->network->write(c->network, &c->write_buf[sent], length, platform_timer_remain(timer));
        if (len < 0)  // there was an error writing the data
            break;
        sent += len;
    }

    if (sent == length) {
        platform_timer_cutdown(&c->last_sent, (c->connect_params->keep_alive_interval * 1000));
        RETURN_ERROR(MQTT_SUCCESS_ERROR);
    }
    
    RETURN_ERROR(MQTT_SEND_PACKET_ERROR);
}

static int mqtt_is_topic_equals(const char *topic_filter, const char *topic)
{
    int topic_len = 0;
    
    topic_len = strlen(topic);
    if (strlen(topic_filter) != topic_len) {
        return 0;
    }

    if (strncmp(topic_filter, topic, topic_len) == 0) {
        return 1;
    }

    return 0;
}

static char mqtt_topic_is_matched(char* topic_filter, MQTTString* topic_name)
{
    char* curf = topic_filter;
    char* curn = topic_name->lenstring.data;
    char* curn_end = curn + topic_name->lenstring.len;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+') {
            char* nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

static void mqtt_new_message_data(message_data_t* md, MQTTString* topic_name, mqtt_message_t* message)
{
    int len;
    len = (topic_name->lenstring.len < MQTT_TOPIC_LEN_MAX - 1) ? topic_name->lenstring.len : MQTT_TOPIC_LEN_MAX - 1;
    memcpy(md->topic_name, topic_name->lenstring.data, len);
    md->topic_name[len] = '\0';
    md->message = message;
}

static message_handlers_t *mqtt_get_msg_handler(mqtt_client_t* c, MQTTString* topic_name)
{
    list_t *curr, *next;
    message_handlers_t *msg_handler;

    LIST_FOR_EACH_SAFE(curr, next, &c->msg_handler_list) {
        msg_handler = LIST_ENTRY(curr, message_handlers_t, list);

        if ((NULL != msg_handler->topic_filter) && ((MQTTPacket_equals(topic_name, (char*)msg_handler->topic_filter)) || 
            (mqtt_topic_is_matched((char*)msg_handler->topic_filter, topic_name)))) {
                return msg_handler;
            }
    }
    return NULL;
}

static int mqtt_deliver_message(mqtt_client_t* c, MQTTString* topic_name, mqtt_message_t* message)
{
    int rc = MQTT_FAILED_ERROR;
    message_handlers_t *msg_handler;
    
    msg_handler = mqtt_get_msg_handler(c, topic_name);
    
    if (NULL != msg_handler) {
        message_data_t md;
        mqtt_new_message_data(&md, topic_name, message);
        msg_handler->handler(c, &md);
        rc = MQTT_SUCCESS_ERROR;
    } else
        goto exit;
    
    memset(message->payload, 0, strlen(message->payload));
    memset(topic_name->lenstring.data, 0, topic_name->lenstring.len);
exit:

    RETURN_ERROR(rc);
}

static ack_handlers_t *mqtt_ack_handler_create(mqtt_client_t* c, int type, unsigned short packet_id, unsigned short payload_len, message_handlers_t* handler)
{
    ack_handlers_t *ack_handler = NULL;

    ack_handler = (ack_handlers_t *) platform_memory_alloc(sizeof(ack_handlers_t) + payload_len);
    if (NULL == ack_handler)
        return NULL;

    list_init(&ack_handler->list);
    platform_timer_init(&ack_handler->timer);
    platform_timer_cutdown(&ack_handler->timer, c->cmd_timeout);

    ack_handler->type = type;
    ack_handler->packet_id = packet_id;
    ack_handler->payload_len = payload_len;
    ack_handler->payload = (unsigned char *)ack_handler + sizeof(ack_handlers_t);
    ack_handler->handler = handler;
    memcpy(ack_handler->payload, c->write_buf, payload_len);
    
    return ack_handler;
}

static void mqtt_ack_handler_destroy(ack_handlers_t* ack_handler)
{ 
    if (NULL != &ack_handler->list) {
        list_del(&ack_handler->list);
        platform_memory_free(ack_handler);
    }
}

static void mqtt_ack_handler_resend(mqtt_client_t* c, ack_handlers_t* ack_handler)
{ 
    platform_timer_t timer;
    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);
    platform_timer_cutdown(&ack_handler->timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);
    memcpy(c->write_buf, ack_handler->payload, ack_handler->payload_len);
    
    mqtt_send_packet(c, ack_handler->payload_len, &timer);
    LOG_E("%s:%d %s()... resend %d package, packet_id is %d ", __FILE__, __LINE__, __FUNCTION__, ack_handler->type, ack_handler->packet_id);
    platform_mutex_unlock(&c->write_lock);
}

static int mqtt_ack_list_node_is_exist(mqtt_client_t* c, int type, unsigned short packet_id)
{
    list_t *curr, *next;
    ack_handlers_t *ack_handler;

    if (list_is_empty(&c->ack_handler_list))
        return 0;

    LIST_FOR_EACH_SAFE(curr, next, &c->ack_handler_list) {
        ack_handler = LIST_ENTRY(curr, ack_handlers_t, list);

        if ((packet_id  == ack_handler->packet_id) && (type == ack_handler->type))
            return 1;
    }
    
    return 0;
}

static int mqtt_ack_list_record(mqtt_client_t* c, int type, unsigned short packet_id, unsigned short payload_len, message_handlers_t* handler)
{
    int rc = MQTT_SUCCESS_ERROR;
    ack_handlers_t *ack_handler = NULL;
    
    if (mqtt_ack_list_node_is_exist(c, type, packet_id))
        RETURN_ERROR(MQTT_ACK_NODE_IS_EXIST);

    ack_handler = mqtt_ack_handler_create(c, type, packet_id, payload_len, handler);
    if (NULL == ack_handler)
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);

    mqtt_add_ack_handler_num(c);

    list_add_tail(&ack_handler->list, &c->ack_handler_list);

    RETURN_ERROR(rc);
}

static int mqtt_ack_list_unrecord(mqtt_client_t* c, int type, unsigned short packet_id, message_handlers_t **handler)
{
    list_t *curr, *next;
    ack_handlers_t *ack_handler;

    if (list_is_empty(&c->ack_handler_list))
        RETURN_ERROR(MQTT_SUCCESS_ERROR);

    LIST_FOR_EACH_SAFE(curr, next, &c->ack_handler_list) {
        ack_handler = LIST_ENTRY(curr, ack_handlers_t, list);

        if ((packet_id != ack_handler->packet_id) || (type != ack_handler->type))
            continue;

        if (handler)
            *handler = ack_handler->handler;
        
        mqtt_ack_handler_destroy(ack_handler);
        mqtt_subtract_ack_handler_num(c);
    }
    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}


static message_handlers_t *mqtt_msg_handler_create(const char* topic_filter, mqtt_qos_t qos, message_handler_t handler)
{
    message_handlers_t *msg_handler = NULL;

    msg_handler = (message_handlers_t *) platform_memory_alloc(sizeof(message_handlers_t));
    if (NULL == msg_handler)
        return NULL;
    
    list_init(&msg_handler->list);
    
    msg_handler->qos = qos;
    msg_handler->handler = handler;
    msg_handler->topic_filter = topic_filter;

    return msg_handler;
}

static void mqtt_msg_handler_destory(message_handlers_t *msg_handler)
{
    if (NULL != &msg_handler->list) {
        list_del(&msg_handler->list);
        platform_memory_free(msg_handler);
    }
}

static int mqtt_msg_handler_is_exist(mqtt_client_t* c, message_handlers_t *handler)
{
    list_t *curr, *next;
    message_handlers_t *msg_handler;

    if ((NULL == c) || (NULL == handler))
        return 0;
    
    if (list_is_empty(&c->msg_handler_list))
        return 0;

    LIST_FOR_EACH_SAFE(curr, next, &c->msg_handler_list) {
        msg_handler = LIST_ENTRY(curr, message_handlers_t, list);

        if ((NULL != msg_handler->topic_filter) && (mqtt_is_topic_equals(msg_handler->topic_filter, handler->topic_filter))) {
            LOG_W("%s:%d %s()...msg_handler->topic_filter: %s, handler->topic_filter: %s", __FILE__, __LINE__, __FUNCTION__, msg_handler->topic_filter, handler->topic_filter);
            return 1;
        }
    }
    
    return 0;
}

static int mqtt_msg_handlers_install(mqtt_client_t* c, message_handlers_t *handler)
{
    if (mqtt_msg_handler_is_exist(c, handler)) {
        mqtt_msg_handler_destory(handler);
        RETURN_ERROR(MQTT_SUCCESS_ERROR);
    }

    list_add_tail(&handler->list, &c->msg_handler_list);

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}


static void mqtt_clean_session(mqtt_client_t* c)
{
    list_t *curr, *next;
    ack_handlers_t *ack_handler;
    message_handlers_t *msg_handler;
    
    if (!(list_is_empty(&c->ack_handler_list))) {
        LIST_FOR_EACH_SAFE(curr, next, &c->ack_handler_list) {
            ack_handler = LIST_ENTRY(curr, ack_handlers_t, list);
            platform_memory_free(ack_handler);
        }
        list_del_init(&c->ack_handler_list);
    }

    if (!(list_is_empty(&c->msg_handler_list))) {
        LIST_FOR_EACH_SAFE(curr, next, &c->msg_handler_list) {
            msg_handler = LIST_ENTRY(curr, message_handlers_t, list);
            msg_handler->topic_filter = NULL;
            platform_memory_free(msg_handler);
        }
        list_del_init(&c->msg_handler_list);
    }
}

static void mqtt_ack_list_scan(mqtt_client_t* c)
{
    list_t *curr, *next;
    ack_handlers_t *ack_handler;

    if ((list_is_empty(&c->ack_handler_list)) || (CLIENT_STATE_CONNECTED != mqtt_get_client_state(c)))
        return;

    LIST_FOR_EACH_SAFE(curr, next, &c->ack_handler_list) {
        ack_handler = LIST_ENTRY(curr, ack_handlers_t, list);

        if (!platform_timer_is_expired(&ack_handler->timer))
            continue;
        
        if ((ack_handler->type ==  PUBACK) || (ack_handler->type ==  PUBREC) || (ack_handler->type ==  PUBREL) || (ack_handler->type ==  PUBCOMP)) {
            mqtt_ack_handler_resend(c, ack_handler);
            continue;
        }
        mqtt_ack_handler_destroy(ack_handler);
    }
}

static int mqtt_try_resubscribe(mqtt_client_t* c)
{
    int rc = MQTT_RESUBSCRIBE_ERROR;
    list_t *curr, *next;
    message_handlers_t *msg_handler;

    LOG_W("%s:%d %s()... mqtt try resubscribe ...", __FILE__, __LINE__, __FUNCTION__);
    
    if (list_is_empty(&c->msg_handler_list))
        RETURN_ERROR(MQTT_SUCCESS_ERROR);
    
    LIST_FOR_EACH_SAFE(curr, next, &c->msg_handler_list) {
        msg_handler = LIST_ENTRY(curr, message_handlers_t, list);

        if ((rc = mqtt_subscribe(c, msg_handler->topic_filter, msg_handler->qos, msg_handler->handler)) == MQTT_ACK_HANDLER_NUM_TOO_MUCH)
            LOG_W("%s:%d %s()... mqtt ack handler num too much ...", __FILE__, __LINE__, __FUNCTION__);

    }

    RETURN_ERROR(rc);
}

static int mqtt_try_do_reconnect(mqtt_client_t* c)
{
    int rc = MQTT_CONNECT_FAILED_ERROR;

    if (CLIENT_STATE_CONNECTED != mqtt_get_client_state(c))
        rc = mqtt_connect(c);
    
    if (MQTT_SUCCESS_ERROR == rc) {
        rc = mqtt_try_resubscribe(c);
    }

    LOG_I("%s:%d %s()... mqtt try connect result is %#x", __FILE__, __LINE__, __FUNCTION__, rc);
    
    RETURN_ERROR(rc);
}

static int mqtt_try_reconnect(mqtt_client_t* c)
{
    int rc = MQTT_SUCCESS_ERROR;

    rc = mqtt_try_do_reconnect(c);

    if (platform_timer_is_expired(&c->reconnect_timer)) {
        platform_timer_cutdown(&c->reconnect_timer, c->reconnect_try_duration);
        if (NULL != c->reconnect_handler)
            c->reconnect_handler(c, c->reconnect_date);
        RETURN_ERROR(MQTT_RECONNECT_TIMEOUT_ERROR);
    } 
    
    RETURN_ERROR(rc);
}

static int mqtt_publish_ack_packet(mqtt_client_t *c, unsigned short packet_id, int packet_type)
{
    int len = 0;
    int rc = MQTT_SUCCESS_ERROR;
    platform_timer_t timer;
    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);

    switch (packet_type) {
        case PUBREC: 
            len = MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBREL, 0, packet_id);
            rc = mqtt_ack_list_record(c, PUBCOMP, packet_id, len, NULL);
            if (MQTT_SUCCESS_ERROR != rc)
                goto exit;
            break;
            
        case PUBREL:
            len = MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBCOMP, 0, packet_id);
            break;
            
        default:
            rc = MQTT_PUBLISH_ACK_TYPE_ERROR;
            goto exit;
    }

    if (len <= 0) {
        rc = MQTT_PUBLISH_ACK_PACKET_ERROR;
        goto exit;
    }

    rc = mqtt_send_packet(c, len, &timer);

exit:
    platform_mutex_unlock(&c->write_lock);

    RETURN_ERROR(rc);
}

static int mqtt_puback_and_pubcomp_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
{
    int rc = MQTT_FAILED_ERROR;
    unsigned short packet_id;
    unsigned char dup, packet_type;

    if (MQTTDeserialize_ack(&packet_type, &dup, &packet_id, c->read_buf, c->read_buf_size) != 1)
        rc = MQTT_PUBREC_PACKET_ERROR;
    
    (void) dup;
    rc = mqtt_ack_list_unrecord(c, packet_type, packet_id, NULL);

    RETURN_ERROR(rc);
}

static int mqtt_suback_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
{
    int rc = MQTT_FAILED_ERROR;
    int count = 0;
    int granted_qos = 0;
    unsigned short packet_id;
    int is_nack = 0;
    message_handlers_t *msg_handler = NULL;
    
    if (MQTTDeserialize_suback(&packet_id, 1, &count, (int*)&granted_qos, c->read_buf, c->read_buf_size) != 1) 
        RETURN_ERROR(MQTT_SUBSCRIBE_ACK_PACKET_ERROR);

    is_nack = (granted_qos == SUBFAIL);
    
    rc = mqtt_ack_list_unrecord(c, SUBACK, packet_id, &msg_handler);
    
    if (!msg_handler)
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);
    
    if (is_nack) {
        mqtt_msg_handler_destory(msg_handler);
        RETURN_ERROR(MQTT_SUBSCRIBE_NOT_ACK_ERROR);
    }
    
    rc = mqtt_msg_handlers_install(c, msg_handler);
    
    RETURN_ERROR(rc);
}

static int mqtt_unsuback_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
{
    int rc = MQTT_FAILED_ERROR;
    message_handlers_t *msg_handler;
    unsigned short packet_id = 0;

    if (MQTTDeserialize_unsuback(&packet_id, c->read_buf, c->read_buf_size) != 1)
        RETURN_ERROR(MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR);

    rc = mqtt_ack_list_unrecord(c, UNSUBACK, packet_id, &msg_handler);
    
    if (!msg_handler)
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);
    
    mqtt_msg_handler_destory(msg_handler);

    RETURN_ERROR(rc);
}

static int mqtt_publish_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
{
    int len = 0, rc = MQTT_SUCCESS_ERROR;
    MQTTString topic_name;
    mqtt_message_t msg;
    int qos;
    msg.payloadlen = 0; 

    if (MQTTDeserialize_publish(&msg.dup, &qos, &msg.retained, &msg.id, &topic_name,
        (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->read_buf, c->read_buf_size) != 1)
        RETURN_ERROR(MQTT_PUBLISH_PACKET_ERROR);
    
    msg.qos = (mqtt_qos_t)qos;

    if (msg.qos != QOS0) {
        platform_mutex_lock(&c->write_lock);
        
        if (msg.qos == QOS1)
            len = MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBACK, 0, msg.id);
        else if (msg.qos == QOS2)
            len = MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBREC, 0, msg.id);

        if (len <= 0)
            rc = MQTT_SERIALIZE_PUBLISH_ACK_PACKET_ERROR;
        else
            rc = mqtt_send_packet(c, len, timer);
        
        platform_mutex_unlock(&c->write_lock);
    }

    if (rc < 0)
        RETURN_ERROR(rc);

    if (msg.qos != QOS2)
        mqtt_deliver_message(c, &topic_name, &msg);
    else {
        if ((rc = mqtt_ack_list_record(c, PUBREL, msg.id, len, NULL)) != MQTT_ACK_NODE_IS_EXIST)
            mqtt_deliver_message(c, &topic_name, &msg);
    }
    
    RETURN_ERROR(rc);
}


static int mqtt_pubrec_and_pubrel_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
{
    int rc = MQTT_FAILED_ERROR;
    unsigned short packet_id;
    unsigned char dup, packet_type;

    if (MQTTDeserialize_ack(&packet_type, &dup, &packet_id, c->read_buf, c->read_buf_size) != 1)
        RETURN_ERROR(MQTT_PUBREC_PACKET_ERROR);

    (void) dup;
    rc = mqtt_publish_ack_packet(c, packet_id, packet_type);
    rc = mqtt_ack_list_unrecord(c, packet_type, packet_id, NULL);

    RETURN_ERROR(rc);
}

static int mqtt_packet_handle(mqtt_client_t* c, platform_timer_t* timer)
{

    int rc = MQTT_SUCCESS_ERROR;
    int packet_type = 0;
    
    rc = mqtt_read_packet(c, &packet_type, timer);
    
    platform_timer_init(timer);
    platform_timer_cutdown(timer, c->cmd_timeout);

    switch (packet_type) {
        case 0: /* timed out reading packet */
            break;

        case CONNACK:
            break;

        case PUBACK:
        case PUBCOMP:
            rc = mqtt_puback_and_pubcomp_packet_handle(c, timer);
            break;

        case SUBACK:
            rc = mqtt_suback_packet_handle(c, timer);
            break;
            
        case UNSUBACK:
            rc = mqtt_unsuback_packet_handle(c, timer);
            break;

        case PUBLISH:
            rc = mqtt_publish_packet_handle(c, timer);
            break;

        case PUBREC:
        case PUBREL:
            rc = mqtt_pubrec_and_pubrel_packet_handle(c, timer);
            break;

        case PINGRESP:
            c->ping_outstanding = 0;
            break;

        default:
            goto exit;
    }

    if (mqtt_keep_alive(c) != MQTT_SUCCESS_ERROR) {
        mqtt_set_client_state(c, CLIENT_STATE_DISCONNECTED);
        rc = MQTT_NOT_CONNECT_ERROR;
    }
        
    
exit:
    if (rc == MQTT_SUCCESS_ERROR)
        rc = packet_type;

    RETURN_ERROR(rc);
}

static int mqtt_wait_packet(mqtt_client_t* c, int packet_type, platform_timer_t* timer)
{
    int rc = MQTT_FAILED_ERROR;

    do {
        if (platform_timer_is_expired(timer))
            break; 
        rc = mqtt_packet_handle(c, timer);
    } while (rc != packet_type && rc >= 0);

    RETURN_ERROR(rc);
}

static void mqtt_yield_thread(void *arg)
{
    int rc;
    mqtt_client_t *c = (mqtt_client_t *)arg;
    while (1) {
        rc = mqtt_yield(c, c->cmd_timeout);
        if (MQTT_CLOSE_SESSION_ERROR == rc) {
            LOG_E("%s:%d %s()..., mqtt close session....", __FILE__, __LINE__, __FUNCTION__);
            c->network->disconnect(c->network);
            mqtt_clean_session(c);
            goto exit;
        } else if (MQTT_RECONNECT_TIMEOUT_ERROR == rc) {
            LOG_E("%s:%d %s()..., mqtt reconnect timeout....", __FILE__, __LINE__, __FUNCTION__);
        }
    }
    
exit:
    platform_thread_destroy(c->thread);
}

static int mqtt_connect_with_results(mqtt_client_t* c)
{
    int len = 0;
    int rc = MQTT_CONNECT_FAILED_ERROR;
    platform_timer_t connect_timer;
    mqtt_connack_data_t connack_data = {0};
    MQTTPacket_connectData connect_data = MQTTPacket_connectData_initializer;

    if (NULL == c)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);

    if (CLIENT_STATE_CONNECTED == mqtt_get_client_state(c))
        RETURN_ERROR(MQTT_SUCCESS_ERROR);

    platform_timer_init(&connect_timer);
    platform_timer_cutdown(&connect_timer, c->cmd_timeout);

    rc = c->network->connect(c->network);
    if (MQTT_SUCCESS_ERROR != rc)
        RETURN_ERROR(rc);
    
    LOG_I("%s:%d %s()... mqtt connect success...", __FILE__, __LINE__, __FUNCTION__);

    connect_data.keepAliveInterval = c->connect_params->keep_alive_interval;
    connect_data.cleansession = c->connect_params->clean_session;
    connect_data.MQTTVersion = c->connect_params->mqtt_version;
    connect_data.clientID.cstring= c->connect_params->client_id;
    connect_data.username.cstring = c->connect_params->user_name;
    connect_data.password.cstring = c->connect_params->password;

    platform_timer_cutdown(&c->last_received, (c->connect_params->keep_alive_interval * 1000));

    platform_mutex_lock(&c->write_lock);

    if ((len = MQTTSerialize_connect(c->write_buf, c->write_buf_size, &connect_data)) <= 0)
        goto exit;
    if ((rc = mqtt_send_packet(c, len, &connect_timer)) != MQTT_SUCCESS_ERROR)
        goto exit;

    if (mqtt_wait_packet(c, CONNACK, &connect_timer) == CONNACK) {
        if (MQTTDeserialize_connack(&connack_data.session_present, &connack_data.rc, c->read_buf, c->read_buf_size) == 1)
            rc = connack_data.rc;
        else
            rc = MQTT_CONNECT_FAILED_ERROR;
    } else
        rc = MQTT_CONNECT_FAILED_ERROR;

exit:
    if (rc == MQTT_SUCCESS_ERROR) {
        if(NULL ==c->thread) 
            c->thread= platform_thread_init("mqtt_yield_thread", mqtt_yield_thread, c, MQTT_THREAD_STACK_SIZE, MQTT_THREAD_PRIO, MQTT_THREAD_TICK);
        c->ping_outstanding = 0;
        mqtt_set_client_state(c, CLIENT_STATE_CONNECTED);
    } else {
        mqtt_set_client_state(c, CLIENT_STATE_INITIALIZED);
    }
    
    platform_mutex_unlock(&c->write_lock);

    RETURN_ERROR(rc);
}

/********************************************************* mqttclient global function ********************************************************/

int mqtt_keep_alive(mqtt_client_t* c)
{
    int rc = MQTT_SUCCESS_ERROR;

    if (platform_timer_is_expired(&c->last_sent) || platform_timer_is_expired(&c->last_received)) {
        if (c->ping_outstanding) {
            LOG_W("%s:%d %s()... ping outstanding", __FILE__, __LINE__, __FUNCTION__);
            rc = MQTT_FAILED_ERROR; /* PINGRESP not received in keepalive interval */
        } else {
            platform_timer_t timer;
            platform_timer_init(&timer);
            platform_timer_cutdown(&timer, c->cmd_timeout);
            int len = MQTTSerialize_pingreq(c->write_buf, c->write_buf_size);
            if (len > 0 && (rc = mqtt_send_packet(c, len, &timer)) == MQTT_SUCCESS_ERROR) // send the ping packet
                c->ping_outstanding++;
        }
    }

    RETURN_ERROR(rc);
}

int mqtt_init(mqtt_client_t* c, client_init_params_t* init)
{
    int rc;
    if ((NULL == c) || (NULL == init)) 
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);

    memset(c, 0, sizeof(mqtt_client_t));

    /* network init */
    c->network = (network_t*) platform_memory_alloc(sizeof(network_t));
    if (NULL == c->network) {
        LOG_E("%s:%d %s()... malloc network failed...", __FILE__, __LINE__, __FUNCTION__);
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);
    }
    memset(c->network, 0, sizeof(network_t));

    if ((MQTT_MIN_PAYLOAD_SIZE <= init->read_buf_size) || (MQTT_MAX_PAYLOAD_SIZE >= init->read_buf_size))
        init->read_buf_size = MQTT_DEFAULT_BUF_SIZE;
    if ((MQTT_MIN_PAYLOAD_SIZE <= init->write_buf_size) || (MQTT_MAX_PAYLOAD_SIZE >= init->read_buf_size))
        init->write_buf_size = MQTT_DEFAULT_BUF_SIZE;
    
    c->read_buf = (unsigned char*) platform_memory_alloc(init->read_buf_size);
    c->write_buf = (unsigned char*) platform_memory_alloc(init->write_buf_size);
    if ((NULL == c->read_buf) || (NULL == c->write_buf)){
        LOG_E("%s:%d %s()... malloc buf failed...", __FILE__, __LINE__, __FUNCTION__);
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);
    }

    c->read_buf_size = init->read_buf_size;
    c->write_buf_size =  init->write_buf_size;

    c->packet_id = 1;
    if ((init->cmd_timeout < MQTT_MIN_CMD_TIMEOUT) || (init->cmd_timeout > MQTT_MAX_CMD_TIMEOUT))
        c->cmd_timeout = MQTT_DEFAULT_CMD_TIMEOUT;
    else
        c->cmd_timeout = init->cmd_timeout;
    
    c->ping_outstanding = 0;
    c->ack_handler_number = 0;
    c->client_state = CLIENT_STATE_INITIALIZED;

    if (0 == init->connect_params.keep_alive_interval)
        init->connect_params.keep_alive_interval = MQTT_KEEP_ALIVE_INTERVAL;
    if (0 == init->connect_params.mqtt_version)
        init->connect_params.mqtt_version = MQTT_VERSION;
    if (0 == init->reconnect_try_duration)
        init->reconnect_try_duration = MQTT_RECONNECT_DEFAULT_DURATION;
    
    init->connect_params.client_id_len = strlen(init->connect_params.client_id);
    init->connect_params.user_name_len = strlen(init->connect_params.user_name);
    init->connect_params.password_len = strlen(init->connect_params.password);
    
    c->connect_params = &init->connect_params;
    c->reconnect_try_duration = init->reconnect_try_duration;
    c->connect_params->keep_alive_interval = init->connect_params.keep_alive_interval;

    c->reconnect_date = init->reconnect_date;
    c->reconnect_handler = init->reconnect_handler;

    // c->network->network_params = &init->connect_params.network_params;
    if ((rc = network_init(c->network, &init->connect_params.network_params)) < 0)
        RETURN_ERROR(rc);

    list_init(&c->msg_handler_list);
    list_init(&c->ack_handler_list);
    
    platform_mutex_init(&c->write_lock);
    platform_mutex_init(&c->global_lock);

    platform_timer_init(&c->reconnect_timer);
    platform_timer_init(&c->last_sent);
    platform_timer_init(&c->last_received);

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

int mqtt_release(mqtt_client_t* c)
{
    if (NULL == c)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
    
    if (NULL != c->network) {
        network_release(c->network);
        platform_memory_free(c->network);
        c->network = NULL;
    }

    if (NULL != c->read_buf) {
        platform_memory_free(c->read_buf);
        c->read_buf = NULL;
    }

    if (NULL != c->read_buf) {
        platform_memory_free(c->read_buf);
        c->read_buf = NULL;
    }

    mqtt_clean_session(c);

    memset(c, 0, sizeof(mqtt_client_t));

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

int mqtt_connect(mqtt_client_t* c)
{
    return mqtt_connect_with_results(c);
}

int mqtt_disconnect(mqtt_client_t* c)
{
    int rc = MQTT_FAILED_ERROR;
    platform_timer_t timer;
    int len = 0;

    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);

	len = MQTTSerialize_disconnect(c->write_buf, c->write_buf_size);
    if (len > 0)
        rc = mqtt_send_packet(c, len, &timer);

    platform_mutex_unlock(&c->write_lock);

    mqtt_set_client_state(c, CLIENT_STATE_INVALID);
    
    RETURN_ERROR(rc);
}

int mqtt_subscribe(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t handler)
{
    int rc = MQTT_SUBSCRIBE_ERROR;
    int len = 0;
    unsigned short packet_id;
    platform_timer_t timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topic_filter;
    message_handlers_t *msg_handler = NULL;

    if (CLIENT_STATE_CONNECTED != mqtt_get_client_state(c))
        RETURN_ERROR(MQTT_NOT_CONNECT_ERROR);
    
    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);

    packet_id = mqtt_get_next_packet_id(c);

    len = MQTTSerialize_subscribe(c->write_buf, c->write_buf_size, 0, packet_id, 1, &topic, (int*)&qos);
    if (len <= 0)
        goto exit;
    if ((rc = mqtt_send_packet(c, len, &timer)) != MQTT_SUCCESS_ERROR)
        goto exit; 

    if (NULL == handler)
        handler = default_msg_handler;

    msg_handler = mqtt_msg_handler_create(topic_filter, qos, handler);
    if (NULL == msg_handler)
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);

    rc = mqtt_ack_list_record(c, SUBACK, packet_id, len, msg_handler);

exit:

    platform_mutex_unlock(&c->write_lock);

    RETURN_ERROR(rc);
}

int mqtt_unsubscribe(mqtt_client_t* c, const char* topic_filter)
{
    int len = 0;
    int rc = MQTT_FAILED_ERROR;
    unsigned short packet_id;
    platform_timer_t timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topic_filter;
    message_handlers_t *msg_handler = NULL;

    if (CLIENT_STATE_CONNECTED != mqtt_get_client_state(c))
        RETURN_ERROR(MQTT_NOT_CONNECT_ERROR);
    
    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);

    packet_id = mqtt_get_next_packet_id(c);

    if ((len = MQTTSerialize_unsubscribe(c->write_buf, c->write_buf_size, 0, packet_id, 1, &topic)) <= 0)
        goto exit;
    if ((rc = mqtt_send_packet(c, len, &timer)) != MQTT_SUCCESS_ERROR)
        goto exit; 

    msg_handler = mqtt_msg_handler_create((const char*)topic_filter, QOS0, NULL);
    if (NULL == msg_handler)
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);

    rc = mqtt_ack_list_record(c, UNSUBACK, packet_id, len, msg_handler);

exit:

    platform_mutex_unlock(&c->write_lock);

    RETURN_ERROR(rc);
}

int mqtt_publish(mqtt_client_t* c, const char* topic_filter, mqtt_message_t* msg)
{
    int len = 0;
    int rc = MQTT_FAILED_ERROR;
    platform_timer_t timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topic_filter;

    if (CLIENT_STATE_CONNECTED != mqtt_get_client_state(c)) {
        rc = MQTT_NOT_CONNECT_ERROR;
        goto exit;
    }

    if ((NULL != msg->payload) && (0 == msg->payloadlen))
        msg->payloadlen = strlen((char*)msg->payload);

    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);

    if (msg->qos == QOS1 || msg->qos == QOS2) {
        if (mqtt_ack_handler_is_maximum(c)) {
            rc = MQTT_ACK_HANDLER_NUM_TOO_MUCH;
            goto exit;
        }
        msg->id = mqtt_get_next_packet_id(c);
    }
    
    len = MQTTSerialize_publish(c->write_buf, c->write_buf_size, 0, msg->qos, msg->retained, msg->id,
              topic, (unsigned char*)msg->payload, msg->payloadlen);
    if (len <= 0)
        goto exit;
    
    if ((rc = mqtt_send_packet(c, len, &timer)) != MQTT_SUCCESS_ERROR)
        goto exit;
    
    if (QOS0 != msg->qos) {
        mqtt_set_publish_dup(c,1);
        
        if (QOS1 == msg->qos) {
            rc = mqtt_ack_list_record(c, PUBACK, msg->id, len, NULL);
        } else if (QOS2 == msg->qos) {
            rc = mqtt_ack_list_record(c, PUBREC, msg->id, len, NULL);
        }
    }
    
exit:
    platform_mutex_unlock(&c->write_lock);

    RETURN_ERROR(rc);     
}


int mqtt_yield(mqtt_client_t* c, int timeout_ms)
{
    int rc = MQTT_SUCCESS_ERROR;
    client_state_t state;
    platform_timer_t timer;

    if (NULL == c)
        RETURN_ERROR(MQTT_FAILED_ERROR);

    if (0 == timeout_ms)
        timeout_ms = c->cmd_timeout;

    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, timeout_ms);
    
    while (!platform_timer_is_expired(&timer)) {
        state = mqtt_get_client_state(c);
        if (CLIENT_STATE_INVALID ==  state) {
            RETURN_ERROR(MQTT_CLOSE_SESSION_ERROR);
        } else if (CLIENT_STATE_CONNECTED != state) {
            rc = mqtt_try_reconnect(c);
            if (MQTT_RECONNECT_TIMEOUT_ERROR == rc)
                RETURN_ERROR(rc);
            continue;
        }
        
        rc = mqtt_packet_handle(c, &timer);

        if (rc >= 0) {
            mqtt_ack_list_scan(c);
        } else if (MQTT_NOT_CONNECT_ERROR == rc) {
            LOG_E("%s:%d %s()... mqtt not connect", __FILE__, __LINE__, __FUNCTION__);
            platform_timer_cutdown(&c->reconnect_timer, c->reconnect_try_duration);
        } else {
            break;
        }
    }
    
    RETURN_ERROR(rc);
}
