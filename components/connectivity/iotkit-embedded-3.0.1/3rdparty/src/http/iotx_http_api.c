/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "infra_httpc.h"
#include "infra_json_parser.h"
#include "infra_timer.h"
#include "infra_sha1.h"
#include "infra_report.h"
#include "http_debug.h"
#include "http_api.h"
#include "http_wrapper.h"



#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define HTTP_API_MALLOC(size)               LITE_malloc(size, MEM_MAGIC, "http.api")
    #define HTTP_API_FREE(ptr)                  LITE_free(ptr)
#else
    #define HTTP_API_MALLOC(size)               HAL_Malloc(size)
    #define HTTP_API_FREE(ptr)                  {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#define HTTP_LITE_JSON_VALUE_OF(key, src)       LITE_json_value_of(key, src, 0x1234, "http.api")

#ifndef CONFIG_MID_HTTP_TIMEOUT
    #define CONFIG_MID_HTTP_TIMEOUT             (5 * 1000)
#endif

#ifndef CONFIG_HTTP_AUTH_TIMEOUT
    #define CONFIG_HTTP_AUTH_TIMEOUT            (5 * 1000)
#endif

/*
#define IOTX_HTTP_TIMESTAMP_OPTIONAL_ENABLE
*/
#define IOTX_HTTP_SIGN_LENGTH       (41)
#define IOTX_HTTP_SIGN_SOURCE_LEN   (256)
#define IOTX_HTTP_AUTH_TOKEN_LEN    (192+1)
#define IOTX_HTTP_URL_LEN_MAX       (135)

#ifdef IOTX_HTTP_TIMESTAMP_OPTIONAL_ENABLE
#define IOTX_HTTP_SIGN_SRC_STR          "clientId%sdeviceName%sproductKey%stimestamp%s"
#define IOTX_HTTP_AUTH_DEVICENAME_STR   \
    "{" \
    "\"version\":\"%s\", \"clientId\":\"%s\"," \
    "\"signmethod\":\"%s\",\"sign\":\"%s\"," \
    "\"productKey\":\"%s\",\"deviceName\":\"%s\"," \
    "\"timestamp\":\"%s\"" \
    "}"
#else
#define IOTX_HTTP_SIGN_SRC_STR          "clientId%sdeviceName%sproductKey%s"
#define IOTX_HTTP_AUTH_DEVICENAME_STR   \
    "{" \
    "\"version\":\"%s\", \"clientId\":\"%s\"," \
    "\"signmethod\":\"%s\",\"sign\":\"%s\"," \
    "\"productKey\":\"%s\",\"deviceName\":\"%s\"" \
    "}"
#endif

#define IOTX_HTTP_AUTH_STR              "auth"
#define IOTX_HTTP_ONLINE_SERVER_URL     "https://iot-as-http.cn-shanghai.aliyuncs.com"
#define IOTX_HTTP_ONLINE_SERVER_PORT    443

#define IOTX_SHA_METHOD                 "hmacsha1"

#define IOTX_HTTP_HEADER_KEEPALIVE_STR  "Connection: Keep-Alive\r\n"
#define IOTX_HTTP_HEADER_PASSWORD_STR   "password:"
#define IOTX_HTTP_UPSTREAM_HEADER_STR   \
    IOTX_HTTP_HEADER_KEEPALIVE_STR \
    IOTX_HTTP_HEADER_PASSWORD_STR \
    "%s" \
    IOTX_HTTP_HEADER_END_STR
#define IOTX_HTTP_HEADER_END_STR "\r\n"

#define HTTP_AUTH_RESP_MAX_LEN      (256)

static iotx_http_t *iotx_http_context_bak = NULL;


/*
  Http server url: https://iot-as-http.cn-shanghai.aliyuncs.com
  Only https protocal is supported at present.
  The /auth interface is used to get access token before any data transform which need to be requested just one time.
  The device must call /auth to get the token before transfer any data.
  Every data transfer need to be with access token.
  You need to request for a new token if the current token have been expired.
  Token can be cached in local devices, which has 48 hours term of validity.
  POST /auth HTTP/1.1
  Host: iot-as-http.cn-shanghai.aliyuncs.com
  Content-Type: application/json
  body: {"version":"default","clientId":"xxxxx","signmethod":"hmacsha1","sign":"xxxxxxxxxx","productKey":"xxxxxx","deviceName":"xxxxxxx","timestamp":"xxxxxxx"}
*/

static int iotx_calc_sign(const char *p_device_secret, const char *p_msg, char *sign)
{
    http_info("| method: %s", IOTX_SHA_METHOD);
    utils_hmac_sha1(p_msg, strlen(p_msg), sign, p_device_secret, strlen(p_device_secret));
    return SUCCESS_RETURN;
}

static int calc_snprintf_string_length(char *fmt, ...)
{
    va_list args;
    int     rc = 0;
    char *p = NULL;
    char *sval = NULL;

    if (NULL == fmt) {
        return -1;
    }

    va_start(args, fmt);

    for (p = fmt; *p; p++) {
        if (*p != '%') {
            rc++;
            continue;
        }
        switch (*++p) {
            case 's':
                for (sval = va_arg(args, char *); *sval; sval++) {
                    rc++;
                }
                break;
            default:
                rc++;
                break;
        }
    }

    va_end(args);

    return rc;
}

static int construct_full_http_authenticate_url(char *buf)
{
    LITE_snprintf(buf, IOTX_HTTP_URL_LEN_MAX,
                  "%s/%s", IOTX_HTTP_ONLINE_SERVER_URL, IOTX_HTTP_AUTH_STR);
    http_info("get_http_authenticate_url is %s", buf);
    return 0;
}

static int construct_full_http_upstream_url(char *buf, const char *topic_path)
{
    LITE_snprintf(buf, IOTX_HTTP_URL_LEN_MAX,
                  "%s%s", IOTX_HTTP_ONLINE_SERVER_URL, topic_path);
    http_info("construct_full_http_upstream_url is %s", buf);
    return 0;
}

static int http_report_func(void *handle, const char *topic_name, int req_ack, void *data, int len)
{
    iotx_http_message_param_t   msg_param;
    char                        request_buf[1024];
    char                        topic_path[100];

    if (handle == NULL || topic_name == NULL || data == NULL) {
        http_err("params err");
        return -1;
    }

    HAL_Snprintf(topic_path, sizeof(topic_path), "/topic%s", topic_name);

    memset(&msg_param, 0, sizeof(iotx_http_message_param_t));
    msg_param.request_payload = (char *)data;
    msg_param.response_payload = request_buf;
    msg_param.timeout_ms = CONFIG_MID_HTTP_TIMEOUT;
    msg_param.request_payload_len = len;
    msg_param.response_payload_len = 1024;
    msg_param.topic_path = topic_path;

    return IOT_HTTP_SendMessage(handle, &msg_param);
}

static void *verify_iotx_http_context(void *handle)
{
    iotx_http_t *iotx_http_context = (iotx_http_t *)handle;

    if (NULL == iotx_http_context ||
        NULL == iotx_http_context_bak ||
        iotx_http_context_bak != iotx_http_context) {
        http_err("iotx_http_context not valid pointer!");

        iotx_http_context =  NULL;
    }

    return iotx_http_context;
}

void *IOT_HTTP_Init(iotx_http_param_t *pInitParams)
{
    iotx_device_info_t *p_devinfo;
    iotx_http_t        *iotx_http_context;

    /* currently http is singleton, init twice not allowed. */
    if (NULL != iotx_http_context_bak) {
        http_err("Init twice not allowed, please deinit first");
        return NULL;
    }

    if (NULL == pInitParams || NULL == pInitParams->device_info) {
        http_err("Invalid argument: pInitParams or device_info = NULL");
        return NULL;
    }

    p_devinfo = pInitParams->device_info;

/*
    HAL_SetProductKey(p_devinfo->product_key);
    HAL_SetDeviceName(p_devinfo->device_name);
    HAL_SetDeviceSecret(p_devinfo->device_secret);
*/

    iotx_http_context = (iotx_http_t *)HTTP_API_MALLOC(sizeof(iotx_http_t));

    if (NULL == iotx_http_context) {
        http_err("Allocate memory for iotx_http_context failed");
        return NULL;
    }

    memset(iotx_http_context, 0x00, sizeof(iotx_http_t));

    iotx_http_context->keep_alive = pInitParams->keep_alive;
    iotx_http_context->timeout_ms = pInitParams->timeout_ms;
    iotx_http_context->p_auth_token = HTTP_API_MALLOC(IOTX_HTTP_AUTH_TOKEN_LEN);
    if (NULL == iotx_http_context->p_auth_token) {
        http_err("Allocate memory for auth token failed");
        goto err;
    }
    memset(iotx_http_context->p_auth_token, 0x00, IOTX_HTTP_AUTH_TOKEN_LEN);
    iotx_http_context->is_authed = 0;
    iotx_http_context->auth_token_len = IOTX_HTTP_AUTH_TOKEN_LEN;

    /*Get deivce information*/
    iotx_http_context->p_devinfo = (iotx_device_info_t *)HTTP_API_MALLOC(sizeof(iotx_device_info_t));
    if (NULL == iotx_http_context->p_devinfo) {
        http_err("Allocate memory for iotx_http_context->p_devinfo failed");
        goto err;
    }
    memset(iotx_http_context->p_devinfo, 0x00, sizeof(iotx_device_info_t));

    /*It should be implement by the user*/
    memset(iotx_http_context->p_devinfo, 0x00, sizeof(iotx_device_info_t));
    strncpy(iotx_http_context->p_devinfo->device_id,     p_devinfo->device_id,     strlen(p_devinfo->device_id));
    strncpy(iotx_http_context->p_devinfo->product_key,   p_devinfo->product_key,   strlen(p_devinfo->product_key));
    strncpy(iotx_http_context->p_devinfo->device_secret, p_devinfo->device_secret, strlen(p_devinfo->device_secret));
    strncpy(iotx_http_context->p_devinfo->device_name,   p_devinfo->device_name,   strlen(p_devinfo->device_name));

    iotx_http_context->httpc = HTTP_API_MALLOC(sizeof(httpclient_t));
    if (NULL == iotx_http_context->httpc) {
        http_err("Allocate memory for iotx_http_context->httpc failed");
        goto err;
    }
    memset(iotx_http_context->httpc, 0x00, sizeof(httpclient_t));

    iotx_http_context_bak = iotx_http_context;

    return iotx_http_context;
err:
    /* Error, release the memory */
    if (NULL != iotx_http_context) {
        if (NULL != iotx_http_context->p_devinfo) {
            HTTP_API_FREE(iotx_http_context->p_devinfo);
        }
        if (NULL != iotx_http_context->p_auth_token) {
            HTTP_API_FREE(iotx_http_context->p_auth_token);
        }

        iotx_http_context->auth_token_len = 0;
        HTTP_API_FREE(iotx_http_context);
    }
    return NULL;
}

void IOT_HTTP_DeInit(void **handle)
{
    iotx_http_t *iotx_http_context;
    if (NULL == handle) {
        http_err("handle is NULL pointer");
        return;
    }
    if (NULL == (iotx_http_context = verify_iotx_http_context(*handle))) {
        return;
    }

    if (NULL != iotx_http_context->p_devinfo) {
        HTTP_API_FREE(iotx_http_context->p_devinfo);
    }
    if (NULL != iotx_http_context->p_auth_token) {
        HTTP_API_FREE(iotx_http_context->p_auth_token);
    }
    if (NULL != iotx_http_context->httpc) {
        HTTP_API_FREE(iotx_http_context->httpc);
    }

    iotx_http_context->auth_token_len = 0;
    HTTP_API_FREE(iotx_http_context);
    iotx_http_context_bak = NULL;
}

int IOT_HTTP_DeviceNameAuth(void *handle)
{
    int                 ret = -1;
    int                 ret_code = 0;
    char               *pvalue = NULL;
    char               *response_message = NULL;
    char                sign[IOTX_HTTP_SIGN_LENGTH]   = {0};
    char                http_url[IOTX_HTTP_URL_LEN_MAX] = {0};
    char                timestamp[14] = {0};
    httpclient_t       *httpc;
    httpclient_data_t   httpc_data = {0};
    char               *req_payload = NULL;
    char               *rsp_payload = NULL;
    int                 len = 0;
    char                p_msg_unsign[IOTX_HTTP_SIGN_SOURCE_LEN] = {0};
    iotx_time_t         timer;
    iotx_http_t        *iotx_http_context;
    const char          *pub_key = NULL;
    /* */
    /* body: */
    /* { */
    /* "version": "default",//默认default */
    /* "clientId": "xxxxxx",//必填 */
    /* "signmethod": "hmacsha1",//只支持hmacmd5和hmacsha1，默认hmacmd5 */
    /* "sign": "xxxxxxxxxxxxx",//必填，signmethod(deviceSecret,content), content = 将所有提交给服务器的参数（version,sign,signmethod除外）, 按照字母顺序排序, 然后将参数值依次拼接，无拼接符号 */
    /* "productKey": "xxxxxx",//必填 */
    /* "deviceName": "xxxxxx",//必填 */
    /* "timestamp": "xxxxxx"//选填  13byte */
    /* } */

    if (NULL == (iotx_http_context = verify_iotx_http_context(handle))) {
        goto do_exit;
    }
    iotx_http_context->is_authed = 0;

    /* FIXME:some compile error when calling this function. Get TimeStamp */
    /*
    if(!utils_get_epoch_time_from_ntp(timestamp, sizeof(timestamp)))
    {
            http_info("http time response: \r\n\r\n%s", timestamp);
            goto do_exit;
    }
    */

    /* Calculate Sign */
    LITE_snprintf(p_msg_unsign, IOTX_HTTP_SIGN_SOURCE_LEN,
                  IOTX_HTTP_SIGN_SRC_STR,
                  iotx_http_context->p_devinfo->device_id,
                  iotx_http_context->p_devinfo->device_name,
                  iotx_http_context->p_devinfo->product_key
#ifdef IOTX_HTTP_TIMESTAMP_OPTIONAL_ENABLE
                  , timestamp
#endif
                 );

    iotx_calc_sign(iotx_http_context->p_devinfo->device_secret, p_msg_unsign, sign);

    /* to save stack memory*/
    len = calc_snprintf_string_length(IOTX_HTTP_AUTH_DEVICENAME_STR,
                                      "default",
                                      iotx_http_context->p_devinfo->device_id,
                                      IOTX_SHA_METHOD,
                                      sign,
                                      iotx_http_context->p_devinfo->product_key,
                                      iotx_http_context->p_devinfo->device_name,
                                      timestamp
                                     );

    if (len < 0) {
        goto do_exit;
    }

    req_payload = (char *)HTTP_API_MALLOC(len + 1);
    memset(req_payload, 0, len + 1);

    http_debug("allocate req_payload: len = %d", len);

    len = HAL_Snprintf(req_payload, len + 1,
                       IOTX_HTTP_AUTH_DEVICENAME_STR,
                       "default",
                       iotx_http_context->p_devinfo->device_id,
                       IOTX_SHA_METHOD,
                       sign,
                       iotx_http_context->p_devinfo->product_key,
                       iotx_http_context->p_devinfo->device_name,
                       timestamp
                      );
    http_debug("len = %d, req_payload: \r\n%s", len, req_payload);

    /* Malloc Http Response Payload */
    rsp_payload = (char *)HTTP_API_MALLOC(HTTP_AUTH_RESP_MAX_LEN);
    if (NULL == rsp_payload) {
        http_err("Allocate HTTP rsp_payload buf failed!");
        goto do_exit;
    }
    memset(rsp_payload, 0, HTTP_AUTH_RESP_MAX_LEN);

    /* Construct Auth Url */
    construct_full_http_authenticate_url(http_url);

    /* Set httpclient and httpclient_data */
    httpc = (httpclient_t *)iotx_http_context->httpc;

    httpc_data.post_content_type = "application/json";
    httpc_data.post_buf = req_payload;
    httpc_data.post_buf_len = len;
    httpc_data.response_buf = rsp_payload;
    httpc_data.response_buf_len = HTTP_AUTH_RESP_MAX_LEN;

    httpc->header = "Connection: Keep-Alive\r\n";

    /*
    Test Code
    iotx_http_context->p_auth_token = "eyJ0eXBlIjoiSldUIiwiYWxnIjoiaG1hY3NoYTEifQ.eyJleHBpcmUiOjE1MDQ3ODE4MzQ5MDAsInRva2VuIjoiM2EyZTRmYzMyNjk5NDE0Y2E3MDFjNzIzNzI1YjIyNDgifQ.e87AFhkvNKiqF5xdgm1P47f9DwY";
    iotx_http_context->is_authed = 1;
    ret = 0;
    goto do_exit;
    Test Code
    */

    {
        extern const char *iotx_ca_crt;
        pub_key = iotx_ca_crt;
    }

    /* Send Request and Get Response */
    if (0 != iotx_post(httpc,
                       http_url,
                       IOTX_HTTP_ONLINE_SERVER_PORT,
                       pub_key,
                       &httpc_data)) {
        goto do_exit;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, CONFIG_HTTP_AUTH_TIMEOUT);


    ret = httpclient_recv_response(httpc, iotx_time_left(&timer), &httpc_data);
    if (ret < 0) {
        http_err("httpclient_recv_response error, ret = %d", ret);
        httpclient_close(httpc);
        return ret;
    }
    if (0 == iotx_http_context->keep_alive) {
        http_info("http not keepalive");
        httpclient_close(httpc);
    }
    /*
    body:
    {
      "code": 0,//业务状态码
      "message": "success",//业务信息
      "info": {
        "token": "eyJ0eXBlIjoiSldUIiwiYWxnIjoiaG1hY3NoYTEifQ.eyJleHBpcmUiOjE1MDI1MzE1MDc0NzcsInRva2VuIjoiODA0ZmFjYTBiZTE3NGUxNjliZjY0ODVlNWNiNDg3MTkifQ.OjMwu29F0CY2YR_6oOyiOLXz0c8"
      }
    }
    */
    http_info("http response: \r\n\r\n%s\r\n", httpc_data.response_buf);

    pvalue = HTTP_LITE_JSON_VALUE_OF("code", httpc_data.response_buf);
    if (!pvalue) {
        goto do_exit;
    }
    ret_code = atoi(pvalue);
    http_info("ret_code = %d", ret_code);
    HTTP_API_FREE(pvalue);
    pvalue = NULL;

    pvalue = HTTP_LITE_JSON_VALUE_OF("message", httpc_data.response_buf);
    if (NULL == pvalue) {
        goto do_exit;
    }
    response_message = pvalue;
    http_info("response_message: %s", response_message);
    (void)response_message;
    HTTP_API_FREE(pvalue);
    pvalue = NULL;

    switch (ret_code) {
        case IOTX_HTTP_SUCCESS:
            break;
        case IOTX_HTTP_COMMON_ERROR:
        case IOTX_HTTP_PARAM_ERROR:
        case IOTX_HTTP_AUTH_CHECK_ERROR:
        case IOTX_HTTP_UPDATE_SESSION_ERROR:
        case IOTX_HTTP_REQUEST_TOO_MANY_ERROR:
        default:
            ret = FAIL_RETURN;
            goto do_exit;
    }

    pvalue = HTTP_LITE_JSON_VALUE_OF("info.token", httpc_data.response_buf);
    if (NULL == pvalue) {
        http_err("can't get token from json, Abort!");
        goto do_exit;
    }

    strcpy(iotx_http_context->p_auth_token, pvalue);
    iotx_http_context->is_authed = 1;
    HTTP_API_FREE(pvalue);
    pvalue = NULL;


    iotx_set_report_func(http_report_func);
    /* report module id */
    ret = iotx_report_mid(iotx_http_context);
    if (SUCCESS_RETURN != ret) {
        http_err("Send ModuleId message to server(Http) failed, ret = %d", ret);
        goto do_exit;
    }
    /* report device information */
    ret = iotx_report_devinfo(iotx_http_context);
    if (SUCCESS_RETURN != ret) {
        http_err("Send devinfo message to server(Http) failed, ret = %d", ret);
        goto do_exit;
    }
    /* report firmware */
    ret = iotx_report_firmware_version(iotx_http_context);
    if (SUCCESS_RETURN != ret) {
        http_err("Send firmware message to server(Http) failed, ret = %d", ret);
        goto do_exit;
    }

    ret = 0;

do_exit:
    if (pvalue) {
        HTTP_API_FREE(pvalue);
        pvalue = NULL;
    }
    if (req_payload) {
        HTTP_API_FREE(req_payload);
        req_payload = NULL;
    }
    if (rsp_payload) {
        HTTP_API_FREE(rsp_payload);
        rsp_payload = NULL;
    }

    return ret;
}

int IOT_HTTP_SendMessage(void *handle, iotx_http_message_param_t *msg_param)
{
    int                 ret = -1;
    int                 response_code = 0;
    char               *pvalue = NULL;
    char                http_url[IOTX_HTTP_URL_LEN_MAX] = {0};
    httpclient_t       *httpc = NULL;
    httpclient_data_t   httpc_data = {0};
    char               *messageId = NULL;
    char               *user_data = NULL;
    int                 len = 0;
    uint32_t            payload_len = 0;
    iotx_time_t         timer;
    iotx_http_t        *iotx_http_context;
    const char         *pub_key = NULL;
    /*
        POST /topic/${topic} HTTP/1.1
        Host: iot-as-http.cn-shanghai.aliyuncs.com
        password:${token}
        Content-Type: application/octet-stream
        body: ${your_data}
    */
    if (NULL == (iotx_http_context = verify_iotx_http_context(handle))) {
        goto do_exit;
    }

    if (NULL == msg_param) {
        http_err("iotx_http_context or msg_param NULL pointer!");
        goto do_exit;
    }

    httpc = (httpclient_t *)iotx_http_context->httpc;

    if (NULL == httpc) {
        http_err("httpc null pointer");
        goto do_exit;
    }

    if (0 == iotx_http_context->is_authed) {
        http_err("Device is not authed");
        goto do_exit;
    }

    if (NULL == msg_param->request_payload) {
        http_err("IOT_HTTP_SendMessage request_payload NULL!");
        goto do_exit;
    }

    if (NULL == msg_param->response_payload) {
        http_err("IOT_HTTP_SendMessage response_payload NULL!");
        goto do_exit;
    }

    if (NULL == msg_param->topic_path) {
        http_err("IOT_HTTP_SendMessage topic_path NULL!");
        goto do_exit;
    }

    payload_len = strlen(msg_param->request_payload) + 1;
    msg_param->request_payload_len = msg_param->request_payload_len > payload_len \
                                     ? payload_len : msg_param->request_payload_len;

    /* Construct Auth Url */
    construct_full_http_upstream_url(http_url, msg_param->topic_path);

    len = strlen(IOTX_HTTP_HEADER_PASSWORD_STR) + strlen(iotx_http_context->p_auth_token) + strlen(
                      IOTX_HTTP_HEADER_KEEPALIVE_STR) + strlen(IOTX_HTTP_HEADER_END_STR);
    httpc->header = HTTP_API_MALLOC(len + 1);
    if (NULL == httpc->header) {
        http_err("Allocate memory for httpc->header failed");
        goto do_exit;
    }
    LITE_snprintf(httpc->header, len + 1,
                  IOTX_HTTP_UPSTREAM_HEADER_STR, iotx_http_context->p_auth_token);
    http_info("httpc->header = %s", httpc->header);

    httpc_data.post_content_type = "application/octet-stream";
    httpc_data.post_buf = msg_param->request_payload;
    httpc_data.post_buf_len = msg_param->request_payload_len;
    httpc_data.response_buf = msg_param->response_payload;
    httpc_data.response_buf_len = msg_param->response_payload_len;

    http_info("request_payload: \r\n\r\n%s\r\n", httpc_data.post_buf);

    {
        extern const char *iotx_ca_crt;
        pub_key = iotx_ca_crt;
    }

    /* Send Request and Get Response */
    if (iotx_post(httpc,
                  http_url,
                  IOTX_HTTP_ONLINE_SERVER_PORT,
                  pub_key,
                  &httpc_data)) {
        goto do_exit_pre;
    }
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, msg_param->timeout_ms);

    ret = httpclient_recv_response(httpc, iotx_time_left(&timer), &httpc_data);
    if (ret < 0) {
        http_err("httpclient_recv_response error, ret = %d", ret);
        httpclient_close(httpc);
        goto do_exit_pre;
    }

    if (0 == iotx_http_context->keep_alive) {
        httpclient_close(httpc);
    }

    /*
        body:
        {
          "code": 0,
          "message": "success",
          "info": {
            "messageId": 892687627916247040,
            "data": byte[]//may be NULL
          }
        }
    */
    http_info("http response: \r\n\r\n%s\r\n", httpc_data.response_buf);

    pvalue = HTTP_LITE_JSON_VALUE_OF("code", httpc_data.response_buf);
    if (!pvalue) {
        goto do_exit_pre;
    }

    response_code = atoi(pvalue);
    HTTP_API_FREE(pvalue);
    pvalue = NULL;
    http_info("response code: %d", response_code);

    pvalue = HTTP_LITE_JSON_VALUE_OF("message", httpc_data.response_buf);
    if (NULL == pvalue) {
        goto do_exit_pre;
    }
    http_info("response_message: %s", pvalue);
    HTTP_API_FREE(pvalue);
    pvalue = NULL;

    switch (response_code) {
        case IOTX_HTTP_SUCCESS:
            break;
        case IOTX_HTTP_TOKEN_EXPIRED_ERROR:
            iotx_http_context->is_authed = IOT_FALSE;
            IOT_HTTP_DeviceNameAuth((iotx_http_t *)iotx_http_context);
        case IOTX_HTTP_COMMON_ERROR:
        case IOTX_HTTP_PARAM_ERROR:
        case IOTX_HTTP_AUTH_CHECK_ERROR:
        case IOTX_HTTP_TOKEN_NULL_ERROR:
        case IOTX_HTTP_TOKEN_CHECK_ERROR:
        case IOTX_HTTP_UPDATE_SESSION_ERROR:
        case IOTX_HTTP_PUBLISH_MESSAGE_ERROR:
        case IOTX_HTTP_REQUEST_TOO_MANY_ERROR:
        default:
            goto do_exit_pre;
    }

    /* info.messageId */
    pvalue = HTTP_LITE_JSON_VALUE_OF("info.messageId", httpc_data.response_buf);
    if (NULL == pvalue) {
        http_err("messageId: NULL");
        goto do_exit_pre;
    }
    messageId = pvalue;
    http_info("messageId: %s", messageId);
    (void)messageId;
    HTTP_API_FREE(pvalue);
    pvalue = NULL;

    /* info.data */
    pvalue = HTTP_LITE_JSON_VALUE_OF("info.data", httpc_data.response_buf);
    user_data = pvalue;

    /* Maybe NULL */
    if (user_data) {
        http_info("user_data: %s", user_data);
    } else {
        http_info("user_data: %p", user_data);
    }
    if (NULL != pvalue) {
        HTTP_API_FREE(pvalue);
    }
    pvalue = NULL;

    ret = 0;

do_exit_pre:

    if (pvalue) {
        HTTP_API_FREE(pvalue);
    }

    if (httpc != NULL && httpc->header) {
        HTTP_API_FREE(httpc->header);
    }

do_exit:

    return ret;
}

void IOT_HTTP_Disconnect(void *handle)
{
    iotx_http_t *iotx_http_context;
    if (NULL != (iotx_http_context = verify_iotx_http_context(handle))) {
        httpclient_close(iotx_http_context->httpc);
    }
}

