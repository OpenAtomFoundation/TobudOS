/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_ENROLLEE_H__
#define __AWSS_ENROLLEE_H__

#include <stdint.h>
#include "infra_sha1.h"
#include "infra_sha256.h"
#include "passwd.h"
#include "os.h"
#include "zconfig_ieee80211.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/* enrollee/registrar doc see following
 * http://docs.alibaba-inc.com/pages/viewpage.action?pageId=450855381
 */
/* ie oui def. */
#define WLAN_OUI_ALIBABA            (0xD896E0)
#define WLAN_OUI_TYPE_ENROLLEE      (0xAA)
#define WLAN_OUI_TYPE_REGISTRAR     (0xAB)

#define DEVICE_TYPE_VERSION         (1)
#define ENROLLEE_FRAME_TYPE         (0)
#define REGISTRAR_FRAME_TYPE        (1)
#define REGISTRAR_IDLE_DUTY         (1)

struct ieee80211_enrollee_alibaba_ie {
    uint8_t element_id;     /* 221 */
    uint8_t len;            /* len of this struct, exclude element id & len field */
    uint8_t oui[3];         /* D896E0 */
    uint8_t oui_type;       /* 0xAA, device request */

    uint8_t version:4;      /* bit7 - bit4 */
    uint8_t dev_type:4;     /* bit3 - bit0; alink=0, alink_cloud=1, yoc=8 */
    uint8_t dn_len;         /* device name length*/
#ifdef __GNUC__
    uint8_t dev_name[0];    /* device name, unique name for device */
#endif
    uint8_t frame_type;     /* frame_type = 0 */

    uint8_t pk_len;         /* product key length */
#ifdef __GNUC__
    uint8_t pk[0];          /* product key */
#endif
    uint8_t rand_len;       /* random length */
#ifdef __GNUC__
    uint8_t random[0];      /* random salt */
#endif
    uint8_t security;       /* securation type, per product(3) or device(4) or manufacture(5) */
    uint8_t sign_method;    /* 0: hmacsha1, 1:hmacsha256 */
    uint8_t sign_len;       /* signature length */
#ifdef __GNUC__
    uint8_t sign[0];        /* sign = hmacsha1(secret, random+dev_name+product_key) */
#endif
};

/* len = 17 + sign[n] + ssid[n] + passwd[n] */
struct ieee80211_registrar_alibaba_ie {
    uint8_t element_id;     /* 221 */
    uint8_t len;            /* len of this struct, exclude element id & len field */
    uint8_t oui[3];         /* D896E0 */
    uint8_t oui_type;       /* 0xAB, device response */

    uint8_t version:4;     /* bit7 - bit4 */
    uint8_t dev_type:4;    /* bit3 - bit0; alink=0, alink_cloud=1, yoc=8 */
    uint8_t sign_len;       /* signature length */
#ifdef __GNUC__
    uint8_t sign[0];        /* sign = hmacsha1(secret, random+dev_name+product_key)*/
#endif
    uint8_t frame_type;     /* frame_type = 0 */

    uint8_t ssid_len;       /* AP's SSID length */
#ifdef __GNUC__
    uint8_t ssid[0];        /* SSID of AP */
#endif
    uint8_t passwd_len;     /* AP's PASSWORD length */
#ifdef __GNUC__
    uint8_t passwd[0];      /* PASSWORD of AP */
#endif
    uint8_t bssid[6];       /* BSSID of AP */
};

#define MAX_DEV_NAME_LEN            (64)
#define MAX_PK_LEN                  (20)
#define MAX_KEY_LEN                 (32)
#define MAX_TOKEN_LEN               (32)
#define ZC_PROBE_LEN                (46)
#define ENROLLEE_SIGN_SIZE          (SHA1_DIGEST_SIZE)
#define ENROLLEE_IE_FIX_LEN         (sizeof(struct ieee80211_enrollee_alibaba_ie) + RANDOM_MAX_LEN + ENROLLEE_SIGN_SIZE)
#define REGISTRAR_IE_FIX_LEN        (sizeof(struct ieee80211_registrar_alibaba_ie))
#define ENROLLEE_INFO_HDR_SIZE      (ENROLLEE_IE_FIX_LEN - 6 + MAX_DEV_NAME_LEN + 1 + MAX_PK_LEN + 1)

#ifndef AWSS_DISABLE_REGISTRAR
struct enrollee_info {
    uint8_t dev_type_ver;
    uint8_t dev_name_len;
    uint8_t dev_name[MAX_DEV_NAME_LEN + 1];
    uint8_t frame_type;

    uint8_t pk_len;
    uint8_t pk[MAX_PK_LEN + 1];
    uint8_t rand_len;
    uint8_t random[RANDOM_MAX_LEN];
    uint8_t security;  /* encryption per product(3) or device(4) or manufacture(5) */
    uint8_t sign_method;  /* 0:hmacsha1, 1:hmacsha256 */
    uint8_t sign_len;
    uint8_t sign[ENROLLEE_SIGN_SIZE];

    signed char rssi;

    uint8_t key[MAX_KEY_LEN + 1];  /* aes key */

    uint8_t state;             /* free or not */
    uint8_t checkin_priority;  /* smaller means high pri */
    uint32_t checkin_timestamp;
    uint32_t report_timestamp;
    uint32_t interval;         /* report timeout */
    uint32_t checkin_timeout;
};
#endif
/* registrar configuration */
#define MAX_ENROLLEE_NUM            (5)  /* Note: max enrollee num supported */

/*
 * ENR_FREE     --producer-->   ENR_IN_QUEUE
 * ENR_IN_QUEUE     --cloud----->   ENR_CHECKIN_ENABLE
 * ENR_CHECKIN_ENABLE   --consumer-->   ENR_CHECKIN_ONGOING --> ENR_CHECKIN_END/ENR_FREE
 * *any state*      --consumer-->   ENR_FREE
 */
enum enrollee_state {
    ENR_FREE = 0,
    ENR_IN_QUEUE,
    ENR_FOUND,
    ENR_CHECKIN_ENABLE,
    ENR_CHECKIN_CIPHER,
    ENR_CHECKIN_ONGOING,
    ENR_CHECKIN_END,
    /* ENR_OUTOFDATE = 0 */
};

#define AES_KEY_LEN                 (16)
/* return 0 for success, -1 dev_name not match, otherwise return -2 */
extern const uint8_t probe_req_frame[ZC_PROBE_LEN];
#define SA_POS                      (10) /* source mac pos */
#define FCS_SIZE                    (4)

/* enrollee API */
#ifdef AWSS_DISABLE_ENROLLEE
#if 0
static inline void awss_init_enrollee_info(void) { }
static inline void awss_broadcast_enrollee_info(void) { }
static inline void awss_destroy_enrollee_info(void) { }
#endif
#else
void awss_init_enrollee_info(void);
void awss_broadcast_enrollee_info(void);
void awss_destroy_enrollee_info(void);
int awss_recv_callback_zconfig(struct parser_res *res);
int awss_ieee80211_zconfig_process(uint8_t *mgmt_header, int len, int link_type,
                                   struct parser_res *res, signed char rssi);
#endif

/* registrar API */
#ifdef AWSS_DISABLE_REGISTRAR
#if 0
static inline void awss_registrar_deinit(void) { }
static inline void awss_registrar_init(void) { }
#endif
#else
void awss_registrar_init(void);
void awss_registrar_deinit(void);
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
