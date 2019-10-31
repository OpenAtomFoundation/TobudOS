#include "wifi_provision_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/* following is broadcast protocol related code */
uint8_t is_start_frame(uint16_t len)
{
    return (len == START_FRAME);
}

uint8_t is_group_frame(uint16_t len)
{
    /* is group frame? */
    return (len > GROUP_FRAME && len <= GROUP_FRAME_END);
}

uint8_t is_data_frame(uint16_t len)
{
    uint8_t group_frame, index;
    /* is start frame */
    if (is_start_frame(len)) {
        return 0;
    }

    /* is group frame? */
    group_frame = is_group_frame(len);
    if (group_frame) {
        return 0;
    }

    index = (len >> PAYLOAD_BITS_CNT) & 0xF;
    return (index >= ZC_GRP_PKT_IDX_START && index <= ZC_GRP_PKT_IDX_END);
}

uint8_t get_group_index(uint16_t len)
{
    if (is_start_frame(len)) {
        return 0;
    }

    return (len - GROUP_FRAME) * GROUP_NUMBER;
}

uint8_t get_data_index(uint16_t len)
{
    uint8_t index = (len >> PAYLOAD_BITS_CNT) & 0xF;  /* from 2 to 9 */
    return index - (ZC_GRP_PKT_IDX_START - 1);   /* adjust, from 1 to 8 */
}

#define sn_minus(a,b)    (((a) - (b)) & 0xfff)

/* a, b must be serial seq number */
static int sn_compare(uint16_t a, uint16_t b)
{
    /*
        case1: sn = 3, sn_prev = 5;            a < b
        case2: sn = 0, sn_prev = 0xfff;        a > b
        case3: sn = 4, sn_prev = 3;            a > b
    */
    uint16_t res = sn_minus(a, b);

    return res < 1000 ? res : -1;
}

/*
 * zconfig_get_data_len()
 *     here we guess the total_len of protocl message,
 *     base on the score of tods and fromds side.
 */
int zconfig_get_data_len(void)
{
    uint8_t len;    /* total len, include len(1B) & crc(2B) */
    uint8_t score;

    /* tods > fromds */
    if (zconfig_data->pkg[1][1].score > zconfig_data->pkg[0][1].score) {
        len = zconfig_data->pkg[1][1].len & PAYLOAD_BITS_MASK;
        score = zconfig_data->pkg[1][1].score;
    } else {
        len = zconfig_data->pkg[0][1].len & PAYLOAD_BITS_MASK;
        score = zconfig_data->pkg[0][1].score;
    }

    if (len && score > score_mid) {
        /* awss_debug("zconfig_get_data_len = %d\r\n", len); */
        goto out;
    }

    if (zconfig_data->data[1].max_pos > zconfig_data->data[0].max_pos) {
        len = zconfig_data->data[1].max_pos;
    } else {
        len = zconfig_data->data[0].max_pos;
    }
out:
    if (len < GROUP_NUMBER) {
        return GROUP_NUMBER;
    } else {
        return len >= MAX_PKG_NUMS ? (MAX_PKG_NUMS - GROUP_NUMBER - 1) : len;
    }
}

/* check recv completed or not */
int zconfig_recv_completed(uint8_t tods)
{
    int i;
    uint8_t len, flag, ssid_len, passwd_len;
    /*
        byte:    0    1        2    3        4        5    6
        name:        total_len    flag    ssid_len    passwd_len    ssid    ...
        index:        0x100        0x180    0x200        0x280        0x300    0x380
    */
    len = pkg_len(1) & PAYLOAD_BITS_MASK;/* total len, include len(1B) & crc(2B) */
    flag = pkg_len(2) & PAYLOAD_BITS_MASK;
    if (flag & SSID_EXIST_MASK) {/* ssid exist */
        ssid_len = pkg_len(3) & PAYLOAD_BITS_MASK;
        passwd_len = pkg_len(4) & PAYLOAD_BITS_MASK;
    } else {
        ssid_len = 0;
        passwd_len = pkg_len(3) & PAYLOAD_BITS_MASK;
    }

    if (!len || pkg_score(1) <= score_min) {
        /* awss_trace("len=%d, pkg_score(1)=%d\r\n", len, pkg_score(1));*/
        return 0;
    }

#ifndef DISABLE_SSID_AUTO_COMPLETE
#define SSID_AUTO_COMPLETE_SCORE    (score_max + 1)
    /* ssid atuo-completion */
    if (zc_ssid[0] != '\0' && (flag & SSID_EXIST_MASK)
        && pkg_score(2) < SSID_AUTO_COMPLETE_SCORE
        && pkg_score(3) > score_mid
        && !zc_ssid_auto_complete_disable) {

        /* over-written ssid_len here */
        ssid_len = strlen((char const *)zc_ssid);
        if (ssid_len > ZC_MAX_SSID_LEN - 1) {
            ssid_len = ZC_MAX_SSID_LEN - 1;
        }

        if (!(flag & SSID_ENCODE_MASK)) {/* ASCLL ssid */
            if ((ssid_len | 0x200) != pkg_len(3)) {
                awss_warn("ssid len not match! ssid:%s != %d\r\n",
                          zc_ssid, (pkg_len(3) & ~0x200));
                zc_ssid_auto_complete_disable = 1;
                goto skip_ssid_auto_complete;
            }

            awss_trace("ssid auto-complete: %s\r\n", zc_ssid);
            pkg_score(2) = SSID_AUTO_COMPLETE_SCORE;

            pkg_len(3) = ssid_len | 0x200;    /* 0x200 is the index 3 */
            pkg_score(3) = SSID_AUTO_COMPLETE_SCORE;

            for (i = 5; i < ssid_len + 5; i ++) {
                pkg_len(i) = (zc_ssid[i - 5] - 32) | (0x100 + 0x80 * ((i - 1) % GROUP_NUMBER));
                pkg_score(i) =  SSID_AUTO_COMPLETE_SCORE;
            }
        } else if ((flag & SSID_ENCODE_MASK)) { /* include chinese ssid */
            uint8_t *buf, buf_len = 0;

            uint8_t ssid_encode_len = (ssid_len * 8 + 5) / 6;

            /*
             * for GBK encoded chinese, ssid auto-completion lead to crc error.
             * because Android APP may send utf8 encoded ssid.
             */
            if ((ssid_encode_len | 0x200) != pkg_len(3)) {
                zc_ssid_is_gbk = 1;
                zc_ssid_auto_complete_disable = 1;
                goto skip_ssid_auto_complete;
            } else {
                zc_ssid_is_gbk = 0;
            }

            buf = os_zalloc(ssid_encode_len + 1);
            if (buf == NULL) {
                awss_crit("malloc failed!\r\n");
                return 0;
            }

            awss_trace("chinese ssid auto-complete: %s\r\n", zc_ssid);
            encode_chinese(zc_ssid, ssid_len, buf, &buf_len, 6);

            pkg_score(2) = SSID_AUTO_COMPLETE_SCORE;

            pkg_len(3) = buf_len | 0x200;    /* 0x200 is the index 3 */
            pkg_score(3) =  SSID_AUTO_COMPLETE_SCORE;

            for (i = 5; i < buf_len + 5; i ++) {
                pkg_len(i) = buf[i - 5] | (0x100 + 0x80 * ((i - 1) % GROUP_NUMBER));
                pkg_score(i) =  SSID_AUTO_COMPLETE_SCORE;
            }
            HAL_Free(buf);
        }
    }
#endif

skip_ssid_auto_complete:
    /* awss_debug("expect len = %d, max len = %d\r\n", len, zc_max_pos); */
    if (zc_max_pos < len) {
        return 0;    /* receive all the packets */
    }

    for (i = 1; i <= len; i ++) {  /* check score for all the packets */
        if (pkg_score(i) <= score_min) {
            return 0;
        }
    }

    /* 4 for total_len, flag, ssid_len, passwd_len, 2 for crc */
    if (flag & SSID_EXIST_MASK) { /* ssid exist */
        if (len != ssid_len + passwd_len + 4 + 2) {
            return 0;
        }
    } else if (len != passwd_len + 3 + 2) {
        return 0;
    }

    return 1;
}

int zconfig_get_ssid_passwd(uint8_t tods)
{
    int i, ssid_len, package_num, passwd_len, ret;
    uint8_t *buf, *pbuf, *tmp, flag, passwd_encrypt, passwd_cipher_len = 0;
    uint16_t crc, cal_crc;
    uint8_t data, score;
    uint8_t tods_tmp;

    if (!zconfig_recv_completed(tods)) {
        return -1;
    }

    buf = os_zalloc(256);
    if (NULL == buf) {
        awss_crit("malloc failed!\r\n");
        return -1;
    }
    tmp = os_zalloc(128);
    if (NULL == tmp) {
        HAL_Free(buf);
        awss_crit("malloc failed!\r\n");
        return -1;
    }

    /* package num */
    package_num = pkg_len(1) & PAYLOAD_BITS_MASK;/* total len, include len(1B) & crc(2B) */

    awss_trace("\r\n");
    for (i = 1; i <= package_num; i ++) {
        data = pkg_len(i);
        score = pkg_score(i);
        buf[i - 1] = data & PAYLOAD_BITS_MASK;
        tmp[i - 1] = score;
    }

    dump_hex(&tmp[0], package_num, GROUP_NUMBER);
    awss_trace("\r\n");
    dump_hex(&buf[0], package_num, GROUP_NUMBER);
    awss_trace("\r\n");

    crc = os_get_unaligned_be16(&buf[package_num - 2]);

    pbuf = &buf[0]; /* len @ [0] */

    flag = pbuf[1];    /* flag @ [1] */
    pbuf += 2;        /* 2B for total_len, flag */

    passwd_encrypt = (flag & PASSWD_ENCRYPT_MASK) >> PASSWD_ENCRYPT_BIT_OFFSET;

    if (passwd_encrypt == PASSWD_ENCRYPT_CIPHER || passwd_encrypt == PASSWD_ENCRYPT_OPEN) {
        awss_trace("!aes128-cfb is not support: flag 0x%x\r\n", flag);
        ret = -1;
        goto exit;
    } else {
        cal_crc = zconfig_checksum_v3(buf, package_num - 2);
    }

    if (crc != cal_crc) {
        awss_trace("crc error: recv 0x%x != 0x%x\r\n", crc, cal_crc);
        /* memset(zconfig_data, 0, sizeof(*zconfig_data)); */
        tods_tmp = tods;
        for (tods = 0; tods < 2; tods ++) {
            for (i = 1; i <= package_num; i ++) {
                score = pkg_score(i);
                if (score > 0x60) {
                    pkg_score(i) = 0x60;
                } else {
                    pkg_score(i) = score >> 1;
                }
            }
        }
        tods = tods_tmp;
        ret = -1;
        awss_event_post(IOTX_AWSS_CS_ERR);
        AWSS_UPDATE_STATIS(AWSS_STATIS_SM_IDX, AWSS_STATIS_TYPE_CRC_ERR);
        goto exit;
    }

    if (flag & SSID_EXIST_MASK) {/* ssid exist */
        ssid_len = pbuf[0];
        passwd_len = pbuf[1];
        pbuf += 2; /* 2B for ssid_len, passwd_len */

        if (!(flag & SSID_ENCODE_MASK))    {  /* ascii */
            /* CAN'T use snprintf here, because of SPACE char */
            memcpy((char *)tmp, pbuf, ssid_len);
            tmp[ssid_len] = '\0';
            for (i = 0; i < ssid_len; i ++) {
                tmp[i] += 32;
            }
        } else {/* chinese format */
            decode_chinese(pbuf, ssid_len, tmp, NULL, 6);
            /* make sure 'tmp' is null-terminated */
        }
        pbuf += ssid_len; /* ssid */

        if (zc_ssid[0] == '\0' || zc_ssid_auto_complete_disable) {
            strncpy((char *)zc_ssid, (const char *)tmp, ZC_MAX_SSID_LEN - 1);
            awss_trace("SSID0: [%s]\r\n", zc_ssid);
        } else {
            if (!strncmp((const char *)tmp, (char *)zc_ssid, ZC_MAX_SSID_LEN - 1)) {
                awss_trace("SSID1: [%s]\r\n", zc_ssid);
            } else {
                awss_trace("gbk%s SSID:[%s]\r\n", zc_ssid_is_gbk ? "" : "?", zc_ssid);
            }
        }
#ifdef AWSS_SUPPORT_APLIST
        do {  /* amend SSID automatically */
            struct ap_info *ap = NULL;
            ap = zconfig_get_apinfo(zc_bssid);
            if (ap == NULL || ap->ssid[0] == '\0') {
                break;
            }
#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)
            if (strncmp(ap->ssid, zc_adha_ssid, ZC_MAX_SSID_LEN) == 0 ||
                strncmp(ap->ssid, zc_default_ssid, ZC_MAX_SSID_LEN) == 0) {
                memset(zc_bssid, 0, ETH_ALEN);
                break;
            }
#endif
            strncpy((char *)zc_ssid, (const char *)ap->ssid, ZC_MAX_SSID_LEN - 1);
        } while (0);
#endif
    } else {
        passwd_len = pbuf[0];
        pbuf += 1; /* 1B for passwd_len */
    }

    /* CAN'T use snprintf here, because of SPACE char */
    if (passwd_encrypt > PASSWD_ENCRYPT_CIPHER) {
        /* decypt passwd using aes128-cfb */
        decode_chinese(pbuf, passwd_len, tmp, &passwd_cipher_len, 6);
        passwd_len = passwd_cipher_len;
        memset(zc_passwd, 0, ZC_MAX_PASSWD_LEN);
        aes_decrypt_string((char *)tmp, (char *)zc_passwd, passwd_len,
                           1, awss_get_encrypt_type(), 0, NULL);
        if (is_utf8((const char *)zc_passwd, passwd_len) == 0) {
            void *tmp_mutex = zc_mutex;
            awss_trace("passwd err\r\n");
            memset(zconfig_data, 0, sizeof(*zconfig_data));
            zc_mutex = tmp_mutex;
            awss_event_post(IOTX_AWSS_PASSWD_ERR);
            AWSS_UPDATE_STATIS(AWSS_STATIS_SM_IDX, AWSS_STATIS_TYPE_PASSWD_ERR);
            ret = -1;
            goto exit;
        }
    } else {
        void *tmp_mutex;
        memcpy((void *)tmp, (const void *)pbuf, passwd_len);
        tmp[passwd_len] = '\0';
        for (i = 0; i < passwd_len; i ++) {
            tmp[i] += 32;
        }
        strncpy((char *)zc_passwd, (const char *)tmp, ZC_MAX_PASSWD_LEN - 1);

        awss_trace("encrypt:%d not support\r\n", passwd_encrypt);
        tmp_mutex = zc_mutex;
        memset(zconfig_data, 0, sizeof(*zconfig_data));
        zc_mutex = tmp_mutex;
        ret = -1;
        goto exit;
    }


    /* awss_debug("PASSWD: [%s]\r\n", zc_passwd); */
    pbuf += passwd_len; /* passwd */
    ret = 0;
exit:
    HAL_Free(buf);
    HAL_Free(tmp);

    return ret;
}

int package_cmp(uint8_t *package, uint8_t *src, uint8_t *dst, uint8_t tods, uint16_t len)
{
    struct package *pkg = (struct package *)package;

    if (pkg->len != len) {
        return 1;
    }
    return 0;
}

int package_save(uint8_t *package, uint8_t *src, uint8_t *dst, uint8_t tods, uint16_t len)
{
    struct package *pkg = (struct package *)package;

    pkg->len = len;
    return 0;
}

/* len -= (rth->it_len + hdrlen); see ieee80211.c */
const uint8_t zconfig_fixed_offset[ZC_ENC_TYPE_MAX + 1][2] = {
    {  /* open, none, ip(20) + udp(8) + 8(LLC) */
        36, 36
    },
    {  /* wep, + iv(4) + data + ICV(4) */
        44, 44  /* feixun, wep64(10byte), wep128(26byte) */
    },
    {  /* tkip, + iv/keyID(4) + Ext IV(4) + data + MIC(8) + ICV(4) */
        56, 56  /* tkip(10byte, 20byte), wpa2+tkip(20byte) */
    },
    {  /* aes, + ccmp header(8) + data + MIC(8) + ICV(4) */
        52, 52
    },
    {  /* tkip-aes */
        56, 52  /* fromDs==tkip,toDs==aes */
    }
};

const uint16_t zconfig_hint_frame[] = {/* GROUP_FRAME is not used, gourp 0 - 7 */
    START_FRAME, GROUP_FRAME + 1, GROUP_FRAME + 2, GROUP_FRAME + 3, GROUP_FRAME + 4,
    GROUP_FRAME + 5, GROUP_FRAME + 6, GROUP_FRAME + 7,
    0    /* NULL terminated */
};

/*
 * is_hint_frame()
 *
 * start frame or group frame can be used as a hint frame
 *
 * @Return:
 *     1/is start or group frame, otherwise return 0.
 */
int is_hint_frame(uint8_t encry, int len, uint8_t *bssid, uint8_t *src,
                  uint8_t channel, uint8_t tods, uint16_t sn)
{
    int i;

    if (encry > ZC_ENC_TYPE_MAX) {
        return 0;
    }

    len -= zconfig_fixed_offset[encry][0];    /* dont't care about tkip-aes */

    for (i = 0; zconfig_hint_frame[i]; i++) {
        if (zconfig_hint_frame[i] == len) {
            goto found_match;
        }
    }

    return 0;

found_match:
    /* tods/fromds already locked? */
    if (!memcmp(zc_bssid, zero_mac, ETH_ALEN)) {
        /* zero mac means not locked */
        memcpy(zc_bssid, bssid, ETH_ALEN);
        memcpy(zc_src_mac, src, ETH_ALEN);
    } else {
        /*
         * 1) src not equal, bssid equal, interference
         * 2) src not equal, bssid not equal, interference
         * 3) src equal, bssid equal, good, go on
         * 4) src equal, bssid not equal
         *     if tods is true, replace old ssid in case of WDS
         *     if fromds is true, APP change the AP?? or WDS??
         *         in this situation, zc_bssid is set by tods,
         *         in WDS case, zc_bssid should be unchanged
         */

        if (memcmp(zc_src_mac, src, ETH_ALEN)) {/* case 1,2 */
            /* someone must be working in aws at the same time */
            awss_warn("%c interference src:"MAC_FORMAT", bssid:"MAC_FORMAT"\r\n",
                      flag_tods(tods), MAC_VALUE(src), MAC_VALUE(bssid));
            return 0;
        } else {
            if (memcmp(zc_bssid, bssid, ETH_ALEN)) {/* case 4 */
                if (tods) {
                    memcpy(zc_bssid, bssid, ETH_ALEN);
                    memcpy(zc_src_mac, src, ETH_ALEN);
                    /* TODO: clear previous buffer, channel lock state? */
                    if (zconfig_data->data[0].state_machine == STATE_CHN_LOCKED_BY_BR) {
                        zconfig_data->data[0].state_machine = STATE_CHN_SCANNING;
                    }
                    awss_warn("%c WDS! bssid:"MAC_FORMAT" -> bssid:"MAC_FORMAT"\r\n",
                              flag_tods(tods), MAC_VALUE(zc_bssid),
                              MAC_VALUE(bssid));
                } else {
                    awss_trace("%c WDS? src:"MAC_FORMAT" -> bssid:"MAC_FORMAT"\r\n",
                               flag_tods(tods), MAC_VALUE(src),
                               MAC_VALUE(bssid));
                    return 0;
                }
            } /* else case  */
        }
    }

    zc_frame_offset = zconfig_fixed_offset[encry][0];/* delta, len(80211) - len(8023) */
    zc_group_pos = i * GROUP_NUMBER;
    zc_cur_pos = zc_group_pos;
    zc_group_sn = sn;
    zc_prev_sn = sn;
    zc_score_uplimit = score_max;

    memset(zc_ssid, 0, ZC_MAX_SSID_LEN);
#ifdef AWSS_SUPPORT_APLIST
    /* fix channel with apinfo if exist, otherwise return anyway. */
    do {
        struct ap_info *ap_info = zconfig_get_apinfo(bssid);
        extern void aws_set_dst_chan(int channel);
        if (ap_info && ap_info->encry[tods] > ZC_ENC_TYPE_MAX) {
            awss_warn("invalid apinfo ssid:%s\r\n", ap_info->ssid);
        }

        if (ap_info && ap_info->encry[tods] == encry && ap_info->channel) {
            if (channel != ap_info->channel) {
                awss_info("fix channel from %d to %d\r\n", channel, ap_info->channel);
                zc_channel = ap_info->channel;  /* fix by ap_info channel */
                aws_set_dst_chan(zc_channel);
            }
        } else {
            /* warning: channel may eq 0! */
        };

        if (ap_info) { /* save ssid */
            strncpy((char *)zc_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
        }
    } while (0);
#endif

    return 1;
}

/*
 * get_data_score()
 *
 * calc package score
 *
 * @Return:
 *     score between [0, 100]
 */
uint8_t get_data_score(uint16_t group_sn, uint16_t sn_now, uint16_t sn_last,
                       uint8_t index_now, uint8_t index_last, uint8_t tods)
{
    /*
    example: 1
     8+3 250 0  d0e cc:fa:00:c8:cf:d2 > ff:ff:ff:ff:ff:ff
     8+4 2bf 0  d15 cc:fa:00:c8:cf:d2 > ff:ff:ff:ff:ff:ff    //两个包index_delta=1, sn_delta=7

    example: 2
     8+0, 3e1,  9a5
     8+1, 13f,  9a7
             group_sn=9a7, sn=9ab-9a7, pos=e-9, len=3ce        //here, index_delta=5, sn_delta=4
             group_sn=9a7, sn=9ac-9ab, pos=f-9, len=454
             group_sn=9a7, sn=9ad-9ac, pos=10-9, len=4d2
    example: 3
     8+3, 225,  a32
     8+6, 3c7,  a39        //此处应该是16+6, index_delta=3, sn_delta=7
    example: 4
     0+0, 4e0,  da5
     0+7, 441,  dab        //此处应该是8+7， index_delta=7, sn_delta=6
     0+0, 4e0,  d89
     0+8, 4c2,  d8f        //此处应该是8+8， index_delta=8, sn_delta=6

    //example: 4
     0+0 [100] 294 0 4e0
     0+1 [60] 2a2 0 11a
     0+2 [40] 2aa 0 181
             group_sn:2aa, sn:2b8-2aa=14, pos:3-2, len:20a
             group_sn:2aa, sn:2bc-2b8=18, pos:4-2, len:28a
             group_sn:2aa, sn:2c0-2bc=22, pos:5-2, len:310
             group_sn:2aa, sn:2c4-2c0=26, pos:6-2, len:391
             group_sn:2aa, sn:2c8-2c4=30, pos:7-2, len:412
             group_sn:2aa, sn:2cc-2c8=34, pos:8-2, len:493
    */
    static const uint16_t score_level[][2] = {
        {0,      0},
        {1,      2}, /* include, example 1, 3 */
        {4,      8},
        {8,      16},/* example 1 */
        {15,     30},
        {40,     40},
        {0xFFFF, score_max}    /* the end missing seq, example 2 */
    };

    uint16_t sn_delta = sn_minus(sn_now, group_sn) - 1;
    uint16_t index_delta = (index_now - index_last) - 1;
    uint16_t delta = sn_delta + index_delta;

    uint8_t i = 0;
    uint8_t res;

    /* suppose: sn > zc_prev_sn, pos > zc_cur_pos */
    if (sn_compare(sn_now, group_sn) <= 0 || sn_compare(sn_now, zc_prev_sn) <= 0) {
        return score_min;
    } else if (index_now <= index_last) {
        return score_min;
    }

    while (delta > score_level[i][0]) { /* include */
        i ++;
    }

    res = score_level[i][1];

    if (zc_score_uplimit > res) {
        return zc_score_uplimit - res;
    } else {
        return score_low;
    }
}

/*
    遍历所有分组，找到最多匹配的分组: 分组号，匹配的起止位置，匹配的最小score
    遍历分两步：遍历相等的数量 和 遍历空位置的数量
    guess_pos: 根据前后位置确定的pos
    match_pos: 根据匹配计算的pos

    1) 如果guess_pos && match_pos存在，且相等，则score += 5, pos = match_pos
    2)                                    不相等，则score -= 5, pos = match_pos
    3) 只有guess_pos存在，则score = 2
    4) 只有match_pos存在，则score不变
*/
int try_to_sync_pos(uint8_t tods, uint16_t last_sn, uint8_t sn,
                    int last_group_pos, int group_pos)
{
    int ret = -1, empty_match = 0, reason = 0;
    int guess_pos = -1, final_pos = -1;

    int max_match = 0, match_group = -1, match_end = GROUP_NUMBER, match_score = 0;
    int match, i, j, score;    /* loop variable */

retry:
    for (i = 0; i <= zconfig_get_data_len(); i += GROUP_NUMBER) {
        for (match = 0, score = score_max, j = 1; j <= GROUP_NUMBER; j ++) {
            if (!tmp_score(j)) {
                continue;
            }

            if (empty_match) {
                if (pkg_score(i + j) <= 1) {
                    match++;
                    score = 1;
                }
            } else {
                if (!pkg_len(i + j)) {
                    continue;
                }
                if (pkg_len(i + j) == tmp_len(j)) {
                    match ++;
                    score = (score > pkg_score(i + j)) ? pkg_score(i + j) : score;
                } else {/* encounter first unmatch */
                    awss_trace("[%d]=%x, [%d]=%x\r\n", i + j, pkg_len(i + j), j, tmp_len(j));
                    break;
                }
            }
        }
        if (match > max_match) {
            max_match = match;
            match_group = i;
            match_end = j - 1;
            match_score = score;
            awss_trace("match=%d, match_group=%d, match_end=%d\r\n",
                       match, match_group, match_end);
        }
    }

    if (!max_match && !empty_match) {/* retry empty place match */
        empty_match = 1;
        goto retry;
    }

    if (group_pos != -1) {/* 根据后位置确定 */
        guess_pos = group_pos - GROUP_NUMBER;/* 前一组 */
        if (guess_pos < 0) {
            guess_pos = (zconfig_get_data_len() / GROUP_NUMBER) * GROUP_NUMBER;
        }

        if (!max_match || empty_match) {/* case 3 */
            match_score = 2;
            final_pos = guess_pos;
            reason = 3;
            goto replace;
            /* can not del goto, cause guess_pos has higher priority than empty match */
        }
    }
    /* 前位置 有效性难以判断，忽略 */

    if (max_match > 0) {
        if (max_match == 1) {
            match_score = match_score > 10 ? 10 : match_score;
        } else if (max_match == 2) {
            match_score = match_score > 20 ? 20 : match_score;
        } else if (max_match <= GROUP_NUMBER) {
            match_score = match_score > 30 ? 30 : match_score;
        } else {
            goto clear;
        }

        if (guess_pos != -1) {
            if (guess_pos == match_group) {/* case 1 */
                match_end = GROUP_NUMBER;
                match_score += 2;/*bonus */
                final_pos = match_group;
                reason = 1;
            } else {/*case 2*/
                match_score -= 0;/*bonus*/
                if (max_match >= 2 && !empty_match) {
                    final_pos = match_group;
                } else {
                    final_pos = guess_pos;
                }
                reason = 2;
            }
        } else {/*case 4: 只有match_pos存在*/
            final_pos = match_group;

            reason = 4;
        }
    } else {
        goto clear;
    }

replace:
    if (final_pos != -1) {
        reason = reason;
        awss_trace("\tX = %d, score=%d, match=%d, reason=%d\r\n", final_pos, match_score, max_match, reason);
        if (match_end != GROUP_NUMBER) {
            awss_trace("\t match from [1-%d]\r\n", match_end);
        }
        for (i = final_pos + 1, j = 1; i <= final_pos + match_end; i++, j++) {
            if (j > GROUP_NUMBER || i >= MAX_PKG_NUMS) {
                break;
            }
            if (pkg_score(i) < match_score && tmp_score(j)) {
                pkg_len(i) = tmp_len(j);
                pkg_score(i) = (match_score > tmp_score(j) - 1) ?
                               (match_score - (tmp_score(j) - 1)) : match_score;/*TODO*/
                awss_trace("\t%d+%d [%d] %c %-3x\r\n", final_pos, j, pkg_score(i), flag_tods(tods), tmp_len(j));

                zc_replace = 1;
                if (zc_max_pos < i) {
                    zc_max_pos = i;
                }

                ret = 0;
            }
        }
    }

clear:
    zc_pos_unsync = 0;
    memset((uint8_t *)tmp(0), 0, sizeof(zconfig_data->tmp_pkg[0]));
    return ret;
}

/*
    判断同一个位置是否应发生替换
    调用该函数的前提: 同一个位置pos, 相同的得分score, 不同的数据
    替换条件: 在各分组相同位置下，旧的数据有重复，新的数据无重复
*/
int try_to_replace_same_pos(int tods, int pos, int new_len)
{
    int replace = 0, i, old_match = 0, new_match = 0;

    for (i = pos % GROUP_NUMBER; i <= zconfig_get_data_len();
         i += GROUP_NUMBER) {
        if (i != pos && pkg_len(i) == pkg_len(pos)) {
            old_match = 1;
        }

        if (pkg_len(i) == new_len) {
            new_match = 1;
        }
    }

    if ((old_match && !new_match) || tods == 0) {
        replace = 1;
        pkg_len(pos) = new_len;
    }

    return replace;
}

int awss_ieee80211_smartconfig_process(uint8_t *ieee80211, int len, int link_type, struct parser_res *res,
                                       signed char rssi)
{
    int hdrlen, fc, seq_ctrl;
    struct ieee80211_hdr *hdr;
    uint8_t *data, *bssid_mac, *dst_mac;
    uint8_t encry = ZC_ENC_TYPE_INVALID;
    uint8_t tods;

    /*
     * when device try to connect current router (include adha and aha)
     * skip the new packet.
     */
    if (ieee80211 == NULL || zconfig_finished) {
        return ALINK_INVALID;
    }

    /*
     * we don't process smartconfig until user press configure button
     */
    if (awss_get_config_press() == 0) {
        return ALINK_INVALID;
    }

    hdr = (struct ieee80211_hdr *)ieee80211;
    fc = hdr->frame_control;
    seq_ctrl = hdr->seq_ctrl;

    /*
     * for smartconfig with bcast of data
     */
    if (!ieee80211_is_data_exact(fc)) {
        return ALINK_INVALID;
    }

    /* tods = 1, fromds = 0 || tods = 0, fromds = 1 */
    if (ieee80211_has_tods(fc) == ieee80211_has_fromds(fc)) {
        return ALINK_INVALID;
    }
    /* drop frag, more, order*/
    if (ieee80211_has_frags(fc)) {
        return ALINK_INVALID;
    }

    dst_mac = (uint8_t *)ieee80211_get_DA(hdr);
    if (memcmp(dst_mac, br_mac, ETH_ALEN)) {
        return ALINK_INVALID;    /* only handle br frame */
    }

    bssid_mac = (uint8_t *)ieee80211_get_BSSID(hdr);

    /*
     * payload len = frame.len - (radio_header + wlan_hdr)
     */
    hdrlen = ieee80211_hdrlen(fc);
    if (hdrlen > len) {
        return ALINK_INVALID;
    }

#ifdef _PLATFORM_QCOM_
    /* Note: http://stackoverflow.com/questions/17688710/802-11-qos-data-frames */
    hdrlen = (hdrlen + 3) & 0xFC;/* align header to 32bit boundary */
#endif

    res->u.br.data_len = len - hdrlen;       /* eating the hdr */
    res->u.br.sn = IEEE80211_SEQ_TO_SN(os_le16toh(seq_ctrl));

    data = ieee80211 + hdrlen;               /* eating the hdr */
    tods = ieee80211_has_tods(fc);

    do {
#ifdef AWSS_SUPPORT_APLIST
        struct ap_info *ap_info;
        ap_info = zconfig_get_apinfo(bssid_mac);
        if (ap_info && ZC_ENC_TYPE_INVALID != ap_info->encry[tods]) {
            encry = ap_info->encry[tods];
        } else
#endif
        {
            if (!ieee80211_has_protected(fc)) {
                encry = ZC_ENC_TYPE_NONE;
            } else {
                /* Note: avoid empty null data */
                if (len < 8) {      /* IV + ICV + DATA >= 8 */
                    return ALINK_INVALID;
                }
                if (!(ieee80211[3] & 0x3F)) {
                    encry = ZC_ENC_TYPE_WEP;
                } else if (data[3] & (1 << 5)) {/* Extended IV */
                    if (data[1] == ((data[0] | 0x20) & 0x7F)) { /* tkip, WEPSeed  = (TSC1 | 0x20 ) & 0x7F */
                        encry = ZC_ENC_TYPE_TKIP;
                    }
                    if (data[2] == 0 && (!(data[3] & 0x0F))) {
                        encry = ZC_ENC_TYPE_AES;
                    }

                    /*
                     * Note: above code use if(tkip) and if(ase)
                     * instead of if(tkip) else if(aes)
                     * beacause two condition may bother match.
                     */
                }
            }
        }
    } while (0);

    if (encry == ZC_ENC_TYPE_INVALID) {
        awss_warn("invalid encry type!\r\n");
    }
    res->u.br.encry_type = encry;

    /* convert IEEE 802.11 header + possible LLC headers into Ethernet header
     * IEEE 802.11 address fields:
     * ToDS FromDS Addr1 Addr2 Addr3 Addr4
     *   0     0   DA    SA    BSSID n/a
     *   0     1   DA    BSSID SA    n/a
     *   1     0   BSSID SA    DA    n/a
     *   1     1   RA    TA    DA    SA
     */
    res->src = ieee80211_get_SA(hdr);
    res->dst = ieee80211_get_DA(hdr);
    res->bssid = ieee80211_get_BSSID(hdr);
    res->tods = ieee80211_has_tods(fc);

    return ALINK_BROADCAST;
}

int awss_recv_callback_smartconfig(struct parser_res *res)
{
    static char statis = 0;
    uint32_t timestamp = os_get_time_ms();

    uint8_t *src = res->src;
    uint8_t *dst = res->dst;
    uint8_t *bssid = res->bssid;
    uint8_t tods = res->tods;
    uint8_t channel = res->channel;

    uint16_t sn = res->u.br.sn;
    uint16_t len = res->u.br.data_len;
    uint8_t encry_type = res->u.br.encry_type;

    int ret, pkg_type = PKG_INVALID;
    uint8_t score = 0, timeout = 0, equal = 0;

    uint16_t pos = 0, index = 0;

    awss_flow("len=%d, %c, sn=%d, enc=%d, chn=%d, src=%02x%02x%02x%02x%02x%02x\r\n",
              len, flag_tods(tods), sn, encry_type, channel,
              src[0], src[1], src[2], src[3], src[4], src[5]);
    /*
     * STATE_CHN_LOCKED_BY_P2P is set by v2 wps/action frame, which means
     * APP is sending v2, but if v2 is fail, APP will rollback to v1,
     * so still need to parse br frame here
     * even zc_state == STATE_CHN_LOCKED_BY_P2P.
     */
    if (zc_state == STATE_CHN_LOCKED_BY_P2P ||
        zc_state == STATE_CHN_SCANNING) {
        if (is_hint_frame(encry_type, len, bssid, src, channel, tods, sn)) {
            if (statis == 0) {
                statis = 1;
                AWSS_UPDATE_STATIS(AWSS_STATIS_SM_IDX, AWSS_STATIS_TYPE_TIME_START);
            }
            awss_trace("hint frame: offset:%d, %c, sn:%x\r\n",
                       zc_frame_offset, flag_tods(tods), sn);

            awss_trace("src:%02x%02x%02x%02x%02x%02x, bssid:%02x%02x%02x%02x%02x%02x\r\n",
                       src[0], src[1], src[2], src[3], src[4], src[5],
                       bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

            pkg_type = PKG_START_FRAME;
            zconfig_set_state(STATE_CHN_LOCKED_BY_BR, tods, channel);

            goto update_sn;
        } else if (!memcmp(zc_android_src, src, ETH_ALEN)) {
#ifdef AWSS_SUPPORT_APLIST
            struct ap_info *ap_info = zconfig_get_apinfo(bssid);
            if (ap_info) {
                if (ap_info->ssid[0] != 0x00 && ap_info->ssid[0] != 0xFF) {
                    strncpy((char *)zc_android_ssid, (const char *)ap_info->ssid, ZC_MAX_SSID_LEN - 1);
                }
                memcpy(zc_android_bssid, bssid, ETH_ALEN);
                awss_trace("src %02x%02x%02x match %02x%02x%02x\r\n",
                           zc_android_src[0], zc_android_src[1], zc_android_src[2],
                           zc_android_bssid[0], zc_android_bssid[1], zc_android_bssid[2]);
            }
#endif
        }
    } else if (zc_state == STATE_CHN_LOCKED_BY_BR) {
        /* same src mac & br & bssid */
        if (memcmp(&src[0], zc_src_mac, ETH_ALEN) ||
            memcmp(&dst[0], br_mac, sizeof(br_mac)) ||
            memcmp(bssid, zc_bssid, ETH_ALEN)) { /* in case of WDS */
            goto drop;
        }

        if (timestamp - zc_timestamp > time_interval) {
            awss_debug("\t\t\t\t\ttimestamp = %d\r\n", timestamp - zc_timestamp);
            timeout = 1;
        }

        ret = sn_compare(sn, zc_prev_sn);
        if (ret <= 0) { /* retry packet, update timestamp */
            zc_timestamp = timestamp;
        }
        if (ret == 0) {
            awss_debug("drop: %3x == %3x\r\n", sn, zc_prev_sn);/* log level, too many retry pkg */
            goto drop;
        } else if (ret < 0 && !timeout) {/* if timeout, goto pos_unsync */
            awss_debug("drop: %3x < %3x\r\n", sn, zc_prev_sn);/* TODO: better not drop */
            goto update_sn;/* FIXME: update sn??? */
        }

        /* assert(sn > zc_prev_sn && !timeout); */

        if (len <= zc_frame_offset) { /* length invalid */
            goto drop;
        }

        len -= zc_frame_offset;

        if (is_data_frame(len)) {
            pkg_type = PKG_DATA_FRAME;
            index = get_data_index(len);
            pos = zc_group_pos + index;

            if (index > GROUP_NUMBER || pos >= MAX_PKG_NUMS) {
                goto drop;
            }
            /*
             * pos_unsync: 进入条件,任一条
             *     case1: index rollback
             *     case2: index equal but len not equal
             *     case3: data frame & timeout
             * 退出条件：
             *     case1: 进入条件同时也是退出条件
             *     case2: 收到同步帧
             */
            if (index < zc_last_index ||
                (index == zc_last_index && len != zc_last_len) || timeout) {
                if (zc_pos_unsync) {/* already in pos_unsync state */
                    awss_trace("\texit try_to_sync_pos: re-enter!\r\n");
                    try_to_sync_pos(tods, zc_prev_sn, sn, zc_group_pos, -1);
                }
                zc_pos_unsync = 1;/* also a new start */
                if (index < zc_last_index) {
                    awss_trace("\tenter try_to_sync_pos: rollback \r\n");
                } else if (timeout) {
                    awss_trace("\tenter try_to_sync_pos: timeout \r\n");
                } else {
                    awss_trace("\tenter try_to_sync_pos: != \r\n");
                }
            }
pos_unsync:
            if (zc_pos_unsync) {/* tmp save */
                package_save((uint8_t *)tmp(index), src, dst, tods, len);
                if (zc_pos_unsync == 1) {
                    tmp_score(index) = 1;
                } else {
                    tmp_score(index) = (sn - zc_prev_sn);    /* TODO: index? last_tmp_score */
                }
                zc_pos_unsync ++; /* unsync pkg counter */
                awss_trace("\tX+%d [%d] %-3x %c %-3x\r\n", index, tmp_score(index), sn, flag_tods(tods), len);
                goto update_sn;/* FIXME: update prev_sn or not? */
            }

            /* assert(sn > zc_prev_sn && pos > zc_cur_pos) */
            score = get_data_score(zc_group_sn, sn, zc_prev_sn, pos, zc_cur_pos, tods);
            if (score == score_min) {/* better not drop any pkg here */
                awss_trace("\t drop: group_sn:%x, sn:%x-%x=%x, pos:%d-%d, len:%x\r\n",
                           zc_group_sn, sn, zc_prev_sn, sn_minus(sn, zc_group_sn), pos, zc_cur_pos, len);
                goto update_sn;
            } else {
                if (zc_score_uplimit > score) {
                    zc_score_uplimit = score;    /* inherit last limit */
                }

                zc_group_sn = sn;/* TODO */
                awss_trace("%d+%d [%d] %-3x %c %-3x\r\n", zc_group_pos, index, score, sn, flag_tods(tods), len);
            }
        } else {
            if (is_start_frame(len) || is_group_frame(len)) {
                uint8_t group = get_group_index(len);

                if (zc_pos_unsync) {
                    awss_trace("\texit try_to_sync_pos: group frame\r\n");
                    try_to_sync_pos(tods, zc_prev_sn, sn, zc_group_pos, group);
                }

                zc_cur_pos = group;
                zc_group_pos = group;
                zc_group_sn = sn;
                zc_score_uplimit = score_max;

                awss_trace("%d+%d [%d] %-3x %c %-3x\r\n", group, 0, zc_score_uplimit, sn, flag_tods(tods), len);

                /* ignore PKG_GROUP_FRAME here */
                pkg_type = PKG_START_FRAME;

                /*
                 * keep calling zconfig_set_state(), see Note about
                 * zconfig_callback_channel_locked()
                 */
                zconfig_set_state(STATE_CHN_LOCKED_BY_BR, tods, channel);

                /* zc_replace may happen in try_to_sync_pos(), so goto is_recv_completed */
                goto is_recv_completed;
            } else {
                awss_trace("\t invalid len = %d\r\n", len + zc_frame_offset);
                goto drop;
            }
        }

        /* start from pkg(1), leave pkg(0) for start frame */
        if (pos >= MAX_PKG_NUMS || pos <= 0) {
            awss_warn("msg index(%d) out of range!\r\n", pos);
            goto drop;
        }

        zc_cur_pos = pos;

        /*
           score now > last:
           1) data equal:    pkg_score = now
           2) not equal:    pkg_score = now, data replace
           score now == last:
           1) data equal:    pkg_score++    and ???
           2) not equal:    pkg_score cut down & give warning & try_to_replace
           score now < last:
           1) data equal:    score_uplimit up???
           2) not equal:    goto pos_unsync
         */
        for (tods = 0; tods < 2; tods ++) {
            equal = !package_cmp((uint8_t *)pkg(pos), src, dst, tods, len);

            if (score > pkg_score(pos)) {
                pkg_score(pos) = score;    /* update score first */
                if (equal) {
                    continue;
                }
                /* not equal */
                zc_replace = 1;
                package_save((uint8_t *)pkg(pos), src, dst, tods, len);
            } else if (score == pkg_score(pos)) {/* range check ? */
                int replace;
                if (equal) {
                    pkg_score(pos) ++;
                    continue;
                }
                /* not equal */
                replace = try_to_replace_same_pos(tods, pos, len);
                if (replace) {
                    awss_trace("\t replace @ %d, len=%x\r\n", pos, len);
                    continue;
                }
                pkg_score(pos) /= 2;
                if (score >= score_mid)  /* better not happen */
                    awss_warn("xxxxxxxx warn: pos=%d, score=[%d], %x != %x\r\n",
                              pos, score, pkg_len(pos), len);

            } else if (tods == res->tods) {/* pkg_score(pos) > score */
                if (!equal) {/* data not equal */
                    if (zc_pos_unsync) {
                        continue;
                    }
                    zc_pos_unsync = 1;
                    awss_trace("\tenter try_to_sync_pos: data mismatch\r\n");
                    tods = res->tods;
                    goto pos_unsync;
                } else if (zc_score_uplimit >= score_mid && pkg_score(pos) - score < 10) { /* data equal */
                    uint8_t uplimit = (zc_score_uplimit + pkg_score(pos)) / 2;
                    if (zc_score_uplimit != uplimit) {
                        awss_trace("\t\t\t uplimit [%d] -> [%d]\r\n", zc_score_uplimit, uplimit);
                    }
                    zc_score_uplimit = uplimit;
                }
            }
        }
        tods = res->tods;

is_recv_completed:
        zc_max_pos = (zc_max_pos < zc_cur_pos) ? zc_cur_pos : zc_max_pos;
        if (zc_replace && zconfig_recv_completed(tods)) {
            zc_replace = 0;
            memcpy(zc_bssid, res->bssid, ETH_ALEN);
            if (!zconfig_get_ssid_passwd(tods)) {
                /* we got it! */
                AWSS_UPDATE_STATIS(AWSS_STATIS_SM_IDX, AWSS_STATIS_TYPE_TIME_SUC);
                statis = 0;
                zconfig_set_state(STATE_RCV_DONE, tods, channel);
                return PKG_END;
            }
        }
    }

update_sn:
    zc_prev_sn = sn;

    zc_timestamp = timestamp;
    zc_last_index = index;
    zc_last_len = len;

    return pkg_type;

drop:
    return PKG_INVALID;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
