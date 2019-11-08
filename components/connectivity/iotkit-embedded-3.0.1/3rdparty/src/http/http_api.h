/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOT_EXPORT_HTTP_H_
#define _IOT_EXPORT_HTTP_H_

#include "infra_types.h"
#include "infra_defs.h"

#if defined(__cplusplus)
extern "C" {
#endif


typedef struct {
    char        product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char        device_name[IOTX_DEVICE_NAME_LEN + 1];
    char        device_id[IOTX_DEVICE_ID_LEN + 1];
    char        device_secret[IOTX_DEVICE_SECRET_LEN + 1];
    char        module_vendor_id[IOTX_MODULE_ID_LEN + 1];
} iotx_device_info_t;

/* IoTx http initial param */
typedef struct {
    iotx_device_info_t *device_info;
    int                 keep_alive;
    int                 timeout_ms;
} iotx_http_param_t;

/* IoTx http context */
typedef struct {
    char               *p_auth_token;
    uint32_t           auth_token_len;
    char                is_authed;
    const char         *version;
    const char         *signmethod;
    const char         *sign;
    iotx_device_info_t *p_devinfo;
    const char         *timestamp;
    void               *httpc;
    int                 keep_alive;
    int                 timeout_ms;
} iotx_http_t, *iotx_http_pt;

/* IoTx http message definition
 * request_payload and response_payload need to be allocate in order to save memory.
 * topic_path specify the topic url you want to publish message.
 */
typedef struct {
    char       *topic_path;
    uint32_t   request_payload_len;
    char       *request_payload;
    uint32_t   response_payload_len;
    char       *response_payload;
    uint32_t   timeout_ms;
} iotx_http_message_param_t;

/* The response code from sever */
typedef enum {
    IOTX_HTTP_SUCCESS = 0,
    IOTX_HTTP_COMMON_ERROR = 10000,
    IOTX_HTTP_PARAM_ERROR = 10001,
    IOTX_HTTP_AUTH_CHECK_ERROR = 20000,
    IOTX_HTTP_TOKEN_EXPIRED_ERROR = 20001,
    IOTX_HTTP_TOKEN_NULL_ERROR = 20002,
    IOTX_HTTP_TOKEN_CHECK_ERROR = 20003,
    IOTX_HTTP_UPDATE_SESSION_ERROR = 20004,
    IOTX_HTTP_PUBLISH_MESSAGE_ERROR = 30001,
    IOTX_HTTP_REQUEST_TOO_MANY_ERROR = 40000,
} iotx_http_upstream_response_t;

/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_api_http http
 *  @{
 */

/**
 * @brief   Initialize the HTTP client
 *        This function initialize the data.
 *
 * @param [in] pInitParams: Specify the init param infomation.
 *
 * @retval NULL     : Initialize failed.
 * @retval NOT_NULL : The contex of HTTP client.
 * @see None.
 */
void   *IOT_HTTP_Init(iotx_http_param_t *pInitParams);

/**
 * @brief   De-initialize the HTTP client
 *        This function release the related resource.
 *
 * @param [in] handle: pointer to http context pointer.
 * @return None.
 * @see None.
 */
void    IOT_HTTP_DeInit(void **handle);

/**
 * @brief   Handle device name authentication with remote server.
 *
 * @param [in] handle: Pointer of context, specify the HTTP client.
 *
 * @retval  0 : Authenticate success.
 * @retval -1 : Authenticate failed.
 * @see iotx_err_t.
 */
int     IOT_HTTP_DeviceNameAuth(void *handle);

/**
 * @brief   Send a message with specific path to server.
 *        Client must authentication with server before send message.
 *
 * @param [in] handle: Pointer of contex, specify the HTTP client.
 * @param [in] msg_param: Specify the topic path and http payload configuration.
 *
 * @retval  0 : Success.
 * @retval -1 : Failed.
 * @see iotx_err_t.
 */
int     IOT_HTTP_SendMessage(void *handle, iotx_http_message_param_t *msg_param);

/**
 * @brief   close tcp connection from client to server.
 *
 * @param [in] handle: Pointer of contex, specify the HTTP client.
 * @return None.
 * @see None.
 */
void     IOT_HTTP_Disconnect(void *handle);

/** @} */ /* end of api_http */
/** @} */ /* end of api */

#if defined(__cplusplus)
}
#endif
#endif
