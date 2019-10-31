/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "nghttp2_debug.h"

#include <stdio.h>

#ifdef DEBUGBUILD

static void nghttp2_default_debug_vfprintf_callback(const char *fmt,
                                                    va_list args) {
  vfprintf(stderr, fmt, args);
}

static nghttp2_debug_vprintf_callback static_debug_vprintf_callback =
    nghttp2_default_debug_vfprintf_callback;

void nghttp2_debug_vprintf(const char *format, ...) {
  if (static_debug_vprintf_callback) {
    va_list args;
    va_start(args, format);
    static_debug_vprintf_callback(format, args);
    va_end(args);
  }
}

void nghttp2_set_debug_vprintf_callback(
    nghttp2_debug_vprintf_callback debug_vprintf_callback) {
  static_debug_vprintf_callback = debug_vprintf_callback;
}

#else /* !DEBUGBUILD */

void nghttp2_set_debug_vprintf_callback(
    nghttp2_debug_vprintf_callback debug_vprintf_callback) {
  (void)debug_vprintf_callback;
}

#endif /* !DEBUGBUILD */
