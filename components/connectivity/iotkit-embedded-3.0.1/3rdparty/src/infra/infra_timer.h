/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef _INFRA_TIMER_H_
#define _INFRA_TIMER_H_

#include "infra_types.h"

typedef struct {
    uint32_t time;
} iotx_time_t;


void iotx_time_start(iotx_time_t *timer);

uint32_t utils_time_spend(iotx_time_t *start);

uint32_t iotx_time_left(iotx_time_t *end);

uint32_t utils_time_is_expired(iotx_time_t *timer);

void iotx_time_init(iotx_time_t *timer);

void utils_time_countdown_ms(iotx_time_t *timer, uint32_t millisecond);

uint32_t utils_time_get_ms(void);

#endif /* _IOTX_COMMON_TIMER_H_ */


