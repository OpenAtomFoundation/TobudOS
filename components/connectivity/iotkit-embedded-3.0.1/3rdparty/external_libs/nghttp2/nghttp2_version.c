/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"

static nghttp2_info version = {NGHTTP2_VERSION_AGE, NGHTTP2_VERSION_NUM,
                               NGHTTP2_VERSION, NGHTTP2_PROTO_VERSION_ID};

nghttp2_info *nghttp2_version(int least_version) {
  if (least_version > NGHTTP2_VERSION_NUM)
    return NULL;
  return &version;
}
