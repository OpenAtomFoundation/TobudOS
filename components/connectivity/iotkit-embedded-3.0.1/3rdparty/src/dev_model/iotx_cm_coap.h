/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOTX_CM_COAP_H_
#define _IOTX_CM_COAP_H_
#include "iotx_cm.h"
#include "iotx_cm_internal.h"
#include "coap_api.h"


typedef struct {
    uint32_t token_num;
    void *user_data;
    char *topic;
    iotx_cm_data_handle_cb responce_cb;
    void *context;
    dlist_t linked_list;
} coap_response_node_t;

iotx_cm_connection_t *iotx_cm_open_coap(iotx_cm_init_param_t *params);

#endif /* _LINKKIT_CM_H_ */

