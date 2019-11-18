/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_DEBUG_H
#define NGHTTP2_DEBUG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"

#ifdef DEBUGBUILD
#define DEBUGF(...) nghttp2_debug_vprintf(__VA_ARGS__)
void nghttp2_debug_vprintf(const char *format, ...);
#else
#define DEBUGF(...)                                                            \
  do {                                                                         \
  } while (0)
#endif

#endif /* NGHTTP2_DEBUG_H */
