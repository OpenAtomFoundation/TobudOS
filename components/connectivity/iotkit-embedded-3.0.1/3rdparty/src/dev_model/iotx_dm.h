/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _IOT_EXPORT_DM_H_
#define _IOT_EXPORT_DM_H_

#ifndef _IN_
    #define _IN_
#endif

#ifndef _OU_
    #define _OU_
#endif

#ifdef DEVICE_MODEL_GATEWAY
    #define IOTX_DM_DEVICE_TYPE IOTX_DM_DEVICE_GATEWAY
#else
    #define IOTX_DM_DEVICE_TYPE IOTX_DM_DEVICE_SINGLE
#endif

#define IOTX_DM_LOCAL_NODE_DEVID (0)

#define IOTX_DM_DEVICE_SINGLE  (0x01)
#define IOTX_DM_DEVICE_SUBDEV  (0x02)
#define IOTX_DM_DEVICE_GATEWAY (0x04)
#define IOTX_DM_DEVICE_MAIN    (IOTX_DM_DEVICE_SINGLE|IOTX_DM_DEVICE_GATEWAY)
#define IOTX_DM_DEVICE_ALL     (IOTX_DM_DEVICE_SINGLE|IOTX_DM_DEVICE_SUBDEV|IOTX_DM_DEVICE_GATEWAY)

/* Service Type 0~7bit: type, 8~15bit: extended*/
#define IOTX_DM_SERVICE_CLOUD         (0x0001)
#define IOTX_DM_SERVICE_LOCAL         (0x0002)
#define IOTX_DM_SERVICE_LOCAL_NO_AUTH (0x0000)
#define IOTX_DM_SERVICE_LOCAL_AUTH    (0x0100)

#define IOTX_DM_LOCAL_AUTH            (IOTX_DM_SERVICE_LOCAL|IOTX_DM_SERVICE_LOCAL_AUTH)
#define IOTX_DM_LOCAL_NO_AUTH         (IOTX_DM_SERVICE_LOCAL|IOTX_DM_SERVICE_LOCAL_NO_AUTH)

#define IOTX_DM_SERVICE_ALL           (IOTX_DM_SERVICE_CLOUD|IOTX_DM_LOCAL_AUTH)

typedef enum {
    IOTX_DM_ERR_CODE_SUCCESS              = 200,
    IOTX_DM_ERR_CODE_REQUEST_ERROR        = 400,
    IOTX_DM_ERR_CODE_REQUEST_PARAMS_ERROR = 460,
    IOTX_DM_ERR_CODE_REQUEST_TOO_MANY     = 429,
    IOTX_DM_ERR_CODE_NO_ACTIVE_SESSION    = 520,
    IOTX_DM_ERR_CODE_TIMEOUT              = 100000
} iotx_dm_error_code_t;

typedef enum {
    IOTX_DM_EVENT_CLOUD_CONNECTED  = 0,
    IOTX_DM_EVENT_CLOUD_DISCONNECT,
    IOTX_DM_EVENT_CLOUD_RECONNECT,
    IOTX_DM_EVENT_LOCAL_CONNECTED,
    IOTX_DM_EVENT_LOCAL_DISCONNECT,
    IOTX_DM_EVENT_LOCAL_RECONNECT,
    IOTX_DM_EVENT_FOUND_DEVICE,
    IOTX_DM_EVENT_REMOVE_DEVICE,
    IOTX_DM_EVENT_REGISTER_RESULT,
    IOTX_DM_EVENT_UNREGISTER_RESULT,
    IOTX_DM_EVENT_INITIALIZED,
    IOTX_DM_EVENT_SEND_RESULT,
    IOTX_DM_EVENT_ADD_SERVICE_RESULT,
    IOTX_DM_EVENT_REMOVE_SERVICE_RESULT,
    IOTX_DM_EVENT_NEW_DATA_RECEIVED,
    IOTX_DM_EVENT_PROPERTY_SET,
    IOTX_DM_EVENT_PROPERTY_GET,
    IOTX_DM_EVENT_PROPERTY_DESIRED_GET_REPLY,
    IOTX_DM_EVENT_PROPERTY_DESIRED_DELETE_REPLY,
    IOTX_DM_EVENT_TOPO_ADD_NOTIFY,
    IOTX_DM_EVENT_THING_SERVICE_REQUEST,
    IOTX_DM_EVENT_THING_DISABLE,
    IOTX_DM_EVENT_THING_ENABLE,
    IOTX_DM_EVENT_THING_DELETE,
    IOTX_DM_EVENT_MODEL_DOWN_RAW,
    IOTX_DM_EVENT_GATEWAY_PERMIT,
    IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY,
    IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY,
    IOTX_DM_EVENT_TOPO_ADD_REPLY,
    IOTX_DM_EVENT_TOPO_DELETE_REPLY,
    IOTX_DM_EVENT_TOPO_GET_REPLY,
    IOTX_DM_EVENT_TOPO_ADD_NOTIFY_REPLY,
    IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY,
    IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY,
    IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY,
    IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY,
    IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY,
    IOTX_DM_EVENT_COMBINE_LOGIN_REPLY,
    IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY,
    IOTX_DM_EVENT_MODEL_UP_RAW_REPLY,
    IOTX_DM_EVENT_LEGACY_THING_CREATED,
    IOTX_DM_EVENT_COTA_NEW_CONFIG,
    IOTX_DM_EVENT_FOTA_NEW_FIRMWARE,
    IOTX_DM_EVENT_NTP_RESPONSE,
    IOTX_DM_EVENT_RRPC_REQUEST,
    IOTX_DM_EVENT_MAX
} iotx_dm_event_types_t;

typedef void (*iotx_dm_event_callback)(iotx_dm_event_types_t type, char *payload);

typedef enum {
    IOTX_DM_DEVICE_SECRET_PRODUCT,
    IOTX_DM_DEVICE_SECRET_DEVICE,
    IOTX_DM_DEVICE_SECRET_TYPES_MAX
} iotx_dm_device_secret_types_t;

typedef enum {
    IOTX_DM_CLOUD_DOMAIN_SHANGHAI,
    IOTX_DM_CLOUD_DOMAIN_SINGAPORE,
    IOTX_DM_CLOUD_DOMAIN_JAPAN,
    IOTX_DM_CLOUD_DOMAIN_AMERICA,
    IOTX_DM_CLOUD_DOMAIN_GERMANY,
    IOTX_DM_CLOUD_DOMAIN_MAX
} iotx_dm_cloud_domain_types_t;

typedef enum {
    IOTX_DM_MESSAGE_NO_AUTH,
    IOTX_DM_MESSAGE_AUTH,
    IOTX_DM_MESSAGE_AUTH_MAX
} iotx_dm_message_auth_types_t;

typedef enum {
    IOTX_DM_TSL_SOURCE_LOCAL,
    IOTX_DM_TSL_SOURCE_CLOUD
} iotx_dm_tsl_source_t;

typedef enum {
    IOTX_DM_TSL_TYPE_ALINK,
    IOTX_DM_TSL_TYPE_TLV
} iotx_dm_tsl_type_t;

typedef struct {
    iotx_dm_device_secret_types_t secret_type;
    iotx_dm_cloud_domain_types_t domain_type;
    iotx_dm_event_callback event_callback;
} iotx_dm_init_params_t;

typedef enum {
    IOTX_DM_DEV_AVAIL_ENABLE,
    IOTX_DM_DEV_AVAIL_DISABLE
} iotx_dm_dev_avail_t;

typedef enum {
    IOTX_DM_DEV_STATUS_UNAUTHORIZED,   /* Subdev Created */
    IOTX_DM_DEV_STATUS_AUTHORIZED,     /* Receive Topo Add Notify */
    IOTX_DM_DEV_STATUS_REGISTERED,     /* Receive Subdev Registered */
    IOTX_DM_DEV_STATUS_ATTACHED,       /* Receive Subdev Topo Add Reply */
    IOTX_DM_DEV_STATUS_LOGINED,        /* Receive Subdev Login Reply */
    IOTX_DM_DEV_STATUS_ONLINE          /* After All Topic Subscribed */
} iotx_dm_dev_status_t;

typedef enum {
    DM_TSL_SERVICE_GET_FAILED = -13,
    DM_TSL_SERVICE_SET_FAILED = -12,
    DM_TSL_EVENT_GET_FAILED = -11,
    DM_TSL_EVENT_SET_FAILED = -10,
    DM_TSL_PROPERTY_GET_FAILED = -9,
    DM_TSL_PROPERTY_SET_FAILED = -8,
    DM_TSL_EVENT_NOT_EXIST = -7,
    DM_TSL_PROPERTY_NOT_EXIST = -6,
    DM_TSL_SERVICE_NOT_EXIST = -5,
    DM_JSON_PARSE_FAILED = -4,
    DM_MEMORY_NOT_ENOUGH = -3,
    DM_INVALID_PARAMETER = -2
} dm_error_code_t;

#define IOTX_DM_POST_PROPERTY_ALL (NULL)

int iotx_dm_open(void);
int iotx_dm_connect(_IN_ iotx_dm_init_params_t *init_params);
int iotx_dm_subscribe(_IN_ int devid);
int iotx_dm_close(void);
int iotx_dm_yield(int timeout_ms);
void iotx_dm_dispatch(void);

int iotx_dm_post_rawdata(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
int iotx_dm_set_opt(int opt, void *data);
int iotx_dm_get_opt(int opt, void *data);
#ifdef LOG_REPORT_TO_CLOUD
    int iotx_dm_log_post(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
#endif
int iotx_dm_post_property(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int iotx_dm_property_desired_get(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int iotx_dm_property_desired_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int iotx_dm_post_event(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload,
                       _IN_ int payload_len);

int iotx_dm_send_service_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len, _IN_ iotx_dm_error_code_t code,
                                  _IN_ char *identifier,
                                  _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len, void *ctx);
int iotx_dm_send_property_get_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
                                       _IN_ iotx_dm_error_code_t code,
                                       _IN_ char *payload, _IN_ int payload_len, _IN_ void *ctx);
int iotx_dm_send_rrpc_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len, _IN_ iotx_dm_error_code_t code,
                               _IN_ char *rrpcid, _IN_ int rrpcid_len, _IN_ char *payload, _IN_ int payload_len);
int iotx_dm_deviceinfo_update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int iotx_dm_deviceinfo_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len);
int iotx_dm_qurey_ntp(void);
int iotx_dm_send_aos_active(int devid);
#endif

int iotx_dm_cota_perform_sync(_OU_ char *buffer, _IN_ int buffer_len);
int iotx_dm_cota_get_config(_IN_ const char *config_scope, const char *get_type, const char *attribute_keys);
int iotx_dm_fota_perform_sync(_OU_ char *buffer, _IN_ int buffer_len);
int iotx_dm_fota_request_image(_IN_ const char *version, _IN_ int buffer_len);

#ifdef DEVICE_MODEL_GATEWAY
int iotx_dm_query_topo_list(void);
int iotx_dm_subdev_query(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                         _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                         _OU_ int *devid);
int iotx_dm_subdev_create(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                          _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1], _OU_ int *devid);
int iotx_dm_subdev_destroy(_IN_ int devid);
int iotx_dm_subdev_number(void);
int iotx_dm_subdev_register(_IN_ int devid);
int iotx_dm_subdev_unregister(_IN_ int devid);
int iotx_dm_subdev_topo_add(_IN_ int devid);
int iotx_dm_subdev_topo_del(_IN_ int devid);
int iotx_dm_subdev_login(_IN_ int devid);
int iotx_dm_subdev_logout(_IN_ int devid);
int iotx_dm_get_device_type(_IN_ int devid, _OU_ int *type);
int iotx_dm_get_device_avail_status(_IN_ int devid, _OU_ iotx_dm_dev_avail_t *status);
int iotx_dm_get_device_status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status);
#ifdef DEVICE_MODEL_SUBDEV_OTA
    int iotx_dm_send_firmware_version(int devid, const char *firmware_version);
    int iotx_dm_ota_switch_device(_IN_ int devid);
#endif
#endif

#ifdef DEPRECATED_LINKKIT
int iotx_dm_deprecated_subdev_register(_IN_ int devid, _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1]);
int iotx_dm_deprecated_set_tsl(_IN_ int devid, _IN_ iotx_dm_tsl_source_t source, _IN_ const char *tsl,
                               _IN_ int tsl_len);
int iotx_dm_deprecated_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len);
int iotx_dm_deprecated_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int iotx_dm_deprecated_set_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len);
int iotx_dm_deprecated_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int iotx_dm_deprecated_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);
int iotx_dm_deprecated_set_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len);
int iotx_dm_deprecated_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value);

int iotx_dm_deprecated_post_property_start(_IN_ int devid, _OU_ void **handle);
int iotx_dm_deprecated_post_property_add(_IN_ void *handle, _IN_ char *identifier, _IN_ int identifier_len);
int iotx_dm_deprecated_post_property_end(_IN_ void **handle);
int iotx_dm_deprecated_post_event(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len);
int iotx_dm_deprecated_send_service_response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier,
        _IN_ int identifier_len);

int iotx_dm_deprecated_legacy_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char *value_str);
int iotx_dm_deprecated_legacy_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char **value_str);
int iotx_dm_deprecated_legacy_set_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char *value_str);
int iotx_dm_deprecated_legacy_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ char **value_str);
int iotx_dm_deprecated_legacy_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len,
        _IN_ void *value,
        _IN_ char **value_str);
int iotx_dm_deprecated_legacy_set_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len,
        _IN_ void *value,
        _IN_ char *value_str);
int iotx_dm_deprecated_legacy_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len,
        _IN_ void *value,
        _IN_ char **value_str);

int iotx_dm_deprecated_legacy_get_pkdn_by_devid(_IN_ int devid, _OU_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
        _OU_ char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int iotx_dm_deprecated_legacy_get_devid_by_pkdn(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
        _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], _OU_ int *devid);
int iotx_dm_deprecated_legacy_get_thingid_by_devid(_IN_ int devid, _OU_ void **thing_id);
int iotx_dm_deprecated_legacy_get_devid_by_thingid(_IN_ void *thing_id, _OU_ int *devid);
int iotx_dm_deprecated_legacy_get_pkdn_ptr_by_devid(_IN_ int devid, _OU_ char **product_key, _OU_ char **device_name);
int iotx_dm_deprecated_legacy_send_service_response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len);
#ifdef DEVICE_MODEL_GATEWAY
    int iotx_dm_deprecated_subdev_register(_IN_ int devid, _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1]);
#endif
#endif
#endif
