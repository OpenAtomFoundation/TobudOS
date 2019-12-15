/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "dev_reset_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

static int g_dev_reset_sub_flag = 0;
iotx_devrst_evt_handle_t g_devrst_event_handle = NULL;


#include "mqtt_api.h"
static void mqtt_sub_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    switch (msg->event_type)
    {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED: {
            iotx_mqtt_topic_info_t *packet_info = (iotx_mqtt_topic_info_t *)msg->msg;
            if (g_devrst_event_handle) {
                iotx_devrst_evt_recv_msg_t recv_msg;
                memset(&recv_msg, 0, sizeof(iotx_devrst_evt_recv_msg_t));
                recv_msg.msgid = packet_info->packet_id;
                recv_msg.payload = (char *)packet_info->payload;
                recv_msg.payload_len = packet_info->payload_len;
                g_devrst_event_handle(IOTX_DEVRST_EVT_RECEIVED, (void *)&recv_msg);
            }
        }
        break;

        default:
            break;
    }
}

int IOT_DevReset_Report(iotx_dev_meta_info_t *meta_info, iotx_devrst_evt_handle_t handle, void *extended)
{
    int res = 0;
    const char *reset_fmt = "/sys/%s/%s/thing/reset";
    const char *reset_reply_fmt = "/sys/%s/%s/thing/reset_reply";
    const char *payload_fmt = "{\"id\":%d, \"version\":\"1.0\", \"method\":\"thing.reset\", \"params\":{}}";
    char topic[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 30] = {0};
    char payload[128] = {0};

    if (meta_info == NULL || handle== NULL ) {
        return FAIL_RETURN;
    }
    g_devrst_event_handle = handle;

    memset(topic, 0, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 30);
    HAL_Snprintf(topic,IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 30, reset_reply_fmt, meta_info->product_key, meta_info->device_name);

    if (g_dev_reset_sub_flag == 0) {
        res = IOT_MQTT_Subscribe_Sync(NULL, topic, IOTX_MQTT_QOS0, mqtt_sub_handle, NULL, 5000);
        if (res < 0 ) {
            return FAIL_RETURN;
        }
        g_dev_reset_sub_flag = 1;
    }

    memset(topic, 0, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 30);
    HAL_Snprintf(topic, IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 30, reset_fmt, meta_info->product_key, meta_info->device_name);

    memset(payload, 0, 128);
    HAL_Snprintf(payload, 128, payload_fmt, iotx_report_id());

    res = IOT_MQTT_Publish_Simple(NULL, topic, IOTX_MQTT_QOS0, payload, strlen(payload));

    return res;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

