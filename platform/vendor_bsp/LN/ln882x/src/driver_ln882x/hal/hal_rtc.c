#include "ll/ll_rtc.h"
#include "hal/hal_rtc.h"

/**
 * @brief  Get the current value of the internal counter.
 * @return return the interal counter of rtc.
 */
uint32_t HAL_RTC_GetCurValue(void)
{
    return LL_RTC_GetCurValue();
}

/**
 * @brief  Set the match counter of rtc.
 * @param  matchCounter: the match counter of rtc. 
 *         The rtc counter always count forward. when the load counter reach the match counter an interrupt will trigger.
 * @return This function has no return value.
 */
void HAL_RTC_SetMatchCounter(uint32_t matchCounter)
{
    LL_RTC_SetMatchCounter(matchCounter);
}

/**
 * @brief  Set the load counter of rtc.
 * @param  loadCounter: the load counter of rtc. It is the value where rtc start counting forward
 * @return This function has no return value.
 */
void HAL_RTC_SetLoadCounter(uint32_t loadCounter)
{
    LL_RTC_SetLoadCounter(loadCounter);
}

/**
 * @brief  To force the counter to wrap when a match occurs
 * @param  en: 1 to enable a wrap, 0 to disable a wrap
 * @return This function has no return value.
 */
void HAL_RTC_Wrap(RTC_WrapEn wrapEn)
{
    LL_RTC_Wrap(wrapEn);
}

/**
 * @brief  Enable the RTC counter.
 * @param  en: 1 to enable the counter, 0 to disable the counter
 * @return This function has no return value.
 */
void HAL_RTC_Enable(RTC_Enable enable)
{
    LL_RTC_Enable(enable);
}

/**
 * @brief  Set whether to mask the rtc interrupt or not
 * @param  en: 1 to mask the interrupt, 0 to unmask the interrupt
 * @return This function has no return value.
 */
void HAL_RTC_IntMask(RTC_IntMask mask)
{
    LL_RTC_IntMask(mask);
}

/**
 * @brief  Set whether to enable the rtc interrupt or not
 * @param  en: 1 to enable the interrupt, 0 to disable the interrupt
 * @return This function has no return value.
 */
void HAL_RTC_IntEnable(RTC_IntEn enable)
{
    LL_RTC_IntEnable(enable);
}
/**
 * @brief  Get the status of rtc interrupt, after the mask
 * @return The interrupt status after the int mask. 1 means interrupt is active, 0 means interrupt is inactive.
 */
RTC_IntStatus HAL_RTC_IntStatus(void)
{
    return (RTC_IntStatus)LL_RTC_IntStatus();
}

/**
 * @brief  Get the raw status of rtc interrupt, before the mask
 * @return The interrupt status before the int mask. 1 means interrupt is active, 0 means interrupt is inactive.
 */
RTC_IntStatus HAL_RTC_IntRawStatus(void)
{
    return (RTC_IntStatus)LL_RTC_IntRawStatus();
}

/**
 * @brief  Clear rtc interrupt
 * @return This function has no return value.
 */
void HAL_RTC_ClearInt(void)
{
    LL_RTC_ClearInt();
}

/**
 * @brief Initialize the RTC
 * @param config: Struct to define parameter of RTC. 
 * @return This function has no return value.
 */
void HAL_RTC_Init(RTC_InitTypeDef config)
{
    LL_RTC_SetMatchCounter(config.counter_match);
    LL_RTC_SetLoadCounter(config.counter_load);
    
    LL_RTC_Wrap(config.wrap_en);
    LL_RTC_IntMask(config.int_mask);
    LL_RTC_IntEnable(config.int_en);
}
uint32_t HAL_RTC_GetMatchCounter(void)
{
    return LL_RTC_GetMatchCounter();
}

