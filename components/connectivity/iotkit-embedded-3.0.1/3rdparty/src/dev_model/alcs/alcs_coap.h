/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "CoAPExport.h"

#ifndef __COAP_ALCS_H__
#define __COAP_ALCS_H__

#define OPTSESESSION 62
#define ALCSPORT 5683
#define ALCSPORT_SECURE 5684
#define MULTICAST_ADDRESS "224.0.1.187"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef bool
#define bool char
#endif

typedef struct {
    NetworkAddr addr;
    char *path;
    char *query;
} ResourceAddr;

/* 会自动生成msgid & token */
void alcs_msg_init(CoAPContext *ctx, CoAPMessage *message, int code, unsigned char type, int keep,
                   CoAPLenString *payload, void *userdata);
void alcs_msg_deinit(CoAPMessage *message);
int alcs_msg_setAddr(CoAPMessage *message, const char *path, const char *query);

/* observe: 0 register */
/* observer:1 deregister */
/* observer:other 没意义 */
int alcs_sendmsg(CoAPContext *ctx, NetworkAddr *addr, CoAPMessage *message, char observe, CoAPSendMsgHandler handler);

/* msgid & token从接收到CoAPMessage获取, 若发送的是事件通知，msgid设置为0 */
/* observe: 0： accept register */
/* observe: other: 没意义   */
int alcs_sendrsp(CoAPContext *ctx, NetworkAddr *addr, CoAPMessage *message, char observe, unsigned short msgid,
                 CoAPLenString *token);

void alcs_start_loop(CoAPContext *ctx, int newThread);
void alcs_stop_loop(CoAPContext *ctx);

/* 服务端接口
 *
 */
/* observe */
int alcs_observe_notify(CoAPContext *context, const char *path, CoAPLenString *payload);

/* resource */
int alcs_resource_register(CoAPContext *context, const char *pk, const char *dn, const char *path,
                           unsigned short permission,
                           unsigned int ctype, unsigned int maxage, char needAuth, CoAPRecvMsgHandler callback);

int alcs_resource_need_auth(CoAPContext *context, const char *path);


/* init */
void alcs_init();
void alcs_deinit();

#ifdef SUPPORT_MULTI_DEVICES
CoAPContext *alcs_context_create(CoAPInitParam *param);
void alcs_context_free(CoAPContext *ctx);
#else
CoAPContext *alcs_context_init(CoAPInitParam *param);
void alcs_context_deinit();
CoAPContext *alcs_get_context();
#endif

/* option  */
extern int CoAPUintOption_add(CoAPMessage *message, unsigned short  optnum, unsigned int data);
extern int CoAPUintOption_get(CoAPMessage *message, unsigned short  optnum, unsigned int *data);
extern int CoAPStrOption_add(CoAPMessage *message, unsigned short optnum,
                             unsigned char *data, unsigned short datalen);
extern int CoAPMessage_cancel(CoAPContext *context, CoAPMessage *message);

uint32_t getToken();

/*  */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
