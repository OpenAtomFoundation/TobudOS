/**
 * @file log_upload.c
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-01-24
 *
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @par Change Log:
 * <table>
 * Date				Version		Author			Description
 * 2022-01-24		1.0			hubertxxu		first commit
 * </table>
 */

#include "log_upload.h"

#ifdef LOG_UPLOAD_TYPE_JSON
#define LOG_DELIMITER "\", "
#else
#define LOG_DELIMITER "\n\f"
#endif

/**
 * @brief data structure for log upload
 *
 */
typedef struct {
    bool     upload_only_in_comm_err;
    LogLevel log_level;
    Timer    upload_timer;
    void    *upload_buffer_handle;

    const char    *save_log_filename;
    LogSaveFunc    save_func;
    LogReadFunc    read_func;
    LogDelFunc     del_func;
    LogGetSizeFunc get_size_func;

    bool log_save_enabled;
    bool log_client_init_done;
} QcloudIoTLogUpload;

static QcloudIoTLogUpload *sg_log_uploader_client;

// ----------------------------------------------------------------------------
// Internal API
// ----------------------------------------------------------------------------

/**
 * @brief
 *
 * @return QcloudIoTLogUpload* @see QcloudIoTLogUpload
 */
static QcloudIoTLogUpload *_get_log_uploader_client(void)
{
    if (!sg_log_uploader_client) {
        return NULL;
    }
    if (!sg_log_uploader_client->log_client_init_done) {
        return NULL;
    }
    return sg_log_uploader_client;
}

/**
 * @brief
 *
 * @param client @see QcloudIoTLogUpload
 */
static void _set_log_uploader_client(void *client)
{
    sg_log_uploader_client = client;
}

/**
 * @brief set log upload in comm err state
 *
 * @param state true or false
 */
void log_upload_set_upload_log_in_comm_err(bool state)
{
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    POINTER_SANITY_CHECK_RTN(log_uploader_handle);
    log_uploader_handle->upload_only_in_comm_err = state;
}

/**
 * @brief change log upload level
 *
 * @param level
 */
void log_upload_set_log_upload_level(LogLevel level)
{
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    POINTER_SANITY_CHECK_RTN(log_uploader_handle);
    log_uploader_handle->log_level = level;
}

/**
 * @brief get log upload level
 *
 * @return LogLevel
 */
LogLevel log_upload_get_log_upload_level(void)
{
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    POINTER_SANITY_CHECK(log_uploader_handle, LOG_LEVEL_DISABLE);
    return log_uploader_handle->log_level;
}

/**
 * @brief check if need force upload
 *
 * @param[in] force_upload
 * @return true need force upload
 * @return false not needed force upload
 */
static bool _check_force_upload(bool force_upload)
{
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    if (!log_uploader_handle) {
        return false;
    }
    /* 1: check if have data to upload */
    if (have_data_need_update(log_uploader_handle->upload_buffer_handle)) {
        return false;
    }

    uint32_t write_index   = get_log_buffer_write_index(log_uploader_handle->upload_buffer_handle);
    bool     is_low_buffer = (get_log_buffer_size() - write_index) < LOG_LOW_BUFFER_THRESHOLD ? true : false;

    /* 2. check  if upload only in comm err */
    if (!force_upload && log_uploader_handle->upload_only_in_comm_err) {
        /* buffer is low but we couldn't upload now, reset buffer */
        if (is_low_buffer) {
            log_upload_clear_buffer();
        }
        HAL_Timer_CountdownMs(&log_uploader_handle->upload_timer, LOG_UPLOAD_INTERVAL_MS);
        return false;
    }

    /* 3: force upload is true, handle it right now */
    if (force_upload) {
        return true;
    }
    /* 4 : check if buffer is low */
    if (is_low_buffer) {
        /* buffer is low, handle it right now */
        return true;
    }
    /*5: check if timeout, handle it right now*/
    return HAL_Timer_Expired(&log_uploader_handle->upload_timer);
}

/**
 * @brief save log to file or flash
 *
 * @param[in] log_buf need saved buffer
 * @param[in] log_size need saved buffer length
 * @return @see IotReturnCode
 */
static int _save_log(char *log_buf, size_t log_size)
{
    int                 rc = 0;
    size_t              write_size, current_size;
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    POINTER_SANITY_CHECK(log_uploader_handle, QCLOUD_ERR_INVAL);
    if (!log_size) {
        UPLOAD_ERR("nothing need to save. log_size : %ld", log_size);
        return -1;
    }

    current_size = log_uploader_handle->get_size_func(log_uploader_handle->save_log_filename);

    /* overwrite the previous saved log to avoid too many saved logs */
    if ((current_size + log_size) > MAX_LOG_SAVE_SIZE) {
        UPLOAD_ERR("overwrite the previous saved log. %ld", current_size);
        rc = log_uploader_handle->del_func(log_uploader_handle->save_log_filename);
        if (rc) {
            Log_e("fail to delete previous log");
        }
    }
    UPLOAD_DBG("saved data [%ld]", log_size);
    write_size = log_uploader_handle->save_func(log_uploader_handle->save_log_filename, log_buf, log_size);
    if (write_size != log_size) {
        Log_e("fail to save log. RC %ld - log size %ld", write_size, log_size);
    }

    return write_size != log_size ? -1 : 0;
}

static uint32_t _buffer_get_at_most_message(char *post_buf)
{
    uint32_t possible_size = 0;
    char    *next_log_buf  = NULL;
    size_t   delimiter_len = strlen(LOG_DELIMITER);
    while (possible_size < MAX_HTTP_LOG_POST_SIZE) {
        /* locate the delimiter */
        next_log_buf = strstr(post_buf + possible_size, LOG_DELIMITER);
        if (!next_log_buf) {
            return 0;
        }
        possible_size = (size_t)(next_log_buf - post_buf + delimiter_len);
        /* end of log */
        if (next_log_buf[delimiter_len] == 0 && possible_size < MAX_HTTP_LOG_POST_SIZE) {
            break;
        }
    }
    return possible_size;
}

/**
 * @brief report these to the server
 *
 * @param post_buf
 * @param post_size
 * @param is_log_buffer
 * @return int
 */
static int _post_log_buffer_to_cloud(QcloudIoTLogUpload *log_uploader_handle, char *post_buf, size_t post_size,
                                     bool is_log_buffer)
{
    int      rc                  = 0;
    uint32_t log_buffer_head_len = get_log_buffer_head_len(log_uploader_handle->upload_buffer_handle);

    size_t orig_post_size = post_size;
    size_t post_payload, possible_size, actual_post_payload = 0;
    do {
        // 1. get message in buffer
        possible_size = _buffer_get_at_most_message(post_buf);
        if (!possible_size) {
            UPLOAD_ERR("Upload size should not be 0! Total sent: %ld. Left: %ld",
                       actual_post_payload + log_buffer_head_len, post_size);
            if (is_log_buffer) {
                log_upload_clear_buffer();
            }
            return QCLOUD_RET_SUCCESS;
        }
        if (is_log_buffer && (possible_size > get_log_buffer_size() || possible_size > orig_post_size ||
                              possible_size <= log_buffer_head_len)) {
            log_upload_clear_buffer();
            UPLOAD_ERR("possible size (%ld) is too large.", possible_size);
            return QCLOUD_RET_SUCCESS;
        }
        // 2. post to server
        rc = post_one_http_to_server(log_uploader_handle->upload_buffer_handle, post_buf, possible_size);
        if (rc) {
            UPLOAD_ERR("Send log failed. Total sent: %ld. Left: %ld", actual_post_payload + log_buffer_head_len,
                       post_size);
            // when send failed save log in file.
            if (log_uploader_handle->log_save_enabled && is_log_buffer) {
                _save_log(post_buf + log_buffer_head_len, possible_size - log_buffer_head_len);
            }
        }

        // 3. remove have be post message
        memmove(post_buf + log_buffer_head_len, post_buf + possible_size, post_size - possible_size);
        post_payload = possible_size - log_buffer_head_len;
        post_size -= post_payload;
        actual_post_payload += post_payload;
        memset(post_buf + post_size, 0, orig_post_size - post_size);
        UPLOAD_DBG("post log %d OK. Total sent: %d. Left: %d ", possible_size,
                   actual_post_payload + log_buffer_head_len, post_size);
    } while (post_size > log_buffer_head_len);

    if (is_log_buffer) {
        remove_log_from_buffer(log_uploader_handle->upload_buffer_handle, orig_post_size);
    }

    return rc;
}

/**
 * @brief handle log to save flash or file
 *
 * @return @see IotReturnCode
 */
static int _handle_saved_log(void)
{
    int                 rc                  = QCLOUD_RET_SUCCESS;
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    if (!log_uploader_handle) {
        return QCLOUD_ERR_FAILURE;
    }

    size_t whole_log_size = log_uploader_handle->get_size_func(log_uploader_handle->save_log_filename);
    if (!whole_log_size) {
        return rc;
    }
    uint32_t log_buffer_head_len = get_log_buffer_head_len(log_uploader_handle->upload_buffer_handle);
    size_t   buf_size            = whole_log_size + log_buffer_head_len + 1;
    char    *log_buf             = HAL_Malloc(buf_size);
    if (!log_buf) {
        UPLOAD_ERR("Malloc failed, size: %ld", buf_size);
        return QCLOUD_ERR_FAILURE;
    }

    /* read the whole log to buffer */
    size_t read_len = log_uploader_handle->read_func(log_uploader_handle->save_log_filename,
                                                     log_buf + log_buffer_head_len, whole_log_size);
    if (read_len != whole_log_size) {
        Log_e("fail to read whole saved log. Size: %ld - read: %ld", whole_log_size, read_len);
        rc = QCLOUD_ERR_FAILURE;
        goto exit;
    }

    /* copy header from global log buffer */
    size_t upload_size = whole_log_size + log_buffer_head_len;
    copy_log_buffer_header(log_buf, log_uploader_handle->upload_buffer_handle);
    log_buf[buf_size - 1] = 0;

    /* post server */
    rc = _post_log_buffer_to_cloud(log_uploader_handle, log_buf, upload_size, false);
    if (rc == QCLOUD_RET_SUCCESS || rc == QCLOUD_ERR_INVAL || rc == QCLOUD_ERR_HTTP) {
        UPLOAD_DBG("handle saved log done! Size: %ld. ", whole_log_size);
        log_uploader_handle->del_func(log_uploader_handle->save_log_filename);
    }
exit:
    HAL_Free(log_buf);
    return rc;
}

/**
 * @brief append need report log to log upload buffer
 *
 * @param[in] log_content data of need to report
 */
static void _log_upload_append_to_log_buffer(int log_level, const char *log_content)
{
    int                 rc                  = 0;
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    // POINTER_SANITY_CHECK_RTN(log_uploader_handle);
    if (!log_uploader_handle) {
        return;
    }
    // if not necessary. just give up upload.
    if (log_level > log_uploader_handle->log_level) {
        return;
    }

    uint16_t log_size = strlen(log_content);
    if (!log_size) {
        UPLOAD_ERR("log_size is not 0!");
        return;
    }
    rc = append_data_to_log_buffer(log_content);
    if (rc) {
        /* log buffer is full. upload data right now */
        HAL_Timer_CountdownMs(&log_uploader_handle->upload_timer, 0);
    }
}

/**
 * @brief upload data to tencent cloud
 *
 * @param force_upload if necessary
 * @return @see IotReturnCode
 */
static int _do_log_upload(bool force_upload)
{
    int         rc;
    static bool sg_need_handle_saved_log = true;

    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    POINTER_SANITY_CHECK(log_uploader_handle, QCLOUD_ERR_INVAL);

    /* step1: handle previously saved log */
    if (log_uploader_handle->log_save_enabled && sg_need_handle_saved_log) {
        rc = _handle_saved_log();
        if (!rc) {
            sg_need_handle_saved_log = false;
        }
    }

    /* step2: check if have data to upload */
    if (!_check_force_upload(force_upload)) {
        return QCLOUD_RET_SUCCESS;
    }

    // log_buffer_lock();

    /* step3: post log buffer data to cloud */
    uint32_t upload_log_size = get_log_buffer_write_index(log_uploader_handle->upload_buffer_handle);
    rc = _post_log_buffer_to_cloud(log_uploader_handle, get_log_buffer(log_uploader_handle->upload_buffer_handle),
                                   upload_log_size, true);
    if (rc) {
        sg_need_handle_saved_log = true;
    }

    HAL_Timer_CountdownMs(&log_uploader_handle->upload_timer, LOG_UPLOAD_INTERVAL_MS);

    // log_buffer_unlock();
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief init log upload previously
 *
 * @param[in] init_params @see LogUploadInitParams
 * @return @see IotReturnCode
 */
static int log_upload_init_pre(LogUploadInitParams *init_params)
{
    if (_get_log_uploader_client()) {
        UPLOAD_DBG("log upload initialized.");
        return QCLOUD_RET_SUCCESS;
    }

    QcloudIoTLogUpload *log_uploader_handle = HAL_Malloc(sizeof(QcloudIoTLogUpload));
    if (!log_uploader_handle) {
        UPLOAD_ERR("allocate for log client failed");
        goto err_exit;
    }

    log_uploader_handle->upload_only_in_comm_err = false;

    /* all the call back functions are necessary to handle log save and re-upload*/
    if (init_params->save_func && init_params->read_func && init_params->del_func && init_params->get_size_func &&
        init_params->save_log_filename) {
        log_uploader_handle->save_log_filename = init_params->save_log_filename;
        log_uploader_handle->save_func         = init_params->save_func;
        log_uploader_handle->read_func         = init_params->read_func;
        log_uploader_handle->del_func          = init_params->del_func;
        log_uploader_handle->get_size_func     = init_params->get_size_func;
        log_uploader_handle->log_save_enabled  = true;
    } else {
        log_uploader_handle->log_save_enabled = false;
    }

    log_uploader_handle->upload_buffer_handle = log_upload_buffer_init(init_params);
    if (!log_uploader_handle->upload_buffer_handle) {
        goto err_exit;
    }

    _set_log_uploader_client(log_uploader_handle);
    log_uploader_handle->log_level            = LOG_LEVEL_DEBUG;
    log_uploader_handle->log_client_init_done = true;
    return QCLOUD_RET_SUCCESS;

err_exit:
    if (log_uploader_handle) {
        log_upload_buffer_deinit(log_uploader_handle->upload_buffer_handle);
        HAL_Free(log_uploader_handle);
        log_uploader_handle = NULL;
    }
    return QCLOUD_ERR_FAILURE;
}

/**
 * @brief stop log upload add release resources
 *
 * @return @see IotReturnCode
 */
static int log_upload_deinit(void)
{
    QcloudIoTLogUpload *log_uploader_handle = _get_log_uploader_client();
    POINTER_SANITY_CHECK(log_uploader_handle, QCLOUD_ERR_INVAL);
    log_upload_buffer_deinit(log_uploader_handle->upload_buffer_handle);
    HAL_Free(log_uploader_handle);
    log_uploader_handle = NULL;
    return QCLOUD_RET_SUCCESS;
}

// ----------------------------------------------------------------------------
// External API
// ----------------------------------------------------------------------------

/**
 * @brief init log upload previously
 *
 * @param[in] init_params @see LogUploadInitParams
 * @return @see IotReturnCode
 */
int IOT_Log_Upload_InitPre(LogUploadInitParams *init_params)
{
    POINTER_SANITY_CHECK(init_params, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(init_params->product_id, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(init_params->device_name, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(init_params->sign_key, QCLOUD_ERR_INVAL);
    return log_upload_init_pre(init_params);
}

/**
 * @brief init log upload module
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int IOT_Log_Upload_Init(void *client)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    return log_upload_init(client);
}

/**
 * @brief stop log upload add release resources
 *
 * @return @see IotReturnCode
 */
int IOT_Log_Upload_Deinit(void)
{
    return log_upload_deinit();
}

/**
 * @brief do log upload
 *
 * @param force_upload if necessary
 * @return @see IotReturnCode
 */
int IOT_Log_Upload(bool force_upload)
{
    return _do_log_upload(force_upload);
}

/**
 * @brief append need report log to log upload buffer
 *
 * @param[in] log_content data of need to report
 */
void IOT_Log_Upload_AppendToUploadBuffer(int log_level, const char *log_content)
{
    POINTER_SANITY_CHECK_RTN(log_content);
    _log_upload_append_to_log_buffer(log_level, log_content);
}
