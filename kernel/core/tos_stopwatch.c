/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include "tos_k.h"

__API__ k_err_t tos_stopwatch_create(k_stopwatch_t *stopwatch)
{
    TOS_PTR_SANITY_CHECK(stopwatch);

    stopwatch->until = 0u;
    TOS_OBJ_INIT(stopwatch, KNL_OBJ_TYPE_STOPWATCH);

    return K_ERR_NONE;
}

__API__ k_err_t tos_stopwatch_destroy(k_stopwatch_t *stopwatch)
{
    TOS_PTR_SANITY_CHECK(stopwatch);
    TOS_OBJ_VERIFY(stopwatch, KNL_OBJ_TYPE_STOPWATCH);

    stopwatch->until = 0u;
    TOS_OBJ_DEINIT(stopwatch);

    return K_ERR_NONE;
}

__API__ k_err_t tos_stopwatch_countdown(k_stopwatch_t *stopwatch, k_tick_t tick)
{
    k_tick_t now;

    TOS_PTR_SANITY_CHECK(stopwatch);
    TOS_OBJ_VERIFY(stopwatch, KNL_OBJ_TYPE_STOPWATCH);

    now = tos_systick_get();
    stopwatch->until = now + tick;

    return K_ERR_NONE;
}

__API__ k_err_t tos_stopwatch_countdown_ms(k_stopwatch_t *stopwatch, k_time_t millisec)
{
    k_tick_t tick;

    TOS_PTR_SANITY_CHECK(stopwatch);
    TOS_OBJ_VERIFY(stopwatch, KNL_OBJ_TYPE_STOPWATCH);

    tick = tos_millisec2tick(millisec);
    return tos_stopwatch_countdown(stopwatch, tick);
}

__API__ void tos_stopwatch_delay(k_tick_t tick)
{
    k_tick_t now;

    now = tos_systick_get();
    while ((tos_systick_get() - now) < tick) {
        ;
    }
}

__API__ void tos_stopwatch_delay_ms(k_time_t millisec)
{
    k_tick_t tick;

    tick = tos_millisec2tick(millisec);
    tos_stopwatch_delay(tick);
}

__API__ k_tick_t tos_stopwatch_remain(k_stopwatch_t *stopwatch)
{
    k_tick_t now;

    TOS_PTR_SANITY_CHECK_RC(stopwatch, (k_tick_t)-1);
    TOS_OBJ_VERIFY_RC(stopwatch, KNL_OBJ_TYPE_STOPWATCH, (k_tick_t)-1);

    if (tos_stopwatch_is_expired(stopwatch)) {
        return (k_tick_t)0u;
    }

    now = tos_systick_get();
    return stopwatch->until - now;
}

__API__ k_time_t tos_stopwatch_remain_ms(k_stopwatch_t *stopwatch)
{
    k_tick_t now;

    TOS_PTR_SANITY_CHECK_RC(stopwatch, (k_time_t)-1);
    TOS_OBJ_VERIFY_RC(stopwatch, KNL_OBJ_TYPE_STOPWATCH, (k_time_t)-1);

    if (tos_stopwatch_is_expired(stopwatch)) {
        return (k_tick_t)0u;
    }

    now = tos_systick_get();
    return (k_time_t)(((stopwatch->until) - now + TOS_CFG_CPU_TICK_PER_SECOND - 1) / TOS_CFG_CPU_TICK_PER_SECOND);
}

__API__ int tos_stopwatch_is_expired(k_stopwatch_t *stopwatch)
{
    k_tick_t now;

    TOS_PTR_SANITY_CHECK_RC(stopwatch, K_FALSE);
    TOS_OBJ_VERIFY_RC(stopwatch, KNL_OBJ_TYPE_STOPWATCH, K_FALSE);

    now = tos_systick_get();
    return now >= stopwatch->until ? K_TRUE : K_FALSE;
}

