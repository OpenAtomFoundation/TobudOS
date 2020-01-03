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

__API__ k_tick_t tos_systick_get(void)
{
    TOS_CPU_CPSR_ALLOC();
    k_tick_t tick;

    TOS_CPU_INT_DISABLE();
    tick = k_tick_count;
    TOS_CPU_INT_ENABLE();
    return tick;
}

__API__ void tos_systick_set(k_tick_t tick)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_CPU_INT_DISABLE();
    k_tick_count = tick;
    TOS_CPU_INT_ENABLE();
}

__API__ k_time_t tos_tick2millisec(k_tick_t tick)
{
    return (k_time_t)(tick * K_TIME_MILLISEC_PER_SEC / TOS_CFG_CPU_TICK_PER_SECOND);
}

__API__ k_tick_t tos_millisec2tick(k_time_t ms)
{
    return ((k_tick_t)ms * TOS_CFG_CPU_TICK_PER_SECOND / K_TIME_MILLISEC_PER_SEC);
}

__API__ k_err_t tos_sleep_ms(k_time_t ms)
{
    return tos_task_delay(tos_millisec2tick(ms));
}

__STATIC_INLINE__ k_tick_t time_hmsm2tick(k_time_t hour, k_time_t minute, k_time_t second, k_time_t millisec)
{
    return ((k_tick_t)hour * (k_tick_t)3600u + (k_tick_t)minute * (k_tick_t)60u +
            (k_tick_t)second) * TOS_CFG_CPU_TICK_PER_SECOND +
            (TOS_CFG_CPU_TICK_PER_SECOND * ((k_tick_t)millisec + (k_tick_t)500u / TOS_CFG_CPU_TICK_PER_SECOND)) / (k_tick_t)1000u;
}

__API__ k_err_t tos_sleep_hmsm(k_time_t hour, k_time_t minute, k_time_t second, k_time_t millisec)
{
    return tos_task_delay(time_hmsm2tick(hour, minute, second, millisec));
}

