/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

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

#define HTTP_PORT   80
#define HTTPS_PORT  443

typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_HEAD
} HttpMethod;

typedef struct {
    int         remote_port;        // 端口号
    int         response_code;      // 响应码
    char        *header;            // 自定义头部
    char        *auth_user;         // 身份验证的用户名
    char        *auth_password;     // 身份验证的密码
    qcloud_network_t     network;      
} HTTPClient;

typedef struct {
    bool    is_more;                // 是否需要检索更多的数据
    bool    is_chunked;             // 响应数据是否以分块进行编码
    int     retrieve_len;           // 要检索的内容长度
    int     response_content_len;   // 响应内容长度
    int     post_buf_len;           // post data length
    int     response_buf_len;       // 响应包缓冲区长度
    char    *post_content_type;     // post数据的内容类型
    char    *post_buf;              // post的数据
    char    *response_buf;          // 存储响应数据的缓冲区
} HTTPClientData;

/**
 * @brief http 网络请求
 *
 * @param client        http client
 * @param url           请求url
 * @param port          请求端口
 * @param ca_crt_dir    ca证书路径
 * @param method        请求方法
 * @param client_data   http数据负载
 * @return              返回QCLOUD_ERR_SUCCESS, 表示设置成功
 */
int qcloud_http_client_common(HTTPClient *client, const char *url, int port, const char *ca_crt, HttpMethod method, HTTPClientData *client_data);

int qcloud_http_recv_data(HTTPClient *client, uint32_t timeout_ms, HTTPClientData *client_data);
    
int qcloud_http_client_connect(HTTPClient *client, const char *url, int port, const char *ca_crt);

void qcloud_http_client_close(HTTPClient *client);


#ifdef __cplusplus
}
#endif
#endif /* QCLOUD_IOT_UTILS_HTTPC_H_ */
