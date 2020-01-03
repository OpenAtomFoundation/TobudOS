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

#if TOS_CFG_PWR_MGR_EN > 0u

#if TOS_CFG_TICKLESS_EN > 0u
__API__ k_err_t tos_pm_cpu_lpwr_mode_set(k_cpu_lpwr_mode_t cpu_lpwr_mode)
{
    TOS_CPU_CPSR_ALLOC();

    if (!tickless_wkup_alarm_is_installed(cpu_lpwr_mode)) {
        return K_ERR_PM_WKUP_SOURCE_NOT_INSTALL;
    }

    TOS_CPU_INT_DISABLE();
    k_cpu_lpwr_mode = cpu_lpwr_mode;
    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}
#endif

__STATIC__ int pm_device_is_registered(k_pm_device_t *device)
{
    uint8_t i = 0;

    for (i = 0; i < k_pm_device_ctl.count; ++i) {
        if (strcmp(k_pm_device_ctl.mgr[i]->name, device->name) == 0) {
            return 1;
        }
    }
    return 0;
}

__API__ k_err_t tos_pm_device_register(k_pm_device_t *device)
{
    TOS_PTR_SANITY_CHECK(device);

    if (pm_device_is_registered(device)) {
        return K_ERR_PM_DEVICE_ALREADY_REG;
    }

    if (k_pm_device_ctl.count >= K_PM_DEVICE_MAX_COUNT) {
        return K_ERR_PM_DEVICE_OVERFLOW;
    }

    k_pm_device_ctl.mgr[k_pm_device_ctl.count++] = device;

    return K_ERR_NONE;
}

__KERNEL__ void pm_init(void)
{
    memset(&k_pm_device_ctl, 0, sizeof(k_pm_device_ctl));
    k_pm_device_ctl.count = 0u;
}

__STATIC_INLINE__ void pm_cpu_sleep_mode_enter(void)
{
    cpu_sleep_mode_enter();
}

__STATIC_INLINE__ void pm_cpu_stop_mode_enter(void)
{
    cpu_stop_mode_enter();
}

__STATIC_INLINE__ void pm_cpu_standby_mode_enter(void)
{
    cpu_standby_mode_enter();
}

__KERNEL__ void pm_cpu_lpwr_mode_enter(k_cpu_lpwr_mode_t lpwr_mode)
{
    if (TOS_LOW_POWER_MODE_SLEEP == lpwr_mode) {
        pm_cpu_sleep_mode_enter();
    } else if (TOS_LOW_POWER_MODE_STOP == lpwr_mode) {
        pm_device_suspend();
        pm_cpu_stop_mode_enter();
        pm_device_resume();
    } else if (TOS_LOW_POWER_MODE_STANDBY == lpwr_mode) {
        pm_device_suspend();
        pm_cpu_standby_mode_enter();
        pm_device_resume();
    }
}

__KERNEL__ k_cpu_lpwr_mode_t pm_cpu_lpwr_mode_get(void)
{
    return k_cpu_lpwr_mode;
}

__KERNEL__ void pm_idle_pwr_mgr_mode_set(idle_pwrmgr_mode_t idle_pwrmgr_mode)
{
    k_idle_pwr_mgr_mode = idle_pwrmgr_mode;
}

__KERNEL__ int pm_idle_pwr_mgr_mode_is_sleep(void)
{
    return k_idle_pwr_mgr_mode == IDLE_POWER_MANAGER_MODE_SLEEP;
}

__KERNEL__ int pm_idle_pwr_mgr_mode_is_tickless(void)
{
    return k_idle_pwr_mgr_mode == IDLE_POWER_MANAGER_MODE_TICKLESS;
}

__KERNEL__ void pm_power_manager(void)
{
    if (pm_idle_pwr_mgr_mode_is_sleep()) {
        pm_cpu_sleep_mode_enter();
    }

#if TOS_CFG_TICKLESS_EN > 0u
    else if (pm_idle_pwr_mgr_mode_is_tickless()) {
        tickless_proc();
    }
#endif
}

__KERNEL__ int pm_device_suspend(void)
{
    uint8_t i = 0;

    for (i = 0; i < k_pm_device_ctl.count; ++i) {
        if (*k_pm_device_ctl.mgr[i]->suspend) {
            (*k_pm_device_ctl.mgr[i]->suspend)();
        }
    }
    return 0;
}

__KERNEL__ int pm_device_resume(void)
{
    uint8_t i = 0;

    for (i = 0; i < k_pm_device_ctl.count; ++i) {
        if (*k_pm_device_ctl.mgr[i]->resume) {
            (*k_pm_device_ctl.mgr[i]->resume)();
        }
    }
    return 0;
}

#endif

