#include "tos.h"
#include "mcu_init.h"

#if TOS_CFG_PWR_MGR_EN > 0u

static int pm_device_uart_init(void)
{
    return 0;
}

static int pm_device_uart_suspend(void)
{
    return 0;
}

static int pm_device_uart_resume(void)
{
    SystemClock_Config();

    return 0;
}

k_pm_device_t pm_device_uart = {
    .name       = "uart",
    .init       = pm_device_uart_init,
    .suspend    = pm_device_uart_suspend,
    .resume     = pm_device_uart_resume,
};

#endif

