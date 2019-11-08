/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_ADHA_H__
#define __AWSS_ADHA_H__

#if defined(AWSS_SUPPORT_ADHA) || defined(AWSS_SUPPORT_AHA)

#include "zconfig_lib.h"
#include "zconfig_protocol.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

struct adha_info {
    uint8_t try_idx;
    uint8_t cnt;
    uint8_t aplist[MAX_APLIST_NUM];
};

int awss_init_adha_aplist(void);
int awss_deinit_adha_aplist(void);
extern struct adha_info *adha_aplist;
extern const char *zc_adha_passwd;
extern const char *zc_adha_ssid;

#ifdef AWSS_SUPPORT_ADHA
int awss_open_adha_monitor(void);
int awss_close_adha_monitor(void);
int aws_send_adha_probe_req(void);

int awss_is_ready_switch_next_adha(void);
int awss_recv_callback_adha_ssid(struct parser_res *res);
int awss_ieee80211_adha_process(uint8_t *mgmt_header, int len, int link_type,
                                struct parser_res *res, signed char rssi);
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif  /* end AWSS_SUPPORT_ADHA || AWSS_SUPPORT_AHA */
#endif
