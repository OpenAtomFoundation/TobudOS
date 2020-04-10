#include "ll/ll_wdt.h"
#include "hal/hal_wdt.h"
#include "hal/hal_common.h"


/**
 * @brief Enable the watch dog. Once enabled it cannot be disabled.
 * @return This function has no return value.
 */
void HAL_WDT_Enable(void)
{
    LL_WDT_Enable();
}


/**
 * @brief Set the watch dog timeout period
 * @param  period: the period to trigger an wdt interrupt. For example WDT_PERIOD_3FF means the period wdt int trigger is 0x3ff cycles
 * @return This function has no return value.
 */
void HAL_WDT_SetTimeoutPeriod(WDT_TimeoutPeriod period)
{
    assert_param(IS_WDT_TIMEOUT_PERIOD(period));

    LL_WDT_TimeoutPeriod(period);
}


/**
 * @brief Clears the watchdog interrupt. This can be used to clear the interrupt without restarting the watchdog counter.
 * @return return 1 when an interrupt is triggered.
 */
uint8_t HAL_WDT_ClrIRQ(void)
{
    return LL_WDT_ClrIRQ();
}


/**
 * @brief Get the current value of the internal counter
 * @return return the current value of the internal counter.
 */
uint16_t HAL_WDT_CurrentCounter(void)
{
    return LL_WDT_CurrentCounter();
}


/**
 * @brief To restart the WDT counter (and clear WDT interrupt.)
 * @return This function has no return value.
 */
void HAL_WDT_Restart(void)
{
    LL_WDT_Restart();
}


/**
 * @brief To get the interrupt status of the WDT.
 * @return return the interrupt status. 0 means interrupt is inactive, 1 means interrupt is active.
 */
uint8_t HAL_WDT_IntStatus(void)
{
    return LL_WDT_IntStatus();
}


/**
 * @brief Initialize the watch dog
 * @param config: Struct to define parameter of watch dog.
 * @return This function has no return value.
 */
void HAL_WDT_Init(WDT_InitTypeDef config)
{
    assert_param(IS_WDT_RESET_MODE(config.mode));
    assert_param(IS_WDT_TIMEOUT_PERIOD(config.period));
    assert_param(IS_WDT_RESET_PULSE(config.pulse_width));

    LL_WDT_ResetModeSet(config.mode);
    LL_WDT_TimeoutPeriod(config.period);
    LL_WDT_ResetPulseWidthSet(config.pulse_width);

    // NOTE: reset the counter before the first run.
    LL_WDT_Restart();
}

