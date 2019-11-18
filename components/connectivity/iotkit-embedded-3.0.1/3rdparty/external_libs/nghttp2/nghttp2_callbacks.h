/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_CALLBACKS_H
#define NGHTTP2_CALLBACKS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"

/*
 * Callback functions.
 */
struct nghttp2_session_callbacks {
  /**
   * Callback function invoked when the session wants to send data to
   * the remote peer.  This callback is not necessary if the
   * application uses solely `nghttp2_session_mem_send()` to serialize
   * data to transmit.
   */
  nghttp2_send_callback send_callback;
  /**
   * Callback function invoked when the session wants to receive data
   * from the remote peer.  This callback is not necessary if the
   * application uses solely `nghttp2_session_mem_recv()` to process
   * received data.
   */
  nghttp2_recv_callback recv_callback;
  /**
   * Callback function invoked by `nghttp2_session_recv()` when a
   * frame is received.
   */
  nghttp2_on_frame_recv_callback on_frame_recv_callback;
  /**
   * Callback function invoked by `nghttp2_session_recv()` when an
   * invalid non-DATA frame is received.
   */
  nghttp2_on_invalid_frame_recv_callback on_invalid_frame_recv_callback;
  /**
   * Callback function invoked when a chunk of data in DATA frame is
   * received.
   */
  nghttp2_on_data_chunk_recv_callback on_data_chunk_recv_callback;
  /**
   * Callback function invoked before a non-DATA frame is sent.
   */
  nghttp2_before_frame_send_callback before_frame_send_callback;
  /**
   * Callback function invoked after a frame is sent.
   */
  nghttp2_on_frame_send_callback on_frame_send_callback;
  /**
   * The callback function invoked when a non-DATA frame is not sent
   * because of an error.
   */
  nghttp2_on_frame_not_send_callback on_frame_not_send_callback;
  /**
   * Callback function invoked when the stream is closed.
   */
  nghttp2_on_stream_close_callback on_stream_close_callback;
  /**
   * Callback function invoked when the reception of header block in
   * HEADERS or PUSH_PROMISE is started.
   */
  nghttp2_on_begin_headers_callback on_begin_headers_callback;
  /**
   * Callback function invoked when a header name/value pair is
   * received.
   */
  nghttp2_on_header_callback on_header_callback;
  nghttp2_on_header_callback2 on_header_callback2;
  /**
   * Callback function invoked when a invalid header name/value pair
   * is received which is silently ignored if these callbacks are not
   * set.
   */
  nghttp2_on_invalid_header_callback on_invalid_header_callback;
  nghttp2_on_invalid_header_callback2 on_invalid_header_callback2;
  /**
   * Callback function invoked when the library asks application how
   * many padding bytes are required for the transmission of the given
   * frame.
   */
  nghttp2_select_padding_callback select_padding_callback;
  /**
   * The callback function used to determine the length allowed in
   * `nghttp2_data_source_read_callback()`
   */
  nghttp2_data_source_read_length_callback read_length_callback;
  /**
   * Sets callback function invoked when a frame header is received.
   */
  nghttp2_on_begin_frame_callback on_begin_frame_callback;
  nghttp2_send_data_callback send_data_callback;
  nghttp2_pack_extension_callback pack_extension_callback;
  nghttp2_unpack_extension_callback unpack_extension_callback;
  nghttp2_on_extension_chunk_recv_callback on_extension_chunk_recv_callback;
  nghttp2_error_callback error_callback;
  nghttp2_error_callback2 error_callback2;
};

#endif /* NGHTTP2_CALLBACKS_H */
