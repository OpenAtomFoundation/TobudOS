/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AT_MQTT_H_
#define _AT_MQTT_H_

#ifndef PLATFORM_HAS_DYNMEM
    #ifndef IOTX_MC_SUBHANDLE_LIST_MAX_LEN
        #define IOTX_MC_SUBHANDLE_LIST_MAX_LEN          (5)
    #endif

    #ifndef CONFIG_MQTT_TOPIC_MAXLEN
        #define CONFIG_MQTT_TOPIC_MAXLEN                   (128)
    #endif
#endif

/* State of MQTT client */
typedef enum {
    IOTX_MC_STATE_INVALID = 0,                    /* MQTT in invalid state */
    IOTX_MC_STATE_INITIALIZED = 1,                /* MQTT in initializing state */
    IOTX_MC_STATE_CONNECTED = 2,                  /* MQTT in connected state */
    IOTX_MC_STATE_DISCONNECTED = 3,               /* MQTT in disconnected state */
    IOTX_MC_STATE_DISCONNECTED_RECONNECTING = 4,  /* MQTT in reconnecting state */
} iotx_mc_state_t;

typedef enum {
    TOPIC_NAME_TYPE = 0,
    TOPIC_FILTER_TYPE
} iotx_mc_topic_type_t;

/* Handle structure of subscribed topic */
typedef struct iotx_mc_topic_handle_s {
#ifdef PLATFORM_HAS_DYNMEM
    const char *topic_filter;
#else
    const char topic_filter[CONFIG_MQTT_TOPIC_MAXLEN];
    int used;
#endif
    iotx_mc_topic_type_t topic_type;
    iotx_mqtt_event_handle_t handle;
    struct iotx_mc_topic_handle_s *next;
} iotx_mc_topic_handle_t;

typedef struct Client {
    uint32_t                        packet_id;                                  /* packet id */
    void                           *lock_generic;                               /* generic lock */
    void                           *lock_yield;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_topic_handle_t         *first_sub_handle;                           /* list of subscribe handle */
#else
    iotx_mc_topic_handle_t          list_sub_handle[IOTX_MC_SUBHANDLE_LIST_MAX_LEN];
#endif
    void                           *lock_list_sub;                              /* lock for list of sub/unsub */
    iotx_mc_state_t                 client_state;                               /* state of MQTT client */
    iotx_mqtt_event_handle_t        handle_event;                               /* event handle */
#ifndef PLATFORM_HAS_DYNMEM
    int                             used;
#endif
} iotx_mc_client_t, *iotx_mc_client_pt;

#endif
