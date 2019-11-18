/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef IOT_MQTT_CLIENT_H_
#define IOT_MQTT_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "utils_param_check.h"

#include "mqtt_client_net.h"

#include "utils_timer.h"
#include "utils_list.h"

/* packet id, random from [1 - 65536] */
#define MAX_PACKET_ID               								(65535)

/* Max size of conn Id  */
#define MAX_CONN_ID_LEN												(6)

/* Max number of topic subscribed */
#define MAX_MESSAGE_HANDLERS        								(10)

/* Max number in repub list */
#define MAX_REPUB_NUM                                               (20)

/* Minimal wait interval when reconnect */
#define MIN_RECONNECT_WAIT_INTERVAL 								(1000)

/* Minimal MQTT timeout value */
#define MIN_COMMAND_TIMEOUT         								(500)

/* Maxmal MQTT timeout value  */
#define MAX_COMMAND_TIMEOUT         								(20000)

/* Max size of a topic name */
#define MAX_SIZE_OF_CLOUD_TOPIC                                     ((MAX_SIZE_OF_DEVICE_NAME) + (MAX_SIZE_OF_PRODUCT_ID) + 64 + 6)

/* minimal TLS handshaking timeout value (unit: ms) */
#define QCLOUD_IOT_TLS_HANDSHAKE_TIMEOUT                            (5 * 1000)


/**
 * @brief MQTT Message Type
 */
typedef enum msgTypes {
	RESERVED    = 0,     // Reserved
	CONNECT     = 1,     // Client request to connect to Server
	CONNACK     = 2,     // Connect Acknowledgment
	PUBLISH     = 3,     // Publish message
	PUBACK      = 4,     // Publish Acknowledgment
	PUBREC      = 5,     // Publish Received
	PUBREL      = 6,     // Publish Release
	PUBCOMP     = 7,     // Publish Complete
	SUBSCRIBE   = 8,     // Client Subscribe request
	SUBACK      = 9,     // Subscribe Acknowledgment
	UNSUBSCRIBE = 10,    // Client Unsubscribe request
	UNSUBACK    = 11,    // Unsubscribe Acknowledgment
	PINGREQ     = 12,    // PING Request
	PINGRESP    = 13,    // PING Response
	DISCONNECT  = 14     // Client is Disconnecting
} MessageTypes;

typedef enum {
	NOTCONNECTED = 0,
	CONNECTED	 = 1
} ConnStatus;


/** 
 * MQTT byte 1: fixed header
 * bits  |7654: Message Type  | 3:DUP flag |  21:QoS level | 0:RETAIN |
 */
#define MQTT_HEADER_TYPE_SHIFT          0x04
#define MQTT_HEADER_TYPE_MASK           0xF0
#define MQTT_HEADER_DUP_SHIFT           0x03
#define MQTT_HEADER_DUP_MASK            0x08
#define MQTT_HEADER_QOS_SHIFT           0x01
#define MQTT_HEADER_QOS_MASK            0x06
#define MQTT_HEADER_RETAIN_MASK	        0x01


/**
 * @brief MQTT will options sturcture
 *
 */
typedef struct {
    char        struct_id[4];      // The eyecatcher for this structure. must be MQTW
    uint8_t     struct_version;    // struct version = 0
    char        *topic_name;       
    char        *message;          
    uint8_t     retained;          
    QoS         qos;               
} WillOptions;

/**
 * default MQTT will options
 */
#define DEFAULT_WILL_OPTIONS { {'M', 'Q', 'T', 'W'}, 0, NULL, NULL, 0, QOS0 }

/**
 * @brief define MQTT connect parameters structure
 *
 */
typedef struct {
    char            			*client_id;             // unique client id
    char            			*username;              // user name
    char						*password;				// passwrod

    char					 	conn_id[MAX_CONN_ID_LEN];

    char            			struct_id[4];           // The eyecatcher for this structure.  must be MQTC.
    uint8_t         			struct_version;         // struct version = 0
    uint8_t         			mqtt_version;            // MQTT protocol version: 4 = 3.1.1

    uint16_t        			keep_alive_interval;    // keep alive interval, unit: second
    uint8_t         			clean_session;          // flag of clean session, refer to MQTT spec 3.1.2.4

    uint8_t                   	auto_connect_enable;    // enable auto connection or not

#ifdef AUTH_WITH_NOTLS
    char						*device_secret;					// PSK
    int						    device_secret_len;			    // length of PSK
#endif

} MQTTConnectParams;

/**
 * default value of MQTT connect parameters structure
 */
#ifdef AUTH_WITH_NOTLS
#define DEFAULT_MQTTCONNECT_PARAMS { NULL, NULL, NULL, {0}, {'M', 'Q', 'T', 'C'}, 0, 4, 240, 1, 1, NULL, 0}
#else
#define DEFAULT_MQTTCONNECT_PARAMS { NULL, NULL, NULL, {0}, {'M', 'Q', 'T', 'C'}, 0, 4, 240, 1, 1}
#endif

/**
 * @brief data structure for topic subscription handle
 */
typedef struct SubTopicHandle {
    const char              *topic_filter;               // topic name, wildcard filter is supported
    OnMessageHandler        message_handler;             // callback when msg of this subscription arrives
    OnSubEventHandler       sub_event_handler;           // callback when event of this subscription happens
    void                    *handler_user_data;          // user context for callback
    QoS                     qos;                         // QoS
} SubTopicHandle;

/**
 * @brief MQTT QCloud IoT Client structure
 */
typedef struct Client {
    uint8_t                  is_connected;                                  
    uint8_t                  was_manually_disconnected;                     
    uint8_t                  is_ping_outstanding;                           // 1 = ping request is sent while ping response not arrived yet

    uint16_t                 next_packet_id;                                // MQTT random packet id
    uint32_t                 command_timeout_ms;                            // MQTT command timeout, unit:ms

    uint32_t                 current_reconnect_wait_interval;               // unit:ms
    uint32_t                 counter_network_disconnected;                  // number of disconnection

    size_t                   write_buf_size;                                // size of MQTT write buffer
    size_t                   read_buf_size;                                 // size of MQTT read buffer
    unsigned char            write_buf[QCLOUD_IOT_MQTT_TX_BUF_LEN];         // MQTT write buffer
    unsigned char            read_buf[QCLOUD_IOT_MQTT_RX_BUF_LEN];          // MQTT read buffer

    void                     *lock_generic;                                 // mutex/lock for this client struture
    void                     *lock_write_buf;                          		// mutex/lock for write buffer 

    void                     *lock_list_pub;                                // mutex/lock for puback waiting list
    void                     *lock_list_sub;                                // mutex/lock for suback waiting list

    List                     *list_pub_wait_ack;                            // puback waiting list
    List                     *list_sub_wait_ack;                            // suback waiting list

    MQTTEventHandler         event_handle;                                  // callback for MQTT event

    MQTTConnectParams        options;                                       // handle to connection parameters

    Network                  network_stack;                                 // MQTT network stack

    Timer                    ping_timer;                                    // MQTT ping timer
    Timer                    reconnect_delay_timer;                         // MQTT reconnect delay timer

    SubTopicHandle           sub_handles[MAX_MESSAGE_HANDLERS];             // subscription handle array

} Qcloud_IoT_Client;

/**
 * @brief MQTT protocol version
 */
typedef enum {
    MQTT_3_1_1 = 4
} MQTT_VERSION;


typedef enum MQTT_NODE_STATE {
    MQTT_NODE_STATE_NORMANL = 0,
    MQTT_NODE_STATE_INVALID,
} MQTTNodeState;

/* topic publish info */
typedef struct REPUBLISH_INFO {
    Timer                   pub_start_time;     /* timer for puback waiting */
    MQTTNodeState           node_state;         /* node state in wait list */
    uint16_t                msg_id;             /* packet id */
    uint32_t                len;                /* msg length */
    unsigned char          *buf;                /* msg buffer */
} QcloudIotPubInfo;

/* topic subscribe/unsubscribe info */
typedef struct SUBSCRIBE_INFO {
    enum msgTypes           type;           /* type: sub or unsub */
    uint16_t                msg_id;         /* packet id */
    Timer                   sub_start_time; /* timer for suback waiting */
    MQTTNodeState           node_state;     /* node state in wait list */
    SubTopicHandle          handler;        /* handle of topic subscribed(unsubcribed) */
    uint16_t                len;            /* msg length */
    unsigned char          *buf;            /* msg buffer */
} QcloudIotSubInfo;

/**
 * @brief Init MQTT client
 *
 * @param pClient    handle to MQTT client
 * @param pParams    MQTT init parameters
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_init(Qcloud_IoT_Client *pClient, MQTTInitParams *pParams);

/**
 * @brief Release resources of MQTT client
 *
 * @param pClient    handle to MQTT client
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_deinit(Qcloud_IoT_Client *pClient);


/**
 * @brief Connect with MQTT server
 *
 * @param pClient    handle to MQTT client
 * @param pParams    MQTT connect parameters
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_connect(Qcloud_IoT_Client *pClient, MQTTConnectParams *pParams);

/**
 * @brief Reconnect with MQTT server and re-subscribe topics if reconnected
 *
 * @param pClient    handle to MQTT client
 *
 * @return QCLOUD_RET_MQTT_RECONNECTED for success, or err code for failure
 */
int qcloud_iot_mqtt_attempt_reconnect(Qcloud_IoT_Client *pClient);

/**
 * @brief Disconnect with MQTT server
 *
 * @param pClient    handle to MQTT client
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_disconnect(Qcloud_IoT_Client *pClient);

/**
 * @brief Publish MQTT message
 *
 * @param pClient       handle to MQTT client 
 * @param topicName     MQTT topic name
 * @param pParams       publish parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int qcloud_iot_mqtt_publish(Qcloud_IoT_Client *pClient, char *topicName, PublishParams *pParams);

/**
 * @brief Subscribe MQTT topic
 *
 * @param pClient       handle to MQTT client 
 * @param topicFilter   MQTT topic filter
 * @param pParams       subscribe parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int qcloud_iot_mqtt_subscribe(Qcloud_IoT_Client *pClient, char *topicFilter, SubscribeParams *pParams);

/**
 * @brief Re-subscribe MQTT topics
 *
 * @param pClient       handle to MQTT client 
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure 
 */
int qcloud_iot_mqtt_resubscribe(Qcloud_IoT_Client *pClient);

/**
 * @brief Unsubscribe MQTT topic
 *
 * @param pClient       handle to MQTT client 
 * @param topicFilter   MQTT topic filter
 *
 * @return packet id (>=0) when success, or err code (<0) for failure  
 */
int qcloud_iot_mqtt_unsubscribe(Qcloud_IoT_Client *pClient, char *topicFilter);

/**
 * @brief Check connection and keep alive state, read/handle MQTT message in synchronized way
 *
 * @param pClient    handle to MQTT client
 * @param timeout_ms timeout value (unit: ms) for this operation
 *
 * @return QCLOUD_RET_SUCCESS when success, QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT when try reconnecing, or err code for failure
 */
int qcloud_iot_mqtt_yield(Qcloud_IoT_Client *pClient, uint32_t timeout_ms);

/**
 * @brief Check if auto reconnect is enabled or not
 *
 * @param pClient    handle to MQTT client
 * @return true if auto reconnect is enabled
 */
bool qcloud_iot_mqtt_is_autoreconnect_enabled(Qcloud_IoT_Client *pClient);

/**
 * @brief Set to enable auto reconnect or not
 *
 * @param pClient    handle to MQTT client
 * @param value enable or disable
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_set_autoreconnect(Qcloud_IoT_Client *pClient, bool value);

/**
 * @brief Get the count of disconnection
 *
 * @param pClient handle to MQTT client
 * @return count of disconnection
 */
int qcloud_iot_mqtt_get_network_disconnected_count(Qcloud_IoT_Client *pClient);

/**
 * @brief Set the count of disconnection
 *
 * @param pClient handle to MQTT client
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_reset_network_disconnected_count(Qcloud_IoT_Client *pClient);

/**
 * @brief Get next packet id
 *
 * @param pClient
 * @return
 */
uint16_t get_next_packet_id(Qcloud_IoT_Client *pClient);

/**
 * @brief Get next conn id
 *
 * @param options
 * @return
 */
void get_next_conn_id(char *conn_id);

/**
 * @brief Init packet header
 * @param header
 * @param message_type
 * @param qos
 * @param dup
 * @param retained
 * @return
 */
int mqtt_init_packet_header(unsigned char *header, MessageTypes message_type, QoS qos, uint8_t dup, uint8_t retained);

/**
 * @brief Read and handle one MQTT msg/ack from server
 *
 * @param pClient
 * @param timer
 * @param packet_type
 * @param qos
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int cycle_for_read(Qcloud_IoT_Client *pClient, Timer *timer, uint8_t *packet_type, QoS qos);

/**
 * @brief Send the packet in buffer 
 *
 * @param pClient       
 * @param length        
 * @param timer         
 * @return
 */
int send_mqtt_packet(Qcloud_IoT_Client *pClient, size_t length, Timer *timer);

/**
 * @brief wait for a specific packet with timeout
 *
 * only used in single-threaded mode where one command at a time is in process
 *
 * @param pClient       MQTT Client
 * @param packet_type   MQTT packet type
 * @param timer         timer with timeout
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int wait_for_read(Qcloud_IoT_Client *pClient, uint8_t packet_type, Timer *timer, QoS qos);

/**
 * @brief Set MQTT connection state
 *
 * @param pClient 		MQTT Client
 * @param connected 	0: disconnected  1: connected
 * @return
 */
void set_client_conn_state(Qcloud_IoT_Client *pClient, uint8_t connected);

/**
 * @brief Get MQTT connection state
 *
 * @param pClient 		MQTT Client
 * @return				0: disconnected  1: connected
 */
uint8_t get_client_conn_state(Qcloud_IoT_Client *pClient);

/**
 * @brief Check Publish ACK waiting list, remove the node if PUBACK received or timeout
 *
 * @param pClient MQTT client
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_pub_info_proc(Qcloud_IoT_Client *pClient);

/**
 * @brief Check Subscribe ACK waiting list, remove the node if SUBACK received or timeout
 *
 * @param pClient MQTT client
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_iot_mqtt_sub_info_proc(Qcloud_IoT_Client *pClient);

int push_sub_info_to(Qcloud_IoT_Client *c, int len, unsigned short msgId, MessageTypes type,
								   SubTopicHandle *handler, ListNode **node);

int serialize_pub_ack_packet(unsigned char *buf, size_t buf_len, MessageTypes packet_type, uint8_t dup,
							 uint16_t packet_id,
							 uint32_t *serialized_len);

int serialize_packet_with_zero_payload(unsigned char *buf, size_t buf_len, MessageTypes packetType, uint32_t *serialized_len);

int deserialize_publish_packet(unsigned char *dup, QoS *qos, uint8_t *retained, uint16_t *packet_id, char **topicName,
							   uint16_t *topicNameLen, unsigned char **payload, size_t *payload_len, unsigned char *buf, size_t buf_len);

int deserialize_suback_packet(uint16_t *packet_id, uint32_t max_count, uint32_t *count,
                                     QoS *grantedQoSs, unsigned char *buf, size_t buf_len);

int deserialize_unsuback_packet(uint16_t *packet_id, unsigned char *buf, size_t buf_len);

int deserialize_ack_packet(uint8_t *packet_type, uint8_t *dup, uint16_t *packet_id, unsigned char *buf, size_t buf_len);

#ifdef MQTT_RMDUP_MSG_ENABLED

void reset_repeat_packet_id_buffer(void);

#endif

size_t get_mqtt_packet_len(size_t rem_len);

size_t mqtt_write_packet_rem_len(unsigned char *buf, uint32_t length);

int mqtt_read_packet_rem_len_form_buf(unsigned char *buf, uint32_t *value, uint32_t *readBytesLen);

uint16_t mqtt_read_uint16_t(unsigned char **pptr);

unsigned char mqtt_read_char(unsigned char **pptr);

void mqtt_write_char(unsigned char **pptr, unsigned char c);

void mqtt_write_uint_16(unsigned char **pptr, uint16_t anInt);

void mqtt_write_utf8_string(unsigned char **pptr, const char *string);

#ifdef __cplusplus
}
#endif

#endif //IOT_MQTT_CLIENT_H_
