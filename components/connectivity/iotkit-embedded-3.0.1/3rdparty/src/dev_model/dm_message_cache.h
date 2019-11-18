/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#if !defined(DM_MESSAGE_CACHE_DISABLED)
#ifndef _DM_MESSAGE_CACHE_H_
#define _DM_MESSAGE_CACHE_H_

#include "iotx_dm_internal.h"

#define DM_MSG_CACHE_TIMEOUT_MS_DEFAULT (10000)

typedef struct {
    int msgid;
    int devid;
    iotx_dm_event_types_t response_type;
    char *data;
    uint64_t ctime;
    struct list_head linked_list;
} dm_msg_cache_node_t;

typedef struct {
    void *mutex;
    int dmc_list_size;
    struct list_head dmc_list;
} dm_msg_cache_ctx_t;

int dm_msg_cache_init(void);
int dm_msg_cache_deinit(void);
int dm_msg_cache_insert(int msg_id, int devid, iotx_dm_event_types_t type, char *data);
int dm_msg_cache_search(_IN_ int msg_id, _OU_ dm_msg_cache_node_t **node);
int dm_msg_cache_remove(int msg_id);
void dm_msg_cache_tick(void);

#endif
#endif