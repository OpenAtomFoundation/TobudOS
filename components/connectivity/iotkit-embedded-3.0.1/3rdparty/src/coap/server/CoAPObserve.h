/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef __COAP_OBSERVE_H__
#define __COAP_OBSERVE_H__
#include "CoAPExport.h"
#include "CoAPMessage.h"
#include "CoAPResource.h"
#include "iotx_coap_internal.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct
{
    NetworkAddr              remote;
    unsigned char            token[COAP_MSG_MAX_TOKEN_LEN];
    unsigned char            tokenlen;
    unsigned char            ctype;
    CoAPResource             *p_resource_of_interest;
    unsigned int             observer_sequence_num;
    CoAPMessageCode          msg_type;
    struct list_head         obslist;
} CoapObserver;

typedef struct
{
    NetworkAddr            remote;
    unsigned char          token[COAP_MSG_MAX_TOKEN_LEN];
    unsigned char          tokenlen;
    CoAPSendMsgHandler     callback;
    unsigned int           max_age;
    struct list_head       obslist;
    void                   *userdata;
} CoAPObservable;

int CoAPObsServer_init(CoAPContext *context, unsigned char  obs_maxcount);
int CoAPObsServer_deinit(CoAPContext *context);

int CoAPObsServer_add(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *request);
int CoapObsServer_delete(CoAPContext *context,          NetworkAddr  *remote,
                                      CoAPResource *resource);
int CoapObsServerAll_delete(CoAPContext *context,          NetworkAddr  *remote);

int CoAPObsServer_notify(CoAPContext *context,
                            const char *path, unsigned char *payload,
                            unsigned short payloadlen, CoAPDataEncrypt handler);

int CoAPObsClient_init(CoAPContext *context, unsigned char  obs_maxcount);
int CoAPObsClient_deinit(CoAPContext *context);
int CoAPObsClient_add(CoAPContext *context, CoAPMessage *message, NetworkAddr *remote, CoAPSendNode *sendnode);
int CoAPObsClient_delete(CoAPContext *context, CoAPMessage *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
