/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "dev_bind_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
#define AWSS_CONNAP_MONITOR_TIMEOUT_MS  (60 * 1000)

static char g_awss_connectap_info_avaliable = 0;
static void *connectap_monitor_timer = NULL;
static void *connectap_monitor_mutex = NULL;

static void awss_release_connectap_monitor()
{
    if (connectap_monitor_timer) {
        HAL_Timer_Stop(connectap_monitor_timer);
        HAL_Timer_Delete(connectap_monitor_timer);
        connectap_monitor_timer = NULL;
    }
    if (connectap_monitor_mutex) {
        HAL_MutexUnlock(connectap_monitor_mutex);
        HAL_MutexDestroy(connectap_monitor_mutex);
        connectap_monitor_mutex = NULL;
    }
}

static void awss_connectap_monitor(void *param)
{
    if (connectap_monitor_mutex) {
        HAL_MutexLock(connectap_monitor_mutex);
    }
    g_awss_connectap_info_avaliable = 0;
    awss_release_connectap_monitor();
}

int awss_stop_connectap_monitor()
{
    awss_connectap_monitor(NULL);
    return 0;
}

int awss_start_connectap_monitor()
{
    if (connectap_monitor_timer) {
        awss_debug("connap-m exist");
        return 0;
    }

    if (connectap_monitor_mutex == NULL) {
        connectap_monitor_mutex = HAL_MutexCreate();
        if (connectap_monitor_mutex == NULL) {
            awss_err("connap alloc-m fail");
            goto CONNAP_M_FAIL;
        }
    }

    HAL_MutexLock(connectap_monitor_mutex);

    connectap_monitor_timer = HAL_Timer_Create("connap_monitor",
                              awss_connectap_monitor, NULL);
    if (connectap_monitor_timer == NULL) {
        awss_err("connap alloc-t fail");
        goto CONNAP_M_FAIL;
    }

    g_awss_connectap_info_avaliable = 1;
    HAL_Timer_Stop(connectap_monitor_timer);
    HAL_Timer_Start(connectap_monitor_timer, AWSS_CONNAP_MONITOR_TIMEOUT_MS);
    HAL_MutexUnlock(connectap_monitor_mutex);
    return 0;

CONNAP_M_FAIL:
    awss_release_connectap_monitor();
    return -1;
}

int process_get_device_info(void *ctx, void *resource, void *remote, void *request, char is_mcast, int type)
{
    char *buf = NULL;
    char *dev_info = NULL;
    int len = 0, id_len = 0;
    char *msg = NULL, *id = NULL;
    const char *topic_fmt = NULL;
    char topic[TOPIC_LEN_MAX] = {0};
    char req_msg_id[MSG_REQ_ID_LEN] = {0};

    buf = os_zalloc(DEV_INFO_LEN_MAX);
    if (!buf) {
        goto DEV_INFO_ERR;
    }

    dev_info = os_zalloc(DEV_INFO_LEN_MAX);
    if (!dev_info) {
        goto DEV_INFO_ERR;
    }

    msg = awss_cmp_get_coap_payload(request, &len);
    id = json_get_value_by_name(msg, len, "id", &id_len, 0);
    if (id && id_len < MSG_REQ_ID_LEN) {
        memcpy(req_msg_id, id, id_len);
    }

    if (type == AWSS_NOTIFY_DEV_RAND_SIGN) {
        topic_fmt = is_mcast ? TOPIC_AWSS_GETDEVICEINFO_MCAST : TOPIC_AWSS_GETDEVICEINFO_UCAST;
    } else if (type == AWSS_NOTIFY_SUCCESS) {
        topic_fmt = is_mcast ? TOPIC_AWSS_GET_CONNECTAP_INFO_MCAST : TOPIC_AWSS_GET_CONNECTAP_INFO_UCAST;
    } else {
        goto DEV_INFO_ERR;
    }
    awss_build_dev_info(type, buf, DEV_INFO_LEN_MAX);
    HAL_Snprintf(dev_info, DEV_INFO_LEN_MAX - 1, "{%s}", buf);

    memset(buf, 0x00, DEV_INFO_LEN_MAX);
    HAL_Snprintf(buf, DEV_INFO_LEN_MAX - 1, AWSS_ACK_FMT, req_msg_id, 200, dev_info);

    HAL_Free(dev_info);

    awss_info("tx msg to app: %s", buf);

    awss_build_topic(topic_fmt, topic, TOPIC_LEN_MAX);

    if (0 != awss_cmp_coap_send_resp(buf, strlen(buf), remote, topic, request, NULL, NULL, 0)) {
        awss_err("tx dev info rsp fail.");
    }

    HAL_Free(buf);
    return 0;

DEV_INFO_ERR:
    if (buf) {
        HAL_Free(buf);
    }
    if (dev_info) {
        HAL_Free(dev_info);
    }

    return -1;
}

int awss_process_mcast_get_connectap_info(void *ctx, void *resource, void *remote, void *request)
{
    if (g_awss_connectap_info_avaliable == 0) {
        return -1;
    }
    return process_get_device_info(ctx, resource, remote, request, 1, AWSS_NOTIFY_SUCCESS);
}

int awss_process_ucast_get_connectap_info(void *ctx, void *resource, void *remote, void *request)
{
    if (g_awss_connectap_info_avaliable == 0) {
        return -1;
    }
    return process_get_device_info(ctx, resource, remote, request, 0, AWSS_NOTIFY_SUCCESS);
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
