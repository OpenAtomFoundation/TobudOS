/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "alcs_internal.h"
#include "alcs_coap.h"
#include "CoAPPlatform.h"
#include "CoAPResource.h"
#include "alcs_api_internal.h"
#include "CoAPServer.h"

#define MAX_PATH_CHECKSUM_LEN (5)
typedef struct {
    char               path[MAX_PATH_CHECKSUM_LEN];
    CoAPRecvMsgHandler cb;
    struct list_head   lst;
} resource_cb_item;

LIST_HEAD(resource_cb_head);

static uint32_t tokenSeed = 0;
uint32_t getToken()
{
    if (tokenSeed == 0) {
        HAL_Srandom((uint32_t)HAL_UptimeMs());
        tokenSeed = HAL_Random(0xffffffff);
    } else {
        ++tokenSeed;
    }

    return tokenSeed;
}

void alcs_msg_init(CoAPContext *ctx, CoAPMessage *message, int code, unsigned char type,
                   int keep, CoAPLenString *payload, void *userdata)
{
    uint32_t token = 0;

    CoAPMessage_init(message);
    message->header.code = code;
    message->header.type = type;
    message->user = userdata;
    message->payload = payload->data;
    message->payloadlen = payload->len;
    message->keep = keep;

    message->header.msgid = CoAPMessageId_gen(ctx);
    message->header.tokenlen = 4;
    token = getToken();
    memcpy(&message->token, &token, 4);
}

void alcs_msg_deinit(CoAPMessage *message)
{
    CoAPMessage_destory(message);
}

static int do_sendmsg(CoAPContext *context, NetworkAddr *addr, CoAPMessage *message, char observe, unsigned short msgid,
                      CoAPLenString *token)
{
    int ret = COAP_SUCCESS;
    if (!context || !addr || !message) {
        return COAP_ERROR_NULL;
    }

    if (msgid == 0) {
        message->header.msgid = CoAPMessageId_gen(context);
    } else {
        message->header.msgid = msgid;
    }

    if (observe == 0) {
        CoAPUintOption_add(message, COAP_OPTION_OBSERVE, observe);
    }

    if (token) {
        message->header.tokenlen = token->len;
        memcpy(&message->token, token->data, token->len);
    }

    ret = CoAPMessage_send(context, addr, message);
    CoAPMessage_destory(message);
    return ret;
}

int alcs_sendmsg(CoAPContext *context, NetworkAddr *addr, CoAPMessage *message, char observe,
                 CoAPSendMsgHandler handler)
{
    message->handler = handler;
    return do_sendmsg(context, addr, message, observe, message->header.msgid, NULL);
}

int alcs_sendrsp(CoAPContext *context, NetworkAddr *addr, CoAPMessage *message, char observe, unsigned short msgid,
                 CoAPLenString *token)
{
    return do_sendmsg(context, addr, message, observe, msgid, token);
}

/* observe */
int alcs_observe_notify(CoAPContext *context, const char *path, CoAPLenString *payload)
{
    int needAuth = 0;
#ifdef USE_ALCS_SECURE
    needAuth = alcs_resource_need_auth(context, path);
#endif
    COAP_DEBUG("payload:%s", payload->data);
    /* HEXDUMP_DEBUG(payload->data, payload->len); */
    return CoAPObsServer_notify(context, path, payload->data, payload->len,
                                needAuth ? &observe_data_encrypt : NULL);
}

static void send_err_rsp(CoAPContext *ctx, NetworkAddr *addr, int code, CoAPMessage *fromMsg)
{
    CoAPMessage sendMsg;
    CoAPLenString payload = {0};
    CoAPLenString token = {fromMsg->header.tokenlen, fromMsg->token};

    alcs_msg_init(ctx, &sendMsg, code, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    alcs_sendrsp(ctx, addr, &sendMsg, 1, fromMsg->header.msgid, &token);
}

static void recv_msg_handler(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *message)
{
    unsigned int obsVal;
    resource_cb_item *node = NULL, *next = NULL;
    char path_calc[MAX_PATH_CHECKSUM_LEN] = {0};
    CoAPPathMD5_sum(path, strlen(path), path_calc, MAX_PATH_CHECKSUM_LEN);

    list_for_each_entry_safe(node, next, &resource_cb_head, lst, resource_cb_item) {
        if (0 == memcmp(path_calc, node->path, MAX_PATH_CHECKSUM_LEN)) {
            if (CoAPUintOption_get(message, COAP_OPTION_OBSERVE, &obsVal) == COAP_SUCCESS) {
                if (obsVal == 0) {
                    CoAPObsServer_add(context, path, remote, message);
                }
            }
            COAP_INFO("recv_msg_handler call callback");
            node->cb(context, path, remote, message);
            return;
        }
    }

    COAP_ERR("receive unknown request, path:%s", path);
    send_err_rsp(context, remote, COAP_MSG_CODE_401_UNAUTHORIZED, message);
}

/* resource */
int alcs_resource_register(CoAPContext *context, const char *pk, const char *dn, const char *path,
                           unsigned short permission,
                           unsigned int ctype, unsigned int maxage, char needAuth, CoAPRecvMsgHandler callback)
{
    COAP_DEBUG("alcs_resource_register, ctx:%p", context);
    COAP_DEBUG("ALCS Resource Register: %s", path);

    if (!needAuth) {
        resource_cb_item *item = (resource_cb_item *)coap_malloc(sizeof(resource_cb_item));
        CoAPPathMD5_sum(path, strlen(path), item->path, MAX_PATH_CHECKSUM_LEN);
        item->cb = callback;
        list_add_tail(&item->lst, &resource_cb_head);

        return CoAPResource_register(context, path, permission, ctype, maxage, &recv_msg_handler);
    } else {
#ifdef USE_ALCS_SECURE
        return alcs_resource_register_secure(context, pk, dn, path, permission, ctype, maxage, callback);
#else
        return -1;
#endif
    }
}

int alcs_resource_need_auth(CoAPContext *context, const char *path)
{
    resource_cb_item *node = NULL, *next = NULL;
    char path_calc[MAX_PATH_CHECKSUM_LEN] = {0};
    CoAPPathMD5_sum(path, strlen(path), path_calc, MAX_PATH_CHECKSUM_LEN);

    list_for_each_entry_safe(node, next, &resource_cb_head, lst, resource_cb_item) {
        if (memcmp(path_calc, node->path, MAX_PATH_CHECKSUM_LEN) == 0) {
            return 0;
        }
    }

    return 1;
}

typedef struct {
    CoAPContext *ctx;
    char loop;
    bool inited;
    struct list_head lst;
} ALCSContext;

#ifdef SUPPORT_MULTI_DEVICES
LIST_HEAD(context_head);

ALCSContext *get_context(CoAPContext *ctx)
{
    ALCSContext *node = NULL, *next = NULL;

    list_for_each_entry_safe(node, next, &context_head, lst, ALCSContext) {
        if (node->ctx == ctx) {
            return node;
        }
    }
    return NULL;
}

CoAPContext *alcs_context_create(CoAPInitParam *param)
{
    ALCSContext *alcs_ctx = (ALCSContext *) coap_malloc(sizeof(ALCSContext));
    alcs_ctx->ctx = CoAPContext_create(param);
    COAP_INFO("CoAPContext_create return :%p", alcs_ctx->ctx);
    alcs_ctx->loop = 0;
    alcs_ctx->inited = 0;

    list_add_tail(&alcs_ctx->lst, &context_head);
    return alcs_ctx->ctx;
}

void alcs_context_free(CoAPContext *ctx)
{
    ALCSContext *alcs_ctx = get_context(ctx);
    if (alcs_ctx) {
        CoAPContext_free(alcs_ctx->ctx);
        coap_free(alcs_ctx);
    }
}

#else
ALCSContext *g_alcs_ctx = NULL;
ALCSContext *get_context(CoAPContext *ctx)
{
    return g_alcs_ctx;
}

CoAPContext *alcs_context_init(CoAPInitParam *param)
{
    if (g_alcs_ctx) {
        return g_alcs_ctx->ctx;
    }

    g_alcs_ctx = (ALCSContext *)coap_malloc(sizeof(ALCSContext));
    if (g_alcs_ctx) {
        g_alcs_ctx->loop = 0;
        g_alcs_ctx->inited = 0;
        g_alcs_ctx->ctx = CoAPServer_init();
        COAP_INFO("CoAPServer_init return :%p", g_alcs_ctx->ctx);
        if (!g_alcs_ctx->ctx) {
            coap_free(g_alcs_ctx);
            g_alcs_ctx = NULL;
            return NULL;
        }
        return g_alcs_ctx->ctx;
    } else {
        return NULL;
    }
}

void alcs_context_deinit()
{
    if (g_alcs_ctx) {
        if (g_alcs_ctx->ctx) {
            CoAPServer_deinit(g_alcs_ctx->ctx);
        }
        coap_free(g_alcs_ctx);
        g_alcs_ctx = NULL;
    }
}

CoAPContext *alcs_get_context()
{
    return g_alcs_ctx ? g_alcs_ctx->ctx : NULL;
}

#endif

extern void on_auth_timer(void *arg);

void *thread_routine(void *arg)
{
    ALCSContext *ctx = (ALCSContext *)arg;
    ctx->loop = 1;

    COAP_INFO("thread_routine");

    while (ctx->loop) {
        CoAPMessage_cycle(ctx->ctx);
#ifdef USE_ALCS_SECURE
        on_auth_timer(ctx->ctx);
#endif
    }

    return NULL;
}

void alcs_start_loop(CoAPContext *ctx, int newThread)
{
#ifdef SUPPORT_MULTI_DEVICES
    void *handle = NULL;
    ALCSContext *alcs_ctx = get_context(ctx);
    if (alcs_ctx && !alcs_ctx->loop) {
        int stack_used = 0;
        if (!newThread || 0 != HAL_ThreadCreate(&handle, thread_routine, alcs_ctx, NULL, &stack_used)) {
            thread_routine(alcs_ctx);
        }
    }
#else

#ifdef USE_ALCS_SECURE
    CoAPServer_add_timer(on_auth_timer);
#endif
    CoAPServer_loop(ctx);
#endif
}

void alcs_stop_loop(CoAPContext *ctx)
{
#ifdef SUPPORT_MULTI_DEVICES
    ALCSContext *alcs_ctx = get_context(ctx);
    if (alcs_ctx) {
        alcs_ctx->loop = 0;
    }
#else
    CoAPServer_deinit(ctx);
#endif
}

void alcs_init()
{
#ifdef SUPPORT_MULTI_DEVICES
    INIT_LIST_HEAD(&context_head);
#endif
    INIT_LIST_HEAD(&resource_cb_head);
}

void alcs_deinit()
{
    resource_cb_item *del_item = NULL;

    list_for_each_entry(del_item, &resource_cb_head, lst, resource_cb_item) {
        list_del(&del_item->lst);
        coap_free(del_item);
        del_item = list_entry(&resource_cb_head, resource_cb_item, lst);
    }
}

static int path_2_option(const char *uri, CoAPMessage *message)
{
    const char *ptr     = NULL;
    const char *pstr    = NULL;
    char  path[COAP_MSG_MAX_PATH_LEN]  = {0};

    if (256 < strlen(uri)) {
        COAP_ERR("The uri length is too loog,len = %d", (int)strlen(uri));
        return COAP_ERROR_INVALID_LENGTH;
    }
    COAP_DEBUG("The uri is %s", uri);
    ptr = pstr = uri;
    while ('\0' != *ptr) {
        if ('/' == *ptr) {
            if (ptr != pstr) {
                memset(path, 0x00, sizeof(path));
                strncpy(path, pstr, ptr - pstr);
                COAP_DEBUG("path: %s,len=%d", path, (int)(ptr - pstr));
                CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                                  (unsigned char *)path, (int)strlen(path));
            }
            pstr = ptr + 1;

        }
        if ('\0' == *(ptr + 1) && '\0' != *pstr) {
            memset(path, 0x00, sizeof(path));
            strncpy(path, pstr, sizeof(path) - 1);
            COAP_DEBUG("path: %s,len=%d", path, (int)strlen(path));
            CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                              (unsigned char *)path, (int)strlen(path));
        }
        ptr ++;
    }
    return COAP_SUCCESS;
}

int alcs_msg_setAddr(CoAPMessage *message, const char *path, const char *query)
{
    int rt = 0;

    if (NULL == path || NULL == message) {
        COAP_ERR("Invalid paramter p_path %p, p_message %p", path, message);
        return COAP_ERROR_INVALID_PARAM;
    }

    if (255 < strlen(path)) {
        COAP_ERR("The uri length is too loog,len = %d", (int)strlen(path));
        return COAP_ERROR_INVALID_LENGTH;
    }

    rt = path_2_option(path, message);
    if (query) {
        CoAPStrOption_add(message, COAP_OPTION_URI_QUERY, (unsigned char *)query, strlen(query));
    }

    return rt;
}

