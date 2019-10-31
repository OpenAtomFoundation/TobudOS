/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include "nghttp2_option.h"
#include "nghttp2_session.h"
#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#endif

extern void *HAL_Malloc(uint32_t size);
extern void *HAL_Realloc(void *ptr, uint32_t size);
extern void HAL_Free(void *ptr);

#if INFRA_MEM_STATS
#define NGHTTP2_OPTION_MALLOC(size)         LITE_malloc(size, MEM_MAGIC, "nghttp2.option")
#define NGHTTP2_OPTION_FREE(ptr)            LITE_free(ptr)
#else
#define NGHTTP2_OPTION_MALLOC(size)         HAL_Malloc(size)
#define NGHTTP2_OPTION_FREE(ptr)            {HAL_Free((void *)ptr);ptr = NULL;}
#endif

int nghttp2_option_new(nghttp2_option **option_ptr) {
  *option_ptr = NGHTTP2_OPTION_MALLOC(sizeof(nghttp2_option));

  if (*option_ptr == NULL) {
    return NGHTTP2_ERR_NOMEM;
  }

  memset(*option_ptr, 0, sizeof(nghttp2_option));
  return 0;
}

void nghttp2_option_del(nghttp2_option *option) { NGHTTP2_OPTION_FREE(option); }

void nghttp2_option_set_no_auto_window_update(nghttp2_option *option, int val) {
  option->opt_set_mask |= NGHTTP2_OPT_NO_AUTO_WINDOW_UPDATE;
  option->no_auto_window_update = val;
}

void nghttp2_option_set_peer_max_concurrent_streams(nghttp2_option *option,
                                                    uint32_t val) {
  option->opt_set_mask |= NGHTTP2_OPT_PEER_MAX_CONCURRENT_STREAMS;
  option->peer_max_concurrent_streams = val;
}

void nghttp2_option_set_no_recv_client_magic(nghttp2_option *option, int val) {
  option->opt_set_mask |= NGHTTP2_OPT_NO_RECV_CLIENT_MAGIC;
  option->no_recv_client_magic = val;
}

void nghttp2_option_set_no_http_messaging(nghttp2_option *option, int val) {
  option->opt_set_mask |= NGHTTP2_OPT_NO_HTTP_MESSAGING;
  option->no_http_messaging = val;
}

void nghttp2_option_set_max_reserved_remote_streams(nghttp2_option *option,
                                                    uint32_t val) {
  option->opt_set_mask |= NGHTTP2_OPT_MAX_RESERVED_REMOTE_STREAMS;
  option->max_reserved_remote_streams = val;
}

static void set_ext_type(uint8_t *ext_types, uint8_t type) {
  ext_types[type / 8] = (uint8_t)(ext_types[type / 8] | (1 << (type & 0x7)));
}

void nghttp2_option_set_user_recv_extension_type(nghttp2_option *option,
                                                 uint8_t type) {
  if (type < 10) {
    return;
  }

  option->opt_set_mask |= NGHTTP2_OPT_USER_RECV_EXT_TYPES;
  set_ext_type(option->user_recv_ext_types, type);
}

void nghttp2_option_set_builtin_recv_extension_type(nghttp2_option *option,
                                                    uint8_t type) {
  switch (type) {
  case NGHTTP2_ALTSVC:
    option->opt_set_mask |= NGHTTP2_OPT_BUILTIN_RECV_EXT_TYPES;
    option->builtin_recv_ext_types |= NGHTTP2_TYPEMASK_ALTSVC;
    return;
  default:
    return;
  }
}

void nghttp2_option_set_no_auto_ping_ack(nghttp2_option *option, int val) {
  option->opt_set_mask |= NGHTTP2_OPT_NO_AUTO_PING_ACK;
  option->no_auto_ping_ack = val;
}

void nghttp2_option_set_max_send_header_block_length(nghttp2_option *option,
                                                     size_t val) {
  option->opt_set_mask |= NGHTTP2_OPT_MAX_SEND_HEADER_BLOCK_LENGTH;
  option->max_send_header_block_length = val;
}

void nghttp2_option_set_max_deflate_dynamic_table_size(nghttp2_option *option,
                                                       size_t val) {
  option->opt_set_mask |= NGHTTP2_OPT_MAX_DEFLATE_DYNAMIC_TABLE_SIZE;
  option->max_deflate_dynamic_table_size = val;
}

void nghttp2_option_set_no_closed_streams(nghttp2_option *option, int val) {
  option->opt_set_mask |= NGHTTP2_OPT_NO_CLOSED_STREAMS;
  option->no_closed_streams = val;
}
