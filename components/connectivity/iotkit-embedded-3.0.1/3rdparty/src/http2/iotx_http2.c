/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nghttp2.h"
#include "nghttp2_session.h"
#include "infra_httpc.h"
#include "http2_internal.h"
#include "http2_wrapper.h"


#define MAX_HTTP2_HOST_LEN                  (128)
#define NGHTTP2_DBG                         h2_info

typedef enum {
    PING_IDLE,
    PING_SENDING,
    PING_SENT,
    PING_RECVED,
} http2_ping_state_t;


enum { IO_NONE, WANT_READ, WANT_WRITE };

typedef struct _http2_request_struct_ {
    /* Stream ID for this request. */
    int32_t stream_id;
} http2_request;


extern const char *iotx_ca_get(void);
extern int httpclient_connect(httpclient_t *client);
static int http2_nv_copy_nghttp2_nv(nghttp2_nv *nva, int start, http2_header *nva_copy, int end);
/*static int http2_parse_host(char *url, char *host, size_t maxHostLen);*/

static http2_ping_state_t ping_state = PING_IDLE;

int g_recv_timeout = 10;

int set_http2_recv_timeout(int timeout)
{
    g_recv_timeout = timeout;
    return 1;
}

static ssize_t send_callback(nghttp2_session *session, const uint8_t *data,
                             size_t length, int flags, void *user_data)
{
    http2_connection_t *connection;
    httpclient_t  *client;
    int rv;

    connection = (http2_connection_t *)user_data;
    if (connection == NULL) {
        return 0;
    }

    NGHTTP2_DBG("send_callback data len %d, session->remote_window_size=%d!\r\n", (int)length,
                session->remote_window_size);
    if (session->remote_window_size < length * 2) {
        HAL_SleepMs(50);
        NGHTTP2_DBG("wait a munite ....");
    }
    /*if(length < 50)
        LITE_hexdump("data:", data, length);*/
    client = (httpclient_t *)connection->network;
    rv = client->net.write(&client->net, (char *)data, length, 5000);
    NGHTTP2_DBG("send_callback data ends len = %d!\r\n", rv);
    if (rv < 0 || rv < length) {
        rv = NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    return rv;
}


/**
* @brief      The implementation of nghttp2_recv_callback type. Here we read |data| from the network
*             and write them in |buf|. The capacity of |buf| is |length| bytes. Returns the number of
*             bytes stored in |buf|. See the documentation of nghttp2_recv_callback for the details.
*             To set this callback to :type:`nghttp2_session_callbacks`, use
*             `nghttp2_session_callbacks_set_on_frame_send_callback()`.
* @param[in]  session: nghttp2 session.
* @param[in]  buf: receive data buffer.
* @param[in]  length: data length.
* @param[in]  flags: no using.
* @param[in]  user_data: user data.
* @return     Received data length.
 */
static ssize_t recv_callback(nghttp2_session *session, uint8_t *buf,
                             size_t length, int flags, void *user_data)
{
    http2_connection_t *connection;
    int rv;
    httpclient_t  *client;

    connection = (http2_connection_t *)user_data;
    if (connection == NULL) {
        return 0;
    }

    client = (httpclient_t *)connection->network;

    rv = client->net.read(&client->net, (char *)buf, length, g_recv_timeout);
    /* NGHTTP2_DBG("recv_callback len= %d\r\n", rv); */
    if (rv < 0) {
        rv = NGHTTP2_ERR_CALLBACK_FAILURE;
    } else if (rv == 0) {
        rv = 0;
    }
    return rv;
}
/**
* @brief       Callback function invoked after the frame |frame| is sent.
*              To set this callback to :type:`nghttp2_session_callbacks`, use
*              `nghttp2_session_callbacks_set_on_frame_send_callback()`.
* @param[in]   session: nghttp2 session.
* @param[in]   frame: nghttp2 frame.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/
static int on_frame_send_callback(nghttp2_session *session,
                                  const nghttp2_frame *frame,
                                  void *user_data)
{
    size_t i;

    http2_connection_t *connection  = (http2_connection_t *)user_data;
    if (connection == NULL) {
        return 0;
    }

    switch (frame->hd.type) {
        case NGHTTP2_HEADERS: {
            const nghttp2_nv *nva = frame->headers.nva;
            NGHTTP2_DBG("[INFO] C ---------> S (HEADERS) stream_id [%d]\n", frame->hd.stream_id);
            for (i = 0; i < frame->headers.nvlen; ++i) {
                NGHTTP2_DBG("> %s: %s\n", nva[i].name, nva[i].value);
            }
            (void)nva;
        } break;
        case NGHTTP2_RST_STREAM: {
            NGHTTP2_DBG("[INFO] C ---------> S (RST_STREAM)\n");
        } break;
        case NGHTTP2_GOAWAY: {
            NGHTTP2_DBG("[INFO] C ---------> S (GOAWAY) code = %d\n",frame->goaway.error_code);
        } break;
        case NGHTTP2_PING: {
            NGHTTP2_DBG("[INFO] C ---------> S (PING)\n");
            ping_state = PING_SENDING;
        } break;
        default: break;
    }

    if (connection->cbs && connection->cbs->on_user_frame_send_cb) {
        connection->cbs->on_user_frame_send_cb(frame->hd.stream_id, frame->hd.type, frame->hd.flags);
    }
    return 0;
}


/**
* @brief       Callback function invoked by `nghttp2_session_recv()` and `nghttp2_session_mem_recv()` when a frame is received.
*              If frame is HEADERS or PUSH_PROMISE, the ``nva`` and ``nvlen``member of their data structure are always
*              ``NULL`` and 0 respectively.  The header name/value pairs are emitted via:type:`nghttp2_on_header_callback`
*              To set this callback to :type:`nghttp2_session_callbacks`, use`nghttp2_session_callbacks_set_on_frame_send_callback()`.
*              For HEADERS, PUSH_PROMISE and DATA frames, this callback may be called after stream is closed (see:type:
*              `nghttp2_on_stream_close_callback`).  The application should check that stream is still alive using its own stream
*              management or :func:`nghttp2_session_get_stream_user_data()`.
*              Only HEADERS and DATA frame can signal the end of incoming data. If ``frame->hd.flags & NGHTTP2_FLAG_END_STREAM``
*              is nonzero, the|frame| is the last frame from the remote peer in this stream.
*              This callback won't be called for CONTINUATION frames.
*              HEADERS/PUSH_PROMISE + CONTINUATIONs are treated as single frame.
* @param[in]   session: nghttp2 session.
* @param[in]   frame: nghttp2 frame.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/
static int on_frame_recv_callback(nghttp2_session *session,
                                  const nghttp2_frame *frame,
                                  void *user_data)
{
    http2_connection_t *connection  = (http2_connection_t *)user_data;
    http2_request *req;
    NGHTTP2_DBG("on_frame_recv_callback, type = %d\n", frame->hd.type);
    NGHTTP2_DBG("on_frame_recv_callback, stream_id = %d\n", frame->hd.stream_id);

    if (connection == NULL) {
        return 0;
    }

    req = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
    if (req == NULL) {
        NGHTTP2_DBG("stream user data is not exist\n");
    }

    switch (frame->hd.type) {
        case NGHTTP2_HEADERS: {
            if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE) {

            }
        } break;
        case NGHTTP2_RST_STREAM: {
            connection->status = 0;
            NGHTTP2_DBG("[INFO] C <--------- S (RST_STREAM)\n");
        } break;
        case NGHTTP2_GOAWAY: {
            connection->status = 0;
            NGHTTP2_DBG("[INFO] C <--------- S (GOAWAY) code = %d\n",frame->goaway.error_code);
        } break;
        case NGHTTP2_DATA: {
            if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
                NGHTTP2_DBG("end stream flag\r\n");
            }
        } break;
        case NGHTTP2_PING: {
            NGHTTP2_DBG("[INFO] C <--------- S (PING)\n");
            ping_state = PING_RECVED;
        } break;
    }

    if (connection->cbs && connection->cbs->on_user_frame_recv_cb) {
        connection->cbs->on_user_frame_recv_cb(frame->hd.stream_id, frame->hd.type, frame->hd.flags);
    }
    return 0;
}

/**
* @brief       Callback function invoked when the stream |stream_id| is closed.
*              We use this function to know if the response is fully received. Since we just fetch 1 resource in this program, after
*              the response is received, we submit GOAWAY and close the session.
* @param[in]   session: nghttp2 session.
* @param[in]   stream_id: stream id.
* @param[in]   error_code: The reason of closure.
*              Usually one of :enum:`nghttp2_error_code`, but that is not guaranteed.  The stream_user_data, which was specified in
*              `nghttp2_submit_request()` or `nghttp2_submit_headers()`, is still available in this function.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
 */
static int on_h2_stream_close_callback(nghttp2_session *session, int32_t stream_id,
                                       uint32_t error_code,
                                       void *user_data)
{
    http2_request *req;
    http2_connection_t *connection  = (http2_connection_t *)user_data;

    if (connection == NULL) {
        return 0;
    }
    req = nghttp2_session_get_stream_user_data(session, stream_id);
    if (req) {
        int rv;
        rv = nghttp2_session_terminate_session(session, NGHTTP2_NO_ERROR);

        if (rv != 0) {
            NGHTTP2_DBG("stream close nghttp2_session_terminate_session\r\n");
        }
    }
    if (connection->cbs && connection->cbs->on_user_stream_close_cb) {
        connection->cbs->on_user_stream_close_cb(stream_id, error_code);
    }
    return 0;
}


/**
* @brief        Callback function invoked when a chunk of data in DATA frame is received.
*               The implementation of nghttp2_on_data_chunk_recv_callback type. We use this function to print the received response body.
* @param[in]    session: nghttp2 session.
* @param[in]    flags: no using.
* @param[in]    stream_id: the stream ID this DATA frame belongs to.
* @param[in]    data: receive data.
* @param[in]    len: data length.
* @param[in]    user_data: The |user_data| pointer is the third argument passed in to the call to
*               `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return       The implementation of this function must return 0 if it succeeds.
*               If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*               and `nghttp2_session_mem_send()` functions immediately return :enum:
*               `NGHTTP2_ERR_CALLBACK_FAILURE`.
 */
static int on_data_chunk_recv_callback(nghttp2_session *session,
                                       uint8_t flags, int32_t stream_id,
                                       const uint8_t *data, size_t len,
                                       void *user_data)
{
    http2_request *req;
    http2_connection_t *connection = (http2_connection_t *)user_data;

    if (connection == NULL) {
        return 0;
    }

    req = nghttp2_session_get_stream_user_data(session, stream_id);
    if (req) {
        NGHTTP2_DBG("stream user data is not exist\n");
    }
    NGHTTP2_DBG("[INFO] C <----------- S (DATA chunk) stream_id [%d] :: %lu bytes\n", stream_id, (unsigned long int)len);

    if (connection->cbs && connection->cbs->on_user_chunk_recv_cb) {
        connection->cbs->on_user_chunk_recv_cb(stream_id, data, len, flags);
    }

    nghttp2_session_consume_connection(session, len);
    nghttp2_session_consume(session, stream_id, len);
    nghttp2_submit_window_update(session, NGHTTP2_FLAG_NONE, 0, len);
    nghttp2_submit_window_update(session, NGHTTP2_FLAG_NONE, stream_id, len);
    nghttp2_session_send(session);

    return 0;
}


/**
* @brief       Callback function invoked when a header name/value pair is received.
*              The implementation of nghttp2_on_data_chunk_recv_callback type. We use this function to print the received response body.
* @param[in]   session: nghttp2 session.
* @param[in]   frame: nghttp2 frame.
* @param[in]   name: header name.
* @param[in]   namelen: length of header name.
* @param[in]   value: header value.
* @param[in]   valuelen: length of header value.
* @param[in]   flags: no using.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/


static int on_header_callback(nghttp2_session *session,
                              const nghttp2_frame *frame, const uint8_t *name,
                              size_t namelen, const uint8_t *value,
                              size_t valuelen, uint8_t flags,
                              void *user_data)
{
    http2_connection_t *connection  = (http2_connection_t *)user_data;
    if (connection == NULL) {
        return 0;
    }
    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:

            if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE) {
                http2_connection_t *connection  = (http2_connection_t *)user_data;
                /* Print response headers for the initiated request. */
                NGHTTP2_DBG("< %s: %s\n", name, value);

                if (connection->cbs && connection->cbs->on_user_header_cb) {
                    connection->cbs->on_user_header_cb(frame->hd.stream_id, (int)frame->headers.cat, name, namelen, value, valuelen, flags);
                }
                break;
            }

    }
    return 0;
}

/**
* @brief        Called when nghttp2 library gets started to receive header block.
* @param[in]    session: nghttp2 session.
* @param[in]    frame: nghttp2 frame.
* @param[in]    user_data: The |user_data| pointer is the third argument passed in to the call to
*               `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return       The implementation of this function must return 0 if it succeeds.
*               If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*               and `nghttp2_session_mem_send()` functions immediately return :enum:
*               `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/
static int on_begin_headers_callback(nghttp2_session *session,
                                     const nghttp2_frame *frame,
                                     void *user_data)
{
    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:
            if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE) {
                NGHTTP2_DBG("[INFO] C <--------- S (HEADERS) stream_id [%d]\n", (int)frame->hd.stream_id);
            }
            break;
    }
    return 0;
}


/**
* @brief           Setup callback functions.
*                  nghttp2 API offers many callback functions, but most of them are optional. The send_callback is always required.
*                  Since we use nghttp2_session_recv(), the recv_callback is also required.
* @param[in|out]   callbacks: nghttp2 callbacks.
* @return          None.
 */
static void setup_nghttp2_callbacks(nghttp2_session_callbacks *callbacks)
{
    nghttp2_session_callbacks_set_send_callback(callbacks, send_callback);

    nghttp2_session_callbacks_set_recv_callback(callbacks, recv_callback);

    nghttp2_session_callbacks_set_on_frame_send_callback(callbacks,
            on_frame_send_callback);

    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks,
            on_frame_recv_callback);

    nghttp2_session_callbacks_set_on_stream_close_callback(
                callbacks, on_h2_stream_close_callback);

    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
                callbacks, on_data_chunk_recv_callback);

    nghttp2_session_callbacks_set_on_header_callback(callbacks,
            on_header_callback);

    nghttp2_session_callbacks_set_on_begin_headers_callback(
                callbacks, on_begin_headers_callback);

}


static ssize_t data_read_callback(nghttp2_session *session, int32_t stream_id,
                                  uint8_t *buf, size_t length,
                                  uint32_t *data_flags,
                                  nghttp2_data_source *source,
                                  void *user_data)
{
    int len = 0;
    http2_connection_t *connection = (http2_connection_t *)user_data;
    if (connection == NULL) {
        return 0;
    }

    if (source->ptr == NULL) {
        return 0;
    }
    if (connection != NULL && connection->flag != NGHTTP2_FLAG_END_STREAM) {
        *data_flags |= NGHTTP2_DATA_FLAG_NO_END_STREAM;
        connection->flag = NGHTTP2_FLAG_NONE;
    }
    *data_flags |= NGHTTP2_DATA_FLAG_EOF;

    /*len  = strlen((char *)source->ptr);*/
    len  = source->len;

    if (length < len) {
        len = length;
    }
    memcpy(buf, source->ptr, len);
    return len;
}


static int http2_nv_copy_nghttp2_nv(nghttp2_nv *nva, int start, http2_header *nva_copy, int end)
{
    int i, j;
    for (i = start, j = 0; j < end; i++, j++) {
        nva[i].flags = NGHTTP2_NV_FLAG_NONE;
        nva[i].name = (uint8_t *)nva_copy[j].name;
        nva[i].value = (uint8_t *)nva_copy[j].value;
        nva[i].namelen = nva_copy[j].namelen;
        nva[i].valuelen = nva_copy[j].valuelen;
    }
    return i;
}


/**
* @brief          Connect the SSL client.
* @param[in]      pclient: http client.
* @param[in]      url. destination url.
* @param[in]      port. destination port.
* @param[in]      ssl_config: custome config.
* @return         The result. 0 is ok.
*/
static int http2_client_conn(httpclient_t *pclient, char *url, int port)
{
    int ret = 0;
    /*char host[MAX_HTTP2_HOST_LEN] = { 0 };*/

    /*http2_parse_host(url, host, sizeof(host));*/
    if (0 == pclient->net.handle) {
        /* Establish connection if no. */
        extern const char *iotx_ca_crt;

        ret = iotx_net_init(&pclient->net, url, port, iotx_ca_crt);

        if (0 != ret) {
            return ret;
        }
        ret = httpclient_connect(pclient);
        if (0 != ret) {
            h2_err("http2client_connect is error, ret = %d", ret);
            httpclient_close(pclient);
            return ret;
        }
    }
    return ret;
}

int iotx_http2_client_send(http2_connection_t *conn, http2_data *h2_data)
{
    int send_flag = 0;
    int rv = 0;
    nghttp2_data_provider data_prd;
    nghttp2_nv *nva = NULL;
    int nva_size = 0;
    http2_header *header = h2_data->header;
    int header_count = h2_data->header_count;
    char *data = h2_data->data;
    int len = h2_data->len;
    int stream_id = h2_data->stream_id;
    int flags = h2_data->flag;

    if (conn == NULL) {
        return -1;
    }

    if (header != NULL && header_count != 0) {
        nva = (nghttp2_nv *)HTTP2_STREAM_MALLOC(sizeof(nghttp2_nv) * header_count);
        if (nva == NULL) {
            return -1;
        }
        nva_size = http2_nv_copy_nghttp2_nv(nva, nva_size, header, header_count);
    }
    /*upload to server*/
    if (data != NULL && len != 0) {
        data_prd.source.ptr = data;
        data_prd.source.len = len;
        data_prd.read_callback = data_read_callback;
        if (nva_size != 0) {
            rv = nghttp2_submit_request(conn->session, NULL, nva, nva_size, &data_prd, NULL);
            h2_data->stream_id = rv;
        } else {
            rv = nghttp2_submit_data(conn->session, flags, stream_id, &data_prd);
        }
    } else {
        rv = nghttp2_submit_request(conn->session, NULL, nva, nva_size, NULL, NULL);
        h2_data->stream_id = rv;
    }
    HTTP2_STREAM_FREE(nva);

    if (rv < 0) {
        return rv;
    }

    send_flag = nghttp2_session_want_write(conn->session);
    if (send_flag) {
        rv = nghttp2_session_send(conn->session);
        NGHTTP2_DBG("nghttp2_session_send %d\r\n", rv);
    }

    return rv;
}

int iotx_http2_client_recv(http2_connection_t *conn, char *data, int data_len, int *len, int timeout)
{
    int rv = 0;
    int read_flag = 0;

    if (conn == NULL) {
        return -1;
    }

    set_http2_recv_timeout(timeout);
    read_flag = nghttp2_session_want_read(conn->session);
    if (read_flag) {
        rv = nghttp2_session_recv(conn->session);
        NGHTTP2_DBG("nghttp2_client_recv %d\r\n", rv);
        if (rv < 0) {
            read_flag = 0;
        }
    }
    return rv;
}

/**
* @brief          the http2 client connect.
* @param[in]      pclient: http client.
* @return         http2 client connection handler.
*/
http2_connection_t *iotx_http2_client_connect(void *pclient, char *url, int port)
{
    http2_connection_t *connection;
    nghttp2_session_callbacks *callbacks;
    int rv;
    int ret = 0;

    connection = HTTP2_STREAM_MALLOC(sizeof(http2_connection_t));
    if (connection == NULL) {
        return NULL;
    }
    memset(connection, 0, sizeof(http2_connection_t));

    if (0 != (ret = http2_client_conn((httpclient_t *)pclient, url, port))) {
        NGHTTP2_DBG("https_client_conn failed %d\r\n", ret);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }
    connection->network = pclient;

    rv = nghttp2_session_callbacks_new(&callbacks);
    if (rv != 0) {
        NGHTTP2_DBG("nghttp2_session_callbacks_new1 %d", rv);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }

    setup_nghttp2_callbacks(callbacks);
    rv = nghttp2_session_client_new((nghttp2_session **)&connection->session, callbacks, connection);
    if (rv != 0) {
        NGHTTP2_DBG("nghttp2_session_client_new3 %d", rv);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }
    nghttp2_session_callbacks_del(callbacks);

    nghttp2_submit_settings(connection->session, NGHTTP2_FLAG_NONE, NULL, 0);
#if 0

    parse_uri(&uri, url);
    request_init(&req, &uri);
    /* Submit the HTTP request to the outbound queue. */
    submit_request(connection, &req);
#endif

    rv = nghttp2_session_send(connection->session);
    /*request_free(&req);*/
    if (rv < 0) {
        NGHTTP2_DBG("nghttp2_session_send fail %d", rv);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }
    connection->status = 1;
    return connection;
}

/**
* @brief          the http2 client connect.
* @param[in]      pclient: http client.
* @return         http2 client connection handler.
*/
http2_connection_t *iotx_http2_client_connect_with_cb(void *pclient, char *url, int port, http2_user_cb_t  *cb)
{
    http2_connection_t *connection;
    nghttp2_session_callbacks *callbacks;
    int rv;
    int ret = 0;

    connection = HTTP2_STREAM_MALLOC(sizeof(http2_connection_t));
    if (connection == NULL) {
        return NULL;
    }
    memset(connection, 0, sizeof(http2_connection_t));

    if (0 != (ret = http2_client_conn((httpclient_t *)pclient, url, port))) {
        NGHTTP2_DBG("https_client_conn failed %d\r\n", ret);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }
    connection->network = pclient;

    rv = nghttp2_session_callbacks_new(&callbacks);
    if (rv != 0) {
        NGHTTP2_DBG("nghttp2_session_callbacks_new1 %d", rv);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }

    connection->cbs = cb;
    setup_nghttp2_callbacks(callbacks);

    rv = nghttp2_session_client_new((nghttp2_session **)&connection->session, callbacks, connection);
    if (rv != 0) {
        NGHTTP2_DBG("nghttp2_session_client_new3 %d", rv);
        nghttp2_session_callbacks_del(callbacks);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }
    nghttp2_session_callbacks_del(callbacks);

    nghttp2_submit_settings(connection->session, NGHTTP2_FLAG_NONE, NULL, 0);
#if 0

    parse_uri(&uri, url);
    request_init(&req, &uri);
    /* Submit the HTTP request to the outbound queue. */
    submit_request(connection, &req);
#endif

    rv = nghttp2_session_send(connection->session);
    /*request_free(&req);*/
    if (rv < 0) {
        nghttp2_session_del(connection->session);
        NGHTTP2_DBG("nghttp2_session_send fail %d", rv);
        HTTP2_STREAM_FREE(connection);
        return NULL;
    }
    connection->status = 1;
    return connection;
}

int iotx_http2_client_disconnect(http2_connection_t *conn)
{
    /* Resource cleanup */
    if (conn == NULL) {
        return -1;
    }
    httpclient_close((httpclient_t *)conn->network);
    nghttp2_session_del(conn->session);
    HTTP2_STREAM_FREE(conn);
    return 0;
}

int iotx_http2_client_send_ping(http2_connection_t *conn)
{
    int rv = 0;
    int send_flag;

    ping_state = PING_IDLE;

    if (conn == NULL) {
        return -1;
    }
    rv = nghttp2_submit_ping(conn->session, NGHTTP2_FLAG_NONE, NULL);
    if (rv < 0) {
        return rv;
    }
    send_flag = nghttp2_session_want_write(conn->session);
    if (send_flag) {
        rv = nghttp2_session_send(conn->session);
        NGHTTP2_DBG("nghttp2_session_send %d\r\n", rv);
        if (rv < 0) {
            return rv;
        }
    }

    ping_state = PING_SENDING;
    return 0;
}

int iotx_http2_client_recv_ping(void)
{
    if (ping_state == PING_RECVED || ping_state == PING_IDLE) {
        NGHTTP2_DBG("ping recv secceed\r\n");
        return 0;
    }
    else {
        NGHTTP2_DBG("ping recv timeout");
        return -1;
    }
}

int iotx_http2_get_available_window_size(http2_connection_t *conn)
{
    int windows_size = 0;

    if (conn == NULL) {
        return -1;
    }

    windows_size = nghttp2_session_get_remote_window_size(conn->session);
    return windows_size;
}


int iotx_http2_update_window_size(http2_connection_t *conn)
{
    int rv;

    if (conn == NULL) {
        return -1;
    }

    rv = nghttp2_session_recv(conn->session);
    if (rv < 0) {
        return -1;
    }
    return 0;
}

/*
 * Performs the network I/O.
 */
int iotx_http2_exec_io(http2_connection_t *connection)
{
    if (connection == NULL) {
        return -1;
    }

    if (nghttp2_session_want_read(connection->session) /*||
        nghttp2_session_want_write(connection->session)*/) {

        int rv;
        rv = nghttp2_session_recv(connection->session);
        if (rv < 0) {
            NGHTTP2_DBG("nghttp2_session_recv error");
            return -1;
        }
        /* rv = nghttp2_session_send(connection->session); */
        /* if (rv < 0) { */
        /* NGHTTP2_DBG("nghttp2_session_send error"); */
        /* return -1; */
        /* } */
    }
    return 0;
}

int iotx_http2_reset_stream(http2_connection_t *connection, int32_t stream_id)
{
    int rv = 0;
    if(connection == NULL){
        return -1;
    }
    if(!nghttp2_session_get_stream_local_close(connection->session,stream_id)) {
        rv = nghttp2_submit_rst_stream(connection->session,0, stream_id, NGHTTP2_NO_ERROR);
    }
    if (rv < 0) {
        return rv;
    }

    rv = nghttp2_session_want_write(connection->session);
    if (rv) {
        rv = nghttp2_session_send(connection->session);
        NGHTTP2_DBG("nghttp2_session_send %d\r\n", rv);
    }
    return rv;
}
