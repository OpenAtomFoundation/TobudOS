#ifndef _MQTT_WRAPPER_H_
#define _MQTT_WRAPPER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTPacket.h"
#include "transport_wrapper.h"

/**
 * @brief connection option
 */
typedef struct mqtt_connect_option_st {
    unsigned short      keep_alive_interval;    /**< keep alive */
    unsigned char       cleansession;           /**< clean session flag */
    char               *client_id;              /**< client id */
    char               *username;               /**< username */
    char               *password;               /**< password */
} mqtt_con_opt_t;

/**
 * @brief publish/receive parameter for a certain subscirbed topic
 */
typedef struct mqtt_publish_option_st {
    int                 qos;            /**< quality of service */
    unsigned char       retained;       /**< RETAIN flag */
    unsigned char       dup;            /**< DUP flag */
    unsigned short      id;             /**< message id */

    char               *topic;          /**< mqtt topic */

    unsigned char      *payload;        /**< mqtt payload */
    int                 payload_len;    /**< mqtt payload length */
} mqtt_pub_opt_t;

/**
 * @brief subscribe parameter for a certain topic
 */
typedef struct mqtt_subscribe_option_st {
    int                 req_qos;    /**< request quality of service */
    int                 count;      /**< number of members in the topicFilters and reqQos arrays */
    unsigned char       dup;        /**< DUP flag */
    unsigned short      id;         /**< message id */
    char               *topic;      /**< mqtt topic */
} mqtt_sub_opt_t;

/**
 * @brief Connect to mqtt server.
 *
 * @param[in]   host    mqtt server(domain name and ip address both are fine).
 * @param[in]   port    mqtt port.
 * @param[in]   opt     mqtt connect parameter.
 *
 * @return  socket id or error if -1.
 */
int tos_mqtt_connect(char *host, const char *port, mqtt_con_opt_t *opt);

/**
 * @brief Publish a message.
 *
 * @param[in]   sock    socket id returned by a tos_mqtt_connect.
 * @param[in]   opt     mqtt publish parameter.
 *
 * @return  successful if 0, or error if -1.
 */
int tos_mqtt_publish(int sock, mqtt_pub_opt_t *opt);

/**
 * @brief Subscribe a topic.
 *
 * @param[in]   sock    socket id returned by a tos_mqtt_connect.
 * @param[in]   opt     mqtt subscribe parameter.
 *
 * @return  successful if 0, or error if -1.
 */
int tos_mqtt_subscribe(int sock, mqtt_sub_opt_t *opt);

/**
 * @brief Receive a message.
 * message of any subscribed topic is received.
 *
 * @param[out]  topic       what topic do we receive.
 * @param[in]   topic_len   length of the topic buffer.
 * @param[out]  payload     what payload of the topic do we receive.
 * @param[in]   payload_len length of the topic payload.
 *
 * @return  incoming payload length, or error if -1.
 */
int tos_mqtt_receive(char *topic, int topic_len, unsigned char *payload, int payload_len);

#endif /* _MQTT_WRAPPER_H_ */

