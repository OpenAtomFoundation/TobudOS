/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_MEM_H
#define NGHTTP2_MEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"
#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#endif

/* The default, system standard memory allocator */
nghttp2_mem *nghttp2_mem_default(void);

/* Convenient wrapper functions to call allocator function in
   |mem|. */
void *nghttp2_mem_malloc(nghttp2_mem *mem, size_t size);
void nghttp2_mem_free(nghttp2_mem *mem, void *ptr);
void nghttp2_mem_free2(nghttp2_free free_func, void *ptr, void *mem_user_data);
void *nghttp2_mem_calloc(nghttp2_mem *mem, size_t nmemb, size_t size);
void *nghttp2_mem_realloc(nghttp2_mem *mem, void *ptr, size_t size);

#endif /* NGHTTP2_MEM_H */
