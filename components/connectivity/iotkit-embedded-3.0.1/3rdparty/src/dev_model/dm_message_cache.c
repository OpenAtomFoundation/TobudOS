/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "iotx_dm_internal.h"

#if !defined(DM_MESSAGE_CACHE_DISABLED)

dm_msg_cache_ctx_t g_dm_msg_cache_ctx;

dm_msg_cache_ctx_t *_dm_msg_cache_get_ctx(void)
{
    return &g_dm_msg_cache_ctx;
}

static void _dm_msg_cache_mutex_lock(void)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();
    if (ctx->mutex) {
        HAL_MutexLock(ctx->mutex);
    }
}

static void _dm_msg_cache_mutex_unlock(void)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();
    if (ctx->mutex) {
        HAL_MutexUnlock(ctx->mutex);
    }
}

int dm_msg_cache_init(void)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();

    memset(ctx, 0, sizeof(dm_msg_cache_ctx_t));

    /* Create Mutex */
    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }

    /* Init Message Cache List */
    INIT_LIST_HEAD(&ctx->dmc_list);

    return SUCCESS_RETURN;
}

int dm_msg_cache_deinit(void)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();
    dm_msg_cache_node_t *node = NULL;
    dm_msg_cache_node_t *next = NULL;

    _dm_msg_cache_mutex_lock();
    list_for_each_entry_safe(node, next, &ctx->dmc_list, linked_list, dm_msg_cache_node_t) {
        list_del(&node->linked_list);
        if (node->data) {
            DM_free(node->data);
        }
        DM_free(node);
        _dm_msg_cache_mutex_unlock();
    }
    _dm_msg_cache_mutex_unlock();

    if (ctx->mutex) {
        HAL_MutexDestroy(ctx->mutex);
    }

    return SUCCESS_RETURN;
}

int dm_msg_cache_insert(int msgid, int devid, iotx_dm_event_types_t type, char *data)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();
    dm_msg_cache_node_t *node = NULL;

    dm_log_debug("dmc list size: %d", ctx->dmc_list_size);
    if (ctx->dmc_list_size >= CONFIG_MSGCACHE_QUEUE_MAXLEN) {
        return FAIL_RETURN;
    }

    node = DM_malloc(sizeof(dm_msg_cache_node_t));
    if (node == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(node, 0, sizeof(dm_msg_cache_node_t));

    node->msgid = msgid;
    node->devid = devid;
    node->response_type = type;
    node->data = data;
    node->ctime = HAL_UptimeMs();
    INIT_LIST_HEAD(&node->linked_list);

    _dm_msg_cache_mutex_lock();
    list_add_tail(&node->linked_list, &ctx->dmc_list);
    ctx->dmc_list_size++;
    _dm_msg_cache_mutex_unlock();

    return SUCCESS_RETURN;
}

int dm_msg_cache_search(_IN_ int msgid, _OU_ dm_msg_cache_node_t **node)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();
    dm_msg_cache_node_t *search_node = NULL;

    if (msgid <= 0 || node == NULL || *node != NULL) {
        return DM_INVALID_PARAMETER;
    }

    _dm_msg_cache_mutex_lock();
    list_for_each_entry(search_node, &ctx->dmc_list, linked_list, dm_msg_cache_node_t) {
        if (search_node->msgid == msgid) {
            *node = search_node;
            _dm_msg_cache_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    _dm_msg_cache_mutex_unlock();
    return FAIL_RETURN;
}

int dm_msg_cache_remove(int msgid)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();
    dm_msg_cache_node_t *node = NULL;
    dm_msg_cache_node_t *next = NULL;

    _dm_msg_cache_mutex_lock();
    list_for_each_entry_safe(node, next, &ctx->dmc_list, linked_list, dm_msg_cache_node_t) {
        if (node->msgid == msgid) {
            list_del(&node->linked_list);
            if (node->data) {
                DM_free(node->data);
            }
            ctx->dmc_list_size--;
            DM_free(node);
            dm_log_debug("Remove Message ID: %d", msgid);
            _dm_msg_cache_mutex_unlock();
            return SUCCESS_RETURN;
        }
    }

    _dm_msg_cache_mutex_unlock();
    return FAIL_RETURN;
}

void dm_msg_cache_tick(void)
{
    dm_msg_cache_ctx_t *ctx = _dm_msg_cache_get_ctx();
    dm_msg_cache_node_t *node = NULL;
    dm_msg_cache_node_t *next = NULL;
    uint64_t current_time = HAL_UptimeMs();

    _dm_msg_cache_mutex_lock();
    list_for_each_entry_safe(node, next, &ctx->dmc_list, linked_list, dm_msg_cache_node_t) {
        if (current_time < node->ctime) {
            node->ctime = current_time;
        }
        if (current_time - node->ctime >= DM_MSG_CACHE_TIMEOUT_MS_DEFAULT) {
            dm_log_debug("Message ID Timeout: %d", node->msgid);
            /* Send Timeout Message To User */
            dm_msg_send_msg_timeout_to_user(node->msgid, node->devid, node->response_type);
            list_del(&node->linked_list);
            if (node->data) {
                DM_free(node->data);
            }
            DM_free(node);
            ctx->dmc_list_size--;
        }
    }
    _dm_msg_cache_mutex_unlock();
}
#endif