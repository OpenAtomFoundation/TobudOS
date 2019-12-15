/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __COAP_SERVER_H__
#define __COAP_SERVER_H__

#include "CoAPExport.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define COAP_SERV_MULTITHREAD

CoAPContext *CoAPServer_init();

void CoAPServer_add_timer (void (*on_timer)(void*));
void CoAPServer_loop(CoAPContext *context);

void CoAPServer_deinit(CoAPContext *context);

int CoAPServer_register(CoAPContext *context, const char *uri, CoAPRecvMsgHandler callback);

int CoAPServerMultiCast_send(CoAPContext *context, NetworkAddr *remote, const char *uri,
        unsigned char *buff, unsigned short len, CoAPSendMsgHandler callback, unsigned short *msgid);

int CoAPServerResp_send(CoAPContext *context, NetworkAddr *remote, unsigned char *buff, unsigned short len, void *req,
        const char *paths, CoAPSendMsgHandler callback, unsigned short *msgid, char qos);

void CoAPServer_thread_leave();
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
