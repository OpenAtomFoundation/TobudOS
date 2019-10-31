/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "iotx_dm_internal.h"

#ifdef DEPRECATED_LINKKIT

#include "infra_json_parser.h"
#include "mqtt_api.h"
#include "impl_ntp.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#define IMPL_NTP_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "impl.ntp")
#define IMPL_NTP_FREE(ptr)               LITE_free(ptr)
#else
#define IMPL_NTP_MALLOC(size)            HAL_Malloc(size)
#define IMPL_NTP_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

typedef void (*ntp_reply_cb_t)(const char *);
static ntp_reply_cb_t g_ntp_reply_cb = NULL;
static char g_ntp_time[NTP_TIME_STR_MAX_LEN + 1] = {0};

static void linkkit_ntp_time_reply(void *pcontext, void *pclient, void *mesg)
{
#define DEV_TX_TIME          "deviceSendTime"
#define SERVER_RX_TIME       "serverRecvTime"
#define SERVER_TX_TIME       "serverSendTime"

    int len = 0;
    char *elem = NULL;
    char server_rx_time[NTP_TIME_STR_MAX_LEN + 1] = {0};
    char server_tx_time[NTP_TIME_STR_MAX_LEN + 1] = {0};
    uint32_t payload_len;
    char *payload;
    uint32_t tx = 0;
    uint32_t rx = 0;
    uint32_t diff = 0;

    iotx_mqtt_event_msg_pt msg = (iotx_mqtt_event_msg_pt)mesg;
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            payload_len = ptopic_info->payload_len;
            payload = (char *)ptopic_info->payload;
            break;
        default:
            goto NTP_FAIL;
    }

    if (payload == NULL || payload_len == 0) {
        goto NTP_FAIL;
    }

    memset(g_ntp_time, 0, sizeof(g_ntp_time));
    log_debug("[ntp]", "ntp reply len:%u, payload:%s\r\n", payload_len, payload);

    /*
     * get deviceSendTime, serverRecvTime, serverSendTime
     */
    elem = json_get_value_by_name(payload, payload_len, SERVER_TX_TIME, &len, NULL);
    if (elem == NULL || len <= 0 || len > NTP_TIME_STR_MAX_LEN) {
        goto NTP_FAIL;
    }

    memcpy(server_tx_time, elem, len);

    elem = json_get_value_by_name(payload, payload_len, SERVER_RX_TIME, &len, NULL);
    if (elem == NULL || len <= 0 || len > NTP_TIME_STR_MAX_LEN) {
        goto NTP_FAIL;
    }

    memcpy(server_rx_time, elem, len);

    elem = json_get_value_by_name(payload, payload_len, DEV_TX_TIME, &len, NULL);
    if (elem == NULL || len <= 0 || len > NTP_TIME_STR_MAX_LEN) {
        goto NTP_FAIL;
    }
    /*
     * atoi fails to convert string to integer
     * so we convert manully
     */
    while (len -- > 0) {
        tx *= 10;
        tx += elem[0] - '0';
        elem ++;
    }
    rx = HAL_UptimeMs();
    diff = (rx - tx) >> 1;
    if (diff >= 1000000) {
        goto NTP_FAIL;
    }

    len = strlen(server_tx_time);
    elem = &server_tx_time[len > 9 ? len - 9 : 0];
    tx = atoi(elem);
    tx += diff;

    if (tx > 999999999) {
        tx = 999999999;
    }

    if (len > 9) {
        sprintf(elem, "%09u", (unsigned int)tx);
    } else {
        sprintf(elem, "%u", (unsigned int)tx);
    }

    strncpy(g_ntp_time, (const char *)server_tx_time, sizeof(g_ntp_time) - 1);

NTP_FAIL:
    if (g_ntp_reply_cb != NULL) {
        g_ntp_reply_cb(g_ntp_time);
    }
    return;
}

int linkkit_ntp_time_request(void (*ntp_reply)(const char *ntp_offset_time_ms))
{
    int ret = -1;
    int final_len = 0;
    int packet_len = 64;
    int topic_len = 128;
    char *packet = NULL;
    char *topic = NULL;

    do {
        char pk[IOTX_PRODUCT_KEY_LEN + 1] = {0};
        char dn[IOTX_DEVICE_NAME_LEN + 1] = {0};

        HAL_GetProductKey(pk);
        HAL_GetDeviceName(dn);

        topic = (char *)IMPL_NTP_MALLOC(topic_len + 1);
        if (topic == NULL) {
            goto NTP_REQ_ERR;
        }
        memset(topic, 0, topic_len + 1);

        HAL_Snprintf(topic, topic_len, TOPIC_NTP_REPLY, pk, dn);
        ret = IOT_MQTT_Subscribe_Sync(NULL, topic, IOTX_MQTT_QOS0,
                                      (iotx_mqtt_event_handle_func_fpt)linkkit_ntp_time_reply, NULL, 1000);
        if (ret < 0) {
            goto NTP_REQ_ERR;
        }

        memset(topic, 0, topic_len + 1);
        HAL_Snprintf(topic, topic_len, TOPIC_NTP, pk, dn);
    } while (0);

    packet = (char *)IMPL_NTP_MALLOC(packet_len + 1);
    if (packet == NULL) {
        ret = -1;
        goto NTP_REQ_ERR;
    }
    memset(packet, 0, packet_len + 1);

    g_ntp_reply_cb = ntp_reply;
    final_len = HAL_Snprintf(packet, packet_len, "{\"deviceSendTime\":\"%u\"}", (unsigned int)(HAL_UptimeMs()));

    log_debug("[ntp]", "report ntp:%s\r\n", packet);


    ret = IOT_MQTT_Publish_Simple(NULL, topic, IOTX_MQTT_QOS0, packet, final_len);

NTP_REQ_ERR:
    if (topic) {
        IMPL_NTP_FREE(topic);
    }
    if (packet) {
        IMPL_NTP_FREE(packet);
    }
    return ret;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif