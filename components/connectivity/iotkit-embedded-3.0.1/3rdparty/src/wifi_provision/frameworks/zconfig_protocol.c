/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#if defined(AWSS_SUPPORT_SMARTCONFIG) || defined(AWSS_SUPPORT_AHA) || defined(AWSS_SUPPORT_SMARTCONFIG_WPS) || defined(AWSS_SUPPORT_SMARTCONFIG_MCAST) || defined(AWSS_SUPPORT_ZEROCONFIG)

/* broadcast mac address */
uint8_t br_mac[ETH_ALEN];
/* all zero mac address */
uint8_t zero_mac[ETH_ALEN];

/* which channel lock at */
uint8_t zconfig_channel_locked = INVALID_CHANNEL;/* locked channel */
/*
 * avoid zconfig_callback_over() was called twice,
 * once from tods, once from fromds
 */
uint8_t zconfig_finished;

/* global data structure, which hold all broadcast data */
struct zconfig_data *zconfig_data;

/*
 * 8bit -> x bit
 *
 * serialize chinese char from 8bit to x bit
 */
void encode_chinese(uint8_t *in, uint8_t in_len,
                    uint8_t *out, uint8_t *out_len, uint8_t bits)
{
    if (bits == 0 || bits > 7) {
        return;
    }

    do {
        uint8_t i, j;
        uint8_t bit[ZC_MAX_SSID_LEN * 8] = {0};
        uint8_t output_len = ((in_len * 8) + bits - 1) / bits;

        /* char to bit stream */
        for (i = 0; i < in_len; i ++) {
            for (j = 0; j < 8; j ++) {
                bit[i * 8 + j] = (in[i] >> j) & 0x01;
            }
        }

        out[output_len] = '\0'; /* NULL-terminated */
        for (i = 0; i < output_len; i ++) {
            for (j = 0, out[i] = 0; j < bits; j ++) {
                out[i] |= bit[i * bits + j] << j;
            }
        }

        if (out_len) {
            *out_len = output_len;
        }
    } while (0);
}

/* x bit -> 8bit */
void decode_chinese(uint8_t *in, uint8_t in_len,
                    uint8_t *out, uint8_t *out_len, uint8_t bits)
{
    if (bits == 0 || bits > 7 || in_len == 0) {
        return;
    }

    do {
        uint8_t i, j;
        uint8_t output_len = (in_len * bits) / 8;
        uint8_t *bit = (uint8_t *)os_zalloc(in_len * bits);

        if (bit == NULL) {
            awss_crit("decode malloc failed!\r\n");
            return;
        }

        /* char to bit stream */
        for (i = 0; i < in_len; i ++) {
            for (j = 0; j < bits; j ++) {
                bit[i * bits + j] = (in[i] >> j) & 0x01;
            }
        }

        out[output_len] = '\0'; /* NULL-terminated */
        for (i = 0; i < output_len; i++) {
            for (j = 0, out[i] = 0; j < 8; j ++) {
                out[i] |= bit[i * 8 + j] << j;
            }
        }

        HAL_Free(bit);
        if (out_len) {
            *out_len = output_len;
        }
    } while (0);
}

/*
 * 1/locked, 0/not locked
 */
uint8_t is_channel_locked(void)
{
    return zconfig_channel_locked != INVALID_CHANNEL;
}

/*
 * Note: this notification will be kept called, in case of
 *     user clear the channel locked state to re-scanning
 *    channel because of waiting timeout.
 */
uint8_t zconfig_callback_channel_locked(uint8_t channel)
{
    if (channel != zconfig_channel_locked) {
        awss_info("channel lock @ %d\r\n", channel);
        zconfig_channel_locked = channel;
    }

    /*
     * if recv timeout, vendor may re-scanning channel,
     * so keep calling channel locked notification here.
     */
    zconfig_channel_locked_callback(channel, 0, zc_bssid);

    return 0;
}

uint8_t zconfig_callback_over(uint8_t *ssid, uint8_t *passwd, uint8_t *bssid)
{
    uint8_t auth = ZC_AUTH_TYPE_INVALID, encry = ZC_ENC_TYPE_INVALID, channel = 0;

    awss_info("zconfig done. ssid:%s, mac:%02x%02x%02x%02x%02x%02x\r\n",
               ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

    if (zconfig_finished) {
        return 0;
    }

#ifdef AWSS_SUPPORT_APLIST
    awss_get_auth_info(ssid, bssid, &auth, &encry, &channel);
#endif

    zconfig_got_ssid_passwd_callback(ssid, passwd, bssid, auth, encry, channel);

    zconfig_finished = 1;

    HAL_Awss_Close_Monitor();

    return 0;
}

void zconfig_set_state(uint8_t state, uint8_t tods, uint8_t channel)
{
    /* state change callback */
    switch (state) {
        case STATE_CHN_SCANNING:
            break;
        case STATE_CHN_LOCKED_BY_P2P:
            /* locked state used by action/wps frame */
            zconfig_callback_channel_locked(channel);
            break;
        case STATE_CHN_LOCKED_BY_BR:
            /* locked state used by br frame */
            zconfig_callback_channel_locked(zc_channel ? zc_channel : channel);
            break;
        case STATE_RCV_DONE:
            /* prevent main_thread_func to free zconfig_data until curent task is finished. */
            HAL_MutexLock(zc_mutex);
            /*
             * in case of p2p/router, direct into RCV_DONE state,
             * skiped the chn lock state, so better to call channel lock here
             */
            if (!is_channel_locked()) {
                zconfig_callback_channel_locked(channel);
            }
            zconfig_callback_over(zc_ssid, zc_passwd, zc_bssid);
            break;
        default:
            break;
    }

    /*
     * state machine loop:
     * scanning -> p2p lock -> rcv_done
     * scanning -> (p2p) rcv_done
     * scanning -> br lock -> (br) rcv_done
     * scanning -> br lock -> (p2p) recv_done
     * scanning -> p2p lock -> br lock -> (br) recv_done
     *
     * watch out zc_state rolling back.
     * zconfig_set_state(CHN_LOCKED) will be called more than once,
     */
    if (zc_state < state) {
        zc_state = state;
    }
    if (state == STATE_RCV_DONE) {
        HAL_MutexUnlock(zc_mutex);
    }
}

/*
    pkt_data & pkt_length:
        radio_hdr + 80211 hdr + payload, without fcs(4B)
    return:
        PKG_INVALID -- invalid pkt,
        PKG_START_FRAME -- start frame,
        PKG_DATA_FRAME -- data frame,
        PKG_ALINK_ROUTER -- alink router,
        PKG_GROUP_FRAME -- group frame,
        PKG_BC_FRAME -- broadcast frame
*/
int is_invalid_pkg(void *pkt_data, uint32_t pkt_length)
{
#define MIN_PKG         (33)
#define MAX_PKG         (1480 + 56 + 200)
    if (pkt_length < MIN_PKG || pkt_length > MAX_PKG) {
        return 1;
    }
    return 0;
}

/*
 * zconfig_recv_callback()
 *
 * ieee80211 package parser
 *
 * @Return:
 *     zconfig state
 */
int zconfig_recv_callback(void *pkt_data, uint32_t pkt_length, uint8_t channel,
                          int link_type, int with_fcs, signed char rssi)
{
    int pkt_type = PKG_INVALID;
    struct parser_res res;
    memset(&res, 0, sizeof(res));

    /* remove FCS filed */
    if (with_fcs) {
        pkt_length -= 4;
    }

    /* useless, will be removed */
    if (is_invalid_pkg(pkt_data, pkt_length)) {
        return PKG_INVALID;
    }

    res.channel = channel;

    pkt_type = ieee80211_data_extract(pkt_data, pkt_length, link_type, &res, rssi);

    return pkt_type;
}

/* init mem & timer */
void zconfig_init()
{
    awss_info("%s\r\n", __func__);

    zconfig_channel_locked = INVALID_CHANNEL;
    zconfig_finished = 0;

    memset(br_mac, 0xff, ETH_ALEN);
    memset(zero_mac, 0x00, ETH_ALEN);

    zconfig_data = (struct zconfig_data *)os_zalloc(sizeof(struct zconfig_data));
    if (zconfig_data == NULL) {
        goto ZCONFIG_INIT_FAIL;
    }
    zc_mutex = HAL_MutexCreate();
    if (zc_mutex == NULL) {
        goto ZCONFIG_INIT_FAIL;
    }

#ifdef AWSS_SUPPORT_APLIST
    if (awss_init_ieee80211_aplist()) {
        goto ZCONFIG_INIT_FAIL;
    }
#endif
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
    if (awss_init_adha_aplist()) {
        goto ZCONFIG_INIT_FAIL;
    }
#endif

#ifdef AWSS_SUPPORT_HT40
    ht40_init();
#endif
    return;

ZCONFIG_INIT_FAIL:
    awss_crit("malloc failed!\r\n");
    zconfig_destroy();

#ifdef AWSS_SUPPORT_APLIST
    awss_deinit_ieee80211_aplist();
#endif
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
    awss_deinit_adha_aplist();
#endif
    return;
}

void zconfig_destroy(void)
{
    if (zconfig_data) {
        if (zc_mutex) {
            HAL_MutexDestroy(zc_mutex);
        }
        HAL_Free((void *)zconfig_data);
        zconfig_data = NULL;
    }
}

void zconfig_force_destroy(void)
{
    zconfig_destroy();

#ifdef AWSS_SUPPORT_APLIST
    awss_deinit_ieee80211_aplist();
    awss_close_aplist_monitor();
#endif

#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
    awss_deinit_adha_aplist();
#endif
}

int zconfig_is_valid_channel(int channel)
{
    return (ZC_MIN_CHANNEL <= channel && channel <= ZC_MAX_CHANNEL);
}
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
