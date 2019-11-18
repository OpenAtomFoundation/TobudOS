/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_MESSAGE_H_
#define _DM_MESSAGE_H_

#include "iotx_dm_internal.h"

#define DM_MSG_KEY_ID                   "id"
#define DM_MSG_KEY_VERSION              "version"
#define DM_MSG_KEY_METHOD               "method"
#define DM_MSG_KEY_PARAMS               "params"
#define DM_MSG_KEY_CODE                 "code"
#define DM_MSG_KEY_DATA                 "data"
#define DM_MSG_KEY_MESSAGE              "message"

#define DM_MSG_VERSION                  "1.0"

#define DM_MSG_KEY_PRODUCT_KEY          "productKey"
#define DM_MSG_KEY_DEVICE_NAME          "deviceName"
#define DM_MSG_KEY_DEVICE_SECRET        "deviceSecret"
#define DM_MSG_KEY_TIME                 "time"

#define DM_MSG_SIGN_METHOD_SHA256       "Sha256"
#define DM_MSG_SIGN_METHOD_HMACMD5      "hmacMd5"
#define DM_MSG_SIGN_METHOD_HMACSHA1     "hmacSha1"
#define DM_MSG_SIGN_METHOD_HMACSHA256   "hmacSha256"

typedef enum {
    DM_MSG_DEST_CLOUD = 0x01,
    DM_MSG_DEST_LOCAL = 0x02,
    DM_MSG_DEST_ALL   = 0x03
} dm_msg_dest_type_t;

typedef struct {
    const char *uri;
    unsigned char *payload;
    unsigned int payload_len;
    void *context;
} dm_msg_source_t;

typedef struct {
    const char *uri_name;
} dm_msg_dest_t;

typedef struct {
    lite_cjson_t id;
    lite_cjson_t version;
    lite_cjson_t method;
    lite_cjson_t params;
} dm_msg_request_payload_t;

typedef struct {
    lite_cjson_t id;
    lite_cjson_t code;
    lite_cjson_t data;
    lite_cjson_t message;
} dm_msg_response_payload_t;

typedef struct {
    int msgid;
    int devid;
    const char *service_prefix;
    const char *service_name;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char device_name[IOTX_DEVICE_NAME_LEN + 1];
    char *params;
    int params_len;
    char *method;
    iotx_cm_data_handle_cb callback;
} dm_msg_request_t;

typedef struct {
    const char *service_prefix;
    const char *service_name;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char device_name[IOTX_DEVICE_NAME_LEN + 1];
    iotx_dm_error_code_t code;
} dm_msg_response_t;

typedef struct {
    int id;
} dm_msg_ctx_t;


int dm_msg_init(void);
int dm_msg_deinit(void);
int _dm_msg_send_to_user(iotx_dm_event_types_t type, char *message);
int dm_msg_send_msg_timeout_to_user(int msg_id, int devid, iotx_dm_event_types_t type);
int dm_msg_uri_parse_pkdn(_IN_ char *uri, _IN_ int uri_len, _IN_ int start_deli, _IN_ int end_deli,
                          _OU_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _OU_ char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int dm_msg_request_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ dm_msg_request_payload_t *request);
int dm_msg_response_parse(_IN_ char *payload, _IN_ int payload_len, _OU_ dm_msg_response_payload_t *response);
int dm_msg_request(dm_msg_dest_type_t type, _IN_ dm_msg_request_t *request);
int dm_msg_response(dm_msg_dest_type_t type, _IN_ dm_msg_request_payload_t *request, _IN_ dm_msg_response_t *response,
                    _IN_ char *data, _IN_ int data_len, _IN_ void *user_data);
int dm_msg_thing_model_down_raw(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                _IN_ char *payload, _IN_ int payload_len);
int dm_msg_thing_model_up_raw_reply(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                    _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], char *payload, int payload_len);
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
int dm_msg_property_set(int devid, dm_msg_request_payload_t *request);
#ifndef DEPRECATED_LINKKIT
int dm_msg_property_get(_IN_ int devid, _IN_ dm_msg_request_payload_t *request, _IN_ void *ctx);
#else
int dm_msg_property_get(_IN_ int devid, _IN_ dm_msg_request_payload_t *request, _IN_ char **payload,
                        _IN_ int *payload_len);
#endif
int dm_msg_thing_service_request(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                 char *identifier, int identifier_len, dm_msg_request_payload_t *request,  _IN_ void *ctx);
int dm_msg_rrpc_request(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                        char *messageid, int messageid_len, dm_msg_request_payload_t *request);
int dm_msg_thing_event_property_post_reply(dm_msg_response_payload_t *response);
int dm_msg_thing_event_post_reply(_IN_ char *identifier, _IN_ int identifier_len,
                                  _IN_ dm_msg_response_payload_t *response);
int dm_msg_thing_deviceinfo_update_reply(dm_msg_response_payload_t *response);
int dm_msg_thing_property_desired_get_reply(dm_msg_response_payload_t *response);
int dm_msg_thing_property_desired_delete_reply(dm_msg_response_payload_t *response);
int dm_msg_thing_deviceinfo_delete_reply(dm_msg_response_payload_t *response);
int dm_msg_thing_dsltemplate_get_reply(dm_msg_response_payload_t *response);
int dm_msg_thing_dynamictsl_get_reply(dm_msg_response_payload_t *response);
int dm_msg_ntp_response(char *payload, int payload_len);
int dm_msg_ext_error_reply(dm_msg_response_payload_t *response);
#endif

#ifdef DEVICE_MODEL_GATEWAY
    int dm_msg_topo_add_notify(_IN_ char *payload, _IN_ int payload_len);
    int dm_msg_thing_disable(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1]);
    int dm_msg_thing_enable(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1]);
    int dm_msg_thing_delete(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1]);
    int dm_msg_thing_gateway_permit(_IN_ char *payload, _IN_ int payload_len);
    int dm_msg_thing_sub_register_reply(dm_msg_response_payload_t *response);
    int dm_msg_thing_sub_unregister_reply(dm_msg_response_payload_t *response);
    int dm_msg_thing_topo_add_reply(dm_msg_response_payload_t *response);
    int dm_msg_thing_topo_delete_reply(dm_msg_response_payload_t *response);
    int dm_msg_topo_get_reply(dm_msg_response_payload_t *response);
    int dm_msg_thing_list_found_reply(dm_msg_response_payload_t *response);
    int dm_msg_combine_login_reply(dm_msg_response_payload_t *response);
    int dm_msg_combine_logout_reply(dm_msg_response_payload_t *response);
#endif
#ifdef ALCS_ENABLED
    int dm_msg_dev_core_service_dev(char **payload, int *payload_len);
#endif
int dm_msg_cloud_connected(void);
int dm_msg_cloud_disconnect(void);
int dm_msg_cloud_reconnect(void);
#if 0
    int dm_msg_found_device(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1]);
    int dm_msg_remove_device(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1]);
    int dm_msg_unregister_result(_IN_ char *uri, _IN_ int result);
    int dm_msg_send_result(_IN_ char *uri, _IN_ int result);
    int dm_msg_add_service_result(_IN_ char *uri, _IN_ int result);
    int dm_msg_remove_service_result(_IN_ char *uri, _IN_ int result);
#endif
int dm_msg_register_result(_IN_ char *uri, _IN_ int result);

#ifdef DEVICE_MODEL_GATEWAY
int dm_msg_thing_sub_register(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                              _OU_ dm_msg_request_t *request);
int dm_msg_thing_sub_unregister(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                _OU_ dm_msg_request_t *request);
int dm_msg_thing_topo_add(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                          _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1], _OU_ dm_msg_request_t *request);
int dm_msg_thing_topo_delete(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                             _OU_ dm_msg_request_t *request);
int dm_msg_thing_topo_get(_OU_ dm_msg_request_t *request);
int dm_msg_thing_list_found(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                            _OU_ dm_msg_request_t *request);
int dm_msg_combine_login(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                         _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1], _OU_ dm_msg_request_t *request);
int dm_msg_combine_logout(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                          _OU_ dm_msg_request_t *request);
#endif
#endif
