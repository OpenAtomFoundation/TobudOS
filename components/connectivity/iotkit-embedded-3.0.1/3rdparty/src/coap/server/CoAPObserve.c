/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "CoAPExport.h"
#include "CoAPResource.h"
#include "CoAPObserve.h"
#include "CoAPMessage.h"
#include "iotx_coap_internal.h"
#include "CoAPPlatform.h"
#include "CoAPInternal.h"

#ifndef COAP_OBSERVE_SERVER_DISABLE
int CoAPObsServer_init(CoAPContext *context, unsigned char        obs_maxcount)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    ctx->obsserver.list_mutex = HAL_MutexCreate();

    HAL_MutexLock(ctx->obsserver.list_mutex);
    INIT_LIST_HEAD(&ctx->obsserver.list);
    ctx->obsserver.count = 0;
    ctx->obsserver.maxcount = obs_maxcount;
    HAL_MutexUnlock(ctx->obsserver.list_mutex);

    return COAP_SUCCESS;
}

int CoAPObsServer_deinit(CoAPContext *context)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;
    CoapObserver *node = NULL, *next = NULL;

    HAL_MutexLock(ctx->obsserver.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->obsserver.list, obslist, CoapObserver) {
        list_del(&node->obslist);
        COAP_DEBUG("Delete %s:%d from observe server", node->remote.addr, node->remote.port);
        coap_free(node);
    }
    ctx->obsserver.count = 0;
    ctx->obsserver.maxcount = 0;
    HAL_MutexUnlock(ctx->obsserver.list_mutex);

    HAL_MutexDestroy(ctx->obsserver.list_mutex);
    ctx->obsserver.list_mutex = NULL;

    return COAP_SUCCESS;
}


int CoAPObsServer_add(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *request)
{
    int ret = COAP_SUCCESS;
    unsigned int observe;
    unsigned int  acceptype = 0;
    CoapObserver *obs = NULL;
    CoAPResource *resource = NULL;
    CoapObserver *node = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    resource = CoAPResourceByPath_get(ctx, path);

    ret = CoAPUintOption_get(request, COAP_OPTION_OBSERVE,  &observe);

    if (NULL != resource && COAP_SUCCESS == ret && 0 == observe) {
        /*Check if the observe client already exist*/
        HAL_MutexLock(ctx->obsserver.list_mutex);
        list_for_each_entry(node, &ctx->obsserver.list, obslist, CoapObserver) {
            if ((node->p_resource_of_interest == resource) &&
                (node->remote.port == remote->port)  &&
                (0 == memcmp(node->remote.addr, remote->addr, NETWORK_ADDR_LEN))) {
                COAP_DEBUG("The observe client %s:%d already exist,update it", node->remote.addr, node->remote.port);
                memcpy(node->token, request->token, request->header.tokenlen);
                node->tokenlen = request->header.tokenlen;
                HAL_MutexUnlock(ctx->obsserver.list_mutex);
                return COAP_ERROR_OBJ_ALREADY_EXIST;
            }
        }
        HAL_MutexUnlock(ctx->obsserver.list_mutex);


        obs = coap_malloc(sizeof(CoapObserver));
        if (NULL == obs) {
            COAP_ERR("Allocate memory failed");
            return COAP_ERROR_MALLOC;
        }

        memset(obs, 0x00, sizeof(CoapObserver));
        obs->msg_type = request->header.type;
        obs->p_resource_of_interest = resource;
        memcpy(&obs->remote, remote, sizeof(NetworkAddr));
        memcpy(obs->token, request->token, request->header.tokenlen);
        obs->tokenlen = request->header.tokenlen;


        CoAPUintOption_get(request, COAP_OPTION_ACCEPT, &acceptype);
        obs->ctype = (acceptype == 0) ? COAP_CT_APP_JSON : acceptype;
        obs->observer_sequence_num = 0;

        /* TODO: */
        /* CoAPObsServer_find(); */

        HAL_MutexLock(ctx->obsserver.list_mutex);
        if (ctx->obsserver.count >= ctx->obsserver.maxcount) {
            HAL_MutexUnlock(ctx->obsserver.list_mutex);
            coap_free(obs);
            COAP_INFO("Cur have %d observer, max allow %d", ctx->obsserver.count, ctx->obsserver.maxcount);
            return COAP_ERROR_DATA_SIZE;
        } else {
            list_add_tail(&obs->obslist, &ctx->obsserver.list);
            ctx->obsserver.count ++;
            COAP_DEBUG("Create a observe node, cur have %d nodes", ctx->obsserver.count);
            HAL_MutexUnlock(ctx->obsserver.list_mutex);
            return COAP_SUCCESS;
        }

    }

    return COAP_ERROR_NOT_FOUND;
}


int CoapObsServer_delete(CoAPContext *context,          NetworkAddr  *remote,
                         CoAPResource *resource)
{
    CoapObserver *node = NULL, *next = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    HAL_MutexLock(ctx->obsserver.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->obsserver.list, obslist, CoapObserver) {
        if ((node->p_resource_of_interest == resource) &&
            (node->remote.port == remote->port)  &&
            (0 == memcmp(node->remote.addr, remote->addr, NETWORK_ADDR_LEN))) {
            ctx->obsserver.count --;
            list_del(&node->obslist);
            COAP_DEBUG("Delete %s:%d from observe server", node->remote.addr, node->remote.port);
            coap_free(node);
            break;
        }
    }
    HAL_MutexUnlock(ctx->obsserver.list_mutex);

    return COAP_SUCCESS;
}

int CoapObsServerAll_delete(CoAPContext *context,          NetworkAddr  *remote)
{
    CoapObserver *node = NULL, *next = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    HAL_MutexLock(ctx->obsserver.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->obsserver.list, obslist, CoapObserver) {
        if (NULL != node && (node->remote.port == remote->port)  &&
            (0 == memcmp(node->remote.addr, remote->addr, NETWORK_ADDR_LEN))) {
            ctx->obsserver.count --;
            list_del(&node->obslist);
            COAP_DEBUG("Delete %s:%d from observe server, cur observe count %d",
                       node->remote.addr, node->remote.port, ctx->obsserver.count);
            coap_free(node);
        }
    }
    HAL_MutexUnlock(ctx->obsserver.list_mutex);

    return COAP_SUCCESS;
}


int CoAPObsServer_notify(CoAPContext *context,
                         const char *path, unsigned char *payload,
                         unsigned short payloadlen, CoAPDataEncrypt handler)
{
    unsigned int ret  = COAP_SUCCESS;
    CoAPResource *resource = NULL;
    CoapObserver *node     = NULL;
    CoAPLenString src;
    CoAPLenString dest;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    resource = CoAPResourceByPath_get(ctx, path);

    if (NULL != resource) {
        HAL_MutexLock(ctx->obsserver.list_mutex);
        list_for_each_entry(node, &ctx->obsserver.list, obslist, CoapObserver) {
            if (node->p_resource_of_interest == resource) {
                CoAPMessage message;
                CoAPMessage_init(&message);
                CoAPMessageType_set(&message, node->msg_type);
                CoAPMessageCode_set(&message, COAP_MSG_CODE_205_CONTENT);
                CoAPMessageId_set(&message, CoAPMessageId_gen(ctx));
                CoAPMessageHandler_set(&message, NULL);
                CoAPMessageUserData_set(&message, node->p_resource_of_interest);
                CoAPMessageToken_set(&message, node->token, node->tokenlen);
                CoAPUintOption_add(&message, COAP_OPTION_OBSERVE, node->observer_sequence_num++);
                CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, node->ctype);
                CoAPUintOption_add(&message, COAP_OPTION_MAXAGE, resource->maxage);
                COAP_DEBUG("Send notify message path %s to remote %s:%d ",
                           path, node->remote.addr, node->remote.port);

                memset(&dest, 0x00, sizeof(CoAPLenString));
                if (NULL != handler) {
                    src.len = payloadlen;
                    src.data = payload;
                    ret = handler(context, path, &node->remote, &message, &src, &dest);
                    if (COAP_SUCCESS == ret) {
                        CoAPMessagePayload_set(&message, dest.data, dest.len);
                    } else {
                        COAP_INFO("Encrypt payload failed");
                    }
                } else {
                    CoAPMessagePayload_set(&message, payload, payloadlen);
                }
                ret = CoAPMessage_send(ctx, &node->remote, &message);
                if (NULL != handler && 0 != dest.len && NULL != dest.data) {
                    coap_free(dest.data);
                    dest.len = 0;
                }
                CoAPMessage_destory(&message);
            }
        }

        HAL_MutexUnlock(ctx->obsserver.list_mutex);
    }
    return ret;
}

#endif

#ifndef COAP_OBSERVE_CLIENT_DISABLE
int CoAPObsClient_init(CoAPContext *context, unsigned char        obs_maxcount)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    ctx->obsclient.list_mutex = HAL_MutexCreate();

    HAL_MutexLock(ctx->obsclient.list_mutex);
    INIT_LIST_HEAD(&ctx->obsclient.list);
    ctx->obsclient.count = 0;
    ctx->obsclient.maxcount = obs_maxcount;
    HAL_MutexUnlock(ctx->obsclient.list_mutex);

    return COAP_SUCCESS;
}

int CoAPObsClient_deinit(CoAPContext *context)
{
    CoAPObservable *node = NULL, *next =  NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    HAL_MutexLock(ctx->obsclient.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->obsclient.list, obslist, CoAPObservable) {
        list_del(&node->obslist);
        coap_free(node);
    }
    ctx->obsclient.count = 0;
    ctx->obsclient.maxcount = 0;
    HAL_MutexUnlock(ctx->obsclient.list_mutex);

    HAL_MutexDestroy(ctx->obsclient.list_mutex);
    ctx->obsclient.list_mutex = NULL;
    return COAP_SUCCESS;
}

int CoAPObsClient_add(CoAPContext *context, CoAPMessage *message, NetworkAddr *remote, CoAPSendNode *sendnode)
{
    CoAPObservable *node = NULL, *next =  NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if (COAP_SUCCESS == CoAPOption_present(message, COAP_OPTION_OBSERVE)) {
        COAP_DEBUG("There is Observe option in message, handle it");
        if (NULL == sendnode) { /* Not the first response */

            HAL_MutexLock(ctx->obsclient.list_mutex);
            list_for_each_entry(node, &ctx->obsclient.list, obslist, CoAPObservable) {
                if (0 != node->tokenlen && node->tokenlen == message->header.tokenlen
                    && 0 == memcmp(node->token, message->token, node->tokenlen)) {
                    CoAPUintOption_get(message, COAP_OPTION_MAXAGE, &node->max_age);
                    if (NULL != node->callback) {
                        COAP_DEBUG("Call the observe client callback %p", node->callback);
                        node->callback(ctx, COAP_REQUEST_SUCCESS, node->userdata, remote, message);
                    } else {
                        COAP_INFO("The observe client callback is NULL");
                    }
                    break;
                }
            }
            HAL_MutexUnlock(ctx->obsclient.list_mutex);

        } else {
            int found = 0;
            HAL_MutexLock(ctx->obsclient.list_mutex);
            list_for_each_entry(node, &ctx->obsclient.list, obslist, CoAPObservable) {
                if (0 != node->tokenlen && node->tokenlen == message->header.tokenlen
                    && 0 == memcmp(node->token, message->token, node->tokenlen)) {
                    found = 1;
                    break;
                }
            }
            if (!found && ctx->obsclient.count < ctx->obsclient.maxcount) {
                CoAPObservable *newnode = coap_malloc(sizeof(CoAPObservable));
                if (NULL != newnode) {
                    memset(newnode, 0x00, sizeof(CoAPObservable));
                    newnode->tokenlen = message->header.tokenlen;
                    memcpy(newnode->token, message->token, message->header.tokenlen);
                    memcpy(&newnode->remote, remote, sizeof(NetworkAddr));
                    newnode->callback = sendnode->handler;
                    newnode->userdata = sendnode->user;
                    CoAPUintOption_get(message, COAP_OPTION_MAXAGE, &newnode->max_age);
                    list_add_tail(&newnode->obslist, &ctx->obsclient.list);
                    ctx->obsclient.count ++;
                    COAP_DEBUG("Add a new obsclient");
                }
            } else {
                COAP_INFO("Cur have %d obsclient, max allow %d", ctx->obsclient.count, ctx->obsclient.maxcount);
            }
            HAL_MutexUnlock(ctx->obsclient.list_mutex);
        }
    } else {
        HAL_MutexLock(ctx->obsclient.list_mutex);
        list_for_each_entry_safe(node, next, &ctx->obsclient.list, obslist, CoAPObservable) {
            if (0 != node->tokenlen && node->tokenlen == message->header.tokenlen
                && 0 == memcmp(node->token, message->token, node->tokenlen)) {
                list_del(&node->obslist);
                ctx->obsclient.count --;
                coap_free(node);
                break;
            }
        }
        HAL_MutexUnlock(ctx->obsclient.list_mutex);

    }

    return COAP_SUCCESS;
}

int CoAPObsClient_delete(CoAPContext *context, CoAPMessage *message)
{
    int ret = COAP_SUCCESS;
    unsigned int observe_option = 0;
    CoAPObservable *node = NULL, *next = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if (NULL == ctx || NULL == message) {
        return COAP_ERROR_INVALID_PARAM;
    }
    if (COAP_MSG_CODE_GET == message->header.code) {
        if (COAP_SUCCESS == CoAPOption_present(message, COAP_OPTION_OBSERVE)) {
            ret = CoAPUintOption_get(message, COAP_OPTION_OBSERVE, &observe_option);
            if (COAP_SUCCESS == ret && 1 == observe_option) {
                HAL_MutexLock(ctx->obsclient.list_mutex);
                list_for_each_entry_safe(node, next, &ctx->obsclient.list, obslist, CoAPObservable) {
                    if (0 != node->tokenlen && node->tokenlen == message->header.tokenlen
                        && 0 == memcmp(node->token, message->token, node->tokenlen)) {
                        list_del(&node->obslist);
                        ctx->obsclient.count --;
                        coap_free(node);
                        break;
                    }
                }
                HAL_MutexUnlock(ctx->obsclient.list_mutex);
            }
        }
    }
    return COAP_SUCCESS;
}

#endif
