/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef _IOTX_CM_H_
#define _IOTX_CM_H_

#include "infra_types.h"

#define CM_MAX_FD_NUM             3
#define CM_DEFAULT_YIELD_TIMEOUT  200
/* message confirmation type */
typedef enum {
    /* non ACK */
    /* MQTT: QoS is 0 */
    /* CoAP: NON */
    /* default */
    IOTX_CM_MESSAGE_NO_ACK,
    /* need ACK */
    /* MQTT: QoS is 1 */
    /* CoAP: CON */
    IOTX_CM_MESSAGE_NEED_ACK,
    /* non ACK */
    /* MQTT: QoS is 3 */
    /* CoAP: NONE*/
    IOTX_CM_MESSAGE_SUB_LOCAL,
    /* Maximum number of ack type */
    IOTX_CM_MESSAGE_ACK_MAX
} iotx_cm_ack_types_t;

/* message confirmation type */
typedef enum  {
    /* non ACK */
    /* MQTT: QoS is 0 */
    /* CoAP: NON */
    /* default */
    IOTX_CM_ASYNC,
    /* need ACK */
    /* MQTT: QoS is 1 */
    /* CoAP: CON */
    IOTX_CM_SYNC,
    /* Maximum number of ack type */
    IOTX_CM_SYNC_MAX
} iotx_cm_sync_mode_types_t;

/* protocol type */
typedef enum IOTX_CM_PROTOCOL_TYPES {
    /* MQTT */
    IOTX_CM_PROTOCOL_TYPE_MQTT = 1,
    /* COAP */
    IOTX_CM_PROTOCOL_TYPE_COAP = 2,
    /* HTTP */
    IOTX_CM_PROTOCOL_TYPE_HTTP = 3,
    /* HTTP2 */
    IOTX_CM_PROTOCOL_TYPE_HTTP2 = 4,
    /* Maximum number of protocol type */
    IOTX_CM_PROTOCOL_TYPE_MAX
} iotx_cm_protocol_types_t;


/* event type */
typedef enum IOTX_CM_EVENT_TYPES {
    /* cloud connected */
    IOTX_CM_EVENT_CLOUD_CONNECTED  = 0,
    /* cloud: disconnect */
    /* event_msg is null */
    IOTX_CM_EVENT_CLOUD_CONNECT_FAILED,
    /* cloud: disconnect */
    /* event_msg is null */
    IOTX_CM_EVENT_CLOUD_DISCONNECT,
    /* event_msg is iotx_cm_event_result_pt */
    IOTX_CM_EVENT_SUBCRIBE_SUCCESS,
    IOTX_CM_EVENT_SUBCRIBE_FAILED,
    IOTX_CM_EVENT_UNSUB_SUCCESS,
    IOTX_CM_EVENT_UNSUB_FAILED,
    IOTX_CM_EVENT_PUBLISH_SUCCESS,
    IOTX_CM_EVENT_PUBLISH_FAILED,
    /* Maximum number of event */
    IOTX_CM_EVENT_MAX
} iotx_cm_event_types_t;

/* The structure of cloud Connection event struct */
typedef struct {
    iotx_cm_event_types_t                    type;
    void                                     *msg;
} iotx_cm_event_msg_t;

typedef struct {
    char *topic;
    uint8_t *payload;
    uint32_t  payload_len;
} event_msg_data_t;

#ifdef DEVICE_MODEL_ALINK2
typedef void (*iotx_cm_data_handle_cb)(int fd, const char *topic, uint32_t topic_len, const char *payload, unsigned int payload_len, void *context);
#else
typedef void (*iotx_cm_data_handle_cb)(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                       void *context);
#endif

typedef void (*iotx_cm_event_handle_cb)(int fd, iotx_cm_event_msg_t *event, void *context);


/* IoTx initializa parameters */
typedef struct {
    uint32_t                      request_timeout_ms;
    uint32_t                      keepalive_interval_ms;
    uint32_t                      write_buf_size;
    uint32_t                      read_buf_size;
    iotx_cm_protocol_types_t      protocol_type;
    iotx_cm_event_handle_cb       handle_event;             /* Specify MQTT event handle */
    void                          *context;
#ifdef DEVICE_MODEL_ALINK2
    iotx_dev_meta_info_t         *dev_info;
    iotx_mqtt_region_types_t      region;
#endif
} iotx_cm_init_param_t;

typedef struct {
    iotx_cm_ack_types_t           ack_type;
    iotx_cm_sync_mode_types_t     sync_mode;
    uint32_t                      sync_timeout;
    iotx_cm_data_handle_cb        ack_cb;
    void                          *cb_context;
} iotx_cm_ext_params_t;

int iotx_cm_open(iotx_cm_init_param_t *params);
int iotx_cm_connect(int fd, uint32_t timeout);
int iotx_cm_yield(int fd, unsigned int timeout);
int iotx_cm_sub(int fd, iotx_cm_ext_params_t *ext, const char *topic,
                iotx_cm_data_handle_cb topic_handle_func, void *pcontext);
int iotx_cm_unsub(int fd, const char *topic);
int iotx_cm_pub(int fd, iotx_cm_ext_params_t *ext, const char *topic, const char *payload, unsigned int payload_len);
int iotx_cm_close(int fd);
#endif /* _LINKKIT_CM_H_ */
