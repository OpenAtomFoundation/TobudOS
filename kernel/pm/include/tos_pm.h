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

#ifndef _TOS_PM_H_
#define  _TOS_PM_H_

#if TOS_CFG_PWR_MGR_EN > 0u

#define K_PM_DEVICE_MAX_COUNT       10u

typedef enum idle_power_manager_mode_en {
    IDLE_POWER_MANAGER_MODE_SLEEP,
    IDLE_POWER_MANAGER_MODE_TICKLESS,
} idle_pwrmgr_mode_t;

/*
                                              Low-power mode summary
|-------------------------------------------------------------------------------------------------------------------|
|   Mode name    |       Entry      |       Wakeup       |   Effect on 1.8V   |  Effect on VDD  | Voltage regulator |
|                |                  |                    |   domain clocks    |  domain clocks  |                   |
|----------------|------------------|--------------------|--------------------|-----------------|-------------------|
| Sleep          | WFI              | Any interrupt      | CPU clock OFF      |                 |                   |
| (Sleep now or  |------------------|--------------------| no effect on other | None            | ON                |
| Sleep-on-exit) | WFE              | Wakeup event       | clocks or analog   |                 |                   |
|                |                  |                    | clock sources      |                 |                   |
|----------------|------------------|--------------------|--------------------|-----------------|-------------------|
|                |                  |                    |                    |                 | ON or in          |
| Stop           | PDDS and LPDS    | Any EXTI line      |                    |                 | low-power mode    |
|                | bits + SLEEPDEEP | (configured in the |                    |                 | (depends on Power |
|                | bit + WFI or WFE | EXTI registers     |                    |                 | control register  |
|                |                  |                    |                    |                 | (PWR_CR)          |
|----------------|------------------|--------------------| All 1.8V domain    | HSI and HSE     |-------------------|
|                |                  | WKUP pin rising    | clocks OFF         | oscillators OFF |                   |
| Standby        | PDDS bit +       | edge, RTC alarm,   |                    |                 |                   |
|                | SLEEPDEEP bit +  | external reset in  |                    |                 | OFF               |
|                | WFI or WFE       | NRST pin,          |                    |                 |                   |
|                |                  | IWDG reset         |                    |                 |                   |
|                |                  |                    |                    |                 |                   |
|-------------------------------------------------------------------------------------------------------------------|
 */
typedef enum k_cpu_low_power_mode_en {
    TOS_LOW_POWER_MODE_SLEEP    = 0,    /* wakeup source: systick/tim/rtc */
    TOS_LOW_POWER_MODE_STOP,            /* wakeup source: rtc wakeup/alarm */
    TOS_LOW_POWER_MODE_STANDBY,         /* wakeup source: rtc alarm */
    __LOW_POWER_MODE_DUMMY,
} k_cpu_lpwr_mode_t;

typedef struct k_pm_device_st {
    char *name;

    int (*init)(void);
    int (*suspend)(void);
    int (*resume)(void);
} k_pm_device_t;

typedef struct pm_device_control_st {
    uint8_t         count;
    k_pm_device_t  *mgr[K_PM_DEVICE_MAX_COUNT];
} pm_device_ctl_t;

#if TOS_CFG_TICKLESS_EN > 0u
__API__ k_err_t tos_pm_cpu_lpwr_mode_set(k_cpu_lpwr_mode_t cpu_lpwr_mode);
#endif

__API__ k_err_t tos_pm_device_register(k_pm_device_t *device);

__KNL__ void pm_init(void);

__KNL__ void pm_cpu_lpwr_mode_enter(k_cpu_lpwr_mode_t lpwr_mode);

__KNL__ k_cpu_lpwr_mode_t pm_cpu_lpwr_mode_get(void);

__KNL__ void pm_idle_pwr_mgr_mode_set(idle_pwrmgr_mode_t idle_pwrmgr_mode);

__KNL__ int  pm_idle_pwr_mgr_mode_is_sleep(void);

__KNL__ int  pm_idle_pwr_mgr_mode_is_tickless(void);

__KNL__ void pm_power_manager(void);

__KNL__ int  pm_device_suspend(void);

__KNL__ int  pm_device_resume(void);

#endif /* TOS_CFG_PWR_MGR_EN */

#endif /* _TOS_PM_H_ */

