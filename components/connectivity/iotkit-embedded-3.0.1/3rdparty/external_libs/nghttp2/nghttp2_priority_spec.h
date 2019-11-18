/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_PRIORITY_SPEC_H
#define NGHTTP2_PRIORITY_SPEC_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"

/*
 * This function normalizes pri_spec->weight if it is out of range.
 * If pri_spec->weight is less than NGHTTP2_MIN_WEIGHT, it is set to
 * NGHTTP2_MIN_WEIGHT.  If pri_spec->weight is larger than
 * NGHTTP2_MAX_WEIGHT, it is set to NGHTTP2_MAX_WEIGHT.
 */
void nghttp2_priority_spec_normalize_weight(nghttp2_priority_spec *pri_spec);

#endif /* NGHTTP2_PRIORITY_SPEC_H */
