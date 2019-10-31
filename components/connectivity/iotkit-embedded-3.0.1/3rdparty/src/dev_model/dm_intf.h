/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _IOT_DM_API_H_
#define _IOT_DM_API_H_

typedef struct {
    void *mutex;
    void *cloud_connectivity;
    void *local_connectivity;
    iotx_dm_event_callback event_callback;
} dm_api_ctx_t;

#if defined(DEPRECATED_LINKKIT)
typedef struct {
    void *mutex;
    int devid;
    lite_cjson_item_t *lite;
} dm_api_property_t;
#endif

#endif
