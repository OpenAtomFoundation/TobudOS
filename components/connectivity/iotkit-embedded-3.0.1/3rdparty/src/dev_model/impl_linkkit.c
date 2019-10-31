/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_dm_internal.h"

#if defined(DEVICE_MODEL_ENABLED) && !defined(DEPRECATED_LINKKIT)
#include "dev_model_api.h"

#ifdef LOG_REPORT_TO_CLOUD
    #include "iotx_log_report.h"
#endif

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define IMPL_LINKKIT_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "impl.linkkit")
    #define IMPL_LINKKIT_FREE(ptr)               LITE_free(ptr)
#else
    #define IMPL_LINKKIT_MALLOC(size)            HAL_Malloc(size)
    #define IMPL_LINKKIT_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#ifdef DEV_BIND_ENABLED
    #include "dev_bind_api.h"
#endif

#define IOTX_LINKKIT_KEY_ID          "id"
#define IOTX_LINKKIT_KEY_CODE        "code"
#define IOTX_LINKKIT_KEY_DEVID       "devid"
#define IOTX_LINKKIT_KEY_SERVICEID   "serviceid"
#define IOTX_LINKKIT_KEY_PROPERTYID  "propertyid"
#define IOTX_LINKKIT_KEY_EVENTID     "eventid"
#define IOTX_LINKKIT_KEY_PAYLOAD     "payload"
#define IOTX_LINKKIT_KEY_CONFIG_ID   "configId"
#define IOTX_LINKKIT_KEY_CONFIG_SIZE "configSize"
#define IOTX_LINKKIT_KEY_GET_TYPE    "getType"
#define IOTX_LINKKIT_KEY_SIGN        "sign"
#define IOTX_LINKKIT_KEY_SIGN_METHOD "signMethod"
#define IOTX_LINKKIT_KEY_URL         "url"
#define IOTX_LINKKIT_KEY_VERSION     "version"
#define IOTX_LINKKIT_KEY_UTC         "utc"
#define IOTX_LINKKIT_KEY_RRPCID      "rrpcid"
#define IOTX_LINKKIT_KEY_CTX         "ctx"
#define IOTX_LINKKIT_KEY_TOPO        "topo"
#define IOTX_LINKKIT_KEY_PRODUCT_KEY "productKey"
#define IOTX_LINKKIT_KEY_TIME        "time"
#define IOTX_LINKKIT_KEY_DATA        "data"

#define IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS 10000

typedef struct {
    int msgid;
    void *semaphore;
    int code;
    struct list_head linked_list;
} iotx_linkkit_upstream_sync_callback_node_t;

typedef struct {
    void *mutex;
    void *upstream_mutex;
    int is_opened;
    int is_connected;
    struct list_head upstream_sync_callback_list;
} iotx_linkkit_ctx_t;

static iotx_linkkit_ctx_t g_iotx_linkkit_ctx = {0};

static iotx_linkkit_ctx_t *_iotx_linkkit_get_ctx(void)
{
    return &g_iotx_linkkit_ctx;
}

static void _iotx_linkkit_mutex_lock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->mutex) {
        HAL_MutexLock(ctx->mutex);
    }
}

static void _iotx_linkkit_mutex_unlock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->mutex) {
        HAL_MutexUnlock(ctx->mutex);
    }
}

static int _impl_copy(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len)
{
    if (input == NULL || output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *output = IMPL_LINKKIT_MALLOC(output_len);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, output_len);
    memcpy(*output, input, input_len);

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
static void _iotx_linkkit_upstream_mutex_lock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->upstream_mutex) {
        HAL_MutexLock(ctx->upstream_mutex);
    }
}

static void _iotx_linkkit_upstream_mutex_unlock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->upstream_mutex) {
        HAL_MutexUnlock(ctx->upstream_mutex);
    }
}


static int _iotx_linkkit_upstream_sync_callback_list_insert(int msgid, void *semaphore,
        iotx_linkkit_upstream_sync_callback_node_t **node)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->upstream_sync_callback_list, linked_list,
                        iotx_linkkit_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            dm_log_debug("Message Already Exist: %d", msgid);
            return FAIL_RETURN;
        }
    }

    search_node = IMPL_LINKKIT_MALLOC(sizeof(iotx_linkkit_upstream_sync_callback_node_t));
    if (search_node == NULL) {
        dm_log_debug("malloc error");
        return FAIL_RETURN;
    }
    memset(search_node, 0, sizeof(iotx_linkkit_upstream_sync_callback_node_t));
    search_node->msgid = msgid;
    search_node->semaphore = semaphore;
    INIT_LIST_HEAD(&search_node->linked_list);

    list_add(&search_node->linked_list, &ctx->upstream_sync_callback_list);
    dm_log_debug("New Message, msgid: %d", msgid);

    *node = search_node;
    return SUCCESS_RETURN;
}

static int _iotx_linkkit_upstream_sync_callback_list_remove(int msgid)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->upstream_sync_callback_list, linked_list,
                        iotx_linkkit_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            dm_log_debug("Message Found: %d, Delete It", msgid);
            HAL_SemaphoreDestroy(search_node->semaphore);
            list_del(&search_node->linked_list);
            IMPL_LINKKIT_FREE(search_node);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _iotx_linkkit_upstream_sync_callback_list_search(int msgid,
        iotx_linkkit_upstream_sync_callback_node_t **node)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL;

    if (node == NULL || *node != NULL) {
        dm_log_debug("invalid param");
        return FAIL_RETURN;
    }

    list_for_each_entry(search_node, &ctx->upstream_sync_callback_list, linked_list,
                        iotx_linkkit_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            dm_log_debug("Sync Message Found: %d", msgid);
            *node = search_node;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static void _iotx_linkkit_upstream_sync_callback_list_destroy(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *search_node = NULL, *next_node = NULL;

    list_for_each_entry_safe(search_node, next_node, &ctx->upstream_sync_callback_list, linked_list,
                             iotx_linkkit_upstream_sync_callback_node_t) {
        list_del(&search_node->linked_list);
        HAL_SemaphoreDestroy(search_node->semaphore);
        IMPL_LINKKIT_FREE(search_node);
    }
}


static void _iotx_linkkit_upstream_callback_remove(int msgid, int code)
{
    int res = 0;
    iotx_linkkit_upstream_sync_callback_node_t *sync_node = NULL;
    res = _iotx_linkkit_upstream_sync_callback_list_search(msgid, &sync_node);
    if (res == SUCCESS_RETURN) {
        sync_node->code = (code == IOTX_DM_ERR_CODE_SUCCESS) ? (SUCCESS_RETURN) : (FAIL_RETURN);
        dm_log_debug("Sync Message %d Result: %d", msgid, sync_node->code);
        HAL_SemaphorePost(sync_node->semaphore);
    }
}
#endif

#ifdef LOG_REPORT_TO_CLOUD
    int  report_sample = 0;
#endif
#ifdef ALCS_ENABLED
    extern void dm_server_free_context(_IN_ void *ctx);
#endif

static void _iotx_linkkit_event_callback(iotx_dm_event_types_t type, char *payload)
{
    int res = 0;
    void *callback;
#ifdef LOG_REPORT_TO_CLOUD
    lite_cjson_t msg_id;
#endif
    lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid, lite_item_payload, lite_item_ctx;
    lite_cjson_t lite_item_code, lite_item_eventid, lite_item_utc, lite_item_rrpcid, lite_item_topo;
    lite_cjson_t lite_item_pk, lite_item_time;
    lite_cjson_t lite_item_version, lite_item_configid, lite_item_configsize, lite_item_gettype, lite_item_sign,
                 lite_item_signmethod, lite_item_url;

    dm_log_info("Receive Message Type: %d", type);
    if (payload) {
        dm_log_info("Receive Message: %s", payload);
        res = dm_utils_json_parse(payload, strlen(payload), cJSON_Invalid, &lite);
        if (res != SUCCESS_RETURN) {
            return;
        }
#ifdef LOG_REPORT_TO_CLOUD
        dm_utils_json_object_item(&lite, "msgid", 5, cJSON_Invalid, &msg_id);
#endif
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_ID, strlen(IOTX_LINKKIT_KEY_ID), cJSON_Invalid, &lite_item_id);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID), cJSON_Invalid,
                                  &lite_item_devid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_SERVICEID, strlen(IOTX_LINKKIT_KEY_SERVICEID), cJSON_Invalid,
                                  &lite_item_serviceid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD), cJSON_Invalid,
                                  &lite_item_payload);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CTX, strlen(IOTX_LINKKIT_KEY_CTX), cJSON_Invalid, &lite_item_ctx);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CODE, strlen(IOTX_LINKKIT_KEY_CODE), cJSON_Invalid, &lite_item_code);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_EVENTID, strlen(IOTX_LINKKIT_KEY_EVENTID), cJSON_Invalid,
                                  &lite_item_eventid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_UTC, strlen(IOTX_LINKKIT_KEY_UTC), cJSON_Invalid, &lite_item_utc);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_RRPCID, strlen(IOTX_LINKKIT_KEY_RRPCID), cJSON_Invalid,
                                  &lite_item_rrpcid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_TOPO, strlen(IOTX_LINKKIT_KEY_TOPO), cJSON_Invalid,
                                  &lite_item_topo);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_PRODUCT_KEY, strlen(IOTX_LINKKIT_KEY_PRODUCT_KEY), cJSON_Invalid,
                                  &lite_item_pk);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_TIME, strlen(IOTX_LINKKIT_KEY_TIME), cJSON_Invalid,
                                  &lite_item_time);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_VERSION, strlen(IOTX_LINKKIT_KEY_VERSION), cJSON_Invalid,
                                  &lite_item_version);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CONFIG_ID, strlen(IOTX_LINKKIT_KEY_CONFIG_ID), cJSON_Invalid,
                                  &lite_item_configid);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_CONFIG_SIZE, strlen(IOTX_LINKKIT_KEY_CONFIG_SIZE), cJSON_Invalid,
                                  &lite_item_configsize);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_GET_TYPE, strlen(IOTX_LINKKIT_KEY_GET_TYPE), cJSON_Invalid,
                                  &lite_item_gettype);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_SIGN, strlen(IOTX_LINKKIT_KEY_SIGN), cJSON_Invalid,
                                  &lite_item_sign);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_SIGN_METHOD, strlen(IOTX_LINKKIT_KEY_SIGN_METHOD), cJSON_Invalid,
                                  &lite_item_signmethod);
        dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_URL, strlen(IOTX_LINKKIT_KEY_URL), cJSON_Invalid,
                                  &lite_item_url);

    }

    switch (type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            callback = iotx_event_callback(ITE_CONNECT_SUCC);
            if (callback) {
                ((int (*)(void))callback)();
            }
        }
        break;
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            callback = iotx_event_callback(ITE_DISCONNECTED);
            if (callback) {
                ((int (*)(void))callback)();
            }
        }
        break;
        case IOTX_DM_EVENT_INITIALIZED: {
            if (payload == NULL || lite_item_devid.type != cJSON_Number) {
                return;
            }

            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            callback = iotx_event_callback(ITE_INITIALIZE_COMPLETED);
            if (callback) {
                ((int (*)(const int))callback)(lite_item_devid.value_int);
            }
        }
        break;
        case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
            int raw_data_len = 0;
            unsigned char *raw_data = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_payload.type != cJSON_String) {
                return;
            }

            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current Raw Data: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            raw_data_len = lite_item_payload.value_length / 2;
            raw_data = IMPL_LINKKIT_MALLOC(raw_data_len);
            if (raw_data == NULL) {
                dm_log_err("No Enough Memory");
                return;
            }
            LITE_hexstr_convert(lite_item_payload.value, lite_item_payload.value_length, raw_data, raw_data_len);

            HEXDUMP_DEBUG(raw_data, raw_data_len);
            callback = iotx_event_callback(ITE_RAWDATA_ARRIVED);
            if (callback) {
                ((int (*)(const int, const unsigned char *, const int))callback)(lite_item_devid.value_int, raw_data, raw_data_len);
            }

            IMPL_LINKKIT_FREE(raw_data);
        }
        break;
        case IOTX_DM_EVENT_MODEL_UP_RAW_REPLY: {
            int raw_data_len = 0;
            unsigned char *raw_data = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_payload.type != cJSON_String) {
                return;
            }

            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current Raw Data: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            raw_data_len = lite_item_payload.value_length / 2;
            raw_data = IMPL_LINKKIT_MALLOC(raw_data_len);
            if (raw_data == NULL) {
                dm_log_err("No Enough Memory");
                return;
            }
            memset(raw_data, 0, raw_data_len);
            LITE_hexstr_convert(lite_item_payload.value, lite_item_payload.value_length, raw_data, raw_data_len);

            HEXDUMP_DEBUG(raw_data, raw_data_len);

            callback = iotx_event_callback(ITE_RAWDATA_ARRIVED);
            if (callback) {
                ((int (*)(const int, const unsigned char *, const int))callback)(lite_item_devid.value_int, raw_data, raw_data_len);
            }

            IMPL_LINKKIT_FREE(raw_data);
        }
        break;
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            int response_len = 0;
            char *request = NULL, *response = NULL;

            uintptr_t property_get_ctx_num = 0;
            void *property_get_ctx = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_String || lite_item_devid.type != cJSON_Number ||
                lite_item_serviceid.type != cJSON_String || lite_item_payload.type != cJSON_Object) {
                return;
            }

            dm_log_debug("Current Id: %.*s", lite_item_id.value_length, lite_item_id.value);
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);
            dm_log_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);
            dm_log_debug("Current Ctx: %.*s", lite_item_ctx.value_length, lite_item_ctx.value);

            LITE_hexstr_convert(lite_item_ctx.value, lite_item_ctx.value_length, (unsigned char *)&property_get_ctx_num,
                                sizeof(uintptr_t));
            property_get_ctx = (void *)property_get_ctx_num;

            request = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (request == NULL) {
                dm_log_err("Not Enough Memory");
                return;
            }
            memset(request, 0, lite_item_payload.value_length + 1);
            memcpy(request, lite_item_payload.value, lite_item_payload.value_length);

            callback = iotx_event_callback(ITE_SERVICE_REQUEST);
            if (callback) {
                res = ((int (*)(const int, const char *, const int, const char *, const int, char **,
                                int *))callback)(lite_item_devid.value_int, lite_item_serviceid.value,
                                                 lite_item_serviceid.value_length, request, lite_item_payload.value_length, &response, &response_len);
                if (response != NULL && response_len > 0) {
                    /* service response exist */
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_service_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                                  lite_item_serviceid.value,
                                                  lite_item_serviceid.value_length,
                                                  response, response_len, property_get_ctx);
                    HAL_Free(response);
                }
            }
#ifdef ALCS_ENABLED
            if (property_get_ctx) {
                dm_server_free_context(property_get_ctx);
            }
#endif
            IMPL_LINKKIT_FREE(request);
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            char *property_payload = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_payload.type != cJSON_Object) {
                return;
            }

            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            property_payload = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (property_payload == NULL) {
                dm_log_err("No Enough Memory");
                return;
            }
            memset(property_payload, 0, lite_item_payload.value_length + 1);
            memcpy(property_payload, lite_item_payload.value, lite_item_payload.value_length);
#ifdef LOG_REPORT_TO_CLOUD
            if (SUCCESS_RETURN == check_target_msg(msg_id.value, msg_id.value_length)) {
                report_sample = 1;
                send_permance_info(msg_id.value, msg_id.value_length, "3", 1);
            }
#endif
            callback = iotx_event_callback(ITE_PROPERTY_SET);
            if (callback) {
                ((int (*)(const int, const char *, const int))callback)(lite_item_devid.value_int, property_payload,
                        lite_item_payload.value_length);
            }
#ifdef LOG_REPORT_TO_CLOUD
            if (1 == report_sample) {
                send_permance_info(NULL, 0, "5", 2);
                report_sample = 0;
            }
#endif

            IMPL_LINKKIT_FREE(property_payload);
        }
        break;
#ifdef DEVICE_MODEL_SHADOW
        case IOTX_DM_EVENT_PROPERTY_DESIRED_GET_REPLY: {
            char *property_data = NULL;
            lite_cjson_t lite_item_data;

            memset(&lite_item_data, 0, sizeof(lite_cjson_t));
            dm_utils_json_object_item(&lite, IOTX_LINKKIT_KEY_DATA, strlen(IOTX_LINKKIT_KEY_DATA), cJSON_Invalid,
                                      &lite_item_data);
            if (payload == NULL || lite_item_data.type != cJSON_Object) {
                return;
            }
            dm_log_debug("Current Data: %.*s", lite_item_data.value_length, lite_item_data.value);

            property_data = IMPL_LINKKIT_MALLOC(lite_item_data.value_length + 1);
            if (property_data == NULL) {
                dm_log_err("No Enough Memory");
                return;
            }
            memset(property_data, 0, lite_item_data.value_length + 1);
            memcpy(property_data, lite_item_data.value, lite_item_data.value_length);

            callback = iotx_event_callback(ITE_PROPERTY_DESIRED_GET_REPLY);
            if (callback) {
                ((int (*)(const char *, const int))callback)(property_data,
                        lite_item_data.value_length);
            }

            IMPL_LINKKIT_FREE(property_data);
        }
        break;
#endif
        case IOTX_DM_EVENT_PROPERTY_GET: {
            int response_len = 0;
            char *request = NULL, *response = NULL;
            uintptr_t property_get_ctx_num = 0;
            void *property_get_ctx = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_String || lite_item_devid.type != cJSON_Number ||
                lite_item_payload.type != cJSON_Array) {
                return;
            }

            dm_log_debug("Current Id: %.*s", lite_item_id.value_length, lite_item_id.value);
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);
            dm_log_debug("Current Ctx: %.*s", lite_item_ctx.value_length, lite_item_ctx.value);

            LITE_hexstr_convert(lite_item_ctx.value, lite_item_ctx.value_length, (unsigned char *)&property_get_ctx_num,
                                sizeof(uintptr_t));
            property_get_ctx = (void *)property_get_ctx_num;
            dm_log_debug("property_get_ctx_num: %0x016llX", (unsigned int)property_get_ctx_num);
            dm_log_debug("property_get_ctx: %p", property_get_ctx);

            request = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (request == NULL) {
                dm_log_err("No Enough Memory");
                return;
            }
            memset(request, 0, lite_item_payload.value_length + 1);
            memcpy(request, lite_item_payload.value, lite_item_payload.value_length);

            callback = iotx_event_callback(ITE_PROPERTY_GET);
            if (callback) {
                res = ((int (*)(const int, const char *, const int, char **, int *))callback)(lite_item_devid.value_int, request,
                        lite_item_payload.value_length, &response, &response_len);

                if (response != NULL && response_len > 0) {
                    /* property get response exist */
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_property_get_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                                       response, response_len, property_get_ctx);
                    HAL_Free(response);
                }
            }

            IMPL_LINKKIT_FREE(request);
        }
        break;
        case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY:
        case IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY:
#ifdef DEVICE_MODEL_SHADOW
        case IOTX_DM_EVENT_PROPERTY_DESIRED_DELETE_REPLY:
#endif
        case IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY: {
            char *user_payload = NULL;
            int user_payload_length = 0;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_code.type != cJSON_Number
                || lite_item_devid.type != cJSON_Number) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);
            dm_log_debug("Current Code: %d", lite_item_code.value_int);
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            if (lite_item_payload.type == cJSON_Object && lite_item_payload.value_length > 0) {
                user_payload = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
                if (user_payload == NULL) {
                    dm_log_err("No Enough Memory");
                    return;
                }
                memset(user_payload, 0, lite_item_payload.value_length + 1);
                memcpy(user_payload, lite_item_payload.value, lite_item_payload.value_length);
                user_payload_length = lite_item_payload.value_length;
            }

            callback = iotx_event_callback(ITE_REPORT_REPLY);
            if (callback) {
                ((int (*)(const int, const int, const int, const char *, const int))callback)(lite_item_devid.value_int,
                        lite_item_id.value_int, lite_item_code.value_int, user_payload,
                        user_payload_length);
            }

            if (user_payload) {
                IMPL_LINKKIT_FREE(user_payload);
            }
        }
        break;
        case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
            char *user_eventid = NULL;
            char *user_payload = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_code.type != cJSON_Number ||
                lite_item_devid.type != cJSON_Number || lite_item_eventid.type != cJSON_String
                || lite_item_payload.type != cJSON_String) {
                return;
            }

            dm_log_debug("Current Id: %d", lite_item_id.value_int);
            dm_log_debug("Current Code: %d", lite_item_code.value_int);
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current EventID: %.*s", lite_item_eventid.value_length, lite_item_eventid.value);
            dm_log_debug("Current Message: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            user_eventid = IMPL_LINKKIT_MALLOC(lite_item_eventid.value_length + 1);
            if (user_eventid == NULL) {
                dm_log_err("Not Enough Memory");
                return;
            }
            memset(user_eventid, 0, lite_item_eventid.value_length + 1);
            memcpy(user_eventid, lite_item_eventid.value, lite_item_eventid.value_length);

            user_payload = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (user_payload == NULL) {
                dm_log_err("Not Enough Memory");
                IMPL_LINKKIT_FREE(user_eventid);
                return;
            }
            memset(user_payload, 0, lite_item_payload.value_length + 1);
            memcpy(user_payload, lite_item_payload.value, lite_item_payload.value_length);


            callback = iotx_event_callback(ITE_TRIGGER_EVENT_REPLY);
            if (callback) {
                ((int (*)(const int, const int, const int, const char *, const int, const char *,
                          const int))callback)(lite_item_devid.value_int,
                                               lite_item_id.value_int, lite_item_code.value_int,
                                               user_eventid, lite_item_eventid.value_length, user_payload, lite_item_payload.value_length);
            }

            IMPL_LINKKIT_FREE(user_eventid);
            IMPL_LINKKIT_FREE(user_payload);
        }
        break;
        case IOTX_DM_EVENT_NTP_RESPONSE: {
            char *utc_payload = NULL;

            if (payload == NULL || lite_item_utc.type != cJSON_String) {
                return;
            }

            dm_log_debug("Current UTC: %.*s", lite_item_utc.value_length, lite_item_utc.value);

            utc_payload = IMPL_LINKKIT_MALLOC(lite_item_utc.value_length + 1);
            if (utc_payload == NULL) {
                dm_log_err("Not Enough Memory");
                return;
            }
            memset(utc_payload, 0, lite_item_utc.value_length + 1);
            memcpy(utc_payload, lite_item_utc.value, lite_item_utc.value_length);

            callback = iotx_event_callback(ITE_TIMESTAMP_REPLY);
            if (callback) {
                ((int (*)(const char *))callback)(utc_payload);
            }

            IMPL_LINKKIT_FREE(utc_payload);
        }
        break;
        case IOTX_DM_EVENT_RRPC_REQUEST: {
            int rrpc_response_len = 0;
            char *rrpc_request = NULL, *rrpc_response = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_String || lite_item_devid.type != cJSON_Number ||
                lite_item_serviceid.type != cJSON_String || lite_item_rrpcid.type != cJSON_String
                || lite_item_payload.type != cJSON_Object) {
                return;
            }

            dm_log_debug("Current Id: %.*s", lite_item_id.value_length, lite_item_id.value);
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);
            dm_log_debug("Current RRPC ID: %.*s", lite_item_rrpcid.value_length, lite_item_rrpcid.value);
            dm_log_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            rrpc_request = IMPL_LINKKIT_MALLOC(lite_item_payload.value_length + 1);
            if (rrpc_request == NULL) {
                dm_log_err("Not Enough Memory");
                return;
            }
            memset(rrpc_request, 0, lite_item_payload.value_length + 1);
            memcpy(rrpc_request, lite_item_payload.value, lite_item_payload.value_length);

            callback = iotx_event_callback(ITE_SERVICE_REQUEST);
            if (callback) {
                res = ((int (*)(const int, const char *, const int, const char *, const int, char **,
                                int *))callback)(lite_item_devid.value_int, lite_item_serviceid.value,
                                                 lite_item_serviceid.value_length,
                                                 rrpc_request, lite_item_payload.value_length, &rrpc_response, &rrpc_response_len);
                if (rrpc_response != NULL && rrpc_response_len > 0) {
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_rrpc_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                               lite_item_rrpcid.value,
                                               lite_item_rrpcid.value_length,
                                               rrpc_response, rrpc_response_len);
                    HAL_Free(rrpc_response);
                }
            }

            IMPL_LINKKIT_FREE(rrpc_request);
        }
        break;
#endif
        case IOTX_DM_EVENT_FOTA_NEW_FIRMWARE: {
            char *version = NULL;

            if (payload == NULL || lite_item_version.type != cJSON_String) {
                return;
            }

            dm_log_debug("Current Firmware Version: %.*s", lite_item_version.value_length, lite_item_version.value);

            version = IMPL_LINKKIT_MALLOC(lite_item_version.value_length + 1);
            if (version == NULL) {
                return;
            }
            memset(version, 0, lite_item_version.value_length + 1);
            memcpy(version, lite_item_version.value, lite_item_version.value_length);

            callback = iotx_event_callback(ITE_FOTA);
            if (callback) {
                ((int (*)(const int, const char *))callback)(0, version);
            }

            if (version) {
                IMPL_LINKKIT_FREE(version);
            }
        }
        break;
        case IOTX_DM_EVENT_COTA_NEW_CONFIG: {
            char *config_id = NULL, *get_type = NULL, *sign = NULL, *sign_method = NULL, *url = NULL;

            if (payload == NULL || lite_item_configid.type != cJSON_String || lite_item_configsize.type != cJSON_Number ||
                lite_item_gettype.type != cJSON_String || lite_item_sign.type != cJSON_String
                || lite_item_signmethod.type != cJSON_String ||
                lite_item_url.type != cJSON_String) {
                return;
            }

            dm_log_debug("Current Config ID: %.*s", lite_item_configid.value_length, lite_item_configid.value);
            dm_log_debug("Current Config Size: %d", lite_item_configsize.value_int);
            dm_log_debug("Current Get Type: %.*s", lite_item_gettype.value_length, lite_item_gettype.value);
            dm_log_debug("Current Sign: %.*s", lite_item_sign.value_length, lite_item_sign.value);
            dm_log_debug("Current Sign Method: %.*s", lite_item_signmethod.value_length, lite_item_signmethod.value);
            dm_log_debug("Current URL: %.*s", lite_item_url.value_length, lite_item_url.value);

            _impl_copy(lite_item_configid.value, lite_item_configid.value_length, (void **)&config_id,
                       lite_item_configid.value_length + 1);
            _impl_copy(lite_item_gettype.value, lite_item_gettype.value_length, (void **)&get_type,
                       lite_item_gettype.value_length + 1);
            _impl_copy(lite_item_sign.value, lite_item_sign.value_length, (void **)&sign, lite_item_sign.value_length + 1);
            _impl_copy(lite_item_signmethod.value, lite_item_signmethod.value_length, (void **)&sign_method,
                       lite_item_signmethod.value_length + 1);
            _impl_copy(lite_item_url.value, lite_item_url.value_length, (void **)&url, lite_item_url.value_length + 1);

            if (config_id == NULL || get_type == NULL || sign == NULL || sign_method == NULL || url == NULL) {
                if (config_id) {
                    IMPL_LINKKIT_FREE(config_id);
                }
                if (get_type) {
                    IMPL_LINKKIT_FREE(get_type);
                }
                if (sign) {
                    IMPL_LINKKIT_FREE(sign);
                }
                if (sign_method) {
                    IMPL_LINKKIT_FREE(sign_method);
                }
                if (url) {
                    IMPL_LINKKIT_FREE(url);
                }
                return;
            }

            callback = iotx_event_callback(ITE_COTA);
            if (callback) {
                ((int (*)(const int, const char *, int, const char *, const char *, const char *, const char *))callback)(0, config_id,
                        lite_item_configsize.value_int, get_type, sign, sign_method, url);
            }

            if (config_id) {
                IMPL_LINKKIT_FREE(config_id);
            }
            if (get_type) {
                IMPL_LINKKIT_FREE(get_type);
            }
            if (sign) {
                IMPL_LINKKIT_FREE(sign);
            }
            if (sign_method) {
                IMPL_LINKKIT_FREE(sign_method);
            }
            if (url) {
                IMPL_LINKKIT_FREE(url);
            }
        }
        break;
#ifdef DEVICE_MODEL_GATEWAY
        case IOTX_DM_EVENT_TOPO_GET_REPLY: {
            char *topo_list = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_devid.type != cJSON_Number ||
                lite_item_code.type != cJSON_Number || lite_item_topo.type != cJSON_Array) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);
            dm_log_debug("Current Code: %d", lite_item_code.value_int);
            dm_log_debug("Current Topo List: %.*s", lite_item_topo.value_length, lite_item_topo.value);

            topo_list = IMPL_LINKKIT_MALLOC(lite_item_topo.value_length + 1);
            if (topo_list == NULL) {
                dm_log_err("Not Enough Memory");
                return;
            }
            memset(topo_list, 0, lite_item_topo.value_length + 1);
            memcpy(topo_list, lite_item_topo.value, lite_item_topo.value_length);

            callback = iotx_event_callback(ITE_TOPOLIST_REPLY);
            if (callback) {
                ((int (*)(const int, const int, const int, const char *, const int))callback)(lite_item_devid.value_int,
                        lite_item_id.value_int,
                        lite_item_code.value_int, topo_list, lite_item_topo.value_length);
            }

            IMPL_LINKKIT_FREE(topo_list);
        }
        break;
        case IOTX_DM_EVENT_TOPO_DELETE_REPLY:
        case IOTX_DM_EVENT_TOPO_ADD_REPLY:
        case IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY:
        case IOTX_DM_EVENT_COMBINE_LOGIN_REPLY:
        case IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY: {
            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_devid.type != cJSON_Number ||
                lite_item_code.type != cJSON_Number) {
                return;
            }
            dm_log_debug("Current Id: %d", lite_item_id.value_int);
            dm_log_debug("Current Code: %d", lite_item_code.value_int);
            dm_log_debug("Current Devid: %d", lite_item_devid.value_int);

            _iotx_linkkit_upstream_mutex_lock();
            _iotx_linkkit_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _iotx_linkkit_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_GATEWAY_PERMIT: {
            char *product_key = "";

            if (payload == NULL || lite_item_time.type != cJSON_Number) {
                return;
            }
            dm_log_debug("Current Time: %d", lite_item_time.value_int);

            if (lite_item_pk.type == cJSON_String) {
                dm_log_debug("Current Product Key: %.*s", lite_item_pk.value_length, lite_item_pk.value);
                product_key = IMPL_LINKKIT_MALLOC(lite_item_pk.value_length + 1);
                if (product_key == NULL) {
                    dm_log_err("Not Enough Memory");
                    return;
                }
                memset(product_key, 0, lite_item_pk.value_length + 1);
                memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);
            }

            callback = iotx_event_callback(ITE_PERMIT_JOIN);
            if (callback) {
                ((int (*)(const char *, int))callback)((const char *)product_key, (const int)lite_item_time.value_int);
            }

            if (lite_item_pk.type == cJSON_String) {
                IMPL_LINKKIT_FREE(product_key);
            }
        }
        break;
#endif
        default: {
        }
        break;
    }
}

static int _iotx_linkkit_master_open(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (ctx->is_opened) {
        return FAIL_RETURN;
    }
    ctx->is_opened = 1;

    HAL_SetProductKey(meta_info->product_key);
    HAL_SetProductSecret(meta_info->product_secret);
    HAL_SetDeviceName(meta_info->device_name);
    HAL_SetDeviceSecret(meta_info->device_secret);

    /* Create Mutex */
    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        dm_log_err("Not Enough Memory");
        ctx->is_opened = 0;
        return FAIL_RETURN;
    }

#ifdef DEVICE_MODEL_GATEWAY
    ctx->upstream_mutex = HAL_MutexCreate();
    if (ctx->upstream_mutex == NULL) {
        HAL_MutexDestroy(ctx->mutex);
        dm_log_err("Not Enough Memory");
        ctx->is_opened = 0;
        return FAIL_RETURN;
    }
#endif

    res = iotx_dm_open();
    if (res != SUCCESS_RETURN) {
#ifdef DEVICE_MODEL_GATEWAY
        HAL_MutexDestroy(ctx->upstream_mutex);
#endif
        HAL_MutexDestroy(ctx->mutex);
        ctx->is_opened = 0;
        return FAIL_RETURN;
    }

    INIT_LIST_HEAD(&ctx->upstream_sync_callback_list);

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_slave_open(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0, devid;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (!ctx->is_opened) {
        return FAIL_RETURN;
    }

    res = iotx_dm_subdev_create(meta_info->product_key, meta_info->device_name, meta_info->device_secret, &devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return devid;
}
#endif

static int _iotx_linkkit_master_connect(void)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_dm_init_params_t dm_init_params;
    iotx_dm_event_types_t type;

    if (ctx->is_connected) {
        return FAIL_RETURN;
    }
    ctx->is_connected = 1;

    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.event_callback = _iotx_linkkit_event_callback;

    res = iotx_dm_connect(&dm_init_params);
    if (res != SUCCESS_RETURN) {
        dm_log_err("DM Start Failed");
        ctx->is_connected = 0;
        return FAIL_RETURN;
    }

    res = iotx_dm_subscribe(IOTX_DM_LOCAL_NODE_DEVID);
    if (res != SUCCESS_RETURN) {
        dm_log_err("DM Subscribe Failed");
        ctx->is_connected = 0;
        return FAIL_RETURN;
    }

    type = IOTX_DM_EVENT_INITIALIZED;
    _iotx_linkkit_event_callback(type, "{\"devid\":0}");

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_slave_connect(int devid)
{
    int res = 0, msgid = 0, code = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;

    if (ctx->is_connected == 0) {
        dm_log_err("master isn't start");
        return FAIL_RETURN;
    }

    if (devid <= 0) {
        dm_log_err("devid invalid");
        return FAIL_RETURN;
    }

    /* Subdev Register */
    res = iotx_dm_subdev_register(devid);
    if (res < SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (res > SUCCESS_RETURN) {
        semaphore = HAL_SemaphoreCreate();
        if (semaphore == NULL) {
            return FAIL_RETURN;
        }

        msgid = res;

        _iotx_linkkit_upstream_mutex_lock();
        res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
        if (res != SUCCESS_RETURN) {
            HAL_SemaphoreDestroy(semaphore);
            _iotx_linkkit_upstream_mutex_unlock();
            return FAIL_RETURN;
        }
        _iotx_linkkit_upstream_mutex_unlock();

        res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
        if (res < SUCCESS_RETURN) {
            _iotx_linkkit_upstream_mutex_lock();
            _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
            _iotx_linkkit_upstream_mutex_unlock();
            return FAIL_RETURN;
        }

        _iotx_linkkit_upstream_mutex_lock();
        code = node->code;
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        if (code != SUCCESS_RETURN) {
            _iotx_linkkit_upstream_mutex_unlock();
            return FAIL_RETURN;
        }
        _iotx_linkkit_upstream_mutex_unlock();
    }

    /* Subdev Add Topo */
    res = iotx_dm_subdev_topo_add(devid);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_mutex_unlock();
        return FAIL_RETURN;
    }
    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _iotx_linkkit_mutex_unlock();
        return FAIL_RETURN;
    }

    msgid = res;
    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    return SUCCESS_RETURN;
}

static int _iotx_linkkit_subdev_delete_topo(int devid)
{
    int res = 0, msgid = 0, code = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;

    if (ctx->is_connected == 0) {
        dm_log_err("master isn't start");
        return FAIL_RETURN;
    }

    if (devid <= 0) {
        dm_log_err("devid invalid");
        return FAIL_RETURN;
    }

    /* Subdev Delete Topo */
    res = iotx_dm_subdev_topo_del(devid);
    if (res < SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    return SUCCESS_RETURN;
}
#endif

static int _iotx_linkkit_master_close(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    _iotx_linkkit_mutex_lock();
    if (ctx->is_opened == 0) {
        _iotx_linkkit_mutex_unlock();
        return FAIL_RETURN;
    }
    ctx->is_opened = 0;

    iotx_dm_close();
#ifdef DEVICE_MODEL_GATEWAY
    _iotx_linkkit_upstream_sync_callback_list_destroy();
    HAL_MutexDestroy(ctx->upstream_mutex);
#endif
    _iotx_linkkit_mutex_unlock();
    HAL_MutexDestroy(ctx->mutex);
    memset(ctx, 0, sizeof(iotx_linkkit_ctx_t));

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_slave_close(int devid)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    _iotx_linkkit_mutex_lock();
    if (ctx->is_opened == 0) {
        _iotx_linkkit_mutex_unlock();
        return FAIL_RETURN;
    }

    /* Release Subdev Resources */
    iotx_dm_subdev_destroy(devid);

    _iotx_linkkit_mutex_unlock();

    return SUCCESS_RETURN;
}
#endif

int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0;

    if (dev_type < 0 || dev_type >= IOTX_LINKKIT_DEV_TYPE_MAX || meta_info == NULL) {
        dm_log_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    switch (dev_type) {
        case IOTX_LINKKIT_DEV_TYPE_MASTER: {
            res = _iotx_linkkit_master_open(meta_info);
            if (res == SUCCESS_RETURN) {
                res = IOTX_DM_LOCAL_NODE_DEVID;
            }
        }
        break;
        case IOTX_LINKKIT_DEV_TYPE_SLAVE: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_slave_open(meta_info);
#else
            res = FAIL_RETURN;
#endif
        }
        break;
        default: {
            dm_log_err("Unknown Device Type");
            res = FAIL_RETURN;
        }
        break;
    }

    return res;
}

int IOT_Linkkit_Connect(int devid)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0) {
        dm_log_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0) {

        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();

    if (devid == IOTX_DM_LOCAL_NODE_DEVID) {
        res = _iotx_linkkit_master_connect();
    } else {
#ifdef DEVICE_MODEL_GATEWAY
        res = _iotx_linkkit_slave_connect(devid);
#else
        res = FAIL_RETURN;
#endif
    }
    _iotx_linkkit_mutex_unlock();

    return res;
}

void IOT_Linkkit_Yield(int timeout_ms)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (timeout_ms <= 0) {
        dm_log_err("Invalid Parameter");
        return;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return;
    }

    iotx_dm_yield(timeout_ms);
    iotx_dm_dispatch();

#ifdef DEVICE_MODEL_GATEWAY
    HAL_SleepMs(timeout_ms);
#endif
}

int IOT_Linkkit_Close(int devid)
{
    int res = 0;

    if (devid < 0) {
        dm_log_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (devid == IOTX_DM_LOCAL_NODE_DEVID) {
        res = _iotx_linkkit_master_close();
#ifdef DEV_BIND_ENABLED
        awss_bind_deinit();
#endif
    } else {
#ifdef DEVICE_MODEL_GATEWAY
        res = _iotx_linkkit_slave_close(devid);
#else
        res = FAIL_RETURN;
#endif
    }

    return res;
}

#ifdef DEVICE_MODEL_GATEWAY
static int _iotx_linkkit_subdev_login(int devid)
{
    int res = 0, msgid = 0, code = 0;
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;
    void *callback = NULL;

    res = iotx_dm_subdev_login(devid);
    if (res < SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    msgid = res;
    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = iotx_dm_subscribe(devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    iotx_dm_send_aos_active(devid);
    callback = iotx_event_callback(ITE_INITIALIZE_COMPLETED);
    if (callback) {
        ((int (*)(const int))callback)(devid);
    }

    return res;
}

static int _iotx_linkkit_subdev_logout(int devid)
{
    int res = 0, msgid = 0, code = 0;
    iotx_linkkit_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;

    res = iotx_dm_subdev_logout(devid);
    if (res < SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    msgid = res;
    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_upstream_mutex_lock();
    res = _iotx_linkkit_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, IOTX_LINKKIT_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_lock();
        _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }

    _iotx_linkkit_upstream_mutex_lock();
    code = node->code;
    _iotx_linkkit_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _iotx_linkkit_upstream_mutex_unlock();
        return FAIL_RETURN;
    }
    _iotx_linkkit_upstream_mutex_unlock();

    return res;
}
#endif

int IOT_Linkkit_Report(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || msg_type < 0 || msg_type >= IOTX_LINKKIT_MSG_MAX) {
        dm_log_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();
    switch (msg_type) {
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        case ITM_MSG_POST_PROPERTY: {
            if (payload == NULL || payload_len <= 0) {
                dm_log_err("Invalid Parameter");
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
            res = iotx_dm_post_property(devid, (char *)payload, payload_len);
#ifdef LOG_REPORT_TO_CLOUD
            if (1 == report_sample) {
                send_permance_info(NULL, 0, "4", 1);
            }
#endif
        }
        break;
#ifdef DEVICE_MODEL_SHADOW
        case ITM_MSG_PROPERTY_DESIRED_GET: {
            if (payload == NULL || payload_len <= 0) {
                dm_log_err("Invalid Parameter");
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
            res = iotx_dm_property_desired_get(devid, (char *)payload, payload_len);
        }
        break;
        case ITM_MSG_PROPERTY_DESIRED_DELETE: {
            if (payload == NULL || payload_len <= 0) {
                dm_log_err("Invalid Parameter");
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
            res = iotx_dm_property_desired_delete(devid, (char *)payload, payload_len);
        }
        break;
#endif
        case ITM_MSG_DEVICEINFO_UPDATE: {
            if (payload == NULL || payload_len <= 0) {
                dm_log_err("Invalid Parameter");
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
            res = iotx_dm_deviceinfo_update(devid, (char *)payload, payload_len);
        }
        break;
        case ITM_MSG_DEVICEINFO_DELETE: {
            if (payload == NULL || payload_len <= 0) {
                dm_log_err("Invalid Parameter");
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
            res = iotx_dm_deviceinfo_delete(devid, (char *)payload, payload_len);
        }
        break;
#endif
        case ITM_MSG_POST_RAW_DATA: {
            if (payload == NULL || payload_len <= 0) {
                dm_log_err("Invalid Parameter");
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
            res = iotx_dm_post_rawdata(devid, (char *)payload, payload_len);
        }
        break;
        case ITM_MSG_LOGIN: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_subdev_login(devid);
            if (res != SUCCESS_RETURN) {
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
#else
            res = FAIL_RETURN;
#endif
        }
        break;
        case ITM_MSG_LOGOUT: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_subdev_logout(devid);
            if (res != SUCCESS_RETURN) {
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
#else
            res = FAIL_RETURN;
#endif
        }
        break;
        case ITM_MSG_DELETE_TOPO: {
#ifdef DEVICE_MODEL_GATEWAY
            res = _iotx_linkkit_subdev_delete_topo(devid);
            if (res != SUCCESS_RETURN) {
                _iotx_linkkit_mutex_unlock();
                return FAIL_RETURN;
            }
#else
            res = FAIL_RETURN;
#endif
        }
        break;
#ifdef DEVICE_MODEL_GATEWAY
#ifdef DEVICE_MODEL_SUBDEV_OTA
        case ITM_MSG_REPORT_SUBDEV_FIRMWARE_VERSION: {
            res = iotx_dm_send_firmware_version(devid, (const char *)payload);
        }
        break;
#endif
#endif
        default: {
            dm_log_err("Unknown Message Type");
            res = FAIL_RETURN;
        }
        break;
    }
    _iotx_linkkit_mutex_unlock();
    return res;
}

int IOT_Linkkit_Query(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || msg_type < 0 || msg_type >= IOTX_LINKKIT_MSG_MAX) {
        dm_log_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();
    switch (msg_type) {
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
        case ITM_MSG_QUERY_TIMESTAMP: {
            res = iotx_dm_qurey_ntp();
        }
        break;
#endif
        case ITM_MSG_QUERY_TOPOLIST: {
#ifdef DEVICE_MODEL_GATEWAY
            res = iotx_dm_query_topo_list();
#else
            res = FAIL_RETURN;
#endif
        }
        break;
        case ITM_MSG_QUERY_FOTA_DATA: {
            res = iotx_dm_fota_perform_sync((char *)payload, payload_len);
        }
        break;
        case ITM_MSG_QUERY_COTA_DATA: {
            res = iotx_dm_cota_perform_sync((char *)payload, payload_len);
        }
        break;
        case ITM_MSG_REQUEST_COTA: {
            res = iotx_dm_cota_get_config("product", "file", "");
        }
        break;
        case ITM_MSG_REQUEST_FOTA_IMAGE: {
            res = iotx_dm_fota_request_image((const char *)payload, payload_len);
        }
        break;
        default: {
            dm_log_err("Unknown Message Type");
            res = FAIL_RETURN;
        }
        break;
    }
    _iotx_linkkit_mutex_unlock();
    return res;
}

int IOT_Linkkit_TriggerEvent(int devid, char *eventid, int eventid_len, char *payload, int payload_len)
{
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || eventid == NULL || eventid_len <= 0 || payload == NULL || payload_len <= 0) {
        dm_log_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0 || ctx->is_connected == 0) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();
    res = iotx_dm_post_event(devid, eventid, eventid_len, payload, payload_len);
    _iotx_linkkit_mutex_unlock();

    return res;
#else
    return -1;
#endif
}

#ifdef DEVICE_MODEL_GATEWAY
int iot_linkkit_subdev_query_id(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = -1;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (ctx->is_opened == 0) {
        return res;
    }

    iotx_dm_subdev_query(product_key, device_name, &res);
    return res;
}
#endif /* #ifdef DEVICE_MODEL_GATEWAY */

#endif
