#include "tos.h"
#include "tickless/bsp_pm_device.h"
#include "tickless/bsp_tickless_alarm.h"

int tos_bsp_tickless_setup(void)
{
#if TOS_CFG_TICKLESS_EN > 0u
    tos_pm_device_register(&pm_device_uart);

    /* we set a default one shot timer here(systick) */
    // tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_SLEEP, &tickless_wkup_alarm_systick);
    tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_SLEEP, &tickless_wkup_alarm_tim);
    tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_SLEEP);

    tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_STOP, &tickless_wkup_alarm_rtc_wkupirq);
    tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_STOP);

    tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_STANDBY, &tickless_wkup_alarm_rtc_alarmirq);
    tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_STANDBY);

    tos_pm_cpu_lpwr_mode_set(TOS_LOW_POWER_MODE_SLEEP);
#endif

    return 0;
}

