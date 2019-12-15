/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#ifdef AWSS_SUPPORT_SMARTCONFIG_WPS

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

int is_ascii_string(uint8_t *str)
{
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] < 128) {
            i ++;
        } else {
            return 0;
        }
    }
    return 1;
}

/**
 * extract device name attribute from wps ie struct
 *
 * @wps_ie: [IN] wps ie struct
 * @len: [OUT] len of dev name attr if exist
 *
 * Return:
 *     %NULL if dev name attr could not be found, otherwise return a
 *     pointer to dev name attr
 */
static uint8_t *get_device_name_attr_from_wps(uint8_t *wps_ie, uint8_t *len)
{
    /*  6 = 1(Element ID) + 1(Length) + 4(WPS OUI) */
    uint8_t *attr_ptr = wps_ie + 6; /* goto first attr */
    uint8_t wps_ielen = wps_ie[1];

#define device_name_id        (0x1011)
    while (attr_ptr - wps_ie < wps_ielen) {
        /*  4 = 2(Attribute ID) + 2(Length) */
        uint16_t attr_id = os_get_unaligned_be16(attr_ptr);
        uint16_t attr_data_len = os_get_unaligned_be16(attr_ptr + 2);
        uint16_t attr_len = attr_data_len + 4;

        if (attr_id == device_name_id) {
            *len = attr_len;
            return attr_ptr;
        } else {
            attr_ptr += attr_len; /* goto next */
        }
    }
    return NULL;
}

/*
 * passwd_check_utf8()
 *
 * @Note: see andriod smartconfig with p2p
 *     if the byte in passwd is zero, jave will change 0x00 with 0xc080
 *     the function will restore 0xc080 to 0x00
 */
void passwd_check_utf8(uint8_t *passwd, int *passwd_len)
{
    int i, len;

    if (!passwd || !passwd_len) {
        return;
    }

    len = *passwd_len;
    for (i = 0; i < len; i ++) {
        if (passwd[i] < 0x80) { /* [0x01 ~ 0x7F] */
            continue;
        }
        passwd[i] = 0;  /* resetore to 0x00 */
        if (i + 2 < len) { /* move the rest to overwrite useless content. */
            memmove(passwd + i + 1, passwd + i + 2, len - i - 2);
        }
        len --;
    }
    *passwd_len = len;
}

/*
 * get_ssid_passwd_from_wps()
 *
 * @Note: see andriod zconfig protocol
 *     android sdk limit sizeof(passwd) <= 23
 *
 * @Return: _GOT_RESULT_
 *
 * use src mac to do ssid completion
 */
static int get_ssid_passwd_from_w(uint8_t *in, int total_len, uint8_t *src, uint8_t *bssid)
{
    uint8_t tmp_ssid[ZC_MAX_SSID_LEN + 1] = {0}, tmp_passwd[ZC_MAX_PASSWD_LEN + 1] = {0};
    int ssid_len, passwd_len, ssid_truncated = 0;
    uint16_t crc, cal_crc;
    char encrypt = 0;
    /* used by prepare frame */
    char *magic_p_w_d = "zl&ws";/* FIXME: Maybe it will be dangerous when opening source. */
    static uint32_t start_time = 0;

#define W_LEN        (32)  /* total_len */
#define EXTRA_LEN    (3)   /* ssid_len(1B) + checksum(2B) */
    if (!in || total_len <= 4 + EXTRA_LEN) {
        return GOT_NOTHING;
    }

    /* attr_id(2) + attr_len(2) = 4 */
    in += 4;
    total_len -= 4;

    if (total_len > W_LEN) {
        awss_warn("ssid len > 32\r\n");
    }

    /* total_len: ssid_len(1B), ssid, passwd, crc(2B) */
    ssid_len = in[0];
    if (ssid_len & P2P_ENCRYPT_BIT_MASK) {
        encrypt = (ssid_len & P2P_ENCRYPT_BIT_MASK) >> P2P_ENCODE_TYPE_OFFSET;
        ssid_len &= P2P_SSID_LEN_MASK;
    }
    if (encrypt > P2P_ENCODE_TYPE_ENCRYPT) {
        return GOT_NOTHING;
    }

    passwd_len = total_len - ssid_len - EXTRA_LEN; /* ssid_len(1B), crc(2B) */
    if (ssid_len > W_LEN - EXTRA_LEN || passwd_len < 0) {
        return GOT_NOTHING;
    }

    AWSS_UPDATE_STATIS(AWSS_STATIS_WPS_IDX, AWSS_STATIS_TYPE_TIME_START);
    /* ssid_len(1B), ssid, passwd, crc(2B) */
    crc = os_get_unaligned_be16(in + 1 + ssid_len + passwd_len);
    /* restore 0xc080 to 0x00 */
    passwd_check_utf8(in + 1 + ssid_len, &passwd_len);
    cal_crc = zconfig_checksum_v3(in, 1 + ssid_len + passwd_len);
    if (crc != cal_crc) {
        memset(zc_android_src, 0, sizeof(zconfig_data->android_src));
        memset(zc_pre_ssid, 0, sizeof(zconfig_data->android_pre_ssid));
        memset(zc_android_ssid, 0, sizeof(zconfig_data->android_ssid));
        memset(zc_android_bssid, 0, sizeof(zconfig_data->android_bssid));
        awss_debug("rx illegal p2p (0x%x != 0x%x)\r\n", crc, cal_crc);
        awss_event_post(IOTX_AWSS_CS_ERR);
        AWSS_UPDATE_STATIS(AWSS_STATIS_WPS_IDX, AWSS_STATIS_TYPE_CRC_ERR);
        /*
         * use zconfig_checksum_v3() because
         * java modified UTF-8, U+C080 equal U+00,
         * ssid len & ssid & crc is not be 0,
         * the content of passwd encrypted may be 0
         */
        return GOT_NOTHING;
    }

    if (start_time == 0) {
        start_time = os_get_time_ms();
    }

#define MAC_LOCAL_ADMINISTERED_BIT    (0x02)
    memcpy(zc_android_src, src, ETH_ALEN);
    if (zc_android_src[0] & MAC_LOCAL_ADMINISTERED_BIT) {
        zc_android_src[0] &= ~MAC_LOCAL_ADMINISTERED_BIT;
        /*awss_debug("android src: %02x%02x%02x\r\n", zc_android_src[0], src[1], src[2]); */
    } else {
        awss_warn("local administered bit not set: %02x%02x%02x\r\n",
                  src[0], src[1], src[2]);
    }

    in += 1;/* eating ssid_len(1B) */

    memset(tmp_ssid, 0, ZC_MAX_SSID_LEN);
    memset(tmp_passwd, 0, ZC_MAX_PASSWD_LEN);

    memcpy(tmp_ssid, in, ssid_len);
    in += ssid_len;
    if (passwd_len) {
        memcpy(tmp_passwd, in, passwd_len);
    }

    awss_dict_crypt(SSID_DECODE_TABLE, tmp_ssid, ssid_len);

    switch (encrypt) {
        case P2P_ENCODE_TYPE_ENCRYPT: {
            /* decypt passwd using aes128-cfb */
            uint8_t passwd_cipher_len = 0;
            uint8_t *passwd_cipher = os_zalloc(128);
            if (passwd_cipher == NULL) {
                return GOT_NOTHING;
            }

            decode_chinese(tmp_passwd, passwd_len, passwd_cipher, &passwd_cipher_len, 7);
            passwd_len = passwd_cipher_len;
            memset(tmp_passwd, 0, ZC_MAX_PASSWD_LEN);
            aes_decrypt_string((char *)passwd_cipher, (char *)tmp_passwd, passwd_len,
                               1, awss_get_encrypt_type(), 0, NULL);
            HAL_Free(passwd_cipher);
            if (is_utf8((const char *)tmp_passwd, passwd_len) == 0) {
                /* memset(zconfig_data, 0, sizeof(*zconfig_data)); */
                memset(zc_android_src, 0, sizeof(zconfig_data->android_src));
                memset(zc_pre_ssid, 0, sizeof(zconfig_data->android_pre_ssid));
                memset(zc_android_ssid, 0, sizeof(zconfig_data->android_ssid));
                memset(zc_android_bssid, 0, sizeof(zconfig_data->android_bssid));

                awss_warn("p2p decrypt passwd content err\r\n");
                awss_event_post(IOTX_AWSS_PASSWD_ERR);
                AWSS_UPDATE_STATIS(AWSS_STATIS_WPS_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
                return GOT_NOTHING;
            }
            break;
        }
        default: {
            void *tmp_mutex = zc_mutex;
            awss_warn("p2p encypt:%d not support\r\n", encrypt);
            memset(zconfig_data, 0, sizeof(*zconfig_data));
            zc_mutex = tmp_mutex;
            return GOT_NOTHING;
        }
    }

    awss_debug("ssid:%s, tlen:%d\r\n", tmp_ssid, total_len);
    if (passwd_len && !memcmp(tmp_passwd, magic_p_w_d, passwd_len)) {
        /* Note: when v2 rollback to v1, zc_preapre_ssid will useless */
        strncpy((char *)zc_pre_ssid, (char const *)tmp_ssid, ZC_MAX_SSID_LEN - 1);
        return GOT_CHN_LOCK;
    }
    /*
        // for ascii ssid, max length is 29(32 - 1 - 2).
        // for utf-8 ssid, max length is 29 - 2 or 29 - 3
        // gbk ssid also encoded as utf-8
        // SAMSUNG S4 max name length = 22
    */
    if (!is_ascii_string((uint8_t *)tmp_ssid)) { /* chinese ssid */
        ssid_truncated = 1;    /* in case of gbk chinese */
    } else if (total_len >= W_LEN - EXTRA_LEN) {
        ssid_truncated = 1;
    }

    if (ssid_truncated) {
        uint8_t *best_ssid;
        int cur_ssid_len = strlen((const char *)tmp_ssid);  /* current_ssid */
        int pre_ssid_len = strlen((const char *)zc_pre_ssid);  /* prepare_ssid */
        if (pre_ssid_len && pre_ssid_len < cur_ssid_len) {
            /* should not happen */
            awss_warn("pre:%s < cur:%s\r\n", zc_pre_ssid, tmp_ssid);
            best_ssid = tmp_ssid;  /* current ssid */
        } else if (pre_ssid_len) {
            best_ssid = zc_pre_ssid;    /* prepare ssid */
        } else {
            best_ssid = tmp_ssid;    /* default use current ssid */
        }

        /* awss_debug("ssid truncated, best ssid: %s\r\n", best_ssid); */

        do {
#ifdef AWSS_SUPPORT_APLIST
            struct ap_info *ap_info;
            ap_info = zconfig_get_apinfo_by_ssid_suffix(best_ssid);
            if (ap_info) {
                awss_debug("ssid truncated, got ssid from aplist:%s\r\n", best_ssid);
                strncpy((char *)zc_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
                memcpy(zc_bssid, ap_info->mac, ETH_ALEN);
            } else
#endif
            {
                if (memcmp(bssid, zero_mac, ETH_ALEN) && memcmp(bssid, br_mac, ETH_ALEN)) {
                    memcpy(zc_android_bssid, bssid, ETH_ALEN);
                }
#ifdef AWSS_SUPPORT_APLIST
                ap_info = zconfig_get_apinfo(zc_android_bssid);
                if (ap_info) {
                    if (ap_info->ssid[0] == '\0') {  /* hide ssid, MUST not truncate */
                        strncpy((char *)zc_android_ssid, (const char *)best_ssid, ZC_MAX_SSID_LEN - 1);
                    } else {  /* not hide ssid, amend ssid according to ap list */
                        strncpy((char *)zc_android_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
                    }
                } else
#endif
                    if (time_elapsed_ms_since(start_time) > HAL_Awss_Get_Channelscan_Interval_Ms() * (13 + 3) * 2) {
                        start_time = 0;
                        strncpy((char *)zc_android_ssid, (const char *)best_ssid, ZC_MAX_SSID_LEN - 1);
                    }

                if (zc_android_ssid[0] == '\0') {
                    return GOT_NOTHING;
                }
                strncpy((char *)zc_ssid, (const char *)zc_android_ssid, ZC_MAX_SSID_LEN - 1);
                memcpy(zc_bssid, zc_android_bssid, ETH_ALEN);
            }
        } while (0);
    } else {
        strncpy((char *)zc_ssid, (char const *)tmp_ssid, ZC_MAX_SSID_LEN - 1);
        if (memcmp(bssid, zero_mac, ETH_ALEN) && memcmp(bssid, br_mac, ETH_ALEN)) {
            memcpy(zc_bssid, bssid, ETH_ALEN);
        }
    }

    strncpy((char *)zc_passwd, (char const *)tmp_passwd, ZC_MAX_PASSWD_LEN - 1);
    start_time = 0;

    return GOT_SSID_PASSWD;
}


int awss_recv_callback_wps(struct parser_res *res)
{
    uint8_t *data = res->u.wps.data;
    uint16_t len = res->u.wps.data_len;

    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    int ret = get_ssid_passwd_from_w(data, len, res->src, res->bssid);
    if (ret == GOT_CHN_LOCK) {
        awss_debug("callback for v2:%02x%02x%02x\r\n",
                   res->src[0], res->src[1], res->src[2]);
        goto chn_locked;
    } else if (ret == GOT_SSID_PASSWD) {
        goto rcv_done;
    } else if (ret == GOT_NOTHING) {
        return PKG_INVALID;
    } else {
        return PKG_INVALID;
    }

chn_locked:
    zconfig_set_state(STATE_CHN_LOCKED_BY_P2P, tods, channel);
    return PKG_START_FRAME;
rcv_done:
    AWSS_UPDATE_STATIS(AWSS_STATIS_WPS_IDX, AWSS_STATIS_TYPE_TIME_SUC);
    zconfig_set_state(STATE_RCV_DONE, tods, channel);
    return PKG_END;
}

int awss_ieee80211_wps_process(uint8_t *mgmt_header, int len, int link_type, struct parser_res *res, signed char rssi)
{
    const uint8_t *wps_ie = NULL;
    struct ieee80211_hdr *hdr;
    uint8_t attr_len = 0;
    uint16_t ieoffset;
    int fc;

    /*
     * when device try to connect current router (include adha and aha)
     * skip the wps packet.
     */
    if (mgmt_header == NULL || zconfig_finished) {
        return ALINK_INVALID;
    }

    /*
     * we don't process wps until user press configure button
     */
    if (awss_get_config_press() == 0) {
        return ALINK_INVALID;
    }

    hdr = (struct ieee80211_hdr *)mgmt_header;
    fc = hdr->frame_control;

    if (!ieee80211_is_probe_req(fc)) {
        return ALINK_INVALID;
    }

    ieoffset = offsetof(struct ieee80211_mgmt, u.probe_req.variable);
    if (ieoffset > len) {
        return ALINK_INVALID;
    }
    /* get wps ie */
    wps_ie = (const uint8_t *)cfg80211_find_vendor_ie(WLAN_OUI_WPS, WLAN_OUI_TYPE_WPS,
             mgmt_header + ieoffset, len - ieoffset);
    if (wps_ie == NULL) {
        return ALINK_INVALID;
    }
    /* get wps name in wps ie */
    wps_ie = (const uint8_t *)get_device_name_attr_from_wps((uint8_t *)wps_ie, &attr_len);
    if (wps_ie == NULL) {
        return ALINK_INVALID;
    }
    res->u.wps.data_len = attr_len;
    res->u.wps.data = (uint8_t *)wps_ie;
    return ALINK_WPS;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
