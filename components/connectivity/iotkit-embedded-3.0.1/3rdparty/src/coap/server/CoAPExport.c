/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"
#include "iotx_coap_internal.h"
#include "CoAPPlatform.h"
#include "CoAPInternal.h"
#include "CoAPNetwork.h"
#include "CoAPExport.h"
#include "CoAPObserve.h"

#define COAP_DEFAULT_PORT           5683 /* CoAP default UDP port */
#define COAPS_DEFAULT_PORT          5684 /* CoAP default UDP port for secure transmission */
#define COAP_DEFAULT_SENDLIST_MAXCOUNT  8
#define COAP_DEFAULT_RES_MAXCOUNT       8
#define COAP_DEFAULT_OBS_MAXCOUNT       8

#define COAP_DEFAULT_SCHEME         "coap" /* the default scheme for CoAP URIs */
#define COAP_DEFAULT_HOST_LEN       128
#define COAP_DEFAULT_WAIT_TIME_MS   2000


CoAPContext *CoAPContext_create(CoAPInitParam *param)
{
    CoAPIntContext    *p_ctx = NULL;
    NetworkInit    network_param;

    memset(&network_param, 0x00, sizeof(NetworkInit));
    p_ctx = coap_malloc(sizeof(CoAPIntContext));
    if (NULL == p_ctx) {
        COAP_ERR("malloc for coap context failed");
        goto err;
    }
    COAP_DEBUG("Send List Max-Count:      %d", param->send_maxcount);
    COAP_DEBUG("Observe Server Max-Count: %d", param->obs_maxcount);
    COAP_DEBUG("Observe Client Max-Count: %d", param->obs_maxcount);
    COAP_DEBUG("Resource Max-Count:       %d", param->res_maxcount);
    COAP_DEBUG("MultiCast Address:        %s:%d", param->group, param->port);
    COAP_DEBUG("Send/Recv Wait time:      %dms", param->waittime);

    memset(p_ctx, 0, sizeof(CoAPIntContext));
    p_ctx->message_id = 1;
    p_ctx->notifier = param->notifier;
    p_ctx->appdata = param->appdata;

#ifdef USE_SENDBUFF
    p_ctx->sendbuf = coap_malloc(COAP_MSG_MAX_PDU_LEN);
    if (NULL == p_ctx->sendbuf) {
        COAP_ERR("not enough memory");
        goto err;
    }
    memset(p_ctx->sendbuf, 0x00, COAP_MSG_MAX_PDU_LEN);
#endif

    p_ctx->recvbuf = coap_malloc(COAP_MSG_MAX_PDU_LEN);
    if (NULL == p_ctx->recvbuf) {
        COAP_ERR("not enough memory");
        goto err;
    }
    memset(p_ctx->recvbuf, 0x00, COAP_MSG_MAX_PDU_LEN);

    if (0 == param->waittime) {
        p_ctx->waittime = COAP_DEFAULT_WAIT_TIME_MS;
    } else {
        p_ctx->waittime = param->waittime;
    }
    p_ctx->mutex = HAL_MutexCreate();
    if (NULL == p_ctx->mutex) {
        COAP_ERR("Mutex Create failed");
        goto err;
    }

    /*Init message send list mutex*/
    p_ctx->sendlist.list_mutex = HAL_MutexCreate();
    HAL_MutexLock(p_ctx->sendlist.list_mutex);
    /*CoAP message send list*/
    INIT_LIST_HEAD(&p_ctx->sendlist.list);
    p_ctx->sendlist.count = 0;
    HAL_MutexUnlock(p_ctx->sendlist.list_mutex);

    if (0 != param->send_maxcount) {
        p_ctx->sendlist.maxcount = param->send_maxcount;
    } else {
        p_ctx->sendlist.maxcount = COAP_DEFAULT_SENDLIST_MAXCOUNT;
    }

    if (0 == param->res_maxcount) {
        param->res_maxcount = COAP_DEFAULT_RES_MAXCOUNT;
    }
    CoAPResource_init(p_ctx, param->res_maxcount);

#ifndef COAP_OBSERVE_SERVER_DISABLE
    if (0 == param->obs_maxcount) {
        param->obs_maxcount = COAP_DEFAULT_OBS_MAXCOUNT;
    }
    CoAPObsServer_init(p_ctx, param->obs_maxcount);
#endif

#ifndef COAP_OBSERVE_CLIENT_DISABLE
    if (0 == param->obs_maxcount) {
        param->obs_maxcount = COAP_DEFAULT_OBS_MAXCOUNT;
    }
    CoAPObsClient_init(p_ctx, param->obs_maxcount);
#endif

#ifdef COAP_DTLS_SUPPORT
    network_param.type = COAP_NETWORK_DTLS;
    network_param.port = COAPS_DEFAULT_PORT;
#else
    network_param.type = COAP_NETWORK_NOSEC;
    network_param.port = param->port;
    network_param.group = param->group;
#endif


    /*CoAP network init*/
    p_ctx->p_network = CoAPNetwork_init(&network_param);

    if (NULL == p_ctx->p_network) {
        COAP_ERR("CoAP Network init failed, exit");
        goto err;
    }

    return p_ctx;
err:
    if (NULL == p_ctx) {
        return p_ctx;
    }

    if (NULL != p_ctx->recvbuf) {
        coap_free(p_ctx->recvbuf);
        p_ctx->recvbuf = NULL;
    }

#ifdef USE_SENDBUFF
    if (NULL != p_ctx->sendbuf) {
        coap_free(p_ctx->sendbuf);
        p_ctx->sendbuf = NULL;
    }
#endif

#ifndef COAP_OBSERVE_SERVER_DISABLE
    CoAPObsServer_deinit(p_ctx);
#endif

#ifndef COAP_OBSERVE_CLIENT_DISABLE
    CoAPObsClient_deinit(p_ctx);
#endif

    CoAPResource_deinit(p_ctx);

    if (NULL != p_ctx->sendlist.list_mutex) {
        HAL_MutexDestroy(p_ctx->sendlist.list_mutex);
        p_ctx->sendlist.list_mutex = NULL;
    }

    if (NULL != p_ctx->mutex) {
        HAL_MutexDestroy(p_ctx->mutex);
        p_ctx->mutex = NULL;
    }

    coap_free(p_ctx);
    p_ctx = NULL;

    /* TODO: release the resource */
    return (CoAPContext *)p_ctx;
}

void *CoAPContextAppdata_get(CoAPContext *context)
{
    CoAPIntContext *p_ctx = (CoAPIntContext *)context;
    if (NULL == p_ctx) {
        return NULL;
    }

    return (void *)p_ctx->appdata;
}


void CoAPContext_free(CoAPContext *context)
{
    CoAPIntContext *p_ctx = NULL;
    CoAPSendNode *cur = NULL, *next = NULL;
    if (NULL == context) {
        return;
    }

    p_ctx = (CoAPIntContext *)context;

    CoAPNetwork_deinit(p_ctx->p_network);
    COAP_DEBUG("CoAP Network Deinit");

    HAL_MutexLock(p_ctx->sendlist.list_mutex);
    list_for_each_entry_safe(cur, next, &p_ctx->sendlist.list, sendlist, CoAPSendNode) {
        if (NULL != cur) {
            if (NULL != cur->message) {
                coap_free(cur->message);
                cur->message = NULL;
            }
            coap_free(cur);
            cur = NULL;
        }
    }
    INIT_LIST_HEAD(&p_ctx->sendlist.list);
    HAL_MutexUnlock(p_ctx->sendlist.list_mutex);
    HAL_MutexDestroy(p_ctx->sendlist.list_mutex);
    p_ctx->sendlist.list_mutex = NULL;
    HAL_MutexDestroy(p_ctx->mutex);
    p_ctx->mutex = NULL;
    COAP_DEBUG("Release Send List and Memory");

#ifndef COAP_OBSERVE_SERVER_DISABLE
    CoAPObsServer_deinit(p_ctx);
    COAP_DEBUG("CoAP Observe Server Deinit");
#endif


#ifndef COAP_OBSERVE_CLIENT_DISABLE
    CoAPObsClient_deinit(p_ctx);
    COAP_DEBUG("CoAP Observe Client Deinit");
#endif

    CoAPResource_deinit(p_ctx);
    COAP_DEBUG("CoAP Resource unregister");

    if (NULL != p_ctx->recvbuf) {
        coap_free(p_ctx->recvbuf);
        p_ctx->recvbuf = NULL;
        COAP_DEBUG("Release The Recv Memory");
    }
#ifdef USE_SENDBUFF
    if (NULL != p_ctx->sendbuf) {
        coap_free(p_ctx->sendbuf);
        p_ctx->sendbuf = NULL;
        COAP_DEBUG("Release The Send Memory");
    }
#endif

    if (NULL != p_ctx) {
        coap_free(p_ctx);
        p_ctx    =  NULL;
        COAP_DEBUG("Release The CoAP Context");
    }
}
