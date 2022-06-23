/**
 * @file log_buffer.c
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-01-23
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
 * 2022-01-23		1.0			hubertxxu		first commit
 * </table>
 */

#include "log_upload.h"
#include "utils_hmac.h"

typedef struct {
    void       *lock_buf;
    char       *log_buffer;
    const char *host;
    uint32_t    log_buffer_size;
    uint32_t    log_buffer_head_len;
    uint32_t    write_index;
    char        sign_key[SIGN_KEY_SIZE + 1];
} LogUploadBuffer;

static LogUploadBuffer *sg_log_upload_buffer_handle = NULL;

// ----------------------------------------------------------------------------
// Common
// ----------------------------------------------------------------------------

/**
 * @brief Reset log upload buffer.
 *
 * @param[in,out] log_upload_buffer_handle handle of log upload buffer
 */
static void _reset_log_buffer(LogUploadBuffer *log_upload_buffer_handle)
{
    log_upload_buffer_handle->write_index = log_upload_buffer_handle->log_buffer_head_len;
    memset(log_upload_buffer_handle->log_buffer + log_upload_buffer_handle->log_buffer_head_len, 0,
           log_upload_buffer_handle->log_buffer_size - log_upload_buffer_handle->log_buffer_head_len);
}

/**
 * @brief Init log buffer common logic.
 *
 * @param[in] init_params @see LogUploadInitParams
 * @return pointer to log buffer
 */
static LogUploadBuffer *_log_upload_buffer_init_pre(const LogUploadInitParams *init_params)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)HAL_Malloc(sizeof(LogUploadBuffer));
    if (!handle) {
        UPLOAD_ERR("log upload buffer handle malloc error.");
        goto err_exit;
    }

    memset(handle, 0, sizeof(LogUploadBuffer));

    handle->lock_buf = HAL_MutexCreate();
    if (!handle->lock_buf) {
        UPLOAD_ERR("mutex create failed");
        goto err_exit;
    }
    handle->log_buffer_size = init_params->log_buffer_size;
    handle->log_buffer      = HAL_Malloc(handle->log_buffer_size);
    if (!handle->log_buffer) {
        UPLOAD_ERR("malloc log buffer failed");
        goto err_exit;
    }
    memset(handle->log_buffer, 0, handle->log_buffer_size);

    /*init sign key*/
    int key_len = strlen(init_params->sign_key);
    if (!key_len) {
        UPLOAD_ERR("invalid sign key length");
        goto err_exit;
    }
    memcpy(handle->sign_key, init_params->sign_key, key_len > SIGN_KEY_SIZE ? SIGN_KEY_SIZE : key_len);

    sg_log_upload_buffer_handle = handle;
    return handle;
err_exit:
    if (handle) {
        HAL_MutexDestroy(handle->lock_buf);
        HAL_Free(handle);
    }
    return NULL;
}

/**
 * @brief Deinit log buffer handle.
 *
 * @param[in] upload_buffer_handle @see LogUploadBuffer
 */
void log_upload_buffer_deinit(void *upload_buffer_handle)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)upload_buffer_handle;
    if (!handle) {
        return;
    }
    HAL_MutexLock(handle->lock_buf);
    _reset_log_buffer(handle);
    HAL_Free(handle->log_buffer);
    handle->log_buffer = NULL;
    HAL_MutexUnlock(handle->lock_buf);
    HAL_MutexDestroy(handle->lock_buf);
    handle->lock_buf = NULL;
    HAL_Free(handle);
    sg_log_upload_buffer_handle = NULL;
}

/**
 * @brief Get the log buffer write index object.
 *
 * @param[in] log_upload_buffer_handle @see LogUploadBuffer
 * @return write index
 */
uint32_t log_upload_buffer_get_write_index(void *log_upload_buffer_handle)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)log_upload_buffer_handle;
    HAL_MutexLock(handle->lock_buf);
    uint32_t write_index = handle->write_index;
    HAL_MutexUnlock(handle->lock_buf);
    return write_index;
}

/**
 * @brief Get the log buffer object.
 *
 * @param[in] log_upload_buffer_handle @see LogUploadBuffer
 * @return log buffer
 */
char *log_upload_buffer_get(void *log_upload_buffer_handle)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)log_upload_buffer_handle;
    return handle->log_buffer;
}

/**
 * @brief Get the log buffer head len object.
 *
 * @param[in] log_upload_buffer_handle @see LogUploadBuffer
 * @return head length
 */
uint32_t log_upload_buffer_get_head_len(void *log_upload_buffer_handle)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)log_upload_buffer_handle;
    return handle->log_buffer_head_len;
}

/**
 * @brief Check log buffer is empty or not.
 *
 * @param[in] log_upload_buffer_handle @see LogUploadBuffer
 * @return true empty
 * @return false not empty
 */
bool log_upload_buffer_is_empty(void *log_upload_buffer_handle)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)log_upload_buffer_handle;
    return handle->write_index == handle->log_buffer_head_len;
}

/**
 * @brief When a new log appears during the reporting period, move the log to the specified position in the buffer.
 *
 * @param[in] log_upload_buffer_handle @see LogUploadBuffer
 * @param[in] need_remove_size
 */
void log_upload_buffer_remove_log(void *log_upload_buffer_handle, uint32_t need_remove_size)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)log_upload_buffer_handle;
    if (need_remove_size > handle->write_index || need_remove_size > handle->log_buffer_size) {
        return;
    }
    HAL_MutexLock(handle->lock_buf);
    memmove(handle->log_buffer + handle->log_buffer_head_len, handle->log_buffer + need_remove_size,
            handle->write_index - need_remove_size);
    handle->write_index = handle->write_index - need_remove_size + handle->log_buffer_head_len;
    memset(handle->log_buffer + handle->write_index, 0, handle->log_buffer_size - handle->write_index);
    HAL_MutexUnlock(handle->lock_buf);
}

/**
 * @brief Copy log buffer head.
 *
 * @param[out] dst Destination address
 * @param[in] log_upload_buffer_handle @see LogUploadBuffer
 */
void log_upload_buffer_copy_header(char *dst, void *log_upload_buffer_handle)
{
    LogUploadBuffer *handle = (LogUploadBuffer *)log_upload_buffer_handle;
    memcpy(dst, handle->log_buffer, handle->log_buffer_head_len);
}

/**
 * @brief Clear log upload buffer.
 *
 */
void log_upload_buffer_clear(void)
{
    if (!sg_log_upload_buffer_handle) {
        UPLOAD_ERR("log upload buffer not init.");
        return;
    }
    HAL_MutexLock(sg_log_upload_buffer_handle->lock_buf);
    _reset_log_buffer(sg_log_upload_buffer_handle);
    HAL_MutexUnlock(sg_log_upload_buffer_handle->lock_buf);
}

/**
 * @brief Get the log buffer size object.
 *
 * @return log buffer size
 */
uint32_t log_upload_buffer_get_size(void)
{
    if (!sg_log_upload_buffer_handle) {
        UPLOAD_ERR("log upload buffer not init.");
        return 0;
    }
    return sg_log_upload_buffer_handle->log_buffer_size;
}

// ----------------------------------------------------------------------------
// New log fromat by json
// ----------------------------------------------------------------------------
#ifdef LOG_UPLOAD_TYPE_JSON
/**
 * @brief Log upload buffer init.
 *
 * @param[in] init_params @see LogUploadInitParams
 * @return void* if success return @see LogUploadBuffer
 */
void *log_upload_buffer_init(const LogUploadInitParams *init_params)
{
    LogUploadBuffer *handle = _log_upload_buffer_init_pre(init_params);
    if (!handle) {
        return NULL;
    }
    handle->log_buffer_head_len = HAL_Snprintf(handle->log_buffer, handle->log_buffer_size,
                                               "{\"DeviceName\":\"%s\",\"ProductId\":\"%s\",\"Message\":[",
                                               init_params->device_name, init_params->product_id);
    handle->write_index         = handle->log_buffer_head_len;
    handle->host                = init_params->host;
    _reset_log_buffer(handle);
    return handle;
}

#ifdef LOG_CHECK_HTTP_RET_CODE
/**
 * @brief Get http respose result.
 *
 * @param[in] json http response buffer
 * @param[in] json_len http response buffer length
 * @return true log report success
 * @return false  log report error
 */
static bool _get_json_ret_code(char *json, size_t json_len)
{
    int            rc = 0;
    UtilsJsonValue resposse;
    // get response
    rc = utils_json_value_get("Response.Error", strlen("Response.Error"), json, json_len, &resposse);
    if (rc) {
        return true;
    }
    return false;
}
#endif

/**
 * @brief When upload, append endchar in the upload buffer ending.
 *
 * @param[in,out] log_buf upload buffer
 * @param[in] log_size  upload buffer index
 */
static void _append_endchar_to_upload_buffer(char *log_buf, size_t log_size)
{
    log_buf[log_size - 2] = ']';
    log_buf[log_size - 1] = '}';
}

/**
 * @brief When upload, remove endchar in the upload buffer ending.
 *
 * @param[in,out] log_buf upload buffer
 * @param[in] log_size  upload buffer index
 */
static void _remove_endchar_from_upload_buffer(char *log_buf, size_t log_size)
{
    log_buf[log_size - 2] = ',';
    log_buf[log_size - 1] = ' ';
}

/**
 * @brief Post data to tencent cloud over http.
 *
 * @param[in] post_buf  need post data buffer
 * @param[in] post_size need post data buffer length
 * @return @see IotReturnCode
 */
int post_one_http_to_server(void *log_upload_buffer_handle, char *post_buf, size_t post_size)
{
    int              rc     = 0;
    LogUploadBuffer *handle = (LogUploadBuffer *)log_upload_buffer_handle;
    if (!post_size) {
        return 0;
    }

    _append_endchar_to_upload_buffer(post_buf, post_size);

    HttpSignedParams params = {
        .host      = handle->host ? handle->host : LOG_UPLOAD_SERVER_URL,
        .uri       = LOG_UPLOAD_URI_PATH,
        .secretkey = handle->sign_key,
#ifdef LOG_CHECK_HTTP_RET_CODE
        .need_recv       = true,
        .recv_timeout_ms = HTTP_WAIT_RET_TIMEOUT_MS,
#else
        .need_recv       = false,
        .recv_timeout_ms = 0,
#endif
    };

#ifdef LOG_CHECK_HTTP_RET_CODE
    char buf[HTTP_RET_JSON_LENGTH] = {0};
    rc = IOT_HTTP_SignedRequest(&params, post_buf, post_size, (uint8_t *)buf, HTTP_RET_JSON_LENGTH);
    if (rc < 0) {
        UPLOAD_ERR("IOT_HTTP_SignedRequest failed, rc = %d", rc);
        goto exit;
    }

    if (rc == 0) {
        UPLOAD_ERR("HTTP Recv length = %d", rc);
        rc = QCLOUD_ERR_HTTP;
        goto exit;
    }

    buf[rc - 1] = '\0';  // json_parse relies on a string
    if (strlen(buf) > 0 && _get_json_ret_code(buf, rc)) {
        UPLOAD_DBG("Log server return SUCCESS: %s", buf);
        rc = QCLOUD_RET_SUCCESS;
    } else {
        UPLOAD_ERR("Log server return FAIL: %s", buf);
        rc = QCLOUD_ERR_HTTP;
    }

#else
    rc = IOT_HTTP_SignedRequest(&params, post_buf, post_size, NULL, 0);
#endif

exit:
    _remove_endchar_from_upload_buffer(post_buf, post_size);
    UPLOAD_DBG("%ld data have be post to server. rc : %d", post_size, rc);
    return rc;
}

/**
 * @brief Push data to log buffer.
 *
 * @param[in] log_content need push data
 * @return if 0 success else QCLOUD_ERR_FAILURE
 */
int log_upload_buffer_append(const char *log_content)
{
    uint16_t log_size = strlen(log_content);
    if (HAL_MutexTryLock(sg_log_upload_buffer_handle->lock_buf) != 0) {
        UPLOAD_ERR("trylock buffer failed!");
        return QCLOUD_ERR_FAILURE;
    }

    if ((sg_log_upload_buffer_handle->write_index + log_size + 5) > sg_log_upload_buffer_handle->log_buffer_size) {
        HAL_MutexUnlock(sg_log_upload_buffer_handle->lock_buf);
        UPLOAD_ERR("log upload buffer is not enough!");
        return QCLOUD_ERR_FAILURE;
    }
    sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\"';
    int index                                                                           = 0;
    while (index < (log_size - 2)) {
        // replace may overflow buffer size
        if ((sg_log_upload_buffer_handle->write_index + 5) > sg_log_upload_buffer_handle->log_buffer_size) {
            break;
        }

        switch (log_content[index]) {
            case '\"':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\"';
                break;
            case '\\':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                break;
            case '/':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '/';
                break;
            case '\b':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = 'b';
                break;
            case '\f':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = 'f';
                break;
            case '\n':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = 'n';
                break;
            case '\r':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = 'r';
                break;
            case '\t':
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\\';
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = 't';
                break;
            default:
                sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] =
                    log_content[index];
        }
        index++;
    }

    /* replace \r\n to \", add  as delimiter */
    sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = '\"';
    sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = ',';
    sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index++] = ' ';

    HAL_MutexUnlock(sg_log_upload_buffer_handle->lock_buf);
    return QCLOUD_RET_SUCCESS;
}

#else
// ----------------------------------------------------------------------------
// Old log format by text
// ----------------------------------------------------------------------------

/* log post header format */
#define TIMESTAMP_SIZE  (10)
#define SIGNATURE_SIZE  (40)
#define CTRL_BYTES_SIZE (4)
// LOG_BUF_FIXED_HEADER_SIZE = 112
#define LOG_BUF_FIXED_HEADER_SIZE \
    (SIGNATURE_SIZE + CTRL_BYTES_SIZE + MAX_SIZE_OF_PRODUCT_ID + MAX_SIZE_OF_DEVICE_NAME + TIMESTAMP_SIZE)

#ifdef LOG_UPLOAD_AES_ENCRYPT_POST
#include "mbedtls/aes.h"

/**
 * @brief Header of encrypt.
 *
 */
static char sg_head_content[128] = {0};

/**
 * @brief Log data encrypt by aes cbc.
 *
 * @param[in] handle @see LogUploadBuffer
 * @param[in] buf need encrypt data
 * @param[in] datalen need encrypt data length
 * @return @see IotReturnCode
 */
static int _log_aes_cbc_encrypt(LogUploadBuffer *handle, uint8_t *buf, uint32_t datalen)
{
    int ret = QCLOUD_RET_SUCCESS;

    mbedtls_aes_context ctx;

    unsigned char iv[16];
    memset(iv, '0', 16);

    mbedtls_aes_init(&ctx);

    ret = mbedtls_aes_setkey_enc(&ctx, (uint8_t *)handle->sign_key, 128);
    if (ret) {
        UPLOAD_ERR("Set encry key err,ret:%d", ret);
        ret = QCLOUD_ERR_FAILURE;
        goto exit;
    }

    ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, datalen, iv, buf, buf);
    if (ret) {
        Log_e("encrypt err,ret:%d", ret);
        ret = QCLOUD_ERR_FAILURE;
        goto exit;
    }

    mbedtls_aes_free(&ctx);
    return QCLOUD_RET_SUCCESS;

exit:
    mbedtls_aes_free(&ctx);
    return ret;
}
#endif

/**
 * @brief Log upload buffer init.
 *
 * @param[in] init_params @see LogUploadInitParams
 * @return void* if success return @see LogUploadBuffer
 */
void *log_upload_buffer_init(const LogUploadInitParams *init_params)
{
    LogUploadBuffer *handle = _log_upload_buffer_init_pre(init_params);
    if (!handle) {
        return NULL;
    }
    handle->log_buffer_head_len = LOG_BUF_FIXED_HEADER_SIZE;
    memset(handle->log_buffer, '#', handle->log_buffer_head_len);
    handle->log_buffer[SIGNATURE_SIZE] = 'P';
    memcpy(handle->log_buffer + SIGNATURE_SIZE + CTRL_BYTES_SIZE, init_params->product_id, MAX_SIZE_OF_PRODUCT_ID);
    memcpy(handle->log_buffer + SIGNATURE_SIZE + CTRL_BYTES_SIZE + MAX_SIZE_OF_PRODUCT_ID, init_params->device_name,
           strlen(init_params->device_name));
    handle->write_index = handle->log_buffer_head_len;
    handle->host        = init_params->host;
    _reset_log_buffer(handle);

#ifdef LOG_UPLOAD_AES_ENCRYPT_POST
    HAL_Snprintf(sg_head_content, 128,
                 "Accept:application/json;*/*\r\n"
                 "X-TC-IotEncryType:%c\r\n"
                 "X-TC-IotCid:%s%s\r\n",
                 handle->log_buffer[SIGNATURE_SIZE], init_params->product_id, init_params->device_name);
#endif

    return handle;
}

#ifdef LOG_CHECK_HTTP_RET_CODE
/**
 * @brief Get http respose result.
 *
 * @param[in] json http response buffer
 * @param[in] json_len http response buffer length
 * @return true log report success
 * @return false  log report error
 */
static bool _get_json_ret_code(char *json, size_t json_len)
{
    int      rc = 0;
    uint32_t ret_code;
    rc = utils_json_get_uint32("Retcode", strlen("Retcode"), json, json_len, &ret_code);
    if (!rc) {
        if (!ret_code) {
            return true;
        }
    }
    return false;
}
#endif

/**
 * @brief Update timestamp add signature post buffer.
 *
 * @param[in] handle @see LogUploadBuffer
 * @param[in] post_buf  need post data buffer
 * @param[in] post_size need post data buffer length
 */
static void _update_time_and_signature(LogUploadBuffer *handle, char *post_buf, size_t post_size)
{
    char     timestamp[TIMESTAMP_SIZE + 1] = {0};
    char     signature[SIGNATURE_SIZE + 1] = {0};
    uint32_t now_time                      = IOT_Timer_CurrentSec();

    /* record the timestamp for this log uploading */
    HAL_Snprintf(timestamp, TIMESTAMP_SIZE + 1, "%010d", now_time);
    memcpy(post_buf + sg_log_upload_buffer_handle->log_buffer_head_len - TIMESTAMP_SIZE, timestamp, strlen(timestamp));

    /* signature of this log uploading */
    utils_hmac_sha1_hex(post_buf + SIGNATURE_SIZE, post_size - SIGNATURE_SIZE, (uint8_t *)handle->sign_key,
                        strlen(handle->sign_key), signature);
    memcpy(post_buf, signature, SIGNATURE_SIZE);
}

/**
 * @brief Post data to tencent cloud over http.
 *
 * @param[in] post_buf  need post data buffer
 * @param[in] post_size need post data buffer length
 * @return @see IotReturnCode
 */
int post_one_http_to_server(void *log_upload_buffer_handle, char *post_buf, size_t post_size)
{
    int              rc                 = 0;
#ifdef LOG_UPLOAD_AES_ENCRYPT_POST
    char            *log_encrypt_buffer = NULL;
#endif
    LogUploadBuffer *handle             = (LogUploadBuffer *)log_upload_buffer_handle;
    _update_time_and_signature(handle, post_buf, post_size);
    void *http_client = IOT_HTTP_Init();
    if (!http_client) {
        UPLOAD_ERR("http client init error");
        goto exit;
    }
    IotHTTPConnectParams connect_params = {
        .url    = handle->host ? handle->host : LOG_UPLOAD_OLD_SERVER_URL,
        .port   = LOG_UPLOAD_OLD_SERVER_PORT,
        .ca_crt = NULL,  // TODO: support cert
    };

    rc = IOT_HTTP_Connect(http_client, &connect_params);
    if (rc) {
        UPLOAD_ERR("http connect error.");
        goto exit;
    }
    IotHTTPRequestParams request = {
        .url            = handle->host ? handle->host : LOG_UPLOAD_OLD_SERVER_URL,
        .method         = IOT_HTTP_METHOD_POST,
        .content_length = post_size,
        .content_type   = "text/plain;charset=utf-8",
        .content        = post_buf,
        .header         = "Accept:application/json;*/*\r\n",
    };

#ifdef LOG_UPLOAD_AES_ENCRYPT_POST
    /*ANSI X.923: zero-padding with last byte as padlen */
    int padlen         = 16 - post_size % 16;
    int datalen        = post_size + padlen;
    log_encrypt_buffer = HAL_Malloc(datalen + 1);
    if (!log_encrypt_buffer) {
        UPLOAD_ERR("malloc log encrypt buffer error. malloc length : %d", datalen);
        rc = QCLOUD_ERR_MALLOC;
        goto exit;
    }
    memset(log_encrypt_buffer, 0, datalen + 1);
    memcpy(log_encrypt_buffer, post_buf, post_size);
    log_encrypt_buffer[datalen - 1] = padlen;

    // aes cbc
    rc = _log_aes_cbc_encrypt(handle, (uint8_t *)log_encrypt_buffer, datalen);
    if (QCLOUD_RET_SUCCESS != rc) {
        UPLOAD_ERR("log encrypt failed, ret = %d", rc);
        goto exit;
    }
    request.header         = sg_head_content;
    request.content_type   = "application/octet-stream";
    request.content_length = datalen;
    request.content        = log_encrypt_buffer;
#endif

    rc = IOT_HTTP_Request(http_client, &request);
    // 2. check rc
    if (rc) {
        UPLOAD_ERR("http post error.");
        goto exit;
    }

#ifdef LOG_CHECK_HTTP_RET_CODE
    char buf[HTTP_RET_JSON_LENGTH] = {0};
    rc = IOT_HTTP_Recv(http_client, (uint8_t *)buf, HTTP_RET_JSON_LENGTH, HTTP_WAIT_RET_TIMEOUT_MS);
    if (rc < 0) {
        UPLOAD_ERR("qcloud_http_recv_data failed, rc = %d", rc);
    } else if (rc == 0) {
        UPLOAD_ERR("HTTP Recv length == %d", rc);
        rc = QCLOUD_ERR_HTTP_TIMEOUT;
    } else {
        buf[rc - 1] = '\0';  // json_parse relies on a string
        if (strlen(buf) > 0 && _get_json_ret_code(buf, rc)) {
            UPLOAD_DBG("Log server return SUCCESS: %s", buf);
            rc = QCLOUD_RET_SUCCESS;
        } else {
            UPLOAD_ERR("Log server return FAIL: %s", buf);
            rc = QCLOUD_ERR_HTTP;
        }
    }
#endif

exit:
    if (http_client) {
        IOT_HTTP_Disconnect(http_client);
        IOT_HTTP_Deinit(http_client);
    }
#ifdef LOG_UPLOAD_AES_ENCRYPT_POST
    if (log_encrypt_buffer) {
        HAL_Free(log_encrypt_buffer);
        log_encrypt_buffer = NULL;
    }
#endif
    UPLOAD_DBG("%ld data have be post to server. rc : %d", post_size, rc);
    return rc;
}

/**
 * @brief Push data to log buffer.
 *
 * @param[in] log_content need push data
 * @return if 0 success else QCLOUD_ERR_FAILURE
 */
int log_upload_buffer_append(const char *log_content)
{
    uint16_t log_size = strlen(log_content);
    if (HAL_MutexTryLock(sg_log_upload_buffer_handle->lock_buf) != 0) {
        UPLOAD_ERR("trylock buffer failed!");
        return QCLOUD_ERR_FAILURE;
    }

    if ((sg_log_upload_buffer_handle->write_index + log_size + 1) > sg_log_upload_buffer_handle->log_buffer_size) {
        HAL_MutexUnlock(sg_log_upload_buffer_handle->lock_buf);
        UPLOAD_DBG("log upload buffer is not enough! remain : %d, need upload length: %d",
                   sg_log_upload_buffer_handle->log_buffer_size - sg_log_upload_buffer_handle->write_index, log_size);
        return QCLOUD_ERR_BUF_TOO_SHORT;
    }

    memcpy(sg_log_upload_buffer_handle->log_buffer + sg_log_upload_buffer_handle->write_index, log_content, log_size);
    sg_log_upload_buffer_handle->write_index += log_size;

    /* replace \r\n to \n\f as delimiter */
    sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index - 1] = '\f';
    sg_log_upload_buffer_handle->log_buffer[sg_log_upload_buffer_handle->write_index - 2] = '\n';

    HAL_MutexUnlock(sg_log_upload_buffer_handle->lock_buf);
    return QCLOUD_RET_SUCCESS;
}

#endif
