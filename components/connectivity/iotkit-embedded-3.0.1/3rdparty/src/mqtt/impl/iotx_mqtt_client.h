/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __IOTX_MQTT_H__
#define __IOTX_MQTT_H__

#include "infra_types.h"
#include "infra_list.h"
#include "infra_timer.h"
#include "iotx_mqtt_config.h"
#include "mqtt_api.h"

#include "MQTTPacket.h"

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define mqtt_malloc(size)            LITE_malloc(size, MEM_MAGIC, "mqtt")
    #define mqtt_free(ptr)               LITE_free(ptr)
#else
    #define mqtt_malloc(size)            HAL_Malloc(size)
    #define mqtt_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#define MQTT_DYNBUF_SEND_MARGIN                      (64)

#define MQTT_DYNBUF_RECV_MARGIN                      (8)

typedef enum {
    IOTX_MC_CONNECTION_ACCEPTED = 0,
    IOTX_MC_CONNECTION_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION = 1,
    IOTX_MC_CONNECTION_REFUSED_IDENTIFIER_REJECTED = 2,
    IOTX_MC_CONNECTION_REFUSED_SERVER_UNAVAILABLE = 3,
    IOTX_MC_CONNECTION_REFUSED_BAD_USERDATA = 4,
    IOTX_MC_CONNECTION_REFUSED_NOT_AUTHORIZED = 5
} iotx_mc_connect_ack_code_t;

/* State of MQTT client */
typedef enum {
    IOTX_MC_STATE_INVALID = 0,                    /* MQTT in invalid state */
    IOTX_MC_STATE_INITIALIZED = 1,                /* MQTT in initializing state */
    IOTX_MC_STATE_CONNECTED = 2,                  /* MQTT in connected state */
    IOTX_MC_STATE_DISCONNECTED = 3,               /* MQTT in disconnected state */
    IOTX_MC_STATE_DISCONNECTED_RECONNECTING = 4,  /* MQTT in reconnecting state */
    IOTX_MC_STATE_CONNECT_BLOCK = 5               /* MQTT in connecting state when using async protocol stack */
} iotx_mc_state_t;

typedef enum MQTT_NODE_STATE {
    IOTX_MC_NODE_STATE_NORMANL = 0,
    IOTX_MC_NODE_STATE_INVALID,
} iotx_mc_node_t;

typedef enum {
    TOPIC_NAME_TYPE = 0,
    TOPIC_FILTER_TYPE
} iotx_mc_topic_type_t;

/* Handle structure of subscribed topic */
typedef struct iotx_mc_topic_handle_s {
    iotx_mc_topic_type_t topic_type;
    iotx_mqtt_event_handle_t handle;
#ifdef PLATFORM_HAS_DYNMEM
    const char *topic_filter;
    struct list_head linked_list;
#else
    const char topic_filter[CONFIG_MQTT_TOPIC_MAXLEN];
    int used;
#endif
} iotx_mc_topic_handle_t;

#if !WITH_MQTT_ONLY_QOS0
/* Information structure of published topic */
typedef struct REPUBLISH_INFO {
    iotx_time_t                 pub_start_time;     /* start time of publish request */
    iotx_mc_node_t              node_state;         /* state of this node */
    uint16_t                    msg_id;             /* packet id of publish */
    uint32_t                    len;                /* length of publish message */
#ifdef PLATFORM_HAS_DYNMEM
    unsigned char              *buf;                /* publish message */
    struct list_head            linked_list;
#else
    unsigned char               buf[IOTX_MC_TX_MAX_LEN];  /* publish message */
    int                         used;
#endif
} iotx_mc_pub_info_t, *iotx_mc_pub_info_pt;
#endif
/* Reconnected parameter of MQTT client */
typedef struct {
    iotx_time_t         reconnect_next_time;        /* the next time point of reconnect */
    uint32_t            reconnect_time_interval_ms; /* time interval of this reconnect */
} iotx_mc_reconnect_param_t;

typedef struct {
    uintptr_t packet_id;
    uint8_t ack_type;
    iotx_mqtt_event_handle_func_fpt sub_state_cb;
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head linked_list;
#else
    int used;
#endif
} mqtt_sub_sync_node_t;

/* structure of MQTT client */
typedef struct Client {
    void                           *lock_generic;                               /* generic lock */
    uint32_t                        packet_id;                                  /* packet id */
    uint32_t                        request_timeout_ms;                         /* request timeout in millisecond */
    uint32_t                        cycle_timeout_ms;
    uint32_t                        buf_size_send;                              /* send buffer size in byte */
#ifdef PLATFORM_HAS_DYNMEM
#if WITH_MQTT_DYN_BUF
    uint32_t                        buf_size_send_max;                          /* send buffer size max limit in byte */
    uint32_t                        buf_size_read_max;                          /* recv buffer size max limit in byte */
#endif
#endif
    uint32_t                        buf_size_read;                              /* read buffer size in byte */
    uint8_t                         keepalive_probes;                           /* keepalive probes */
#ifdef PLATFORM_HAS_DYNMEM
    char                           *buf_send;                                   /* pointer of send buffer */
    char                           *buf_read;                                   /* pointer of read buffer */
#else
    char                            buf_send[IOTX_MC_TX_MAX_LEN];
    char                            buf_read[IOTX_MC_RX_MAX_LEN];
#endif
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head                list_sub_handle;                            /* list of subscribe handle */
#else
    iotx_mc_topic_handle_t          list_sub_handle[IOTX_MC_SUBHANDLE_LIST_MAX_LEN];
#endif
    utils_network_t                 ipstack;                                    /* network parameter */
    iotx_time_t                     next_ping_time;                             /* next ping time */
    iotx_mc_state_t                 client_state;                               /* state of MQTT client */
    iotx_mc_reconnect_param_t       reconnect_param;                            /* reconnect parameter */
    MQTTPacket_connectData          connect_data;                               /* connection parameter */
#if !WITH_MQTT_ONLY_QOS0
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head                list_pub_wait_ack;                          /* list of wait publish ack */
#else
    iotx_mc_pub_info_t              list_pub_wait_ack[IOTX_MC_PUBWAIT_LIST_MAX_LEN];
#endif
#endif
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head                list_sub_sync_ack;
#else
    mqtt_sub_sync_node_t            list_sub_sync_ack[IOTX_MC_SUBSYNC_LIST_MAX_LEN];
#endif
    void                           *lock_list_pub;                              /* lock for list of QoS1 pub */
    void                           *lock_write_buf;                             /* lock of write */
    void                           *lock_read_buf;                             /* lock of write */
    void                           *lock_yield;
    iotx_mqtt_event_handle_t        handle_event;                               /* event handle */
#ifndef PLATFORM_HAS_DYNMEM
    int                            used;
#endif
} iotx_mc_client_t, *iotx_mc_client_pt;

/* Information structure of mutli-subscribe */
typedef struct {
    const char                                    *topicFilter;
    iotx_mqtt_qos_t                                qos;
    iotx_mqtt_event_handle_func_fpt                messageHandler;
} iotx_mutli_sub_info_t, *iotx_mutli_sub_info_pt;


#endif  /* __IOTX_MQTT_H__ */


