/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef _IOTX_CM_MQTT_H_
#define _IOTX_CM_MQTT_H_

#include "iotx_cm.h"
#include "iotx_cm_internal.h"


typedef struct {
    uintptr_t packet_id;
    char * topic;
    void * user_data;
    iotx_mqtt_event_handle_func_fpt sub_state_cb;
    iotx_cm_data_handle_cb sub_recieve_cb;
    dlist_t linked_list;
} mqtt_sub_node_t;

iotx_cm_connection_t *iotx_cm_open_mqtt(iotx_cm_init_param_t *params);


#endif /* _LINKKIT_CM_H_ */
