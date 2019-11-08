/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "infra_compat.h"
#include "dev_model_api.h"
#include "dm_wrapper.h"
#include "cJSON.h"

#ifdef ATM_ENABLED
    #include "at_api.h"
#endif

void HAL_Printf(const char *fmt, ...);
int HAL_SetProductKey(char *product_key);
int HAL_SetDeviceName(char *device_name);
int HAL_SetProductSecret(char *product_secret);
int HAL_SetDeviceSecret(char *device_secret);

/* Fill in your device info here */
#define PRODUCT_KEY      " ###DEVICE_PK### "
#define PRODUCT_SECRET   "0HxtPRZjwr14bi7K"
#define DEVICE_NAME      "auto_example1"
#define DEVICE_SECRET    "L1xQk7AntumkNNc8wvHwMJsh9PBoexN0"

/* This time interval will be given to SDK for receiving packets from network */
#define USER_EXAMPLE_YIELD_TIMEOUT_MS   (200)

#define EXAMPLE_TRACE(...)  \
    do { \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__); \
        HAL_Printf("\033[0m\r\n"); \
    } while (0)

typedef struct {
    int cloud_connected;
    int master_devid;
    int master_initialized;
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;
static user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Alibaba Cloud Connected Successfully!");
    user_example_ctx->cloud_connected = 1;
    return 0;
}

static int user_master_dev_available(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    if (user_example_ctx->cloud_connected && user_example_ctx->master_initialized) {
        return 1;
    }

    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    EXAMPLE_TRACE("Cloud Disconnected Event Arrived!");
    user_example_ctx->cloud_connected = 0;
    return 0;
}

static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Property Set Received, Devid: %d, Request: %s", devid, request);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid,
                             ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request,
                             request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}

static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    EXAMPLE_TRACE("Device Initialized, Devid: %d", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
    }

    return 0;
}

void user_post_property(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *property_payload = "NULL";

    /* Normal Example */
    property_payload = "{\" ###DM_PROP_ID### \":0}";

    res = IOT_Linkkit_Report(user_example_ctx->master_devid,
                             ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload,
                             strlen(property_payload));

    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

void user_post_event(void)
{
    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char *event_id = " ###DM_EVT_ID### ";
    char *event_payload = "NULL";

    /* Normal Example */
    event_payload = "{\" ###DM_EVT_OU_KEY### \":0}";

    res = IOT_Linkkit_TriggerEvent(user_example_ctx->master_devid,
                                   event_id,
                                   strlen(event_id),
                                   event_payload,
                                   strlen(event_payload));
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}

void set_iotx_info()
{
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
}

int main(void)
{
    unsigned int                    loop_cnt = 0;
    int                             res = 0;
    iotx_linkkit_dev_meta_info_t    master_meta_info;
    user_example_ctx_t             *user_example_ctx = user_example_get_ctx();

#ifdef ATM_ENABLED
    if (IOT_ATM_Init() < 0) {
        EXAMPLE_TRACE("IOT ATM init failed!\n");
        return -1;
    }
#endif

    IOT_SetLogLevel(IOT_LOG_DEBUG);
    set_iotx_info();

    /* Register Callback to Listen What You Interested inside SDK */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);

    /* Create Meta Information Struct for IOT_Linkkit_Open() */
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    memcpy(master_meta_info.product_secret, PRODUCT_SECRET, strlen(PRODUCT_SECRET));
    memcpy(master_meta_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(master_meta_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));

    /* Create Master Device Resources by IOT_Linkkit_Open() */
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open() Failed\n");
        return -1;
    }
    EXAMPLE_TRACE("IOT_Linkkit_Open() Success!\n");

    /* Start Connectting Alibaba Cloud */
    res = IOT_Linkkit_Connect(user_example_ctx->master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect() Failed\n");
        return -1;
    }
    EXAMPLE_TRACE("IOT_Linkkit_Connect() Success! res = IOT_Linkkit_Connect() = %d\n", res);

    /* Infinite Loop of Processing Command Received from Cloud */
    while (1) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);

        /* Post Proprety Example */
        if (loop_cnt % 100 == 0 && user_master_dev_available()) {
            EXAMPLE_TRACE("loop_cnt = %d, going to post property\n", loop_cnt);
            user_post_property();
        }
        /* Post Event Example */
        if (loop_cnt % 180 == 0 && user_master_dev_available()) {
            EXAMPLE_TRACE("loop_cnt = %d, going to post event\n", loop_cnt);
            user_post_event();
        }

        ++ loop_cnt;
    }

    IOT_Linkkit_Close(user_example_ctx->master_devid);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    return 0;
}
