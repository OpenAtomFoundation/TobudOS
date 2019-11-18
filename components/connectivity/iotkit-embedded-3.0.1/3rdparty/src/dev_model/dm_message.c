/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_dm_internal.h"

static dm_msg_ctx_t g_dm_msg_ctx;

static dm_msg_ctx_t *_dm_msg_get_ctx(void)
{
    return &g_dm_msg_ctx;
}

int dm_msg_init(void)
{
    dm_msg_ctx_t *ctx = _dm_msg_get_ctx();
    memset(ctx, 0, sizeof(dm_msg_ctx_t));

    return SUCCESS_RETURN;
}

int dm_msg_deinit(void)
{
    dm_msg_ctx_t *ctx = _dm_msg_get_ctx();
    memset(ctx, 0, sizeof(dm_msg_ctx_t));

    return SUCCESS_RETURN;
}

int _dm_msg_send_to_user(iotx_dm_event_types_t type, char *message)
{
    int res = 0;
    dm_ipc_msg_t *dipc_msg = NULL;

    dipc_msg = DM_malloc(sizeof(dm_ipc_msg_t));
    if (dipc_msg == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(dipc_msg, 0, sizeof(dm_ipc_msg_t));

    dipc_msg->type = type;
    dipc_msg->data = message;

    res = dm_ipc_msg_insert((void *)dipc_msg);
    if (res != SUCCESS_RETURN) {
        DM_free(dipc_msg);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_SEND_MSG_TIMEOUT_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_send_msg_timeout_to_user(int msg_id, int devid, iotx_dm_event_types_t type)
{
    int res = 0, message_len = 0;
    char *message = NULL;

    message_len = strlen(DM_MSG_SEND_MSG_TIMEOUT_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len + 1);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_SEND_MSG_TIMEOUT_FMT, msg_id, IOTX_DM_ERR_CODE_TIMEOUT, devid);

    res = _dm_msg_send_to_user(type, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_msg_uri_parse_pkdn(_IN_ char *uri, _IN_ int uri_len, _IN_ int start_deli, _IN_ int end_deli,
                          _OU_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _OU_ char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0, start = 0, end = 0, slice = 0;

    if (uri == NULL || uri_len <= 0 || product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) || (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(uri, uri_len, DM_URI_SERVICE_DELIMITER, start_deli, &start);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_utils_memtok(uri, uri_len, DM_URI_SERVICE_DELIMITER, start_deli + 1, &slice);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_utils_memtok(uri, uri_len, DM_URI_SERVICE_DELIMITER, end_deli, &end);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* dm_log_debug("URI Product Key: %.*s, Device Name: %.*s", slice - start - 1, uri + start + 1, end - slice - 1,
                 uri + slice + 1); */

    memcpy(product_key, uri + start + 1, slice - start - 1);
    memcpy(device_name, uri + slice + 1, end - slice - 1);

    return SUCCESS_RETURN;
}

int dm_msg_request_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ dm_msg_request_payload_t *request)
{
    lite_cjson_t lite;

    if (payload == NULL || payload_len <= 0 || request == NULL) {
        return DM_INVALID_PARAMETER;
    }

    if (dm_utils_json_parse(payload, payload_len, cJSON_Object, &lite) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, DM_MSG_KEY_ID, strlen(DM_MSG_KEY_ID), cJSON_String, &request->id) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, DM_MSG_KEY_VERSION, strlen(DM_MSG_KEY_VERSION), cJSON_String,
                                  &request->version) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, DM_MSG_KEY_METHOD, strlen(DM_MSG_KEY_METHOD), cJSON_String,
                                  &request->method) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, DM_MSG_KEY_PARAMS, strlen(DM_MSG_KEY_PARAMS), cJSON_Invalid,
                                  &request->params) != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Current Request Message ID: %.*s", request->id.value_length, request->id.value);
    dm_log_debug("Current Request Message Version: %.*s", request->version.value_length, request->version.value);
    dm_log_debug("Current Request Message Method: %.*s", request->method.value_length, request->method.value);
    dm_log_debug("Current Request Message Params: %.*s", request->params.value_length, request->params.value);

    return SUCCESS_RETURN;
}

int dm_msg_response_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ dm_msg_response_payload_t *response)
{
    lite_cjson_t lite, lite_message;

    if (payload == NULL || payload_len <= 0 || response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    if (dm_utils_json_parse(payload, payload_len, cJSON_Object, &lite) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, DM_MSG_KEY_ID, strlen(DM_MSG_KEY_ID), cJSON_String, &response->id) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, DM_MSG_KEY_CODE, strlen(DM_MSG_KEY_CODE), cJSON_Number,
                                  &response->code) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, DM_MSG_KEY_DATA, strlen(DM_MSG_KEY_DATA), cJSON_Invalid,
                                  &response->data) != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Current Request Message ID: %.*s", response->id.value_length, response->id.value);
    dm_log_debug("Current Request Message Code: %d", response->code.value_int);
    dm_log_debug("Current Request Message Data: %.*s", response->data.value_length, response->data.value);

    memset(&lite_message, 0, sizeof(lite_cjson_t));
    if (dm_utils_json_object_item(&lite, DM_MSG_KEY_MESSAGE, strlen(DM_MSG_KEY_MESSAGE), cJSON_Invalid,
                                  &response->message) == SUCCESS_RETURN) {
        dm_log_debug("Current Request Message Desc: %.*s", response->message.value_length, response->message.value);
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_REQUEST[] DM_READ_ONLY = "{\"id\":\"%d\",\"version\":\"%s\",\"params\":%.*s,\"method\":\"%s\"}";
int dm_msg_request(dm_msg_dest_type_t type, _IN_ dm_msg_request_t *request)
{
    int res = 0, payload_len = 0;
    char *payload = NULL, *uri = NULL;
    lite_cjson_t lite;

    if (request == NULL || request->params == NULL || request->method == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Request URI */
    res = dm_utils_service_name(request->service_prefix, request->service_name,
                                request->product_key, request->device_name, &uri);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    payload_len = strlen(DM_MSG_REQUEST) + 10 + strlen(DM_MSG_VERSION) + request->params_len + strlen(
                              request->method) + 1;
    payload = DM_malloc(payload_len);
    if (payload == NULL) {
        DM_free(uri);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(payload, 0, payload_len);
    HAL_Snprintf(payload, payload_len, DM_MSG_REQUEST, request->msgid,
                 DM_MSG_VERSION, request->params_len, request->params, request->method);

    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(payload, payload_len, &lite);
    if (res < SUCCESS_RETURN) {
        dm_log_info("Wrong JSON Format, URI: %s, Payload: %s", uri, payload);
        DM_free(uri);
        DM_free(payload);
        return FAIL_RETURN;
    }

    dm_log_info("DM Send Message, URI: %s, Payload: %s", uri, payload);

    if (type & DM_MSG_DEST_CLOUD) {
        dm_client_publish(uri, (unsigned char *)payload, strlen(payload), request->callback);
    }

#ifdef ALCS_ENABLED
    if (type & DM_MSG_DEST_LOCAL) {
        dm_server_send(uri, (unsigned char *)payload, strlen(payload), NULL);
    }
#endif

    DM_free(uri);
    DM_free(payload);
    return SUCCESS_RETURN;
}

const char DM_MSG_RESPONSE_WITH_DATA[] DM_READ_ONLY = "{\"id\":\"%.*s\",\"code\":%d,\"data\":%.*s}";
int dm_msg_response(dm_msg_dest_type_t type, _IN_ dm_msg_request_payload_t *request, _IN_ dm_msg_response_t *response,
                    _IN_ char *data, _IN_ int data_len, _IN_ void *user_data)
{
    int res = 0, payload_len = 0;
    char *uri = NULL, *payload = NULL;
    lite_cjson_t lite;

    if (request == NULL || response == NULL || data == NULL || data_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    /* Response URI */
    res = dm_utils_service_name(response->service_prefix, response->service_name,
                                response->product_key, response->device_name, &uri);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Response Payload */
    payload_len = strlen(DM_MSG_RESPONSE_WITH_DATA) + request->id.value_length + DM_UTILS_UINT32_STRLEN + data_len + 1;
    payload = DM_malloc(payload_len);
    if (payload == NULL) {
        DM_free(uri);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(payload, 0, payload_len);
    HAL_Snprintf(payload, payload_len, DM_MSG_RESPONSE_WITH_DATA,
                 request->id.value_length, request->id.value, response->code, data_len, data);

    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(payload, payload_len, &lite);
    if (res < SUCCESS_RETURN) {
        dm_log_info("Wrong JSON Format, URI: %s, Payload: %s", uri, payload);
        DM_free(uri);
        DM_free(payload);
        return FAIL_RETURN;
    }

    dm_log_info("Send URI: %s, Payload: %s", uri, payload);

    if (type & DM_MSG_DEST_CLOUD) {
        dm_client_publish(uri, (unsigned char *)payload, strlen(payload), NULL);
    }

#ifdef ALCS_ENABLED
    if (type & DM_MSG_DEST_LOCAL) {
        char *end = NULL;
        do {
            if (strlen(uri) < 6) {
                break;
            }
            end = uri + strlen(uri) - 6;
            if (strstr(end, "_reply") != 0) {
                *end = '\0';
            }
            dm_server_send(uri, (unsigned char *)payload, strlen(payload), user_data);
        } while (0);

    }
#endif

    DM_free(uri);
    DM_free(payload);

    return SUCCESS_RETURN;
}


const char DM_MSG_THING_MODEL_DOWN_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":\"%.*s\"}";
int dm_msg_thing_model_down_raw(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, devid = 0, message_len = 0;
    char *hexstr = NULL, *message = NULL;

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_utils_hex_to_str((unsigned char *)payload, payload_len, &hexstr);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    message_len = strlen(DM_MSG_THING_MODEL_DOWN_FMT) + DM_UTILS_UINT32_STRLEN + strlen(hexstr) + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        DM_free(hexstr);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_THING_MODEL_DOWN_FMT, devid, strlen(hexstr), hexstr);
    DM_free(hexstr);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_MODEL_DOWN_RAW, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}


const char DM_MSG_THING_MODEL_UP_RAW_REPLY_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":\"%.*s\"}";
int dm_msg_thing_model_up_raw_reply(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                    _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], char *payload, int payload_len)
{
    int res = 0, devid = 0, message_len = 0;
    char *hexstr = NULL, *message = NULL;

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_utils_hex_to_str((unsigned char *)payload, payload_len, &hexstr);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    message_len = strlen(DM_MSG_THING_MODEL_DOWN_FMT) + DM_UTILS_UINT32_STRLEN + strlen(hexstr) + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        DM_free(hexstr);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_THING_MODEL_DOWN_FMT, devid, strlen(hexstr), hexstr);
    DM_free(hexstr);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_MODEL_UP_RAW_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
#ifndef DEPRECATED_LINKKIT
#ifdef LOG_REPORT_TO_CLOUD
    const char DM_MSG_PROPERTY_SET_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":%.*s,\"msgid\":%.*s}";
#else
    const char DM_MSG_PROPERTY_SET_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":%.*s}";
#endif
int dm_msg_property_set(int devid, dm_msg_request_payload_t *request)
{
    int res = 0, message_len = 0;
    char *message = NULL;

    message_len = strlen(DM_MSG_PROPERTY_SET_FMT) + DM_UTILS_UINT32_STRLEN + request->params.value_length + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
#ifdef LOG_REPORT_TO_CLOUD
    HAL_Snprintf(message, message_len, DM_MSG_PROPERTY_SET_FMT, devid, request->params.value_length, request->params.value,
                 request->id.value_length, request->id.value);
#else
    HAL_Snprintf(message, message_len, DM_MSG_PROPERTY_SET_FMT, devid, request->params.value_length, request->params.value);
#endif
    res = _dm_msg_send_to_user(IOTX_DM_EVENT_PROPERTY_SET, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}

const char DM_MSG_THING_PROPERTY_GET_FMT[] DM_READ_ONLY =
            "{\"id\":\"%.*s\",\"devid\":%d,\"payload\":%.*s,\"ctx\":\"%s\"}";
int dm_msg_property_get(_IN_ int devid, _IN_ dm_msg_request_payload_t *request, _IN_ void *ctx)
{
    int res = 0, message_len = 0;
    uintptr_t ctx_addr_num = (uintptr_t)ctx;
    char *ctx_addr_str = NULL, *message = NULL;

    ctx_addr_str = DM_malloc(sizeof(uintptr_t) * 2 + 1);
    if (ctx_addr_str == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(ctx_addr_str, 0, sizeof(uintptr_t) * 2 + 1);

    /*  dm_log_debug("ctx: %p", ctx);
     dm_log_debug("ctx_addr_num: %0x016llX", ctx_addr_num); */
    infra_hex2str((unsigned char *)&ctx_addr_num, sizeof(uintptr_t), ctx_addr_str);
    /* dm_log_debug("ctx_addr_str: %s", ctx_addr_str); */

    message_len = strlen(DM_MSG_THING_PROPERTY_GET_FMT) + request->id.value_length + DM_UTILS_UINT32_STRLEN +
                  request->params.value_length + strlen(ctx_addr_str) + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        DM_free(ctx_addr_str);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_THING_PROPERTY_GET_FMT, request->id.value_length, request->id.value, devid,
                 request->params.value_length, request->params.value, ctx_addr_str);

    DM_free(ctx_addr_str);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_PROPERTY_GET, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_SERVICE_REQUEST_FMT[] DM_READ_ONLY =
            "{\"id\":\"%.*s\",\"devid\":%d,\"serviceid\":\"%.*s\",\"payload\":%.*s,\"ctx\":\"%s\"}";
int dm_msg_thing_service_request(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                 _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                 char *identifier, int identifier_len, dm_msg_request_payload_t *request,  _IN_ void *ctx)
{
    int res = 0, devid = 0, message_len = 0;
    char *message = NULL;
    uintptr_t ctx_addr_num = (uintptr_t)ctx;
    char *ctx_addr_str = NULL;

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
#ifdef LOG_REPORT_TO_CLOUD
    if (0 == strncmp(identifier, "SetProfilerOptions", identifier_len)) {
        extern void parse_switch_info(const char *input, int len);
        parse_switch_info(request->params.value, request->params.value_length);
        return SUCCESS_RETURN;
    }
#endif

    ctx_addr_str = DM_malloc(sizeof(uintptr_t) * 2 + 1);
    if (ctx_addr_str == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(ctx_addr_str, 0, sizeof(uintptr_t) * 2 + 1);
    infra_hex2str((unsigned char *)&ctx_addr_num, sizeof(uintptr_t), ctx_addr_str);

    message_len = strlen(DM_MSG_SERVICE_REQUEST_FMT) + request->id.value_length + DM_UTILS_UINT32_STRLEN + identifier_len +
                  request->params.value_length + strlen(ctx_addr_str)  + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        DM_free(ctx_addr_str);
        return DM_MEMORY_NOT_ENOUGH;
    }

    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_SERVICE_REQUEST_FMT, request->id.value_length, request->id.value, devid,
                 identifier_len, identifier,
                 request->params.value_length, request->params.value, ctx_addr_str);

    DM_free(ctx_addr_str);
    res = _dm_msg_send_to_user(IOTX_DM_EVENT_THING_SERVICE_REQUEST, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#endif

const char DM_MSG_EVENT_RRPC_REQUEST_FMT[] DM_READ_ONLY =
            "{\"id\":\"%.*s\",\"devid\":%d,\"serviceid\":\"%.*s\",\"rrpcid\":\"%.*s\",\"payload\":%.*s}";
int dm_msg_rrpc_request(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                        _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                        char *rrpcid, int rrpcid_len, dm_msg_request_payload_t *request)
{
    int res = 0, devid = 0, message_len = 0;
    int service_offset = 0, serviceid_len = 0;
    char *serviceid = NULL, *message = NULL;

    /* Get Devid */
    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Service ID */
    res = dm_utils_memtok(request->method.value, request->method.value_length, '.', 2, &service_offset);
    if (res != SUCCESS_RETURN || service_offset >= request->method.value_length - 1) {
        return FAIL_RETURN;
    }
    serviceid_len = request->method.value_length - service_offset - 1;
    serviceid = request->method.value + service_offset + 1;
    /* dm_log_info("Current RRPC Service ID: %.*s", serviceid_len, serviceid); */

    /* Send Message To User */
    message_len = strlen(DM_MSG_EVENT_RRPC_REQUEST_FMT) + request->id.value_length + DM_UTILS_UINT32_STRLEN + serviceid_len
                  + rrpcid_len +
                  request->params.value_length + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_RRPC_REQUEST_FMT, request->id.value_length, request->id.value, devid,
                 serviceid_len, serviceid, rrpcid_len, rrpcid,
                 request->params.value_length, request->params.value);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_RRPC_REQUEST, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_PROPERTY_POST_REPLY_FMT[] DM_READ_ONLY =
            "{\"id\":%d,\"code\":%d,\"devid\":%d,\"payload\":%.*s}";
int dm_msg_thing_event_property_post_reply(dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0, id = 0, message_len = 0, payload_len = 0;
    char *message = NULL, *payload = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    if ((strlen("success") == response->message.value_length) &&
        (memcmp("success", response->message.value, response->message.value_length) == 0)) {
        payload = response->data.value;
        payload_len = response->data.value_length;
    } else {
        payload = response->message.value;
        payload_len = response->message.value_length;
    }

    message_len = strlen(DM_MSG_EVENT_PROPERTY_POST_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + payload_len +
                  1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_PROPERTY_POST_REPLY_FMT, id, response->code.value_int, devid,
                 payload_len, payload);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_SPECIFIC_POST_REPLY_FMT[] DM_READ_ONLY =
            "{\"id\":%d,\"code\":%d,\"devid\":%d,\"eventid\":\"%.*s\",\"payload\":\"%.*s\"}";
int dm_msg_thing_event_post_reply(_IN_ char *identifier, _IN_ int identifier_len,
                                  _IN_ dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0, id = 0, message_len = 0;
    char *message = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif
    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    message_len = strlen(DM_MSG_EVENT_SPECIFIC_POST_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + strlen(
                              identifier) + response->message.value_length + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_SPECIFIC_POST_REPLY_FMT, id, response->code.value_int, devid,
                 identifier_len, identifier, response->message.value_length, response->message.value);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#ifdef DEVICE_MODEL_SHADOW
const char DM_MSG_EVENT_PROPERTY_DESIRED_GET_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"data\":%.*s}";
int dm_msg_thing_property_desired_get_reply(dm_msg_response_payload_t *response)
{
    int res = 0, id = 0, message_len = 0;
    char *message = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
#endif

    message_len = strlen(DM_MSG_EVENT_PROPERTY_DESIRED_GET_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 2 + 1 +
                  response->data.value_length;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_PROPERTY_DESIRED_GET_REPLY_FMT, id, response->code.value_int,
                 response->data.value_length, response->data.value);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_PROPERTY_DESIRED_GET_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_PROPERTY_DESIRED_DELETE_REPLY_FMT[] DM_READ_ONLY =
            "{\"id\":%d,\"code\":%d,\"data\":%.*s,\"devid\":%d}";
int dm_msg_thing_property_desired_delete_reply(dm_msg_response_payload_t *response)
{
    int res = 0, id = 0, devid = 0,  message_len = 0;
    char *message = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    message_len = strlen(DM_MSG_EVENT_PROPERTY_DESIRED_DELETE_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1 +
                  response->data.value_length;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_PROPERTY_DESIRED_DELETE_REPLY_FMT, id, response->code.value_int,
                 response->data.value_length, response->data.value, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_PROPERTY_DESIRED_DELETE_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#endif


const char DM_MSG_EVENT_DEVICEINFO_UPDATE_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_thing_deviceinfo_update_reply(dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0, id = 0, message_len = 0;
    char *message = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    message_len = strlen(DM_MSG_EVENT_DEVICEINFO_UPDATE_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_DEVICEINFO_UPDATE_REPLY_FMT, id, response->code.value_int, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_DEVICEINFO_DELETE_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_thing_deviceinfo_delete_reply(dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0, id = 0, message_len = 0;
    char *message = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    message_len = strlen(DM_MSG_EVENT_DEVICEINFO_DELETE_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_DEVICEINFO_DELETE_REPLY_FMT, id, response->code.value_int, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_msg_thing_dsltemplate_get_reply(dm_msg_response_payload_t *response)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0, id = 0;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif
    if (response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    dm_mgr_deprecated_set_tsl(devid, IOTX_DM_TSL_TYPE_ALINK, (const char *)response->data.value,
                              response->data.value_length);
#endif

    return SUCCESS_RETURN;
}

int dm_msg_thing_dynamictsl_get_reply(dm_msg_response_payload_t *response)
{
#ifdef DEPRECATED_LINKKIT
    int res = 0, devid = 0, id = 0;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif
    if (response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    dm_mgr_deprecated_set_tsl(devid, IOTX_DM_TSL_TYPE_ALINK, (const char *)response->data.value,
                              response->data.value_length);
    dm_mgr_dev_initialized(devid);
#endif
    return SUCCESS_RETURN;
}

const char DM_MSG_THING_NTP_RESPONSE_FMT[] DM_READ_ONLY = "{\"utc\":\"%.*s\"}";
int dm_msg_ntp_response(char *payload, int payload_len)
{
    int res = 0, message_len = 0;
    char *message = NULL;
    lite_cjson_t lite, lite_item_server_send_time;
    const char *serverSendTime = "serverSendTime";

    if (payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    if (dm_utils_json_parse(payload, payload_len, cJSON_Object, &lite) != SUCCESS_RETURN ||
        dm_utils_json_object_item(&lite, serverSendTime, strlen(serverSendTime), cJSON_String,
                                  &lite_item_server_send_time) != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* dm_log_debug("NTP Time In String: %.*s", lite_item_server_send_time.value_length, lite_item_server_send_time.value); */

    /* Send Message To User */
    message_len = strlen(DM_MSG_THING_NTP_RESPONSE_FMT) + DM_UTILS_UINT32_STRLEN + lite_item_server_send_time.value_length +
                  1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_THING_NTP_RESPONSE_FMT, lite_item_server_send_time.value_length,
                 lite_item_server_send_time.value);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_NTP_RESPONSE, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_msg_ext_error_reply(dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0;
    lite_cjson_t lite, lite_item_pk, lite_item_dn;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};

    if (response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_json_parse(response->data.value, response->data.value_length, cJSON_Invalid, &lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_utils_json_object_item(&lite, DM_MSG_KEY_PRODUCT_KEY, strlen(DM_MSG_KEY_PRODUCT_KEY), cJSON_Invalid, &lite_item_pk);
    dm_utils_json_object_item(&lite, DM_MSG_KEY_DEVICE_NAME, strlen(DM_MSG_KEY_DEVICE_NAME), cJSON_Invalid, &lite_item_dn);
    if (lite_item_pk.type != cJSON_String || lite_item_dn.type != cJSON_String) {
        return FAIL_RETURN;
    }
    memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);
    memcpy(device_name, lite_item_dn.value, lite_item_dn.value_length);

    /* Get Device Id */
    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Login again if error code is 520 */
    if (response->code.value_int == IOTX_DM_ERR_CODE_NO_ACTIVE_SESSION) {
        dm_log_err("log in again test\r\n");
#ifdef DEVICE_MODEL_GATEWAY
        dm_mgr_upstream_combine_login(devid);
#endif
    }

    return SUCCESS_RETURN;
}
#endif

#ifdef DEVICE_MODEL_GATEWAY
const char DM_MSG_TOPO_ADD_NOTIFY_USER_PAYLOAD[] DM_READ_ONLY =
            "{\"result\":%d,\"devid\":%d,\"product_key\":\"%s\",\"device_name\":\"%s\"}";
int dm_msg_topo_add_notify(_IN_ char *payload, _IN_ int payload_len)
{
    int ret = SUCCESS_RETURN, res = 0, index = 0, devid = 0, message_len = 0;
    lite_cjson_t lite, lite_item, lite_item_pk, lite_item_dn;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char *message = NULL;

    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(payload, payload_len, &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite)) {
        return DM_JSON_PARSE_FAILED;
    }

    for (index = 0; index < lite.size; index++) {
        devid = 0;
        message_len = 0;
        message = NULL;
        memset(&lite_item, 0, sizeof(lite_cjson_t));
        memset(&lite_item_pk, 0, sizeof(lite_cjson_t));
        memset(&lite_item_dn, 0, sizeof(lite_cjson_t));
        memset(product_key, 0, IOTX_PRODUCT_KEY_LEN + 1);
        memset(device_name, 0, IOTX_DEVICE_NAME_LEN + 1);

        res = lite_cjson_array_item(&lite, index, &lite_item);
        if (res != SUCCESS_RETURN) {
            ret = FAIL_RETURN;
            continue;
        }

        res = lite_cjson_object_item(&lite_item, DM_MSG_KEY_PRODUCT_KEY, strlen(DM_MSG_KEY_PRODUCT_KEY), &lite_item_pk);
        if (res != SUCCESS_RETURN) {
            ret = FAIL_RETURN;
            continue;
        }

        res = lite_cjson_object_item(&lite_item, DM_MSG_KEY_DEVICE_NAME, strlen(DM_MSG_KEY_DEVICE_NAME), &lite_item_dn);
        if (res != SUCCESS_RETURN) {
            ret = FAIL_RETURN;
            continue;
        }

        /* dm_log_debug("Current Product Key: %.*s, Device Name: %.*s",
                     lite_item_pk.value_length, lite_item_pk.value,
                     lite_item_dn.value_length, lite_item_dn.value); */

        if (lite_item_pk.value_length >= IOTX_PRODUCT_KEY_LEN + 1 ||
            lite_item_dn.value_length >= IOTX_DEVICE_NAME_LEN + 1) {
            ret = FAIL_RETURN;
            continue;
        }
        memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);
        memcpy(device_name, lite_item_dn.value, lite_item_dn.value_length);

        res = dm_mgr_device_create(IOTX_DM_DEVICE_SUBDEV, product_key, device_name, NULL, &devid);
        if (res != SUCCESS_RETURN) {
            ret = FAIL_RETURN;
        }

        /* Send To User */
        message_len = strlen(DM_MSG_TOPO_ADD_NOTIFY_USER_PAYLOAD) + 20 +
                      strlen(product_key) + strlen(device_name) + 1;
        message = DM_malloc(message_len);
        if (message == NULL) {
            ret = DM_MEMORY_NOT_ENOUGH;
            continue;
        }
        memset(message, 0, message_len);
        HAL_Snprintf(message, message_len, DM_MSG_TOPO_ADD_NOTIFY_USER_PAYLOAD, res, devid, product_key, device_name);
        res = _dm_msg_send_to_user(IOTX_DM_EVENT_TOPO_ADD_NOTIFY, message);
        if (res != SUCCESS_RETURN) {
            ret = FAIL_RETURN;
            DM_free(message);
        }

    }

    return ret;
}

const char DM_MSG_EVENT_THING_DISABLE_FMT[] DM_READ_ONLY = "{\"devid\":%d}";
int dm_msg_thing_disable(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                         _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0, devid = 0, message_len = 0;
    char *message = NULL;

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_set_dev_disable(devid);

    message_len = strlen(DM_MSG_EVENT_THING_DISABLE_FMT) + DM_UTILS_UINT32_STRLEN + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_THING_DISABLE_FMT, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_THING_DISABLE, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_THING_ENABLE_FMT[] DM_READ_ONLY = "{\"devid\":%d}";
int dm_msg_thing_enable(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                        _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0, devid = 0, message_len = 0;
    char *message = NULL;

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_set_dev_enable(devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    message_len = strlen(DM_MSG_EVENT_THING_ENABLE_FMT) + DM_UTILS_UINT32_STRLEN + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_THING_ENABLE_FMT, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_THING_ENABLE, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_THING_DELETE_FMT[] DM_READ_ONLY =
            "{\"res\":%d,\"productKey\":\"%s\",\"deviceName\":\"%s\",\"devid\":%d}";
int dm_msg_thing_delete(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                        _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0, message_len = 0, devid = 0;
    char *message = NULL;

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res == SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_device_destroy(devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    message_len = strlen(DM_MSG_EVENT_THING_DELETE_FMT) + strlen(product_key) + strlen(device_name) + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_THING_DELETE_FMT, res, product_key, device_name, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_THING_DELETE, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_msg_thing_gateway_permit(_IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, message_len = 0;
    char *message = NULL;
    lite_cjson_t lite;

    if (payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = lite_cjson_parse(payload, payload_len, &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
        return DM_JSON_PARSE_FAILED;
    }

    message_len = payload_len + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memcpy(message, payload, payload_len);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_GATEWAY_PERMIT, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_SUBDEV_REGISTER_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_thing_sub_register_reply(dm_msg_response_payload_t *response)
{
    int res = 0, index = 0, message_len = 0, devid = 0;
    lite_cjson_t lite, lite_item, lite_item_pk, lite_item_dn, lite_item_ds;
    char *message = NULL;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};
    char temp_id[DM_UTILS_UINT32_STRLEN] = {0};

    if (response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = lite_cjson_parse(response->data.value, response->data.value_length, &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite)) {
        return DM_JSON_PARSE_FAILED;
    }

    for (index = 0; index < lite.size; index++) {
        devid = 0;
        message_len = 0;
        message = NULL;
        memset(temp_id, 0, DM_UTILS_UINT32_STRLEN);
        memset(product_key, 0, IOTX_PRODUCT_KEY_LEN + 1);
        memset(device_name, 0, IOTX_DEVICE_NAME_LEN + 1);
        memset(&lite_item, 0, sizeof(lite_cjson_t));
        memset(&lite_item_pk, 0, sizeof(lite_cjson_t));
        memset(&lite_item_dn, 0, sizeof(lite_cjson_t));
        memset(&lite_item_ds, 0, sizeof(lite_cjson_t));

        /* dm_log_debug("Current Index: %d", index); */
        /* Item */
        res = lite_cjson_array_item(&lite, index, &lite_item);
        if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
            continue;
        }

        /* Product Key */
        res = lite_cjson_object_item(&lite_item, DM_MSG_KEY_PRODUCT_KEY, strlen(DM_MSG_KEY_PRODUCT_KEY), &lite_item_pk);
        if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_pk)) {
            continue;
        }
        /* dm_log_debug("Current Product Key: %.*s", lite_item_pk.value_length, lite_item_pk.value); */

        /* Device Name */
        res = lite_cjson_object_item(&lite_item, DM_MSG_KEY_DEVICE_NAME, strlen(DM_MSG_KEY_DEVICE_NAME), &lite_item_dn);
        if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_dn)) {
            continue;
        }
        /* dm_log_debug("Current Device Name: %.*s", lite_item_dn.value_length, lite_item_dn.value); */

        /* Device Secret */
        res = lite_cjson_object_item(&lite_item, DM_MSG_KEY_DEVICE_SECRET, strlen(DM_MSG_KEY_DEVICE_SECRET), &lite_item_ds);
        if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_ds)) {
            continue;
        }
        /* dm_log_debug("Current Device Secret: %.*s", lite_item_ds.value_length, lite_item_ds.value); */

        /* Get Device ID */
        memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);
        memcpy(device_name, lite_item_dn.value, lite_item_dn.value_length);
        memcpy(device_secret, lite_item_ds.value, lite_item_ds.value_length);
        res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        /* Update State Machine */
        if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS) {
            dm_mgr_set_dev_status(devid, IOTX_DM_DEV_STATUS_REGISTERED);
        }

        /* Set Device Secret */
        res = dm_mgr_set_device_secret(devid, device_secret);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        /* Send Message To User */
        memcpy(temp_id, response->id.value, response->id.value_length);
        message_len = strlen(DM_MSG_EVENT_SUBDEV_REGISTER_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 2 + 1;
        message = DM_malloc(message_len);
        if (message == NULL) {
            dm_log_warning("Memory Not Enough");
            continue;
        }
        memset(message, 0, message_len);
        HAL_Snprintf(message, message_len, DM_MSG_EVENT_SUBDEV_REGISTER_REPLY_FMT, atoi(temp_id), response->code.value_int,
                     devid);

        res = _dm_msg_send_to_user(IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY, message);
        if (res != SUCCESS_RETURN) {
            DM_free(message);
        }
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_SUBDEV_UNREGISTER_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_thing_sub_unregister_reply(dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0, id, message_len = 0;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
    char *message = NULL;
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    if (response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;
#endif

    message_len = strlen(DM_MSG_EVENT_SUBDEV_UNREGISTER_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_SUBDEV_UNREGISTER_REPLY_FMT, id, response->code.value_int, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_THING_TOPO_ADD_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_thing_topo_add_reply(dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0, id = 0, message_len = 0;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
    char *message = NULL;
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;

    /* Update State Machine */
    if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS) {
        dm_mgr_set_dev_status(node->devid, IOTX_DM_DEV_STATUS_ATTACHED);
    }

#endif

    message_len = strlen(DM_MSG_EVENT_THING_TOPO_ADD_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_THING_TOPO_ADD_REPLY_FMT, id, response->code.value_int, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_TOPO_ADD_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_THING_TOPO_DELETE_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_thing_topo_delete_reply(dm_msg_response_payload_t *response)
{
    int res = 0, devid = 0, id = 0, message_len = 0;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};
    char *message = NULL;
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    dm_msg_cache_node_t *node = NULL;
#endif

    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

#if !defined(DM_MESSAGE_CACHE_DISABLED)
    res = dm_msg_cache_search(id, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    devid = node->devid;

    /* Update State Machine */
    if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS) {
        dm_mgr_set_dev_status(node->devid, IOTX_DM_DEV_STATUS_ATTACHED);
    }

#endif

    message_len = strlen(DM_MSG_EVENT_THING_TOPO_DELETE_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_THING_TOPO_DELETE_REPLY_FMT, id, response->code.value_int, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_TOPO_DELETE_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_TOPO_GET_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d,\"topo\":%.*s}";
int dm_msg_topo_get_reply(dm_msg_response_payload_t *response)
{
    int res = 0, id = 0, message_len = 0;
    char *message = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    if (response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Message ID */
    if (response->id.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(int_id, response->id.value, response->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

    message_len = strlen(DM_MSG_TOPO_GET_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + response->data.value_length + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_TOPO_GET_REPLY_FMT, id, response->code.value_int, IOTX_DM_LOCAL_NODE_DEVID,
                 response->data.value_length,
                 response->data.value);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_TOPO_GET_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_msg_thing_list_found_reply(dm_msg_response_payload_t *response)
{
    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_COMBINE_LOGIN_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_combine_login_reply(dm_msg_response_payload_t *response)
{
    int res = 0, message_len = 0, devid = 0;
    char *message = NULL;
    lite_cjson_t lite, lite_item_pk, lite_item_dn;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char temp_id[DM_UTILS_UINT32_STRLEN] = {0};

    if (response == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }

    /* Parse JSON */
    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(response->data.value, response->data.value_length, &lite);
    if (res != SUCCESS_RETURN) {
        return DM_JSON_PARSE_FAILED;
    }

    /* Parse Product Key */
    res = lite_cjson_object_item(&lite, DM_MSG_KEY_PRODUCT_KEY, strlen(DM_MSG_KEY_PRODUCT_KEY), &lite_item_pk);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_pk)
        || lite_item_pk.value_length >= IOTX_PRODUCT_KEY_LEN + 1) {
        return DM_JSON_PARSE_FAILED;
    }
    memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);

    /* Parse Device Name */
    res = lite_cjson_object_item(&lite, DM_MSG_KEY_DEVICE_NAME, strlen(DM_MSG_KEY_DEVICE_NAME), &lite_item_dn);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_dn)
        || lite_item_dn.value_length >= IOTX_DEVICE_NAME_LEN + 1) {
        return DM_JSON_PARSE_FAILED;
    }
    memcpy(device_name, lite_item_dn.value, lite_item_dn.value_length);

    /* Get Device Id */
    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Update State Machine */
    if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS) {
        dm_mgr_set_dev_status(devid, IOTX_DM_DEV_STATUS_LOGINED);
    }

    /* Message ID */
    memcpy(temp_id, response->id.value, response->id.value_length);

    message_len = strlen(DM_MSG_EVENT_COMBINE_LOGIN_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_COMBINE_LOGIN_REPLY_FMT, atoi(temp_id), response->code.value_int,
                 devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_COMBINE_LOGIN_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    if (response->code.value_int != IOTX_DM_ERR_CODE_SUCCESS) {
        return SUCCESS_RETURN;
    }

    return SUCCESS_RETURN;
}

const char DM_MSG_EVENT_COMBINE_LOGOUT_REPLY_FMT[] DM_READ_ONLY = "{\"id\":%d,\"code\":%d,\"devid\":%d}";
int dm_msg_combine_logout_reply(dm_msg_response_payload_t *response)
{
    int res = 0, message_len = 0, devid = 0;
    char *message = NULL;
    lite_cjson_t lite, lite_item_pk, lite_item_dn;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char temp_id[DM_UTILS_UINT32_STRLEN] = {0};

    if (response == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Parse JSON */
    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(response->data.value, response->data.value_length, &lite);
    if (res != SUCCESS_RETURN) {
        return DM_JSON_PARSE_FAILED;
    }

    /* Parse Product Key */
    res = lite_cjson_object_item(&lite, DM_MSG_KEY_PRODUCT_KEY, strlen(DM_MSG_KEY_PRODUCT_KEY), &lite_item_pk);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_pk)
        || lite_item_pk.value_length >= IOTX_PRODUCT_KEY_LEN + 1) {
        return DM_JSON_PARSE_FAILED;
    }
    memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);

    /* Parse Device Name */
    res = lite_cjson_object_item(&lite, DM_MSG_KEY_DEVICE_NAME, strlen(DM_MSG_KEY_DEVICE_NAME), &lite_item_dn);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_dn)
        || lite_item_dn.value_length >= IOTX_DEVICE_NAME_LEN + 1) {
        return DM_JSON_PARSE_FAILED;
    }
    memcpy(device_name, lite_item_dn.value, lite_item_dn.value_length);

    /* Get Device Id */
    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Update State Machine */
    if (response->code.value_int == IOTX_DM_ERR_CODE_SUCCESS) {
        dm_mgr_set_dev_status(devid, IOTX_DM_DEV_STATUS_ATTACHED);
    }

    /* Message ID */
    memcpy(temp_id, response->id.value, response->id.value_length);

    message_len = strlen(DM_MSG_EVENT_COMBINE_LOGOUT_REPLY_FMT) + DM_UTILS_UINT32_STRLEN * 3 + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_EVENT_COMBINE_LOGOUT_REPLY_FMT, atoi(temp_id), response->code.value_int,
                 devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#endif

#ifdef ALCS_ENABLED
const char DM_MSG_DEV_CORE_SERVICE_DEV[] DM_READ_ONLY =
            "{\"devices\":{\"addr\":\"%s\",\"port\":%d,\"pal\":\"linkkit-ica\",\"profile\":%s}}";
int dm_msg_dev_core_service_dev(char **payload, int *payload_len)
{
    int res = 0, index = 0, search_devid = 0;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};
    char ip_addr[16] = {0};
    char *device_array = NULL;
    lite_cjson_item_t *lite_array = NULL, *lite_object = NULL;
    uint16_t port = 5683;

    if (payload == NULL || *payload != NULL || payload_len == NULL) {
        return DM_INVALID_PARAMETER;
    }

    lite_array = lite_cjson_create_array();
    if (lite_array == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }

    /* Get Product Key And Device Name Of All Device */
    for (index = 0; index < dm_mgr_device_number(); index++) {
        search_devid = 0;
        lite_object = NULL;
        memset(product_key, 0, IOTX_PRODUCT_KEY_LEN + 1);
        memset(device_name, 0, IOTX_DEVICE_NAME_LEN + 1);
        memset(device_secret, 0, IOTX_DEVICE_SECRET_LEN + 1);

        res = dm_mgr_get_devid_by_index(index, &search_devid);
        if (res != SUCCESS_RETURN) {
            lite_cjson_delete(lite_array);
            return FAIL_RETURN;
        }

        res = dm_mgr_search_device_by_devid(search_devid, product_key, device_name, device_secret);
        if (res != SUCCESS_RETURN) {
            lite_cjson_delete(lite_array);
            return FAIL_RETURN;
        }

        lite_object = lite_cjson_create_object();
        if (lite_object == NULL) {
            lite_cjson_delete(lite_array);
            return FAIL_RETURN;
        }
        lite_cjson_add_string_to_object(lite_object, "productKey", product_key);
        lite_cjson_add_string_to_object(lite_object, "deviceName", device_name);
        lite_cjson_add_item_to_array(lite_array, lite_object);
    }

    device_array = lite_cjson_print_unformatted(lite_array);
    lite_cjson_delete(lite_array);
    if (device_array == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }

    HAL_Wifi_Get_IP(ip_addr, 0);

    *payload_len = strlen(DM_MSG_DEV_CORE_SERVICE_DEV) + strlen(ip_addr) + DM_UTILS_UINT16_STRLEN + strlen(
                               device_array) + 1;
    *payload = DM_malloc(*payload_len);
    if (*payload == NULL) {
        HAL_Free(device_array);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*payload, 0, *payload_len);
    HAL_Snprintf(*payload, *payload_len, DM_MSG_DEV_CORE_SERVICE_DEV, ip_addr, port, device_array);
    DM_free(device_array);

    return SUCCESS_RETURN;
}
#endif

int dm_msg_cloud_connected(void)
{
    return _dm_msg_send_to_user(IOTX_DM_EVENT_CLOUD_CONNECTED, NULL);
}

int dm_msg_cloud_disconnect(void)
{
    return _dm_msg_send_to_user(IOTX_DM_EVENT_CLOUD_DISCONNECT, NULL);
}

int dm_msg_cloud_reconnect(void)
{
    int res = 0;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    /* Send To User */
    res = _dm_msg_send_to_user(IOTX_DM_EVENT_CLOUD_RECONNECT, NULL);

    return res;
}

#ifdef DEVICE_MODEL_GATEWAY
const char DM_MSG_THING_SUB_REGISTER_METHOD[] DM_READ_ONLY = "thing.sub.register";
const char DM_MSG_THING_SUB_REGISTER_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int dm_msg_thing_sub_register(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                              _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                              _OU_ dm_msg_request_t *request)
{
    int params_len = 0;
    char *params = NULL;

    if (request == NULL || product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        (strlen(request->product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(request->device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    params_len = strlen(DM_MSG_THING_SUB_REGISTER_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
    params = DM_malloc(params_len);
    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    HAL_Snprintf(params, params_len, DM_MSG_THING_SUB_REGISTER_PARAMS, product_key, device_name);

    /* Get Params */
    request->params = params;
    request->params_len = strlen(request->params);

    /* Get Method */
    request->method = (char *)DM_MSG_THING_SUB_REGISTER_METHOD;

    return SUCCESS_RETURN;
}

const char DM_MSG_THING_SUB_UNREGISTER_METHOD[] DM_READ_ONLY = "thing.sub.unregister";
const char DM_MSG_THING_SUB_UNREGISTER_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int dm_msg_thing_sub_unregister(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                _OU_ dm_msg_request_t *request)
{
    int params_len = 0;
    char *params = NULL;

    if (request == NULL || product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        (strlen(request->product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(request->device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    params_len = strlen(DM_MSG_THING_SUB_UNREGISTER_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
    params = DM_malloc(params_len);
    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    HAL_Snprintf(params, params_len, DM_MSG_THING_SUB_UNREGISTER_PARAMS, product_key, device_name);

    /* Get Params */
    request->params = params;
    request->params_len = strlen(request->params);

    /* Get Method */
    request->method = (char *)DM_MSG_THING_SUB_UNREGISTER_METHOD;

    return SUCCESS_RETURN;
}

const char DM_MSG_THING_TOPO_ADD_SIGN_SOURCE[] DM_READ_ONLY = "clientId%sdeviceName%sproductKey%stimestamp%s";
const char DM_MSG_THING_TOPO_ADD_METHOD[] DM_READ_ONLY = "thing.topo.add";
const char DM_MSG_THING_TOPO_ADD_PARAMS[] DM_READ_ONLY =
            "[{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"signmethod\":\"%s\",\"sign\":\"%s\",\"timestamp\":\"%s\",\"clientId\":\"%s\"}]";
int dm_msg_thing_topo_add(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                          _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                          _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1], _OU_ dm_msg_request_t *request)
{
    char *params = NULL;
    int params_len = 0;
    char timestamp[DM_UTILS_UINT64_STRLEN] = {0};
    char client_id[IOTX_PRODUCT_KEY_LEN + 1 + IOTX_DEVICE_NAME_LEN + 1 + 1] = {0};
    char *sign_source = NULL;
    int sign_source_len = 0;
    char *sign_method = DM_MSG_SIGN_METHOD_HMACSHA1;
    char sign[65] = {0};


    if (request == NULL || product_key == NULL ||
        device_name == NULL || device_secret == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        (strlen(device_secret) >= IOTX_DEVICE_SECRET_LEN + 1) ||
        (strlen(request->product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(request->device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    /* TimeStamp */
    HAL_Snprintf(timestamp, DM_UTILS_UINT64_STRLEN, "%llu", (unsigned long long)HAL_UptimeMs());
    /* dm_log_debug("Time Stamp: %s", timestamp); */

    /* Client ID */
    HAL_Snprintf(client_id, IOTX_PRODUCT_KEY_LEN + 1 + IOTX_DEVICE_NAME_LEN + 1 + 1, "%s.%s", product_key, device_name);

    /* Sign */
    sign_source_len = strlen(DM_MSG_THING_TOPO_ADD_SIGN_SOURCE) + strlen(client_id) +
                      strlen(device_name) + strlen(product_key) + strlen(timestamp) + 1;
    sign_source = DM_malloc(sign_source_len);
    if (sign_source == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(sign_source, 0, sign_source_len);
    HAL_Snprintf(sign_source, sign_source_len, DM_MSG_THING_TOPO_ADD_SIGN_SOURCE, client_id,
                 device_name, product_key, timestamp);

    /* dm_log_debug("Sign Srouce: %s", sign_source); */
#if 0
    if (strcmp(sign_method, DM_MSG_SIGN_METHOD_HMACMD5) == 0) {
        utils_hmac_md5(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
    } else if (strcmp(sign_method, DM_MSG_SIGN_METHOD_HMACSHA1) == 0) {
        utils_hmac_sha1(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
    } else if (strcmp(sign_method, DM_MSG_SIGN_METHOD_HMACSHA256) == 0) {
        utils_hmac_sha256(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
    } else {
        DM_free(sign_source);
        return FAIL_RETURN;
    }
#else
    utils_hmac_sha1(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
#endif
    DM_free(sign_source);
    /* dm_log_debug("Sign : %s", sign); */

    /* Params */
    request->method = (char *)DM_MSG_THING_TOPO_ADD_METHOD;
    params_len = strlen(DM_MSG_THING_TOPO_ADD_PARAMS) + strlen(product_key) + strlen(device_name) +
                 strlen(sign_method) + strlen(sign) + strlen(timestamp) + strlen(client_id) + 1;
    params = DM_malloc(params_len);

    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    HAL_Snprintf(params, params_len, DM_MSG_THING_TOPO_ADD_PARAMS, product_key, device_name,
                 sign_method, sign, timestamp, client_id);

    request->params = params;
    request->params_len = strlen(request->params);

    return SUCCESS_RETURN;
}

const char DM_MSG_THING_TOPO_DELETE_METHOD[] DM_READ_ONLY = "thing.topo.delete";
const char DM_MSG_THING_TOPO_DELETE_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int dm_msg_thing_topo_delete(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                             _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                             _OU_ dm_msg_request_t *request)
{
    char *params = NULL;
    int params_len = 0;

    if (request == NULL || product_key == NULL ||
        device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        (strlen(request->product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(request->device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    /* Params */
    request->method = (char *)DM_MSG_THING_TOPO_DELETE_METHOD;
    params_len = strlen(DM_MSG_THING_TOPO_DELETE_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
    params = DM_malloc(params_len);
    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    HAL_Snprintf(params, params_len, DM_MSG_THING_TOPO_DELETE_PARAMS, product_key, device_name);

    request->params = params;
    request->params_len = strlen(request->params);

    return SUCCESS_RETURN;
}

const char DM_MSG_THING_TOPO_GET_METHOD[] DM_READ_ONLY = "thing.topo.get";
const char DM_MSG_THING_TOPO_GET_PARAMS[] DM_READ_ONLY = "{}";
int dm_msg_thing_topo_get(_OU_ dm_msg_request_t *request)
{
    char *params = NULL;
    int params_len = 0;

    /* Params */
    request->method = (char *)DM_MSG_THING_TOPO_GET_METHOD;
    params_len = strlen(DM_MSG_THING_TOPO_GET_PARAMS) + 1;
    params = DM_malloc(params_len);
    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    memcpy(params, DM_MSG_THING_TOPO_GET_PARAMS, strlen(DM_MSG_THING_TOPO_GET_PARAMS));

    request->params = params;
    request->params_len = strlen(request->params);

    return SUCCESS_RETURN;
}

const char DM_MSG_THING_LIST_FOUND_METHOD[] DM_READ_ONLY = "thing.list.found";
const char DM_MSG_THING_LIST_FOUND_PARAMS[] DM_READ_ONLY = "[{\"productKey\":\"%s\",\"deviceName\":\"%s\"}]";
int dm_msg_thing_list_found(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                            _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                            _OU_ dm_msg_request_t *request)
{
    char *params = NULL;
    int params_len = 0;

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        request == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Params */
    request->method = (char *)DM_MSG_THING_LIST_FOUND_METHOD;
    params_len = strlen(DM_MSG_THING_LIST_FOUND_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
    params = DM_malloc(params_len);
    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    HAL_Snprintf(params, params_len, DM_MSG_THING_LIST_FOUND_PARAMS, product_key, device_name);

    request->params = params;
    request->params_len = strlen(request->params);

    return SUCCESS_RETURN;
}


const char DM_MSG_COMBINE_LOGIN_SIGN_SOURCE[] DM_READ_ONLY = "clientId%sdeviceName%sproductKey%stimestamp%s";
const char DM_MSG_COMBINE_LOGIN_METHOD[] DM_READ_ONLY = "combine.login";
const char DM_MSG_COMBINE_LOGIN_PARAMS[] DM_READ_ONLY =
            "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"timestamp\":\"%s\",\"signMethod\":\"%s\",\"sign\":\"%s\",\"cleanSession\":\"%s\"}";
int dm_msg_combine_login(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                         _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                         _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1], _OU_ dm_msg_request_t *request)
{
    char *params = NULL;
    int params_len = 0;
    char timestamp[DM_UTILS_UINT64_STRLEN] = {0};
    char client_id[IOTX_PRODUCT_KEY_LEN + 1 + IOTX_DEVICE_NAME_LEN + 21] = {0};
    char *sign_source = NULL;
    int sign_source_len = 0;
    char *sign_method = DM_MSG_SIGN_METHOD_HMACSHA1;
    char sign[64] = {0};


    if (request == NULL || product_key == NULL ||
        device_name == NULL || device_secret == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        (strlen(device_secret) >= IOTX_DEVICE_SECRET_LEN + 1) ||
        (strlen(request->product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(request->device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    /* TimeStamp */
    HAL_Snprintf(timestamp, DM_UTILS_UINT64_STRLEN, "%llu", (unsigned long long)HAL_UptimeMs());
    /* dm_log_debug("Time Stamp: %s", timestamp); */

    /* Client ID */
    HAL_Snprintf(client_id, IOTX_PRODUCT_KEY_LEN + 1 + IOTX_DEVICE_NAME_LEN + 21, "%s.%s|_v=sdk-c-"IOTX_SDK_VERSION"|", product_key, device_name);

    /* Sign */
    sign_source_len = strlen(DM_MSG_COMBINE_LOGIN_SIGN_SOURCE) + strlen(client_id) +
                      strlen(device_name) + strlen(product_key) + strlen(timestamp) + 1;
    sign_source = DM_malloc(sign_source_len);
    if (sign_source == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(sign_source, 0, sign_source_len);
    HAL_Snprintf(sign_source, sign_source_len, DM_MSG_COMBINE_LOGIN_SIGN_SOURCE, client_id,
                 device_name, product_key, timestamp);

    /* dm_log_debug("Sign Srouce: %s", sign_source); */
#if 0
    if (strcmp(sign_method, DM_MSG_SIGN_METHOD_HMACMD5) == 0) {
        utils_hmac_md5(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
    } else if (strcmp(sign_method, DM_MSG_SIGN_METHOD_HMACSHA1) == 0) {
        utils_hmac_sha1(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
    } else if (strcmp(sign_method, DM_MSG_SIGN_METHOD_HMACSHA256) == 0) {
        utils_hmac_sha256(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
    } else {
        DM_free(sign_source);
        return FAIL_RETURN;
    }
#else
    utils_hmac_sha1(sign_source, strlen(sign_source), sign, device_secret, strlen(device_secret));
#endif
    DM_free(sign_source);
    /* dm_log_debug("Sign : %s", sign); */

    /* Params */
    request->method = (char *)DM_MSG_COMBINE_LOGIN_METHOD;
    params_len = strlen(DM_MSG_COMBINE_LOGIN_PARAMS) + strlen(product_key) + strlen(device_name) +
                 strlen(sign_method) + strlen(sign) + strlen(timestamp) + strlen(client_id) + 1;
    params = DM_malloc(params_len);

    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    HAL_Snprintf(params, params_len, DM_MSG_COMBINE_LOGIN_PARAMS, product_key, device_name,
                 client_id, timestamp, sign_method, sign, "true");

    request->params = params;
    request->params_len = strlen(request->params);

    return SUCCESS_RETURN;
}

const char DM_MSG_COMBINE_LOGOUT_METHOD[] DM_READ_ONLY = "combine.logout";
const char DM_MSG_COMBINE_LOGOUT_PARAMS[] DM_READ_ONLY = "{\"productKey\":\"%s\",\"deviceName\":\"%s\"}";
int dm_msg_combine_logout(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                          _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                          _OU_ dm_msg_request_t *request)
{
    char *params = NULL;
    int params_len = 0;

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        request == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Params */
    request->method = (char *)DM_MSG_COMBINE_LOGOUT_METHOD;
    params_len = strlen(DM_MSG_COMBINE_LOGOUT_PARAMS) + strlen(product_key) + strlen(device_name) + 1;
    params = DM_malloc(params_len);

    if (params == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(params, 0, params_len);
    HAL_Snprintf(params, params_len, DM_MSG_COMBINE_LOGOUT_PARAMS, product_key, device_name);

    request->params = params;
    request->params_len = strlen(request->params);

    return SUCCESS_RETURN;
}
#endif

#ifdef DEPRECATED_LINKKIT
typedef enum {
    DM_MSG_PROPERTY_SET,
    DM_MSG_SERVICE_SET
} dm_msg_set_type_t;
typedef int (*dm_get_shadow_data)(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data);
typedef int (*dm_set_shadow_data)(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
                                  _IN_ int value_len);

static int _dm_msg_set_number(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root);
static int _dm_msg_set_string(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root);
static int _dm_msg_set_object(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root);
static int _dm_msg_set_array(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root);


static int _dm_msg_set_number(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e data_type;
    dm_get_shadow_data get_shadow_data_func = (type == DM_MSG_PROPERTY_SET) ? (dm_mgr_deprecated_get_property_data) :
            (dm_mgr_deprecated_get_service_input_data);
    dm_set_shadow_data set_shadow_data_func = (type == DM_MSG_PROPERTY_SET) ? (dm_mgr_deprecated_set_property_value) :
            (dm_mgr_deprecated_set_service_input_value);

    /* dm_log_debug("Current Key: %s", key); */

    res = get_shadow_data_func(devid, key, strlen(key), &data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &data_type);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* dm_log_debug("Current Type: %d", data_type); */
    switch (data_type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_BOOL: {
            res = set_shadow_data_func(devid, key, strlen(key), &root->value_int, 0);
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value_float = (float)root->value_double;
            res = set_shadow_data_func(devid, key, strlen(key), &value_float, 0);
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            res = set_shadow_data_func(devid, key, strlen(key), &root->value_double, 0);
        }
        break;
        default:
            dm_log_warning("Unkonwn Number Type");
            break;
    }

    return res;
}

static int _dm_msg_set_string(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root)
{
    int res = 0;
    void *data = NULL;
    dm_shw_data_type_e data_type;
    dm_get_shadow_data get_shadow_data_func = (type == DM_MSG_PROPERTY_SET) ? (dm_mgr_deprecated_get_property_data) :
            (dm_mgr_deprecated_get_service_input_data);
    dm_set_shadow_data set_shadow_data_func = (type == DM_MSG_PROPERTY_SET) ? (dm_mgr_deprecated_set_property_value) :
            (dm_mgr_deprecated_set_service_input_value);

    /* dm_log_debug("Current Key: %s", key); */

    res = get_shadow_data_func(devid, key, strlen(key), &data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_mgr_deprecated_get_data_type(data, &data_type);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* dm_log_debug("Current Type: %d", data_type); */

    switch (data_type) {
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            res = set_shadow_data_func(devid, key, strlen(key), root->value, root->value_length);
        }
        break;
        default:
            dm_log_warning("Unkonwn String Type");
            break;
    }

    return res;
}

static int _dm_msg_set_object(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_item_key;
    lite_cjson_t lite_item_value;
    char *new_key = NULL;
    int new_key_len = 0;

    for (index = 0; index < root->size; index++) {
        res = lite_cjson_object_item_by_index(root, index, &lite_item_key, &lite_item_value);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        /* dm_log_debug("Current Key: %.*s, Value: %.*s",
                     lite_item_key.value_length, lite_item_key.value,
                     lite_item_value.value_length, lite_item_value.value); */
        /* new_key_len = lite_item_key.value_length + 1; */
        new_key_len = ((key == NULL) ? (0) : (strlen(key) + 1)) + lite_item_key.value_length + 1;
        /* dm_log_debug("new_key_len: %d", new_key_len); */
        new_key = DM_malloc(new_key_len);
        if (new_key == NULL) {
            return DM_MEMORY_NOT_ENOUGH;
        }
        memset(new_key, 0, new_key_len);
        if (key) {
            memcpy(new_key, key, strlen(key));
            new_key[strlen(new_key)] = DM_SHW_KEY_DELIMITER;
        }
        memcpy(new_key + strlen(new_key), lite_item_key.value, lite_item_key.value_length);
        /* dm_log_debug("New Key: %s", new_key); */

        if (lite_cjson_is_object(&lite_item_value)) {
            res = _dm_msg_set_object(type, devid, new_key, &lite_item_value);
        }
        if (lite_cjson_is_array(&lite_item_value)) {
            res = _dm_msg_set_array(type, devid, new_key, &lite_item_value);
        }
        if (lite_cjson_is_number(&lite_item_value)) {
            res = _dm_msg_set_number(type, devid, new_key, &lite_item_value);
        }
        if (lite_cjson_is_string(&lite_item_value)) {
            res = _dm_msg_set_string(type, devid, new_key, &lite_item_value);
        }

        DM_free(new_key);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }
    return SUCCESS_RETURN;
}

static int _dm_msg_set_array(dm_msg_set_type_t type, int devid, char *key, lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_item_value;
    char *ascii_index = NULL;
    char *new_key = NULL;
    int new_key_len = 0;

    for (index = 0; index < root->size; index++) {

        res = lite_cjson_array_item(root, index, &lite_item_value);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        /* dm_log_debug("Current Value: %.*s", lite_item_value.value_length, lite_item_value.value); */

        res = dm_utils_itoa(index, &ascii_index);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        /*                         Original Key              '['         Index         ']'*/
        new_key_len = ((key == NULL) ? (0) : (strlen(key) + 1)) + 1 + strlen(ascii_index) + 1 + 1;
        new_key = DM_malloc(new_key_len);
        if (new_key == NULL) {
            DM_free(ascii_index);
            return DM_MEMORY_NOT_ENOUGH;
        }
        memset(new_key, 0, new_key_len);
        if (key) {
            memcpy(new_key, key, strlen(key));
        }
        new_key[strlen(new_key)] = '[';
        memcpy(new_key + strlen(new_key), ascii_index, strlen(ascii_index));
        new_key[strlen(new_key)] = ']';
        /* dm_log_debug("New Key: %s", new_key); */
        DM_free(ascii_index);

        if (lite_cjson_is_object(&lite_item_value)) {
            res = _dm_msg_set_object(type, devid, new_key, &lite_item_value);
        }
        if (lite_cjson_is_array(&lite_item_value)) {
            res = _dm_msg_set_array(type, devid, new_key, &lite_item_value);
        }
        if (lite_cjson_is_number(&lite_item_value)) {
            res = _dm_msg_set_number(type, devid, new_key, &lite_item_value);
        }
        if (lite_cjson_is_string(&lite_item_value)) {
            res = _dm_msg_set_string(type, devid, new_key, &lite_item_value);
        }

        DM_free(new_key);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
    const char DM_MSG_PROPERTY_SET_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"payload\":%.*s}";
#else
    const char DM_MSG_PROPERTY_SET_FMT[] DM_READ_ONLY = "{\"devid\":%d,\"propertyid\":\"%.*s\"}";
#endif
int dm_msg_property_set(int devid, dm_msg_request_payload_t *request)
{
    int res = 0, message_len = 0;
    char *message = NULL;
#ifndef DEVICE_MODEL_GATEWAY
    int index = 0;
    lite_cjson_t lite, lite_item_key, lite_item_value;
#endif
    if (request == NULL) {
        return DM_INVALID_PARAMETER;
    }

#ifdef DEVICE_MODEL_GATEWAY
    message_len = strlen(DM_MSG_PROPERTY_SET_FMT) + DM_UTILS_UINT32_STRLEN + request->params.value_length + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_PROPERTY_SET_FMT, devid, request->params.value_length, request->params.value);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_PROPERTY_SET, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
    }
#else
    /* Parse Root */
    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(request->params.value, request->params.value_length, &lite);
    if (res != SUCCESS_RETURN || (!lite_cjson_is_object(&lite) && !lite_cjson_is_array(&lite))) {
        return DM_JSON_PARSE_FAILED;
    }
    /* dm_log_info("Property Set, Size: %d", lite.size); */

    if (lite_cjson_is_object(&lite)) {
        res = _dm_msg_set_object(DM_MSG_PROPERTY_SET, devid, NULL, &lite);
    }

    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    for (index = 0; index < lite.size; index++) {
        memset(&lite_item_key, 0, sizeof(lite_cjson_t));
        memset(&lite_item_value, 0, sizeof(lite_cjson_t));

        res = lite_cjson_object_item_by_index(&lite, index, &lite_item_key, &lite_item_value);
        if (res != SUCCESS_RETURN) {
            continue;
        }

        message_len = strlen(DM_MSG_PROPERTY_SET_FMT) + DM_UTILS_UINT32_STRLEN + lite_item_key.value_length + 1;
        message = DM_malloc(message_len);
        if (message == NULL) {
            return DM_MEMORY_NOT_ENOUGH;
        }
        memset(message, 0, message_len);
        HAL_Snprintf(message, message_len, DM_MSG_PROPERTY_SET_FMT, devid, lite_item_key.value_length, lite_item_key.value);

        res = _dm_msg_send_to_user(IOTX_DM_EVENT_PROPERTY_SET, message);
        if (res != SUCCESS_RETURN) {
            DM_free(message);
        }
    }
#endif

    return SUCCESS_RETURN;
}

int dm_msg_property_get(_IN_ int devid, _IN_ dm_msg_request_payload_t *request, _IN_ char **payload,
                        _IN_ int *payload_len)
{
    int res = 0, index = 0;
    lite_cjson_t lite, lite_item;
    lite_cjson_item_t *lite_cjson_item = NULL;

    if (devid < 0 || request == NULL || payload == NULL || *payload != NULL || payload_len == NULL) {
        return DM_INVALID_PARAMETER;
    }

    lite_cjson_item = lite_cjson_create_object();
    if (lite_cjson_item == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }

    /* Parse Root */
    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(request->params.value, request->params.value_length, &lite);
    if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite)) {
        return DM_JSON_PARSE_FAILED;
    }
    /* dm_log_info("Property Get, Size: %d", lite.size); */

    /* Parse Params */
    for (index = 0; index < lite.size; index++) {
        memset(&lite_item, 0, sizeof(lite_cjson_t));
        res = lite_cjson_array_item(&lite, index, &lite_item);
        if (res != SUCCESS_RETURN) {
            lite_cjson_delete(lite_cjson_item);
            return FAIL_RETURN;
        }

        if (!lite_cjson_is_string(&lite_item)) {
            lite_cjson_delete(lite_cjson_item);
            return FAIL_RETURN;
        }

        res = dm_mgr_deprecated_assemble_property(devid, lite_item.value, lite_item.value_length, lite_cjson_item);
        if (res != SUCCESS_RETURN) {
            lite_cjson_delete(lite_cjson_item);
            return FAIL_RETURN;
        }
    }

    *payload = lite_cjson_print_unformatted(lite_cjson_item);
    if (*payload == NULL) {
        lite_cjson_delete(lite_cjson_item);
        return FAIL_RETURN;
    }
    lite_cjson_delete(lite_cjson_item);
    *payload_len = strlen(*payload);

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
    const char DM_MSG_SERVICE_REQUEST_FMT[] DM_READ_ONLY =
    "{\"id\":%d,\"devid\":%d,\"serviceid\":\"%.*s\",\"payload\":%.*s}";
#else
    const char DM_MSG_SERVICE_REQUEST_FMT[] DM_READ_ONLY = "{\"id\":%d,\"devid\":%d,\"serviceid\":\"%.*s\"}";
#endif
int dm_msg_thing_service_request(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                 _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                 char *identifier, int identifier_len, dm_msg_request_payload_t *request,  _IN_ void *ctx)
{
    int res = 0, id = 0, devid = 0, message_len = 0;
    lite_cjson_t lite;
#ifndef DEVICE_MODEL_GATEWAY
    char *key = NULL;
#endif
    char *message = NULL;
    char int_id[DM_UTILS_UINT32_STRLEN] = {0};

    if (product_key == NULL || device_name == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) ||
        identifier == NULL || identifier_len == 0 || request == NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* Message ID */
    memcpy(int_id, request->id.value, request->id.value_length);
    id = atoi(int_id);

    /* dm_log_debug("Current ID: %d", id); */

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

#ifdef DEVICE_MODEL_GATEWAY
    message_len = strlen(DM_MSG_SERVICE_REQUEST_FMT) + DM_UTILS_UINT32_STRLEN * 2 + identifier_len +
                  request->params.value_length + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_SERVICE_REQUEST_FMT, id, devid, identifier_len, identifier,
                 request->params.value_length, request->params.value);
#else
    key = DM_malloc(identifier_len + 1);
    if (key == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(key, 0, identifier_len + 1);
    memcpy(key, identifier, identifier_len);

    /* Parse Root */
    memset(&lite, 0, sizeof(lite_cjson_t));
    res = lite_cjson_parse(request->params.value, request->params.value_length, &lite);
    if (res != SUCCESS_RETURN || (!lite_cjson_is_object(&lite) && !lite_cjson_is_array(&lite))) {
        DM_free(key);
        return DM_JSON_PARSE_FAILED;
    }
    /* dm_log_info("Service Request, Size: %d", lite.size); */

    if (lite_cjson_is_object(&lite)) {
        res = _dm_msg_set_object(DM_MSG_SERVICE_SET, devid, key, &lite);
    }
    DM_free(key);

    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    message_len = strlen(DM_MSG_SERVICE_REQUEST_FMT) + DM_UTILS_UINT32_STRLEN * 2 + identifier_len + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, DM_MSG_SERVICE_REQUEST_FMT, id, devid, identifier_len, identifier);
#endif
    res = _dm_msg_send_to_user(IOTX_DM_EVENT_THING_SERVICE_REQUEST, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#endif
