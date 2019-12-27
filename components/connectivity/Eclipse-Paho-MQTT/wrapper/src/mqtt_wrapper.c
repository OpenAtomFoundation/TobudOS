#include "mqtt_wrapper.h"

#define BUFFER_LEN      256

static MQTTPacket_connectData mqtt_form_connect_packet(mqtt_con_opt_t *opt)
{
    MQTTPacket_connectData connect_packet = MQTTPacket_connectData_initializer;

    connect_packet.clientID.cstring     = opt->client_id;
    connect_packet.keepAliveInterval    = opt->keep_alive_interval;
    connect_packet.cleansession         = opt->cleansession;
    connect_packet.username.cstring     = opt->username;
    connect_packet.password.cstring     = opt->password;

    return connect_packet;
}

int tos_mqtt_connect(char *host, const char *port, mqtt_con_opt_t *opt)
{
    int sock = 0;
    int serialize_len = 0;
    unsigned char session_present, connack_rc;
    unsigned char buffer[BUFFER_LEN];
    MQTTPacket_connectData connect_packet;

    sock = transport_open(host, port);
    if (sock < 0) {
        return -1;
    }

    connect_packet = mqtt_form_connect_packet(opt);
    serialize_len = MQTTSerialize_connect(buffer, sizeof(buffer), &connect_packet);
    if (serialize_len == 0) {
        goto errout;
    }

    if (transport_sendPacketBuffer(sock, buffer, serialize_len) < 0) {
        goto errout;
    }

    /* wait for connack */
    if (MQTTPacket_read(buffer, sizeof(buffer), transport_getdata) != CONNACK) {
        goto errout;
    }

    if (MQTTDeserialize_connack(&session_present, &connack_rc, buffer, sizeof(buffer)) != 1) {
        goto errout;
    }

    if (connack_rc != 0) {
        goto errout;
    }

    return sock;

errout:
    transport_close(sock);
    return -1;
}

int tos_mqtt_publish(int sock, mqtt_pub_opt_t*opt)
{
    int serialize_len;
    unsigned char buffer[BUFFER_LEN];
    MQTTString pub_topic;

    pub_topic.cstring = opt->topic;
    pub_topic.lenstring.data = NULL;
    pub_topic.lenstring.len = 0;

    serialize_len = MQTTSerialize_publish(buffer, sizeof(buffer), opt->dup, opt->qos,
                                            opt->retained, opt->id, pub_topic, opt->payload,
                                            opt->payload_len);
    if (serialize_len <= 0) {
        return -1;
    }

    if (transport_sendPacketBuffer(sock, buffer, serialize_len) < 0) {
        return -1;
    }

#if 0
    if (MQTTPacket_read(buffer, sizeof(buffer), transport_getdata) != PUBACK) {
        return -1;
    }
#endif

    return 0;
}

int tos_mqtt_subscribe(int sock, mqtt_sub_opt_t *opt)
{
    unsigned short packet_id;
    int max_count, granted_qos;
    int serialize_len;
    unsigned char buffer[BUFFER_LEN];
    MQTTString sub_topic;

    sub_topic.cstring = opt->topic;
    sub_topic.lenstring.data = NULL;
    sub_topic.lenstring.len = 0;

    serialize_len = MQTTSerialize_subscribe(buffer, sizeof(buffer), opt->dup, opt->id,
                                                opt->count, &sub_topic, &opt->req_qos);
    if (serialize_len <= 0) {
        return -1;
    }

    if (transport_sendPacketBuffer(sock, buffer, serialize_len) < 0) {
        return -1;
    }

    if (MQTTPacket_read(buffer, sizeof(buffer), transport_getdata) != SUBACK) {
        return -1;
    }

    if (MQTTDeserialize_suback(&packet_id, 1, &max_count, &granted_qos, buffer, sizeof(buffer)) != 1) {
        return -1;
    }

    return 0;
}

int tos_mqtt_receive(char *topic, int topic_len, unsigned char *payload, int payload_len)
{
    int topic_copy_len, qos;
    unsigned char dup, retained;
    unsigned short packet_id;
    unsigned char *incoming_data;
    int incoming_data_len, payload_copy_len;
    MQTTString incoming_topic;
    unsigned char buffer[BUFFER_LEN];

    if (MQTTPacket_read(buffer, sizeof(buffer), transport_getdata) != PUBLISH)	 {
        return -1;
    }

    if (MQTTDeserialize_publish(&dup, &qos, &retained, &packet_id,
                                    &incoming_topic, &incoming_data, &incoming_data_len,
                                    buffer, sizeof(buffer)) != 1) {
        return -1;
    }

    if (payload) {
        payload_copy_len = incoming_data_len < payload_len ? incoming_data_len : payload_len;
        memcpy(payload, incoming_data, payload_copy_len);
    }

    if (topic) {
        topic_copy_len = incoming_topic.lenstring.len < topic_len ? incoming_topic.lenstring.len : topic_len;
        strncpy(topic, incoming_topic.lenstring.data, topic_copy_len);
        if (topic_copy_len <= topic_len - 1) {
            topic[topic_copy_len] = '\0';
        }   
    }

    return incoming_data_len;
}

