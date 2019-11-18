/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "iotx_dm_internal.h"

#ifdef DEPRECATED_LINKKIT

#include "linkkit_export.h"
#include "impl_solo.h"

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define IMPL_SOLO_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "impl.solo")
    #define IMPL_SOLO_FREE(ptr)               LITE_free(ptr)
#else
    #define IMPL_SOLO_MALLOC(size)            HAL_Malloc(size)
    #define IMPL_SOLO_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define impl_solo_err(...)       log_err("impl.solo", __VA_ARGS__)
    #define impl_solo_info(...)      log_info("impl.solo", __VA_ARGS__)
    #define impl_solo_debug(...)     log_debug("impl.solo", __VA_ARGS__)
#else
    #define impl_solo_err(...)
    #define impl_solo_info(...)
    #define impl_solo_debug(...)
#endif

linkkit_solo_legacy_ctx_t g_linkkit_solo_legacy_ctx = {0};

static being_deprecated linkkit_solo_legacy_ctx_t *_linkkit_solo_legacy_get_ctx(void)
{
    return &g_linkkit_solo_legacy_ctx;
}

static void _linkkit_solo_mutex_lock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->mutex) {
        HAL_MutexLock(linkkit_solo_ctx->mutex);
    }
}

static void _linkkit_solo_mutex_unlock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->mutex) {
        HAL_MutexUnlock(linkkit_solo_ctx->mutex);
    }
}

static void _linkkit_solo_upstream_mutex_lock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->upstream_mutex) {
        HAL_MutexLock(linkkit_solo_ctx->upstream_mutex);
    }
}

static void _linkkit_solo_upstream_mutex_unlock(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    if (linkkit_solo_ctx->upstream_mutex) {
        HAL_MutexUnlock(linkkit_solo_ctx->upstream_mutex);
    }
}

static int _impl_copy(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len)
{
    if (input == NULL || output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *output = IMPL_SOLO_MALLOC(output_len);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, output_len);
    memcpy(*output, input, input_len);

    return SUCCESS_RETURN;
}

static int _linkkit_solo_upstream_callback_list_insert(int msgid, handle_post_cb_fp_t callback)
{
    int count = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_solo_ctx->callback_list, linked_list, linkkit_solo_upstream_callback_node_t) {
        count++;
        if (search_node->msgid == msgid) {
            impl_solo_info("Message ID Already Exist: %d", msgid);
            return FAIL_RETURN;
        }
    }

    impl_solo_info("linkkit_solo_upstream_callback_list node count: %d", count);

    search_node = IMPL_SOLO_MALLOC(sizeof(linkkit_solo_upstream_callback_node_t));
    if (search_node == NULL) {
        return FAIL_RETURN;
    }
    memset(search_node, 0, sizeof(linkkit_solo_upstream_callback_node_t));

    search_node->msgid = msgid;
    search_node->callback = callback;
    search_node->callback = callback;
    INIT_LIST_HEAD(&search_node->linked_list);

    list_add(&search_node->linked_list, &linkkit_solo_ctx->callback_list);

    return SUCCESS_RETURN;
}

static int _linkkit_solo_upstream_callback_list_remove(int msgid)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_solo_ctx->callback_list, linked_list, linkkit_solo_upstream_callback_node_t) {
        if (search_node->msgid == msgid) {
            list_del(&search_node->linked_list);
            IMPL_SOLO_FREE(search_node);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _linkkit_solo_upstream_callback_list_search(int msgid, linkkit_solo_upstream_callback_node_t **node)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_solo_ctx->callback_list, linked_list, linkkit_solo_upstream_callback_node_t) {
        if (search_node->msgid == msgid) {
            *node = search_node;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _linkkit_solo_upstream_callback_list_destroy(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    linkkit_solo_upstream_callback_node_t *search_node = NULL, *next_node = NULL;

    list_for_each_entry_safe(search_node, next_node, &linkkit_solo_ctx->callback_list, linked_list,
                             linkkit_solo_upstream_callback_node_t) {
        list_del(&search_node->linked_list);
        IMPL_SOLO_FREE(search_node);
    }

    return FAIL_RETURN;
}

void *linkkit_dispatch(void)
{
    iotx_dm_dispatch();
    return NULL;
}

int being_deprecated linkkit_is_try_leave(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    return linkkit_solo_ctx->is_leaved;
}

int being_deprecated linkkit_set_opt(linkkit_opt_t opt, void *data)
{
    int res = 0;

    if (data == NULL) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_set_opt(opt, data);
    _linkkit_solo_mutex_unlock();

    return res;
}

static void _linkkit_solo_event_callback(iotx_dm_event_types_t type, char *payload)
{
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid, lite_item_payload, lite_item_serviceid;
    lite_cjson_t lite_item_propertyid, lite_item_eventid, lite_item_configid, lite_item_configsize, lite_item_gettype;
    lite_cjson_t lite_item_sign, lite_item_signmethod, lite_item_url, lite_item_version;

    impl_solo_info("Receive Message Type: %d", type);
    if (payload) {
        impl_solo_info("Receive Message: %s", payload);
        res = dm_utils_json_parse(payload, strlen(payload), cJSON_Invalid, &lite);
        if (res != SUCCESS_RETURN) {
            return;
        }
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_ID, strlen(LINKKIT_SOLO_LEGACY_KEY_ID), cJSON_Invalid,
                                  &lite_item_id);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_CODE, strlen(LINKKIT_SOLO_LEGACY_KEY_CODE), cJSON_Invalid,
                                  &lite_item_code);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_DEVID, strlen(LINKKIT_SOLO_LEGACY_KEY_DEVID), cJSON_Invalid,
                                  &lite_item_devid);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_PAYLOAD, strlen(LINKKIT_SOLO_LEGACY_KEY_PAYLOAD),
                                  cJSON_Invalid, &lite_item_payload);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_SERVICEID, strlen(LINKKIT_SOLO_LEGACY_KEY_SERVICEID),
                                  cJSON_Invalid, &lite_item_serviceid);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_PROPERTYID, strlen(LINKKIT_SOLO_LEGACY_KEY_PROPERTYID),
                                  cJSON_Invalid, &lite_item_propertyid);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_EVENTID, strlen(LINKKIT_SOLO_LEGACY_KEY_EVENTID),
                                  cJSON_Invalid, &lite_item_eventid);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_CONFIG_ID, strlen(LINKKIT_SOLO_LEGACY_KEY_CONFIG_ID),
                                  cJSON_Invalid, &lite_item_configid);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_CONFIG_SIZE, strlen(LINKKIT_SOLO_LEGACY_KEY_CONFIG_SIZE),
                                  cJSON_Invalid, &lite_item_configsize);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_GET_TYPE, strlen(LINKKIT_SOLO_LEGACY_KEY_GET_TYPE),
                                  cJSON_Invalid, &lite_item_gettype);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_SIGN, strlen(LINKKIT_SOLO_LEGACY_KEY_SIGN), cJSON_Invalid,
                                  &lite_item_sign);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_SIGN_METHOD, strlen(LINKKIT_SOLO_LEGACY_KEY_SIGN_METHOD),
                                  cJSON_Invalid, &lite_item_signmethod);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_URL, strlen(LINKKIT_SOLO_LEGACY_KEY_URL), cJSON_Invalid,
                                  &lite_item_url);
        dm_utils_json_object_item(&lite, LINKKIT_SOLO_LEGACY_KEY_VERSION, strlen(LINKKIT_SOLO_LEGACY_KEY_VERSION),
                                  cJSON_Invalid, &lite_item_version);
    }

    switch (type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            if (linkkit_solo_ctx->user_callback->on_connect) {
                linkkit_solo_ctx->user_callback->on_connect(linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            if (linkkit_solo_ctx->user_callback->on_disconnect) {
                linkkit_solo_ctx->user_callback->on_disconnect(linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
            int res = 0, raw_data_len = 0;
            void *thing_id = NULL;
            unsigned char *raw_data = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_payload.type != cJSON_String) {
                return;
            }

            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);
            impl_solo_debug("Current Raw Data: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            res = dm_utils_str_to_hex(lite_item_payload.value, lite_item_payload.value_length, &raw_data, &raw_data_len);
            if (res != SUCCESS_RETURN) {
                return;
            }
            HEXDUMP_DEBUG(raw_data, raw_data_len);

            if (linkkit_solo_ctx->user_callback->raw_data_arrived) {
                linkkit_solo_ctx->user_callback->raw_data_arrived(thing_id, raw_data, raw_data_len, linkkit_solo_ctx->user_context);
            }

            IMPL_SOLO_FREE(raw_data);
        }
        break;
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            int res = 0;
            void *thing_id = NULL;
            char *service = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_devid.type != cJSON_Number ||
                lite_item_serviceid.type != cJSON_String) {
                return;
            }

            impl_solo_debug("Current Id: %d", lite_item_id.value_int);
            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);
            impl_solo_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            service = IMPL_SOLO_MALLOC(lite_item_serviceid.value_length + 1);
            if (service == NULL) {
                return;
            }
            memset(service, 0, lite_item_serviceid.value_length + 1);
            memcpy(service, lite_item_serviceid.value, lite_item_serviceid.value_length);

            if (linkkit_solo_ctx->user_callback->thing_call_service) {
                linkkit_solo_ctx->user_callback->thing_call_service(thing_id, (const char *)service, lite_item_id.value_int,
                        linkkit_solo_ctx->user_context);
            }

            IMPL_SOLO_FREE(service);
        }
        break;
        case IOTX_DM_EVENT_LEGACY_THING_CREATED: {
            int res = 0;
            void *thing_id = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number) {
                return;
            }

            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            if (linkkit_solo_ctx->user_callback->thing_create) {
                linkkit_solo_ctx->user_callback->thing_create(thing_id, linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_THING_DISABLE: {
            int res = 0;
            void *thing_id = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number) {
                return;
            }

            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            if (linkkit_solo_ctx->user_callback->thing_disable) {
                linkkit_solo_ctx->user_callback->thing_disable(thing_id, linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_THING_ENABLE: {
            int res = 0;
            void *thing_id = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number) {
                return;
            }

            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            if (linkkit_solo_ctx->user_callback->thing_enable) {
                linkkit_solo_ctx->user_callback->thing_enable(thing_id, linkkit_solo_ctx->user_context);
            }
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            int res = 0;
            void *thing_id = NULL;
            char *propertyid = NULL;

            if (payload == NULL || lite_item_devid.type != cJSON_Number || lite_item_propertyid.type != cJSON_String) {
                return;
            }

            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);
            impl_solo_debug("Current PropertyID: %.*s", lite_item_propertyid.value_length, lite_item_propertyid.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            propertyid = IMPL_SOLO_MALLOC(lite_item_propertyid.value_length + 1);
            if (propertyid == NULL) {
                return;
            }
            memset(propertyid, 0, lite_item_propertyid.value_length + 1);
            memcpy(propertyid, lite_item_propertyid.value, lite_item_propertyid.value_length);

            if (linkkit_solo_ctx->user_callback->thing_prop_changed) {
                linkkit_solo_ctx->user_callback->thing_prop_changed(thing_id, propertyid, linkkit_solo_ctx->user_context);
            }

            IMPL_SOLO_FREE(propertyid);
        }
        break;
        case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY: {
            int res = 0;
            void *thing_id = NULL;
            linkkit_solo_upstream_callback_node_t *node = NULL;
            handle_post_cb_fp_t callback = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_code.type != cJSON_Number ||
                lite_item_devid.type != cJSON_Number) {
                return;
            }

            impl_solo_debug("Current Id: %d", lite_item_id.value_int);
            impl_solo_debug("Current Code: %d", lite_item_code.value_int);
            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            _linkkit_solo_upstream_mutex_lock();
            res = _linkkit_solo_upstream_callback_list_search(lite_item_id.value_int, &node);
            if (res == SUCCESS_RETURN) {
                callback = node->callback;
            }
            _linkkit_solo_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (callback) {
                    callback(thing_id, lite_item_id.value_int, lite_item_code.value_int, NULL, linkkit_solo_ctx->user_context);
                }
                _linkkit_solo_upstream_mutex_lock();
                _linkkit_solo_upstream_callback_list_remove(lite_item_id.value_int);
                _linkkit_solo_upstream_mutex_unlock();
            }
        }
        break;
        case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
            int res = 0;
            void *thing_id = NULL;
            linkkit_solo_upstream_callback_node_t *node = NULL;
            handle_post_cb_fp_t callback = NULL;

            if (payload == NULL || lite_item_id.type != cJSON_Number || lite_item_code.type != cJSON_Number ||
                lite_item_devid.type != cJSON_Number || lite_item_eventid.type != cJSON_String) {
                return;
            }

            impl_solo_debug("Current Id: %d", lite_item_id.value_int);
            impl_solo_debug("Current Code: %d", lite_item_code.value_int);
            impl_solo_debug("Current Devid: %d", lite_item_devid.value_int);
            impl_solo_debug("Current EventID: %.*s", lite_item_eventid.value_length, lite_item_eventid.value);

            res = iotx_dm_deprecated_legacy_get_thingid_by_devid(lite_item_devid.value_int, &thing_id);
            if (res != SUCCESS_RETURN) {
                return;
            }

            _linkkit_solo_upstream_mutex_lock();
            res = _linkkit_solo_upstream_callback_list_search(lite_item_id.value_int, &node);
            if (res == SUCCESS_RETURN) {
                callback = node->callback;
            }
            _linkkit_solo_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (callback) {
                    callback(thing_id, lite_item_id.value_int, lite_item_code.value_int, NULL, linkkit_solo_ctx->user_context);
                }
                _linkkit_solo_upstream_mutex_lock();
                _linkkit_solo_upstream_callback_list_remove(lite_item_id.value_int);
                _linkkit_solo_upstream_mutex_unlock();
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

            impl_solo_debug("Current Config ID: %.*s", lite_item_configid.value_length, lite_item_configid.value);
            impl_solo_debug("Current Config Size: %d", lite_item_configsize.value_int);
            impl_solo_debug("Current Get Type: %.*s", lite_item_gettype.value_length, lite_item_gettype.value);
            impl_solo_debug("Current Sign: %.*s", lite_item_sign.value_length, lite_item_sign.value);
            impl_solo_debug("Current Sign Method: %.*s", lite_item_signmethod.value_length, lite_item_signmethod.value);
            impl_solo_debug("Current URL: %.*s", lite_item_url.value_length, lite_item_url.value);

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
                    IMPL_SOLO_FREE(config_id);
                }
                if (get_type) {
                    IMPL_SOLO_FREE(get_type);
                }
                if (sign) {
                    IMPL_SOLO_FREE(sign);
                }
                if (sign_method) {
                    IMPL_SOLO_FREE(sign_method);
                }
                if (url) {
                    IMPL_SOLO_FREE(url);
                }
                return;
            }

            if (linkkit_solo_ctx->cota_callback) {
                linkkit_solo_ctx->cota_callback(service_cota_callback_type_new_version_detected, config_id,
                                                lite_item_configsize.value_int, get_type, sign, sign_method, url);
            }

            if (config_id) {
                IMPL_SOLO_FREE(config_id);
            }
            if (get_type) {
                IMPL_SOLO_FREE(get_type);
            }
            if (sign) {
                IMPL_SOLO_FREE(sign);
            }
            if (sign_method) {
                IMPL_SOLO_FREE(sign_method);
            }
            if (url) {
                IMPL_SOLO_FREE(url);
            }
        }
        break;
        case IOTX_DM_EVENT_FOTA_NEW_FIRMWARE: {
            char *version = NULL;

            if (payload == NULL || lite_item_version.type != cJSON_String) {
                return;
            }

            impl_solo_debug("Current Firmware Version: %.*s", lite_item_version.value_length, lite_item_version.value);

            _impl_copy(lite_item_version.value, lite_item_version.value_length, (void **)&version,
                       lite_item_version.value_length + 1);
            if (version == NULL) {
                return;
            }

            if (linkkit_solo_ctx->fota_callback) {
                linkkit_solo_ctx->fota_callback(service_fota_callback_type_new_version_detected, version);
            }

            if (version) {
                IMPL_SOLO_FREE(version);
            }
        }
        break;
#ifdef LOCAL_CONN_ENABLE
        case IOTX_DM_EVENT_LOCAL_CONNECTED: {
            if (linkkit_solo_ctx->on_connect) {
                linkkit_solo_ctx->on_connect(context, 0);
            }
        }
        break;
        case IOTX_DM_EVENT_LOCAL_DISCONNECT: {
            if (linkkit_solo_ctx->on_connect) {
                linkkit_solo_ctx->on_connect(context, 0);
            }
        }
        break;
#endif
        default:
            break;
    }
}

int being_deprecated linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level,
                                   linkkit_ops_t *ops,
                                   linkkit_cloud_domain_type_t domain_type, void *user_context)
{
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();
    iotx_dm_init_params_t dm_init_params;

    if (linkkit_solo_ctx->is_started == 1) {
        return FAIL_RETURN;
    }
    linkkit_solo_ctx->is_started = 1;

    if (max_buffered_msg <= 0 || ops == NULL || log_level > LOG_DEBUG_LEVEL ||
        domain_type >= linkkit_cloud_domain_max) {
        impl_solo_err("Invalid Parameter");
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Create Mutex */
    linkkit_solo_ctx->mutex = HAL_MutexCreate();
    if (linkkit_solo_ctx->mutex == NULL) {
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    linkkit_solo_ctx->upstream_mutex = HAL_MutexCreate();
    if (linkkit_solo_ctx->upstream_mutex == NULL) {
        HAL_MutexDestroy(linkkit_solo_ctx->mutex);
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Set Linkkit Log Level */
    IOT_SetLogLevel(log_level);

    /* Set Region */
    IOT_Ioctl(IOTX_IOCTL_SET_REGION, &domain_type);

    /* Initialize Device Manager */
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = (iotx_dm_cloud_domain_types_t)domain_type;
    dm_init_params.event_callback = _linkkit_solo_event_callback;

    res = iotx_dm_open();
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_solo_ctx->mutex);
        HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    res = iotx_dm_connect(&dm_init_params);
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_solo_ctx->mutex);
        HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
        iotx_dm_close();
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    res = iotx_dm_subscribe(IOTX_DM_LOCAL_NODE_DEVID);
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_solo_ctx->mutex);
        HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
        iotx_dm_close();
        linkkit_solo_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Get TSL From Cloud If Need */
    if (get_tsl_from_cloud != 0) {
        res = iotx_dm_deprecated_set_tsl(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_CLOUD, NULL, 0);
        if (res < SUCCESS_RETURN) {
            HAL_MutexDestroy(linkkit_solo_ctx->mutex);
            HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
            iotx_dm_close();
            linkkit_solo_ctx->is_started = 0;
            return FAIL_RETURN;
        }
    }

    /* Init Message Callback List */
    INIT_LIST_HEAD(&linkkit_solo_ctx->callback_list);

    linkkit_solo_ctx->user_callback = ops;
    linkkit_solo_ctx->user_context = user_context;
    linkkit_solo_ctx->is_leaved = 0;

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_end(void)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    _linkkit_solo_upstream_mutex_lock();
    linkkit_solo_ctx->is_started = 0;
    _linkkit_solo_upstream_callback_list_destroy();
    _linkkit_solo_upstream_mutex_unlock();

    iotx_dm_close();
    _linkkit_solo_mutex_unlock();

    HAL_MutexDestroy(linkkit_solo_ctx->upstream_mutex);
    HAL_MutexDestroy(linkkit_solo_ctx->mutex);

    linkkit_solo_ctx->mutex = NULL;
    linkkit_solo_ctx->upstream_mutex = NULL;
    linkkit_solo_ctx->user_callback = NULL;
    linkkit_solo_ctx->user_context = NULL;
    linkkit_solo_ctx->cota_callback = NULL;
    linkkit_solo_ctx->fota_callback = NULL;
    INIT_LIST_HEAD(&linkkit_solo_ctx->callback_list);

    return SUCCESS_RETURN;
}

void *linkkit_set_tsl(const char *tsl, int tsl_len)
{
    int res = 0;
    void *thing_id = NULL;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (tsl == NULL || tsl_len <= 0) {
        impl_solo_err("Invalid Parameter");
        return NULL;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return NULL;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_set_tsl(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_TSL_SOURCE_LOCAL, tsl, tsl_len);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return NULL;
    }

    res = iotx_dm_deprecated_legacy_get_thingid_by_devid(IOTX_DM_LOCAL_NODE_DEVID, &thing_id);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return NULL;
    }

    _linkkit_solo_mutex_unlock();
    return thing_id;
}

int being_deprecated linkkit_set_value(linkkit_method_set_t method_set, const void *thing_id, const char *identifier,
                                       const void *value,
                                       const char *value_str)
{
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || identifier == NULL || (value == NULL && value_str == NULL)) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    switch (method_set) {
        case linkkit_method_set_property_value: {
            res = iotx_dm_deprecated_legacy_set_property_value(devid, (char *)identifier, strlen(identifier), (void *)value,
                    (char *)value_str);
        }
        break;
        case linkkit_method_set_event_output_value: {
            res = iotx_dm_deprecated_legacy_set_event_output_value(devid, (char *)identifier, strlen(identifier), (void *)value,
                    (char *)value_str);
        }
        break;
        case linkkit_method_set_service_output_value: {
            res = iotx_dm_deprecated_legacy_set_service_output_value(devid, (char *)identifier, strlen(identifier), (void *)value,
                    (char *)value_str);
        }
        break;
        default: {
            impl_solo_err("Invalid Parameter");
            res = FAIL_RETURN;
        }
        break;
    }

    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_get_value(linkkit_method_get_t method_get, const void *thing_id, const char *identifier,
                                       void *value,
                                       char **value_str)
{
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || identifier == NULL || (value == NULL && value_str == NULL)) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    switch (method_get) {
        case linkkit_method_get_property_value: {
            res = iotx_dm_deprecated_legacy_get_property_value(devid, (char *)identifier, strlen(identifier), value, value_str);
        }
        break;
        case linkkit_method_get_event_output_value: {
            res = iotx_dm_deprecated_legacy_get_event_output_value(devid, (char *)identifier, strlen(identifier), value, value_str);
        }
        break;
        case linkkit_method_get_service_input_value: {
            res = iotx_dm_deprecated_legacy_get_service_input_value(devid, (char *)identifier, strlen(identifier), value,
                    value_str);
        }
        break;
        case linkkit_method_get_service_output_value: {
            res = iotx_dm_deprecated_legacy_get_service_output_value(devid, (char *)identifier, strlen(identifier), value,
                    value_str);
        }
        break;
        default: {
            impl_solo_err("Invalid Parameter");
            res = FAIL_RETURN;
        }
        break;
    }

    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_answer_service(const void *thing_id, const char *service_identifier, int response_id,
        int code)
{
    int res = 0, devid = 0;

    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || service_identifier == NULL) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_send_service_response(devid, response_id, (iotx_dm_error_code_t)code,
            (char *)service_identifier, strlen(service_identifier));
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_invoke_raw_service(const void *thing_id, int is_up_raw, void *raw_data,
        int raw_data_length)
{
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || raw_data == NULL || raw_data_length <= 0) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_post_rawdata(devid, raw_data, raw_data_length);
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_trigger_extended_info_operate(const void *thing_id, const char *params,
        linkkit_extended_info_operate_t linkkit_extended_info_operation)
{
    int res = 0, devid = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || params == NULL) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    switch (linkkit_extended_info_operation) {
        case linkkit_extended_info_operate_update: {
            res = iotx_dm_deviceinfo_update(devid, (char *)params, strlen(params));
        }
        break;
        case linkkit_extended_info_operate_delete: {
            res = iotx_dm_deviceinfo_delete(devid, (char *)params, strlen(params));
        }
        break;
        default: {
            impl_solo_err("Invalid Parameter");
            res = FAIL_RETURN;
        }
        break;
    }

    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_unlock();

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_trigger_event(const void *thing_id, const char *event_identifier, handle_post_cb_fp_t cb)
{
    int res = 0, devid = 0, msgid = 0, post_event_reply = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL || event_identifier == NULL) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_post_event(devid, (char *)event_identifier, strlen((char *)event_identifier));
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    res = iotx_dm_get_opt(linkkit_opt_event_post_reply, &post_event_reply);
    if (cb != NULL && post_event_reply) {
        /* Insert Message ID Into Linked List */
        _linkkit_solo_upstream_mutex_lock();
        res = _linkkit_solo_upstream_callback_list_insert(msgid, cb);
        _linkkit_solo_upstream_mutex_unlock();
        if (res != SUCCESS_RETURN) {
            _linkkit_solo_mutex_unlock();
            return FAIL_RETURN;
        }
    }

    _linkkit_solo_mutex_unlock();

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_post_property(const void *thing_id, const char *property_identifier,
        handle_post_cb_fp_t cb)
{
    int res = 0, devid = 0, msgid = 0, property_identifier_len = 0, post_property_reply = 0;
    void *property_handle = NULL;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (thing_id == NULL) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_thingid((void *)thing_id, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_post_property_start(devid, &property_handle);
    if (res != SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    property_identifier_len = (property_identifier) ? (strlen((char *)property_identifier)) : (0);
    res = iotx_dm_deprecated_post_property_add(property_handle, (char *)property_identifier, property_identifier_len);
    if (res != SUCCESS_RETURN) {
        iotx_dm_deprecated_post_property_end(&property_handle);
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_deprecated_post_property_end(&property_handle);
    if (res < SUCCESS_RETURN) {
        _linkkit_solo_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    res = iotx_dm_get_opt(linkkit_opt_property_post_reply, &post_property_reply);
    if (cb != NULL && post_property_reply) {
        /* Insert Message ID Into Linked List */
        _linkkit_solo_upstream_mutex_lock();
        res = _linkkit_solo_upstream_callback_list_insert(msgid, cb);
        _linkkit_solo_upstream_mutex_unlock();
        if (res != SUCCESS_RETURN) {
            _linkkit_solo_mutex_unlock();
            return FAIL_RETURN;
        }
    }

    _linkkit_solo_mutex_unlock();

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_yield(int timeout_ms)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (timeout_ms <= 0) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    return iotx_dm_yield(timeout_ms);
}

int being_deprecated linkkit_cota_init(handle_service_cota_callback_fp_t callback_fp)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    linkkit_solo_ctx->cota_callback = callback_fp;

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_invoke_cota_service(void *data_buf, int data_buf_length)
{
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (data_buf == NULL || data_buf_length <= 0) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_cota_perform_sync(data_buf, data_buf_length);
    _linkkit_solo_mutex_unlock();

    return res;
}

int being_deprecated linkkit_invoke_cota_get_config(const char *config_scope, const char *get_type,
        const char *attribute_Keys,
        void *option)
{
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (config_scope == NULL || get_type == NULL || attribute_Keys == NULL) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_cota_get_config(config_scope, get_type, attribute_Keys);
    _linkkit_solo_mutex_unlock();

    return res;
}

int being_deprecated linkkit_fota_init(handle_service_fota_callback_fp_t callback_fp)
{
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    linkkit_solo_ctx->fota_callback = callback_fp;

    return SUCCESS_RETURN;
}

int being_deprecated linkkit_invoke_fota_service(void *data_buf, int data_buf_length)
{
    int res = 0;
    linkkit_solo_legacy_ctx_t *linkkit_solo_ctx = _linkkit_solo_legacy_get_ctx();

    if (data_buf == NULL || data_buf_length <= 0) {
        impl_solo_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (linkkit_solo_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_solo_mutex_lock();
    res = iotx_dm_fota_perform_sync(data_buf, data_buf_length);
    _linkkit_solo_mutex_unlock();

    return res;
}
#endif
