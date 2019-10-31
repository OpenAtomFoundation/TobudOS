/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MQTTCONNECT_H_
#define MQTTCONNECT_H_

#if !defined(DLLImport)
    #define DLLImport
#endif
#if !defined(DLLExport)
    #define DLLExport
#endif


#define MQTT_CONN_FLAG_USER_NAME        (0x80)
#define MQTT_CONN_FLAG_PASSWORD         (0x40)
#define MQTT_CONN_FLAG_WILL_RETAIN      (0x20)
#define MQTT_CONN_FLAG_WILL_QOS         (0x18)
#define MQTT_CONN_FLAG_WILL_FLAG        (0x04)
#define MQTT_CONN_FLAG_CLEAN_SESSION    (0x02)

typedef union {
    unsigned char all;  /**< all connect flags */
} MQTTConnectFlags; /**< connect flags byte */

/**
 * Defines the MQTT "Last Will and Testament" (LWT) settings for
 * the connect packet.
 */
typedef struct {
    /** The eyecatcher for this structure.  must be MQTW. */
    char struct_id[4];
    /** The version number of this structure.  Must be 0 */
    int struct_version;
    /** The LWT topic to which the LWT message will be published. */
    MQTTString topicName;
    /** The LWT payload. */
    MQTTString message;
    /**
      * The retained flag for the LWT message (see MQTTAsync_message.retained).
      */
    unsigned char retained;
    /**
      * The quality of service setting for the LWT message (see
      * MQTTAsync_message.qos and @ref qos).
      */
    char qos;
} MQTTPacket_willOptions;


#define MQTTPacket_willOptions_initializer { {'M', 'Q', 'T', 'W'}, 0, {NULL, {0, NULL}}, {NULL, {0, NULL}}, 0, 0 }


typedef struct {
    /** The eyecatcher for this structure.  must be MQTC. */
    char struct_id[4];
    /** The version number of this structure.  Must be 0 */
    int struct_version;
    /** Version of MQTT to be used.  3 = 3.1 4 = 3.1.1
      */
    unsigned char MQTTVersion;
    MQTTString clientID;
    unsigned short keepAliveInterval; /* 单位s */
    unsigned char cleansession;
    unsigned char willFlag;
    MQTTPacket_willOptions will;
    MQTTString username;
    MQTTString password;
} MQTTPacket_connectData;

typedef union {
    unsigned char all;  /**< all connack flags */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    struct {
        unsigned int sessionpresent : 1;    /**< session present flag */
        unsigned int : 7;                 /**< unused */
    } bits;
#else
    struct {
        unsigned int : 7;                   /**< unused */
        unsigned int sessionpresent : 1;    /**< session present flag */
    } bits;
#endif
} MQTTConnackFlags; /**< connack flags byte */

#define MQTTPacket_connectData_initializer { {'M', 'Q', 'T', 'C'}, 0, 4, {NULL, {0, NULL}}, CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN, 1, 0, \
        MQTTPacket_willOptions_initializer, {NULL, {0, NULL}}, {NULL, {0, NULL}} }

DLLExport int MQTTSerialize_connect(unsigned char *buf, int buflen, MQTTPacket_connectData *options);
DLLExport int MQTTDeserialize_connect(MQTTPacket_connectData *data, unsigned char *buf, int len);

DLLExport int MQTTSerialize_connack(unsigned char *buf, int buflen, unsigned char connack_rc,
                                    unsigned char sessionPresent);
DLLExport int MQTTDeserialize_connack(unsigned char *sessionPresent, unsigned char *connack_rc, unsigned char *buf,
                                      int buflen);

DLLExport int MQTTSerialize_disconnect(unsigned char *buf, int buflen);
DLLExport int MQTTSerialize_pingreq(unsigned char *buf, int buflen);

#endif /* MQTTCONNECT_H_ */


