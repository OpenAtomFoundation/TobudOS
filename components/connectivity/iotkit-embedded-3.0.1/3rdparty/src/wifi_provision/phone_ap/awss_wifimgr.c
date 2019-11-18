/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
#define WIFI_APINFO_LIST_LEN    (512)
#define DEV_SIMPLE_ACK_LEN      (64)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

static char g_req_msg_id[MSG_REQ_ID_LEN];
static platform_netaddr_t g_wifimgr_req_sa;

static void *scan_req_timer = NULL;
static void *scan_tx_wifilist_timer = NULL;
static void wifimgr_scan_request();
static void wifimgr_scan_tx_wifilist();

static char wifi_scan_runninng = 0;
static void *g_scan_mutex;

typedef struct scan_list {
    list_head_t entry;
    void *data;
} scan_list_t;

static LIST_HEAD(g_scan_list);

int wifimgr_scan_init(void)
{
    if (wifi_scan_runninng)
        return 0;

    g_scan_mutex = HAL_MutexCreate();
    INIT_LIST_HEAD(&g_scan_list);
    wifi_scan_runninng = 1;
    return 0;
}

static void wifimgr_scan_tx_wifilist()
{
    scan_list_t *item =  NULL, *next = NULL;

    char topic[TOPIC_LEN_MAX] = {0};
    awss_build_topic((const char *)TOPIC_AWSS_WIFILIST, topic, TOPIC_LEN_MAX);

    HAL_MutexLock(g_scan_mutex);
    list_for_each_entry_safe(item, next, &g_scan_list, entry, scan_list_t) {
        if (item && item->data) {
            if (0 != awss_cmp_coap_ob_send(item->data, strlen((char *)item->data),
                                           &g_wifimgr_req_sa, topic, NULL)) {
                awss_debug("sending failed.");
            }
            HAL_Free(item->data);
        }
        list_del(&item->entry);
        HAL_Free(item);
        item= NULL;
    }
    HAL_MutexUnlock(g_scan_mutex);
}

static int awss_scan_cb(const char ssid[PLATFORM_MAX_SSID_LEN],
                        const uint8_t bssid[ETH_ALEN],
                        enum AWSS_AUTH_TYPE auth,
                        enum AWSS_ENC_TYPE encry,
                        uint8_t channel, signed char rssi,
                        int last_ap)
{
#define ONE_AP_INFO_LEN_MAX           (141)
    static char *aplist = NULL;
    static int msg_len = 0;

    if (aplist == NULL) {
        aplist = os_zalloc(WIFI_APINFO_LIST_LEN);
        if (aplist == NULL)
            return SHUB_ERR;

        msg_len = 0;
        msg_len += HAL_Snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1, "{\"awssVer\":%s, \"wifiList\":[", AWSS_VER);
      }
    if ((ssid != NULL) && (ssid[0] != '\0')) {
        uint8_t bssid_connected[ETH_ALEN] = {0};
        char *other_apinfo = os_zalloc(64);
        char *encode_ssid = os_zalloc(OS_MAX_SSID_LEN * 2 + 1);
        int ssid_len = strlen(ssid);
        ssid_len = ssid_len > OS_MAX_SSID_LEN - 1 ? OS_MAX_SSID_LEN - 1 : ssid_len;

        HAL_Wifi_Get_Ap_Info(NULL, NULL, bssid_connected);

        if (other_apinfo && encode_ssid) {
            if (memcmp(bssid_connected, bssid, ETH_ALEN) == 0) {
                HAL_Snprintf(other_apinfo, 64 - 1, "\"auth\":\"%d\",\"connected\":\"1\"", auth);
            } else {
                HAL_Snprintf(other_apinfo, 64 - 1, "\"auth\":\"%d\"", auth);
            }
            if (is_utf8(ssid, ssid_len)) {
                strncpy(encode_ssid, (const char *)ssid, ssid_len);
                msg_len += HAL_Snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1,
                                    "{\"ssid\":\"%s\",\"bssid\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"rssi\":\"%d\",%s},",
                                    encode_ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
                                    rssi > 0 ? rssi - 256 : rssi, other_apinfo);
            } else {
                utils_hex_to_str((uint8_t *)ssid, ssid_len, encode_ssid, OS_MAX_SSID_LEN * 2);
                msg_len += HAL_Snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1,
                                    "{\"xssid\":\"%s\",\"bssid\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"rssi\":\"%d\",%s},",
                                    encode_ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
                                    rssi > 0 ? rssi - 256 : rssi, other_apinfo);
            }
        }

        if (other_apinfo) HAL_Free(other_apinfo);
        if (encode_ssid) HAL_Free(encode_ssid);
    }
    awss_debug("last_ap:%u\r\n", last_ap);

    if (last_ap || WIFI_APINFO_LIST_LEN < msg_len + ONE_AP_INFO_LEN_MAX + strlen(AWSS_ACK_FMT)) {
        uint32_t tlen; 
        char *msg_aplist = NULL;
        scan_list_t *list = NULL;
        if (last_ap)
            AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_SCAN_STOP);
        if (aplist[msg_len - 1] == ',')
            msg_len--;    /* eating the last ',' */
        msg_len += HAL_Snprintf(aplist + msg_len, WIFI_APINFO_LIST_LEN - msg_len - 1, "]}");

        tlen = DEV_SIMPLE_ACK_LEN + msg_len;
        msg_len = 0;
        msg_aplist = os_zalloc(tlen + 1);
        if (!msg_aplist) {
            HAL_Free(aplist);
            aplist = NULL;
            return SHUB_ERR;
        }

        HAL_Snprintf(msg_aplist, tlen, AWSS_ACK_FMT, g_req_msg_id, 200, aplist);
        HAL_Free(aplist);
        aplist = NULL;

        list = (scan_list_t *)os_zalloc(sizeof(scan_list_t));
        if (!list) {
            awss_debug("scan list fail\n");
            HAL_Free(msg_aplist);
            return SHUB_ERR;
        }
        list->data = msg_aplist;
        HAL_MutexLock(g_scan_mutex);
        list_add(&list->entry, &g_scan_list);
        HAL_MutexUnlock(g_scan_mutex);

        if (last_ap) {
            if (scan_tx_wifilist_timer == NULL)
                scan_tx_wifilist_timer = HAL_Timer_Create("wifilist", (void (*)(void *))wifimgr_scan_tx_wifilist, NULL);
            HAL_Timer_Stop(scan_tx_wifilist_timer);
            HAL_Timer_Start(scan_tx_wifilist_timer, 1);
        }
        awss_debug("sending message to app: %s\n", msg_aplist);
    }

    return 0;
}

static void wifimgr_scan_request()
{
    wifimgr_scan_init();

    AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_SCAN_START);
    HAL_Wifi_Scan(&awss_scan_cb);
}

/*
 * @desc: ????getWifiList??Ϣ
 *
 */
int wifimgr_process_get_wifilist_request(void *ctx, void *resource, void *remote, void *request)
{
    char buf[DEV_SIMPLE_ACK_LEN] = {0};
    char *msg = NULL, *id = NULL;
    int len = 0, id_len = 0;
    char topic[TOPIC_LEN_MAX] = {0};

    msg = awss_cmp_get_coap_payload(request, &len);
    if (msg == NULL || len == 0)
        return -1;

    if (scan_req_timer == NULL)
        scan_req_timer = HAL_Timer_Create("scan_req", (void (*)(void *))wifimgr_scan_request, NULL);
    HAL_Timer_Stop(scan_req_timer);

    id = json_get_value_by_name(msg, len, "id", &id_len, 0);
    memset(g_req_msg_id, 0, sizeof(g_req_msg_id));
    if (id && id_len < sizeof(g_req_msg_id) - 1)
        memcpy(g_req_msg_id, id, id_len);

    HAL_Snprintf(buf, DEV_SIMPLE_ACK_LEN - 1, AWSS_ACK_FMT, g_req_msg_id, 200, "\"success\"");

    awss_debug("sending message to app: %s\n", buf);
    awss_build_topic((const char *)TOPIC_AWSS_WIFILIST, topic, TOPIC_LEN_MAX);
    memcpy(&g_wifimgr_req_sa, remote, sizeof(g_wifimgr_req_sa));
    if (0 != awss_cmp_coap_send_resp(buf, strlen(buf), &g_wifimgr_req_sa, topic, request, NULL, NULL, 0))
        awss_debug("sending failed.");

    HAL_Timer_Start(scan_req_timer, 1);

    return SHUB_OK;
}

int wifimgr_process_mcast_get_device_info(void *ctx, void *resource, void *remote, void *request)
{
    return process_get_device_info(ctx, resource, remote, request, 1, AWSS_NOTIFY_DEV_RAND_SIGN);
}

int wifimgr_process_ucast_get_device_info(void *ctx, void *resource, void *remote, void *request)
{
    return process_get_device_info(ctx, resource, remote, request, 0, AWSS_NOTIFY_DEV_RAND_SIGN);
}
#define WLAN_CONNECTION_TIMEOUT     (30 * 1000) /* 30 seconds */
int switch_ap_done = 0;

void zconfig_force_destroy(void);
int wifimgr_process_switch_ap_request(void *ctx, void *resource, void *remote, void *request)
{
    char ssid[PLATFORM_MAX_SSID_LEN * 2 + 1] = {0}, passwd[PLATFORM_MAX_PASSWD_LEN + 1] = {0};
    int str_len = 0, success = 1, i  = 0, len = 0, enc_lvl = SEC_LVL_OPEN;
    char req_msg_id[MSG_REQ_ID_LEN] = {0};
    char *str = NULL, *buf = NULL;
    char bssid[ETH_ALEN] = {0};
    char msg[128] = {0};
    char ssid_found = 0;
    char topic[TOPIC_LEN_MAX] = {0};

    static char switch_ap_parsed = 0;
    if (switch_ap_parsed != 0)
        return SHUB_ERR;

    switch_ap_parsed = 1;

    buf = awss_cmp_get_coap_payload(request, &len);
    str = json_get_value_by_name(buf, len, "id", &str_len, 0);
    memcpy(req_msg_id, str, str_len > MSG_REQ_ID_LEN - 1 ? MSG_REQ_ID_LEN - 1 : str_len);
    awss_debug("switch ap, len:%u, %s\r\n", len, buf);
    buf = json_get_value_by_name(buf, len, "params", &len, 0);

    do {
        HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, 200, "\"success\"");

        str_len = 0;
        str = json_get_value_by_name(buf, len, "ssid", &str_len, 0);
        awss_debug("ssid, len:%u, %s\r\n", str_len, str != NULL ? str : "NULL");
        if (str && (str_len < PLATFORM_MAX_SSID_LEN)) {
            memcpy(ssid, str, str_len);
            ssid_found = 1;
        }

        if (!ssid_found) {
            str_len = 0;
            str = json_get_value_by_name(buf, len, "xssid", &str_len, 0);
            if (str && (str_len < PLATFORM_MAX_SSID_LEN * 2 - 1)) {
                uint8_t decoded[OS_MAX_SSID_LEN] = {0};
                int len = str_len / 2;
                memcpy(ssid, str, str_len);
                utils_str_to_hex(ssid, str_len, decoded, OS_MAX_SSID_LEN);
                memcpy(ssid, (const char *)decoded, len);
                ssid[len] = '\0';
            } else {
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -1, "\"ssid error\"");
                success = 0;
                break;
            }
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "bssid", &str_len, 0);
        if (str) os_wifi_str2mac(str, bssid);

        str_len = 0;
        str = json_get_value_by_name(buf, len, "cipherType", &str_len, 0);
        if (!str) {
            success = 0;
            HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"no security level error\"");
            break;
        }

        enc_lvl = atoi(str);
        if (enc_lvl != awss_get_encrypt_type()) {
            success = 0;
            HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"security level error\"");
            break;
        }

        str_len = 0;
        str = json_get_value_by_name(buf, len, "passwd", &str_len, 0);
        /* TODO: empty passwd is allow? json parse "passwd":"" result is NULL? */
        switch (enc_lvl) {
            case SEC_LVL_AES256:
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -4, "\"aes256 not support\"");
                success = 0;
                break;
            default:
                break;
        }

        if (success == 0)
            break;

        if (0 == enc_lvl) {
            if (str_len < PLATFORM_MAX_PASSWD_LEN) {
                memcpy(passwd, str, str_len);
            } else {
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -2, "\"passwd len error\"");
                success = 0;
            }
        } else {
            if (str_len < (PLATFORM_MAX_PASSWD_LEN * 2) - 1) {
                char encoded[PLATFORM_MAX_PASSWD_LEN * 2 + 1] = {0};
                memcpy(encoded, str, str_len);
                aes_decrypt_string(encoded, passwd, str_len,
                        0, awss_get_encrypt_type(), 1, (const char *)aes_random);
            } else {
                HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id, -3, "\"passwd len error\"");
                AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
                success = 0;
            }
        }

        if (success && is_utf8(passwd, strlen(passwd)) == 0) {
            HAL_Snprintf(msg, sizeof(msg) - 1, AWSS_ACK_FMT, req_msg_id,
                     enc_lvl == SEC_LVL_OPEN ? -2 : -3 , "\"passwd content error\"");
            AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
            success = 0;
        }
    } while (0);

    awss_devinfo_notify_stop();
    awss_dev_bind_notify_stop();

    awss_debug("Sending message to app: %s", msg);
    awss_debug("switch to ap: '%s'", ssid); 
    awss_build_topic((const char *)TOPIC_AWSS_SWITCHAP, topic, TOPIC_LEN_MAX);
    for (i = 0; i < 5; i ++) {
        if (0 != awss_cmp_coap_send_resp(msg, strlen(msg), remote, topic, request, NULL, NULL, 0)) {
            awss_debug("sending failed.");
        } else {
            awss_debug("sending succeeded.");
        }
    }

    HAL_SleepMs(1000);

    if (!success)
        goto SWITCH_AP_END;
#ifdef AWSS_SUPPORT_APLIST
    do {
        struct ap_info * aplist = NULL;
        aplist = zconfig_get_apinfo_by_ssid((uint8_t *)ssid);
        awss_debug("connect '%s'", ssid);
        if (aplist) {
            memcpy(bssid, aplist->mac, ETH_ALEN);
            awss_debug("bssid: %02x:%02x:%02x:%02x:%02x:%02x", \
                       bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        }
    } while (0);
#endif
    AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_START);
    if (0 != HAL_Awss_Connect_Ap(WLAN_CONNECTION_TIMEOUT,
                                ssid, passwd,
                                AWSS_AUTH_TYPE_INVALID,
                                AWSS_ENC_TYPE_INVALID,
                                (uint8_t *)bssid, 0)) {
    } else {
        AWSS_UPDATE_STATIS(AWSS_STATIS_CONN_ROUTER_IDX, AWSS_STATIS_TYPE_TIME_SUC);
        AWSS_UPDATE_STATIS(AWSS_STATIS_PAP_IDX, AWSS_STATIS_TYPE_TIME_SUC);
        switch_ap_done = 1;
        awss_close_aha_monitor();
        HAL_MutexDestroy(g_scan_mutex);
        g_scan_mutex = NULL;
        wifi_scan_runninng = 0;
        awss_stop_timer(scan_req_timer);
        scan_req_timer = NULL;
        awss_stop_timer(scan_tx_wifilist_timer);
        scan_tx_wifilist_timer = NULL;

        zconfig_force_destroy();

        produce_random(aes_random, sizeof(aes_random));
    }
    awss_debug("connect '%s' %s\r\n", ssid, switch_ap_done == 1 ? "success" : "fail");

SWITCH_AP_END:
    switch_ap_parsed = 0;
    return SHUB_OK;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
