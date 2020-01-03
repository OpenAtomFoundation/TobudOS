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

#if TOS_CFG_TICKLESS_EN > 0u

__API__ void tos_tickless_wkup_alarm_install(k_cpu_lpwr_mode_t mode, k_tickless_wkup_alarm_t *wkup_alarm)
{
    k_tickless_wkup_alarm[mode] = wkup_alarm;
}

__API__ k_err_t tos_tickless_wkup_alarm_init(k_cpu_lpwr_mode_t mode)
{
    if (!k_tickless_wkup_alarm[mode]) {
        return K_ERR_TICKLESS_WKUP_ALARM_NOT_INSTALLED;
    }

    if (!k_tickless_wkup_alarm[mode]->init) {
        return K_ERR_TICKLESS_WKUP_ALARM_NO_INIT;
    }

    if (k_tickless_wkup_alarm[mode]->init() != 0) {
        return K_ERR_TICKLESS_WKUP_ALARM_INIT_FAILED;
    }
    return K_ERR_NONE;
}

__KERNEL__ int tickless_wkup_alarm_is_installed(k_cpu_lpwr_mode_t mode)
{
    return k_tickless_wkup_alarm[mode] != K_NULL;
}

__STATIC__ int tickless_wkup_alarm_setup(k_cpu_lpwr_mode_t mode, k_time_t expires)
{
    if (k_tickless_wkup_alarm[mode] && k_tickless_wkup_alarm[mode]->setup) {
        return k_tickless_wkup_alarm[mode]->setup(expires);
    }
    return -1;
}

__STATIC__ int tickless_wkup_alarm_dismiss(k_cpu_lpwr_mode_t mode)
{
    if (k_tickless_wkup_alarm[mode] && k_tickless_wkup_alarm[mode]->dismiss) {
        return k_tickless_wkup_alarm[mode]->dismiss();
    }
    return -1;
}

__STATIC__ k_time_t tickless_wkup_alarm_max_delay(k_cpu_lpwr_mode_t mode)
{
    if (k_tickless_wkup_alarm[mode] && k_tickless_wkup_alarm[mode]->max_delay) {
        return k_tickless_wkup_alarm[mode]->max_delay();
    }
    return (k_time_t)0u;
}

__STATIC__ k_time_t tickless_cpu_sleep_time_get(k_cpu_lpwr_mode_t lpwr_mode)
{
    k_tick_t next_expires;
    k_time_t time_sleep_ms, max_delay_ms;

    /* the max time(in millisecond) we can sleep */
    max_delay_ms = tickless_wkup_alarm_max_delay(lpwr_mode);

    next_expires = knl_next_expires_get();
    if (next_expires == TOS_TIME_FOREVER) {
        return max_delay_ms;
    }

    /* how much time should we sleep(in millisecond) */
    time_sleep_ms = (k_time_t)(next_expires * K_TIME_MILLISEC_PER_SEC / k_cpu_tick_per_second);

    return time_sleep_ms > max_delay_ms ? max_delay_ms : time_sleep_ms;
}

__STATIC__ void tickless_systick_suspend(void)
{
    cpu_systick_suspend();
    cpu_systick_pending_reset();
}

__STATIC__ void tickless_systick_resume(void)
{
    cpu_systick_suspend();
    cpu_systick_reset();
    cpu_systick_resume();
}

__STATIC__ void tickless_systick_fix(k_tick_t tick_sleep)
{
    TOS_CPU_CPSR_ALLOC();

    TOS_CPU_INT_DISABLE();

    /* we wakeup from SLEEP mode, fix the system's tick & timer */
    tick_update(tick_sleep);

#if TOS_CFG_TIMER_EN > 0u && TOS_CFG_TIMER_AS_PROC > 0u
    timer_update();
#endif

    tickless_systick_resume();

    TOS_CPU_INT_ENABLE();
}

__STATIC__ void tickless_enter(void)
{
    tickless_systick_suspend();
}

__STATIC__ void tickless_leave(k_time_t time_sleep_ms)
{
    k_tick_t tick_sleep;

    /* how many "ticks" have we sleep */
    tick_sleep = k_cpu_tick_per_second * time_sleep_ms / K_TIME_MILLISEC_PER_SEC;

    tickless_systick_fix(tick_sleep);
}

__KERNEL__ void tickless_proc(void)
{
    k_time_t time_sleep;
    k_cpu_lpwr_mode_t lpwr_mode;

    lpwr_mode = pm_cpu_lpwr_mode_get();

    time_sleep = tickless_cpu_sleep_time_get(lpwr_mode); /* in millisecond */

    tickless_enter();
    tickless_wkup_alarm_setup(lpwr_mode, time_sleep);
    pm_cpu_lpwr_mode_enter(lpwr_mode);
    tickless_wkup_alarm_dismiss(lpwr_mode);
    tickless_leave(time_sleep);
}

__KERNEL__ void tickless_init(void)
{
    pm_idle_pwr_mgr_mode_set(IDLE_POWER_MANAGER_MODE_TICKLESS);

    tos_bsp_tickless_setup();
}

#endif

