/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MQTTPACKET_H_
#define MQTTPACKET_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#if defined(WIN32_DLL) || defined(WIN64_DLL)
#define DLLImport __declspec(dllimport)
#define DLLExport __declspec(dllexport)
#elif defined(LINUX_SO)
#define DLLImport extern
#define DLLExport  __attribute__ ((visibility ("default")))
#else
#define DLLImport
#define DLLExport
#endif

enum errors {
    MQTTPACKET_BUFFER_TOO_SHORT = -2,
    MQTTPACKET_READ_ERROR = -1,
    MQTTPACKET_READ_COMPLETE
};


/* CPT, control packet type */
enum msgTypes {
    MQTT_CPT_RESERVED = 0, CONNECT = 1, CONNACK, PUBLISH, PUBACK, PUBREC, PUBREL,
    PUBCOMP, SUBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK,
    PINGREQ, PINGRESP, DISCONNECT
};

#define MQTT_HEADER_BIT_MASK_TYPE               (0xF0)
#define MQTT_HEADER_BIT_MASK_DUP                (0x08)
#define MQTT_HEADER_BIT_MASK_QOS                (0x06)
#define MQTT_HEADER_BIT_MASK_RETAIN             (0x01)

#define MQTT_HEADER_GET_TYPE(head)              ((head & 0xF0) >> 4)
#define MQTT_HEADER_GET_DUP(head)               ((head & 0x08) >> 3)
#define MQTT_HEADER_GET_QOS(head)               ((head & 0x06) >> 1)
#define MQTT_HEADER_GET_RETAIN(head)            (head & 0x01)

#define MQTT_HEADER_SET_TYPE(head, type)        do {head |= ((type << 4) & 0xF0); } while (0)
#define MQTT_HEADER_SET_DUP(head, dup)          do {head |= ((dup << 3) & 0x08); } while (0)
#define MQTT_HEADER_SET_QOS(head, qos)          do {head |= ((qos << 1) & 0x06); } while (0)
#define MQTT_HEADER_SET_RETAIN(head, retain)    do {head |= (retain & 0x01); } while (0)

/**
 * Bitfields for the MQTT header byte.
 */
typedef union
{
	unsigned char byte;	                /**< the whole byte */
} MQTTHeader;

typedef struct {
    int len;
    char *data;
} MQTTLenString;

typedef struct {
    char *cstring;
    MQTTLenString lenstring;
} MQTTString;

#define MQTTString_initializer {NULL, {0, NULL}}

int MQTTstrlen(MQTTString mqttstring);

#include "MQTTConnect.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"

int MQTTSerialize_ack(unsigned char *buf, int buflen, unsigned char type, unsigned char dup, unsigned short packetid);
int MQTTDeserialize_ack(unsigned char *packettype, unsigned char *dup, unsigned short *packetid, unsigned char *buf,
                        int buflen);

int MQTTPacket_len(int rem_len);
int MQTTPacket_equals(MQTTString *a, char *b);

int MQTTPacket_encode(unsigned char *buf, int length);
int MQTTPacket_decode(int (*getcharfn)(unsigned char *, int), int *value);
int MQTTPacket_decodeBuf(unsigned char *buf, int *value);

int readInt(unsigned char **pptr);
char readChar(unsigned char **pptr);
void writeChar(unsigned char **pptr, char c);
void writeInt(unsigned char **pptr, int anInt);
int readMQTTLenString(MQTTString *mqttstring, unsigned char **pptr, unsigned char *enddata);
void writeCString(unsigned char **pptr, const char *string);
void writeMQTTString(unsigned char **pptr, MQTTString mqttstring);

typedef struct {
    int (*getfn)(void *, unsigned char *,
                 int); /* must return -1 for error, 0 for call again, or the number of bytes read */
    void *sck;  /* pointer to whatever the system may use to identify the transport */
    int multiplier;
    int rem_len;
    int len;
    char state;
} MQTTTransport;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif


#endif /* MQTTPACKET_H_ */


