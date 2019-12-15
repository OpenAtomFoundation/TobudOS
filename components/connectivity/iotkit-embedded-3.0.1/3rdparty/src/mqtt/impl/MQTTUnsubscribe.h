/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MQTTUNSUBSCRIBE_H_
#define MQTTUNSUBSCRIBE_H_

#if !defined(DLLImport)
    #define DLLImport
#endif
#if !defined(DLLExport)
    #define DLLExport
#endif

DLLExport int MQTTSerialize_unsubscribe(unsigned char *buf, int buflen, unsigned char dup, unsigned short packetid,
                                        int count, MQTTString topicFilters[]);

DLLExport int MQTTDeserialize_unsubscribe(unsigned char *dup, unsigned short *packetid, int max_count, int *count,
        MQTTString topicFilters[],
        unsigned char *buf, int len);

DLLExport int MQTTSerialize_unsuback(unsigned char *buf, int buflen, unsigned short packetid);

DLLExport int MQTTDeserialize_unsuback(unsigned short *packetid, unsigned char *buf, int len);

#endif /* MQTTUNSUBSCRIBE_H_ */


