/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#ifdef AWSS_SUPPORT_APLIST

#define CLR_APLIST_MONITOR_TIMEOUT_MS    (24 * 60 *60 * 1000)
/* storage to store apinfo */
struct ap_info *zconfig_aplist = NULL;
/* aplist num, less than MAX_APLIST_NUM */
uint8_t zconfig_aplist_num = 0;

static uint8_t clr_aplist = 0;
static void *clr_aplist_timer = NULL;

static void awss_clr_aplist_monitor()
{
    clr_aplist = 1;
    HAL_Timer_Start(clr_aplist_timer, CLR_APLIST_MONITOR_TIMEOUT_MS);
}

int awss_is_ready_clr_aplist(void)
{
    return clr_aplist;
}

int awss_clear_aplist(void)
{
    memset(zconfig_aplist, 0, sizeof(struct ap_info) * MAX_APLIST_NUM);
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
    memset(adha_aplist, 0, sizeof(*adha_aplist));
#endif
    zconfig_aplist_num = 0;
    clr_aplist = 0;

    return 0;
}

int awss_open_aplist_monitor(void)
{
    if (clr_aplist_timer == NULL)
        clr_aplist_timer = HAL_Timer_Create("clr_aplist", (void (*)(void *))awss_clr_aplist_monitor, (void *)NULL);
    if (clr_aplist_timer == NULL)
        return -1;

    HAL_Timer_Stop(clr_aplist_timer);
    HAL_Timer_Start(clr_aplist_timer, CLR_APLIST_MONITOR_TIMEOUT_MS);
    return 0;
}

int awss_close_aplist_monitor(void)
{
    if (clr_aplist_timer == NULL)
        return 0;
    awss_stop_timer(clr_aplist_timer);
    clr_aplist_timer = NULL;
    return 0;
}

int awss_init_ieee80211_aplist(void)
{
    if (zconfig_aplist)
        return 0;
    zconfig_aplist = (struct ap_info *)os_zalloc(sizeof(struct ap_info) * MAX_APLIST_NUM);
    if (zconfig_aplist == NULL)
        return -1;
    zconfig_aplist_num = 0;
    return 0;
}

int awss_deinit_ieee80211_aplist(void)
{
    if (zconfig_aplist == NULL)
        return 0;
    HAL_Free(zconfig_aplist);
    zconfig_aplist = NULL;
    zconfig_aplist_num = 0;
    return 0;
}

struct ap_info *zconfig_get_apinfo(uint8_t *mac)
{
    int i;

    for (i = 1; i < zconfig_aplist_num; i++) {
        if (!memcmp(zconfig_aplist[i].mac, mac, ETH_ALEN))
            return &zconfig_aplist[i];
    }

    return NULL;
}

struct ap_info *zconfig_get_apinfo_by_ssid(uint8_t *ssid)
{
    int i;

    for (i = 1; i < zconfig_aplist_num; i ++) {
        if (!strcmp((char *)zconfig_aplist[i].ssid, (char *)ssid))
            return &zconfig_aplist[i];
    }

    return NULL;
}

/* 通过ssid前缀 */
struct ap_info *zconfig_get_apinfo_by_ssid_prefix(uint8_t *ssid_prefix)
{
    int i;
    int len = strlen((const char *)ssid_prefix);
    if (!len)
        return NULL;

    for (i = 1; i < zconfig_aplist_num; i++) {
        if (!strncmp((char *)zconfig_aplist[i].ssid, (char *)ssid_prefix, len)) {
            /* TODO: first match or best match??? */
            return &zconfig_aplist[i];/* first match */
        }
    }

    return NULL;
}

int str_end_with(const char *str, const char *suffix)
{
    int lenstr, lensuffix;
    if (!str || !suffix)
        return 0;
    lenstr = strlen(str);
    lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

/* 通过ssid后缀 */
struct ap_info *zconfig_get_apinfo_by_ssid_suffix(uint8_t *ssid_suffix)
{
    int i;
    int len = strlen((const char *)ssid_suffix);
    if (!len)
        return NULL;

    for (i = 1; i < zconfig_aplist_num; i++) {
        if (str_end_with((char *)zconfig_aplist[i].ssid, (char *)ssid_suffix)) {
            /* TODO: first match or best match??? */
            return &zconfig_aplist[i];/* first match */
        }
    }

    return NULL;
}

/**
 * save apinfo
 *
 * @ssid: [IN] ap ssid
 * @bssid: [IN] ap bssid
 * @channel: [IN] ap channel
 * @auth: [IN] optional, ap auth mode, like OPEN/WEP/WPA/WPA2/WPAWPA2
 * @encry: [IN], ap encryption mode, i.e. NONE/WEP/TKIP/AES/TKIP-AES
 *
 * Note:
 *     1) if ap num exceed zconfig_aplist[], always save at [0]
 *         but why...I forgot...
 *     2) always update channel if channel != 0
 *     3) if chn is locked, save ssid to zc_ssid, because zc_ssid
 *         can be used for ssid-auto-completion
 * Return:
 *     0/success, -1/invalid params(empty ssid/bssid)
 */

int awss_save_apinfo(uint8_t *ssid, uint8_t* bssid, uint8_t channel, uint8_t auth,
                     uint8_t pairwise_cipher, uint8_t group_cipher, signed char rssi)
{
    int i;

    /* ssid, bssid cannot empty, channel can be 0, auth/encry can be invalid */
    if (!(ssid && bssid))
        return -1;

    /* sanity check */
    if (channel > ZC_MAX_CHANNEL || channel < ZC_MIN_CHANNEL)
        channel = 0;
    else
        zconfig_add_active_channel(channel);

    if (auth > ZC_AUTH_TYPE_MAX)
        auth = ZC_AUTH_TYPE_INVALID;

    if (pairwise_cipher > ZC_ENC_TYPE_MAX)
        pairwise_cipher = ZC_ENC_TYPE_INVALID;
    if (group_cipher > ZC_ENC_TYPE_MAX)
        group_cipher = ZC_ENC_TYPE_INVALID;

    /* FIXME: */
    if (pairwise_cipher == ZC_ENC_TYPE_TKIPAES)
        pairwise_cipher = ZC_ENC_TYPE_AES; /* tods */

    /*
     * start from zconfig_aplist[1], leave [0] for temp use
     * if zconfig_aplist[] is full, always replace [0]
     */
    if (!zconfig_aplist_num) {
        zconfig_aplist_num = 1;
    }

    for (i = 1; i < zconfig_aplist_num; i++) {
        if(!strncmp(zconfig_aplist[i].ssid, (char *)ssid, ZC_MAX_SSID_LEN)
           && !memcmp(zconfig_aplist[i].mac, bssid, ETH_ALEN)) {
            /* FIXME: useless? */
            /* found the same bss */
            if (!zconfig_aplist[i].channel)
                zconfig_aplist[i].channel = channel;
            if (zconfig_aplist[i].auth == ZC_AUTH_TYPE_INVALID)
                zconfig_aplist[i].auth = auth;
            if (zconfig_aplist[i].encry[0] == ZC_ENC_TYPE_INVALID)
                zconfig_aplist[i].encry[0] = group_cipher;
            if (zconfig_aplist[i].encry[1] == ZC_ENC_TYPE_INVALID)
                zconfig_aplist[i].encry[1] = pairwise_cipher;

            return 0;/* duplicated ssid */
        }
    }

    if (i < MAX_APLIST_NUM) {
        zconfig_aplist_num ++;
    } else {
        i = 0;    /* [0] for temp use, always replace [0] */
    }

    strncpy((char *)&zconfig_aplist[i].ssid, (const char *)&ssid[0], ZC_MAX_SSID_LEN - 1);
    memcpy(&zconfig_aplist[i].mac, bssid, ETH_ALEN);
    zconfig_aplist[i].auth = auth;
    zconfig_aplist[i].rssi = rssi;
    zconfig_aplist[i].channel = channel;
    zconfig_aplist[i].encry[0] = group_cipher;
    zconfig_aplist[i].encry[1] = pairwise_cipher;

#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
    do {
        char save_adha = 0;
#ifdef AWSS_SUPPORT_ADHA
        if (!strcmp((void *)ssid, zc_adha_ssid))
            save_adha = 1;
#endif
#ifdef AWSS_SUPPORT_AHA
        if (!strcmp((void *)ssid, zc_default_ssid))
            save_adha = 1;
#endif
        if (save_adha) {
            if (adha_aplist->cnt < MAX_APLIST_NUM)
                adha_aplist->aplist[adha_aplist->cnt ++] = i;
        }
    } while(0);
#endif

    do {
        char adha = 0;
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
        adha = adha_aplist->cnt;
#endif
    awss_trace("[%d] ssid:%s, mac:%02x%02x%02x%02x%02x%02x, chn:%d, rssi:%d, adha:%d\r\n",
        i, ssid, bssid[0], bssid[1], bssid[2],
        bssid[3], bssid[4], bssid[5], channel,
        rssi > 0 ? rssi - 256 : rssi, adha);
    } while (0);
    /*
     * if chn already locked(zc_bssid set),
     * copy ssid to zc_ssid for ssid-auto-completiont
     */
    if (!memcmp(zc_bssid, bssid, ETH_ALEN) && ssid[0] != '\0') {
        strncpy((char *)zc_ssid, (char const *)ssid, ZC_MAX_SSID_LEN - 1);
    }

    return 0;
}

/*
 * [IN] ssid or bssid
 * [OUT] auth, encry, channel
 */
int awss_get_auth_info(uint8_t *ssid, uint8_t *bssid, uint8_t *auth,
                       uint8_t *encry, uint8_t *channel)
{
    uint8_t *valid_bssid = NULL;
    struct ap_info *ap_info = NULL;

    /* sanity check */
    if (!bssid || !memcmp(bssid, zero_mac, ETH_ALEN)) {
        valid_bssid = NULL;
    } else {
        valid_bssid = bssid;
    }

    /* use mac or ssid to search apinfo */
    if (valid_bssid) {
        ap_info = zconfig_get_apinfo(valid_bssid);
    } else {
        ap_info = zconfig_get_apinfo_by_ssid(ssid);
    }

    if (!ap_info)
        return 0;

    if (auth)
        *auth = ap_info->auth;
    if (encry)
        *encry = ap_info->encry[1];    /* tods side */
    if (!valid_bssid && bssid)
        memcpy(bssid, ap_info->mac, ETH_ALEN);
    if (channel)
        *channel = ap_info->channel;

    return 1;

}

void aws_try_adjust_chan(void)
{
    struct ap_info *ap = NULL;
    char ssid[ZC_MAX_SSID_LEN] = {0};
    ap = zconfig_get_apinfo(zc_bssid);
    if (ap == NULL)
        return;
    if (zconfig_get_lock_chn() == ap->channel)
        return;
    if (!zconfig_is_valid_channel(ap->channel))
        return;
    strncpy(ssid, (const char *)ap->ssid, ZC_MAX_SSID_LEN - 1);

#ifdef AWSS_SUPPORT_AHA
    if (strlen(ssid) == strlen(zc_default_ssid) &&
        strncmp(ap->ssid, zc_default_ssid, strlen(zc_default_ssid)) == 0)
        return;
#endif
#ifdef AWSS_SUPPORT_ADHA
    if (strlen(ssid) == strlen(zc_adha_ssid) &&
        strncmp(ap->ssid, zc_adha_ssid, strlen(zc_adha_ssid)) == 0)
        return;
#endif

    aws_set_dst_chan(ap->channel);
    aws_switch_channel();
}

int awss_ieee80211_aplist_process(uint8_t *mgmt_header, int len, int link_type, struct parser_res *res, signed char rssi)
{
    uint8_t ssid[ZC_MAX_SSID_LEN] = {0}, bssid[ETH_ALEN] = {0};
    uint8_t auth, pairwise_cipher, group_cipher;
    struct ieee80211_hdr *hdr;
    int fc, ret, channel;

    if (mgmt_header == NULL)
        return ALINK_INVALID;

    hdr = (struct ieee80211_hdr *)mgmt_header;
    fc = hdr->frame_control;

    /*
     * just for save ap in aplist for ssid amend.
     */
    if (!ieee80211_is_beacon(fc) && !ieee80211_is_probe_resp(fc))
        return ALINK_INVALID;

    ret = ieee80211_get_bssid(mgmt_header, bssid);
    if (ret < 0)
        return ALINK_INVALID;

    ret = ieee80211_get_ssid(mgmt_header, len, ssid);
    if (ret < 0)
        return ALINK_INVALID;

    /*
     * skip all the adha and aha
     */
#ifdef AWSS_SUPPORT_AHA
    if (strcmp((const char *)ssid, zc_default_ssid) == 0)
        return ALINK_INVALID;
#endif
#ifdef AWSS_SUPPORT_ADHA
    if (strcmp((const char *)ssid, zc_adha_ssid) == 0)
        return ALINK_INVALID;
#endif

    channel = cfg80211_get_bss_channel(mgmt_header, len);
    rssi = rssi > 0 ? rssi - 256 : rssi;

    cfg80211_get_cipher_info(mgmt_header, len, &auth,
                             &pairwise_cipher, &group_cipher);
    awss_save_apinfo(ssid, bssid, channel, auth,
                     pairwise_cipher, group_cipher, rssi);
    return ALINK_INVALID;
}

#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
