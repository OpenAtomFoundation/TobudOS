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

#ifndef _QCLOUD_COAP_H_
#define _QCLOUD_COAP_H_

#ifdef __cplusplus
extern "C" {
#endif

static const char *qcloud_coap_server = "iotcloud.tencentdevices.com";

static const int qcloud_coap_port = 5684;

typedef enum qcloud_coap_auth_state_en {
    QCLOUD_COAP_AUTH_STATE_NONE,
    QCLOUD_COAP_AUTH_STATE_SUCCESS,
    QCLOUD_COAP_AUTH_STATE_FAIL,
} qcloud_coap_auth_state_t;

/**
 *  @brief COAP消息分类
 */
typedef enum coap_code_class_en {
    COAP_CODE_CLASS_REQ = 0, // 请求包
    COAP_CODE_CLASS_SUCCESS = 2, // 成功回包
    COAP_CODE_CLASS_CLIENT_ERR = 4,  // 客户端错误回包
    COAP_CODE_CLASS_SERVER_ERR = 5,  // 后台错误回包
    COAP_CODE_CLASS_INTERNAL_ERR = 6,
} coap_code_class_t;

/**
 *  @brief COAP消息请求方法
 */
typedef enum coap_request_method_en {
    COAP_REQUEST_METHOD_GET = 1,   // GET请求
    COAP_REQUEST_METHOD_POST = 2,  // POST请求
    COAP_REQUEST_METHOD_PUT = 3,
    COAP_REQUEST_METHOD_DELETE = 4,
} coap_request_method_t;

typedef enum coap_code_detail_en {
    /* CoAP Success Response code detail */
    COAP_CODE_DETAIL_201_CREATED                    = 01,  /* Mapping to CoAP codeClass.codeDetail 2.01 */
    COAP_CODE_DETAIL_202_DELETED                    = 02,  /* Mapping to CoAP codeClass.codeDetail 2.02 */
    COAP_CODE_DETAIL_203_VALID                      = 03,  /* Mapping to CoAP codeClass.codeDetail 2.03 */
    COAP_CODE_DETAIL_204_CHANGED                    = 04,  /* Mapping to CoAP codeClass.codeDetail 2.04 */
    COAP_CODE_DETAIL_205_CONTENT                    = 05,  /* Mapping to CoAP codeClass.codeDetail 2.05 */
    COAP_CODE_DETAIL_231_CONTINUE                   = 31,  /* Mapping to CoAP codeClass.codeDetail 2.31 */

    /* CoAP Client Error Response code detail */
    COAP_CODE_DETAIL_400_BAD_REQUEST                = 00,  /* Mapping to CoAP codeClass.codeDetail 4.00 */
    COAP_CODE_DETAIL_401_UNAUTHORIZED               = 01,  /* Mapping to CoAP codeClass.codeDetail 4.01 */
    COAP_CODE_DETAIL_402_BAD_OPTION                 = 02,  /* Mapping to CoAP codeClass.codeDetail 4.02 */
    COAP_CODE_DETAIL_403_FORBIDDEN                  = 03,  /* Mapping to CoAP codeClass.codeDetail 4.03 */
    COAP_CODE_DETAIL_404_NOT_FOUND                  = 04,  /* Mapping to CoAP codeClass.codeDetail 4.04 */
    COAP_CODE_DETAIL_405_METHOD_NOT_ALLOWED         = 05,  /* Mapping to CoAP codeClass.codeDetail 4.05 */
    COAP_CODE_DETAIL_406_NOT_ACCEPTABLE             = 06,  /* Mapping to CoAP codeClass.codeDetail 4.06 */
    COAP_CODE_DETAIL_408_REQUEST_ENTITY_INCOMPLETE  = 8,   /* Mapping to CoAP codeClass.codeDetail 4.08 */
    COAP_CODE_DETAIL_412_PRECONDITION_FAILED        = 12,  /* Mapping to CoAP codeClass.codeDetail 4.12 */
    COAP_CODE_DETAIL_413_REQUEST_ENTITY_TOO_LARGE   = 13,  /* Mapping to CoAP codeClass.codeDetail 4.13 */
    COAP_CODE_DETAIL_415_UNSUPPORTED_CONTENT_FORMAT = 15,  /* Mapping to CoAP codeClass.codeDetail 4.15 */

    /* CoAP Server Error Response code detail */
    COAP_CODE_DETAIL_500_INTERNAL_SERVER_ERROR      = 00,  /* Mapping to CoAP codeClass.codeDetail 5.00 */
    COAP_CODE_DETAIL_501_NOT_IMPLEMENTED            = 01,  /* Mapping to CoAP codeClass.codeDetail 5.01 */
    COAP_CODE_DETAIL_502_BAD_GATEWAY                = 02,  /* Mapping to CoAP codeClass.codeDetail 5.02 */
    COAP_CODE_DETAIL_503_SERVICE_UNAVAILABLE        = 03,  /* Mapping to CoAP codeClass.codeDetail 5.03 */
    COAP_CODE_DETAIL_504_GATEWAY_TIMEOUT            = 04,  /* Mapping to CoAP codeClass.codeDetail 5.04 */
    COAP_CODE_DETAIL_505_PROXYING_NOT_SUPPORTED     = 05,  /* Mapping to CoAP codeClass.codeDetail 5.05 */

    COAP_CODE_DETAIL_600_TIMEOUT					 = 00,  /* Mapping to self define CoAP codeClass.codeDetail 6.00 */
} coap_code_detail_t;

/**
 *  @brief Option number enumeration
 */
typedef enum coap_message_option_code_en {
    COAP_MSG_OPTION_CODE_IF_MATCH = 1,       // If-Match option number
    COAP_MSG_OPTION_CODE_URI_HOST = 3,       // URI-Host option number
    COAP_MSG_OPTION_CODE_ETAG = 4,           // Entity-Tag option number
    COAP_MSG_OPTION_CODE_IF_NONE_MATCH = 5,  // If-None-Match option number
    COAP_MSG_OPTION_CODE_URI_PORT = 7,       // URI-Port option number
    COAP_MSG_OPTION_CODE_LOCATION_PATH = 8,  // Location-Path option number
    COAP_MSG_OPTION_CODE_URI_PATH = 11,      // URI-Path option number
    COAP_MSG_OPTION_CODE_CONTENT_FORMAT = 12,    // Content-Format option number
    COAP_MSG_OPTION_CODE_MAX_AGE = 14,       // Max-Age option number
    COAP_MSG_OPTION_CODE_URI_QUERY = 15,     // URI-Query option number
    COAP_MSG_OPTION_CODE_ACCEPT = 17,        // Accept option number
    COAP_MSG_OPTION_CODE_LOCATION_QUERY = 20,    // Location-Query option number
    COAP_MSG_OPTION_CODE_BLOCK2 = 23,        // Block2 option number
    COAP_MSG_OPTION_CODE_BLOCK1 = 27,        // Block1 option number
    COAP_MSG_OPTION_CODE_SIZE2 = 28,         // Size2 option number
    COAP_MSG_OPTION_CODE_PROXY_URI = 35,     // Proxy-URI option number
    COAP_MSG_OPTION_CODE_PROXY_SCHEME = 39,  // Proxy-Scheme option number
    COAP_MSG_OPTION_CODE_SIZE1 = 60,     // Size1 option number
    COAP_MSG_OPTION_CODE_AUTH_TOKEN = 61,    // 设备鉴权token option number
    COAP_MSG_OPTION_CODE_NEED_RESP = 62, // CoAP消息是否需要content response option number
} coap_msg_opt_code_t;

typedef enum coap_incoming_msg_type_en {
    COAP_INCOMING_MSG_TYPE_PIGGY,
    COAP_INCOMING_MSG_TYPE_ACK,
    COAP_INCOMING_MSG_TYPE_RESP,
} coap_incoming_msg_type_t;

/**
 * @brief CoAP needResp类型消息回调处理函数指针定义
 *
 * @param message CoAP消息
 * @param userContext 发送消息时携带的用户上下文
 */
typedef void (*coap_resp_callback_t)(void *message, void *context);

/**
 * @brief COAP发布消息结构体
 */
typedef struct coap_send_option_st {
    char                   *payload;    // COAP 消息负载
    size_t                  payload_len;    // MQTT 消息负载长度
    void                   *context;    // 用于用户传递的消息上下文，SDK不做处理在callback时返回
    coap_resp_callback_t    resp_cb;   // 消息回包callback，仅对need_resp: true 有效
} coap_send_opt_t;

/**
 *  @brief CoAP Option结构体
 */
typedef struct coap_message_option_st {
    qcloud_list_t       list;
    uint16_t            option_code; // Option code
    uint32_t            val_len;    // Option length
    char               *val;    // Pointer to a buffer containing the option value
} coap_msg_option_t;

typedef struct coap_message_sent_info_st {
    qcloud_list_t           list;
    void                   *context;
    int                     is_acked;
    uint8_t                 token_len;
    uint8_t                 token[COAP_MSG_TOKEN_MAX];
    uint8_t                 transmit_count;
    osal_timer_t            timer;
    void                   *message;
    uint16_t                message_id;
    uint32_t                message_len;
    coap_resp_callback_t    resp_cb;
} coap_msg_sent_info_t;

/**
 *  @brief COAP消息类型
 */
typedef enum coap_msg_type_en {
    COAP_MSG_TYPE_CON = 0x0,    /**< 需要等待回包确认的消息 */
    COAP_MSG_TYPE_NON = 0x1,    /**< 无需等待回包确认的消息 */
    COAP_MSG_TYPE_ACK = 0x2,    /**< ACK消息 */
    COAP_MSG_TYPE_RST = 0x3,    /**< Reset消息 */
} coap_msg_type_t;

/**
 *  @brief CoAP消息结构体
 */
typedef struct coap_message_st {
    uint32_t                version;    // CoAP协议版本号
    uint8_t                 type;   // 消息类型

    uint32_t                code_class; // Code class
    uint32_t                code_detail;    // Code detail

    uint16_t                id; // 消息id

    char                   *payload;    // 消息负载
    size_t                  payload_len;    // 消息负载长度

    char                    token[COAP_MSG_TOKEN_MAX];  // 消息token
    uint8_t                 token_len;  // 消息token长度

    coap_resp_callback_t    resp_cb;    // CoAP Response消息回调处理函数
    void                   *context;   // 用户上下文

    qcloud_list_t           option_list;
} coap_message_t;

#define COAP_MESSAGE_INITIALIZER    {COAP_VERSION, COAP_MSG_TYPE_CON, COAP_CODE_CLASS_REQ, COAP_REQUEST_METHOD_POST, 0, NULL, 0, {0}, 0, NULL, NULL, {NULL, NULL}}

/**
 * CoAP 事件类型
 */
typedef enum coap_event_type_en {
    COAP_EVENT_TYPE_RECEIVE_ACK = 0,    // 收到消息ACK

    COAP_EVENT_TYPE_RECEIVE_RESPCONTENT = 1,    // 收到消息回包响应

    COAP_EVENT_TYPE_UNAUTHORIZED = -1,  // 设备鉴权失败或鉴权token过期

    COAP_EVENT_TYPE_FORBIDDEN = -2, // 设备topic无权限

    COAP_EVENT_TYPE_INTERNAL_SERVER_ERROR = -3, // 服务端错误

    COAP_EVENT_TYPE_ACK_TIMEOUT = -4,   // 消息ACK超时

    COAP_EVENT_TYPE_SEPRESP_TIMEOUT = -5,   // 消息回包响应超时
} coap_event_type_t;

typedef struct coap_event_st {
    coap_event_type_t   type; /* 事件类型 */
    void               *message; /* 事件消息 or 内容消息 */
} coap_event_t;

typedef void (*coap_event_handler_fn_t)(void *context, coap_event_t *event);

typedef struct coap_event_handler_st {
    coap_event_handler_fn_t handler;
    void                   *context;
} coap_event_handler_t;

typedef struct qcloud_coap_client_st {
    qcloud_coap_auth_state_t    auth_state;

    uint32_t                    message_token;  // 标识请求响应条消息对应

    uint16_t                    packet_id;  // COAP报文标识符

    uint8_t                     tx_buffer[QCLOUD_COAP_CLIENT_TX_BUF_LEN];
    uint8_t                     rx_buffer[QCLOUD_COAP_CLIENT_RX_BUF_LEN];

    void                       *tx_lock;    // 输出流的锁
    void                       *message_list_lock; // 等待发布消息ack列表的锁
    qcloud_list_t               message_list;   // 请求消息列表

    qcloud_network_t            network;    // 网络调用数据结构

    uint32_t                    command_timeout;    // CoAP消息超时时间, 单位:ms

    uint8_t                     retransmit_max;  // CoAP消息最大重试次数

    coap_event_handler_t        event_handler;   // 事件回调

	char                       *auth_token; // 鉴权token
	int                         auth_token_len;

    char                        auth_id[QCLOUD_COAP_AUTH_ID_MAX + 1];

    char                        auth_uri[QCLOUD_COAP_AUTH_URI_MAX + 1];
} qcloud_coap_client_t;

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_create(qcloud_coap_client_t *client, qcloud_device_t *device, coap_event_handler_fn_t handler);

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_destroy(qcloud_coap_client_t *client);

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_connect(qcloud_coap_client_t *client);

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_yield(qcloud_coap_client_t *client, uint32_t timeout_ms);

__QCLOUD_API__ qcloud_err_t qcloud_coap_client_msg_send(qcloud_coap_client_t *client,
                                                                            char *topic,
                                                                            coap_send_opt_t *send_opt);

__QCLOUD_API__ uint16_t qcloud_coap_msg_id_get(coap_message_t *message);

__QCLOUD_API__ qcloud_err_t qcloud_coap_msg_payload_get(coap_message_t *message, char **payload, int *payload_len);

__QCLOUD_API__ coap_event_type_t qcloud_coap_event_type_get(coap_message_t *message);

__QCLOUD_INTERNAL__ int coap_message_token_get(qcloud_coap_client_t *client, char *buf);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_type_set(coap_message_t *message, uint8_t type);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_code_set(coap_message_t *message, uint32_t code_class, uint32_t code_detail);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_id_set(coap_message_t *message, uint16_t id);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_token_set(coap_message_t *message, char *buf, uint8_t len);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_payload_set(coap_message_t *message, char *buf, size_t len);

__QCLOUD_INTERNAL__ coap_msg_option_t *coap_message_option_construct(uint16_t option_code, uint32_t len, const char *val);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_option_add(coap_message_t *message, coap_msg_opt_code_t option_code, uint32_t len, const char *val);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_callback_set(coap_message_t *message, coap_resp_callback_t callback);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_context_set(coap_message_t *message, void *context);

__QCLOUD_INTERNAL__ void coap_message_init(coap_message_t *message);

__QCLOUD_INTERNAL__ void coap_message_destroy(coap_message_t *message);

__QCLOUD_INTERNAL__ void coap_message_dump(coap_message_t* message);

__QCLOUD_INTERNAL__ void coap_glue_connect_id_generate(char *conn_id);

__QCLOUD_INTERNAL__ uint16_t coap_glue_packet_id_generate(qcloud_coap_client_t *client);

__QCLOUD_INTERNAL__ qcloud_err_t coap_glue_message_list_destroy(qcloud_coap_client_t *client);

__QCLOUD_INTERNAL__ qcloud_err_t coap_glue_spin(qcloud_coap_client_t *client, uint32_t timeout_ms);

__QCLOUD_INTERNAL__ qcloud_err_t coap_glue_msg_send(qcloud_coap_client_t *client, coap_message_t *message);

__QCLOUD_INTERNAL__ qcloud_err_t coap_message_deserialize(coap_message_t *message, char *buf, size_t buf_len);

__QCLOUD_INTERNAL__ int coap_message_serialize(coap_message_t *message, char *buf, size_t buf_len);

__QCLOUD_INTERNAL__ qcloud_err_t coap_auth(qcloud_coap_client_t *client, char *connection_id);

#ifdef __cplusplus
}
#endif

#endif

