/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_httpc.h"
#include "http_api.h"
#include "http_wrapper.h"

#define EXAMPLE_TRACE(fmt, ...)                        \
    do {                                               \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__);                \
        HAL_Printf("%s", "\r\n");                      \
    } while (0)

#define MAX_BUF_LEN 256
#define DEFAULT_TIMEOUT_MS 5000

/*  on line */
#define IOTX_PRODUCT_KEY         "a1KqSriAwh0"
#define IOTX_DEVICE_NAME         "basic_test_01"
#define IOTX_DEVICE_SECRET       "5gme06iFl3W1g8non2ksVs5e8Qlus4Hw"
#define IOTX_DEVICE_ID           "a1KqSriAwh0.basic_test_01"

static char request_buf[MAX_BUF_LEN];
static char response_buf[MAX_BUF_LEN];

void looptest(void *handle)
{
    iotx_http_message_param_t msg;
    char path[IOTX_URI_MAX_LEN + 1] = { 0 };
    int loop_cnt = 3;
    int success_cnt = 0;
    int cnt = 0;

    if (NULL == handle) {
        return;
    }

    HAL_Snprintf(request_buf, MAX_BUF_LEN, "{\"name\":\"hello world\"}");
    memset(response_buf, 0x00, MAX_BUF_LEN);
    HAL_Snprintf(path, IOTX_URI_MAX_LEN, "/topic/%s/%s/user/update",
                 IOTX_PRODUCT_KEY,
                 IOTX_DEVICE_NAME);
    msg.request_payload = request_buf;
    msg.response_payload = response_buf;
    msg.timeout_ms = 5000;
    msg.request_payload_len = strlen(msg.request_payload) + 1;
    msg.response_payload_len = MAX_BUF_LEN;
    msg.topic_path = path;

    while (cnt++ < loop_cnt) {
        if (0 == IOT_HTTP_SendMessage(handle, &msg)) {
            success_cnt++;
        }
    }
    EXAMPLE_TRACE("loop cnt is %d, success_cnt is %d\n", loop_cnt, success_cnt);
}


static int http_upload_test()
{
    iotx_device_info_t      device_info;
    iotx_http_param_t       http_param;
    void                   *handle = NULL;
    memset(&http_param, 0, sizeof(http_param));

    HAL_SetProductKey((char *)IOTX_PRODUCT_KEY);
    HAL_SetDeviceName((char *)IOTX_DEVICE_NAME);
    HAL_SetDeviceSecret((char *)IOTX_DEVICE_SECRET);

    strncpy(device_info.product_key,  IOTX_PRODUCT_KEY, IOTX_PRODUCT_KEY_LEN);
    strncpy(device_info.device_secret, IOTX_DEVICE_SECRET, IOTX_DEVICE_SECRET_LEN);
    strncpy(device_info.device_name,  IOTX_DEVICE_NAME, IOTX_DEVICE_NAME_LEN);
    strncpy(device_info.device_id,  IOTX_DEVICE_ID, IOTX_DEVICE_ID_LEN);

    http_param.device_info = &device_info;
    http_param.timeout_ms = DEFAULT_TIMEOUT_MS;

    handle = IOT_HTTP_Init(&http_param);
    if (NULL != handle) {
        IOT_HTTP_DeviceNameAuth(handle);
        HAL_Printf("IoTx HTTP Message Sent\r\n");
    } else {
        HAL_Printf("IoTx HTTP init failed\r\n");
        return 0;
    }
    looptest(handle);

    IOT_HTTP_Disconnect(handle);
    IOT_HTTP_DeInit(&handle);
    return 0;
}

int main(int argc, char **argv)
{
#if (defined(ON_DAILY)) || (defined(ON_PRE))
    EXAMPLE_TRACE("the device name/device secrete/product name is only valid for ONLINE, EXIT!");
    return 0;
#endif
    int ret;
    ret = http_upload_test();
    return ret;
}
