/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_UTILS_HTTPC_H_
#define QCLOUD_IOT_UTILS_HTTPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "network_interface.h"

#define HTTP_PORT  80
#define HTTPS_PORT 443

typedef enum { HTTP_GET, HTTP_POST, HTTP_PUT, HTTP_DELETE, HTTP_HEAD } HttpMethod;

typedef struct {
    int     remote_port;
    int     response_code;
    char *  header;
    char *  auth_user;
    char *  auth_password;
    Network network_stack;
} HTTPClient;

typedef struct {
    bool  is_more;               // if more data to check
    bool  is_chunked;            // if response in chunked data
    int   retrieve_len;          // length of retrieve
    int   response_content_len;  // length of resposne content
    int   post_buf_len;          // post data length
    int   response_buf_len;      // length of response data buffer
    char *post_content_type;     // type of post content
    char *post_buf;              // post data buffer
    char *response_buf;          // response data buffer
} HTTPClientData;

/**
 * @brief do one http request
 *
 * @param client        http client
 * @param url           server url
 * @param port          server port
 * @param ca_crt_dir    ca path
 * @param method        type of request
 * @param client_data   http data
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_http_client_common(HTTPClient *client, const char *url, int port, const char *ca_crt, HttpMethod method,
                              HTTPClientData *client_data);

int qcloud_http_recv_data(HTTPClient *client, uint32_t timeout_ms, HTTPClientData *client_data);

int qcloud_http_client_connect(HTTPClient *client, const char *url, int port, const char *ca_crt);

void qcloud_http_client_close(HTTPClient *client);

#ifdef __cplusplus
}
#endif
#endif /* QCLOUD_IOT_UTILS_HTTPC_H_ */
