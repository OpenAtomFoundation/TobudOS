/**
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
 * @file cos_download.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-25
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-25 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_cos.h"

/**
 * @brief COS request download header.
 *
 */
#define HTTP_COS_DOWNLOAD_REQUEST_HEADER_LEN 256

/**
 * @brief Download handle.
 *
 */
typedef struct {
    IotCosDownloadParams params;
    void                *http_client;
    IotHTTPRequestParams http_request;
    int                  download_size;
    int                  is_first_run;
} HTTPCosDownloadHandle;

/**
 * @brief Connect cos http server.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 * @return 0 for success. others @see IotReturnCode
 */
static int _cos_download_connect(HTTPCosDownloadHandle *handle)
{
    IotHTTPConnectParams connect_params = {
        .url    = handle->params.url,
        .port   = handle->params.is_https_enabled ? "443" : "80",
        .ca_crt = NULL,  // TODO: support cert

    };
    return IOT_HTTP_Connect(handle->http_client, &connect_params);
}

/**
 * @brief Construct cos download request header.
 *
 * @param[out] header pointer to request header
 * @param[in] is_fragmentation http fragmentation
 * @param[in] begin_byte download begin byte
 * @param[in] end_byte download end byte
 * @return > 0 for header len, others fail
 */
static int _cos_download_request_header_construct(char *header, int is_fragmentation, int begin_byte, int end_byte)
{
    int len = HAL_Snprintf(header, HTTP_COS_DOWNLOAD_REQUEST_HEADER_LEN,
                           "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                           "Accept-Encoding:gzip, deflate\r\nRange:bytes=%d-%d\r\n",
                           begin_byte, end_byte);
    if (len <= 0) {
        return QCLOUD_ERR_BUF_TOO_SHORT;
    }

    if (is_fragmentation) {
        const char *keep_alive     = "Connection:keep-alive\r\n";
        int         keep_alive_len = strlen(keep_alive);
        if (HTTP_COS_DOWNLOAD_REQUEST_HEADER_LEN - len - 1 <= keep_alive_len) {
            return QCLOUD_ERR_BUF_TOO_SHORT;
        }
        strncpy(header + len, keep_alive, keep_alive_len);
        header[len + keep_alive_len] = '\0';
        len += keep_alive_len;
    }
    return len;
}

/**
 * @brief Request cos download.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 * @param[in] max_len max_len for buffer to download in fragmentation
 * @return 0 for success. others @see IotReturnCode
 */
static int _cos_download_request(HTTPCosDownloadHandle *handle, int max_len)
{
    int rc, begin_byte, end_byte = 0;

    begin_byte = handle->download_size;
#define min_http(x, y) (((x) < (y)) ? (x) : (y))
    end_byte = handle->params.is_fragmentation
                   ? begin_byte + min_http(max_len, handle->params.file_size - handle->download_size) - 1
                   : handle->params.file_size - 1;
#undef min_http

    rc = _cos_download_request_header_construct(handle->http_request.header, handle->params.is_fragmentation,
                                                begin_byte, end_byte);
    if (rc <= 0) {
        return rc;
    }

    handle->http_request.url            = handle->params.url;
    handle->http_request.method         = IOT_HTTP_METHOD_GET;
    handle->http_request.content_length = 0;
    handle->http_request.content = handle->http_request.content_type = NULL;
    return IOT_HTTP_Request(handle->http_client, &handle->http_request);
}

/**
 * @brief HTTP recv data.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 * @param[out] buf buf to store received data
 * @param[in] buf_len buf_len
 * @param[in] timeout_ms read socket timeout
 * @return >= 0 for recv data len. others @see IotReturnCode
 */
static int _cos_download_recv_data(HTTPCosDownloadHandle *handle, uint8_t *buf, int buf_len, uint32_t timeout_ms)
{
    int rc = IOT_HTTP_Recv(handle->http_client, buf, buf_len, timeout_ms);
    if (rc > 0) {
        handle->download_size += rc;
    }
    return rc;
}

/**************************************************************************************
 * API
 **************************************************************************************/

/**
 * @brief Init cos download handle.
 *
 * @param[in] params @see IotCosDownloadParams
 * @return pointer to cos download handle
 */
void *IOT_COS_DownloadInit(IotCosDownloadParams *params)
{
    POINTER_SANITY_CHECK(params, NULL);

    HTTPCosDownloadHandle *handle = HAL_Malloc(sizeof(HTTPCosDownloadHandle));
    if (!handle) {
        goto exit;
    }

    handle->http_client = IOT_HTTP_Init();
    if (!handle->http_client) {
        goto exit;
    }

    handle->http_request.header = HAL_Malloc(HTTP_COS_DOWNLOAD_REQUEST_HEADER_LEN);
    if (!handle->http_request.header) {
        goto exit;
    }

    handle->params        = *params;
    handle->download_size = params->offset;

    if (_cos_download_connect(handle)) {
        goto exit;
    }
    handle->is_first_run = 1;
    return handle;
exit:
    if (handle) {
        HAL_Free(handle->http_request.header);
        IOT_HTTP_Deinit(handle->http_client);
        HAL_Free(handle);
    }
    return NULL;
}

/**
 * @brief Fetch data from cos.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 * @param[out] buf buffer to store data
 * @param[in] buf_len buffer length
 * @param timeout_ms timeout for fetching
 * @return >= 0 for recv data len. others @see IotReturnCode
 */
int IOT_COS_DownloadFetch(void *handle, uint8_t *buf, uint32_t buf_len, uint32_t timeout_ms)
{
    POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
    int rc = 0;

    HTTPCosDownloadHandle *download_handle = (HTTPCosDownloadHandle *)handle;

    // download finish
    if (IOT_COS_DownloadIsFinished(handle)) {
        return 0;
    }

    if (download_handle->is_first_run) {
        rc = _cos_download_request(download_handle, buf_len);
        if (rc) {
            Log_e("cos request failed %d", rc);
            return rc;
        }
        download_handle->is_first_run = 0;
        return _cos_download_recv_data(download_handle, buf, buf_len, timeout_ms);
    }

    if (download_handle->params.is_fragmentation && IOT_HTTP_IsRecvFinished(download_handle->http_client)) {
        rc = _cos_download_request(download_handle, buf_len);
        if (rc) {
            Log_e("cos request failed %d", rc);
            return rc;
        }
    }

    return _cos_download_recv_data(download_handle, buf, buf_len, timeout_ms);
}

/**
 * @brief Is download finished.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 * @return true for finished
 */
int IOT_COS_DownloadIsFinished(void *handle)
{
    POINTER_SANITY_CHECK(handle, QCLOUD_ERR_INVAL);
    HTTPCosDownloadHandle *download_handle = (HTTPCosDownloadHandle *)handle;
    return download_handle->download_size == download_handle->params.file_size;
}

/**
 * @brief Deinit cos download.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 */
void IOT_COS_DownloadDeinit(void *handle)
{
    POINTER_SANITY_CHECK_RTN(handle);
    HTTPCosDownloadHandle *download_handle = (HTTPCosDownloadHandle *)handle;
    IOT_HTTP_Disconnect(download_handle->http_client);
    IOT_HTTP_Deinit(download_handle->http_client);
    HAL_Free(download_handle->http_request.header);
    HAL_Free(download_handle);
}
