/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "wifi_provision_internal.h"
#ifdef  AWSS_SUPPORT_HT40

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/*  Variable
 *  Qos: 2Byte
 *  auth offset: 36 44 52 56
 *  group frame: 4e0 3e1~3e7
 */
uint8_t ht40_hint_frame_cnt[64];
uint8_t ht40_filter;
signed char ht40_rssi_low, ht40_rssi_high;
#define ht40_rssi_range         (15)    /* suggested by Fuzhibo */
/* for ios: start frame interval is 20/100 ms */
#define HIT_FRAME_PER_CHANNEL   (2)

#define IEEE80211_MIN_HDRLEN    (24)

uint32_t ht40_timestamp;
uint8_t ht40_channel[ZC_MAX_CHANNEL + 1];
uint8_t ht40_channel_filter[ZC_MAX_CHANNEL + 1];
uint8_t ht40_state;

int ht40_init(void)
{
    ht40_state = 0;
    ht40_filter = 0;
    ht40_rssi_low = 0;
    ht40_rssi_high = 0;
    ht40_timestamp = 0;
    memset(ht40_channel, 0, sizeof(ht40_channel));
    memset(ht40_channel_filter, 0, sizeof(ht40_channel_filter));
    memset(ht40_hint_frame_cnt, 0, sizeof(ht40_hint_frame_cnt));
    return 0;
}

int ht40_lock_channel(uint8_t channel, uint8_t filter)
{
    if (channel < 1 || channel > 14)
        return 0;

    if (!ht40_channel[channel]) {  // replace when 0
        ht40_channel[channel] ++;
        ht40_channel_filter[channel] = filter;
    } else if (filter == ht40_channel_filter[channel]) {
        ht40_channel[channel] ++;  /* increase */
    } else {
        ht40_channel[channel] --;  /* decrease */
    }

    if (ht40_channel[channel] >= HIT_FRAME_PER_CHANNEL)
        return 1;

    return 0;
}

int ht40_scanning_hint_frame(uint8_t filter, signed char rssi, uint32_t length, uint8_t channel)
{
    uint8_t channel_locked = 0, next_loop = 0;
    int hint_pos = -1;
    int tods = 1;
    int i, j, k;

    if (ht40_state != STATE_CHN_SCANNING)
        return -1;

    /* range check, max: 0x4e0 + tkip + qos, min: 0x3e0 + open */
    if (length > START_FRAME + zconfig_fixed_offset[2][0] + 2 ||
        length <= GROUP_FRAME + zconfig_fixed_offset[0][0]) {
        return -1;
    }

    for (i = 1; i >= 0; i--) //Qos or not
        for (j = 3; j >= 0; j--) //auth type, without open
            for (k = 0; k < 8; k++) {//group frame
                if (zconfig_hint_frame[k] + zconfig_fixed_offset[j][0] + i * 2 == length) {
                    hint_pos = i * 32 + j * 8 + k;
#if 1
                    awss_trace("\r\nfilter:%x, rssi:%d, len:%d, Qos:%d, auth:%d, group:%d, %s\r\n",
                              filter, rssi, length, i, j, k,
                              next_loop ? "DUP" : "");
#endif
                    if (!next_loop) {
                        channel_locked = ht40_lock_channel(channel, filter);
                        if (channel_locked)
                            zconfig_set_state(STATE_CHN_LOCKED_BY_BR, tods, channel);
                        next_loop = 1;/* don't enter this loop again */
                    }

                    ht40_hint_frame_cnt[hint_pos]++;
                }
            }//end of for

    if (channel_locked) {
        ht40_rssi_high = rssi + ht40_rssi_range;
        if (ht40_rssi_high > -1)
            ht40_rssi_high = -1;
        ht40_rssi_low = rssi - ht40_rssi_range;
        if (ht40_rssi_low < -128)
            ht40_rssi_low = -128;

        ht40_filter = filter;

        awss_trace("filter:%x, rssi range:[%d, %d]\r\n",
                  filter, ht40_rssi_low, ht40_rssi_high);
    }

    return hint_pos;
}

int ht40_get_qos_auth_group_info(uint32_t length)
{
    int count = 0, max_count = 0, max_count_pos = 0;
    int continues = 0, max_continues = 0, max_continues_pos = 0, second_continues = 0;
    int tods = 1;
    int i, j, k;

    if (zc_state != STATE_CHN_LOCKED_BY_BR ||
        ht40_state != STATE_CHN_SCANNING) {
        return 0;
    }

    for (i = 1; i >= 0; i--) //Qos or not
        for (j = 3; j >= 0; j--) //auth type
            for (count = 0, continues = 0, k = 0; k < 8; k++) {//group frame
                int pos = i * 32 + j * 8 + k;

                if (ht40_hint_frame_cnt[pos]) {
                    count += ht40_hint_frame_cnt[pos];
                    if (count > max_count) {//NOTE: not >=, see continues
                        max_count = count;
                        max_count_pos = pos;
                    }

                    continues += 1;
                    if (continues >= max_continues) {
                        second_continues = max_continues;
                        max_continues = continues;
                        max_continues_pos = pos;
                    }
                }
            }

    awss_debug("max_cont:%d, sec_cont:%d, max_count:%d, max_cont_pos:%d, max_count_pos:%d\r\n",
               max_continues, second_continues, max_count, max_count_pos, max_continues_pos);

    if (max_continues > second_continues // not >=
        && max_count_pos == max_continues_pos) {
        uint8_t qos = max_count_pos / 32;
        uint8_t auth = (max_count_pos % 32) / 8;

        zc_frame_offset = zconfig_fixed_offset[auth][0] + qos * 2;
        length -= zc_frame_offset;
        if (is_start_frame(length) || is_group_frame(length)) {
            uint8_t group = get_group_index(length);

            zc_group_pos = group;
            zc_cur_pos = group;
            zc_score_uplimit = score_mid;
            ht40_timestamp = os_get_time_ms();

            ht40_state = STATE_RCV_IN_PROGRESS;
            awss_debug("len:%d, qos:%d, auth:%d, group:%d, offset:%d\r\n",
                       length, qos, auth, group, zc_frame_offset);
        }
    }

    return 0;
}

int awss_ieee80211_ht_ctrl_process(uint8_t *ht_ctrl, int len, int link_type, struct parser_res *res, signed char rssi)
{
    struct ht40_ctrl *ctrl = NULL;
    /*
     * when device try to connect current router (include adha and aha)
     * skip the new packet.
     */
    if (ht_ctrl == NULL || zconfig_finished)
        return ALINK_INVALID;
    /*
     * we don't process smartconfig until user press configure button
     */
    if (awss_get_config_press() == 0)
        return ALINK_INVALID;

    /*
     * just process ht ctrl
     */
    if (link_type != AWSS_LINK_TYPE_HT40_CTRL)
        return ALINK_INVALID;

    ctrl = (struct ht40_ctrl *)ht_ctrl;
    res->u.ht_ctrl.rssi = rssi;
    res->u.ht_ctrl.filter = ctrl->filter;
    res->u.ht_ctrl.data_len = len;

    return ALINK_HT_CTRL;
}

int awss_recv_callback_ht_ctrl(struct parser_res *res)
{
    uint8_t tods = 1, equal = 0, score = 0;
    uint16_t pos = 0, index = 0, len = 0;
    uint32_t now = os_get_time_ms();
    int pkg_type = PKG_INVALID;
    int hint_pos = -1;
    signed char rssi;
    uint16_t length;
    uint8_t channel;
    uint8_t filter;

    rssi = res->u.ht_ctrl.rssi;
    length = res->u.ht_ctrl.data_len;
    filter = res->u.ht_ctrl.filter;
    channel = res->channel;
    if (length > IEEE80211_MIN_HDRLEN) {
        length -= IEEE80211_MIN_HDRLEN;
        len = length;
    } else {
        goto drop;
    }

    hint_pos = ht40_scanning_hint_frame(filter, rssi, length, channel);

    if (hint_pos >= 0) {
        ht40_get_qos_auth_group_info(length);
        return PKG_GROUP_FRAME;
    }

    if (ht40_state == STATE_RCV_IN_PROGRESS) {
        if (rssi <= ht40_rssi_low && rssi >= ht40_rssi_high)
            goto drop;
        if (filter != ht40_filter)
            goto drop;
        if (len <= zc_frame_offset)  /* length invalid */
            goto drop;

        len -= zc_frame_offset;

        if (is_data_frame(len)) {
            pkg_type = PKG_DATA_FRAME;
            index = get_data_index(len);
            pos = zc_group_pos + index;

            if (now - ht40_timestamp > time_interval) {
                awss_debug("\t\t\t\t\ttimestamp = %d, pos:%d, len:%x\r\n",
                           now - ht40_timestamp, pos, len);
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
            if (pos < zc_cur_pos) {
                awss_debug("drop: index rollback. prev:%d, cur:%d\n", zc_cur_pos, pos);
                goto drop;
            }

            if (pos == zc_cur_pos && len != pkg_len(zc_cur_pos)) {
                awss_debug("drop: index equal, but len not. prev:%x, cur:%x\n",
                           pkg_len(pos), len);
                pkg_score(pos)--;
                goto drop;
            }

            if (pos > zc_cur_pos + 4) {
                awss_debug("drop: over range too much, prev:%d, cur:%d\n",
                           zc_cur_pos, pos);
                goto drop;
            }

            score = zc_score_uplimit - (pos - zc_cur_pos - 1);
            zc_score_uplimit = score;

            awss_trace("ht40 %d+%d [%d] -- T %-3x\r\n", zc_group_pos, index, score, len);
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
            equal = !package_cmp((uint8_t *)pkg(pos), NULL, NULL, tods, len);

            if (score > pkg_score(pos)) {
                pkg_score(pos) = score;    //update score first
                if (!equal) {
                    zc_replace = 1;
                    package_save((uint8_t *)pkg(pos), NULL, NULL, tods, len);
                }
            } else if (score == pkg_score(pos)) {/* range check ? */
                if (equal) {
                    pkg_score(pos)++;
                } else {
                    pkg_score(pos)--;
                }
            } else {//pkg_score(pos) > score
                /* do nothing */
            }

            zc_cur_pos = pos;
            zc_max_pos = (zc_max_pos < zc_cur_pos) ? zc_cur_pos : zc_max_pos;
            if (zc_replace && zconfig_recv_completed(tods)) {
                zc_replace = 0;
                if (!zconfig_get_ssid_passwd(tods)) {
                    /* we got it! */
                    zconfig_set_state(STATE_RCV_DONE, tods, channel);
                    return PKG_END;
                }
            }

            ht40_timestamp = now;
            return pkg_type;

        } else {
            if (is_start_frame(len) || is_group_frame(len)) {
                uint8_t group = get_group_index(len);

                if (!group || group == zc_group_pos + 8) {
                    zc_group_pos = group;
                    zc_score_uplimit = score_mid;

                    if (zc_cur_pos + 1 == group)
                        pkg_score(zc_cur_pos) += 1;

                    zc_cur_pos = group;

                    awss_trace("%d+%d [%d] -- T %-3x\r\n", group, 0, zc_score_uplimit, len);

                    //ignore PKG_GROUP_FRAME here
                    pkg_type = PKG_START_FRAME;
                    ht40_timestamp = now;
                    return pkg_type;
                }
            }
        }
    }

drop:
    return PKG_INVALID;
}
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
