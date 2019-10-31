/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_IPC_H_
#define _DM_IPC_H_

#include "iotx_dm_internal.h"

typedef struct {
    iotx_dm_event_types_t type;
    char *data;
} dm_ipc_msg_t;

typedef struct {
    void *data;
    struct list_head linked_list;
} dm_ipc_msg_node_t;

typedef struct {
    int max_size;
    int size;
    struct list_head message_list;
} dm_ipc_msg_list_t;

typedef struct {
    void *mutex;
    dm_ipc_msg_list_t msg_list;
} dm_ipc_t;

int dm_ipc_init(int max_size);
void dm_ipc_deinit(void);
int dm_ipc_msg_insert(void *data);
int dm_ipc_msg_next(void **data);

#endif
