/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_SMARTCONFIG_H__
#define __AWSS_SMARTCONFIG_H__

#include <stdint.h>
#include "os.h"
#include "zconfig_lib.h"
#include "zconfig_ieee80211.h"
#include "zconfig_protocol.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/* invalid channel, neither 0, 0xff, nor 1-13 */
#define INVALID_CHANNEL     (0)
#define PAYLOAD_BITS_CNT    (7)
#define PAYLOAD_BITS_MASK   ((1 << PAYLOAD_BITS_CNT) - 1)

extern const uint8_t zconfig_fixed_offset[ZC_ENC_TYPE_MAX + 1][2];
extern const uint16_t zconfig_hint_frame[];

uint8_t is_data_frame(uint16_t len);
uint8_t is_start_frame(uint16_t len);
uint8_t is_group_frame(uint16_t len);
uint8_t get_data_index(uint16_t len);
uint8_t get_group_index(uint16_t len);

int zconfig_recv_completed(uint8_t tods);
int zconfig_get_ssid_passwd(uint8_t tods);
int package_cmp(uint8_t *package, uint8_t *src, uint8_t *dst, uint8_t tods, uint16_t len);
int package_save(uint8_t *package, uint8_t *src, uint8_t *dst, uint8_t tods, uint16_t len);
int awss_recv_callback_smartconfig(struct parser_res *res);
int awss_ieee80211_smartconfig_process(uint8_t *ieee80211, int len, int link_type,
                                       struct parser_res *res, signed char rssi);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
