/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdint.h>
#include "nghttp2_net.h"

#if IOT_BYTE_ORDER == LITTLE_ENDIAN
  uint32_t nghttp2_htonl(uint32_t hostlong) {
  uint32_t res;
  unsigned char *p = (unsigned char *)&res;
  *p++ = hostlong >> 24;
  *p++ = (hostlong >> 16) & 0xffu;
  *p++ = (hostlong >> 8) & 0xffu;
  *p = hostlong & 0xffu;
  return res;
}

uint16_t nghttp2_htons(uint16_t hostshort) {
  uint16_t res;
  unsigned char *p = (unsigned char *)&res;
  *p++ = hostshort >> 8;
  *p = hostshort & 0xffu;
  return res;
}

uint32_t nghttp2_ntohl(uint32_t netlong) {
  uint32_t res;
  unsigned char *p = (unsigned char *)&netlong;
  res = *p++ << 24;
  res += *p++ << 16;
  res += *p++ << 8;
  res += *p;
  return res;
}

uint16_t nghttp2_ntohs(uint16_t netshort) {
  uint16_t res;
  unsigned char *p = (unsigned char *)&netshort;
  res = *p++ << 8;
  res += *p;
  return res;
}
#endif
