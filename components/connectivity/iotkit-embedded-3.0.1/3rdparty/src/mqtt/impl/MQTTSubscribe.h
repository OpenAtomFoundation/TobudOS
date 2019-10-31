/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MQTTSUBSCRIBE_H_
#define MQTTSUBSCRIBE_H_

#if !defined(DLLImport)
    #define DLLImport
#endif
#if !defined(DLLExport)
    #define DLLExport
#endif

DLLExport int MQTTSerialize_subscribe(unsigned char *buf, int buflen, unsigned char dup, unsigned short packetid,
                                      int count, MQTTString topicFilters[], int requestedQoSs[]);

DLLExport int MQTTDeserialize_subscribe(unsigned char *dup, unsigned short *packetid,
                                        int maxcount, int *count, MQTTString topicFilters[], int requestedQoSs[], unsigned char *buf, int len);

DLLExport int MQTTSerialize_suback(unsigned char *buf, int buflen, unsigned short packetid, int count,
                                   int *grantedQoSs);

DLLExport int MQTTDeserialize_suback(unsigned short *packetid, int maxcount, int *count, int grantedQoSs[],
                                     unsigned char *buf, int len);


#endif /* MQTTSUBSCRIBE_H_ */


