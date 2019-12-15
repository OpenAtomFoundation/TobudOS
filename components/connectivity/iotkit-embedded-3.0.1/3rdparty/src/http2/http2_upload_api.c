/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "infra_config.h"
#ifdef FS_ENABLED

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "infra_defs.h"
#include "infra_types.h"
#include "infra_httpc.h"
#include "infra_sha1.h"
#include "infra_timer.h"
#include "infra_list.h"
#include "infra_log.h"

#include "http2_internal.h"
#include "http2_api.h"
#include "http2_upload_api.h"
#include "http2_config.h"
#include "http2_wrapper.h"

#include "wrappers_defs.h"

#define PACKET_LEN                  16384
#define HTTP2_FS_SERVICE_ID         "c/iot/sys/thing/file/upload"


typedef enum {
    FS_STATUS_WAITING,
    FS_STATUS_UPLOADING,
    FS_STATUS_BREAK,
    FS_STATUS_END,
    FS_STATUS_NUM
} file_stream_status_t;

typedef enum {
    FS_TYPE_NORMAL,
    FS_TYPE_CONTINUE,
    FS_TYPE_OVERRIDE,
} file_stream_type_t;

typedef struct {
    int idx;
    const char *file_path;
    char upload_id[40];
    uint32_t spec_len;
    uint32_t file_offset;
    uint32_t part_len;
    file_stream_type_t type;
    http2_upload_id_received_cb_t opened_cb;
    http2_upload_completed_cb_t end_cb;
    void *user_data;

    uint8_t if_stop;

    http2_list_t list;
} http2_file_stream_t;

typedef struct {
    stream_handle_t    *http2_handle;
    const char         *service_id;
    http2_list_t        file_list;
    void               *list_mutex;
    void               *file_thread;
    int                 upload_idx;
} http2_file_stream_ctx_t;


typedef struct {
    char *send_buffer;
    const char *upload_id;
    uint32_t file_offset;
    uint32_t part_len;
} fs_send_ext_info_t;


static http2_file_stream_ctx_t g_http2_fs_ctx = { 0 };
static http2_stream_cb_t callback_func = { 0 };


/* utils, get file sizef */
static int http2_stream_get_file_size(const char *file_name)
{
    void *fp = NULL;
    int size = 0;
    if ((fp = HAL_Fopen(file_name, "r")) == NULL) {
        h2_err("The file %s can not be opened.\n", file_name);
        return -1;
    }
    HAL_Fseek(fp, 0L, HAL_SEEK_END);
    size = HAL_Ftell(fp);
    HAL_Fclose(fp);
    return size;
}

/* utils, get file name */
static const char *_http2_fs_get_filename(const char *file_path)
{
    const char *p_name = NULL;

    if (file_path == NULL) {
        return NULL;
    }

    p_name = file_path + strlen(file_path);

    while (--p_name != file_path) {
        if (*p_name == '/') {
            p_name++;
            break;
        }
    }

    return p_name;
}

/* utils, read file data */
static int http2_stream_get_file_data(const char *file_name, char *data, int len, int offset)
{
    void *fp = NULL;
    int ret = 0;
    if ((fp = HAL_Fopen(file_name, "r")) == NULL) {
        h2_err("The file %s can not be opened.\n", file_name);
        return -1;
    }
    ret = HAL_Fseek(fp, offset, HAL_SEEK_SET);
    if (ret != 0) {
        HAL_Fclose(fp);
        h2_err("The file %s can not move offset.\n", file_name);
        return -1;
    }
    ret = HAL_Fread(data, len, 1, fp);
    HAL_Fclose(fp);
    return len;
}

/* open http2 file upload channel */
static int _http2_fs_open_channel(http2_file_stream_t *fs_node, stream_data_info_t *info)
{
    stream_handle_t *h2_handle = g_http2_fs_ctx.http2_handle;
    header_ext_info_t ext_header;
    fs_rsp_header_val_t *open_rsp = (fs_rsp_header_val_t *)info->user_data;
    const char *filename = NULL;
    int ret = UPLOAD_ERROR_COMMON;

    /* header for normal upload */
    http2_header header_filename[] = {
        MAKE_HEADER_CS("x-file-name", ""),
    };

    /* header for override operation */
    http2_header header_overwrite[] = {
        MAKE_HEADER_CS("x-file-name", ""),
        MAKE_HEADER_CS("x-file-overwrite", "1"),
    };

    /* header for continue operation */
    http2_header header_uploadid[] = {
        MAKE_HEADER("x-file-upload-id", ""),
    };

    filename = _http2_fs_get_filename(fs_node->file_path);
    h2_info("filename = %s", filename);

    header_filename[0].value = (char *)filename;
    header_filename[0].valuelen = strlen(filename);
    header_overwrite[0].value = (char *)filename;
    header_overwrite[0].valuelen = strlen(filename);

    /* setup http2 ext_header */
    switch (fs_node->type) {
        case FS_TYPE_NORMAL: {
            ext_header.num = 1;
            ext_header.nva = header_filename;

        } break;
        case FS_TYPE_OVERRIDE: {
            ext_header.num = 2;
            ext_header.nva = header_overwrite;

        } break;
        case FS_TYPE_CONTINUE: {
            /* upload id must be exist */
            header_uploadid[0].value = fs_node->upload_id;
            header_uploadid[0].valuelen = strlen(fs_node->upload_id);
            ext_header.num = 1;
            ext_header.nva = header_uploadid;
        } break;
        default: break;
    }

    ret = IOT_HTTP2_Stream_Open(h2_handle, info, &ext_header);
    if (ret < 0 || open_rsp->fs_offset == -1 || open_rsp->fs_upload_id[0] == '\0') {
        h2_err("IOT_HTTP2_Stream_Open failed %d\n", ret);
        return UPLOAD_STREAM_OPEN_FAILED;
    }
    h2_info("fs channel open succeed");
    return SUCCESS_RETURN;
}


/* file part data send sync api */
static int _http2_fs_part_send_sync(http2_file_stream_t *fs_node, stream_data_info_t *info, fs_send_ext_info_t *ext_info)
{
    stream_handle_t *h2_handle = g_http2_fs_ctx.http2_handle;
    header_ext_info_t ext_header;
    int res;
    http2_header header_uploadid[] = {
        MAKE_HEADER_CS("x-file-upload-id", ext_info->upload_id),
    };

    /* setup ext header */
    ext_header.num = 1;
    ext_header.nva = header_uploadid;

    /* setup Stream_Send params */
    info->stream = ext_info->send_buffer;
    info->stream_len = ext_info->part_len;
    info->send_len = 0;
    info->packet_len = FS_UPLOAD_PACKET_LEN;

    while (info->send_len < info->stream_len) {
        if (!h2_handle->init_state) {
            res = UPLOAD_ERROR_COMMON;
            break;
        }

        res = http2_stream_get_file_data(fs_node->file_path, ext_info->send_buffer, FS_UPLOAD_PACKET_LEN, (info->send_len + ext_info->file_offset)); /* offset used */
        if (res <= 0) {
            res = UPLOAD_FILE_READ_FAILED;
            break;
        }
        info->packet_len = res;

        /* adjust the packet len */
        if (info->stream_len - info->send_len < info->packet_len) {
            info->packet_len = info->stream_len - info->send_len;
        }

        res = IOT_HTTP2_Stream_Send(h2_handle, info, &ext_header);
        if (res < 0) {
            res = UPLOAD_STREAM_SEND_FAILED;
            break;
        }
        h2_debug("send len = %d\n", info->send_len);

        if (fs_node->if_stop) {
            res = UPLOAD_STOP_BY_IOCTL;
            break;
        }

        res = SUCCESS_RETURN;
    }

    return res;
}

void *_http2_fs_node_handle(http2_file_stream_t *fs_node)
{
    stream_handle_t *h2_handle = g_http2_fs_ctx.http2_handle;
    int filesize = 0;
    int upload_len = 0;
    fs_rsp_header_val_t rsp_data;
    fs_send_ext_info_t send_ext_info;
    stream_data_info_t channel_info;
    uint32_t part_len = 0;
    int res = FAIL_RETURN;

    /* params check */
    if (h2_handle == NULL) {
        /* TODO: handle */
        return NULL;
    }

    /* get fileszie */
    filesize = http2_stream_get_file_size(fs_node->file_path);
    if (filesize <= 0) {
        if (fs_node->end_cb) {
            fs_node->end_cb(fs_node->file_path, UPLOAD_FILE_NOT_EXIST, fs_node->user_data);
        }

        return NULL;
    }
    h2_info("filesize = %d", filesize);

    /* open http2 file upload channel */
    memset(&rsp_data, 0, sizeof(fs_rsp_header_val_t));
    memset(&channel_info, 0, sizeof(stream_data_info_t));
    channel_info.identify = g_http2_fs_ctx.service_id;
    channel_info.user_data = (void *)&rsp_data;

    res = _http2_fs_open_channel(fs_node, &channel_info);
    if (res < SUCCESS_RETURN) {
        if (fs_node->end_cb) {
            fs_node->end_cb(fs_node->file_path, res, fs_node->user_data);
        }

        return NULL;
    }

    h2_info("upload_id = %s", rsp_data.fs_upload_id);
    h2_info("upload_offset = %d", rsp_data.fs_offset);
    if (fs_node->opened_cb) {
        fs_node->opened_cb(fs_node->file_path, rsp_data.fs_upload_id, fs_node->user_data);
    }

    if (fs_node->spec_len && (fs_node->spec_len + rsp_data.fs_offset < filesize)) {
        upload_len = fs_node->spec_len + rsp_data.fs_offset;
    }
    else {
        upload_len = filesize;
    }

    /* setup send part len */
    if ((fs_node->part_len < (1024 * 100)) || (fs_node->part_len > (1024 * 1024 * 100))) {
        part_len = FS_UPLOAD_PART_LEN;
    }
    else {
        part_len = fs_node->part_len;
    }

    /* send http2 file upload data */
    send_ext_info.upload_id = rsp_data.fs_upload_id;
    send_ext_info.file_offset = rsp_data.fs_offset;
    send_ext_info.send_buffer = HTTP2_STREAM_MALLOC(FS_UPLOAD_PACKET_LEN);
    if (send_ext_info.send_buffer == NULL) {
        if (fs_node->end_cb) {
            fs_node->end_cb(fs_node->file_path, UPLOAD_MALLOC_FAILED, fs_node->user_data);
        }

        return NULL;
    }

    do {
        /* setup the part len */
        send_ext_info.part_len = ((upload_len - send_ext_info.file_offset) < part_len)?
                                (upload_len - send_ext_info.file_offset): part_len;

        res = _http2_fs_part_send_sync(fs_node, &channel_info, &send_ext_info);
        if (res < SUCCESS_RETURN) {
            h2_err("fs send return %d", res);
            break;
        }

        send_ext_info.file_offset += send_ext_info.part_len;
        h2_info("file offset = %d now", send_ext_info.file_offset);
    } while (send_ext_info.file_offset < upload_len);

    if (res < 0) {
        if (fs_node->end_cb) {
            fs_node->end_cb(fs_node->file_path, res, fs_node->user_data);
        }

        HTTP2_STREAM_FREE(channel_info.channel_id);
        HTTP2_STREAM_FREE(send_ext_info.send_buffer);
        return NULL;
    }

    /* close http2 file upload channel */
    IOT_HTTP2_FS_Close(h2_handle, &channel_info, NULL);

    if (fs_node->end_cb) {
        fs_node->end_cb(fs_node->file_path, UPLOAD_SUCCESS, fs_node->user_data);
    }

    HTTP2_STREAM_FREE(send_ext_info.send_buffer);
    return NULL;
}

static void *http_upload_file_func(void *fs_data)
{
    http2_file_stream_ctx_t *fs_ctx = (http2_file_stream_ctx_t *)fs_data;
    http2_file_stream_t *node = NULL;
    if (fs_ctx == NULL) {
        return NULL;
    }

    while (fs_ctx->http2_handle->init_state) {
        HAL_MutexLock(fs_ctx->list_mutex);
        if (!list_empty((list_head_t *)&g_http2_fs_ctx.file_list)) {
            node = list_first_entry(&fs_ctx->file_list, http2_file_stream_t, list);
            HAL_MutexUnlock(fs_ctx->list_mutex);

            /* execute upload routine */
            _http2_fs_node_handle((void *)node);

            /* delete the completed node */
            HAL_MutexLock(fs_ctx->list_mutex);
            list_del((list_head_t *)&node->list);
            HTTP2_STREAM_FREE(node);
            HAL_MutexUnlock(fs_ctx->list_mutex);
        }
        else {
            HAL_MutexUnlock(fs_ctx->list_mutex);
            h2_debug("file list is empty, file upload thread exit\n");
            g_http2_fs_ctx.file_thread = NULL;
            break;
        }
    }

    return NULL;
}

static void _http2_fs_list_insert(http2_file_stream_ctx_t *fs_ctx, http2_file_stream_t *node)
{
    INIT_LIST_HEAD((list_head_t *)&node->list);
    HAL_MutexLock(fs_ctx->list_mutex);
    list_add_tail((list_head_t *)&node->list, (list_head_t *)&fs_ctx->file_list);
    HAL_MutexUnlock(fs_ctx->list_mutex);
}

typedef enum {
    HTTP2_IOCTL_STOP_UPLOAD,
    HTTP2_IOCTL_COMMAND_NUM,
} http2_file_upload_ioctl_command_t;

static int _http2_fs_list_search_by_idx(int idx, http2_file_stream_t **search_node)
{
    http2_file_stream_t *node = NULL;

    HAL_MutexLock(g_http2_fs_ctx.list_mutex);

    list_for_each_entry(node, &g_http2_fs_ctx.file_list, list, http2_file_stream_t) {
        if (idx == node->idx) {
            *search_node = node;
            HAL_MutexUnlock(g_http2_fs_ctx.list_mutex);
            return SUCCESS_RETURN;
        }
    }

    HAL_MutexUnlock(g_http2_fs_ctx.list_mutex);
    *search_node = NULL;
    return FAIL_RETURN;
}

int IOT_HTTP2_Ioctl(int upload_idx, int command, void *data)
{
    http2_file_stream_t *node = NULL;

    if (g_http2_fs_ctx.http2_handle == NULL) {
        return UPLOAD_ERROR_COMMON;
    }

    _http2_fs_list_search_by_idx(upload_idx, &node);
    if (node == NULL) {
        return UPLOAD_ERROR_COMMON;
    }

    switch (command) {
        case HTTP2_IOCTL_STOP_UPLOAD: {
            if (g_http2_fs_ctx.http2_handle) {
                HAL_MutexLock(g_http2_fs_ctx.list_mutex);
                node->if_stop = 1;
                HAL_MutexUnlock(g_http2_fs_ctx.list_mutex);
                return SUCCESS_RETURN;
            }
            else {
                return UPLOAD_ERROR_COMMON;
            }
        } break;
        default: {
            return UPLOAD_ERROR_COMMON;
        }
    }

    return SUCCESS_RETURN;
}

void *IOT_HTTP2_UploadFile_Connect(http2_upload_conn_info_t *conn_info, http2_status_cb_t *cb)
{
    void *handle;

    memset(&callback_func, 0, sizeof(http2_stream_cb_t));

    if (cb != NULL) {
        callback_func.on_reconnect_cb = cb->on_reconnect_cb;
        callback_func.on_disconnect_cb = cb->on_disconnect_cb;
    }

    handle = IOT_HTTP2_Connect((device_conn_info_t *)conn_info, &callback_func);
    if (handle == NULL) {
        return handle;
    }

    /* TODO */
    g_http2_fs_ctx.list_mutex = HAL_MutexCreate();
    if (g_http2_fs_ctx.list_mutex == NULL) {
        h2_err("fs mutex create error\n");
        IOT_HTTP2_UploadFile_Disconnect(handle);
        return NULL;
    }

    INIT_LIST_HEAD((list_head_t *)&(g_http2_fs_ctx.file_list));
    g_http2_fs_ctx.http2_handle = handle;
    g_http2_fs_ctx.service_id = HTTP2_FS_SERVICE_ID;

    return handle;
}

int IOT_HTTP2_UploadFile_Request(void *http2_handle, http2_upload_params_t *params, http2_upload_result_cb_t *cb, void *user_data)
{
    int ret;
    http2_file_stream_t *file_node = NULL;

    if (http2_handle == NULL || params == NULL || cb == NULL) {
        return NULL_VALUE_ERROR;
    }

    if (params->file_path == NULL) {
        return UPLOAD_FILE_PATH_IS_NULL;
    }

    if ( (params->opt_bit_map & UPLOAD_FILE_OPT_BIT_RESUME) && params->upload_id == NULL) {
        return UPLOAD_ID_IS_NULL;
    }

    if ( (params->opt_bit_map & UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN) && params->upload_len == 0) {
        return UPLOAD_LEN_IS_ZERO;
    }

    file_node = (http2_file_stream_t *)HTTP2_STREAM_MALLOC(sizeof(http2_file_stream_t));
    if (file_node == NULL) {
        return UPLOAD_MALLOC_FAILED;
    }

    memset(file_node, 0, sizeof(http2_file_stream_t));
    file_node->file_path = params->file_path;
    file_node->part_len = params->part_len;
    file_node->opened_cb = cb->upload_id_received_cb;
    file_node->end_cb = cb->upload_completed_cb;
    file_node->user_data = user_data;
    file_node->type = FS_TYPE_NORMAL;
    file_node->idx = g_http2_fs_ctx.upload_idx++;

    if (params->opt_bit_map & UPLOAD_FILE_OPT_BIT_SPECIFIC_LEN) {
        file_node->spec_len = params->upload_len;
    }
    if (params->opt_bit_map & UPLOAD_FILE_OPT_BIT_OVERWRITE) {
        file_node->type = FS_TYPE_OVERRIDE;
    }
    else if (params->opt_bit_map & UPLOAD_FILE_OPT_BIT_RESUME) {
        file_node->type = FS_TYPE_CONTINUE;
        memcpy(file_node->upload_id, params->upload_id, sizeof(file_node->upload_id));
    }

    /* inset http2_fs node */
    _http2_fs_list_insert(&g_http2_fs_ctx, file_node);

    if (g_http2_fs_ctx.file_thread == NULL) {
        hal_os_thread_param_t thread_parms = {0};
        thread_parms.stack_size = 6144;
        thread_parms.name = "file_upload";
        ret = HAL_ThreadCreate(&g_http2_fs_ctx.file_thread, http_upload_file_func, (void *)&g_http2_fs_ctx, &thread_parms, NULL);
        if (ret != 0) {
            h2_err("file upload thread create error\n");
            return -1;
        }
        HAL_ThreadDetach(g_http2_fs_ctx.file_thread);
    }

    return SUCCESS_RETURN;
}

int IOT_HTTP2_UploadFile_Disconnect(void *handle)
{
    int res = FAIL_RETURN;

    res =  IOT_HTTP2_Disconnect(handle);

    if (g_http2_fs_ctx.list_mutex == NULL) {
        memset(&g_http2_fs_ctx, 0, sizeof(g_http2_fs_ctx));
    }
    else {
        http2_file_stream_t *node, *next;
        HAL_MutexLock(g_http2_fs_ctx.list_mutex);
        list_for_each_entry_safe(node, next, &g_http2_fs_ctx.file_list, list, http2_file_stream_t) {
            list_del((list_head_t *)&node->list);
            HTTP2_STREAM_FREE(node);
            break;
        }
        HAL_MutexUnlock(g_http2_fs_ctx.list_mutex);

        HAL_MutexDestroy(g_http2_fs_ctx.list_mutex);
        memset(&g_http2_fs_ctx, 0, sizeof(g_http2_fs_ctx));
    }

    return res;
}

#endif /* #ifdef FS_ENABLED */
