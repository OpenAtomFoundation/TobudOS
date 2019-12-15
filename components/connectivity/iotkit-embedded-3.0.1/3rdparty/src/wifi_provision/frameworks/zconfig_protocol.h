/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __ZCONFIG_PROTOCOL_H__
#define __ZCONFIG_PROTOCOL_H__

#include <stdint.h>
#include "zconfig_utils.h"
#include "zconfig_ieee80211.h"
#include "zconfig_lib.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

enum state_machine {
    STATE_CHN_SCANNING,
    STATE_CHN_LOCKED_BY_P2P,  /* wps/action used for enrollee */
    STATE_CHN_LOCKED_BY_BR,   /* broadcast used for smartconfig */
    STATE_GOT_BEACON,
    STATE_RCV_IN_PROGRESS,
    STATE_RCV_COMPLETE,
    STATE_RCV_DONE
};

enum _GOT_RESULT_ {
    GOT_NOTHING = 0,
    GOT_CHN_LOCK = 1,
    GOT_SSID_PASSWD = 2,
};

#define PASSWD_ENCRYPT_BIT_OFFSET (1)
#define PASSWD_ENCRYPT_MASK       (0x06)
#define SSID_EXIST_BIT            (0)
#define SSID_EXIST_MASK           (1 << SSID_EXIST_BIT)
#define SSID_ENCODE_BIT           (5)
#define SSID_ENCODE_MASK          (1 << SSID_ENCODE_BIT)

enum passwd_encpyt_type {
    PASSWD_ENCRYPT_OPEN = 0,
    PASSWD_ENCRYPT_CIPHER,
    PASSWD_ENCRYPT_AESCFB,
    PASSWD_ENCRYPT_AESOFB,
};

#define flag_tods(tods)           ((tods) ? 'T' : 'F')

#define ZC_MAX_CHANNEL            (14)
#define ZC_MIN_CHANNEL            (1)
int zconfig_is_valid_channel(int channel);

#define P2P_ENCODE_TYPE_OFFSET    (0x05)
#define P2P_SSID_LEN_MASK         ((1 << P2P_ENCODE_TYPE_OFFSET) - 1)
#define P2P_ENCRYPT_BIT_MASK      ((uint8_t)(~P2P_SSID_LEN_MASK))
enum p2p_encode_type {
    P2P_ENCODE_TYPE_DICT = 0x00,
    P2P_ENCODE_TYPE_ENCRYPT,
};

/* global data */
/* max: 48(ssid gbk encode) + 64 (passwd) + 6 (1(tlen) + 1(flag) + 1(ssid_len) + 1(passwd_len) + 2(crc)) */
#define MAX_PKG_NUMS              (128)

/* zconfig protocol */
#define START_FRAME               (0x4E0) /* 0x4E0 is group 0 */
#define GROUP_FRAME               (0x3E0) /* exclusive, 0x401 is group 1, 0x400 is not used */
#define GROUP_FRAME_END           (GROUP_FRAME + MAX_PKG_NUMS / GROUP_NUMBER) /* exclusive */
#define GROUP_NUMBER              (8)
#define ZC_GRP_PKT_IDX_START      (2)
#define ZC_GRP_PKT_IDX_END        (ZC_GRP_PKT_IDX_START + GROUP_NUMBER - 1)

struct package {
    uint16_t len;
    char score;
};

struct zconfig_data {
    struct {
        uint8_t state_machine;  /* state for tods/fromds */
        uint8_t frame_offset;   /* frame fixed offset */
        uint8_t group_pos;      /* latest group pkg pos */
        uint8_t cur_pos;        /* data abs. position */
        uint8_t max_pos;        /* data max len */
        uint8_t last_index;
        uint8_t replace;        /* whether pkg has been replaced recently */
        uint8_t score_uplimit;
#define score_max                 (100)
#define score_high                (98)
#define score_mid                 (50)
#define score_low                 (1)
#define score_min                 (0)

        uint8_t pos_unsync;
        uint16_t group_sn;      /* latest group pkg sn */
        uint16_t prev_sn;       /* last sn */
        uint16_t last_len;      /* len pkg len */
        uint32_t timestamp;     /* last timestamp */
#define time_interval             (300)    /* ms */
    } data[2];

    /* package store */
    struct package pkg[2][MAX_PKG_NUMS];
    struct package tmp_pkg[2][GROUP_NUMBER + 1];
    uint8_t src_mac[ETH_ALEN];
    uint8_t channel;            /* from 1 -- 13 */

    /* result, final result */
    uint8_t ssid[ZC_MAX_SSID_LEN];
    uint8_t passwd[ZC_MAX_PASSWD_LEN];
    uint8_t bssid[ETH_ALEN];
    uint8_t ssid_is_gbk;
    uint8_t ssid_auto_complete_disable;

    /* used by v2 android p2p protocol, for gbk ssid correctness */
    uint8_t android_pre_ssid[ZC_MAX_SSID_LEN];
    uint8_t android_ssid[ZC_MAX_SSID_LEN];
    uint8_t android_bssid[ETH_ALEN];
    uint8_t android_src[ETH_ALEN];
    void *mutex;
};

#define zc_state                       zconfig_data->data[tods].state_machine
#define zc_frame_offset                zconfig_data->data[tods].frame_offset
#define zc_group_pos                   zconfig_data->data[tods].group_pos
#define zc_group_sn                    zconfig_data->data[tods].group_sn
#define zc_prev_sn                     zconfig_data->data[tods].prev_sn
#define zc_cur_pos                     zconfig_data->data[tods].cur_pos
#define zc_max_pos                     zconfig_data->data[tods].max_pos
#define zc_last_index                  zconfig_data->data[tods].last_index
#define zc_last_len                    zconfig_data->data[tods].last_len
#define zc_replace                     zconfig_data->data[tods].replace
#define zc_score_uplimit               zconfig_data->data[tods].score_uplimit
#define zc_timestamp                   zconfig_data->data[tods].timestamp
#define zc_pos_unsync                  zconfig_data->data[tods].pos_unsync

#define zc_src_mac                     &zconfig_data->src_mac[0]

#define zc_channel                     zconfig_data->channel

#define zc_ssid                        (&zconfig_data->ssid[0])
#define zc_passwd                      (&zconfig_data->passwd[0])
#define zc_bssid                       (&zconfig_data->bssid[0])
#define zc_ssid_is_gbk                 (zconfig_data->ssid_is_gbk)
#define zc_ssid_auto_complete_disable  (zconfig_data->ssid_auto_complete_disable)

#define pkg_score(n)                   zconfig_data->pkg[tods][n].score
#define pkg_len(n)                     zconfig_data->pkg[tods][n].len
#define pkg(n)                         &zconfig_data->pkg[tods][n]

#define tmp_score(n)                   zconfig_data->tmp_pkg[tods][n].score
#define tmp_len(n)                     zconfig_data->tmp_pkg[tods][n].len
#define tmp(n)                         &zconfig_data->tmp_pkg[tods][n]

#define zc_pre_ssid                    (&zconfig_data->android_pre_ssid[0])
#define zc_android_ssid                (&zconfig_data->android_ssid[0])
#define zc_android_bssid               (&zconfig_data->android_bssid[0])
#define zc_android_src                 (&zconfig_data->android_src[0])
#define zc_mutex                       zconfig_data->mutex

void zconfig_force_destroy(void);
void encode_chinese(uint8_t *in, uint8_t in_len, uint8_t *out, uint8_t *out_len, uint8_t bits);
void decode_chinese(uint8_t *in, uint8_t in_len, uint8_t *out, uint8_t *out_len, uint8_t bits);
void zconfig_set_state(uint8_t state, uint8_t tods, uint8_t channel);
int is_ascii_string(uint8_t *str);

/*
 * [IN] ssid or bssid
 * [OUT] auth, encry, channel
 */
uint8_t zconfig_get_auth_info(uint8_t *ssid, uint8_t *bssid, uint8_t *auth, uint8_t *encry, uint8_t *channel);
uint8_t zconfig_callback_over(uint8_t *ssid, uint8_t *passwd, uint8_t *bssid);

#define MAC_FORMAT                "%02x%02x%02x%02x%02x%02x"
#define MAC_VALUE(mac)            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

extern const char *zc_default_ssid;
extern const char *zc_default_passwd;
extern struct zconfig_data *zconfig_data;
extern uint8_t zconfig_finished;
/* broadcast mac address */
extern uint8_t br_mac[ETH_ALEN];
/* all zero mac address */
extern uint8_t zero_mac[ETH_ALEN];

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* __IEEE80211_123_H */
