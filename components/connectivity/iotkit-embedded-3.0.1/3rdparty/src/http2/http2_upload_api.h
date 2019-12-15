/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _HTTP2_UPLOAD_API_H_
#define _HTTP2_UPLOAD_API_H_

#include "infra_types.h"
#include "infra_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* bit define of file override option */
#define UPLOAD_FILE_OPT_BIT_OVERWRITE       (0x00000001)
#define UPLOAD_FILE_OPT_BIT_RESUME          (0x00000002)
#define UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN    (0x00000004)

/* http2 connect information */
typedef struct {
    char  *product_key;
    char  *device_name;
    char  *device_secret;
    char  *url;
    int   port;
} http2_upload_conn_info_t;

/* file upload option define */
typedef struct {
    const char *file_path;      /* file path, filename must be ASCII string and strlen < 2014 */
    uint32_t part_len;          /* maximum content len of one http2 request, must be in range of 100KB ~ 100MB */
    const char *upload_id;      /* a specific id used to indicate one upload session, only required when UPLOAD_FILE_OPT_BIT_RESUME option set */
    uint32_t upload_len;        /* used to indicate the upload length, only required when UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN option set */
    uint32_t opt_bit_map;       /* option bit map, support UPLOAD_FILE_OPT_BIT_OVERWRITE, UPLOAD_FILE_OPT_BIT_RESUME and UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN */
} http2_upload_params_t;

/* error code for file upload */
typedef enum {
    UPLOAD_STOP_BY_IOCTL        = -14,
    UPLOAD_HTTP2_HANDLE_NULL    = -13,
    UPLOAD_LEN_IS_ZERO          = -12,
    UPLOAD_FILE_PATH_IS_NULL    = -11,
    UPLOAD_ID_IS_NULL           = -10,
    UPLOAD_FILE_NOT_EXIST     = -9,
    UPLOAD_FILE_READ_FAILED   = -8,
    UPLOAD_STREAM_OPEN_FAILED = -7,
    UPLOAD_STREAM_SEND_FAILED = -6,
    UPLOAD_MALLOC_FAILED      = -5,
    UPLOAD_NULL_POINT         = -2,
    UPLOAD_ERROR_COMMON       = -1,
    UPLOAD_SUCCESS            = 0,
} http2_file_upload_result_t;

/* gerneral callback function, this callback will be invoke when http2 disconnected */
typedef void (*http2_disconnect_cb_t)(void);

/* gerneral callback function, this callback will be invoke when http2 reconnected */
typedef void (*http2_reconnect_cb_t)(void);

/* callback function type define, this callback will be invoke when upload completed */
typedef void (*http2_upload_completed_cb_t)(const char *file_path, int result, void *user_data);

/* callback funciton type define, this callback will be invoke when upload_id received */
typedef void (*http2_upload_id_received_cb_t)(const char *file_path, const char *upload_id, void *user_data);

/* http2 connect status callback define */
typedef struct {
    http2_disconnect_cb_t   on_disconnect_cb;
    http2_reconnect_cb_t    on_reconnect_cb;
} http2_status_cb_t;

/* http2 upload result callback define */
typedef struct {
    http2_upload_id_received_cb_t   upload_id_received_cb;
    http2_upload_completed_cb_t     upload_completed_cb;
} http2_upload_result_cb_t;

/* http2 uploadfile connect api, http2 handle returned */
void *IOT_HTTP2_UploadFile_Connect(http2_upload_conn_info_t *conn_info, http2_status_cb_t *cb);

/* http2 uploadfile start api */
int IOT_HTTP2_UploadFile_Request(void *http2_handle, http2_upload_params_t *params, http2_upload_result_cb_t *cb, void *user_data);

/* http2 uploadfile disconnect api */
int IOT_HTTP2_UploadFile_Disconnect(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _HTTP2_UPLOAD_API_H_ */


