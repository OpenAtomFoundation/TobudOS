/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef IOT_EXPORT_HTTP2_H
#define IOT_EXPORT_HTTP2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "http2_api.h"
#include "http2_wrapper.h"

#ifdef INFRA_LOG
#include "infra_log.h"
#define h2_emerg(...)                   log_emerg("h2", __VA_ARGS__)
#define h2_crit(...)                    log_crit("h2", __VA_ARGS__)
#define h2_err(...)                     log_err("h2", __VA_ARGS__)
#define h2_warning(...)                 log_warning("h2", __VA_ARGS__)
#define h2_info(...)                    log_info("h2", __VA_ARGS__)
#define h2_debug(...)                   log_debug("h2", __VA_ARGS__)
#else
#define h2_emerg(...)                   do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define h2_crit(...)                    do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define h2_err(...)                     do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define h2_warning(...)                 do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define h2_info(...)                    do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define h2_debug(...)                   do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#endif

#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#define HTTP2_STREAM_MALLOC(size)       LITE_malloc(size, MEM_MAGIC, "http2.stream")
#define HTTP2_STREAM_FREE(ptr)          LITE_free(ptr)
#else
#define HTTP2_STREAM_MALLOC(size)       HAL_Malloc(size)
#define HTTP2_STREAM_FREE(ptr)          do{if(ptr != NULL) {HAL_Free(ptr);ptr = NULL;}}while(0)
#endif /* #ifdef INFRA_MEM_STATS */

#define POINTER_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            return (err); \
        } \
    } while(0)

#define ARGUMENT_SANITY_CHECK(expr, err) \
    do { \
        if (!(expr)) { \
            return (err); \
        } \
    } while(0)


typedef enum {

    HTTP2_FLAG_NONE = 0,

    HTTP2_FLAG_END_STREAM = 0x01,

} http2_flag;

typedef struct http2_list_s {
    struct http2_list_s *prev;
    struct http2_list_s *next;
} http2_list_t;

typedef void (*on_user_header_callback)(int32_t stream_id, int cat, const uint8_t *name, uint32_t namelen,
                                        const uint8_t *value, uint32_t valuelen, uint8_t flags);

typedef void (*on_user_chunk_recv_callback)(int32_t stream_id,
        const uint8_t *data, uint32_t len, uint8_t flags);

typedef void (*on_user_stream_close_callback)(int32_t stream_id, uint32_t error_code);

typedef void (*on_user_frame_send_callback)(int32_t stream_id, int type, uint8_t flags);

typedef void (*on_user_frame_recv_callback)(int32_t stream_id, int type, uint8_t flags);

typedef struct {
    on_user_header_callback       on_user_header_cb;
    on_user_chunk_recv_callback   on_user_chunk_recv_cb;
    on_user_stream_close_callback on_user_stream_close_cb;
    on_user_frame_send_callback   on_user_frame_send_cb;
    on_user_frame_recv_callback   on_user_frame_recv_cb;
} http2_user_cb_t;

typedef struct http2_connection {
    void           *network;       /* iot network ptr */
    void           *session;       /* http2 session */
    int            flag;           /* check the stream is end or not */
    int            status;
    http2_user_cb_t *cbs;
} http2_connection_t;

typedef struct http2_data_struct {
    http2_header *header;  /* header data. */
    int header_count;      /* the count of header data. */
    char *data;            /* send data. */
    int len;               /* send data length. */
    int stream_id;         /* send data over specify stream */
    int flag;              /* send data flag. */
} http2_data;

typedef struct {
    http2_connection_t   *http2_connect;
    void                 *mutex;
    void                 *semaphore;
    void                 *rw_thread;
    http2_list_t         stream_list;
    int                  init_state;
    http2_stream_cb_t    *cbs;
    uint8_t              connect_state;
    uint8_t              retry_cnt;
} stream_handle_t;

#ifdef FS_ENABLED

typedef struct {
    char fs_upload_id[50];
    int fs_offset;
} fs_rsp_header_val_t;

int IOT_HTTP2_FS_Close(void *hd, stream_data_info_t *info, header_ext_info_t *header);

#endif /* #ifdef FS_ENABLED */

/**
* @brief          the http2 client connect.
* @param[in]      pclient: http client. <struct httpclient_t>
* @return         http2 client connection handler.
*/
extern http2_connection_t *iotx_http2_client_connect(void *pclient, char *url, int port);

http2_connection_t *iotx_http2_client_connect_with_cb(void *pclient, char *url, int port, http2_user_cb_t *cb);
/**
* @brief          the http2 client send data.
* @param[in]      handler: http2 client connection handler.
* @param[in]      data: send data.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_client_send(http2_connection_t *conn, http2_data *h2_data);
/**
* @brief          the http2 client receive data.
* @param[in]      handler: http2 client connection handler.
* @param[in]      data: receive data buffer.
* @param[in]      data_len: buffer length.
* @param[in]      len: receive data length.
* @param[in]      timeout: receive data timeout.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_client_recv(http2_connection_t *conn, char *data, int data_len, int *len, int timeout);
/**
* @brief          the http2 client connect.
* @param[in]      handler: http2 client connection handler.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_client_disconnect(http2_connection_t *conn);
/**
* @brief          the http2 client send ping to keep alive.
* @param[in]      handler: http2 client connection handler.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_client_send_ping(http2_connection_t *conn);
/**
* @brief          the http2 client get available windows size.
* @param[in]      handler: http2 client connection handler.
* @return         The window size.
*/
extern int iotx_http2_get_available_window_size(http2_connection_t *conn);
/**
* @brief          the http2 client receive windows size packet to update window.
* @param[in]      handler: http2 client connection handler.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_update_window_size(http2_connection_t *conn);
/**
* @brief          the http2 client performs the network I/O.
* @param[in]      handler: http2 client connection handler.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_exec_io(http2_connection_t *connection);

extern int iotx_http2_client_recv_ping(void);

int iotx_http2_reset_stream(http2_connection_t *connection, int32_t stream_id);
#ifdef __cplusplus
}
#endif

#endif /* IOT_EXPORT_HTTP2_H */
