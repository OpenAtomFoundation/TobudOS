/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#ifndef AWSS_DISABLE_REGISTRAR

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#define AWSS_JSON_DEV_NAME      "deviceName"
#define AWSS_JSON_PK            "productKey"
#define AWSS_JSON_DEV_LIST      "data"
#define AWSS_JSON_PERIOD        "timeout"
#define AWSS_JSON_CIPHER        "secret"
#define AWSS_REPORT_PKT_LEN     (512)
#define AWSS_REPORT_PARAM_FMT   "{\"awssVer\":%s,\"type\":0,\"ssid\":\"%s\",\"bssid\":\"%s\",\"rssi\":%d,\"payload\":[\"%s\"]}"

static void awss_wifi_mgnt_frame_callback(uint8_t *buffer, int length, signed char rssi, int buffer_type);
static void registrar_raw_frame_init(struct enrollee_info *enr);
static void registrar_raw_frame_send(void);
static void registrar_raw_frame_destroy(void);
static void enrollee_report(void);
static int enrollee_checkin(void);
static int enrollee_enable_somebody_checkin(char *key, char *dev_name, int timeout);
static int awss_enrollee_get_dev_info(char *payload, int payload_len, char *product_key,
                                      char *dev_name, char *cipher, int *timeout);

/* registrar send pkt interval in ms */
#define REGISTRAR_TIMEOUT               (60)
#define REGISTRAR_WORK_TIME             (16 * 400)

static struct enrollee_info enrollee_info[MAX_ENROLLEE_NUM];
static char registrar_sched_cnt = 0;
static char registrar_inited = 0;
static char registrar_id = 0;

static void *checkin_timer = NULL;
static void *enrollee_report_timer = NULL;
static void *registrar_sched_timer = NULL;

#define ALIBABA_OUI                     {0xD8, 0x96, 0xE0}
#ifdef REGISTRAR_IDLE_DUTY
#if REGISTRAR_IDLE_DUTY > 0
void registrar_schedule(void *param)
{
    uint8_t alibaba_oui[3] = ALIBABA_OUI;
    char last_open = registrar_sched_cnt & 0x01;
    unsigned int idle_duty = REGISTRAR_IDLE_DUTY;

    HAL_Timer_Stop(registrar_sched_timer);
    registrar_sched_cnt ++;

    if (last_open) {  /* need to close */
        HAL_Wifi_Enable_Mgmt_Frame_Filter(FRAME_BEACON_MASK | FRAME_PROBE_REQ_MASK,
                                         (uint8_t *)alibaba_oui, NULL);
        HAL_Timer_Start(registrar_sched_timer, REGISTRAR_WORK_TIME * idle_duty);
    } else {
        HAL_Wifi_Enable_Mgmt_Frame_Filter(FRAME_BEACON_MASK | FRAME_PROBE_REQ_MASK,
                                         (uint8_t *)alibaba_oui, awss_wifi_mgnt_frame_callback);
        HAL_Timer_Start(registrar_sched_timer, REGISTRAR_WORK_TIME);
    }
}
#endif
#endif

void awss_registrar_init(void)
{
    uint8_t alibaba_oui[3] = ALIBABA_OUI;
    if (registrar_inited) {
        return;
    }

    memset(enrollee_info, 0, sizeof(enrollee_info));
    registrar_inited = 1;


    /*
    * if idle duty is zero, don't need to care about power consumption
    */
#ifdef REGISTRAR_IDLE_DUTY
#if REGISTRAR_IDLE_DUTY > 0
    if (registrar_sched_timer == NULL) {
        registrar_sched_timer = HAL_Timer_Create("sched", (void (*)(void *))registrar_schedule, NULL);
    }
    if (registrar_sched_timer) {
        registrar_sched_cnt ++;
        HAL_Timer_Stop(registrar_sched_timer);
        HAL_Timer_Start(registrar_sched_timer, REGISTRAR_WORK_TIME);
    }
#endif
#endif

    HAL_Wifi_Enable_Mgmt_Frame_Filter(FRAME_BEACON_MASK | FRAME_PROBE_REQ_MASK,
                                      (uint8_t *)alibaba_oui, awss_wifi_mgnt_frame_callback);
}

void awss_registrar_deinit(void)
{
    uint8_t alibaba_oui[3] = ALIBABA_OUI;
    HAL_Wifi_Enable_Mgmt_Frame_Filter(FRAME_BEACON_MASK | FRAME_PROBE_REQ_MASK,
                                      (uint8_t *)alibaba_oui, NULL);

    registrar_inited = 0;
    registrar_sched_cnt = 0;
    awss_stop_timer(checkin_timer);
    checkin_timer = NULL;
    awss_stop_timer(enrollee_report_timer);
    enrollee_report_timer = NULL;
    awss_stop_timer(registrar_sched_timer);
    registrar_sched_timer = NULL;
}

int online_dev_bind_monitor(void *ctx, void *resource, void *remote, void *request)
{
    uint8_t i;
    char *payload = NULL;
    int payload_len = 0, dev_info_len = 0;
    char *key = NULL, *dev_name = NULL, *dev_info = NULL;

    payload = awss_cmp_get_coap_payload(request, &payload_len);
    if (payload == NULL || payload_len == 0) {
        goto CONNECTAP_MONITOR_END;
    }

    dev_info = json_get_value_by_name(payload, payload_len, AWSS_JSON_PARAM, &dev_info_len, NULL);
    if (dev_info == NULL || dev_info_len == 0) {
        goto CONNECTAP_MONITOR_END;
    }

    dev_name = os_zalloc(MAX_DEV_NAME_LEN + 1);
    key = os_zalloc(MAX_PK_LEN + 1);

    if (!dev_name || !key) {
        goto CONNECTAP_MONITOR_END;
    }

    if (awss_enrollee_get_dev_info(dev_info, dev_info_len, key, dev_name, NULL, NULL) < 0) {
        goto CONNECTAP_MONITOR_END;
    }

    for (i = 0; i < MAX_ENROLLEE_NUM; i++) {
        if (enrollee_info[i].state != ENR_CHECKIN_ONGOING) {
            continue;
        }

        if (strlen(dev_name) == enrollee_info[i].dev_name_len &&
            0 == memcmp(dev_name, enrollee_info[i].dev_name, enrollee_info[i].dev_name_len) &&
            strlen(key) == enrollee_info[i].pk_len &&
            0 == memcmp(key, enrollee_info[i].pk, enrollee_info[i].pk_len)) {
            enrollee_info[i].state = ENR_FREE;
        }
    }

CONNECTAP_MONITOR_END:
    if (dev_name) {
        HAL_Free(dev_name);
    }
    if (key) {
        HAL_Free(key);
    }
    return 0;
}

void awss_enrollee_checkin(void *pcontext, void *pclient, void *msg)
{
#define CHECK_IN_RSP_LEN  (64)
    char *packet = NULL;
    int len = 0, timeout = 0;
    int packet_len = CHECK_IN_RSP_LEN, dev_info_len = 0;
    char *key = NULL, *dev_name = NULL, *dev_info = NULL;
    uint32_t payload_len;
    char *payload;
    int ret;
    char reply[TOPIC_LEN_MAX] = {0};

    ret = awss_cmp_mqtt_get_payload(msg, &payload, &payload_len);

    if (ret != 0) {
        goto CHECKIN_FAIL;
    }

    if (payload == NULL || payload_len == 0) {
        goto CHECKIN_FAIL;
    }

    dev_name = os_zalloc(MAX_DEV_NAME_LEN + 1);
    packet = os_zalloc(CHECK_IN_RSP_LEN + 1);
    key = os_zalloc(MAX_PK_LEN + 1);

    if (!dev_name || !key || !packet) {
        goto CHECKIN_FAIL;
    }

    awss_debug("checkin len:%u, payload:%s\r\n", payload_len, payload);

    dev_info = json_get_value_by_name(payload, payload_len, AWSS_JSON_PARAM, &dev_info_len, NULL);
    if (dev_info == NULL || dev_info_len == 0) {
        goto CHECKIN_FAIL;
    }

    if (awss_enrollee_get_dev_info(dev_info, dev_info_len, key, dev_name, NULL, &timeout) < 0) {
        goto CHECKIN_FAIL;
    }

    enrollee_enable_somebody_checkin(key, dev_name, timeout);

    {
        char *id = NULL;
        char id_str[MSG_REQ_ID_LEN] = {0};
        id = json_get_value_by_name(payload, payload_len, AWSS_JSON_ID, &len, NULL);
        memcpy(id_str, id, len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : len);
        awss_build_packet(AWSS_CMP_PKT_TYPE_RSP, id_str, ILOP_VER, METHOD_EVENT_ZC_CHECKIN, "{}", 200, packet, &packet_len);
    }

    awss_build_topic(TOPIC_ZC_CHECKIN_REPLY, reply, TOPIC_LEN_MAX);
    awss_cmp_mqtt_send(reply, packet, packet_len, 1);

    HAL_Free(dev_name);
    HAL_Free(packet);
    HAL_Free(key);
    return;

CHECKIN_FAIL:
    if (dev_name) {
        HAL_Free(dev_name);
    }
    if (packet) {
        HAL_Free(packet);
    }
    if (key) {
        HAL_Free(key);
    }

    awss_warn("alink checkin failed");
    return;
}

static int enrollee_enable_somebody_cipher(char *key, char *dev_name, char *cipher)
{
    int i;

    awss_debug("key:%s, dev_name:%s, cipher:%s\r\n", key, dev_name, cipher);

    if (strlen(key) > MAX_PK_LEN ||
        strlen(dev_name) > MAX_DEV_NAME_LEN) {
        goto out;
    }

    for (i = 0; i < MAX_ENROLLEE_NUM; i++) {
        awss_debug("enrollee[%d] state %d", i, enrollee_info[i].state);
        if (enrollee_info[i].state != ENR_CHECKIN_ENABLE) {
            continue;
        }
        if (strlen(dev_name) == enrollee_info[i].dev_name_len &&
            0 == memcmp(dev_name, enrollee_info[i].dev_name, enrollee_info[i].dev_name_len) &&
            strlen(key) == enrollee_info[i].pk_len &&
            0 == memcmp(key, enrollee_info[i].pk, enrollee_info[i].pk_len)) {

            uint8_t *key_byte = os_zalloc(MAX_KEY_LEN + 1);
            if (NULL == key_byte) {
                goto out;
            }

            utils_str_to_hex(cipher, strlen(cipher), key_byte, MAX_KEY_LEN);

            memcpy((char *)&enrollee_info[i].key[0], key_byte, AES_KEY_LEN);

            HAL_Free(key_byte);

            awss_debug("enrollee[%d] state %d->%d", i, enrollee_info[i].state,
                       ENR_CHECKIN_CIPHER);
            enrollee_info[i].state = ENR_CHECKIN_CIPHER;

            HAL_Timer_Stop(checkin_timer);
            HAL_Timer_Start(checkin_timer, 1);
            return 1;/* match */
        }
    }

out:
    return 0;/* mismatch */
}

static int enrollee_enable_somebody_checkin(char *key, char *dev_name, int timeout)
{
    int i;

    awss_debug("key:%s, dev_name:%s, timeout:%u\r\n", key, dev_name, timeout);
    if (strlen(key) > MAX_PK_LEN ||
        strlen(dev_name) > MAX_DEV_NAME_LEN) {
        goto out;
    }

    for (i = 0; i < MAX_ENROLLEE_NUM; i++) {
        awss_debug("len:%u---%lu, name:%s---%s\r\n",
                   enrollee_info[i].dev_name_len, strlen(dev_name),
                   enrollee_info[i].dev_name, dev_name);
        awss_debug("enrollee[%d] state %d", i, enrollee_info[i].state);
        if (enrollee_info[i].state != ENR_FOUND) {
            continue;
        }

        if (strlen(dev_name) == enrollee_info[i].dev_name_len &&
            0 == memcmp(dev_name, enrollee_info[i].dev_name, enrollee_info[i].dev_name_len) &&
            strlen(key) == enrollee_info[i].pk_len &&
            0 == memcmp(key, enrollee_info[i].pk, enrollee_info[i].pk_len)) {

            enrollee_info[i].state = ENR_CHECKIN_ENABLE;
            enrollee_info[i].checkin_priority = 1;  /* TODO: not implement yet */
            enrollee_info[i].checkin_timeout = timeout <= 0 ? REGISTRAR_TIMEOUT : timeout;
            enrollee_info[i].checkin_timestamp = os_get_time_ms();

            HAL_Timer_Stop(checkin_timer);
            HAL_Timer_Start(checkin_timer, 1);
            return 1;/* match */
        }
    }

out:
    return 0;/* mismatch */
}

static int awss_request_cipher_key(int i)
{
    int packet_len = AWSS_REPORT_PKT_LEN - 1;
    char topic[TOPIC_LEN_MAX] = {0};
    char *param = NULL;
    char *packet = NULL;
    if (i < 0) {
        return -1;
    }
#define AWSS_DEV_CIPHER_FMT    "{\"awssVer\":%s,\"productKey\":\"%s\",\"deviceName\":\"%s\",\"cipherType\":%d, \"random\":\"%s\"}"

    param = os_zalloc(AWSS_REPORT_PKT_LEN);
    packet = os_zalloc(AWSS_REPORT_PKT_LEN);
    if (param == NULL || packet == NULL) {
        goto REQ_CIPHER_ERR;
    }

    {
        char id[MSG_REQ_ID_LEN] = {0};
        char rand_str[(RANDOM_MAX_LEN << 1) + 1] = {0};

        utils_hex_to_str(enrollee_info[i].random, RANDOM_MAX_LEN, rand_str, sizeof(rand_str));
        HAL_Snprintf(id, MSG_REQ_ID_LEN - 1, "\"%u\"", registrar_id ++);
        HAL_Snprintf(param, AWSS_REPORT_PKT_LEN - 1, AWSS_DEV_CIPHER_FMT,
                     AWSS_VER, enrollee_info[i].pk, enrollee_info[i].dev_name, enrollee_info[i].security, rand_str);
        awss_build_packet(AWSS_CMP_PKT_TYPE_REQ, id, ILOP_VER, METHOD_EVENT_ZC_CIPHER, param, 0, packet, &packet_len);
        HAL_Free(param);
    }

    awss_build_topic(TOPIC_ZC_CIPHER, topic, TOPIC_LEN_MAX);
    awss_cmp_mqtt_send(topic, packet, packet_len, 1);

    HAL_Free(packet);

    return 0;

REQ_CIPHER_ERR:
    if (param) {
        HAL_Free(param);
    }
    if (packet) {
        HAL_Free(packet);
    }

    return -1;
}

void awss_get_cipher_reply(void *pcontext, void *pclient, void *msg)
{
    int dev_info_len = 0;
    char *key = NULL, *dev_name = NULL, *dev_info = NULL, *cipher = NULL;
    uint32_t payload_len;
    char *payload;
    int ret;

    ret = awss_cmp_mqtt_get_payload(msg, &payload, &payload_len);

    if (ret != 0) {
        goto CIPHER_ERR;
    }

    if (payload == NULL || payload_len == 0) {
        goto CIPHER_ERR;
    }

    dev_name = os_zalloc(MAX_DEV_NAME_LEN + 1);
    cipher = os_zalloc(RANDOM_MAX_LEN * 2 + 1);
    key = os_zalloc(MAX_PK_LEN + 1);

    if (!dev_name || !key || !cipher) {
        goto CIPHER_ERR;
    }

    awss_debug("cipher len:%u, payload:%s\r\n", payload_len, payload);

    dev_info = json_get_value_by_name(payload, payload_len, AWSS_JSON_DEV_LIST, &dev_info_len, NULL);
    if (dev_info == NULL || dev_info_len == 0) {
        goto CIPHER_ERR;
    }

    if (awss_enrollee_get_dev_info(dev_info, dev_info_len, key, dev_name, cipher, NULL) < 0) {
        goto CIPHER_ERR;
    }

    enrollee_enable_somebody_cipher(key, dev_name, cipher);

    HAL_Free(dev_name);
    HAL_Free(cipher);
    HAL_Free(key);

    return;
CIPHER_ERR:
    if (dev_name) {
        HAL_Free(dev_name);
    }
    if (cipher) {
        HAL_Free(cipher);
    }
    if (key) {
        HAL_Free(key);
    }
    return;
}

/* 1 -- checkin onging, 0 -- idle */
static int enrollee_checkin(void)
{
    int pri = 65536;
    uint8_t i, check = 0;
    uint8_t checkin_new = 0xff, get_cipher = 0xff;

    for (i = 0; i < MAX_ENROLLEE_NUM; i++) {
        switch (enrollee_info[i].state) {
            case ENR_CHECKIN_ENABLE:
                if (pri > enrollee_info[i].checkin_priority) {
                    pri = enrollee_info[i].checkin_priority;
                    checkin_new = i;
                    check = 1;
                }
                break;
            case ENR_CHECKIN_CIPHER:
                get_cipher = i;
                check = 1;
                break;
            case ENR_CHECKIN_ONGOING:
                goto ongoing;
            default:
                break;
        }
    }

    awss_debug("cn:%d, ci:%d, c:%d\r\n", checkin_new, get_cipher, check);
    /* no device need to setup */
    if (check == 0) {
        return 0;
    }

    if (get_cipher != 0xff) {
        goto checkin_ongoing;
    }

    /* request cipher */
    awss_request_cipher_key(checkin_new);
    return 1;

    /* checkin_new: */
checkin_ongoing:
    awss_debug("enrollee[%d] state %d->%d", get_cipher,
               enrollee_info[get_cipher].state, ENR_CHECKIN_ONGOING);
    enrollee_info[get_cipher].state = ENR_CHECKIN_ONGOING;
    enrollee_info[get_cipher].checkin_timestamp = os_get_time_ms();
    registrar_raw_frame_init(&enrollee_info[get_cipher]);
    i = get_cipher;

    /* undergoing */
ongoing:
    registrar_raw_frame_send();
    awss_debug("registrar_raw_frame_send");
    if (time_elapsed_ms_since(enrollee_info[i].checkin_timestamp) > enrollee_info[i].checkin_timeout * 1000) {
        memset(&enrollee_info[i], 0, sizeof(enrollee_info[0]));
        awss_debug("enrollee[%d] state %d->%d", i,
                   enrollee_info[i].state, ENR_CHECKIN_END);
        enrollee_info[i].state = ENR_CHECKIN_END;/* FIXME: remove this state? */
        awss_debug("enrollee[%d] state %d->%d", i,
                   enrollee_info[i].state, ENR_FREE);
        enrollee_info[i].state = ENR_FREE;
        registrar_raw_frame_destroy();
    }

    HAL_Timer_Stop(checkin_timer);
    HAL_Timer_Start(checkin_timer, HAL_Awss_Get_Channelscan_Interval_Ms() * 15 / 16);

    return 1;
}

int awss_report_set_interval(char *key, char *dev_name, int interval)
{
    int i;

    awss_debug("key:%s, dev_name:%s, interval:%u\r\n", key, dev_name, interval);
    if (strlen(key) > MAX_PK_LEN ||
        strlen(dev_name) > MAX_DEV_NAME_LEN) {
        return -1;
    }

    for (i = 0; i < MAX_ENROLLEE_NUM; i++) {
        if (enrollee_info[i].state != ENR_FOUND) {
            continue;
        }

        if (strlen(dev_name) == enrollee_info[i].dev_name_len &&
            0 == memcmp(dev_name, enrollee_info[i].dev_name, enrollee_info[i].dev_name_len) &&
            strlen(key) == enrollee_info[i].pk_len &&
            0 == memcmp(key, enrollee_info[i].pk, enrollee_info[i].pk_len)) {

            enrollee_info[i].interval = interval <= 0 ? REGISTRAR_TIMEOUT : interval;
            if (checkin_timer == NULL) {
                checkin_timer = HAL_Timer_Create("checkin", (void (*)(void *))enrollee_checkin, NULL);
            }
            HAL_Timer_Stop(checkin_timer);
            HAL_Timer_Start(checkin_timer, 1);
            return 0;/* match */
        }
    }

    return -1;

}

static int awss_enrollee_get_dev_info(char *payload, int payload_len,
                                      char *product_key, char *dev_name,
                                      char *cipher, int *timeout)
{
    char *elem = NULL;
    int len = 0;
    if (product_key == NULL || dev_name == NULL) {
        return -1;
    }

    elem = json_get_value_by_name(payload, payload_len, AWSS_JSON_PK, &len, NULL);
    if (len > MAX_PK_LEN || elem == NULL) {
        return -1;
    }

    memcpy(product_key, elem, len);

    len = 0;
    elem = json_get_value_by_name(payload, payload_len, AWSS_JSON_DEV_NAME, &len, NULL);
    if (len > MAX_DEV_NAME_LEN || elem == NULL) {
        return -1;
    }

    memcpy(dev_name, elem, len);

    len = 0;
    elem = json_get_value_by_name(payload, payload_len, AWSS_JSON_PERIOD, &len, NULL);
    if (elem && timeout) {
        *timeout = atoi(elem);
    }

    len = 0;
    elem = json_get_value_by_name(payload, payload_len, AWSS_JSON_CIPHER, &len, NULL);
    if (elem && cipher && len <= RANDOM_MAX_LEN * 2) {
        memcpy(cipher, elem, len);
    }

    return 0;
}

void awss_report_enrollee_reply(void *pcontext, void *pclient, void *msg)
{
    int interval = 0;
    int dev_list_len = 0;
    char *dev_list = NULL;
    char *key = NULL, *dev_name = NULL;
    uint32_t payload_len;
    char *payload;
    int ret;
    char *str_pos, *entry;
    int entry_len, type;

    ret = awss_cmp_mqtt_get_payload(msg, &payload, &payload_len);

    if (ret != 0) {
        goto REPORT_REPLY_FAIL;
    }

    if (payload == NULL || payload_len == 0) {
        goto REPORT_REPLY_FAIL;
    }

    awss_debug("found reply:%s\r\n", payload);
    dev_name = os_zalloc(MAX_DEV_NAME_LEN + 1);
    key = os_zalloc(MAX_PK_LEN + 1);

    if (!dev_name || !key) {
        goto REPORT_REPLY_FAIL;
    }

    dev_list = json_get_value_by_name(payload, payload_len, AWSS_JSON_DEV_LIST, &dev_list_len, NULL);
    if (dev_list == NULL) {
        goto REPORT_REPLY_FAIL;
    }

    json_array_for_each_entry(dev_list, dev_list_len, str_pos, entry, entry_len, type) {
        memset(dev_name, 0,  MAX_DEV_NAME_LEN + 1);
        memset(key, 0, MAX_PK_LEN + 1);
        if (awss_enrollee_get_dev_info(entry, entry_len, key, dev_name, NULL, &interval) < 0) {
            continue;
        }

        awss_report_set_interval(key, dev_name, interval);
    }

    HAL_Free(dev_name);
    HAL_Free(key);
    return;

REPORT_REPLY_FAIL:
    if (dev_name) {
        HAL_Free(dev_name);
    }
    if (key) {
        HAL_Free(key);
    }

    awss_warn("ilop report enrollee failed");
    return;
}

int awss_report_enrollee(uint8_t *payload, int payload_len, signed char rssi)
{
    int i;
    char *payload_str = NULL;
    char *param = NULL, *packet = NULL;
    int packet_len = AWSS_REPORT_PKT_LEN - 1;
    char topic[TOPIC_LEN_MAX] = {0};

    payload_str = os_zalloc(payload_len * 2 + 1);
    param = os_zalloc(AWSS_REPORT_PKT_LEN);
    packet = os_zalloc(AWSS_REPORT_PKT_LEN);
    if (!payload_str || !param || !packet) {
        goto REPORT_FAIL;
    }

    {
        char id[MSG_REQ_ID_LEN] = {0};
        uint8_t bssid[OS_ETH_ALEN] = {0};
        char ssid[OS_MAX_SSID_LEN + 1] = {0};
        char bssid_str[OS_ETH_ALEN * 2 + 6] = {0};

        HAL_Wifi_Get_Ap_Info(ssid, NULL, bssid);
        sprintf(bssid_str, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

        for (i = 0; i < payload_len; i ++) {
            sprintf(&payload_str[i * 2], "%02X", payload[i]);
        }

        payload_str[payload_len * 2] = '\0'; /* sprintf not add '\0' in the end of string in qcom */

        HAL_Snprintf(id, MSG_REQ_ID_LEN - 1, "\"%u\"", registrar_id ++);

        HAL_Snprintf(param, AWSS_REPORT_PKT_LEN - 1, AWSS_REPORT_PARAM_FMT,
                     AWSS_VER, ssid, bssid_str, rssi > 0 ? rssi - 256 : rssi, payload_str);
        HAL_Free(payload_str);
        awss_build_packet(AWSS_CMP_PKT_TYPE_REQ, id, ILOP_VER, METHOD_EVENT_ZC_ENROLLEE, param, 0, packet, &packet_len);
        HAL_Free(param);
    }

    awss_build_topic(TOPIC_ZC_ENROLLEE, topic, TOPIC_LEN_MAX);
    awss_debug("topic:%s, packet:%s, method:%s\r\n", topic, packet, METHOD_EVENT_ZC_ENROLLEE);

    awss_cmp_mqtt_send(topic, packet, packet_len, 1);

    HAL_Free(packet);
    return 0;

REPORT_FAIL:
    if (payload_str) {
        HAL_Free(payload_str);
    }
    if (packet) {
        HAL_Free(packet);
    }
    if (param) {
        HAL_Free(param);
    }

    return -1;
}

/* consumer */
static void enrollee_report(void)
{
    int i;
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
    char ssid[OS_MAX_SSID_LEN + 1] = {0};
    HAL_Wifi_Get_Ap_Info(ssid, NULL, NULL);
    if (!strcmp(ssid, DEFAULT_SSID) || !strcmp(ssid, ADHA_SSID)) {
        return;    /* ignore enrollee in 'aha' or 'adha' mode */
    }
#endif

    /* evict timeout enrollee */
    for (i = 0; i < MAX_ENROLLEE_NUM; i++) {
        struct enrollee_info *enrollee = &enrollee_info[i];
        switch (enrollee->state) {
            case ENR_FOUND: {
                if (time_elapsed_ms_since(enrollee->report_timestamp) > enrollee->interval * 1000) {
                    memset(enrollee, 0, sizeof(enrollee_info[0]));
                    enrollee->state = ENR_FREE;
                }
                break;
            }
            case ENR_IN_QUEUE: {
                uint16_t idx = 0;
                int ret = -1;
                uint16_t payload_len = 1 + enrollee->dev_name_len + 1 + enrollee->pk_len +
                                       1 + enrollee->rand_len + 3 + enrollee->sign_len;
                uint8_t *payload = HAL_Malloc(payload_len + 1);
                if (payload == NULL) {
                    break;
                }

                payload[idx ++] = enrollee->dev_name_len;
                memcpy(&payload[idx], enrollee->dev_name, enrollee->dev_name_len);
                idx += enrollee->dev_name_len;

                payload[idx ++] = enrollee->pk_len;
                memcpy(&payload[idx], enrollee->pk, enrollee->pk_len);
                idx += enrollee->pk_len;

                payload[idx ++] = enrollee->rand_len;
                memcpy(&payload[idx], &enrollee->random, enrollee->rand_len);
                idx += enrollee->rand_len;

                payload[idx ++] = enrollee->security;
                payload[idx ++] = enrollee->sign_method;
                payload[idx ++] = enrollee->sign_len;
                memcpy(&payload[idx], &enrollee->sign, enrollee->sign_len);
                idx += enrollee->sign_len;

                ret = awss_report_enrollee(payload, idx, enrollee->rssi);

                enrollee->state = ENR_FOUND;
                enrollee->report_timestamp = os_get_time_ms();

                awss_trace("enrollee report result:%s, period:%dms\n",
                           ret == 0 ? "success" : "failed",
                           enrollee->interval * 1000);

                HAL_Free(payload);
                break;
            }
            default:
                break;
        }
    }
}

int enrollee_put(struct enrollee_info *in);

int process_enrollee_ie(const uint8_t *ie, signed char rssi)
{
    struct enrollee_info tmp_enrollee = {0};
    /* suppose enrollee_ie is complete */
#define ENROLLEE_IE_HDR         (6)
    /* copy to tmp_enrollee */
    ie += ENROLLEE_IE_HDR;

    if (ie[0] != DEVICE_TYPE_VERSION) {
        awss_warn("enrollee(devtype/ver=%d not supported!", ie[0]);
        return -1;
    }
    tmp_enrollee.dev_type_ver = ie[0];
    ie ++;/* eating dev_type_ver */

    if (ie[0] > MAX_DEV_NAME_LEN) {
        awss_warn("enrollee(dev_name_len=%d out of range!\r\n", ie[0]);
        return -1;
    }
    tmp_enrollee.dev_name_len = ie[0];
    memcpy(tmp_enrollee.dev_name, &ie[1], ie[0]);
    ie += ie[0] + 1; /* eating dev_name[n], dev_name_len */

    if (ie[0] != ENROLLEE_FRAME_TYPE) {
        awss_warn("enrollee(frametype=%d invalid!\r\n", ie[0]);
        return -1;
    }
    tmp_enrollee.frame_type = ie[0];
    ie ++;/* eating frame type */

    if (ie[0] > MAX_PK_LEN) {
        awss_warn("enrollee(pk_len=%d invalid!\r\n", ie[0]);
        return -1;
    }
    tmp_enrollee.pk_len = ie[0];
    memcpy(tmp_enrollee.pk, &ie[1], ie[0]);
    ie += ie[0] + 1; /* eating pk[n], pk_len */

    if (ie[0] != RANDOM_MAX_LEN) {
        awss_warn("enrollee(rand_len=%d invalid!\r\n", ie[0]);
        return -1;
    }
    tmp_enrollee.rand_len = ie[0];
    memcpy(tmp_enrollee.random, &ie[1], RANDOM_MAX_LEN);
    ie += ie[0] + 1; /* eating random[n], rand_len */

    if (ie[0] > 5 || ie[0] < 3) {
        awss_warn("enrollee(security=%d invalid!\r\n", ie[0]);
        return -1;
    }
    if (ie[1] > 1) {
        awss_warn("enrollee(sign_method=%d invalid!\r\n", ie[1]);
        return -1;
    }
    if (ie[2] != ENROLLEE_SIGN_SIZE) {
        awss_warn("enrollee(sign_len=%d invalid!\r\n", ie[2]);
        return -1;
    }
    tmp_enrollee.security = ie[0];
    tmp_enrollee.sign_method = ie[1];
    tmp_enrollee.sign_len = ie[2];

    memcpy(tmp_enrollee.sign, &ie[3], ie[2]);
    ie += ie[2] + 3; /* eating signature[n], security, sign_method, sign_len */

    tmp_enrollee.rssi = rssi;

    enrollee_put(&tmp_enrollee);

    return 0;
}

/* producer */
/*
 * 1: already saved, update timestamp
 * 0: new saved
 * -1: no slot to save, drop
 */
int enrollee_put(struct enrollee_info *in)
{
    uint8_t i, empty_slot = MAX_ENROLLEE_NUM;
    do {
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
        char ssid[OS_MAX_SSID_LEN + 1] = {0};
#endif
        /* reduce stack used */
        if (in == NULL || !HAL_Sys_Net_Is_Ready()) { /* not ready to work as registerar */
            return -1;
        }
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
        HAL_Wifi_Get_Ap_Info(ssid, NULL, NULL);
        if (!strcmp(ssid, DEFAULT_SSID) || !strcmp(ssid, ADHA_SSID)) {
            return -1;    /* ignore enrollee in 'aha' or 'adha' mode */
        }
#endif
    } while (0);

    for (i = 0; i < MAX_ENROLLEE_NUM; i++) {
        if (enrollee_info[i].state) {
            if (in->dev_name_len == enrollee_info[i].dev_name_len &&
                0 == memcmp(in->dev_name, enrollee_info[i].dev_name, enrollee_info[i].dev_name_len) &&
                in->pk_len == enrollee_info[i].pk_len &&
                0 == memcmp(in->pk, enrollee_info[i].pk, enrollee_info[i].pk_len)) {
                if (enrollee_info[i].state == ENR_FOUND &&
                    time_elapsed_ms_since(enrollee_info[i].report_timestamp) > enrollee_info[i].interval * 1000) {
                    if (enrollee_report_timer == NULL) {
                        enrollee_report_timer = HAL_Timer_Create("enrollee", (void (*)(void *))enrollee_report, NULL);
                    }
                    HAL_Timer_Stop(enrollee_report_timer);
                    HAL_Timer_Start(enrollee_report_timer, 1);
                }
                if (enrollee_info[i].state != ENR_IN_QUEUE) { /* already reported */
                    return 1;
                }
                memcpy(&enrollee_info[i], in, ENROLLEE_INFO_HDR_SIZE);
                enrollee_info[i].rssi = (2 * enrollee_info[i].rssi + in->rssi) / 3;
                return 1;/* wait for report */
            }
        } else if (enrollee_info[i].state == ENR_FREE && empty_slot >= MAX_ENROLLEE_NUM) {
            empty_slot = i;
        }
    }

    if (empty_slot >= MAX_ENROLLEE_NUM) {
        return -1;    /* no slot to save */
    }

    /* new enrollee */
    memset(&enrollee_info[empty_slot], 0, sizeof(struct enrollee_info));
    memcpy(&enrollee_info[empty_slot], in, ENROLLEE_INFO_HDR_SIZE);
    enrollee_info[empty_slot].rssi = in->rssi;
    enrollee_info[empty_slot].state = ENR_IN_QUEUE;
    enrollee_info[empty_slot].checkin_priority = 1; /* smaller means high pri */
    enrollee_info[empty_slot].interval = REGISTRAR_TIMEOUT;
    enrollee_info[empty_slot].checkin_timeout = REGISTRAR_TIMEOUT;
    awss_debug("new enrollee[%d] dev_name:%s time:%x",
               empty_slot, in->dev_name, os_get_time_ms());

    if (enrollee_report_timer == NULL) {
        enrollee_report_timer = HAL_Timer_Create("enrollee", (void (*)(void *))enrollee_report, NULL);
    }
    HAL_Timer_Stop(enrollee_report_timer);
    HAL_Timer_Start(enrollee_report_timer, 1);

    return 0;
}

extern const uint8_t *cfg80211_find_vendor_ie(
            uint32_t oui, uint8_t oui_type,
            const uint8_t *ies, int len);
/**
 * @brief management frame handler
 *
 * @param[in] buffer @n 80211 raw frame or ie(information element) buffer
 * @param[in] len @n buffer length
 * @param[in] buffer_type @n 0 when buffer is a 80211 frame,
 *                          1 when buffer only contain IE info
 * @return None.
 * @see None.
 * @note None.
 */
void awss_wifi_mgnt_frame_callback(uint8_t *buffer, int length, signed char rssi, int buffer_type)
{
#define MGMT_BEACON     (0x80)
#define MGMT_PROBE_REQ  (0x40)
#define MGMT_PROBE_RESP (0x50)

    /* fc(2) + dur(2) + da(6) + sa(6) + bssid(6) + seq(2) */
#define MGMT_HDR_LEN    (24)

    int type = buffer[0], len = 0, eid;
    const uint8_t *ie;

    if (buffer_type) {
        ie = buffer;
        goto ie_handler;
    }

    switch (type) {
        case MGMT_BEACON:
            /* awss_trace("beacon"); */
            buffer += MGMT_HDR_LEN + 12;/* hdr(24) + 12(timestamp, beacon_interval, cap) */
            length -= MGMT_HDR_LEN + 12;

            eid = buffer[0];
            len = buffer[1];
            if (eid != 0) {
                /* awss_warn("error eid, should be 0!"); */
                return;
            }

            /* skip ssid */
            buffer += 2;
            buffer += len;
            length -= len;

            goto find_ie;
            break;
        case MGMT_PROBE_REQ:
            /* awss_trace("probe req\n"); */
            buffer += MGMT_HDR_LEN;
            length -= MGMT_HDR_LEN;

find_ie:
            ie = cfg80211_find_vendor_ie((uint32_t)WLAN_OUI_ALIBABA,
                                         (uint8_t)WLAN_OUI_TYPE_ENROLLEE,
                                         (const uint8_t *)buffer, (int)length);
            if (ie) {
ie_handler:
                /* awss_trace("ie found to be processed\n"); */
                process_enrollee_ie(ie, rssi);
            }
            break;
        case MGMT_PROBE_RESP:
            /* awss_trace("probe resp"); */
            break;
        default:
            /* awss_trace("frame (%d): %02x \n", length, type); */
            break;
    }
}

static uint8_t *registrar_frame;
static int registrar_frame_len;

static void registrar_raw_frame_init(struct enrollee_info *enr)
{
    int len, ie_len;

    char passwd[OS_MAX_PASSWD_LEN + 1] = {0};
    char ssid[OS_MAX_SSID_LEN + 1] = {0};
    uint8_t bssid[OS_ETH_ALEN] = {0};
    int ssid_len, passwd_len;

    HAL_Wifi_Get_Ap_Info(ssid, passwd, bssid);
    ssid_len = strlen(ssid);
    if (ssid_len > OS_MAX_SSID_LEN - 1) {
        ssid_len = OS_MAX_SSID_LEN - 1;
    }

    passwd_len = strlen(passwd);
    if (passwd_len > OS_MAX_PASSWD_LEN - 1) {
        passwd_len = OS_MAX_PASSWD_LEN - 1;
    }

    ie_len = ENROLLEE_SIGN_SIZE + ssid_len + passwd_len + REGISTRAR_IE_FIX_LEN;
    registrar_frame_len = sizeof(probe_req_frame) + ie_len;

    registrar_frame = HAL_Malloc(registrar_frame_len);
    if (!registrar_frame) {
        awss_err("error: malloc size %d faild\r\n", registrar_frame_len);
        return;
    }

    /* construct the registrar frame right now */
    len = sizeof(probe_req_frame) - FCS_SIZE;
    memcpy(registrar_frame, probe_req_frame, len);

    registrar_frame[len ++] = 221; /* vendor ie */
    registrar_frame[len ++] = ie_len - 2; /* exclude 221 & len */
    registrar_frame[len ++] = 0xD8;
    registrar_frame[len ++] = 0x96;
    registrar_frame[len ++] = 0xE0;
    registrar_frame[len ++] = 0xAB;/* OUI type */
    registrar_frame[len ++] = DEVICE_TYPE_VERSION;/* version & dev type */
    registrar_frame[len ++] = enr->sign_len;/* dev signature len*/
    memcpy(&registrar_frame[len], enr->sign, enr->sign_len);
    len += enr->sign_len;
    registrar_frame[len ++] = REGISTRAR_FRAME_TYPE;/* frame type */

    registrar_frame[len ++] = ssid_len;
    memcpy(&registrar_frame[len], ssid, ssid_len);
    len += ssid_len;

    registrar_frame[len ++] = passwd_len;

    {
        p_aes128_t aes = HAL_Aes128_Init(&enr->key[0], enr->random, PLATFORM_AES_ENCRYPTION);
        HAL_Aes128_Cfb_Encrypt(aes, (uint8_t *)passwd, passwd_len, (uint8_t *)&registrar_frame[len]);
        HAL_Aes128_Destroy(aes);
    }

    len += passwd_len;

    memcpy(&registrar_frame[len], bssid, ETH_ALEN);
    len += ETH_ALEN;

    memcpy(&registrar_frame[len],
           &probe_req_frame[sizeof(probe_req_frame) - FCS_SIZE], FCS_SIZE);

    /* update probe request frame src mac */
    os_wifi_get_mac(registrar_frame + SA_POS);

    {
        /* dump registrar info */
        awss_debug("dump registrar info:");
        dump_hex(registrar_frame, registrar_frame_len, 16);
    }
}

static void registrar_raw_frame_destroy(void)
{
    if (registrar_frame_len) {
        HAL_Free(registrar_frame);
        registrar_frame = NULL;
        registrar_frame_len = 0;
    }
}

static void registrar_raw_frame_send(void)
{
    /* suppose registrar_frame was ready
     * @see enrollee_checkin()
     */
    int ret = HAL_Wifi_Send_80211_Raw_Frame(FRAME_PROBE_REQ, registrar_frame,
                                            registrar_frame_len);
    if (ret) {
        awss_warn("send failed");
    }
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
