/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"

static dm_mgr_ctx g_dm_mgr = {0};

static dm_mgr_ctx *_dm_mgr_get_ctx(void)
{
    return &g_dm_mgr;
}

static void _dm_mgr_mutex_lock(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    if (ctx->mutex) {
        HAL_MutexLock(ctx->mutex);
    }
}

static void _dm_mgr_mutex_unlock(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    if (ctx->mutex) {
        HAL_MutexUnlock(ctx->mutex);
    }
}

static int _dm_mgr_next_devid(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();

    return ctx->global_devid++;
}

static int _dm_mgr_search_dev_by_devid(_IN_ int devid, _OU_ dm_mgr_dev_node_t **node)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (search_node->devid == devid) {
            /* dm_log_debug("Device Found, devid: %d", devid); */
            if (node) {
                *node = search_node;
            }
            return SUCCESS_RETURN;
        }
    }

    dm_log_debug("Device Not Found, devid: %d", devid);
    return FAIL_RETURN;
}

static int _dm_mgr_search_dev_by_pkdn(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                      _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], _OU_ dm_mgr_dev_node_t **node)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if ((strlen(search_node->product_key) == strlen(product_key)) &&
            (memcmp(search_node->product_key, product_key, strlen(product_key)) == 0) &&
            (strlen(search_node->device_name) == strlen(device_name)) &&
            (memcmp(search_node->device_name, device_name, strlen(device_name)) == 0)) {
            /* dm_log_debug("Device Found, Product Key: %s, Device Name: %s", product_key, device_name); */
            if (node) {
                *node = search_node;
            }
            return SUCCESS_RETURN;
        }
    }

    dm_log_debug("Device Not Found, Product Key: %s, Device Name: %s", product_key, device_name);
    return FAIL_RETURN;
}

static int _dm_mgr_insert_dev(_IN_ int devid, _IN_ int dev_type, char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                              char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || product_key == NULL || strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1 ||
        device_name == NULL || strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, NULL);
    if (res == SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node = DM_malloc(sizeof(dm_mgr_dev_node_t));
    if (node == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(node, 0, sizeof(dm_mgr_dev_node_t));

    node->devid = devid;
    node->dev_type = dev_type;
    memcpy(node->product_key, product_key, strlen(product_key));
    memcpy(node->device_name, device_name, strlen(device_name));
    INIT_LIST_HEAD(&node->linked_list);

    list_add_tail(&node->linked_list, &ctx->dev_list);

    return SUCCESS_RETURN;
}

static void _dm_mgr_destroy_devlist(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *del_node = NULL;
    dm_mgr_dev_node_t *next_node = NULL;

    list_for_each_entry_safe(del_node, next_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        list_del(&del_node->linked_list);
#ifdef DEPRECATED_LINKKIT
        dm_shw_destroy(&del_node->dev_shadow);
#endif
        DM_free(del_node);
    }
}

#ifdef DEPRECATED_LINKKIT
static int _dm_mgr_legacy_thing_created(int devid)
{
    int res = 0, message_len = 0;
    const char *thing_created_fmt = "{\"devid\":%d}";
    char *message = NULL;

    message_len = strlen(thing_created_fmt) + DM_UTILS_UINT32_STRLEN + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, thing_created_fmt, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_LEGACY_THING_CREATED, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#endif

int dm_mgr_init(void)
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};

    memset(ctx, 0, sizeof(dm_mgr_ctx));

    /* Create Mutex */
    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        goto ERROR;
    }

    /* Init Device Id*/
    ctx->global_devid = IOTX_DM_LOCAL_NODE_DEVID + 1;

    /* Init Device List */
    INIT_LIST_HEAD(&ctx->dev_list);

    /* Local Node */
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    res = _dm_mgr_insert_dev(IOTX_DM_LOCAL_NODE_DEVID, IOTX_DM_DEVICE_TYPE, product_key, device_name);
    if (res != SUCCESS_RETURN) {
        goto ERROR;
    }

#ifdef DEPRECATED_LINKKIT
    _dm_mgr_legacy_thing_created(IOTX_DM_LOCAL_NODE_DEVID);
#endif

    return SUCCESS_RETURN;

ERROR:
    if (ctx->mutex) {
        HAL_MutexDestroy(ctx->mutex);
    }
    memset(ctx, 0, sizeof(dm_mgr_ctx));
    return FAIL_RETURN;
}

int dm_mgr_deinit(void)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();

    _dm_mgr_mutex_lock();
    _dm_mgr_destroy_devlist();
    _dm_mgr_mutex_unlock();

    if (ctx->mutex) {
        HAL_MutexDestroy(ctx->mutex);
    }

    return SUCCESS_RETURN;
}

int dm_mgr_device_query(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], _OU_ int *devid)
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *node = NULL;

    /* duplicated parameters check is removed */

    res = _dm_mgr_search_dev_by_pkdn(product_key, device_name, &node);
    if (res == SUCCESS_RETURN) {
        if (devid) {
            *devid = node->devid;
        }
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}

int dm_mgr_device_create(_IN_ int dev_type, _IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                         _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1], _OU_ int *devid)
{
    int res = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL ||
        strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1 ||
        strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1) {
        return DM_INVALID_PARAMETER;
    }

    if (device_secret != NULL && strlen(device_secret) >= IOTX_DEVICE_SECRET_LEN + 1) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_pkdn(product_key, device_name, &node);
    if (res == SUCCESS_RETURN) {
        if (devid) {
            *devid = node->devid;
        }
        return FAIL_RETURN;
    }

    node = DM_malloc(sizeof(dm_mgr_dev_node_t));
    if (node == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(node, 0, sizeof(dm_mgr_dev_node_t));

    node->devid = _dm_mgr_next_devid();
    node->dev_type = dev_type;
#if defined(DEPRECATED_LINKKIT)
    node->dev_shadow = NULL;
    node->tsl_source = IOTX_DM_TSL_SOURCE_CLOUD;
#endif
    memcpy(node->product_key, product_key, strlen(product_key));
    memcpy(node->device_name, device_name, strlen(device_name));
    if (device_secret != NULL) {
        memcpy(node->device_secret, device_secret, strlen(device_secret));
    }
    node->dev_status = IOTX_DM_DEV_STATUS_AUTHORIZED;
    INIT_LIST_HEAD(&node->linked_list);

    list_add_tail(&node->linked_list, &ctx->dev_list);

    if (devid) {
        *devid = node->devid;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_device_destroy(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (node->devid == IOTX_DM_LOCAL_NODE_DEVID) {
        return FAIL_RETURN;
    }

    list_del(&node->linked_list);

#if defined(DEPRECATED_LINKKIT)
    if (node->dev_shadow) {
        dm_shw_destroy(&node->dev_shadow);
    }
#endif

#ifdef DEVICE_MODEL_GATEWAY
    dm_client_subdev_unsubscribe(node->product_key,node->device_name);
#endif

    DM_free(node);

    return SUCCESS_RETURN;
}

int dm_mgr_device_number(void)
{
    int index = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        index++;
    }

    return index;
}

int dm_mgr_get_devid_by_index(_IN_ int index, _OU_ int *devid)
{
    int search_index = 0;
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    if (index < 0 || devid == NULL) {
        return DM_INVALID_PARAMETER;
    }

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (search_index == index) {
            *devid = search_node->devid;
            return SUCCESS_RETURN;
        }
        search_index++;
    }

    return FAIL_RETURN;
}

int dm_mgr_get_next_devid(_IN_ int devid, _OU_ int *devid_next)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;
    dm_mgr_dev_node_t *next_node = NULL;

    if (devid < 0 || devid_next == NULL) {
        return DM_INVALID_PARAMETER;
    }

    list_for_each_entry(next_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (search_node && search_node->devid == devid) {
            *devid_next = next_node->devid;
            return SUCCESS_RETURN;
        }

        if (next_node->devid == devid) {
            search_node = next_node;
        }
    }

    return FAIL_RETURN;
}

int dm_mgr_search_device_by_devid(_IN_ int devid, _OU_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                                  _OU_ char device_name[IOTX_DEVICE_NAME_LEN + 1], _OU_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL || device_secret == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memcpy(product_key, node->product_key, strlen(node->product_key));
    memcpy(device_name, node->device_name, strlen(node->device_name));
    memcpy(device_secret, node->device_secret, strlen(node->device_secret));

    return SUCCESS_RETURN;
}

int dm_mgr_search_device_by_pkdn(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                                 _OU_ int *devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_pkdn(product_key, device_name, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (devid) {
        *devid = node->devid;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_search_device_node_by_devid(_IN_ int devid, _OU_ void **node)
{
    int res = 0;
    dm_mgr_dev_node_t *search_node = NULL;

    res = _dm_mgr_search_dev_by_devid(devid, &search_node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (node) {
        *node = (void *)search_node;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_type(_IN_ int devid, _OU_ int *dev_type)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || dev_type == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *dev_type = node->dev_type;

    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_enable(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->status = IOTX_DM_DEV_AVAIL_ENABLE;

    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_disable(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->status = IOTX_DM_DEV_AVAIL_DISABLE;

    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_avail(_IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1], _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1],
                         _OU_ iotx_dm_dev_avail_t *status)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (product_key == NULL || device_name == NULL || status == NULL ||
        (strlen(product_key) >= IOTX_PRODUCT_KEY_LEN + 1) ||
        (strlen(device_name) >= IOTX_DEVICE_NAME_LEN + 1)) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_pkdn(product_key, device_name, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *status = node->status;

    return SUCCESS_RETURN;
}

int dm_mgr_set_dev_status(_IN_ int devid, _IN_ iotx_dm_dev_status_t status)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->dev_status = status;

    return SUCCESS_RETURN;
}

int dm_mgr_get_dev_status(_IN_ int devid, _OU_ iotx_dm_dev_status_t *status)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || status == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *status = node->dev_status;

    return SUCCESS_RETURN;
}

int dm_mgr_set_device_secret(_IN_ int devid, _IN_ char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || device_secret == NULL ||
        strlen(device_secret) >= IOTX_DEVICE_SECRET_LEN + 1) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(node->device_secret, 0, IOTX_DEVICE_SECRET_LEN + 1);
    memcpy(node->device_secret, device_secret, strlen(device_secret));

    return SUCCESS_RETURN;
}

int dm_mgr_dev_initialized(int devid)
{
    int res = 0, message_len = 0;
    char *message = NULL;
    const char *fmt = "{\"devid\":%d}";

    message_len = strlen(fmt) + DM_UTILS_UINT32_STRLEN + 1;
    message = DM_malloc(message_len);
    if (message == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(message, 0, message_len);
    HAL_Snprintf(message, message_len, fmt, devid);

    res = _dm_msg_send_to_user(IOTX_DM_EVENT_INITIALIZED, message);
    if (res != SUCCESS_RETURN) {
        DM_free(message);
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#ifdef DEVICE_MODEL_GATEWAY
int dm_mgr_upstream_thing_sub_register(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_SYS_PREFIX;
    request.service_name = DM_URI_THING_SUB_REGISTER;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_sub_register(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Callback */
    request.callback = dm_client_thing_sub_register_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_SUBDEV_REGISTER_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_sub_unregister(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_SYS_PREFIX;
    request.service_name = DM_URI_THING_SUB_UNREGISTER;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_sub_unregister(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Callback */
    request.callback = dm_client_thing_sub_unregister_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_SUBDEV_UNREGISTER_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_topo_add(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_SYS_PREFIX;
    request.service_name = DM_URI_THING_TOPO_ADD;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_topo_add(node->product_key, node->device_name, node->device_secret, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Callback */
    request.callback = dm_client_thing_topo_add_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_ADD_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_topo_delete(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_SYS_PREFIX;
    request.service_name = DM_URI_THING_TOPO_DELETE;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_topo_delete(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Callback */
    request.callback = dm_client_thing_topo_delete_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_DELETE_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_topo_get(void)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_SYS_PREFIX;
    request.service_name = DM_URI_THING_TOPO_GET;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    res = _dm_mgr_search_dev_by_pkdn(request.product_key, request.device_name, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Params And Method */
    res = dm_msg_thing_topo_get(&request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = node->devid;

    /* Callback */
    request.callback = dm_client_thing_topo_get_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_GET_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_thing_list_found(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_SYS_PREFIX;
    request.service_name = DM_URI_THING_LIST_FOUND;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_thing_list_found(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Callback */
    request.callback = dm_client_thing_list_found_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_TOPO_ADD_NOTIFY_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_combine_login(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_EXT_SESSION_PREFIX;
    request.service_name = DM_URI_COMBINE_LOGIN;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_combine_login(node->product_key, node->device_name, node->device_secret, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Callback */
    request.callback = dm_client_combine_login_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_COMBINE_LOGIN_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

int dm_mgr_upstream_combine_logout(_IN_ int devid)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (node->dev_status < IOTX_DM_DEV_STATUS_LOGINED) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_EXT_SESSION_PREFIX;
    request.service_name = DM_URI_COMBINE_LOGOUT;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Get Params And Method */
    res = dm_msg_combine_logout(node->product_key, node->device_name, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Get Msg ID */
    request.msgid = iotx_report_id();

    /* Get Dev ID */
    request.devid = devid;

    /* Callback */
    request.callback = dm_client_combine_logout_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_COMBINE_LOGOUT_REPLY, NULL);
        res = request.msgid;
    }
#endif
    DM_free(request.params);

    return res;
}

#ifdef DEVICE_MODEL_SUBDEV_OTA
int dm_mgr_upstream_thing_firmware_version_update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, res1 = 0;
    dm_mgr_dev_node_t *node = NULL;
    char *uri = NULL;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_OTA_DEVICE_INFORM;
    request.service_name = NULL;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Request URI */
    res = dm_utils_service_name(request.service_prefix, request.service_name,
                                request.product_key, request.device_name, &uri);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_info("DM Send Raw Data:");
    HEXDUMP_INFO(payload, payload_len);

    res = dm_client_publish(uri, (unsigned char *)payload, strlen(payload), dm_client_thing_model_up_raw_reply);

    if (res < SUCCESS_RETURN || res1 < SUCCESS_RETURN) {
        dm_log_info("res of pub is %d:", res);
        DM_free(uri);
        return FAIL_RETURN;
    }

    DM_free(uri);
    return SUCCESS_RETURN;
}
#endif
#endif

int dm_mgr_upstream_thing_model_up_raw(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, res1 = 0;
    dm_mgr_dev_node_t *node = NULL;
    char *uri = NULL;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_SYS_PREFIX;
    request.service_name = DM_URI_THING_MODEL_UP_RAW;
    memcpy(request.product_key, node->product_key, strlen(node->product_key));
    memcpy(request.device_name, node->device_name, strlen(node->device_name));

    /* Request URI */
    res = dm_utils_service_name(request.service_prefix, request.service_name,
                                request.product_key, request.device_name, &uri);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_info("DM Send Raw Data:");
    HEXDUMP_INFO(payload, payload_len);

    res = dm_client_publish(uri, (unsigned char *)payload, payload_len, dm_client_thing_model_up_raw_reply);
#ifdef ALCS_ENABLED
    res1 = dm_server_send(uri, (unsigned char *)payload, payload_len, NULL);
#endif

    if (res < SUCCESS_RETURN || res1 < SUCCESS_RETURN) {
        DM_free(uri);
        return FAIL_RETURN;
    }

    DM_free(uri);
    return SUCCESS_RETURN;
}

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
static int _dm_mgr_upstream_request_assemble(_IN_ int msgid, _IN_ int devid, _IN_ const char *service_prefix,
        _IN_ const char *service_name,
        _IN_ char *params, _IN_ int params_len, _IN_ char *method, _OU_ dm_msg_request_t *request)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    request->msgid = msgid;
    request->devid = devid;
    request->service_prefix = service_prefix;
    request->service_name = service_name;
    memcpy(request->product_key, node->product_key, strlen(node->product_key));
    memcpy(request->device_name, node->device_name, strlen(node->device_name));
    request->params = params;
    request->params_len = params_len;
    request->method = method;

    return SUCCESS_RETURN;
}
#ifdef DEVICE_MODEL_SHADOW
int dm_mgr_upstream_thing_property_desired_get(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, DM_URI_THING_PROPERTY_DESIRED_GET,
                                            payload, payload_len, "thing.property.desired.get", &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Callback */
    request.callback = dm_client_thing_property_desired_get_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
    /*TODO */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        int prop_desired_get_reply = 0;
        res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_PROPERTY_DESIRED_GET_REPLY, &prop_desired_get_reply);
        if (res == SUCCESS_RETURN && prop_desired_get_reply) {
            dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_PROPERTY_DESIRED_GET_REPLY, NULL);
        }
        res = request.msgid;
    }
#endif
    return res;
}

int dm_mgr_upstream_thing_property_desired_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX,
                                            DM_URI_THING_PROPERTY_DESIRED_DELETE,
                                            payload, payload_len, "thing.property.desired.delete", &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Callback */
    request.callback = dm_client_thing_property_desired_delete_reply;
    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        int prop_desired_delete_reply = 0;
        res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_PROPERTY_DESIRED_DELETE_REPLY, &prop_desired_delete_reply);
        if (res == SUCCESS_RETURN && prop_desired_delete_reply) {
            dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_PROPERTY_DESIRED_DELETE_REPLY, NULL);
        }
        res = request.msgid;
    }
#endif
    return res;
}
#endif

int dm_mgr_upstream_thing_property_post(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, DM_URI_THING_EVENT_PROPERTY_POST,
                                            payload, payload_len, "thing.event.property.post", &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Callback */
    request.callback = dm_client_thing_event_post_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_ALL, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        int prop_post_reply = 0;
        res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_POST_REPLY, &prop_post_reply);
        if (res == SUCCESS_RETURN && prop_post_reply) {
            dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY, NULL);
        }
        res = request.msgid;
    }
#endif
    return res;
}

#ifdef LOG_REPORT_TO_CLOUD
static unsigned int log_size = 0;
int dm_mgr_upstream_thing_log_post(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len, int force_upload)
{
    int res = 0;
    dm_msg_request_t request;
    extern REPORT_STATE g_report_status;
    extern char *g_log_poll;

    if (0 == force_upload) {
        if (devid < 0 || payload == NULL || payload_len <= 0) {
            return DM_INVALID_PARAMETER;
        }

        if (log_size + payload_len < OVERFLOW_LEN) {
            log_size = push_log(payload, payload_len);
        } else {
            /* it should NOT happen; it means that it is too late to upload log files */
            reset_log_poll();
            dm_log_err("it it too late to upload log, reset pool");
            return FAIL_RETURN;
        }

        dm_log_info("push log, len is %d, log_size is %d\n", payload_len, log_size);
        if (!(log_size > REPORT_LEN && DONE == g_report_status)) {
            return SUCCESS_RETURN;
        }
    }

    log_size = add_tail();
    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, DM_URI_THING_LOG_POST,
                                            g_log_poll, log_size + 1, "thing.log.post", &request);

    if (res != SUCCESS_RETURN) {
        reset_log_poll();
        return FAIL_RETURN;
    }
    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
    reset_log_poll();
    return res;
}
#endif



int dm_mgr_upstream_thing_event_post(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *method,
                                     _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, service_name_len = 0;
    char *service_name = NULL;
    dm_msg_request_t request;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 ||
        method == NULL || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    service_name_len = strlen(DM_URI_THING_EVENT_POST) + identifier_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, DM_URI_THING_EVENT_POST, identifier_len, identifier);

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, service_name,
                                            payload, payload_len, method, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Callback */
    request.callback = dm_client_thing_event_post_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_ALL, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        int event_post_reply = 0;
        res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_POST_REPLY, &event_post_reply);
        if (res == SUCCESS_RETURN && event_post_reply) {
            dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY, NULL);
        }
        res = request.msgid;
    }
#endif
    DM_free(service_name);

    return res;
}


int dm_mgr_upstream_thing_deviceinfo_update(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, DM_URI_THING_DEVICEINFO_UPDATE,
                                            payload, payload_len, "thing.deviceinfo.update", &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Callback */
    request.callback = dm_client_thing_deviceinfo_update_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY, NULL);
        res = request.msgid;
    }
#endif
    return res;
}

int dm_mgr_upstream_thing_deviceinfo_delete(_IN_ int devid, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0;
    dm_msg_request_t request;

    if (devid < 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, DM_URI_THING_DEVICEINFO_DELETE,
                                            payload, payload_len, "thing.deviceinfo.delete", &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Callback */
    request.callback = dm_client_thing_deviceinfo_delete_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY, NULL);
        res = request.msgid;
    }
#endif
    return res;
}

int dm_mgr_upstream_thing_dsltemplate_get(_IN_ int devid)
{
    int res = 0;
    char *params = "{}";
    int params_len = strlen(params);
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, DM_URI_THING_DSLTEMPLATE_GET,
                                            params, params_len, "thing.dsltemplate.get", &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY, NULL);
        res = request.msgid;
    }
#endif
    return res;
}

int dm_mgr_upstream_thing_dynamictsl_get(_IN_ int devid)
{
    int res = 0;
    char *params = "{\"nodes\":[\"type\",\"identifier\"],\"addDefault\":false}";
    int params_len = strlen(params);
    dm_msg_request_t request;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_t));
    res = _dm_mgr_upstream_request_assemble(iotx_report_id(), devid, DM_URI_SYS_PREFIX, DM_URI_THING_DYNAMICTSL_GET,
                                            params, params_len, "thing.dynamicTsl.get", &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Callback */
    request.callback = dm_client_thing_dynamictsl_get_reply;

    /* Send Message To Cloud */
    res = dm_msg_request(DM_MSG_DEST_CLOUD, &request);
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    if (res == SUCCESS_RETURN) {
        dm_msg_cache_insert(request.msgid, request.devid, IOTX_DM_EVENT_DSLTEMPLATE_GET_REPLY, NULL);
        res = request.msgid;
    }
#endif
    return res;
}

int dm_mgr_upstream_ntp_request(void)
{
    int res = 0;
    const char *ntp_request_fmt = "{\"deviceSendTime\":\"1234\"}";
    char /* *cloud_payload = NULL, */ *uri = NULL;
    dm_msg_request_t request;

    memset(&request, 0, sizeof(dm_msg_request_t));
    request.service_prefix = DM_URI_EXT_NTP_PREFIX;
    request.service_name = DM_URI_NTP_REQUEST;
    HAL_GetProductKey(request.product_key);
    HAL_GetDeviceName(request.device_name);

    /* Request URI */
    res = dm_utils_service_name(request.service_prefix, request.service_name,
                                request.product_key, request.device_name, &uri);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_client_publish(uri, (unsigned char *)ntp_request_fmt, strlen(ntp_request_fmt), dm_client_ntp_response);
    if (res != SUCCESS_RETURN) {
        DM_free(uri); /* DM_free(cloud_payload); */
        return FAIL_RETURN;
    }

    DM_free(uri); /* DM_free(cloud_payload); */
    return SUCCESS_RETURN;
}

static int _dm_mgr_upstream_response_assemble(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
        _IN_ const char *prefix,
        _IN_ const char *service_name, _IN_ int code, _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    request->id.value = msgid;
    request->id.value_length = msgid_len;

    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = service_name;
    memcpy(response->product_key, node->product_key, strlen(node->product_key));
    memcpy(response->device_name, node->device_name, strlen(node->device_name));
    response->code = code;

    return SUCCESS_RETURN;
}

int dm_mgr_upstream_thing_service_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
        _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len, void *ctx)
{
    int res = 0, service_name_len = 0;
    char *service_name = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    if (devid < 0 || msgid == NULL || msgid_len <= 0 || identifier == NULL || identifier_len <= 0 ||
        payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    /* Service Name */
    service_name_len = strlen(DM_URI_THING_SERVICE_RESPONSE) + identifier_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, DM_URI_THING_SERVICE_RESPONSE, identifier_len, identifier);

    res = _dm_mgr_upstream_response_assemble(devid, msgid, msgid_len, DM_URI_SYS_PREFIX, service_name, code, &request,
            &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Current Service Name: %s", service_name);
    if (ctx != NULL) {
        dm_msg_response(DM_MSG_DEST_LOCAL, &request, &response, payload, payload_len, ctx);
    } else {
        dm_msg_response(DM_MSG_DEST_CLOUD, &request, &response, payload, payload_len, ctx);
    }

    DM_free(service_name);
    return SUCCESS_RETURN;
}

int dm_mgr_upstream_thing_property_get_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len,
        _IN_ iotx_dm_error_code_t code,
        _IN_ char *payload, _IN_ int payload_len, _IN_ void *ctx)
{
    int res = 0;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;
    const char *reply_service_name = NULL;
    dm_msg_dest_type_t reply_msg_type;
#ifdef ALCS_ENABLED
    dm_server_alcs_context_t *alcs_context = NULL;
#endif

    if (devid < 0 || msgid == NULL || msgid_len <= 0 ||
        payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    /* Send Property Get Response Message To Local */
    reply_service_name = DM_URI_THING_SERVICE_PROPERTY_GET;
    reply_msg_type = DM_MSG_DEST_LOCAL;

    /* Send Property Get Response Message To Cloud */
    if (NULL == ctx) {
        reply_service_name = DM_URI_THING_SERVICE_PROPERTY_GET_REPLY;
        reply_msg_type = DM_MSG_DEST_CLOUD;
    }

    res = _dm_mgr_upstream_response_assemble(devid, msgid, msgid_len, DM_URI_SYS_PREFIX,
            reply_service_name, code, &request, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Current Service Name: %s", reply_service_name);
    dm_msg_response(reply_msg_type, &request, &response, payload, payload_len, ctx);

#ifdef ALCS_ENABLED
    alcs_context = (dm_server_alcs_context_t *)ctx;

    if (alcs_context) {
        DM_free(alcs_context->ip);
        DM_free(alcs_context->token);
        DM_free(alcs_context);
    }
#endif

    return SUCCESS_RETURN;
}

int dm_mgr_upstream_rrpc_response(_IN_ int devid, _IN_ char *msgid, _IN_ int msgid_len, _IN_ iotx_dm_error_code_t code,
                                  _IN_ char *rrpcid, _IN_ int rrpcid_len, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, service_name_len = 0;
    const char *rrpc_response_service_name = "rrpc/response/%.*s";
    char *service_name = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    if (devid < 0 || msgid == NULL || msgid_len <= 0 ||
        rrpcid == NULL || rrpcid_len <= 0 || payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    /* Service Name */
    service_name_len = strlen(rrpc_response_service_name) + rrpcid_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, rrpc_response_service_name, rrpcid_len, rrpcid);

    res = _dm_mgr_upstream_response_assemble(devid, msgid, msgid_len, DM_URI_SYS_PREFIX, service_name, code, &request,
            &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Current Service Name: %s", service_name);
    dm_msg_response(DM_MSG_DEST_ALL, &request, &response, payload, payload_len, NULL);

    DM_free(service_name);

    return SUCCESS_RETURN;
}
#endif

#ifdef DEPRECATED_LINKKIT
int dm_mgr_deprecated_set_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t tsl_source)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    node->tsl_source = tsl_source;

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_tsl_source(_IN_ int devid, _IN_ iotx_dm_tsl_source_t *tsl_source)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || tsl_source == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    *tsl_source = node->tsl_source;

    return SUCCESS_RETURN;
}

static int dm_mgr_deprecated_search_devid_by_node(_IN_ dm_mgr_dev_node_t *node, _OU_ int *devid)
{
    dm_mgr_ctx *ctx = _dm_mgr_get_ctx();
    dm_mgr_dev_node_t *search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list, dm_mgr_dev_node_t) {
        if (search_node == node) {
            /* dm_log_debug("Device Found, node: %p", node); */
            if (devid) {
                *devid = search_node->devid;
            }
            return SUCCESS_RETURN;
        }
    }

    dm_log_debug("Device Not Found, node: %p", node);
    return FAIL_RETURN;
}

int dm_mgr_deprecated_search_devid_by_device_node(_IN_ void *node, _OU_ int *devid)
{
    int res = 0;

    if (node == NULL || devid == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_mgr_deprecated_search_devid_by_node((dm_mgr_dev_node_t *)node, devid);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_tsl(int devid, iotx_dm_tsl_type_t tsl_type, const char *tsl, int tsl_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (tsl == NULL || tsl_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_create(tsl_type, tsl, tsl_len, &node->dev_shadow);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_property_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_property_data(node->dev_shadow, key, key_len, data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_input_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_input_output_data(DM_SHW_DATA_TARGET_SERVICE_INPUT_DATA, node->dev_shadow, key, key_len, data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_input_output_data(DM_SHW_DATA_TARGET_SERVICE_OUTPUT_DATA, node->dev_shadow, key, key_len,
            data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_event_output_data(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_event_output_data(node->dev_shadow, key, key_len, data);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_data_type(_IN_ void *data, _OU_ dm_shw_data_type_e *type)
{
    if (data == NULL || type == NULL) {
        return DM_INVALID_PARAMETER;
    }

    return dm_shw_get_data_type(data, type);
}

int dm_mgr_deprecated_get_property_number(_IN_ int devid, _OU_ int *number)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_property_number(node->dev_shadow, number);
}

int dm_mgr_deprecated_get_service_number(_IN_ int devid, _OU_ int *number)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_service_number(node->dev_shadow, number);
}

int dm_mgr_deprecated_get_event_number(_IN_ int devid, _OU_ int *number)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || number == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_event_number(node->dev_shadow, number);
}

int dm_mgr_deprecated_get_property_by_index(_IN_ int devid, _IN_ int index, _OU_ void **property)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_property_by_index(node->dev_shadow, index, property);
}

int dm_mgr_deprecated_get_service_by_index(_IN_ int devid, _IN_ int index, _OU_ void **service)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_service_by_index(node->dev_shadow, index, service);
}

int dm_mgr_deprecated_get_event_by_index(_IN_ int devid, _IN_ int index, _OU_ void **event)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || index < 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_event_by_index(node->dev_shadow, index, event);
}

int dm_mgr_deprecated_get_service_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **service)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || service == NULL || *service != NULL) {
        return DM_INVALID_PARAMETER;
    }
    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_service_by_identifier(node->dev_shadow, identifier, service);
}

int dm_mgr_deprecated_get_event_by_identifier(_IN_ int devid, _IN_ char *identifier, _OU_ void **event)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || event == NULL || *event != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_shw_get_event_by_identifier(node->dev_shadow, identifier, event);
}

int dm_mgr_deprecated_get_property_identifier(_IN_ void *property, _OU_ char **identifier)
{
    if (property == NULL || identifier == NULL) {
        return DM_INVALID_PARAMETER;
    }

    return dm_shw_get_property_identifier(property, identifier);
}

int dm_mgr_deprecated_get_service_method(_IN_ void *service, _OU_ char **method)
{
    if (service == NULL || method == NULL || *method != NULL) {
        return DM_INVALID_PARAMETER;
    }

    return dm_shw_get_service_method(service, method);
}

int dm_mgr_deprecated_get_event_method(_IN_ void *event, _OU_ char **method)
{
    if (event == NULL || method == NULL) {
        return DM_INVALID_PARAMETER;
    }

    return dm_shw_get_event_method(event, method);
}

int dm_mgr_deprecated_set_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_property_value(node->dev_shadow, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_property_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_property_value(node->dev_shadow, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_event_output_value(node->dev_shadow, key, key_len, value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_event_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_event_output_value(node->dev_shadow, key, key_len, value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_service_input_output_value(DM_SHW_DATA_TARGET_SERVICE_INPUT_DATA, node->dev_shadow, key, key_len,
            value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_input_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_input_output_value(DM_SHW_DATA_TARGET_SERVICE_INPUT_DATA, node->dev_shadow, key, key_len,
            value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_set_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
        _IN_ int value_len)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_set_service_input_output_value(DM_SHW_DATA_TARGET_SERVICE_OUTPUT_DATA, node->dev_shadow, key, key_len,
            value, value_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_get_service_output_value(_IN_ int devid, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || key == NULL || key_len <= 0 || value == NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_get_service_input_output_value(DM_SHW_DATA_TARGET_SERVICE_OUTPUT_DATA, node->dev_shadow, key, key_len,
            value);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_assemble_property(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
                                        _IN_ lite_cjson_item_t *lite)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_assemble_property(node->dev_shadow, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_assemble_event_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
        _IN_ lite_cjson_item_t *lite)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_assemble_event_output(node->dev_shadow, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_assemble_service_output(_IN_ int devid, _IN_ char *identifier, _IN_ int identifier_len,
        _IN_ lite_cjson_item_t *lite)
{
    int res = 0;
    dm_mgr_dev_node_t *node = NULL;

    if (devid < 0 || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_mgr_search_dev_by_devid(devid, &node);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_shw_assemble_service_output(node->dev_shadow, identifier, identifier_len, lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_mgr_deprecated_upstream_thing_service_response(_IN_ int devid, _IN_ int msgid, _IN_ iotx_dm_error_code_t code,
        _IN_ char *identifier, _IN_ int identifier_len, _IN_ char *payload, _IN_ int payload_len)
{
    int res = 0, service_name_len = 0;
    char *msgid_str = NULL, *service_name = NULL;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    if (devid < 0 || msgid < 0 || identifier == NULL || identifier_len <= 0 ||
        payload == NULL || payload_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    /* Response Msg ID */
    res = dm_utils_itoa(msgid, &msgid_str);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    request.id.value = msgid_str;
    request.id.value_length = strlen(msgid_str);

    /* Service Name */
    service_name_len = strlen(DM_URI_THING_SERVICE_RESPONSE) + identifier_len + 1;
    service_name = DM_malloc(service_name_len);
    if (service_name == NULL) {
        DM_free(msgid_str);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(service_name, 0, service_name_len);
    HAL_Snprintf(service_name, service_name_len, DM_URI_THING_SERVICE_RESPONSE, identifier_len, identifier);

    res = _dm_mgr_upstream_response_assemble(devid, msgid_str, strlen(msgid_str), DM_URI_SYS_PREFIX, service_name, code,
            &request,
            &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Current Service Name: %s", service_name);
    dm_msg_response(DM_MSG_DEST_ALL, &request, &response, payload, payload_len, NULL);

    DM_free(msgid_str);
    DM_free(service_name);
    return SUCCESS_RETURN;
}
#endif
