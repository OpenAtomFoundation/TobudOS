/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime: 2020-10-17 14:16:58
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "MQTTPacket.h"
#include "mqtt_list.h"
#include "platform_timer.h"
#include "platform_memory.h"
#include "platform_mutex.h"
#include "platform_thread.h"
#include "mqtt_defconfig.h"
#include "network.h"
#include "random.h"
#include "mqtt_error.h"
#include "mqtt_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mqtt_qos {
    QOS0 = 0,
    QOS1 = 1,
    QOS2 = 2,
    SUBFAIL = 0x80
} mqtt_qos_t;

typedef enum client_state {
	CLIENT_STATE_INVALID = -1,
	CLIENT_STATE_INITIALIZED = 0,
	CLIENT_STATE_CONNECTED = 1,
	CLIENT_STATE_DISCONNECTED = 2,
    CLIENT_STATE_CLEAN_SESSION = 3
}client_state_t;

typedef struct mqtt_connack_data {
    uint8_t rc;
    uint8_t session_present;
} mqtt_connack_data_t;

typedef struct mqtt_message {
    mqtt_qos_t          qos;
    uint8_t             retained;
    uint8_t             dup;
    uint16_t            id;
    size_t              payloadlen;
    void                *payload;
} mqtt_message_t;

typedef struct message_data {
    char                topic_name[MQTT_TOPIC_LEN_MAX];
    mqtt_message_t      *message;
} message_data_t;

typedef void (*interceptor_handler_t)(void* client, message_data_t* msg);
typedef void (*message_handler_t)(void* client, message_data_t* msg);
typedef void (*reconnect_handler_t)(void* client, void* reconnect_date);

typedef struct message_handlers {
    mqtt_list_t         list;
    mqtt_qos_t          qos;
    const char*         topic_filter;
    message_handler_t   handler;
} message_handlers_t;

typedef struct ack_handlers {
    mqtt_list_t         list;
    platform_timer_t    timer;
    uint32_t            type;
    uint16_t            packet_id;
    message_handlers_t  *handler;
    uint16_t            payload_len;
    uint8_t             *payload;
} ack_handlers_t;

typedef struct mqtt_will_options {
    mqtt_qos_t          will_qos;
    uint8_t             will_retained;
    char                *will_topic;
    char                *will_message;
} mqtt_will_options_t;

typedef struct mqtt_client {
    char                        *mqtt_client_id;
    char                        *mqtt_user_name;
    char                        *mqtt_password;
    char                        *mqtt_host;
    char                        *mqtt_port;
    char                        *mqtt_ca;
    void                        *mqtt_reconnect_data;
    uint8_t                     *mqtt_read_buf;
    uint8_t                     *mqtt_write_buf;
    uint16_t                    mqtt_keep_alive_interval;
    uint16_t                    mqtt_packet_id;
    uint32_t                    mqtt_will_flag          : 1;
    uint32_t                    mqtt_clean_session      : 1;
    uint32_t                    mqtt_ping_outstanding   : 2;
    uint32_t                    mqtt_version            : 4;
    uint32_t                    mqtt_ack_handler_number : 24;
    uint32_t                    mqtt_cmd_timeout;
    uint32_t                    mqtt_read_buf_size;
    uint32_t                    mqtt_write_buf_size;
    uint32_t                    mqtt_reconnect_try_duration;
    size_t                      mqtt_client_id_len;
    size_t                      mqtt_user_name_len;
    size_t                      mqtt_password_len;
    mqtt_will_options_t         *mqtt_will_options;
    client_state_t              mqtt_client_state;
    platform_mutex_t            mqtt_write_lock;
    platform_mutex_t            mqtt_global_lock;
    mqtt_list_t                 mqtt_msg_handler_list;
    mqtt_list_t                 mqtt_ack_handler_list;
    network_t                   *mqtt_network;
    platform_thread_t           *mqtt_thread;
    platform_timer_t            mqtt_last_sent;
    platform_timer_t            mqtt_last_received;
    reconnect_handler_t         mqtt_reconnect_handler;
    interceptor_handler_t       mqtt_interceptor_handler;
} mqtt_client_t;


#define MQTT_ROBUSTNESS_CHECK(item, err) if (!(item)) {                                         \
        MQTT_LOG_E("%s:%d %s()... check for error.", __FILE__, __LINE__, __FUNCTION__);         \
        return err; }

#define MQTT_CLIENT_SET_DEFINE(name, type, res)         \
    type mqtt_set_##name(mqtt_client_t *c, type t) {    \
        MQTT_ROBUSTNESS_CHECK((c), res);                \
        c->mqtt_##name = t;                             \
        return c->mqtt_##name;                          \
    }

#define MQTT_CLIENT_SET_STATEMENT(name, type)           \
    type mqtt_set_##name(mqtt_client_t *, type);

MQTT_CLIENT_SET_STATEMENT(client_id, char*)
MQTT_CLIENT_SET_STATEMENT(user_name, char*)
MQTT_CLIENT_SET_STATEMENT(password, char*)
MQTT_CLIENT_SET_STATEMENT(host, char*)
MQTT_CLIENT_SET_STATEMENT(port, char*)
MQTT_CLIENT_SET_STATEMENT(ca, char*)
MQTT_CLIENT_SET_STATEMENT(reconnect_data, void*)
MQTT_CLIENT_SET_STATEMENT(keep_alive_interval, uint16_t)
MQTT_CLIENT_SET_STATEMENT(will_flag, uint32_t)
MQTT_CLIENT_SET_STATEMENT(clean_session, uint32_t)
MQTT_CLIENT_SET_STATEMENT(version, uint32_t)
MQTT_CLIENT_SET_STATEMENT(cmd_timeout, uint32_t)
MQTT_CLIENT_SET_STATEMENT(read_buf_size, uint32_t)
MQTT_CLIENT_SET_STATEMENT(write_buf_size, uint32_t)
MQTT_CLIENT_SET_STATEMENT(reconnect_try_duration, uint32_t)
MQTT_CLIENT_SET_STATEMENT(reconnect_handler, reconnect_handler_t)
MQTT_CLIENT_SET_STATEMENT(interceptor_handler, interceptor_handler_t)

void mqtt_sleep_ms(int ms);
mqtt_client_t *mqtt_lease(void);
int mqtt_release(mqtt_client_t* c);
int mqtt_connect(mqtt_client_t* c);
int mqtt_disconnect(mqtt_client_t* c);
int mqtt_keep_alive(mqtt_client_t* c);
int mqtt_subscribe(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t msg_handler);
int mqtt_unsubscribe(mqtt_client_t* c, const char* topic_filter);
int mqtt_publish(mqtt_client_t* c, const char* topic_filter, mqtt_message_t* msg);
int mqtt_list_subscribe_topic(mqtt_client_t* c);
int mqtt_set_will_options(mqtt_client_t* c, char *topic, mqtt_qos_t qos, uint8_t retained, char *message);

#ifdef __cplusplus
}
#endif

#endif /* _MQTTCLIENT_H_ */
