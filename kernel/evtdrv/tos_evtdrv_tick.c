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

