/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "iotx_dm_internal.h"

#if defined(DEPRECATED_LINKKIT) && defined(DEVICE_MODEL_GATEWAY)

#include "impl_gateway.h"

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define IMPL_GATEWAY_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "impl.gateway")
    #define IMPL_GATEWAY_FREE(ptr)               IMPL_GATEWAY_FREE(ptr)
#else
    #define IMPL_GATEWAY_MALLOC(size)            HAL_Malloc(size)
    #define IMPL_GATEWAY_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define impl_gateway_err(...)       log_err("impl.gateway", __VA_ARGS__)
    #define impl_gateway_info(...)      log_info("impl.gateway", __VA_ARGS__)
    #define impl_gateway_debug(...)     log_debug("impl.gateway", __VA_ARGS__)
#else
    #define impl_gateway_err(...)
    #define impl_gateway_info(...)
    #define impl_gateway_debug(...)
#endif

static linkkit_gateway_legacy_ctx_t g_linkkit_gateway_legacy_ctx = {0};

static linkkit_gateway_legacy_ctx_t *_linkkit_gateway_legacy_get_ctx(void)
{
    return &g_linkkit_gateway_legacy_ctx;
}

static void _linkkit_gateway_mutex_lock(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    if (linkkit_gateway_ctx->mutex) {
        HAL_MutexLock(linkkit_gateway_ctx->mutex);
    }
}

static void _linkkit_gateway_mutex_unlock(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    if (linkkit_gateway_ctx->mutex) {
        HAL_MutexUnlock(linkkit_gateway_ctx->mutex);
    }
}

static void _linkkit_gateway_upstream_mutex_lock(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    if (linkkit_gateway_ctx->upstream_mutex) {
        HAL_MutexLock(linkkit_gateway_ctx->upstream_mutex);
    }
}

static void _linkkit_gateway_upstream_mutex_unlock(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    if (linkkit_gateway_ctx->upstream_mutex) {
        HAL_MutexUnlock(linkkit_gateway_ctx->upstream_mutex);
    }
}

static int _linkkit_gateway_callback_list_insert(int devid, linkkit_cbs_t *callback, void *context)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_dev_callback_node_t *search_node = NULL, *node = NULL;

    list_for_each_entry(search_node, &linkkit_gateway_ctx->dev_callback_list, linked_list,
                        linkkit_gateway_dev_callback_node_t) {
        if (search_node->devid == devid) {
            impl_gateway_info("Device Already Exist: %d", devid);
            return SUCCESS_RETURN;
        }
    }

    node = IMPL_GATEWAY_MALLOC(sizeof(linkkit_gateway_dev_callback_node_t));
    if (node == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(node, 0, sizeof(linkkit_gateway_dev_callback_node_t));
    node->devid = devid;
    node->callback = callback;
    node->callback_ctx = context;
    INIT_LIST_HEAD(&node->linked_list);

    list_add(&node->linked_list, &linkkit_gateway_ctx->dev_callback_list);

    return SUCCESS_RETURN;
}

static int _linkkit_gateway_callback_list_remove(int devid)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_dev_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_gateway_ctx->dev_callback_list, linked_list,
                        linkkit_gateway_dev_callback_node_t) {
        if (search_node->devid == devid) {
            impl_gateway_info("Device Found: %d, Delete It", devid);
            list_del(&search_node->linked_list);
            IMPL_GATEWAY_FREE(search_node);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _linkkit_gateway_callback_list_search(int devid, linkkit_gateway_dev_callback_node_t **node)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_dev_callback_node_t *search_node = NULL;

    if (devid < 0 || node == NULL || *node != NULL) {
        return FAIL_RETURN;
    }

    list_for_each_entry(search_node, &linkkit_gateway_ctx->dev_callback_list, linked_list,
                        linkkit_gateway_dev_callback_node_t) {
        if (search_node->devid == devid) {
            impl_gateway_info("Device Found: %d", devid);
            *node = search_node;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static void _linkkit_gateway_callback_list_destroy(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_dev_callback_node_t *search_node = NULL;
    linkkit_gateway_dev_callback_node_t *next_node = NULL;

    list_for_each_entry_safe(search_node, next_node, &linkkit_gateway_ctx->dev_callback_list, linked_list,
                             linkkit_gateway_dev_callback_node_t) {
        list_del(&search_node->linked_list);
        IMPL_GATEWAY_FREE(search_node);
    }
}

static int _linkkit_gateway_upstream_sync_callback_list_insert(int msgid, void *semaphore,
        linkkit_gateway_upstream_sync_callback_node_t **node)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_sync_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_gateway_ctx->upstream_sync_callback_list, linked_list,
                        linkkit_gateway_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            impl_gateway_info("Message Already Exist: %d", msgid);
            return FAIL_RETURN;
        }
    }

    search_node = IMPL_GATEWAY_MALLOC(sizeof(linkkit_gateway_upstream_sync_callback_node_t));
    if (search_node == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(search_node, 0, sizeof(linkkit_gateway_upstream_sync_callback_node_t));
    search_node->msgid = msgid;
    search_node->semaphore = semaphore;
    INIT_LIST_HEAD(&search_node->linked_list);

    list_add(&search_node->linked_list, &linkkit_gateway_ctx->upstream_sync_callback_list);
    impl_gateway_info("New Message, msgid: %d", msgid);

    *node = search_node;
    return SUCCESS_RETURN;
}

static int _linkkit_gateway_upstream_sync_callback_list_remove(int msgid)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_sync_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_gateway_ctx->upstream_sync_callback_list, linked_list,
                        linkkit_gateway_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            impl_gateway_info("Message Found: %d, Delete It", msgid);
            HAL_SemaphoreDestroy(search_node->semaphore);
            list_del(&search_node->linked_list);
            IMPL_GATEWAY_FREE(search_node);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _linkkit_gateway_upstream_sync_callback_list_search(int msgid,
        linkkit_gateway_upstream_sync_callback_node_t **node)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_sync_callback_node_t *search_node = NULL;

    if (node == NULL || *node != NULL) {
        return FAIL_RETURN;
    }

    list_for_each_entry(search_node, &linkkit_gateway_ctx->upstream_sync_callback_list, linked_list,
                        linkkit_gateway_upstream_sync_callback_node_t) {
        if (search_node->msgid == msgid) {
            impl_gateway_info("Sync Message Found: %d", msgid);
            *node = search_node;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static void _linkkit_gateway_upstream_sync_callback_list_destroy(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_sync_callback_node_t *search_node = NULL, *next_node = NULL;

    list_for_each_entry_safe(search_node, next_node, &linkkit_gateway_ctx->upstream_sync_callback_list, linked_list,
                             linkkit_gateway_upstream_sync_callback_node_t) {
        list_del(&search_node->linked_list);
        HAL_SemaphoreDestroy(search_node->semaphore);
        IMPL_GATEWAY_FREE(search_node);
    }
}

static int _linkkit_gateway_upstream_async_callback_list_insert(int msgid, int timeout_ms,
        linkkit_gateway_upstream_async_callback callback, void *context)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_async_callback_node_t *search_node = NULL, *node = NULL;

    list_for_each_entry(search_node, &linkkit_gateway_ctx->upstream_async_callback_list, linked_list,
                        linkkit_gateway_upstream_async_callback_node_t) {
        if (search_node->msgid == msgid) {
            impl_gateway_info("Async Message Already Exist: %d", msgid);
            return FAIL_RETURN;
        }
    }

    node = IMPL_GATEWAY_MALLOC(sizeof(linkkit_gateway_upstream_async_callback_node_t));
    if (node == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(node, 0, sizeof(linkkit_gateway_upstream_async_callback_node_t));
    node->msgid = msgid;
    node->timeout_ms = timeout_ms;
    node->timestamp_ms = HAL_UptimeMs();
    node->callback = callback;
    node->callback_ctx = context;

    INIT_LIST_HEAD(&node->linked_list);

    list_add(&node->linked_list, &linkkit_gateway_ctx->upstream_async_callback_list);

    return SUCCESS_RETURN;
}

static int _linkkit_gateway_upstream_async_callback_list_remove(int msgid)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_async_callback_node_t *search_node = NULL;

    list_for_each_entry(search_node, &linkkit_gateway_ctx->upstream_async_callback_list, linked_list,
                        linkkit_gateway_upstream_async_callback_node_t) {
        if (search_node->msgid == msgid) {
            impl_gateway_info("Async Message Found: %d, Delete It", msgid);
            list_del(&search_node->linked_list);
            IMPL_GATEWAY_FREE(search_node);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _linkkit_gateway_upstream_async_callback_list_search(int msgid,
        linkkit_gateway_upstream_async_callback_node_t **node)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_async_callback_node_t *search_node = NULL;

    if (node == NULL || *node != NULL) {
        return FAIL_RETURN;
    }

    list_for_each_entry(search_node, &linkkit_gateway_ctx->upstream_async_callback_list, linked_list,
                        linkkit_gateway_upstream_async_callback_node_t) {
        if (search_node->msgid == msgid) {
            impl_gateway_info("Async Message Found: %d", msgid);
            *node = search_node;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static void _linkkit_gateway_upstream_async_callback_list_destroy(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_async_callback_node_t *search_node = NULL, *next_node = NULL;

    list_for_each_entry_safe(search_node, next_node, &linkkit_gateway_ctx->upstream_async_callback_list, linked_list,
                             linkkit_gateway_upstream_async_callback_node_t) {
        list_del(&search_node->linked_list);
        IMPL_GATEWAY_FREE(search_node);
    }
}

static void _linkkit_gateway_upstream_callback_remove(int msgid, int code)
{
    int res = 0;
    linkkit_gateway_upstream_sync_callback_node_t *sync_node = NULL;
    res = _linkkit_gateway_upstream_sync_callback_list_search(msgid, &sync_node);
    if (res != SUCCESS_RETURN) {
        linkkit_gateway_upstream_async_callback_node_t *node = NULL;
        res = _linkkit_gateway_upstream_async_callback_list_search(msgid, &node);
        if (res == SUCCESS_RETURN) {
            uint64_t current_time = HAL_UptimeMs();
            if (current_time - node->timestamp_ms > node->timeout_ms) {
                if (node->callback) {
                    node->callback(FAIL_RETURN, node->callback_ctx);
                }
            } else {
                if (node->callback) {
                    int return_value = (code == IOTX_DM_ERR_CODE_SUCCESS) ? (SUCCESS_RETURN) : (FAIL_RETURN);
                    impl_gateway_info("Async Message %d Result: %d", msgid, return_value);
                    node->callback(return_value, node->callback_ctx);
                }
            }
            _linkkit_gateway_upstream_async_callback_list_remove(msgid);
        }
    } else {
        sync_node->code = (code == IOTX_DM_ERR_CODE_SUCCESS) ? (SUCCESS_RETURN) : (FAIL_RETURN);
        impl_gateway_info("Sync Message %d Result: %d", msgid, sync_node->code);
        HAL_SemaphorePost(sync_node->semaphore);
    }
}

linkkit_params_t *linkkit_gateway_get_default_params(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    /* Legacy Parameter */
    linkkit_gateway_ctx->init_params.maxMsgSize = 20 * 1024;
    linkkit_gateway_ctx->init_params.maxMsgQueueSize = 16;
    linkkit_gateway_ctx->init_params.threadPoolSize = 4;
    linkkit_gateway_ctx->init_params.threadStackSize = 8 * 1024;

    return &linkkit_gateway_ctx->init_params;
}

int linkkit_gateway_setopt(linkkit_params_t *params, int option, void *value, int value_len)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (params == NULL || value == NULL) {
        return FAIL_RETURN;
    }

    switch (option) {
        case LINKKIT_OPT_MAX_MSG_SIZE: {
            if (value_len != sizeof(int)) {
                return FAIL_RETURN;
            }
            if (*((int *)value) < 256) {
                impl_gateway_err("maxMsgSize should not less than 256 bytes\n");
                return FAIL_RETURN;
            }
            linkkit_gateway_ctx->init_params.maxMsgSize = *((int *)value);
        }
        break;
        case LINKKIT_OPT_MAX_MSG_QUEUE_SIZE: {
            if (value_len != sizeof(int)) {
                return FAIL_RETURN;
            }
            if (*((int *)value) < 1) {
                impl_gateway_err("maxMsgQueueSize should not less than 1\n");
                return FAIL_RETURN;
            }
            linkkit_gateway_ctx->init_params.maxMsgQueueSize = *((int *)value);
        }
        break;
        case LINKKIT_OPT_THREAD_POOL_SIZE: {
            if (value_len != sizeof(int)) {
                return FAIL_RETURN;
            }
            if (*((int *)value) < 1) {
                impl_gateway_err("threadPoolSize should not less than 1\n");
                return FAIL_RETURN;
            }
            linkkit_gateway_ctx->init_params.threadPoolSize = *((int *)value);
        }
        break;
        case LINKKIT_OPT_THREAD_STACK_SIZE: {
            if (value_len != sizeof(int)) {
                return FAIL_RETURN;
            }
            if (*((int *)value) < 1024) {
                impl_gateway_err("threadStackSize should not less than 1024\n");
                return FAIL_RETURN;
            }
            linkkit_gateway_ctx->init_params.threadStackSize = *((int *)value);
        }
        break;
        case LINKKIT_OPT_PROPERTY_POST_REPLY:
            iotx_dm_set_opt(0, value);
            break;
        case LINKKIT_OPT_EVENT_POST_REPLY:
            iotx_dm_set_opt(1, value);
            break;
        case LINKKIT_OPT_PROPERTY_SET_REPLY:
            iotx_dm_set_opt(2, value);
            break;
        default:
            impl_gateway_err("unknow option: %d\n", option);
            return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int linkkit_gateway_set_event_callback(linkkit_params_t *params, int (*event_cb)(linkkit_event_t *ev, void *ctx),
                                       void *ctx)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (params == NULL || event_cb == NULL) {
        return FAIL_RETURN;
    }

    linkkit_gateway_ctx->init_params.event_cb = event_cb;
    linkkit_gateway_ctx->init_params.ctx = ctx;

    return SUCCESS_RETURN;
}

int linkkit_gateway_init(linkkit_params_t *initParams)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (initParams == NULL) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_inited == 1) {
        return FAIL_RETURN;
    }

    if (initParams->maxMsgSize <  256 ||
        initParams->maxMsgQueueSize < 1 ||
        initParams->threadPoolSize < 1 ||
        initParams->threadStackSize < 1024) {
        return FAIL_RETURN;
    }
    linkkit_gateway_ctx->is_inited = 1;

    return SUCCESS_RETURN;
}

int linkkit_gateway_exit(void)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (linkkit_gateway_ctx->is_inited == 0) {
        return FAIL_RETURN;
    }
    linkkit_gateway_ctx->is_inited = 0;

    return SUCCESS_RETURN;
}

static void _linkkit_gateway_event_callback(iotx_dm_event_types_t type, char *payload)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    impl_gateway_info("Receive Message Type: %d", type);
    if (payload) {
        impl_gateway_info("Receive Message: %s", payload);
    }

    switch (type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            if (linkkit_gateway_ctx->init_params.event_cb) {
                linkkit_event_t event;

                memset(&event, 0, sizeof(linkkit_event_t));
                event.event_type = LINKKIT_EVENT_CLOUD_CONNECTED;
                linkkit_gateway_ctx->init_params.event_cb(&event, linkkit_gateway_ctx->init_params.ctx);
            }
        }
        break;
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            if (linkkit_gateway_ctx->init_params.event_cb) {
                linkkit_event_t event;

                memset(&event, 0, sizeof(linkkit_event_t));
                event.event_type = LINKKIT_EVENT_CLOUD_DISCONNECTED;
                linkkit_gateway_ctx->init_params.event_cb(&event, linkkit_gateway_ctx->init_params.ctx);
            }
        }
        break;
        case IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse JSON */
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
                return;
            }

            /* Parse Message ID */
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {
                return;
            }
            impl_gateway_info("Current Msg ID: %d", lite_item_id.value_int);

            /* Parse Message Code */
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {
                return;
            }
            impl_gateway_info("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {
                return;
            }
            impl_gateway_info("Current devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY: {

        }
        break;
        case IOTX_DM_EVENT_TOPO_ADD_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse JSON */
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
                return;
            }

            /* Parse Message ID */
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_id)) {
                return;
            }
            impl_gateway_info("Current Msg ID: %d", lite_item_id.value_int);

            /* Parse Message Code */
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_code)) {
                return;
            }
            impl_gateway_info("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN || !lite_cjson_is_number(&lite_item_devid)) {
                return;
            }
            impl_gateway_info("Current devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_TOPO_DELETE_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_COMBINE_LOGIN_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            int res = 0;
            linkkit_gateway_dev_callback_node_t *node = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_payload;
            char *params = NULL;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Payload */
            memset(&lite_item_payload, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD, strlen(LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD),
                                         &lite_item_payload);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Payload: %.*", lite_item_payload.value_length, lite_item_payload.value);

            params = IMPL_GATEWAY_MALLOC(lite_item_payload.value_length + 1);
            if (params == NULL) {
                return;
            }
            memset(params, 0, lite_item_payload.value_length + 1);
            memcpy(params, lite_item_payload.value, lite_item_payload.value_length);

            _linkkit_gateway_upstream_mutex_lock();
            res = _linkkit_gateway_callback_list_search(lite_item_devid.value_int, &node);
            _linkkit_gateway_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (node->callback->set_property) {
                    node->callback->set_property(params, node->callback_ctx);
                }
            }

            IMPL_GATEWAY_FREE(params);
        }
        break;
        case IOTX_DM_EVENT_GATEWAY_PERMIT: {
            int res = 0;
            char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
            lite_cjson_t lite, lite_item_pk, lite_item_timeout;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Product Key */
            memset(&lite_item_pk, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_PRODUCT_KEY,
                                         strlen(LINKKIT_GATEWAY_LEGACY_KEY_PRODUCT_KEY), &lite_item_pk);
            if (res != SUCCESS_RETURN || lite_item_pk.value_length >= IOTX_PRODUCT_KEY_LEN + 1) {
                return;
            }
            impl_gateway_debug("Current Product Key: %.*s", lite_item_pk.value_length, lite_item_pk.value);

            /* Parse Timeout */
            memset(&lite_item_timeout, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_TIME, strlen(LINKKIT_GATEWAY_LEGACY_KEY_TIME),
                                         &lite_item_timeout);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Timeout: %d", lite_item_timeout.value_int);

            memcpy(product_key, lite_item_pk.value, lite_item_pk.value_length);

            if (linkkit_gateway_ctx->init_params.event_cb) {
                linkkit_event_t event;

                memset(&event, 0, sizeof(linkkit_event_t));
                event.event_type = LINKKIT_EVENT_SUBDEV_PERMITED;
                event.event_data.subdev_permited.productKey = product_key;
                event.event_data.subdev_permited.timeoutSec = lite_item_timeout.value_int;
                linkkit_gateway_ctx->init_params.event_cb(&event, linkkit_gateway_ctx->init_params.ctx);
            }
        }
        break;
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            int res = 0;
            linkkit_gateway_dev_callback_node_t *node = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid, lite_item_paylaod;
            char *identifier = NULL, *input = NULL, *output = NULL;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Serviceid */
            memset(&lite_item_serviceid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_SERVICEID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_SERVICEID),
                                         &lite_item_serviceid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);

            /* Parse Payload */
            memset(&lite_item_paylaod, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD, strlen(LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD),
                                         &lite_item_paylaod);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Payload: %.*s", lite_item_paylaod.value_length, lite_item_paylaod.value);

            identifier = IMPL_GATEWAY_MALLOC(lite_item_serviceid.value_length + 1);
            if (identifier == NULL) {
                return;
            }
            memset(identifier, 0, lite_item_serviceid.value_length + 1);
            memcpy(identifier, lite_item_serviceid.value, lite_item_serviceid.value_length);

            input = IMPL_GATEWAY_MALLOC(lite_item_paylaod.value_length + 1);
            if (input == NULL) {
                IMPL_GATEWAY_FREE(identifier);
                return;
            }
            memset(input, 0, lite_item_paylaod.value_length + 1);
            memcpy(input, lite_item_paylaod.value, lite_item_paylaod.value_length);


            output = IMPL_GATEWAY_MALLOC(linkkit_gateway_ctx->init_params.maxMsgSize + 1);
            if (output == NULL) {
                IMPL_GATEWAY_FREE(identifier);
                IMPL_GATEWAY_FREE(input);
                return;
            }
            memset(output, 0, linkkit_gateway_ctx->init_params.maxMsgSize + 1);

            _linkkit_gateway_upstream_mutex_lock();
            res = _linkkit_gateway_callback_list_search(lite_item_devid.value_int, &node);
            _linkkit_gateway_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (node->callback->call_service) {
                    res = node->callback->call_service(identifier, input, output, linkkit_gateway_ctx->init_params.maxMsgSize,
                                                       node->callback_ctx);
                    if (res == SUCCESS_RETURN) {
                        iotx_dm_deprecated_legacy_send_service_response(lite_item_devid.value_int, lite_item_id.value_int, 200,
                                lite_item_serviceid.value,
                                lite_item_serviceid.value_length, output, strlen(output));
                    } else {
                        iotx_dm_deprecated_legacy_send_service_response(lite_item_devid.value_int, lite_item_id.value_int, 202,
                                lite_item_serviceid.value,
                                lite_item_serviceid.value_length, "{}", strlen("{}"));
                    }
                }
            }

            IMPL_GATEWAY_FREE(identifier);
            IMPL_GATEWAY_FREE(input);
            IMPL_GATEWAY_FREE(output);
        }
        break;
        case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
            int res = 0;
            linkkit_gateway_dev_callback_node_t *node = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_rawdata;
            char *output = NULL;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Raw Data */
            memset(&lite_item_rawdata, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD, strlen(LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD),
                                         &lite_item_rawdata);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Raw Data: %.*s", lite_item_rawdata.value_length, lite_item_rawdata.value);

            output = IMPL_GATEWAY_MALLOC(linkkit_gateway_ctx->init_params.maxMsgSize + 1);
            if (output == NULL) {
                return;
            }
            memset(output, 0, linkkit_gateway_ctx->init_params.maxMsgSize + 1);

            _linkkit_gateway_upstream_mutex_lock();
            res = _linkkit_gateway_callback_list_search(lite_item_devid.value_int, &node);
            _linkkit_gateway_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (node->callback->down_rawdata) {
                    res = node->callback->down_rawdata(lite_item_rawdata.value, lite_item_rawdata.value_length, output,
                                                       linkkit_gateway_ctx->init_params.maxMsgSize, node->callback_ctx);
                    if (res > 0) {
                        iotx_dm_post_rawdata(lite_item_devid.value_int, output, res);
                    }
                }
            }

            IMPL_GATEWAY_FREE(output);
        }
        break;
        case IOTX_DM_EVENT_MODEL_UP_RAW_REPLY: {
            int res = 0;
            linkkit_gateway_dev_callback_node_t *node = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Raw Data */
            memset(&lite_item_rawdata, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD, strlen(LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD),
                                         &lite_item_rawdata);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Raw Data: %.*s", lite_item_rawdata.value_length, lite_item_rawdata.value);

            _linkkit_gateway_upstream_mutex_lock();
            res = _linkkit_gateway_callback_list_search(lite_item_devid.value_int, &node);
            _linkkit_gateway_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (node->callback->post_rawdata_reply) {
                    node->callback->post_rawdata_reply(lite_item_rawdata.value, lite_item_rawdata.value_length, node->callback_ctx);
                }
            }
        }
        break;
        case IOTX_DM_EVENT_INITIALIZED: {
            int res = 0;
            linkkit_gateway_dev_callback_node_t *node = NULL;
            lite_cjson_t lite, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            res = _linkkit_gateway_callback_list_search(lite_item_devid.value_int, &node);
            _linkkit_gateway_upstream_mutex_unlock();
            if (res == SUCCESS_RETURN) {
                if (node->callback->register_complete) {
                    node->callback->register_complete(node->callback_ctx);
                }
            }
        }
        break;
        case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
            int res = 0;
            char *eventid = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid, lite_item_eventid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Property ID */
            memset(&lite_item_eventid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_EVENTID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_EVENTID),
                                         &lite_item_eventid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current EventID: %.*s", lite_item_eventid.value_length, lite_item_eventid.value);

            eventid = IMPL_GATEWAY_MALLOC(lite_item_eventid.value_length + 1);
            if (eventid == NULL) {
                return;
            }
            memset(eventid, 0, lite_item_eventid.value_length + 1);
            memcpy(eventid, lite_item_eventid.value, lite_item_eventid.value_length);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();

            IMPL_GATEWAY_FREE(eventid);
        }
        break;
        case IOTX_DM_EVENT_FOTA_NEW_FIRMWARE: {
            int res = 0;
            lite_cjson_t lite, lite_item_version;
            char *version = NULL;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite)) {
                return;
            }

            /* Parse Version */
            memset(&lite_item_version, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_VERSION, strlen(LINKKIT_GATEWAY_LEGACY_KEY_VERSION),
                                         &lite_item_version);
            if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item_version)) {
                return;
            }
            impl_gateway_debug("Current Firmware Version: %.*s", lite_item_version.value_length, lite_item_version.value);

            version = IMPL_GATEWAY_MALLOC(lite_item_version.value_length + 1);
            if (version == NULL) {
                return;
            }
            memset(version, 0, lite_item_version.value_length + 1);
            memcpy(version, lite_item_version.value, lite_item_version.value_length);

            if (linkkit_gateway_ctx->fota_callback) {
                linkkit_gateway_ctx->fota_callback(service_fota_callback_type_new_version_detected, version);
            }

            if (version) {
                IMPL_GATEWAY_FREE(version);
            }
        }
        break;
        case IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        case IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_ID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_ID),
                                         &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_CODE, strlen(LINKKIT_GATEWAY_LEGACY_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, LINKKIT_GATEWAY_LEGACY_KEY_DEVID, strlen(LINKKIT_GATEWAY_LEGACY_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            impl_gateway_debug("Current Devid: %d", lite_item_devid.value_int);

            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_callback_remove(lite_item_id.value_int, lite_item_code.value_int);
            _linkkit_gateway_upstream_mutex_unlock();
        }
        break;
        default: {
            impl_gateway_info("Not Found Type For Now, Smile");
        }
        break;
    }
}

static void *_linkkit_gateway_dispatch(void *params)
{
    while (1) {
        iotx_dm_dispatch();
        HAL_SleepMs(20);
    }
    return NULL;
}

int linkkit_gateway_start(linkkit_cbs_t *cbs, void *ctx)
{
    int res = 0, stack_used = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    iotx_dm_init_params_t dm_init_params;
    linkkit_gateway_dev_callback_node_t *node = NULL;

    if (cbs == NULL) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_inited == 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started) {
        impl_gateway_info("Linkkit Gateway Already Started");
        return SUCCESS_RETURN;
    }

    linkkit_gateway_ctx->is_inited = 1;
    linkkit_gateway_ctx->is_started = 1;

    /* Create Mutex */
    linkkit_gateway_ctx->mutex = HAL_MutexCreate();
    if (linkkit_gateway_ctx->mutex == NULL) {
        linkkit_gateway_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    linkkit_gateway_ctx->upstream_mutex = HAL_MutexCreate();
    if (linkkit_gateway_ctx->upstream_mutex == NULL) {
        HAL_MutexDestroy(linkkit_gateway_ctx->mutex);
        linkkit_gateway_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Initialize Device Manager */
    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    dm_init_params.secret_type = IOTX_DM_DEVICE_SECRET_DEVICE;
    dm_init_params.domain_type = IOTX_DM_CLOUD_DOMAIN_SHANGHAI;
    dm_init_params.event_callback = _linkkit_gateway_event_callback;

    res = iotx_dm_open();
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_gateway_ctx->mutex);
        HAL_MutexDestroy(linkkit_gateway_ctx->upstream_mutex);
        linkkit_gateway_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    res = iotx_dm_connect(&dm_init_params);
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_gateway_ctx->mutex);
        HAL_MutexDestroy(linkkit_gateway_ctx->upstream_mutex);
        linkkit_gateway_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    res = iotx_dm_subscribe(IOTX_DM_LOCAL_NODE_DEVID);
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_gateway_ctx->mutex);
        HAL_MutexDestroy(linkkit_gateway_ctx->upstream_mutex);
        iotx_dm_close();
        linkkit_gateway_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    res = HAL_ThreadCreate(&linkkit_gateway_ctx->dispatch_thread, _linkkit_gateway_dispatch, NULL, NULL, &stack_used);
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_gateway_ctx->mutex);
        HAL_MutexDestroy(linkkit_gateway_ctx->upstream_mutex);
        iotx_dm_close();
        linkkit_gateway_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Insert Gateway Callback And Callback Context Into Device Callback Linkked List */
    INIT_LIST_HEAD(&linkkit_gateway_ctx->dev_callback_list);

    res = _linkkit_gateway_callback_list_insert(IOTX_DM_LOCAL_NODE_DEVID, cbs, ctx);
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(linkkit_gateway_ctx->mutex);
        HAL_MutexDestroy(linkkit_gateway_ctx->upstream_mutex);
        iotx_dm_close();
        HAL_ThreadDelete(linkkit_gateway_ctx->dispatch_thread);
        linkkit_gateway_ctx->is_started = 0;
        return FAIL_RETURN;
    }

    /* Init Upstream Callback List */
    INIT_LIST_HEAD(&linkkit_gateway_ctx->upstream_sync_callback_list);
    INIT_LIST_HEAD(&linkkit_gateway_ctx->upstream_async_callback_list);

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_callback_list_search(IOTX_DM_LOCAL_NODE_DEVID, &node);
    _linkkit_gateway_upstream_mutex_unlock();
    if (res == SUCCESS_RETURN) {
        if (node->callback->register_complete) {
            node->callback->register_complete(node->callback_ctx);
        }
    }

    return SUCCESS_RETURN;
}

int linkkit_gateway_stop(int devid)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (linkkit_gateway_ctx->is_started  == 0) {
        return FAIL_RETURN;
    }
    if (devid != IOTX_DM_LOCAL_NODE_DEVID) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    _linkkit_gateway_upstream_mutex_lock();
    linkkit_gateway_ctx->is_started = 0;
    HAL_ThreadDelete(linkkit_gateway_ctx->dispatch_thread);
    iotx_dm_close();
    HAL_SleepMs(200);
    _linkkit_gateway_callback_list_destroy();
    _linkkit_gateway_upstream_sync_callback_list_destroy();
    _linkkit_gateway_upstream_async_callback_list_destroy();
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    HAL_MutexDestroy(linkkit_gateway_ctx->upstream_mutex);
    HAL_MutexDestroy(linkkit_gateway_ctx->mutex);

    linkkit_gateway_ctx->mutex = NULL;
    linkkit_gateway_ctx->upstream_mutex = NULL;
    memset(&linkkit_gateway_ctx->init_params, 0, sizeof(linkkit_params_t));
    linkkit_gateway_ctx->dispatch_thread = NULL;
    linkkit_gateway_ctx->fota_callback = NULL;
    INIT_LIST_HEAD(&linkkit_gateway_ctx->dev_callback_list);
    INIT_LIST_HEAD(&linkkit_gateway_ctx->upstream_sync_callback_list);
    INIT_LIST_HEAD(&linkkit_gateway_ctx->upstream_async_callback_list);

    return SUCCESS_RETURN;
}

int linkkit_gateway_subdev_register(char *productKey, char *deviceName, char *deviceSecret)
{
    int res = 0, msgid = 0, code = 0, devid = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    void *semaphore = NULL;
    linkkit_gateway_dev_callback_node_t *dev_callback_node = NULL;

    if (productKey == NULL || strlen(productKey) >= IOTX_PRODUCT_KEY_LEN + 1 ||
        deviceName == NULL || strlen(deviceName) >= IOTX_DEVICE_NAME_LEN + 1) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_pkdn(productKey, deviceName, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    /* Subdev Delete Topo */
    res = iotx_dm_subdev_topo_del(devid);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, LINKKIT_GATEWAY_LEGACY_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    /* Subdev Register */
    res = iotx_dm_deprecated_subdev_register(devid, deviceSecret);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    if (res > SUCCESS_RETURN) {
        semaphore = HAL_SemaphoreCreate();
        if (semaphore == NULL) {
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }

        msgid = res;

        _linkkit_gateway_upstream_mutex_lock();
        res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
        if (res != SUCCESS_RETURN) {
            HAL_SemaphoreDestroy(semaphore);
            _linkkit_gateway_upstream_mutex_unlock();
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }
        _linkkit_gateway_upstream_mutex_unlock();

        res = HAL_SemaphoreWait(semaphore, LINKKIT_GATEWAY_LEGACY_SYNC_DEFAULT_TIMEOUT_MS);
        if (res < SUCCESS_RETURN) {
            _linkkit_gateway_upstream_mutex_lock();
            _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
            _linkkit_gateway_upstream_mutex_unlock();
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }

        _linkkit_gateway_upstream_mutex_lock();
        code = node->code;
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        if (code != SUCCESS_RETURN) {
            _linkkit_gateway_upstream_mutex_unlock();
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }
        _linkkit_gateway_upstream_mutex_unlock();
    }

    /* Subdev Add Topo */
    res = iotx_dm_subdev_topo_add(devid);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    msgid = res;
    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, LINKKIT_GATEWAY_LEGACY_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_callback_list_search(devid, &dev_callback_node);
    _linkkit_gateway_upstream_mutex_unlock();
    if (res == SUCCESS_RETURN) {
        if (dev_callback_node->callback->register_complete) {
            dev_callback_node->callback->register_complete(dev_callback_node->callback_ctx);
        }
    }

    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_subdev_unregister(char *productKey, char *deviceName)
{
    int res = 0, msgid = 0, code = 0, devid = 0;
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    void *semaphore = NULL;

    if (productKey == NULL || strlen(productKey) >= IOTX_PRODUCT_KEY_LEN + 1 ||
        deviceName == NULL || strlen(deviceName) >= IOTX_DEVICE_NAME_LEN + 1) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_deprecated_legacy_get_devid_by_pkdn(productKey, deviceName, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    /* Subdev Delete Topo */
    res = iotx_dm_subdev_topo_del(devid);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, LINKKIT_GATEWAY_LEGACY_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_subdev_create(char *productKey, char *deviceName, linkkit_cbs_t *cbs, void *ctx)
{
    int res = 0, devid = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (productKey == NULL || strlen(productKey) >= IOTX_PRODUCT_KEY_LEN + 1 ||
        deviceName == NULL || strlen(deviceName) >= IOTX_DEVICE_NAME_LEN + 1 || cbs == NULL) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_subdev_create(productKey, deviceName, NULL, &devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_callback_list_insert(devid, cbs, ctx);
    if (res != SUCCESS_RETURN) {
        iotx_dm_subdev_destroy(devid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return devid;
}

int linkkit_gateway_subdev_destroy(int devid)
{
    int res = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    if (devid <= 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_callback_list_remove(devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = iotx_dm_subdev_destroy(devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_subdev_login(int devid)
{
    int res = 0, msgid = 0, code = 0;
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    void *semaphore = NULL;

    if (devid <= 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_subdev_login(devid);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, LINKKIT_GATEWAY_LEGACY_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = iotx_dm_subscribe(devid);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_subdev_logout(int devid)
{
    int res = 0, msgid = 0, code = 0;
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    void *semaphore = NULL;

    if (devid <= 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_subdev_logout(devid);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, LINKKIT_GATEWAY_LEGACY_SYNC_DEFAULT_TIMEOUT_MS);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_get_devinfo(int devid, linkkit_devinfo_t *devinfo)
{
    int res = 0, type = 0;
    iotx_dm_dev_status_t status;
    iotx_dm_dev_avail_t available;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (devid < 0 || devinfo == NULL) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    memset(devinfo, 0, sizeof(linkkit_devinfo_t));
    res = iotx_dm_deprecated_legacy_get_pkdn_ptr_by_devid(devid, &(devinfo->productKey), &(devinfo->deviceName));
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_get_device_type(devid, &type);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    if (type == IOTX_DM_DEVICE_GATEWAY) {
        devinfo->devtype = 0;
    } else if (type == IOTX_DM_DEVICE_SUBDEV) {
        devinfo->devtype = 1;
    } else {
        impl_gateway_info("wrong device type\n");
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    res = iotx_dm_get_device_status(devid, &status);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    if (status >= IOTX_DM_DEV_STATUS_LOGINED) {
        devinfo->login = 1;
    }
    if (status == IOTX_DM_DEV_STATUS_ONLINE) {
        devinfo->online = 1;
    }

    res = iotx_dm_get_device_avail_status(devid, &available);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    devinfo->state = available;
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_trigger_event_json_sync(int devid, char *identifier, char *event, int timeout_ms)
{
    int res = 0, msgid = 0, code = 0, event_reply_value = 0;
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    void *semaphore = NULL;

    if (devid < 0 || identifier == NULL || event == NULL || timeout_ms < 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_get_opt(1, (void *)&event_reply_value);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    if (timeout_ms == 0 || event_reply_value == 0) {
        res = iotx_dm_post_event(devid, identifier, strlen(identifier), event, strlen(event));
        if (res < SUCCESS_RETURN) {
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        } else {
            _linkkit_gateway_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    res = iotx_dm_post_event(devid, identifier, strlen(identifier), event, strlen(event));
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, timeout_ms);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_trigger_event_json(int devid, char *identifier, char *event, int timeout_ms,
                                       void (*func)(int retval, void *ctx), void *ctx)
{
    int res = 0, event_reply_value = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (devid < 0 || identifier == NULL || event == NULL || timeout_ms < 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_get_opt(1, (void *)&event_reply_value);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    impl_gateway_info("event_reply_value: %d", event_reply_value);

    if (timeout_ms == 0 || event_reply_value == 0) {
        res = iotx_dm_post_event(devid, identifier, strlen(identifier), event, strlen(event));
        if (res < SUCCESS_RETURN) {
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        } else {
            _linkkit_gateway_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    res = iotx_dm_post_event(devid, identifier, strlen(identifier), event, strlen(event));
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_async_callback_list_insert(res, timeout_ms, func, ctx);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_post_property_json_sync(int devid, char *property, int timeout_ms)
{
    int res = 0, msgid = 0, code = 0, property_reply_value = 0;
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    void *semaphore = NULL;

    if (devid < 0 || property == NULL || timeout_ms < 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_get_opt(0, (void *)&property_reply_value);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    if (timeout_ms == 0 || property_reply_value == 0) {
        res = iotx_dm_post_property(devid, property, strlen(property));
        if (res < SUCCESS_RETURN) {
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        } else {
            _linkkit_gateway_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    res = iotx_dm_post_property(devid, property, strlen(property));
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, timeout_ms);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_post_property_json(int devid, char *property, int timeout_ms, void (*func)(int retval, void *ctx),
                                       void *ctx)
{
    int res = 0, property_reply_value = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (devid < 0 || property == NULL || timeout_ms < 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_get_opt(0, (void *)&property_reply_value);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    if (timeout_ms == 0 || property_reply_value == 0) {
        res = iotx_dm_post_property(devid, property, strlen(property));
        if (res < SUCCESS_RETURN) {
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        } else {
            _linkkit_gateway_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    res = iotx_dm_post_property(devid, property, strlen(property));
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_async_callback_list_insert(res, timeout_ms, func, ctx);
    if (res != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_post_rawdata(int devid, void *data, int len)
{
    int res = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (devid < 0 || data == NULL || len <= 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_post_rawdata(devid, data, len);
    _linkkit_gateway_mutex_unlock();

    return res;
}

int linkkit_gateway_fota_init(handle_service_fota_callback_fp_t callback_fp)
{
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    linkkit_gateway_ctx->fota_callback = callback_fp;

    return SUCCESS_RETURN;
}

int linkkit_gateway_invoke_fota_service(void *data_buf, int data_buf_length)
{
    int res = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    res = iotx_dm_fota_perform_sync(data_buf, data_buf_length);
    _linkkit_gateway_mutex_unlock();

    return res;
}

int linkkit_gateway_post_extinfos(int devid, linkkit_extinfo_t *extinfos, int nb_extinfos, int timeout_ms)
{
    int res = 0, index = 0, msgid = 0, code = 0;
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    void *semaphore = NULL;
    char *payload = NULL;
    lite_cjson_item_t *lite_array = NULL, *lite_array_item = NULL;

    if (devid < 0 || extinfos == NULL || nb_extinfos <= 0 || timeout_ms < 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    lite_array = lite_cjson_create_array();
    if (lite_array == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    for (index = 0; index < nb_extinfos; index++) {
        if (extinfos[index].attrKey == NULL || extinfos[index].attrValue == NULL) {
            lite_cjson_delete(lite_array);
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }

        lite_array_item = lite_cjson_create_object();
        if (lite_array_item == NULL) {
            lite_cjson_delete(lite_array);
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }
        lite_cjson_add_string_to_object(lite_array_item, "attrKey", extinfos[index].attrKey);
        lite_cjson_add_string_to_object(lite_array_item, "attrValue", extinfos[index].attrValue);
        lite_cjson_add_item_to_array(lite_array, lite_array_item);
    }

    payload = lite_cjson_print_unformatted(lite_array);
    if (payload == NULL) {
        lite_cjson_delete(lite_array);
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    lite_cjson_delete(lite_array);

    if (timeout_ms == 0) {
        res = iotx_dm_deviceinfo_update(devid, payload, strlen(payload));
        IMPL_GATEWAY_FREE(payload);
        if (res < SUCCESS_RETURN) {
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        } else {
            _linkkit_gateway_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    res = iotx_dm_deviceinfo_update(devid, payload, strlen(payload));
    if (res < SUCCESS_RETURN) {
        IMPL_GATEWAY_FREE(payload);
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;
    IMPL_GATEWAY_FREE(payload);

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, timeout_ms);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_delete_extinfos(int devid, linkkit_extinfo_t *extinfos, int nb_extinfos, int timeout_ms)
{
    int res = 0, index = 0, msgid = 0, code = 0;
    linkkit_gateway_upstream_sync_callback_node_t *node = NULL;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();
    void *semaphore = NULL;
    char *payload = NULL;
    lite_cjson_item_t *lite_array = NULL, *lite_array_item = NULL;

    if (devid < 0 || extinfos == NULL || nb_extinfos <= 0 || timeout_ms < 0) {
        return FAIL_RETURN;
    }

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    lite_array = lite_cjson_create_array();
    if (lite_array == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    for (index = 0; index < nb_extinfos; index++) {
        if (extinfos[index].attrKey == NULL || extinfos[index].attrValue == NULL) {
            lite_cjson_delete(lite_array);
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }

        lite_array_item = lite_cjson_create_object();
        if (lite_array_item == NULL) {
            lite_cjson_delete(lite_array);
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        }
        lite_cjson_add_string_to_object(lite_array_item, "attrKey", extinfos[index].attrKey);
        lite_cjson_add_string_to_object(lite_array_item, "attrValue", extinfos[index].attrValue);
        lite_cjson_add_item_to_array(lite_array, lite_array_item);
    }

    payload = lite_cjson_print_unformatted(lite_array);
    if (payload == NULL) {
        lite_cjson_delete(lite_array);
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    lite_cjson_delete(lite_array);

    if (timeout_ms == 0) {
        res = iotx_dm_deviceinfo_delete(devid, payload, strlen(payload));
        IMPL_GATEWAY_FREE(payload);
        if (res < SUCCESS_RETURN) {
            _linkkit_gateway_mutex_unlock();
            return FAIL_RETURN;
        } else {
            _linkkit_gateway_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    res = iotx_dm_deviceinfo_delete(devid, payload, strlen(payload));
    if (res < SUCCESS_RETURN) {
        IMPL_GATEWAY_FREE(payload);
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    msgid = res;
    IMPL_GATEWAY_FREE(payload);

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    res = _linkkit_gateway_upstream_sync_callback_list_insert(msgid, semaphore, &node);
    if (res != SUCCESS_RETURN) {
        HAL_SemaphoreDestroy(semaphore);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();

    res = HAL_SemaphoreWait(semaphore, timeout_ms);
    if (res < SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_lock();
        _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }

    _linkkit_gateway_upstream_mutex_lock();
    code = node->code;
    _linkkit_gateway_upstream_sync_callback_list_remove(msgid);
    if (code != SUCCESS_RETURN) {
        _linkkit_gateway_upstream_mutex_unlock();
        _linkkit_gateway_mutex_unlock();
        return FAIL_RETURN;
    }
    _linkkit_gateway_upstream_mutex_unlock();
    _linkkit_gateway_mutex_unlock();

    return SUCCESS_RETURN;
}

int linkkit_gateway_get_num_devices(void)
{
    int dev_nums = 0;
    linkkit_gateway_legacy_ctx_t *linkkit_gateway_ctx = _linkkit_gateway_legacy_get_ctx();

    if (linkkit_gateway_ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _linkkit_gateway_mutex_lock();
    dev_nums = iotx_dm_subdev_number();
    _linkkit_gateway_mutex_unlock();

    return dev_nums;
}
#endif
