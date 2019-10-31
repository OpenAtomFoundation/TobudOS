/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "nghttp2_npn.h"

#include <string.h>

static int select_next_protocol(unsigned char **out, unsigned char *outlen,
                                const unsigned char *in, unsigned int inlen,
                                const char *key, unsigned int keylen) {
  unsigned int i;
  for (i = 0; i + keylen <= inlen; i += (unsigned int)(in[i] + 1)) {
    if (memcmp(&in[i], key, keylen) == 0) {
      *out = (unsigned char *)&in[i + 1];
      *outlen = in[i];
      return 0;
    }
  }
  return -1;
}

#define NGHTTP2_HTTP_1_1_ALPN "\x8http/1.1"
#define NGHTTP2_HTTP_1_1_ALPN_LEN (sizeof(NGHTTP2_HTTP_1_1_ALPN) - 1)

int nghttp2_select_next_protocol(unsigned char **out, unsigned char *outlen,
                                 const unsigned char *in, unsigned int inlen) {
  if (select_next_protocol(out, outlen, in, inlen, NGHTTP2_PROTO_ALPN,
                           NGHTTP2_PROTO_ALPN_LEN) == 0) {
    return 1;
  }
  if (select_next_protocol(out, outlen, in, inlen, NGHTTP2_HTTP_1_1_ALPN,
                           NGHTTP2_HTTP_1_1_ALPN_LEN) == 0) {
    return 0;
  }
  return -1;
}
