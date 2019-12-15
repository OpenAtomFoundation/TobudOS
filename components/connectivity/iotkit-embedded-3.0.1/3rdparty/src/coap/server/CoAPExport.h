/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_EXPORT_H__
#define __COAP_EXPORT_H__

#include "../iotx_coap_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    unsigned char        send_maxcount;  /*list maximal count*/
    unsigned char        obs_maxcount;   /*observe maximal count*/
    unsigned short       port;           /* Local port */
    char                 *group;         /* Multicast address */
    unsigned int         waittime;
    CoAPEventNotifier    notifier;
    void                 *appdata;
    unsigned char        res_maxcount;
} CoAPInitParam;

typedef enum {
    PATH_NORMAL,
    PATH_FILTER,
} path_type_t;

CoAPContext *CoAPContext_create(CoAPInitParam *param);

void CoAPContext_free(CoAPContext *context);

void *CoAPContextAppdata_get(CoAPContext *context);

/* CoAP message options APIs*/
extern int CoAPStrOption_add(CoAPMessage *message, unsigned short optnum,
                             unsigned char *data, unsigned short datalen);

extern int CoAPStrOption_get(CoAPMessage *message, unsigned short optnum,
                             unsigned char *data, unsigned short *datalen);

extern int CoAPUintOption_add(CoAPMessage *message, unsigned short  optnum,
                              unsigned int data);

extern int CoAPUintOption_get(CoAPMessage *message,
                              unsigned short  optnum,
                              unsigned int *data);

extern int CoAPOption_present(CoAPMessage *message, unsigned short option);


/*CoAP Message APIs*/
extern unsigned short CoAPMessageId_gen(CoAPContext *context);

extern int CoAPMessageId_set(CoAPMessage *message, unsigned short msgid);

extern int CoAPMessageType_set(CoAPMessage *message, unsigned char type);

extern int CoAPMessageCode_set(CoAPMessage *message, CoAPMessageCode code);

extern int CoAPMessageCode_get(CoAPMessage *message, CoAPMessageCode *code);

extern int CoAPMessageToken_set(CoAPMessage *message, unsigned char *token,
                                unsigned char tokenlen);

extern int CoAPMessageUserData_set(CoAPMessage *message, void *userdata);

extern int CoAPMessageKeep_Set(CoAPMessage *message, int keep);

extern int CoAPMessagePayload_set(CoAPMessage *message, unsigned char *payload,
                                  unsigned short payloadlen);

extern int CoAPMessageHandler_set(CoAPMessage *message, CoAPSendMsgHandler handler);

extern int CoAPMessage_init(CoAPMessage *message);

extern int CoAPMessage_destory(CoAPMessage *message);

extern int CoAPMessage_send(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message);

extern int CoAPMessage_process(CoAPContext *context, unsigned int timeout);

extern int CoAPMessage_retransmit(CoAPContext *context);

extern int CoAPMessage_cycle(CoAPContext *context);

extern int CoAPMessage_cancel(CoAPContext *context, CoAPMessage *message);

extern int CoAPMessageId_cancel(CoAPContext *context, unsigned short msgid);

extern void CoAPMessage_dump(NetworkAddr *remote, CoAPMessage *message);
/*CoAP Resource APIs*/
extern int CoAPResource_register(CoAPContext *context, const char *path,
                                 unsigned short permission, unsigned int ctype,
                                 unsigned int maxage, CoAPRecvMsgHandler callback);

/*CoAP observe APIs*/
extern int CoAPObsServer_add(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *request);

extern int CoAPObsServer_notify(CoAPContext *context,
                                const char *path, unsigned char *payload,
                                unsigned short payloadlen, CoAPDataEncrypt handler);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
