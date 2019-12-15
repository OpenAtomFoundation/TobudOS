/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "infra_config.h"

int at_conn_init(void);
int at_parser_init(void);

/* See detail struct definition in at_wrapper.h */
struct at_conn_input;
struct at_mqtt_input;
int at_conn_input(struct at_conn_input *param);
int at_mqtt_input(struct at_mqtt_input *param);

int IOT_ATM_Init(void)
{
#ifdef AT_PARSER_ENABLED
    if (at_parser_init() < 0) {
    	return -1;
    }
#endif

#ifdef AT_TCP_ENABLED
    if (at_conn_init() < 0) {
    	return -1;
    }
#endif

    return 0;
}


int IOT_ATM_Input(void * param) 
{ 
	int ret = -1;

#if defined(AT_TCP_ENABLED)
    ret = at_conn_input((struct at_conn_input *)param);
#elif defined(AT_MQTT_ENABLED)
    ret = at_mqtt_input((struct at_mqtt_input *)param);
#endif

    return ret;
}
