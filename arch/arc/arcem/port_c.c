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
#include "arc/arc_timer.h"
#include "board.h"
#include "arc/arc_exception.h"

__PORT__ void port_cpu_reset(void)
{
    exc_entry_reset();
}

__PORT__ void port_systick_config(uint32_t cycle_per_tick)
{
    arc_timer_int_clear(0);
    board_timer_update(cycle_per_tick);
}
__PORT__ void port_systick_priority_set(uint32_t prio)
{
    int_pri_set(BOARD_OS_TIMER_INTNO, prio);
}


#if TOS_CFG_TICKLESS_EN > 0u

__PORT__ k_time_t port_systick_max_delay_millisecond(void)
{
    k_time_t max_millisecond;
    uint32_t max_cycle;

    max_cycle = arc_aux_read(AUX_TIMER0_LIMIT); // AUX_TIMER0_CNT, reset value 0x00FFFFFF
    max_millisecond = (k_time_t)((uint64_t)max_cycle * K_TIME_MILLISEC_PER_SEC / TOS_CFG_CPU_CLOCK); // CLOCK: cycle per second
    return max_millisecond;
}

__PORT__ void port_systick_resume(void)
{
    arc_aux_write(AUX_TIMER0_CTRL, TIMER_CTRL_IE); // enables the generation of an interrupt after the timer has reached its limit
    arc_aux_write(AUX_TIMER0_CNT, 0); // writing to this register sets 0 for the timer, and restarts the timer
}

__PORT__ void port_systick_suspend(void)
{
    arc_aux_write(AUX_TIMER0_CTRL, TIMER_CTRL_NH); // counting is suspended during host debugger interactions with the processor.
}

__PORT__ void port_systick_reload(uint32_t cycle_per_tick)
{
    uint32_t max_cycle;
    uint32_t systick_value;

    max_cycle = arc_aux_read(AUX_TIMER0_LIMIT); // AUX_TIMER0_CNT, reset value 0x00FFFFFF
    systick_value = arc_aux_read(AUX_TIMER0_CNT);

    if (max_cycle - systick_value > cycle_per_tick - 1u) {
        arc_aux_write(AUX_TIMER0_LIMIT, max_cycle);
    } else {
        arc_aux_write(AUX_TIMER0_LIMIT, (cycle_per_tick - 1u) + systick_value);
    }

    arc_aux_write(AUX_TIMER0_CNT, 0);
}

__PORT__ void port_systick_pending_reset(void)
{
    arc_aux_write(AUX_TIMER0_CTRL, TIMER_CTRL_IP); // interrupt pending
}

#endif

#if TOS_CFG_PWR_MGR_EN > 0u

__PORT__ void port_sleep_mode_enter(void)
{
    Asm("sleep");
}

__PORT__ void port_stop_mode_enter(void)
{
    Asm("brk");
}

__PORT__ void port_standby_mode_enter(void)
{
    Asm("nop");
}

#endif
