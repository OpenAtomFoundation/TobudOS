/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __ZCONFIG_UTILS_H__
#define __ZCONFIG_UTILS_H__

#include "os.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

void dump_mac(uint8_t *src, uint8_t *dst);
void dump_hex(uint8_t *data, int len, int tab_num);
void dump_ascii(uint8_t *data, int len, int tab_num);

uint16_t zconfig_checksum_v3(uint8_t *data, uint8_t len);
char is_utf8(const char *ansi_str, int length);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
