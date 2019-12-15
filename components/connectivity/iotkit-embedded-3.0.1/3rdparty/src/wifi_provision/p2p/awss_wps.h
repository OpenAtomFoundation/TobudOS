/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_WPS_H__
#define __AWSS_WPS_H__

#ifdef AWSS_SUPPORT_SMARTCONFIG_WPS

#include <stdint.h>
#include "os.h"
#include "zconfig_ieee80211.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define WLAN_OUI_MICROSOFT              (0x0050F2)
#define WLAN_OUI_WPS                    (0x0050F2)
#define WLAN_OUI_TYPE_MICROSOFT_WPA     (1)
#define WLAN_OUI_TYPE_WPS               (4)

int awss_recv_callback_wps(struct parser_res *res);
int awss_ieee80211_wps_process(uint8_t *mgmt_header, int len, int link_type,
                               struct parser_res *res, signed char rssi);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif  /* end AWSS_SUPPORT_SMARTCONFIG_WPS */
#endif
