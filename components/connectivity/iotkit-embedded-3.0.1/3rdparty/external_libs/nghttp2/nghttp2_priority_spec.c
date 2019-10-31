/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "nghttp2_priority_spec.h"

void nghttp2_priority_spec_init(nghttp2_priority_spec *pri_spec,
                                int32_t stream_id, int32_t weight,
                                int exclusive) {
  pri_spec->stream_id = stream_id;
  pri_spec->weight = weight;
  pri_spec->exclusive = exclusive != 0;
}

void nghttp2_priority_spec_default_init(nghttp2_priority_spec *pri_spec) {
  pri_spec->stream_id = 0;
  pri_spec->weight = NGHTTP2_DEFAULT_WEIGHT;
  pri_spec->exclusive = 0;
}

int nghttp2_priority_spec_check_default(const nghttp2_priority_spec *pri_spec) {
  return pri_spec->stream_id == 0 &&
         pri_spec->weight == NGHTTP2_DEFAULT_WEIGHT && pri_spec->exclusive == 0;
}

void nghttp2_priority_spec_normalize_weight(nghttp2_priority_spec *pri_spec) {
  if (pri_spec->weight < NGHTTP2_MIN_WEIGHT) {
    pri_spec->weight = NGHTTP2_MIN_WEIGHT;
  } else if (pri_spec->weight > NGHTTP2_MAX_WEIGHT) {
    pri_spec->weight = NGHTTP2_MAX_WEIGHT;
  }
}
