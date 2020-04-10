#include "proj_config.h"
#include "ll/ll_timer.h"
#include "hal/hal_timer.h"
#include "ll/ll_syscon.h"
#include "hal/hal_common.h"


/**
 * @brief Initialize the timer according to the input param config.
 * @param  config: the structure to initialize the timer, user_freq must meet the requierement: [312.5KHz, 80MHz]
 * @return This function has no return.
 */
void HAL_TIMER_Init(TIMER_InitTypeDef config)
{
    assert_param(IS_TIMER_INDEX(config.index));
    assert_param(IS_TIMER_MODE(config.mode));
    assert_param(IS_TIMER_MASK(config.mask));
    assert_param( ((APBUS0_CLOCK / config.user_freq) >= 1 ) && ( (APBUS0_CLOCK / config.user_freq) <= 256 ));

    HAL_TIMER_Enable(config.index, TIMER_DISABLE);

    switch(config.index)
    {
        case TIMER1:
        {
            LL_TIMER_Mode_Set(config.index, config.mode);
            LL_TIMER_IntMask_Set(config.index, config.mask);
            LL_SYSCON_Timer1Division( APBUS0_CLOCK / config.user_freq );
            break;
        }
        case TIMER2:
        {
            LL_TIMER_Mode_Set(config.index, config.mode);
            LL_TIMER_IntMask_Set(config.index, config.mask);
            LL_SYSCON_Timer2Division( APBUS0_CLOCK / config.user_freq );
            break;
        }
        case TIMER3:
        {
            LL_TIMER_Mode_Set(config.index, config.mode);
            LL_TIMER_IntMask_Set(config.index, config.mask);
            LL_SYSCON_Timer3Division( APBUS0_CLOCK / config.user_freq );
            break;
        }
        case TIMER4:
        {
            LL_TIMER_Mode_Set(config.index, config.mode);
            LL_TIMER_IntMask_Set(config.index, config.mask);
            LL_SYSCON_Timer4Division( APBUS0_CLOCK / config.user_freq );
            break;
        }
        default:
            break;
    }
}



/**
 * @brief Set the loadcounter of the timer
 * @param  index: define which timer to operate.
 * @param  count: the counter is 24bit.
 * @return This function has no return.
 */
void HAL_TIMER_LoadCount_Set(TIMER_Index index, uint32_t count)
{
    assert_param(IS_TIMER_INDEX(index));

    LL_TIMER_LoadCount_Set(index, count);
}


/**
 * @brief Get timer loadcount.
 *
 * @param index timer index.
 * @return uint32_t loadcount of this timer.
 */
uint32_t HAL_TIMER_LoadCount_Get(TIMER_Index index)
{
    assert_param(IS_TIMER_INDEX(index));

    return LL_TIMER_LoadCount_Get(index);
}

/**
 * @brief Enable or disable the timer. When enabled, the counter starts counting down from the loadcounter to 0
 * @param  index: define which timer to operate.
 * @param  en: Enable or disable the timer.
 * @return This function has no return.
 */
void HAL_TIMER_Enable(TIMER_Index index, TIMER_Enable en)
{
    assert_param(IS_TIMER_INDEX(index));
    assert_param(IS_TIMER_ENABLE(en));

    LL_TIMER_Enable(index, en);
}


/**
 * @brief Get the interrupt status of specific timer.
 * @param  index: define which timer to operate.
 * @return return 1 means a timer interrupt is active, else inactive.
 */
uint8_t HAL_TIMER_Int_Status(TIMER_Index index)
{
    assert_param(IS_TIMER_INDEX(index));

    return LL_TIMER_IntStat_Get(index);
}


/**
 * @brief Clear the interrupt status of specific timer.
 * @param  index: define which timer to operate.
 * @return This function has no return.
 */
void HAL_TIMER_Int_Clr(TIMER_Index index)
{
    assert_param(IS_TIMER_INDEX(index));

    LL_TIMER_INT_Clear(index);
}


/**
 * @brief Get the current counter of specific timer.
 * @param  index: define which timer to operate.
 * @return It return the current count value.
 */
uint32_t HAL_TIMER_Current_Value(TIMER_Index index)
{
    assert_param(IS_TIMER_INDEX(index));

    return LL_TIMER_CurrentValue_Get(index);
}


/**
 * @brief Configure and enable the pwm function of timer. Timer should be initialize before using this function. For details please refer to examples.
          And iomux of pwm should be configed by user. Timer use pwm0-pwm3 in function io
 * @param  index: define which timer to operate.
 * @param  high_count: 24bit counter to define high period of pwm
 * @param  low_count: 24bit counter to define low period of pwm
 * @return This function has no return.
 */
void HAL_TIMER_PWM_Enable(TIMER_Index index, uint32_t high_count, uint32_t low_count)
{
    // TODO: HAL_TIMER_PWM_Enable(TIMER_Index index, uint8_t percent)
    assert_param(IS_TIMER_INDEX(index));

    LL_TIMER_LoadCount2_Set(index, high_count);
    LL_TIMER_LoadCount_Set(index, low_count);
    LL_TIMER_PWM_Enable(index, TIMER_PWM_ENABLE);

    LL_TIMER_Enable(index, TIMER_ENABLE_YES);
}


/**
 * @brief Disable the Timer PWM
 * @param  index: define which timer to operate.
 * @return This function has no return.
 */
void HAL_TIMER_PWM_Disable(TIMER_Index index)
{
    assert_param(IS_TIMER_INDEX(index));

    LL_TIMER_Enable(index, TIMER_DISABLE);

    LL_TIMER_LoadCount2_Set(index, 0);
    LL_TIMER_LoadCount_Set(index, 0);
    LL_TIMER_PWM_Enable(index, TIMER_PWM_DISABLE);
}


/**
 * @brief Get all timers interrupt status.
 *
 * @return uint8_t 4bit value, one bit for a timer interrupt status after masked operation.
 */
uint8_t HAL_TIMERS_IntStat_Get(void)
{
    return LL_TIMERS_IntStat_Get();
}


/**
 * @brief Clear all timers interrupt.
 *
 */
void HAL_TIMERS_INT_Clear(void)
{
    LL_TIMERS_INT_Clear();
}


/**
 * @brief Get the raw interrupt status of all timers.
 *
 * @return uint8_t
 */
uint8_t HAL_TIMERS_RawIntStat_Get(void)
{
    return LL_TIMERS_RawIntStat_Get();
}


/**
 * @brief Set loadcount2 for the specified timer.
 *
 * @param index
 * @param loadCount2
 */
void HAL_TIMER_LoadCount2_Set(TIMER_Index index, uint32_t loadCount2)
{
    assert_param(IS_TIMER_INDEX(index));

    LL_TIMER_LoadCount2_Set(index, loadCount2);
}


/**
 * @brief Get loadcount2 of the specified timer.
 *
 * @param index Timer index.
 * @return uint32_t loadcount2 value.
 */
uint32_t HAL_TIMER_LoadCount2_Get(TIMER_Index index)
{
    assert_param(IS_TIMER_INDEX(index));

    return LL_TIMER_LoadCount2_Get(index);
}

