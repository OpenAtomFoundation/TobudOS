/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

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
#include "http2_config.h"
#include "http2_wrapper.h"

#include "wrappers_defs.h"

#define URL_MAX_LEN                     (100)

typedef enum {
    NUM_STRING_ENUM      = 0,
    PATH_CREATE_STR_ENUM = 1,
    PATH_UPLOAD_STR_ENUM = 2,
    CID_STRING_ENUM      = 3,
    ORI_SIGN_STR_ENUM    = 4,
    FS_STRING_ENUM       = 5,
    REAL_SIGN_STR_ENUM   = 6,
} HEADER_TYPE_ENUM;


typedef struct _device_info_struct_ {
    char        product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char        device_name[IOTX_DEVICE_NAME_LEN + 1];
    char        device_secret[IOTX_DEVICE_SECRET_LEN + 1];
    char        url[URL_MAX_LEN + 1];
    int         port;
} device_info;

typedef struct {
    unsigned int stream_id;         /* http2 protocol stream id */
    char *channel_id;               /* string return by server to identify a specific stream channel, different from stream identifier which is a field in http2 frame */
    stream_type_t stream_type;      /* check @stream_type_t */
    void *semaphore;                /* semaphore for http2 response sync */
    char status_code[5];            /* http2 response status code */
    uint8_t  rcv_hd_cnt;            /* the number of concerned heads received*/
    void     *user_data;            /* data passed to the stream callback function */
    http2_list_t list;              /* list_head */
} http2_stream_node_t;

static device_info g_device_info;

static stream_handle_t *g_stream_handle = NULL;
static httpclient_t g_client;

static int _set_device_info(device_conn_info_t *device_info)
{
    memset(g_device_info.product_key, 0, IOTX_PRODUCT_KEY_LEN + 1);
    memset(g_device_info.device_name, 0, IOTX_DEVICE_NAME_LEN + 1);
    memset(g_device_info.device_secret, 0, IOTX_DEVICE_SECRET_LEN + 1);
    memset(g_device_info.url, 0, URL_MAX_LEN + 1);

    strncpy(g_device_info.product_key, device_info->product_key, IOTX_PRODUCT_KEY_LEN);
    strncpy(g_device_info.device_name, device_info->device_name, IOTX_DEVICE_NAME_LEN);
    strncpy(g_device_info.device_secret, device_info->device_secret, IOTX_DEVICE_SECRET_LEN);
    if (device_info->url != NULL) {
        strncpy(g_device_info.url, device_info->url, URL_MAX_LEN);
    }
    g_device_info.port = device_info->port;

    return 0;
}

static int http2_nv_copy(http2_header *nva, int start, http2_header *nva_copy, int num)
{
    int i, j;
    for (i = start, j = 0; j < num; i++, j++) {
        nva[i].name = nva_copy[j].name;
        nva[i].value = nva_copy[j].value;
        nva[i].namelen = nva_copy[j].namelen;
        nva[i].valuelen = nva_copy[j].valuelen;
    }
    return i;
}

static int iotx_http2_get_url(char *buf, char *productkey)
{
    if (strlen(g_device_info.url) != 0) {
        strncpy(buf, g_device_info.url, URL_MAX_LEN);
        return g_device_info.port;
    }
#if defined(ON_DAILY)
    sprintf(buf, "%s", "10.101.12.205");
    return 9999;
#elif defined(ON_PRE)
    sprintf(buf, "%s", "100.67.141.158");
    return 8443;
#else
    sprintf(buf, "%s", productkey);
    strcat(buf, ".iot-as-http2.cn-shanghai.aliyuncs.com");
    return 443;
#endif
}

static void file_upload_gen_string(char *str, int type, char *para1, int para2)
{
    switch (type) {
        case NUM_STRING_ENUM: {
            sprintf(str, "%d", para2);
            break;
        }
        case PATH_CREATE_STR_ENUM:
        case PATH_UPLOAD_STR_ENUM:
        case ORI_SIGN_STR_ENUM:
        case CID_STRING_ENUM: {
            if (type == PATH_CREATE_STR_ENUM) {
                sprintf(str, "/message/pub_with_resp/sys/%s/%s/thing/%s/create",
                        g_device_info.product_key,
                        g_device_info.device_name,
                        para1);
            } else if (type == PATH_UPLOAD_STR_ENUM) {
                sprintf(str, "/message/pub_with_resp/sys/%s/%s/thing/%s/upload",
                        g_device_info.product_key,
                        g_device_info.device_name,
                        para1);
            } else if (type == ORI_SIGN_STR_ENUM) {
                sprintf(str, "clientId%sdeviceName%sproductKey%s",
                        para1,
                        g_device_info.device_name,
                        g_device_info.product_key);
            } else {
                sprintf(str, "%s.%s", g_device_info.product_key, g_device_info.device_name);
            }
            break;
        }
        case REAL_SIGN_STR_ENUM: {
            utils_hmac_sha1(para1, strlen(para1), str, g_device_info.device_secret, strlen(g_device_info.device_secret));
            break;
        }
        default: {
            h2_err("ASSERT\n");
            break;
        }
    }
}

static int http2_stream_node_search(stream_handle_t *handle, unsigned int stream_id, http2_stream_node_t **p_node)
{
    http2_stream_node_t *search_node = NULL;
    *p_node = NULL;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(p_node, NULL_VALUE_ERROR);

    list_for_each_entry(search_node, &handle->stream_list, list, http2_stream_node_t) {
        if (search_node->stream_id == stream_id) {
            *p_node = search_node;
            return SUCCESS_RETURN;
        }
    }

    h2_debug("stream node not exist, stream_id = %d", stream_id);
    return FAIL_RETURN;
}

static void on_stream_header(int32_t stream_id, int cat, const uint8_t *name, uint32_t namelen,
                             const uint8_t *value, uint32_t valuelen, uint8_t flags)
{
    http2_stream_node_t *node = NULL;
    char *channel_id = NULL;
    char *user_data = NULL;

    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node != NULL) {
        channel_id = node->channel_id;
        user_data = node->user_data;

        switch (cat) {
            case 0x01:
                if (strncmp((char *)name, "x-data-stream-id", (int)namelen) == 0) {
                    node->channel_id = HTTP2_STREAM_MALLOC(valuelen + 1);
                    if (node->channel_id == NULL) {
                        return;
                    }
                    memset(node->channel_id, 0, (int)valuelen + 1);
                    memcpy(node->channel_id, (char *)value, (int)valuelen);
                    if (++node->rcv_hd_cnt == 2) {
                        HAL_SemaphorePost(node->semaphore);
                    }
                }
#ifdef FS_ENABLED
                else if (strncmp((char *)name, "x-file-upload-id", (int)namelen) == 0) {
                    fs_rsp_header_val_t *rsp_data = (fs_rsp_header_val_t *)user_data;
                    memcpy(rsp_data->fs_upload_id, value, valuelen);
                }
                else if (strncmp((char *)name, "x-next-append-position", (int)namelen) == 0) {
                    fs_rsp_header_val_t *rsp_data = (fs_rsp_header_val_t *)user_data;
                    rsp_data->fs_offset = atoi((char *)value);
                }
                else if (strncmp((char *)name, "x-response-status", (int)namelen) == 0) {
                    strncpy(node->status_code, (char *)value, sizeof(node->status_code) - 1);
                    if (++node->rcv_hd_cnt == 2) {
                        HAL_SemaphorePost(node->semaphore);
                    }
                }
#else
                else if (strncmp((char *)name, ":status", (int)namelen) == 0) {
                    strncpy(node->status_code, (char *)value, sizeof(node->status_code) - 1);
                    if (++node->rcv_hd_cnt == 2) {
                        HAL_SemaphorePost(node->semaphore);
                    }
                }
#endif
        }
    }

    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_header_cb) {
        g_stream_handle->cbs->on_stream_header_cb(stream_id, channel_id, cat, name, namelen, value, valuelen,
                flags, user_data);
    }
}

static void on_stream_chunk_recv(int32_t stream_id, const uint8_t *data, uint32_t len, uint8_t flags)
{
    http2_stream_node_t *node;
    char *channel_id = NULL;
    char *user_data = NULL;

    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node != NULL) {
        channel_id = node->channel_id;
        user_data = node->user_data;
    }

    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_chunk_recv_cb) {
        g_stream_handle->cbs->on_stream_chunk_recv_cb(stream_id, channel_id, data, len, flags, user_data);
    }
}

static void on_stream_close(int32_t stream_id, uint32_t error_code)
{
    http2_stream_node_t *node;
    char *channel_id = NULL;
    char *user_data = NULL;

    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node != NULL) {
        channel_id = node->channel_id;
        user_data = node->user_data;
    }
    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_close_cb) {
        g_stream_handle->cbs->on_stream_close_cb(stream_id, channel_id, error_code, user_data);
    }
}

static void on_stream_frame_send(int32_t stream_id, int type, uint8_t flags)
{
    http2_stream_node_t *node;
    char *channel_id = NULL;
    char *user_data = NULL;

    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node != NULL) {
        channel_id = node->channel_id;
        user_data = node->user_data;
    }
    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_frame_send_cb) {
        g_stream_handle->cbs->on_stream_frame_send_cb(stream_id, channel_id, type, flags, user_data);
    }
}

static void on_stream_frame_recv(int32_t stream_id, int type, uint8_t flags)
{
    http2_stream_node_t *node;
    char *channel_id = NULL;
    char *user_data = NULL;
    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node != NULL) {
        channel_id = node->channel_id;
        user_data = node->user_data;
    }

    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_frame_recv_cb) {
        g_stream_handle->cbs->on_stream_frame_recv_cb(stream_id, channel_id, type, flags, user_data);
    }
}

static http2_user_cb_t my_cb = {
    .on_user_header_cb = on_stream_header,
    .on_user_chunk_recv_cb = on_stream_chunk_recv,
    .on_user_stream_close_cb = on_stream_close,
    .on_user_frame_send_cb = on_stream_frame_send,
    .on_user_frame_recv_cb = on_stream_frame_recv,
};

static int reconnect(stream_handle_t *handle)
{
    char buf[100] = {0};
    http2_connection_t *conn = NULL;
    int port = 0;

    iotx_http2_client_disconnect(handle->http2_connect);
    handle->http2_connect = NULL;
    port = iotx_http2_get_url(buf, g_device_info.product_key);
    conn = iotx_http2_client_connect_with_cb((void *)&g_client, buf, port, &my_cb);
    if (conn == NULL) {
        return -1;
    }
    handle->http2_connect = conn;
    return 0;
}

static void *http2_io(void *user_data)
{
    stream_handle_t *handle = (stream_handle_t *)user_data;
    int rv = 0;
    iotx_time_t timer, timer_rsp;
    static uint8_t timer_valid = 0;
    POINTER_SANITY_CHECK(handle, NULL);
    iotx_time_init(&timer);
    iotx_time_init(&timer_rsp);
    while (handle->init_state) {
        if (handle->connect_state) {
            HAL_MutexLock(handle->mutex);
            rv = iotx_http2_exec_io(handle->http2_connect);
            HAL_MutexUnlock(handle->mutex);
        }
        if (utils_time_is_expired(&timer)) {
            HAL_MutexLock(handle->mutex);
            rv = iotx_http2_client_send_ping(handle->http2_connect);
            HAL_MutexUnlock(handle->mutex);
            utils_time_countdown_ms(&timer, IOT_HTTP2_KEEP_ALIVE_TIME);
            if (rv >= 0) {
                utils_time_countdown_ms(&timer_rsp, 3000);
                timer_valid = 1;
            }
        }

        if (timer_valid && utils_time_is_expired(&timer_rsp)) {
            timer_valid = 0;
            rv = iotx_http2_client_recv_ping();
        }

        if (rv < 0) {
            if (handle->retry_cnt == IOT_HTTP2_KEEP_ALIVE_CNT - 1) {
                h2_info("rv =%d, try reconnect\n", rv);
                if (handle->connect_state != 0) {
                    handle->connect_state = 0;
                    if (handle->cbs && handle->cbs->on_disconnect_cb) {
                        handle->cbs->on_disconnect_cb();
                    }
                }
                rv = reconnect(handle);
                continue;
            } else {
                handle->retry_cnt++;
            }
        } else {
            if (handle->connect_state == 0) {
                handle->connect_state = 1;
                handle->retry_cnt = 0;
                if (handle->cbs && handle->cbs->on_reconnect_cb) {
                    handle->cbs->on_reconnect_cb();
                }
            }
        }
        HAL_SleepMs(100);
    }
    HAL_SemaphorePost(handle->semaphore);

    return NULL;
}

static int http2_stream_node_insert(stream_handle_t *handle, unsigned int id, void *user_data,
                                    http2_stream_node_t **p_node)
{
    http2_stream_node_t *node = NULL;
    void *semaphore = NULL;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);

    ARGUMENT_SANITY_CHECK(id != 0, FAIL_RETURN);

    if (p_node != NULL) {
        *p_node = NULL;
    }

    node = (http2_stream_node_t *)HTTP2_STREAM_MALLOC(sizeof(http2_stream_node_t));
    if (node == NULL) {
        return FAIL_RETURN;
    }

    memset(node, 0, sizeof(http2_stream_node_t));
    node->stream_id = id;
    node->user_data = user_data;
    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        HTTP2_STREAM_FREE(node);
        return FAIL_RETURN;
    }
    node->semaphore = semaphore;

    INIT_LIST_HEAD((list_head_t *)&node->list);
    list_add((list_head_t *)&node->list, (list_head_t *)&handle->stream_list);

    if (p_node != NULL) {
        *p_node = node;
    }

    return SUCCESS_RETURN;
}

static int http2_stream_node_remove(stream_handle_t *handle, unsigned int id)
{
    http2_stream_node_t *search_node;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    ARGUMENT_SANITY_CHECK(id != 0, FAIL_RETURN);

    list_for_each_entry(search_node, &handle->stream_list, list, http2_stream_node_t) {
        if (id == search_node->stream_id) {
            h2_info("stream_node found, delete\n");

            list_del((list_head_t *)&search_node->list);
            HTTP2_STREAM_FREE(search_node->channel_id);
            HAL_SemaphoreDestroy(search_node->semaphore);
            HTTP2_STREAM_FREE(search_node);
            return SUCCESS_RETURN;
        }
    }
    return FAIL_RETURN;
}

static int get_version_int()
{
    const char *p_version = IOTX_SDK_VERSION;
    int v_int = 0;

    while (*p_version != 0) {
        if (*p_version <= '9' && *p_version >= '0') {
            v_int = v_int * 10 + *p_version - '0';
        }
        p_version ++;
    }
    return v_int;
}

void *IOT_HTTP2_Connect(device_conn_info_t *conn_info, http2_stream_cb_t *user_cb)
{
    stream_handle_t *stream_handle = NULL;
    http2_connection_t *conn = NULL;
    hal_os_thread_param_t thread_parms = {0};
    char buf[URL_MAX_LEN + 1] = {0};
    int port = 0;
    int ret = 0;

    POINTER_SANITY_CHECK(conn_info, NULL);
    POINTER_SANITY_CHECK(conn_info->product_key, NULL);
    POINTER_SANITY_CHECK(conn_info->device_name, NULL);
    POINTER_SANITY_CHECK(conn_info->device_secret, NULL);

    memset(&g_client, 0, sizeof(httpclient_t));

    stream_handle = HTTP2_STREAM_MALLOC(sizeof(stream_handle_t));
    if (stream_handle == NULL) {
        return NULL;
    }

    memset(stream_handle, 0, sizeof(stream_handle_t));
    stream_handle->mutex = HAL_MutexCreate();
    if (stream_handle->mutex == NULL) {
        HTTP2_STREAM_FREE(stream_handle);
        h2_err("mutex create error\n");
        return NULL;
    }
    stream_handle->semaphore = HAL_SemaphoreCreate();
    if (stream_handle->semaphore == NULL) {
        h2_err("semaphore create error\n");
        HAL_MutexDestroy(stream_handle->mutex);
        HTTP2_STREAM_FREE(stream_handle);
        return NULL;
    }

    INIT_LIST_HEAD((list_head_t *) & (stream_handle->stream_list));

    _set_device_info(conn_info);
    g_stream_handle = stream_handle;
    g_stream_handle->cbs = user_cb;

    port = iotx_http2_get_url(buf, conn_info->product_key);
    conn = iotx_http2_client_connect_with_cb((void *)&g_client, buf, port, &my_cb);
    if (conn == NULL) {
        HAL_MutexDestroy(stream_handle->mutex);
        HAL_SemaphoreDestroy(stream_handle->semaphore);
        HTTP2_STREAM_FREE(stream_handle);
        return NULL;
    }
    stream_handle->http2_connect = conn;
    stream_handle->init_state = 1;

    thread_parms.stack_size = 6144;
    thread_parms.name = "http2_io";
    ret = HAL_ThreadCreate(&stream_handle->rw_thread, http2_io, stream_handle, &thread_parms, NULL);
    if (ret != 0) {
        h2_err("thread create error\n");
        IOT_HTTP2_Disconnect(stream_handle);
        return NULL;
    }
    HAL_ThreadDetach(stream_handle->rw_thread);

    return stream_handle;
}

int IOT_HTTP2_Stream_Open(void *hd, stream_data_info_t *info, header_ext_info_t *header)
{
    char client_id[64 + 1] = {0};
    char sign_str[256 + 1] = {0};
    char sign[41 + 1] = {0};
    char path[128] = {0};
    char version[33] = {0};
    int header_count = 0;
    int header_num;
    int rv = 0;
    http2_data h2_data;
    http2_stream_node_t *node = NULL;
    stream_handle_t *handle = (stream_handle_t *)hd;
    http2_header *nva = NULL;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->identify, NULL_VALUE_ERROR);


    memset(&h2_data, 0, sizeof(http2_data));

    HAL_Snprintf(path, sizeof(path), "/stream/open/%s", info->identify);

    file_upload_gen_string(client_id, CID_STRING_ENUM, NULL, 0);
    file_upload_gen_string(sign_str, ORI_SIGN_STR_ENUM, client_id, 0);
    file_upload_gen_string(sign, REAL_SIGN_STR_ENUM, sign_str, 0);

    HAL_Snprintf(version, sizeof(version), "%d", get_version_int());

    {
        const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                            MAKE_HEADER_CS(":path", path),
                                            MAKE_HEADER(":scheme", "https"),
                                            MAKE_HEADER("x-auth-name", "devicename"),
                                            MAKE_HEADER_CS("x-auth-param-client-id", client_id),
                                            MAKE_HEADER("x-auth-param-signmethod", "hmacsha1"),
                                            MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                            MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                            MAKE_HEADER_CS("x-auth-param-sign", sign),
                                            MAKE_HEADER_CS("x-sdk-version", version),
                                            MAKE_HEADER_CS("x-sdk-version-name", IOTX_SDK_VERSION),
                                            MAKE_HEADER("x-sdk-platform", "c"),
                                            MAKE_HEADER("content-length", "0"),
                                            };

        header_num = sizeof(static_header) / sizeof(static_header[0]);
        if (header != NULL) {
            header_num += header->num;
        }
        nva = (http2_header *)HTTP2_STREAM_MALLOC(sizeof(http2_header) * header_num);
        if (nva == NULL) {
            h2_err("nva malloc failed\n");
            return FAIL_RETURN;
        }

        /* add external header if it's not NULL */
        header_count = http2_nv_copy(nva, 0, (http2_header *)static_header, sizeof(static_header) / sizeof(static_header[0]));
        if (header != NULL) {
            header_count = http2_nv_copy(nva, header_count, (http2_header *)header->nva, header->num);
        }

        h2_data.header = (http2_header *)nva;
        h2_data.header_count = header_count;
        h2_data.data = NULL;
        h2_data.len = 0;
        h2_data.flag = 1 ;
        h2_data.stream_id = 0;

        HAL_MutexLock(handle->mutex);
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
        http2_stream_node_insert(handle, h2_data.stream_id, info->user_data, &node);
        HTTP2_STREAM_FREE(nva);
    }

    if (rv < 0) {
        h2_err("client send error\n");
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }

    if (node == NULL) {
        h2_err("node insert failed!");
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }

    node->stream_type = STREAM_TYPE_AUXILIARY;
    HAL_MutexUnlock(handle->mutex);

    rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
    if (rv < 0 || memcmp(node->status_code, "200", 3)) {
        h2_err("semaphore wait overtime or status code error\n");
        HAL_MutexLock(handle->mutex);
        http2_stream_node_remove(handle, node->stream_id);
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }
    info->channel_id = HTTP2_STREAM_MALLOC(strlen(node->channel_id) + 1);
    if (info->channel_id == NULL) {
        h2_err("channel_id malloc failed\n");
        HAL_MutexLock(handle->mutex);
        http2_stream_node_remove(handle, node->stream_id);
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }
    memset(info->channel_id, 0, strlen(node->channel_id) + 1);
    strcpy(info->channel_id, node->channel_id);

    return SUCCESS_RETURN;
}

int IOT_HTTP2_Stream_Send_Message(void *hd, const char *identify,char *channel_id, char *data, uint32_t data_len, header_ext_info_t *header)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};
    char data_len_str[33] = {0};
    int windows_size;
    int count = 0;
    stream_handle_t *handle = (stream_handle_t *)hd;
    http2_header *nva = NULL;
    int header_count, header_num;
    char version[33] = {0};
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(identify, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(channel_id, NULL_VALUE_ERROR);

    windows_size = iotx_http2_get_available_window_size(handle->http2_connect);
    while (windows_size < data_len) {
        h2_warning("windows_size < info->packet_len ,wait ...\n");
        HAL_SleepMs(100);
        if (++count > 50) {
            return FAIL_RETURN;
        }
        windows_size = iotx_http2_get_available_window_size(handle->http2_connect);
    }

    HAL_Snprintf(data_len_str, sizeof(data_len_str), "%d", data_len);
    HAL_Snprintf(path, sizeof(path), "/stream/send/%s", identify);
    HAL_Snprintf(version, sizeof(version), "%d", get_version_int());

    {
        const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                                MAKE_HEADER_CS(":path", path),
                                                MAKE_HEADER(":scheme", "https"),
                                                MAKE_HEADER_CS("content-length", data_len_str),
                                                MAKE_HEADER_CS("x-data-stream-id", channel_id),
                                                MAKE_HEADER_CS("x-sdk-version", version),
                                                MAKE_HEADER_CS("x-sdk-version-name", IOTX_SDK_VERSION),
                                                MAKE_HEADER("x-sdk-platform", "c"),
                                                };

        header_num = sizeof(static_header) / sizeof(static_header[0]);
        if (header != NULL) {
            header_num += header->num;
        }
        nva = (http2_header *)HTTP2_STREAM_MALLOC(sizeof(http2_header) * header_num);
        if (nva == NULL) {
            h2_err("nva malloc failed\n");
            return FAIL_RETURN;
        }

        /* add external header if it's not NULL */
        header_count = http2_nv_copy(nva, 0, (http2_header *)static_header, sizeof(static_header) / sizeof(static_header[0]));
        if (header != NULL) {
            header_count = http2_nv_copy(nva, header_count, (http2_header *)header->nva, header->num);
        }
        memset(&h2_data, 0, sizeof(h2_data));
        h2_data.header = (http2_header *)nva;
        h2_data.header_count = header_count;
        h2_data.data = data;
        h2_data.len = data_len;
        h2_data.flag = 1;

        HAL_MutexLock(handle->mutex);
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
        HAL_MutexUnlock(handle->mutex);
        HTTP2_STREAM_FREE(nva);
    }

    if (rv < 0) {
        h2_err("send failed!");
        return rv;
    }

    return h2_data.stream_id;
}

int IOT_HTTP2_Stream_Send(void *hd, stream_data_info_t *info, header_ext_info_t *header)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};
    char data_len_str[33] = {0};
    int windows_size;
    int count = 0;
    http2_stream_node_t *node = NULL;
    stream_handle_t *handle = (stream_handle_t *)hd;
    http2_header *nva = NULL;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->stream, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->channel_id, NULL_VALUE_ERROR);
    ARGUMENT_SANITY_CHECK(info->stream_len != 0, FAIL_RETURN);
    ARGUMENT_SANITY_CHECK(info->packet_len != 0, FAIL_RETURN);

    windows_size = iotx_http2_get_available_window_size(handle->http2_connect);
    while (windows_size < info->packet_len) {
        h2_warning("windows_size < info->packet_len ,wait ...\n");
        HAL_SleepMs(100);
        if (++count > 50) {
            return FAIL_RETURN;
        }
        windows_size = iotx_http2_get_available_window_size(handle->http2_connect);
    }

    HAL_Snprintf(data_len_str, sizeof(data_len_str), "%d", info->stream_len);
    HAL_Snprintf(path, sizeof(path), "/stream/send/%s", info->identify);
    if (info->send_len == 0) { /* first send,need header */
        int header_count, header_num;
        char version[33] = {0};
        HAL_Snprintf(version, sizeof(version), "%d", get_version_int());
        {
            const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                                MAKE_HEADER_CS(":path", path),
                                                MAKE_HEADER(":scheme", "https"),
                                                MAKE_HEADER_CS("content-length", data_len_str),
                                                MAKE_HEADER_CS("x-data-stream-id", info->channel_id),
                                                MAKE_HEADER_CS("x-sdk-version", version),
                                                MAKE_HEADER_CS("x-sdk-version-name", IOTX_SDK_VERSION),
                                                MAKE_HEADER("x-sdk-platform", "c"),
                                                };

            header_num = sizeof(static_header) / sizeof(static_header[0]);
            if (header != NULL) {
                header_num += header->num;
            }
            nva = (http2_header *)HTTP2_STREAM_MALLOC(sizeof(http2_header) * header_num);
            if (nva == NULL) {
                h2_err("nva malloc failed\n");
                return FAIL_RETURN;
            }

            /* add external header if it's not NULL */
            header_count = http2_nv_copy(nva, 0, (http2_header *)static_header, sizeof(static_header) / sizeof(static_header[0]));
            if (header != NULL) {
                header_count = http2_nv_copy(nva, header_count, (http2_header *)header->nva, header->num);
            }
            memset(&h2_data, 0, sizeof(h2_data));
            h2_data.header = (http2_header *)nva;
            h2_data.header_count = header_count;
            h2_data.data = info->stream;
            h2_data.len = info->packet_len; /* TODO */

            if (info->packet_len + info->send_len == info->stream_len) { /* last frame */
                h2_data.flag = 1;
            } else {
                h2_data.flag = 0;
            }

            HAL_MutexLock(handle->mutex);
            rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
            http2_stream_node_insert(handle, h2_data.stream_id, info->user_data, &node);
            HTTP2_STREAM_FREE(nva);
        }

        if (rv < 0) {
            h2_err("send failed!");
            HAL_MutexUnlock(handle->mutex);
            return FAIL_RETURN;
        }

        if (node == NULL) {
            h2_err("node insert failed!");
            HAL_MutexUnlock(handle->mutex);
            return FAIL_RETURN;
        }

        node->stream_type = STREAM_TYPE_UPLOAD;
        HAL_MutexUnlock(handle->mutex);

        info->h2_stream_id = h2_data.stream_id;
        info->send_len += info->packet_len;
    } else {
        h2_data.header = NULL;
        h2_data.header_count = 0;
        h2_data.data = info->stream;
        h2_data.len = info->packet_len;

        h2_data.stream_id = info->h2_stream_id;
        if (info->packet_len + info->send_len == info->stream_len) { /* last frame */
            h2_data.flag = 1;
        } else {
            h2_data.flag = 0;
        }

        HAL_MutexLock(handle->mutex);
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
        HAL_MutexUnlock(handle->mutex);
        if (rv < 0) {
            return FAIL_RETURN;
        }
        info->send_len += info->packet_len;
    }

    if (h2_data.flag == 1) {
        http2_stream_node_t *node = NULL;
        HAL_MutexLock(handle->mutex);
        http2_stream_node_search(handle, h2_data.stream_id, &node);
        HAL_MutexUnlock(handle->mutex);
        if (node == NULL) {
            h2_err("node search failed!");
            return FAIL_RETURN;
        }
        rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
        if (rv < 0 || memcmp(node->status_code, "200", 3)) {
            h2_err("semaphore wait overtime or status code error,h2_data.stream_id %d\n", h2_data.stream_id);
            HAL_MutexLock(handle->mutex);
            http2_stream_node_remove(handle, node->stream_id);
            HAL_MutexUnlock(handle->mutex);
            return FAIL_RETURN;
        }
    }

    return rv;
}

int IOT_HTTP2_Stream_Query(void *hd, stream_data_info_t *info, header_ext_info_t *header)
{
    int rv = 0;
    http2_data h2_data;
    http2_stream_node_t *node = NULL;
    char path[128] = {0};
    int header_count, header_num;
    stream_handle_t *handle = (stream_handle_t *)hd;
    http2_header *nva = NULL;
    char version[33] = {0};

    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->channel_id, NULL_VALUE_ERROR);
    HAL_Snprintf(version, sizeof(version), "%d", get_version_int());
    HAL_Snprintf(path, sizeof(path), "/stream/send/%s", info->identify);
    {
        const http2_header static_header[] = { MAKE_HEADER(":method", "GET"),
                                            MAKE_HEADER_CS(":path", path),
                                            MAKE_HEADER(":scheme", "https"),
                                            MAKE_HEADER_CS("x-data-stream-id", info->channel_id),
                                            MAKE_HEADER("x-test-downstream", "1"),
                                            MAKE_HEADER_CS("x-sdk-version", version),
                                            MAKE_HEADER_CS("x-sdk-version-name", IOTX_SDK_VERSION),
                                            MAKE_HEADER("x-sdk-platform", "c"),
                                            };

        header_num = sizeof(static_header) / sizeof(static_header[0]);
        if (header != NULL) {
            header_num += header->num;
        }
        nva = (http2_header *)HTTP2_STREAM_MALLOC(sizeof(http2_header) * header_num);
        if (nva == NULL) {
            h2_err("nva malloc failed\n");
            return FAIL_RETURN;
        }

        /* add external header if it's not NULL */
        header_count = http2_nv_copy(nva, 0, (http2_header *)static_header, sizeof(static_header) / sizeof(static_header[0]));
        if (header != NULL) {
            header_count = http2_nv_copy(nva, header_count, (http2_header *)header->nva, header->num);
        }
        h2_data.header = (http2_header *)nva;
        h2_data.header_count = header_count;
        h2_data.data = NULL;
        h2_data.len = 0;
        h2_data.flag = 1;
        h2_data.stream_id = 0;

        HAL_MutexLock(handle->mutex);
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
        http2_stream_node_insert(handle, h2_data.stream_id, info->user_data, &node);
        HTTP2_STREAM_FREE(nva);
    }

    if (rv < 0) {
        h2_err("client send error\n");
        HAL_MutexUnlock(handle->mutex);
        return rv;
    }

    if (node == NULL) {
        h2_err("node insert failed!");
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }

    node->stream_type = STREAM_TYPE_DOWNLOAD;
    HAL_MutexUnlock(handle->mutex);

    rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
    if (rv < 0 || memcmp(node->status_code, "200", 3)) {
        h2_err("semaphore wait overtime or status code error\n");
        HAL_MutexLock(handle->mutex);
        http2_stream_node_remove(handle, node->stream_id);
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }

    return rv;
}

#ifdef FS_ENABLED
int IOT_HTTP2_FS_Close(void *hd, stream_data_info_t *info, header_ext_info_t *header)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};
    stream_handle_t *handle = (stream_handle_t *)hd;
    char version[33] = {0};
    char *stream_id = info->channel_id;
    int len = strlen(stream_id);
    http2_stream_node_t *node, *next;
    http2_header *nva = NULL;
    int header_count, header_num;

    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->channel_id, NULL_VALUE_ERROR);
    HAL_Snprintf(version, sizeof(version), "%d", get_version_int());
    HAL_Snprintf(path, sizeof(path), "/stream/close/%s", info->identify);
    {
        const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                            MAKE_HEADER_CS(":path", path),
                                            MAKE_HEADER(":scheme", "https"),
                                            MAKE_HEADER_CS("x-data-stream-id", info->channel_id),
                                            MAKE_HEADER_CS("x-sdk-version", version),
                                            MAKE_HEADER_CS("x-sdk-version-name", IOTX_SDK_VERSION),
                                            MAKE_HEADER("x-sdk-platform", "c"),
                                            };

        header_num = sizeof(static_header) / sizeof(static_header[0]);
        if (header != NULL) {
            header_num += header->num;
        }
        nva = (http2_header *)HTTP2_STREAM_MALLOC(sizeof(http2_header) * header_num);
        if (nva == NULL) {
            h2_err("nva malloc failed\n");
            HTTP2_STREAM_FREE(info->channel_id);
            return FAIL_RETURN;
        }

        /* add external header if it's not NULL */
        header_count = http2_nv_copy(nva, 0, (http2_header *)static_header, sizeof(static_header) / sizeof(static_header[0]));
        if (header != NULL) {
            header_count = http2_nv_copy(nva, header_count, (http2_header *)header->nva, header->num);
        }

        header_count = sizeof(static_header) / sizeof(static_header[0]);
        h2_data.header = (http2_header *)static_header;
        h2_data.header_count = header_count;
        h2_data.data = NULL;
        h2_data.len = 0;
        h2_data.flag = 1;
        h2_data.stream_id = 0;

        HAL_MutexLock(handle->mutex);
        if(info->send_len < info->stream_len) {
            iotx_http2_reset_stream(handle->http2_connect,info->h2_stream_id);
        }
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
        http2_stream_node_insert(handle, h2_data.stream_id, info->user_data, &node);
        HTTP2_STREAM_FREE(nva);
    }

    if (rv < 0) {
        h2_err("client send error\n");
        HAL_MutexUnlock(handle->mutex);
        HTTP2_STREAM_FREE(info->channel_id);
        return rv;
    }

    if (node == NULL) {
        h2_err("node insert failed!");
        HAL_MutexUnlock(handle->mutex);
        HTTP2_STREAM_FREE(info->channel_id);
        return FAIL_RETURN;
    }

    node->stream_type = STREAM_TYPE_AUXILIARY;
    HAL_MutexUnlock(handle->mutex);

    rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
    if (rv < 0 || memcmp(node->status_code, "200", 3)) {
        h2_err("semaphore wait overtime or status code error\n");
    }

    /* just delete stream node */
    HAL_MutexLock(handle->mutex);
    list_for_each_entry_safe(node, next, &handle->stream_list, list, http2_stream_node_t) {
        if (info->h2_stream_id == node->stream_id) {
            h2_info("stream_node found:stream_id= %d, Delete It", node->stream_id);
            list_del((list_head_t *)&node->list);
            HTTP2_STREAM_FREE(node->channel_id);
            HAL_SemaphoreDestroy(node->semaphore);
            HTTP2_STREAM_FREE(node);
            continue;
        }
        if ((node->channel_id != NULL) && (stream_id != NULL) &&
            (len == strlen(node->channel_id) && !strncmp(node->channel_id, stream_id, len))) {
            list_del((list_head_t *)&node->list);
            HTTP2_STREAM_FREE(node->channel_id);
            HAL_SemaphoreDestroy(node->semaphore);
            HTTP2_STREAM_FREE(node);
        }
    }
    HTTP2_STREAM_FREE(info->channel_id);
    info->channel_id = NULL;
    HAL_MutexUnlock(handle->mutex);

    return rv;
}
#endif /* #ifdef FS_ENABLED */

int IOT_HTTP2_Stream_Close(void *hd, stream_data_info_t *info)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};
    stream_handle_t *handle = (stream_handle_t *)hd;
    char version[33] = {0};
    char *stream_id = info->channel_id;
    int len = strlen(stream_id);
    http2_stream_node_t *node, *next;

    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->channel_id, NULL_VALUE_ERROR);
    HAL_Snprintf(version, sizeof(version), "%d", get_version_int());
    HAL_Snprintf(path, sizeof(path), "/stream/close/%s", info->identify);
    {
        const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                            MAKE_HEADER_CS(":path", path),
                                            MAKE_HEADER(":scheme", "https"),
                                            MAKE_HEADER_CS("x-data-stream-id", info->channel_id),
                                            MAKE_HEADER_CS("x-sdk-version", version),
                                            MAKE_HEADER_CS("x-sdk-version-name", IOTX_SDK_VERSION),
                                            MAKE_HEADER("x-sdk-platform", "c"),
                                            };

        int header_count = sizeof(static_header) / sizeof(static_header[0]);
        h2_data.header = (http2_header *)static_header;
        h2_data.header_count = header_count;
        h2_data.data = NULL;
        h2_data.len = 0;
        h2_data.flag = 1;
        h2_data.stream_id = 0;

        HAL_MutexLock(handle->mutex);
        if(info->send_len < info->stream_len)
            iotx_http2_reset_stream(handle->http2_connect,info->h2_stream_id);
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
        HAL_MutexUnlock(handle->mutex);
    }

    if (rv < 0) {
        h2_warning("client send error\n");
    }

    /* just delete stream node */
    HAL_MutexLock(handle->mutex);
    list_for_each_entry_safe(node, next, &handle->stream_list, list, http2_stream_node_t) {
        if (info->h2_stream_id == node->stream_id) {
            h2_info("stream_node found:stream_id= %d, Delete It", node->stream_id);
            list_del((list_head_t *)&node->list);
            HTTP2_STREAM_FREE(node->channel_id);
            HAL_SemaphoreDestroy(node->semaphore);
            HTTP2_STREAM_FREE(node);
            continue;
        }
        if ((node->channel_id != NULL) && (stream_id != NULL) &&
            (len == strlen(node->channel_id) && !strncmp(node->channel_id, stream_id, len))) {
            list_del((list_head_t *)&node->list);
            HTTP2_STREAM_FREE(node->channel_id);
            HAL_SemaphoreDestroy(node->semaphore);
            HTTP2_STREAM_FREE(node);
        }
    }
    HTTP2_STREAM_FREE(info->channel_id);
    info->channel_id = NULL;
    HAL_MutexUnlock(handle->mutex);
    return rv;
}

int IOT_HTTP2_Disconnect(void *hd)
{
    int ret;
    stream_handle_t *handle = (stream_handle_t *)hd;
    http2_stream_node_t *node, *next;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    handle->init_state = 0;

    ret = HAL_SemaphoreWait(handle->semaphore, PLATFORM_WAIT_INFINITE);
    if (ret < 0) {
        h2_err("semaphore wait err\n");
        return FAIL_RETURN;
    }

    HAL_MutexLock(handle->mutex);
    list_for_each_entry_safe(node, next, &handle->stream_list, list, http2_stream_node_t) {
        list_del((list_head_t *)&node->list);
        HTTP2_STREAM_FREE(node->channel_id);
        HAL_SemaphoreDestroy(node->semaphore);
        HTTP2_STREAM_FREE(node);
    }
    HAL_MutexUnlock(handle->mutex);
    g_stream_handle = NULL;

    HAL_MutexDestroy(handle->mutex);
    HAL_SemaphoreDestroy(handle->semaphore);

    ret = iotx_http2_client_disconnect(handle->http2_connect);
    HTTP2_STREAM_FREE(handle);
    return ret;
}
