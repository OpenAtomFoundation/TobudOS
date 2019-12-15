/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <string.h>
#include "CoAPExport.h"
#include "CoAPResource.h"
#include "CoAPPlatform.h"
#include "CoAPInternal.h"
#include "iotx_coap_internal.h"

#define COAP_PATH_DEFAULT_SUM_LEN (5)

int CoAPPathMD5_sum(const char *path, int len, char outbuf[], int outlen)
{
    unsigned char md5[16] = {0};
    if (!path || !len || !outbuf || !outlen) {
        return -1;
    }

    utils_md5((unsigned char *)path, (size_t)len, md5);
    memcpy(outbuf, md5, outlen > 16 ? 16 : outlen);
    return 0;
}


int CoAPResource_init(CoAPContext *context, int res_maxcount)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    ctx->resource.list_mutex = HAL_MutexCreate();

    HAL_MutexLock(ctx->resource.list_mutex);
    INIT_LIST_HEAD(&ctx->resource.list);
    ctx->resource.count = 0;
    ctx->resource.maxcount = res_maxcount;
    HAL_MutexUnlock(ctx->resource.list_mutex);

    return COAP_SUCCESS;
}

int CoAPResource_deinit(CoAPContext *context)
{
    CoAPResource *node = NULL, *next = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;
    char            tmpbuf[2 * COAP_MAX_PATH_CHECKSUM_LEN + 1] = {0};

    HAL_MutexLock(ctx->resource.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->resource.list, reslist, CoAPResource) {
        if (node->path_type == PATH_FILTER && node->filter_path) {
            coap_free(node->filter_path);
        }
        list_del_init(&node->reslist);
        infra_hex2str((unsigned char *)node->path, COAP_MAX_PATH_CHECKSUM_LEN, tmpbuf);
        COAP_DEBUG("Release the resource %s", tmpbuf);
        coap_free(node);
    }
    ctx->resource.count = 0;
    ctx->resource.maxcount = 0;
    HAL_MutexUnlock(ctx->resource.list_mutex);

    HAL_MutexDestroy(ctx->resource.list_mutex);
    ctx->resource.list_mutex = NULL;
    return COAP_SUCCESS;
}

CoAPResource *CoAPResource_create(const char *path, path_type_t path_type, unsigned short permission,
                                  unsigned int ctype, unsigned int maxage,
                                  CoAPRecvMsgHandler callback)
{
    CoAPResource *resource = NULL;

    if (NULL == path) {
        return NULL;
    }

    if (strlen(path) >= COAP_MSG_MAX_PATH_LEN) {
        return NULL;
    }

    resource = coap_malloc(sizeof(CoAPResource));
    if (NULL == resource) {
        return NULL;
    }

    memset(resource, 0x00, sizeof(CoAPResource));
    if (path_type == PATH_NORMAL) {
        resource->path_type = PATH_NORMAL;
        CoAPPathMD5_sum(path, strlen(path), resource->path, COAP_PATH_DEFAULT_SUM_LEN);
    } else {
        int len = strlen(path) + 1;
        resource->filter_path = coap_malloc(len);
        if (NULL == resource->filter_path) {
            coap_free(resource);
            return NULL;
        }
        resource->path_type = PATH_FILTER;
        memset(resource->filter_path, 0, len);
        strncpy(resource->filter_path, path, strlen(path));

    }
    resource->callback = callback;
    resource->ctype = ctype;
    resource->maxage = maxage;
    resource->permission = permission;

    return resource;
}

int CoAPResource_register(CoAPContext *context, const char *path,
                          unsigned short permission, unsigned int ctype,
                          unsigned int maxage, CoAPRecvMsgHandler callback)
{
    int exist = 0;
    char path_calc[COAP_PATH_DEFAULT_SUM_LEN] = {0};
    CoAPResource *node = NULL, *newnode = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;
    path_type_t type = PATH_NORMAL;

    if (context == NULL) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(ctx->resource.list_mutex);
    if (ctx->resource.count >= ctx->resource.maxcount) {
        HAL_MutexUnlock(ctx->resource.list_mutex);
        COAP_INFO("The resource count exceeds limit, cur %d, max %d",
                  ctx->resource.count,  ctx->resource.maxcount);
        return COAP_ERROR_DATA_SIZE;
    }

    if (strstr(path, "/#") != NULL) {
        type = PATH_FILTER;
    } else {
        CoAPPathMD5_sum(path, strlen(path), path_calc, COAP_PATH_DEFAULT_SUM_LEN);
    }

    list_for_each_entry(node, &ctx->resource.list, reslist, CoAPResource) {
        if (type == PATH_NORMAL && node->path_type == PATH_NORMAL) {
            if (0 == memcmp(path_calc, node->path, COAP_PATH_DEFAULT_SUM_LEN)) {
                /*Alread exist, re-write it*/
                COAP_INFO("CoAPResource_register:Alread exist");
                exist = 1;
                node->callback = callback;
                node->ctype = ctype;
                node->maxage = maxage;
                node->permission = permission;
                COAP_INFO("The resource %s already exist, re-write it", path);
                break;
            }
        } else if (type == PATH_FILTER && node->path_type == PATH_FILTER) {
            if (0 == strncmp((char *)path, node->filter_path, strlen(path))) {
                /*Alread exist, re-write it*/
                COAP_INFO("CoAPResource_register:Alread exist");
                exist = 1;
                node->callback = callback;
                node->ctype = ctype;
                node->maxage = maxage;
                node->permission = permission;
                COAP_INFO("The resource %s already exist, re-write it", path);
                break;
            }
        }
    }

    if (0 == exist) {
        newnode = CoAPResource_create(path, type, permission, ctype, maxage, callback);
        if (NULL != newnode) {
            COAP_DEBUG("CoAPResource_register, context:%p, new node", ctx);
            list_add_tail(&newnode->reslist, &ctx->resource.list);
            ctx->resource.count++;
            COAP_DEBUG("Register new resource %s success, count: %d", path, ctx->resource.count);
        } else {
            COAP_ERR("New resource create failed");
        }
    }

    HAL_MutexUnlock(ctx->resource.list_mutex);

    return COAP_SUCCESS;
}

int CoAPResource_unregister(CoAPContext *context, const char *path)
{
    COAP_DEBUG("This feature isn't supported");
    return COAP_ERROR_UNSUPPORTED;
}

int CoAPResource_topicFilterMatch(const char *filter, const char *topic)
{
    if (filter == NULL || topic == NULL) {
        return -1;
    }
    if (strncmp(filter, topic, strlen(filter) - 1) == 0) {
        if (strlen(topic) > strlen(filter) - 1) {
            const char *more = topic + (strlen(filter) - 1);
            if (strstr(more, "/") == NULL) {
                return 0;
            }
        }
    }
    return -1;
}

CoAPResource *CoAPResourceByPath_get(CoAPContext *context, const char *path)
{
    char path_calc[COAP_PATH_DEFAULT_SUM_LEN] = {0};
    CoAPResource *node = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if (NULL == context || NULL == path) {
        COAP_INFO("%s\n", "NULL == context || NULL == path");
        return NULL;
    }
    COAP_FLOW("CoAPResourceByPath_get, context:%p\n", ctx);

    CoAPPathMD5_sum(path, strlen(path), path_calc, COAP_PATH_DEFAULT_SUM_LEN);

    HAL_MutexLock(ctx->resource.list_mutex);
    list_for_each_entry(node, &ctx->resource.list, reslist, CoAPResource) {
        if (0 == memcmp(path_calc, node->path, COAP_PATH_DEFAULT_SUM_LEN)) {
            HAL_MutexUnlock(ctx->resource.list_mutex);
            COAP_DEBUG("Found the resource: %s", path);
            return node;
        }
    }
    
    list_for_each_entry(node, &ctx->resource.list, reslist, CoAPResource) {
        if (node->path_type == PATH_FILTER && strlen(node->filter_path) > 0) {
            if (CoAPResource_topicFilterMatch(node->filter_path, path) == 0) {
                HAL_MutexUnlock(ctx->resource.list_mutex);
                return node;
            }
        }
    }
    HAL_MutexUnlock(ctx->resource.list_mutex);

    return NULL;
}
