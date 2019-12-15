/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOT_IMPORT_AWSS_H__
#define __IOT_IMPORT_AWSS_H__

#include "infra_types.h"
#include "infra_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _IN_ 
  #define _IN_ 
#endif
#ifndef _OU_
  #define _OU_ 
#endif
#ifndef _IN_OPT_
#define _IN_OPT_
#endif

/* link type */
enum AWSS_LINK_TYPE {
    /* rtos HAL choose this type */
    AWSS_LINK_TYPE_NONE,

    /* linux HAL may choose the following type */
    AWSS_LINK_TYPE_PRISM,
    AWSS_LINK_TYPE_80211_RADIO,
    AWSS_LINK_TYPE_80211_RADIO_AVS,
    AWSS_LINK_TYPE_HT40_CTRL /* for espressif HAL, see struct ht40_ctrl */
};

struct HAL_Ht40_Ctrl {
    uint16_t    length;
    uint8_t     filter;
    signed char rssi;
};

typedef int (*awss_recv_80211_frame_cb_t)(char *buf, int length,
        enum AWSS_LINK_TYPE link_type, int with_fcs, signed char rssi);

/* auth type */
enum AWSS_AUTH_TYPE {
    AWSS_AUTH_TYPE_OPEN,
    AWSS_AUTH_TYPE_SHARED,
    AWSS_AUTH_TYPE_WPAPSK,
    AWSS_AUTH_TYPE_WPA8021X,
    AWSS_AUTH_TYPE_WPA2PSK,
    AWSS_AUTH_TYPE_WPA28021X,
    AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWSS_AUTH_TYPE_MAX = AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWSS_AUTH_TYPE_INVALID = 0xff,
};

/* encryt type */
enum AWSS_ENC_TYPE {
    AWSS_ENC_TYPE_NONE,
    AWSS_ENC_TYPE_WEP,
    AWSS_ENC_TYPE_TKIP,
    AWSS_ENC_TYPE_AES,
    AWSS_ENC_TYPE_TKIPAES,
    AWSS_ENC_TYPE_MAX = AWSS_ENC_TYPE_TKIPAES,
    AWSS_ENC_TYPE_INVALID = 0xff,
};

typedef int (*awss_wifi_scan_result_cb_t)(
            const char ssid[HAL_MAX_SSID_LEN],
            const uint8_t bssid[ETH_ALEN],
            enum AWSS_AUTH_TYPE auth,
            enum AWSS_ENC_TYPE encry,
            uint8_t channel, signed char rssi,
            int is_last_ap);

/* 80211 frame type */
typedef enum HAL_Awss_Frame_Type {
    FRAME_ACTION,
    FRAME_BEACON,
    FRAME_PROBE_REQ,
    FRAME_PROBE_RESPONSE,
    FRAME_DATA
} HAL_Awss_Frame_Type_t;

#define FRAME_ACTION_MASK       (1 << FRAME_ACTION)
#define FRAME_BEACON_MASK       (1 << FRAME_BEACON)
#define FRAME_PROBE_REQ_MASK    (1 << FRAME_PROBE_REQ)
#define FRAME_PROBE_RESP_MASK   (1 << FRAME_PROBE_RESPONSE)
#define FRAME_DATA_MASK         (1 << FRAME_DATA)

typedef void (*awss_wifi_mgmt_frame_cb_t)(_IN_ uint8_t *buffer, _IN_ int len,
        _IN_ signed char rssi_dbm, _IN_ int buffer_type);

typedef struct {
    enum AWSS_AUTH_TYPE auth;
    enum AWSS_ENC_TYPE encry;
    uint8_t channel;
    signed char rssi_dbm;
    char ssid[HAL_MAX_SSID_LEN];
    uint8_t mac[ETH_ALEN];
} awss_ap_info_t;

#ifdef __cplusplus
}
#endif

#endif  /* __IOT_IMPORT_AWSS_H__ */

