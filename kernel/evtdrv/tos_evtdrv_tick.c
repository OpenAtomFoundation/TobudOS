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

#include "tos_evtdrv.h"

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

__API__ evtdrv_time_t tos_evtdrv_tick2millisec(evtdrv_tick_t tick)
{
    return (evtdrv_time_t)(tick * EVTDRV_TIME_MILLISEC_PER_SEC / TOS_CFG_CPU_TICK_PER_SECOND);
}

__API__ evtdrv_tick_t tos_evtdrv_millisec2tick(evtdrv_time_t ms)
{
    return ((evtdrv_tick_t)ms * TOS_CFG_CPU_TICK_PER_SECOND / EVTDRV_TIME_MILLISEC_PER_SEC);
}

__API__ evtdrv_tick_t tos_evtdrv_systick_get(void)
{
    TOS_CPU_CPSR_ALLOC();
    evtdrv_tick_t tick;

    TOS_CPU_INT_DISABLE();
    tick = evtdrv_tick_count;
    TOS_CPU_INT_ENABLE();
    return tick;
}

__API__ void tos_evtdrv_tick_handler(void)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_CPU_INT_DISABLE();
    ++evtdrv_tick_count;
    TOS_CPU_INT_ENABLE();
}

#endif

