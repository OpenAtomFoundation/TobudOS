/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _LINKKIT_GATEWAY_LEGACY_H_
#define _LINKKIT_GATEWAY_LEGACY_H_

#include "infra_list.h"
#include "linkkit_gateway_export.h"

#define LINKKIT_GATEWAY_LEGACY_KEY_ID          "id"
#define LINKKIT_GATEWAY_LEGACY_KEY_CODE        "code"
#define LINKKIT_GATEWAY_LEGACY_KEY_DEVID       "devid"
#define LINKKIT_GATEWAY_LEGACY_KEY_SERVICEID   "serviceid"
#define LINKKIT_GATEWAY_LEGACY_KEY_PROPERTYID  "propertyid"
#define LINKKIT_GATEWAY_LEGACY_KEY_EVENTID     "eventid"
#define LINKKIT_GATEWAY_LEGACY_KEY_PAYLOAD     "payload"
#define LINKKIT_GATEWAY_LEGACY_KEY_PRODUCT_KEY "productKey"
#define LINKKIT_GATEWAY_LEGACY_KEY_TIME        "time"
#define LINKKIT_GATEWAY_LEGACY_KEY_VERSION     "version"

#define LINKKIT_GATEWAY_LEGACY_SYNC_DEFAULT_TIMEOUT_MS (10000)

typedef struct {
    int devid;
    linkkit_cbs_t *callback;
    void *callback_ctx;
    struct list_head linked_list;
} linkkit_gateway_dev_callback_node_t;

typedef void (*linkkit_gateway_upstream_async_callback)(int retval, void *ctx);
typedef struct {
    int msgid;
    void *semaphore;
    int code;
    struct list_head linked_list;
} linkkit_gateway_upstream_sync_callback_node_t;

typedef struct {
    int msgid;
    int timeout_ms;
    uint64_t timestamp_ms;
    linkkit_gateway_upstream_async_callback callback;
    void *callback_ctx;
    struct list_head linked_list;
} linkkit_gateway_upstream_async_callback_node_t;

typedef struct {
    void *mutex;
    void *upstream_mutex;
    int is_inited;
    int is_started;
    linkkit_params_t init_params;
    void *dispatch_thread;
    handle_service_fota_callback_fp_t fota_callback;
    struct list_head dev_callback_list;
    struct list_head upstream_sync_callback_list;
    struct list_head upstream_async_callback_list;
} linkkit_gateway_legacy_ctx_t;


#endif
