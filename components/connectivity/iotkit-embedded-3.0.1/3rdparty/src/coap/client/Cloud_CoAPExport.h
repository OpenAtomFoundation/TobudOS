/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "Cloud_CoAPNetwork.h"
#include "iotx_coap_internal.h"

#ifndef CLOUD__COAP_EXPORT_H__
#define CLOUD__COAP_EXPORT_H__

/* #define COAP_DTLS_SUPPORT */
typedef CoAPMsgOption Cloud_CoAPMsgOption;
typedef CoAPMessageCode Cloud_CoAPMessageCode;
typedef CoAPMessage Cloud_CoAPMessage ;
#define COAP_OPTION_SEQ            2089

typedef void (*Cloud_CoAPRespMsgHandler)(void *data, void *message);

typedef struct {
    void                    *user;
    unsigned short           msgid;
    char                     acked;
    unsigned char            tokenlen;
    unsigned char            token[8];
    unsigned char            retrans_count;
    unsigned short           timeout;
    unsigned short           timeout_val;
    unsigned char           *message;
    unsigned int             msglen;
    Cloud_CoAPRespMsgHandler       resp;
    struct list_head         sendlist;
} Cloud_CoAPSendNode;


typedef struct {
    unsigned char            count;
    unsigned char            maxcount;
    struct list_head         sendlist;
} Cloud_CoAPSendList;


typedef void (*Cloud_CoAPEventNotifier)(unsigned int event, void *p_message);
typedef struct {
    char       *url;
    unsigned char        maxcount;  /*list maximal count*/
    unsigned int         waittime;
    Cloud_CoAPEventNotifier    notifier;
} Cloud_CoAPInitParam;

typedef struct {
    unsigned short           message_id;
    coap_network_t           network;
    Cloud_CoAPEventNotifier        notifier;
    unsigned char            *sendbuf;
    unsigned char            *recvbuf;
    Cloud_CoAPSendList             list;
    unsigned int             waittime;
} Cloud_CoAPContext;

#define COAP_TRC(...)     log_debug("coap_cloud", __VA_ARGS__)
#define COAP_DUMP(...)    log_debug("coap_cloud", __VA_ARGS__)
#define COAP_DEBUG(...)   log_debug("coap_cloud", __VA_ARGS__)
#define COAP_INFO(...)    log_info("coap_cloud", __VA_ARGS__)
#define COAP_WRN(...)     log_warning("coap_cloud", __VA_ARGS__)
#define COAP_ERR(...)     log_err("coap_cloud", __VA_ARGS__)

Cloud_CoAPContext *Cloud_CoAPContext_create(Cloud_CoAPInitParam *param);
void Cloud_CoAPContext_free(Cloud_CoAPContext *p_ctx);


#endif
