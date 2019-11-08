/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _LINKKIT_SOLO_LEGACY_H_
#define _LINKKIT_SOLO_LEGACY_H_

#include "linkkit_export.h"

#define LINKKIT_SOLO_LEGACY_KEY_ID          "id"
#define LINKKIT_SOLO_LEGACY_KEY_CODE        "code"
#define LINKKIT_SOLO_LEGACY_KEY_DEVID       "devid"
#define LINKKIT_SOLO_LEGACY_KEY_SERVICEID   "serviceid"
#define LINKKIT_SOLO_LEGACY_KEY_PROPERTYID  "propertyid"
#define LINKKIT_SOLO_LEGACY_KEY_EVENTID     "eventid"
#define LINKKIT_SOLO_LEGACY_KEY_PAYLOAD     "payload"
#define LINKKIT_SOLO_LEGACY_KEY_CONFIG_ID   "configId"
#define LINKKIT_SOLO_LEGACY_KEY_CONFIG_SIZE "configSize"
#define LINKKIT_SOLO_LEGACY_KEY_GET_TYPE    "getType"
#define LINKKIT_SOLO_LEGACY_KEY_SIGN        "sign"
#define LINKKIT_SOLO_LEGACY_KEY_SIGN_METHOD "signMethod"
#define LINKKIT_SOLO_LEGACY_KEY_URL         "url"
#define LINKKIT_SOLO_LEGACY_KEY_VERSION     "version"

typedef struct {
    int msgid;
    handle_post_cb_fp_t callback;
    struct list_head linked_list;
} linkkit_solo_upstream_callback_node_t;

typedef struct {
    void *mutex;
    void *upstream_mutex;
    int is_started;
    int is_leaved;
    linkkit_ops_t *user_callback;
    void *user_context;
    handle_service_cota_callback_fp_t cota_callback;
    handle_service_fota_callback_fp_t fota_callback;
    struct list_head callback_list;
} linkkit_solo_legacy_ctx_t;

#endif
