/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_RCBUF_H
#define NGHTTP2_RCBUF_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"

struct nghttp2_rcbuf {
  /* custom memory allocator belongs to the mem parameter when
     creating this object. */
  void *mem_user_data;
  nghttp2_free free;
  /* The pointer to the underlying buffer */
  uint8_t *base;
  /* Size of buffer pointed by |base|. */
  size_t len;
  /* Reference count */
  int32_t ref;
};

/*
 * Allocates nghttp2_rcbuf object with |size| as initial buffer size.
 * When the function succeeds, the reference count becomes 1.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP2_ERR_NOMEM:
 *     Out of memory.
 */
int nghttp2_rcbuf_new(nghttp2_rcbuf **rcbuf_ptr, size_t size, nghttp2_mem *mem);

/*
 * Like nghttp2_rcbuf_new(), but initializes the buffer with |src| of
 * length |srclen|.  This function allocates additional byte at the
 * end and puts '\0' into it, so that the resulting buffer could be
 * used as NULL-terminated string.  Still (*rcbuf_ptr)->len equals to
 * |srclen|.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGHTTP2_ERR_NOMEM:
 *     Out of memory.
 */
int nghttp2_rcbuf_new2(nghttp2_rcbuf **rcbuf_ptr, const uint8_t *src,
                       size_t srclen, nghttp2_mem *mem);

/*
 * Frees |rcbuf| itself, regardless of its reference cout.
 */
void nghttp2_rcbuf_del(nghttp2_rcbuf *rcbuf);

#endif /* NGHTTP2_RCBUF_H */
