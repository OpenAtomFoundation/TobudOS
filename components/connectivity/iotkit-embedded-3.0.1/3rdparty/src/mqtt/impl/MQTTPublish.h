/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MQTTPUBLISH_H_
#define MQTTPUBLISH_H_

#if !defined(DLLImport)
    #define DLLImport
#endif
#if !defined(DLLExport)
    #define DLLExport
#endif

DLLExport int MQTTSerialize_publish(unsigned char *buf, int buflen, unsigned char dup, int qos, unsigned char retained,
                                    unsigned short packetid,
                                    MQTTString topicName, unsigned char *payload, int payloadlen);

DLLExport int MQTTDeserialize_publish(unsigned char *dup, int *qos, unsigned char *retained, unsigned short *packetid,
                                      MQTTString *topicName,
                                      unsigned char **payload, int *payloadlen, unsigned char *buf, int len);

/*  DLLExport int MQTTSerialize_puback(unsigned char* buf, int buflen, unsigned short packetid); */
DLLExport int MQTTSerialize_pubrel(unsigned char *buf, int buflen, unsigned char dup, unsigned short packetid);
DLLExport int MQTTSerialize_pubcomp(unsigned char *buf, int buflen, unsigned short packetid);

#endif /* MQTTPUBLISH_H_ */


