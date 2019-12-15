#include "infra_config.h"

#ifdef INFRA_TIMER
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "infra_types.h"
#include "infra_timer.h"

uint64_t HAL_UptimeMs(void);

void iotx_time_start(iotx_time_t *timer)
{
    if (!timer) {
        return;
    }

    timer->time = HAL_UptimeMs();
}

uint32_t utils_time_spend(iotx_time_t *start)
{
    uint32_t now, res;

    if (!start) {
        return 0;
    }

    now = HAL_UptimeMs();
    res = now - start->time;
    return res;
}

uint32_t iotx_time_left(iotx_time_t *end)
{
    uint32_t now, res;

    if (!end) {
        return 0;
    }

    if (utils_time_is_expired(end)) {
        return 0;
    }

    now = HAL_UptimeMs();
    res = end->time - now;
    return res;
}

uint32_t utils_time_is_expired(iotx_time_t *timer)
{
    uint32_t cur_time;

    if (!timer) {
        return 1;
    }

    cur_time = HAL_UptimeMs();
    /*
     *  WARNING: Do NOT change the following code until you know exactly what it do!
     *
     *  check whether it reach destination time or not.
     */
    if ((cur_time - timer->time) < (UINT32_MAX / 2)) {
        return 1;
    } else {
        return 0;
    }
}

void iotx_time_init(iotx_time_t *timer)
{
    if (!timer) {
        return;
    }

    timer->time = 0;
}

void utils_time_countdown_ms(iotx_time_t *timer, uint32_t millisecond)
{
    if (!timer) {
        return;
    }

    timer->time = HAL_UptimeMs() + millisecond;
}

uint32_t utils_time_get_ms(void)
{
    return HAL_UptimeMs();
}
#endif

