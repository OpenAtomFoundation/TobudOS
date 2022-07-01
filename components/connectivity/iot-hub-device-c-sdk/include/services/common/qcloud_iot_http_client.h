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
 * @file qcloud_iot_http_client.h
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

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_HTTP_CLIENT_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_HTTP_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_common.h"

/**
 * @brief HTTP write socket timeout.
 *
 */
#define HTTP_WRITE_TIMEOUT_MS 5000

/**
 * @brief HTTPs read tls timeout.
 *
 */
#define HTTPS_READ_TIMEOUT_MS 2000

/**
 * @brief Connect params set by user.
 *
 */
typedef struct {
    const char *url;    /**< http header */
    const char *port;   /**< content length(optional) */
    const char *ca_crt; /**< content type(optional) */
} IotHTTPConnectParams;

/**
 * @brief Http method.
 *
 */
typedef enum {
    IOT_HTTP_METHOD_GET = 0,
    IOT_HTTP_METHOD_POST,
    IOT_HTTP_METHOD_PUT,
    IOT_HTTP_METHOD_DELETE,
    IOT_HTTP_METHOD_HEAD,
} IotHTTPMethod;

/**
 * @brief Request params set by user.
 * @ref https://datatracker.ietf.org/doc/html/rfc7231
 *
 */
typedef struct {
    const char   *url;            /**< request url */
    IotHTTPMethod method;         /**< request method */
    char         *header;         /**< http header */
    int           content_length; /**< content length(optional) */
    char         *content_type;   /**< content type(optional) */
    char         *content;        /**< content (optional) */
} IotHTTPRequestParams;

/**
 * @brief Malloc http client.
 *
 * @return pointer to http client
 */
void *IOT_HTTP_Init(void);

/**
 * @brief Free http client.
 *
 */
void IOT_HTTP_Deinit(void *client);

/**
 * @brief Connect http server.
 *
 * @param[in,out] client pointer to http client
 * @param[in] params params needed to connect http server, @see IotHTTPConnectParams
 * @return 0 for success. others @see IotReturnCode
 */
int IOT_HTTP_Connect(void *client, IotHTTPConnectParams *params);

/**
 * @brief Request http server.
 *
 * @param[in,out] client pointer to http client
 * @param[in] params params needed to send request to http server, @see IotHTTPRequestParams
 * @return 0 for success. others @see IotReturnCode
 */
int IOT_HTTP_Request(void *client, IotHTTPRequestParams *params);

/**
 * @brief Send data to http server.
 *
 * @param[in,out] client pointer to http client
 * @param[in] data data to send
 * @param[out] data_len data len
 * @return 0 for success. others @see IotReturnCode
 */
int IOT_HTTP_Send(void *client, uint8_t *data, int data_len);

/**
 * @brief Recv data from http server.
 *
 * @param[in,out] client pointer to http client
 * @param[out] buf buffer to store recv data
 * @param[in] buf_len buffer len
 * @param timeout_ms timeout for recv
 * @return >= 0 for recv data len. others @see IotReturnCode
 */
int IOT_HTTP_Recv(void *client, uint8_t *buf, int buf_len, uint32_t timeout_ms);

/**
 * @brief Check is recv finished.
 *
 * @param[in,out] client pointer to http client
 * @return true for finished.
 */
int IOT_HTTP_IsRecvFinished(void *client);

/**
 * @brief Disconnect http server.
 *
 * @param[in,out] client pointer to http client
 */
void IOT_HTTP_Disconnect(void *client);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_HTTP_CLIENT_H_
