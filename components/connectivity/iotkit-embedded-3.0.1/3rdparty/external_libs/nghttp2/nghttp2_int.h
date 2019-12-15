/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_INT_H
#define NGHTTP2_INT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"

/* Macros, types and constants for internal use */

/* "less" function, return nonzero if |lhs| is less than |rhs|. */
typedef int (*nghttp2_less)(const void *lhs, const void *rhs);

/* Internal error code. They must be in the range [-499, -100],
   inclusive. */
typedef enum {
  NGHTTP2_ERR_CREDENTIAL_PENDING = -101,
  NGHTTP2_ERR_IGN_HEADER_BLOCK = -103,
  NGHTTP2_ERR_IGN_PAYLOAD = -104,
  /*
   * Invalid HTTP header field was received but it can be treated as
   * if it was not received because of compatibility reasons.
   */
  NGHTTP2_ERR_IGN_HTTP_HEADER = -105,
  /*
   * Invalid HTTP header field was received, and it is ignored.
   * Unlike NGHTTP2_ERR_IGN_HTTP_HEADER, this does not invoke
   * nghttp2_on_invalid_header_callback.
   */
  NGHTTP2_ERR_REMOVE_HTTP_HEADER = -106
} nghttp2_internal_error;

#endif /* NGHTTP2_INT_H */
