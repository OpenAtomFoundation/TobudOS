/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

struct adha_info *adha_aplist = NULL;

const char *zc_adha_ssid = "adha";
const char *zc_adha_passwd = "08d9f22c60157fd01f57645d791a0b610fe0a558c104d6a1f9d9c0a9913c";

#ifdef AWSS_SUPPORT_ADHA

#define ADHA_WORK_CYCLE      (5 * 1000)
#define ADHA_PROBE_PKT_LEN   (50)
#define ADHA_SA_OFFSET       (10)

static const uint8_t adha_probe_req_frame[ADHA_PROBE_PKT_LEN] = {
    0x40, 0x00,  /* mgnt type, frame control */
    0x00, 0x00,  /* duration */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* DA */
    0x28, 0xC2, 0xDD, 0x61, 0x68, 0x83,  /* SA, to be replaced with wifi mac */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* BSSID */
    0xC0, 0x79,  /* seq */
    0x00, 0x04, 0x61, 0x64, 0x68, 0x61,  /* ssid, adha */
    0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x8C, 0x92, 0x98, 0xA4,  /*supported rates */
    0x32, 0x04, 0xB0, 0x48, 0x60, 0x6C,  /* extended supported rates */
    0x3F, 0x84, 0x10, 0x9E  /* FCS */
};

static void *adha_timer = NULL;
static volatile char adha_switch = 0;

static void adha_monitor(void);

static void adha_monitor(void)
{
    adha_switch = 1;
}

int awss_is_ready_switch_next_adha(void)
{
    return adha_switch > 0;
}

int awss_open_adha_monitor(void)
{
    adha_switch = 0;
    if (adha_timer == NULL)
        adha_timer = (void *)HAL_Timer_Create("adha", (void (*)(void *))adha_monitor, NULL);
    if (adha_timer == NULL)
        return -1;

    HAL_Timer_Stop(adha_timer);
    HAL_Timer_Start(adha_timer, ADHA_WORK_CYCLE);
    return 0;
}

int awss_close_adha_monitor(void)
{
    awss_stop_timer(adha_timer);
    adha_timer = NULL;
    adha_switch = 0;
    return 0;
}

int awss_recv_callback_adha_ssid(struct parser_res *res)
{
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    AWSS_UPDATE_STATIS(AWSS_STATIS_ROUTE_IDX, AWSS_STATIS_TYPE_TIME_START);
    awss_debug("found adha ssid: %s\r\n", zc_adha_ssid);

    strncpy((char *)zc_ssid, zc_adha_ssid, ZC_MAX_SSID_LEN - 1);
    strncpy((char *)zc_passwd, zc_adha_passwd, ZC_MAX_PASSWD_LEN - 1);

    zconfig_set_state(STATE_RCV_DONE, tods, channel);
    return PKG_END;
}

int aws_send_adha_probe_req(void)
{
    uint8_t probe[ADHA_PROBE_PKT_LEN];
    memcpy(probe, adha_probe_req_frame, sizeof(probe));
    os_wifi_get_mac(&probe[ADHA_SA_OFFSET]);
    HAL_Wifi_Send_80211_Raw_Frame(FRAME_PROBE_REQ, probe, sizeof(probe));
    return 0;
}

int awss_ieee80211_adha_process(uint8_t *mgmt_header, int len, int link_type, struct parser_res *res, signed char rssi)
{
    uint8_t ssid[ZC_MAX_SSID_LEN] = {0}, bssid[ETH_ALEN] = {0};
    uint8_t auth, pairwise_cipher, group_cipher;
    struct ieee80211_hdr *hdr;
    int fc, ret, channel;

    /*
     * when device try to connect current router (include adha and aha)
     * skip the new aha and process the new aha in the next scope.
     */
    if (mgmt_header == NULL || zconfig_finished)
        return ALINK_INVALID;

    /*
     * if user press configure button, drop all adha
     */
    if (awss_get_config_press())
        return ALINK_INVALID;

    hdr = (struct ieee80211_hdr *)mgmt_header;
    fc = hdr->frame_control;

    if (!ieee80211_is_beacon(fc) && !ieee80211_is_probe_resp(fc))
        return ALINK_INVALID;
    ret = ieee80211_get_bssid(mgmt_header, bssid);
    if (ret < 0)
        return ALINK_INVALID;

    ret = ieee80211_get_ssid(mgmt_header, len, ssid);
    if (ret < 0)
        return ALINK_INVALID;
    /*
     * skip ap which is not adha
     */
    if (strcmp((const char *)ssid, zc_adha_ssid))
        return ALINK_INVALID;

    channel = cfg80211_get_bss_channel(mgmt_header, len);
    rssi = rssi > 0 ? rssi - 256 : rssi;

    cfg80211_get_cipher_info(mgmt_header, len, &auth,
                             &pairwise_cipher, &group_cipher);
#ifdef AWSS_SUPPORT_APLIST
    awss_save_apinfo(ssid, bssid, channel, auth,
                     pairwise_cipher, group_cipher, rssi);
#endif
    /*
     * If user press the configure button,
     * skip all the adha.
     */
    if (adha_aplist->cnt > adha_aplist->try_idx) {
        uint8_t ap_idx = adha_aplist->aplist[adha_aplist->try_idx ++];
#ifdef AWSS_SUPPORT_APLIST
        memcpy(zc_bssid, zconfig_aplist[ap_idx].mac, ETH_ALEN);
#endif
        return ALINK_ADHA_SSID;
    }
    return ALINK_INVALID;
}

#endif

int awss_init_adha_aplist(void)
{
    if (adha_aplist)
        return 0;
    adha_aplist = (struct adha_info *)os_zalloc(sizeof(struct adha_info));
    if (adha_aplist == NULL)
        return -1;
    return 0;
}

int awss_deinit_adha_aplist(void)
{
    if (adha_aplist == NULL)
        return 0;

    HAL_Free(adha_aplist);
    adha_aplist = NULL;
    return 0;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
