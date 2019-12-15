/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef IOTX_MQTT_CONFIG_H__
#define IOTX_MQTT_CONFIG_H__

#ifndef WITH_MQTT_DYN_BUF
    #define WITH_MQTT_DYN_BUF                   (1)
#endif

#ifndef WITH_MQTT_QOS2_PACKET
    #define WITH_MQTT_QOS2_PACKET               (0)
#endif

#ifndef WITH_MQTT_FLOW_CTRL
    #define WITH_MQTT_FLOW_CTRL                 (0)
#endif

#ifndef WITH_MQTT_ONLY_QOS0
    #define WITH_MQTT_ONLY_QOS0                 (0)
#endif

#ifndef WITH_MQTT_DYN_CONNINFO
    #define WITH_MQTT_DYN_CONNINFO              (1)
#endif

#ifndef WITH_MQTT_ZIP_TOPIC
    #define WITH_MQTT_ZIP_TOPIC                 (0)
#endif

/* maximum republish elements in list */
#define IOTX_MC_REPUB_NUM_MAX                   (20)

/* MQTT client version number */
#define IOTX_MC_MQTT_VERSION                    (4)

/* maximum MQTT packet-id */
#define IOTX_MC_PACKET_ID_MAX                   (65535)

/* maximum number of simultaneously invoke subscribe request */
#define IOTX_MC_SUB_REQUEST_NUM_MAX             (256)

/* Minimum interval of MQTT reconnect in millisecond */
#define IOTX_MC_RECONNECT_INTERVAL_MIN_MS       (1000)

/* Maximum interval of MQTT reconnect in millisecond */
#define IOTX_MC_RECONNECT_INTERVAL_MAX_MS       (60000)

/* Max times of keepalive which has been send and did not received response package */
#define IOTX_MC_KEEPALIVE_PROBE_MAX             (2)


/* Linked List Params When PLATFORM_HAS_DYNMEN Disabled */
#ifndef PLATFORM_HAS_DYNMEN

    /* mqtt pub wait list max length, for QoS 1 */
    #ifndef IOTX_MC_PUBWAIT_LIST_MAX_LEN
        #define IOTX_MC_PUBWAIT_LIST_MAX_LEN            (5)
    #endif

    /* mqtt sub sync list max length */
    #ifndef IOTX_MC_SUBSYNC_LIST_MAX_LEN
        #define IOTX_MC_SUBSYNC_LIST_MAX_LEN            (5)
    #endif

    /* mqtt sub handle list max length */
    #ifndef IOTX_MC_SUBHANDLE_LIST_MAX_LEN
        #define IOTX_MC_SUBHANDLE_LIST_MAX_LEN          (5)
    #endif

    /* mqtt client max count */
    #ifndef IOTX_MC_CLIENT_MAX_COUNT
        #define IOTX_MC_CLIENT_MAX_COUNT                (1)
    #endif

    #ifndef IOTX_MC_TX_MAX_LEN
        #define IOTX_MC_TX_MAX_LEN                      (512)
    #endif

    #ifndef IOTX_MC_RX_MAX_LEN
        #define IOTX_MC_RX_MAX_LEN                      (512)
    #endif

#endif /* PLATFORM_HAS_DYNMEM */

#endif  /* IOTX_MQTT_CONFIG_H__ */


