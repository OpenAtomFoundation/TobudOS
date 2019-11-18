/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include <string.h>
#include "CoAPPlatform.h"
#include "CoAPExport.h"
#include "CoAPServer.h"

#define COAP_INIT_TOKEN     (0x01020304)

static unsigned int g_coap_running = 0;
#ifdef COAP_SERV_MULTITHREAD
    static void *g_coap_thread = NULL;
    static void *g_semphore    = NULL;
#endif
static CoAPContext *g_context = NULL;

static unsigned int CoAPServerToken_get(unsigned char *p_encoded_data)
{
    static unsigned int value = COAP_INIT_TOKEN;
    p_encoded_data[0] = (unsigned char)((value & 0x00FF) >> 0);
    p_encoded_data[1] = (unsigned char)((value & 0xFF00) >> 8);
    p_encoded_data[2] = (unsigned char)((value & 0xFF0000) >> 16);
    p_encoded_data[3] = (unsigned char)((value & 0xFF000000) >> 24);
    value++;
    return sizeof(unsigned int);
}

static int CoAPServerPath_2_option(char *uri, CoAPMessage *message)
{
    char *ptr     = NULL;
    char *pstr    = NULL;
    char  path[COAP_MSG_MAX_PATH_LEN]  = {0};

    if (NULL == uri || NULL == message) {
        COAP_ERR("Invalid paramter p_path %p, p_message %p", uri, message);
        return COAP_ERROR_INVALID_PARAM;
    }
    if (COAP_MSG_MAX_PATH_LEN <= strlen(uri)) {
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
                CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                                  (unsigned char *)path, (int)strlen(path));
            }
            pstr = ptr + 1;

        }
        if ('\0' == *(ptr + 1) && '\0' != *pstr) {
            memset(path, 0x00, sizeof(path));
            strncpy(path, pstr, sizeof(path) - 1);
            CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                              (unsigned char *)path, (int)strlen(path));
        }
        ptr ++;
    }
    return COAP_SUCCESS;
}

void CoAPServer_thread_leave()
{
    g_coap_running = 0;
}

void *coap_yield_mutex = NULL;

static void *CoAPServer_yield(void *param)
{
    CoAPContext *context = (CoAPContext *)param;
    COAP_DEBUG("Enter to CoAP daemon task");

    while (g_coap_running) {
        CoAPMessage_cycle(context);
    }

#ifdef COAP_SERV_MULTITHREAD
    HAL_SemaphorePost(g_semphore);
    COAP_INFO("Exit the CoAP daemon task, Post semphore");

    HAL_ThreadDelete(NULL);
    g_coap_thread = NULL;
#endif
    return NULL;
}

typedef void (*func_v_v)(void *);
static func_v_v coapserver_timer = NULL;
void CoAPServer_add_timer(void (*on_timer)(void *))
{
    coapserver_timer = on_timer;
}


static void *coap_init_mutex = NULL;
CoAPContext *CoAPServer_init()
{
    CoAPInitParam param = {0};
#ifdef COAP_SERV_MULTITHREAD
    int stack_used;
    hal_os_thread_param_t task_parms = {0};
#endif

    if (NULL == coap_init_mutex) {
        coap_init_mutex = HAL_MutexCreate();

        if (NULL == coap_init_mutex) {
            return NULL;
        }
    }
    HAL_MutexLock(coap_init_mutex);

    if (NULL == g_context) {
        param.appdata = NULL;
        param.group = "224.0.1.187";
        param.notifier = NULL;
        param.obs_maxcount = 16;
        param.res_maxcount = 255;
        param.port = 5683;
        param.send_maxcount = 16;
        param.waittime = 50;

#ifdef COAP_SERV_MULTITHREAD
        g_semphore  = HAL_SemaphoreCreate();
        if (NULL == g_semphore) {
            COAP_ERR("Semaphore Create failed");
            HAL_MutexUnlock(coap_init_mutex);
            return NULL;
        }

        coap_yield_mutex = HAL_MutexCreate();
        if (NULL == coap_yield_mutex) {
            COAP_ERR("coap_yield_mutex Create failed");
            HAL_SemaphoreDestroy(g_semphore);
            g_semphore = NULL;
            HAL_MutexUnlock(coap_init_mutex);
            return NULL;
        }
#endif

        g_context = CoAPContext_create(&param);
        if (NULL == g_context) {
#ifdef COAP_SERV_MULTITHREAD
            HAL_SemaphoreDestroy(g_semphore);
            HAL_MutexDestroy(coap_yield_mutex);
            g_semphore = NULL;
            coap_yield_mutex = NULL;
#endif
            COAP_ERR("CoAP Context Create failed");
            HAL_MutexUnlock(coap_init_mutex);
            return NULL;
        }
#ifdef COAP_SERV_MULTITHREAD
        g_coap_running = 1;
        task_parms.stack_size = 4608;
        task_parms.name = "CoAPServer_yield";
        HAL_ThreadCreate(&g_coap_thread, CoAPServer_yield, (void *)g_context, &task_parms, &stack_used);
#endif

    } else {
        COAP_INFO("The CoAP Server already init");
    }

    HAL_MutexUnlock(coap_init_mutex);
    return (CoAPContext *)g_context;
}

void CoAPServer_deinit(CoAPContext *context)
{
    if (context != g_context) {
        COAP_INFO("Invalid CoAP Server context");
        return;
    }

    if (NULL == coap_init_mutex) {
        COAP_ERR("CoAP init mutex is NULL");
        return;
    }

    HAL_MutexLock(coap_init_mutex);

    COAP_INFO("CoAP Server deinit");
    g_coap_running = 0;

#ifdef COAP_SERV_MULTITHREAD
    if (NULL != g_semphore) {
        HAL_SemaphoreWait(g_semphore, PLATFORM_WAIT_INFINITE);
        COAP_INFO("Wait Semaphore, will exit task");
        HAL_SemaphoreDestroy(g_semphore);
        g_semphore = NULL;
    }
    if (NULL != coap_yield_mutex) {
        HAL_MutexDestroy(coap_yield_mutex);
        coap_yield_mutex = NULL;
    }
#endif
    if (NULL != context) {
        CoAPContext_free(context);
        g_context = NULL;
    }

    HAL_MutexUnlock(coap_init_mutex);
    HAL_MutexDestroy(coap_init_mutex);
    coap_init_mutex = NULL;
}

int CoAPServer_register(CoAPContext *context, const char *uri, CoAPRecvMsgHandler callback)
{
    if (NULL == context || g_context != context) {
        return COAP_ERROR_INVALID_PARAM;
    }

    return CoAPResource_register(context, uri, COAP_PERM_GET, COAP_CT_APP_JSON, 60, callback);
}

int CoAPServerMultiCast_send(CoAPContext *context, NetworkAddr *remote, const char *uri, unsigned char *buff,
                             unsigned short len, CoAPSendMsgHandler callback, unsigned short *msgid)
{
    int ret = COAP_SUCCESS;
    CoAPMessage message;
    unsigned char tokenlen;
    unsigned char token[COAP_MSG_MAX_TOKEN_LEN] = {0};

    if (NULL == context || g_context != context || NULL == remote
        || NULL == uri || NULL == buff || NULL == msgid) {
        return COAP_ERROR_INVALID_PARAM;
    }


    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_NON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_POST);
    CoAPMessageId_set(&message, CoAPMessageId_gen(context));
    tokenlen = CoAPServerToken_get(token);
    CoAPMessageToken_set(&message, token, tokenlen);
    CoAPMessageHandler_set(&message, callback);
    CoAPMessageKeep_Set(&message, 1);

    CoAPServerPath_2_option((char *)uri, &message);
    CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPMessagePayload_set(&message, buff, len);
    if (msgid) {
        *msgid = message.header.msgid;
    }
    ret = CoAPMessage_send(context, remote, &message);

    CoAPMessage_destory(&message);

    return ret;
}

int CoAPServerResp_send(CoAPContext *context, NetworkAddr *remote, unsigned char *buff, unsigned short len, void *req,
                        const char *paths, CoAPSendMsgHandler callback, unsigned short *msgid, char qos)
{
    int ret = COAP_SUCCESS;
    CoAPMessage response;
    unsigned int observe = 0;
    CoAPMessage *request = (CoAPMessage *)req;

    if (NULL == context || g_context != context || NULL == remote
        || NULL == buff || NULL == paths || NULL == req) {
        return COAP_ERROR_INVALID_PARAM;
    }

    CoAPMessage_init(&response);
    CoAPMessageType_set(&response, qos == 0 ? COAP_MESSAGE_TYPE_NON : COAP_MESSAGE_TYPE_CON);
    CoAPMessageCode_set(&response, COAP_MSG_CODE_205_CONTENT);
    CoAPMessageId_set(&response, request->header.msgid);
    CoAPMessageToken_set(&response, request->token, request->header.tokenlen);
    CoAPMessageHandler_set(&response, callback);
    if (msgid) {
        *msgid = response.header.msgid;
    }

    ret = CoAPUintOption_get(request, COAP_OPTION_OBSERVE, &observe);
    if (COAP_SUCCESS == ret && 0 == observe) {
        CoAPObsServer_add(context, paths, remote, request);
        CoAPUintOption_add(&response, COAP_OPTION_OBSERVE, 0);
    }

    CoAPUintOption_add(&response, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPMessagePayload_set(&response, buff, len);

    COAP_DEBUG("Send a response message");
    ret = CoAPMessage_send(context, remote, &response);
    CoAPMessage_destory(&response);

    return ret;
}

void CoAPServer_loop(CoAPContext *context)
{
    if (g_context != context  || 1 == g_coap_running) {
        COAP_INFO("The CoAP Server is already running");
        return;
    }

#ifndef COAP_SERV_MULTITHREAD
    g_coap_running = 1;
    CoAPServer_yield((void *)context);
#endif
}
