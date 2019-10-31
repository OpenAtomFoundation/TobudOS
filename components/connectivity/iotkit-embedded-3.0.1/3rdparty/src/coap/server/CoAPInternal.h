/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __COAP_INTERNAL_H__
#define __COAP_INTERNAL_H__
#include "CoAPNetwork.h"
#include "CoAPExport.h"
#include "iotx_coap_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct
{
    void                    *list_mutex;
    struct list_head         list;
    unsigned char            count;
    unsigned char            maxcount;
}CoAPList;


typedef struct
{
    unsigned short           message_id;
    NetworkContext           *p_network;
    CoAPEventNotifier        notifier;
    unsigned char            *sendbuf;
    unsigned char            *recvbuf;
    CoAPList                 sendlist;
    CoAPList                 obsserver;
    CoAPList                 obsclient;
    CoAPList                 resource;
    unsigned int             waittime;
    void                     *appdata;
    void                     *mutex;
}CoAPIntContext;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
