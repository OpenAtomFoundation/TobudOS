/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "nghttp2_callbacks.h"
#include <string.h>
#include <stdlib.h>
#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#endif

extern void *HAL_Malloc(uint32_t size);
extern void *HAL_Realloc(void *ptr, uint32_t size);
extern void HAL_Free(void *ptr);

#if INFRA_MEM_STATS
#define NGHTTP2_CB_MALLOC(size)         LITE_malloc(size, MEM_MAGIC, "nghttp2.cb")
#define NGHTTP2_CB_FREE(ptr)            LITE_free(ptr)
#else
#define NGHTTP2_CB_MALLOC(size)         HAL_Malloc(size)
#define NGHTTP2_CB_FREE(ptr)            {HAL_Free((void *)ptr);ptr = NULL;}
#endif

int nghttp2_session_callbacks_new(nghttp2_session_callbacks **callbacks_ptr) {
  *callbacks_ptr = NGHTTP2_CB_MALLOC(sizeof(nghttp2_session_callbacks));

  if (*callbacks_ptr == NULL) {
    return NGHTTP2_ERR_NOMEM;
  }

  memset(*callbacks_ptr, 0, sizeof(nghttp2_session_callbacks));
  return 0;
}

void nghttp2_session_callbacks_del(nghttp2_session_callbacks *callbacks) {
  NGHTTP2_CB_FREE(callbacks);
}

void nghttp2_session_callbacks_set_send_callback(
    nghttp2_session_callbacks *cbs, nghttp2_send_callback send_callback) {
  cbs->send_callback = send_callback;
}

void nghttp2_session_callbacks_set_recv_callback(
    nghttp2_session_callbacks *cbs, nghttp2_recv_callback recv_callback) {
  cbs->recv_callback = recv_callback;
}

void nghttp2_session_callbacks_set_on_frame_recv_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_frame_recv_callback on_frame_recv_callback) {
  cbs->on_frame_recv_callback = on_frame_recv_callback;
}

void nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_invalid_frame_recv_callback on_invalid_frame_recv_callback) {
  cbs->on_invalid_frame_recv_callback = on_invalid_frame_recv_callback;
}

void nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_data_chunk_recv_callback on_data_chunk_recv_callback) {
  cbs->on_data_chunk_recv_callback = on_data_chunk_recv_callback;
}

void nghttp2_session_callbacks_set_before_frame_send_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_before_frame_send_callback before_frame_send_callback) {
  cbs->before_frame_send_callback = before_frame_send_callback;
}

void nghttp2_session_callbacks_set_on_frame_send_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_frame_send_callback on_frame_send_callback) {
  cbs->on_frame_send_callback = on_frame_send_callback;
}

void nghttp2_session_callbacks_set_on_frame_not_send_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_frame_not_send_callback on_frame_not_send_callback) {
  cbs->on_frame_not_send_callback = on_frame_not_send_callback;
}

void nghttp2_session_callbacks_set_on_stream_close_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_stream_close_callback on_stream_close_callback) {
  cbs->on_stream_close_callback = on_stream_close_callback;
}

void nghttp2_session_callbacks_set_on_begin_headers_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_begin_headers_callback on_begin_headers_callback) {
  cbs->on_begin_headers_callback = on_begin_headers_callback;
}

void nghttp2_session_callbacks_set_on_header_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_header_callback on_header_callback) {
  cbs->on_header_callback = on_header_callback;
}

void nghttp2_session_callbacks_set_on_header_callback2(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_header_callback2 on_header_callback2) {
  cbs->on_header_callback2 = on_header_callback2;
}

void nghttp2_session_callbacks_set_on_invalid_header_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_invalid_header_callback on_invalid_header_callback) {
  cbs->on_invalid_header_callback = on_invalid_header_callback;
}

void nghttp2_session_callbacks_set_on_invalid_header_callback2(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_invalid_header_callback2 on_invalid_header_callback2) {
  cbs->on_invalid_header_callback2 = on_invalid_header_callback2;
}

void nghttp2_session_callbacks_set_select_padding_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_select_padding_callback select_padding_callback) {
  cbs->select_padding_callback = select_padding_callback;
}

void nghttp2_session_callbacks_set_data_source_read_length_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_data_source_read_length_callback data_source_read_length_callback) {
  cbs->read_length_callback = data_source_read_length_callback;
}

void nghttp2_session_callbacks_set_on_begin_frame_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_begin_frame_callback on_begin_frame_callback) {
  cbs->on_begin_frame_callback = on_begin_frame_callback;
}

void nghttp2_session_callbacks_set_send_data_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_send_data_callback send_data_callback) {
  cbs->send_data_callback = send_data_callback;
}

void nghttp2_session_callbacks_set_pack_extension_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_pack_extension_callback pack_extension_callback) {
  cbs->pack_extension_callback = pack_extension_callback;
}

void nghttp2_session_callbacks_set_unpack_extension_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_unpack_extension_callback unpack_extension_callback) {
  cbs->unpack_extension_callback = unpack_extension_callback;
}

void nghttp2_session_callbacks_set_on_extension_chunk_recv_callback(
    nghttp2_session_callbacks *cbs,
    nghttp2_on_extension_chunk_recv_callback on_extension_chunk_recv_callback) {
  cbs->on_extension_chunk_recv_callback = on_extension_chunk_recv_callback;
}

void nghttp2_session_callbacks_set_error_callback(
    nghttp2_session_callbacks *cbs, nghttp2_error_callback error_callback) {
  cbs->error_callback = error_callback;
}

void nghttp2_session_callbacks_set_error_callback2(
    nghttp2_session_callbacks *cbs, nghttp2_error_callback2 error_callback2) {
  cbs->error_callback2 = error_callback2;
}
