/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __ZCONFIG_IEEE80211_H__
#define __ZCONFIG_IEEE80211_H__

#include "zconfig_utils.h"
#include "zconfig_protocol.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define WIFI_RX_SENSITIVITY   (-85)
#ifndef ETH_ALEN
#define ETH_ALEN    6
#endif

/*
 * DS bit usage
 *
 * TA = transmitter address
 * RA = receiver address
 * DA = destination address
 * SA = source address
 *
 * ToDS    FromDS  A1(RA)  A2(TA)  A3      A4      Use
 * -----------------------------------------------------------------
 *  0       0       DA      SA      BSSID   -       IBSS/DLS
 *  0       1       DA      BSSID   SA      -       AP -> STA
 *  1       0       BSSID   SA      DA      -       AP <- STA
 *  1       1       RA      TA      DA      SA      unspecified (WDS)
 */
#define FCS_LEN                         (4)

#define IEEE80211_FCTL_VERS             (0x0003)
#define IEEE80211_FCTL_FTYPE            (0x000c)
#define IEEE80211_FCTL_STYPE            (0x00f0)
#define IEEE80211_FCTL_TODS             (0x0100)
#define IEEE80211_FCTL_FROMDS           (0x0200)
#define IEEE80211_FCTL_MOREFRAGS        (0x0400)
#define IEEE80211_FCTL_RETRY            (0x0800)
#define IEEE80211_FCTL_PM               (0x1000)
#define IEEE80211_FCTL_MOREDATA         (0x2000)
#define IEEE80211_FCTL_PROTECTED        (0x4000)
#define IEEE80211_FCTL_ORDER            (0x8000)
#define IEEE80211_FCTL_CTL_EXT          (0x0f00)

#define IEEE80211_SCTL_FRAG             (0x000F)
#define IEEE80211_SCTL_SEQ              (0xFFF0)

#define IEEE80211_FTYPE_MGMT            (0x0000)
#define IEEE80211_FTYPE_CTL             (0x0004)
#define IEEE80211_FTYPE_DATA            (0x0008)
#define IEEE80211_FTYPE_EXT             (0x000c)

#define IEEE80211_STYPE_DATA            (0x0000)
#define IEEE80211_STYPE_QOS_DATA        (0x0080)
#define IEEE80211_STYPE_PROBE_REQ       (0x0040)
#define IEEE80211_STYPE_PROBE_RESP      (0x0050)
#define IEEE80211_STYPE_BEACON          (0x0080)
#define IEEE80211_STYPE_ACTION          (0x00D0)

#define IEEE80211_QOS_CTL_LEN           (2)
#define IEEE80211_HT_CTL_LEN            (4)

/* beacon capab_info */
#define WLAN_CAPABILITY_PRIVACY         (1 << 4)

#define IEEE80211_SEQ_TO_SN(seq)        (((seq) & IEEE80211_SCTL_SEQ) >> 4)
#define IEEE80211_SN_TO_SEQ(ssn)        (((ssn) << 4) & IEEE80211_SCTL_SEQ)

#define WLAN_CATEGORY_VENDOR_SPECIFIC   (127)

#define WLAN_EID_SSID                   (0)
#define WLAN_EID_DS_PARAMS              (3)
#define WLAN_EID_RSN                    (48)
#define WLAN_EID_HT_OPERATION           (61)
#define WLAN_EID_VENDOR_SPECIFIC        (221)

#define WLAN_OUI_ALIBABA                (0xD896E0)
#define WLAN_OUI_TYPE_ALIBABA           (1)
#define WLAN_OUI_TYPE_ENROLLEE          (0xAA)
#define WLAN_OUI_TYPE_REGISTRAR         (0xAB)

enum ALINK_TYPE {
    ALINK_INVALID = 0,
    ALINK_BROADCAST = 1,
    ALINK_ROUTER = 2,
    ALINK_ACTION = 3,
    ALINK_WPS = 4,
    ALINK_DEFAULT_SSID = 5,
    ALINK_ZERO_CONFIG = 6,
    ALINK_ADHA_SSID = 7,
    ALINK_APLIST,
    ALINK_HT_CTRL,
};

/* 80211 frame parser result */
struct parser_res {
    union _alink_type_ {
        /* for broadcast data frame */
        struct broadcast_info {
            uint8_t encry_type;/* none/wep/tkip/aes */
            uint16_t data_len;/* framelen - 80211 hdr - fcs(4) */
            uint16_t sn;
        } br;
        /* for alink ie frame */
        struct ie_info {
            uint8_t *alink_ie;
            uint16_t alink_ie_len;
        } ie;
        /* for p2p action frame */
        struct action_info {
            uint8_t *data;
            uint16_t data_len;
        } action;
        /* for p2p wps frame */
        struct wps_info {
            uint8_t *data;
            uint16_t data_len;
        } wps;
        /* for ht40 ctrl frame */
        struct ht_ctrl_info {
            signed char rssi;
            uint8_t filter;
            uint16_t data_len;
        } ht_ctrl;
    } u;

    uint8_t *src; /* src mac of sender */
    uint8_t *dst; /* ff:ff:ff:ff:ff:ff */
    uint8_t *bssid; /* mac of AP */

    uint8_t tods; /* fromDs or toDs */
    uint8_t channel; /* 1 - 13 */
};

struct ieee80211_hdr {
    uint16_t frame_control;
    uint16_t duration_id;
    uint8_t addr1[ETH_ALEN];
    uint8_t addr2[ETH_ALEN];
    uint8_t addr3[ETH_ALEN];
    uint16_t seq_ctrl;
    uint8_t addr4[ETH_ALEN];
};

/*
 * The radio capture header precedes the 802.11 header.
 *
 * Note well: all radiotap fields are little-endian.
 */
struct ieee80211_radiotap_header {
    uint8_t  it_version;     /* Version 0. Only increases
                     * for drastic changes,
                     * introduction of compatible
                     * new fields does not count.
                     */
    uint8_t  it_pad;
    uint16_t it_len;         /* length of the whole
                    * header in bytes, including
                    * it_version, it_pad,
                    * it_len, and data fields.
                    */
    uint32_t it_present;     /* A bitmap telling which
                    * fields are present. Set bit 31
                    * (0x80000000) to extend the
                    * bitmap by another 32 bits.
                    * Additional extensions are made
                    * by setting bit 31.
                    */
};

/**
 * struct ieee80211_ht_operation - HT operation IE
 *
 * This structure is the "HT operation element" as
 * described in 802.11n-2009 7.3.2.57
 */
struct ieee80211_ht_operation {
    uint8_t primary_chan;
    uint8_t ht_param;
    uint16_t operation_mode;
    uint16_t stbc_param;
    uint8_t basic_set[16];
};

struct ieee80211_vendor_ie {
    uint8_t element_id;
    uint8_t len;
    uint8_t oui[3];
    uint8_t oui_type;
};
/*
 * i.e.    alibaba ie
 *    @name        @len    @payload
 *    element_id    1    221
 *    len           1    22
 *    oui           3    0xD896E0
 *    oui_type      1    1 -- alink router service advertisement
 *    version       1    1
 *    challenge     16    non-zero-ascii code
 *    reserve       1    0
 */

struct ieee80211_mgmt {
    uint16_t frame_control;
    uint16_t duration;
    uint8_t da[ETH_ALEN];
    uint8_t sa[ETH_ALEN];
    uint8_t bssid[ETH_ALEN];
    uint16_t seq_ctrl;
    union {
        struct {
            /* __le64 timestamp; */
            uint16_t timestamp[4];
            uint16_t beacon_int;
            uint16_t capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params, TIM */
            uint8_t variable;
        } beacon;
        struct {
            /* only variable items: SSID, Supported rates */
            uint8_t variable;
        } probe_req;
        struct {
            /* __le64 timestamp; */
            uint16_t timestamp[4];
            uint16_t beacon_int;
            uint16_t capab_info;
            /* followed by some of SSID, Supported rates,
             * FH Params, DS Params, CF Params, IBSS Params */
           uint8_t variable;
        } probe_resp;
    } u;
};

typedef int (*awss_protocol_process_func_type)(uint8_t *, int, int, struct parser_res *, signed char);
typedef int (*awss_protocol_finish_func_type)(struct parser_res *);

struct awss_protocol_couple_type {
    int type;
    awss_protocol_process_func_type awss_protocol_process_func;
    awss_protocol_finish_func_type awss_protocol_finish_func;
};

int ieee80211_data_extract(uint8_t *in, int len, int link_type,
                           struct parser_res *res, signed char rssi);

struct ap_info *zconfig_get_apinfo(uint8_t *mac);
struct ap_info *zconfig_get_apinfo_by_ssid(uint8_t *ssid);
struct ap_info *zconfig_get_apinfo_by_ssid_prefix(uint8_t *ssid_prefix);
struct ap_info *zconfig_get_apinfo_by_ssid_suffix(uint8_t *ssid_suffix);

/* add channel to scanning channel list */
int zconfig_add_active_channel(int channel);
uint8_t zconfig_get_press_status();

int ieee80211_hdrlen(uint16_t fc);
int ieee80211_has_a4(uint16_t fc);
int ieee80211_is_ctl(uint16_t fc);
int ieee80211_is_mgmt(uint16_t fc);
int ieee80211_is_data(uint16_t fc);
int ieee80211_has_tods(uint16_t fc);
int ieee80211_has_frags(uint16_t fc);
int ieee80211_has_order(uint16_t fc);
int ieee80211_is_beacon(uint16_t fc);
int ieee80211_is_action(uint16_t fc);
int ieee80211_has_fromds(uint16_t fc);
int ieee80211_is_data_qos(uint16_t fc);
int ieee80211_is_probe_req(uint16_t fc);
int ieee80211_is_probe_resp(uint16_t fc);
int ieee80211_is_data_exact(uint16_t fc);
int ieee80211_has_protected(uint16_t fc);
int ieee80211_is_data_present(uint16_t fc);
int ieee80211_get_radiotap_len(uint8_t *data);
int ieee80211_get_bssid(uint8_t *in, uint8_t *mac);
int ieee80211_get_ssid(uint8_t *beacon_frame, uint16_t frame_len, uint8_t *ssid);
int ieee80211_data_extract(uint8_t *in, int len, int link_type, struct parser_res *res, signed char rssi);
int cfg80211_get_bss_channel(uint8_t *beacon_frame, uint16_t frame_len);
int cfg80211_get_cipher_info(uint8_t *beacon_frame, uint16_t frame_len,
                             uint8_t *auth_type, uint8_t *pairwise_cipher_type, uint8_t *group_cipher_type);
uint8_t *ieee80211_get_SA(struct ieee80211_hdr *hdr);
uint8_t *ieee80211_get_DA(struct ieee80211_hdr *hdr);
uint8_t *ieee80211_get_BSSID(struct ieee80211_hdr *hdr);
const uint8_t *cfg80211_find_ie(uint8_t eid, const uint8_t *ies, int len);
const uint8_t *cfg80211_find_vendor_ie(uint32_t oui, uint8_t oui_type, const uint8_t *ies, int len);
struct ap_info *zconfig_get_apinfo(uint8_t *mac);
struct ap_info *zconfig_get_apinfo_by_ssid(uint8_t *ssid);
struct ap_info *zconfig_get_apinfo_by_ssid_prefix(uint8_t *ssid_prefix);
struct ap_info *zconfig_get_apinfo_by_ssid_suffix(uint8_t *ssid_suffix);


#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* __IEEE80211_H */
