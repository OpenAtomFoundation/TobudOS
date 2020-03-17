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

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Armink (armink.ztl@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "tos_k.h"
#include "arc/arc_timer.h"
#include "board.h"
#include "arc/arc_exception.h"

__PORT__ void port_systick_config(uint32_t cycle_per_tick) // Configure SysTick to generate an interrupt every cycle_per_tick
{
    timer_int_clear(0); // nsim #define BOARD_SYS_TIMER_ID          TIMER_0
    board_timer_update(cycle_per_tick); // board.c
}
__PORT__ void port_systick_priority_set(uint32_t prio) // Sets the int priority
{
    int_pri_set(INTNO_TIMER0, prio); // get system tick from timer 0 arc_timer.h
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
    arc_aux_write(AUX_TIMER0_CNT, 0); //Writing to this register sets 0 for the timer, and restarts the timer
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
    // SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
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

#if TOS_CFG_FAULT_BACKTRACE_EN > 0u
__STATIC__ void port_fault_do_diagnosis(port_fault_regs_t *regs) // 硬件错误诊断
{
    k_fault_log_writer("\n\n====================== Fault Diagnosis =====================\n");

   
}

__PORT__ void port_fault_diagnosis(void)
{
    k_fault_log_writer("\n\n====================== Fault Diagnosis .. =====================\n");

}

__PORT__ void __NAKED__ HardFault_Handler(void)
{
    k_fault_log_writer("\n\n====================== Fault Diagnosis Handler =====================\n");
}

#endif /* TOS_CFG_FAULT_BACKTRACE_EN */

