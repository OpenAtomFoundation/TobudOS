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

#ifndef QCLOUD_IOT_EXPORT_MQTT_H_
#define QCLOUD_IOT_EXPORT_MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief MQTT Quality of Service level
 *
 * Check MQTT spec for QoS define
 */
typedef enum _QoS {
    QOS0 = 0,    // At most once delivery
    QOS1 = 1,    // At least once delivery, PUBACK is required
    QOS2 = 2     // Exactly once delivery. NOT supported currently
} QoS;

/**
 * @brief MQTT message parameter for pub/sub
 */
typedef struct {
    QoS         			qos;          // MQTT QoS level
    uint8_t     			retained;     // RETAIN flag
    uint8_t     			dup;          // DUP flag
    uint16_t    			id;           // MQTT Id

    const char  			*ptopic;      // MQTT topic
    size_t      			topic_len;    // topic length

    void        			*payload;     // MQTT msg payload
    size_t      			payload_len;  // MQTT length of msg payload
} MQTTMessage;

typedef MQTTMessage PublishParams;

#define DEFAULT_PUB_PARAMS {QOS0, 0, 0, 0, NULL, 0, NULL, 0}

typedef enum {

    /* MQTT undefined event */
    MQTT_EVENT_UNDEF = 0,

    /* MQTT disconnect */
    MQTT_EVENT_DISCONNECT = 1,

    /* MQTT reconnect */
    MQTT_EVENT_RECONNECT = 2,

    /* MQTT subscribe success */
    MQTT_EVENT_SUBCRIBE_SUCCESS = 3,

    /* MQTT subscribe timeout */
    MQTT_EVENT_SUBCRIBE_TIMEOUT = 4,

    /* MQTT subscribe fail */
    MQTT_EVENT_SUBCRIBE_NACK = 5,

    /* MQTT unsubscribe success */
    MQTT_EVENT_UNSUBCRIBE_SUCCESS = 6,

    /* MQTT unsubscribe timeout */
    MQTT_EVENT_UNSUBCRIBE_TIMEOUT = 7,

    /* MQTT unsubscribe fail */
    MQTT_EVENT_UNSUBCRIBE_NACK = 8,

    /* MQTT publish success */
    MQTT_EVENT_PUBLISH_SUCCESS = 9,

    /* MQTT publish timeout */
    MQTT_EVENT_PUBLISH_TIMEOUT = 10,

    /* MQTT publish fail */
    MQTT_EVENT_PUBLISH_NACK = 11,

    /* MQTT received msg from server */
    MQTT_EVENT_PUBLISH_RECVEIVED = 12,

    /* MQTT client destroy */
    MQTT_EVENT_CLIENT_DESTROY = 13,

    /* MQTT unsubscribe */
    MQTT_EVENT_UNSUBSCRIBE = 14,

} MQTTEventType;

/**
 * @brief Define MQTT SUBSCRIBE callback when message arrived
 */
typedef void (*OnMessageHandler)(void *pClient, MQTTMessage *message, void *pUserData);

/**
 * @brief Define MQTT SUBSCRIBE callback when event happened
 */
typedef void (*OnSubEventHandler)(void *pClient, MQTTEventType event_type, void *pUserData);

/**
 * @brief Define structure to do MQTT subscription
 */
typedef struct {
    QoS                     qos;                    // MQTT QoS level
    OnMessageHandler        on_message_handler;     // callback when message arrived 
    OnSubEventHandler       on_sub_event_handler;   // callback when event happened
    void                    *user_data;             // user context for callback
} SubscribeParams;

/**
 * Default MQTT subscription parameters
 */
#define DEFAULT_SUB_PARAMS {QOS0, NULL, NULL, NULL}


typedef struct {
    /* MQTT event type */
    MQTTEventType  event_type;

    void *msg;
} MQTTEventMsg;

/**
 * @brief Define MQTT callback when MQTT event happen
 *
 * @param pclient, the MQTT client
 * @param context, user callback context
 * @param msg, the event message.
 *
 * @return none
 */
typedef void (*MQTTEventHandleFun)(void *pclient, void *context, MQTTEventMsg *msg);


/* The structure of MQTT event handle */
typedef struct {
    MQTTEventHandleFun      h_fp;
    void                    *context;
} MQTTEventHandler;

/* The structure of MQTT init parameters */
typedef struct {
        /* device info */
        char                        *product_id;             // product ID
        char                        *device_name;            // device name
    
#ifdef AUTH_MODE_CERT
        char                        *cert_file;              // cert file path
        char                        *key_file;               // key file path
#else
        char                        *device_secret;          // device secret
#endif

    uint32_t					command_timeout;		    // timeout value (unit: ms) for MQTT connect/pub/sub/yield
    uint32_t					keep_alive_interval_ms;	    // MQTT keep alive time interval in millisecond

    uint8_t         			clean_session;			    // flag of clean session, 1 clean, 0 not clean

    uint8_t                   	auto_connect_enable;        // flag of auto reconnection, 1 is enable and recommended

    MQTTEventHandler            event_handle;               // event callback

} MQTTInitParams;

/**
 * Default MQTT init parameters
 */
#ifdef AUTH_MODE_CERT
	#define DEFAULT_MQTTINIT_PARAMS { NULL, NULL, NULL, NULL, 5000, 240 * 1000, 1, 1, {0}}
#else
    #define DEFAULT_MQTTINIT_PARAMS { NULL, NULL, NULL, 5000, 240 * 1000, 1, 1, {0}}
#endif

/**
 * @brief Create MQTT client and connect to MQTT server
 *
 * @param pParams MQTT init parameters
 *
 * @return a valid MQTT client handle when success, or NULL otherwise
 */
void* IOT_MQTT_Construct(MQTTInitParams *pParams);

/**
 * @brief Close connection and destroy MQTT client
 *
 * @param pClient    pointer of handle to MQTT client
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int IOT_MQTT_Destroy(void **pClient);

/**
 * @brief Check connection and keep alive state, read/handle MQTT message in synchronized way
 *
 * @param pClient    handle to MQTT client
 * @param timeout_ms timeout value (unit: ms) for this operation
 *
 * @return QCLOUD_RET_SUCCESS when success, QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT when try reconnecing, or err code for failure
 */
int IOT_MQTT_Yield(void *pClient, uint32_t timeout_ms);

/**
 * @brief Publish MQTT message
 *
 * @param pClient       handle to MQTT client 
 * @param topicName     MQTT topic name
 * @param pParams       publish parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_MQTT_Publish(void *pClient, char *topicName, PublishParams *pParams);

/**
 * @brief Subscribe MQTT topic
 *
 * @param pClient       handle to MQTT client 
 * @param topicFilter   MQTT topic filter
 * @param pParams       subscribe parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_MQTT_Subscribe(void *pClient, char *topicFilter, SubscribeParams *pParams);

/**
 * @brief Unsubscribe MQTT topic
 *
 * @param pClient       handle to MQTT client 
 * @param topicFilter   MQTT topic filter
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_MQTT_Unsubscribe(void *pClient, char *topicFilter);

/**
 * @brief Check if MQTT is connected
 *
 * @param pClient       handle to MQTT client 
 * @return true= connected, false = unconnected
 */
bool IOT_MQTT_IsConnected(void *pClient);

/**
 * @brief Get error code of last IOT_MQTT_Construct operation
 *
 * @return error code of last IOT_MQTT_Construct operation
 */
int IOT_MQTT_GetErrCode(void);

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_MQTT_H_ */
