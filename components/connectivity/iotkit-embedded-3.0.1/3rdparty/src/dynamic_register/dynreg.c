/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include "string.h"
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_httpc.h"
#include "infra_sha256.h"
#include "dynreg_internal.h"
#include "dynreg_wrapper.h"

#define HTTP_RESPONSE_PAYLOAD_LEN           (256)

#define DYNREG_RANDOM_KEY_LENGTH            (15)
#define DYNREG_SIGN_LENGTH                  (65)
#define DYNREG_SIGN_METHOD_HMACSHA256       "hmacsha256"

static int _parse_string_value(char *payload, int *pos, int *start, int *end)
{
    int idx = 0;

    for (idx = *pos + 1; idx < strlen(payload); idx++) {
        if (payload[idx] == '\"') {
            break;
        }
    }
    *start = *pos + 1;
    *end = idx - 1;
    *pos = idx;

    return 0;
}

static int _parse_dynreg_value(char *payload, char *key, int *pos, int *start, int *end)
{
    int idx = 0;
    /* printf("=====%s\n",&payload[*pos]); */

    if (memcmp(key, "code", strlen("code")) == 0) {
        for (idx = *pos; idx < strlen(payload); idx++) {
            if (payload[idx] < '0' || payload[idx] > '9') {
                break;
            }
        }
        *start = *pos;
        *end = idx - 1;
        *pos = *end;
        return 0;
    } else if (memcmp(key, "data", strlen("data")) == 0) {
        int bracket_cnt = 0;
        if (payload[*pos] != '{') {
            return -1;
        }
        for (idx = *pos; idx < strlen(payload); idx++) {
            if (payload[idx] == '{') {
                bracket_cnt++;
            } else if (payload[idx] == '}') {
                bracket_cnt--;
            }
            if (bracket_cnt == 0) {
                break;
            }
        }
        *start = *pos;
        *end = idx;
        *pos = *end;
        return 0;
    } else {
        if (payload[*pos] != '\"') {
            return -1;
        }
        return _parse_string_value(payload, pos, start, end);
    }

    return -1;
}

static int _parse_dynreg_result(char *payload, char *key, int *start, int *end)
{
    int res = 0, idx = 0, pos = 0;

    for (idx = 0; idx < strlen(payload); idx++) {
        /* printf("loop start: %s\n",&payload[idx]); */
        if (payload[idx] == '\"') {
            for (pos = idx + 1; pos < strlen(payload); pos++) {
                if (payload[pos] == '\"') {
                    /* printf("key: %.*s\n",pos - idx - 1, &payload[idx+1]); */
                    break;
                }
            }

            if (pos == strlen(payload) || payload[pos + 1] != ':') {
                return -1;
            }

            pos += 2;
            res = _parse_dynreg_value(payload, key, &pos, start, end);
            if (res == 0 && memcmp(key, &payload[idx + 1], strlen(key)) == 0) {
                /* printf("value: %.*s\n",*end - *start + 1,&payload[*start]); */
                return 0;
            }

            idx = pos;
        }
    }

    printf("exit 4\n");
    return -1;
}

static int _calc_dynreg_sign(
            char product_key[IOTX_PRODUCT_KEY_LEN],
            char product_secret[IOTX_PRODUCT_SECRET_LEN],
            char device_name[IOTX_DEVICE_NAME_LEN],
            char random[DYNREG_RANDOM_KEY_LENGTH + 1],
            char sign[DYNREG_SIGN_LENGTH])
{
    int sign_source_len = 0;
    uint8_t signnum[32];
    uint8_t  *sign_source = NULL;
    const char *dynamic_register_sign_fmt = "deviceName%sproductKey%srandom%s";

    /* Start Dynamic Register */
    memcpy(random, "8Ygb7ULYh53B6OA", strlen("8Ygb7ULYh53B6OA"));
    dynreg_info("Random Key: %s", random);

    /* Calculate SHA256 Value */
    sign_source_len = strlen(dynamic_register_sign_fmt) + strlen(device_name) + strlen(product_key) + strlen(random) + 1;
    sign_source = dynreg_malloc(sign_source_len);
    if (sign_source == NULL) {
        dynreg_err("Memory Not Enough");
        return FAIL_RETURN;
    }
    memset(sign_source, 0, sign_source_len);
    HAL_Snprintf((char *)sign_source, sign_source_len, dynamic_register_sign_fmt, device_name, product_key, random);

    utils_hmac_sha256(sign_source, strlen((const char *)sign_source), (uint8_t *)product_secret, strlen(product_secret),
                      signnum);
    infra_hex2str(signnum, 32, sign);
    dynreg_free(sign_source);
    dynreg_info("Sign: %s", sign);

    return SUCCESS_RETURN;
}

static int _fetch_dynreg_http_resp(char *request_payload, char *response_payload,
                                   iotx_http_region_types_t region, char device_secret[IOTX_DEVICE_SECRET_LEN])
{
    int                 res = 0;
    const char         *domain = NULL;
    const char         *url_format = "http://%s/auth/register/device";
    char               *url = NULL;
    int                 url_len = 0;
    const char          *pub_key = NULL;
    httpclient_t        http_client;
    httpclient_data_t   http_client_data;
    int                 start = 0, end = 0, data_start = 0, data_end = 0;

    memset(&http_client, 0, sizeof(httpclient_t));
    memset(&http_client_data, 0, sizeof(httpclient_data_t));

    domain = g_infra_http_domain[region];
    if (NULL == domain) {
        dynreg_err("Get domain failed");
        return FAIL_RETURN;
    }
    url_len = strlen(url_format) + strlen(domain) + 1;
    url = (char *)dynreg_malloc(url_len);
    if (NULL == url) {
        dynreg_err("Not Enough Memory");
        return FAIL_RETURN;
    }
    memset(url, 0, url_len);
    HAL_Snprintf(url, url_len, url_format, domain);

    http_client.header = "Accept: text/xml,text/javascript,text/html,application/json\r\n";

    http_client_data.post_content_type = "application/x-www-form-urlencoded";
    http_client_data.post_buf = request_payload;
    http_client_data.post_buf_len = strlen(request_payload);
    http_client_data.response_buf = response_payload;
    http_client_data.response_buf_len = HTTP_RESPONSE_PAYLOAD_LEN;

#ifdef SUPPORT_TLS
    {
        extern const char *iotx_ca_crt;
        pub_key = iotx_ca_crt;
    }
#endif

    res = httpclient_common(&http_client, url, 443, pub_key, HTTPCLIENT_POST, 10000, &http_client_data);
    if (res != SUCCESS_RETURN) {
        dynreg_err("Http Download Failed");
        dynreg_free(url);
        return FAIL_RETURN;
    }
    dynreg_free(url);
    dynreg_info("Http Response Payload: %s", http_client_data.response_buf);

    _parse_dynreg_result(response_payload, "code", &start, &end);
    dynreg_info("Dynamic Register Code: %.*s", end - start + 1, &response_payload[start]);

    if (memcmp(&response_payload[start], "200", strlen("200")) != 0) {
        return FAIL_RETURN;
    }

    _parse_dynreg_result(response_payload, "data", &data_start, &data_end);
    /*  dynreg_info("value: %.*s\n",data_end - data_start + 1,&response_payload[data_start]); */

    _parse_dynreg_result(&response_payload[data_start + 1], "deviceSecret", &start, &end);
    dynreg_info("Dynamic Register Device Secret: %.*s", end - start + 1, &response_payload[data_start + 1 + start]);

    if (end - start + 1 > IOTX_DEVICE_SECRET_LEN) {
        return FAIL_RETURN;
    }

    memcpy(device_secret, &response_payload[data_start + 1 + start], end - start + 1);

    return SUCCESS_RETURN;
}

int32_t IOT_Dynamic_Register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta)
{
    int             res = 0, dynamic_register_request_len = 0;
    char            sign[DYNREG_SIGN_LENGTH] = {0};
    char            random[DYNREG_RANDOM_KEY_LENGTH + 1] = {0};
    const char     *dynamic_register_format = "productKey=%s&deviceName=%s&random=%s&sign=%s&signMethod=%s";
    char           *dynamic_register_request = NULL;
    char           *dynamic_register_response = NULL;

    if (strlen(meta->product_key) > IOTX_PRODUCT_KEY_LEN ||
        strlen(meta->product_secret) > IOTX_PRODUCT_SECRET_LEN ||
        strlen(meta->device_name) > IOTX_DEVICE_NAME_LEN) {
        return FAIL_RETURN;
    }

    /* Calcute Signature */
    res = _calc_dynreg_sign(meta->product_key, meta->product_secret, meta->device_name, random, sign);
    if (res != SUCCESS_RETURN) {
        dynreg_err("Calculate Sign Failed");
        return FAIL_RETURN;
    }

    /* Assemble Http Dynamic Register Request Payload */
    dynamic_register_request_len = strlen(dynamic_register_format) + strlen(meta->product_key) + strlen(meta->device_name) +
                                   strlen(random) + strlen(sign) + strlen(DYNREG_SIGN_METHOD_HMACSHA256) + 1;
    dynamic_register_request = dynreg_malloc(dynamic_register_request_len);
    if (dynamic_register_request == NULL) {
        dynreg_err("Not Enough Memory");
        return FAIL_RETURN;
    }
    memset(dynamic_register_request, 0, dynamic_register_request_len);
    HAL_Snprintf(dynamic_register_request, dynamic_register_request_len, dynamic_register_format,
                 meta->product_key, meta->device_name, random, sign, DYNREG_SIGN_METHOD_HMACSHA256);

    dynamic_register_response = dynreg_malloc(HTTP_RESPONSE_PAYLOAD_LEN);
    if (dynamic_register_response == NULL) {
        dynreg_err("Not Enough Memory");
        dynreg_free(dynamic_register_request);
        return FAIL_RETURN;
    }

    /* Send Http Request For Getting Device Secret */
    res = _fetch_dynreg_http_resp(dynamic_register_request, dynamic_register_response, region, meta->device_secret);

#ifdef INFRA_LOG_NETWORK_PAYLOAD
    dynreg_dbg("Downstream Payload:");
    iotx_facility_json_print(dynamic_register_response, LOG_DEBUG_LEVEL, '<');
#endif

    dynreg_free(dynamic_register_request);
    dynreg_free(dynamic_register_response);
    if (res != SUCCESS_RETURN) {
        dynreg_err("Get Device Secret Failed");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

