#ifndef __LL_TIMER_H_
#define __LL_TIMER_H_

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#include "types.h"

/**
 * @brief Timer Index.
 * 
 */
#define TIMER1  1
#define TIMER2  2
#define TIMER3  3
#define TIMER4  4



#define TIMER_ENABLE_BIT                _BIT(0)
#define TIMER_MODE_BIT                  _BIT(1)
#define TIMER_INTERRUPT_MASK_BIT        _BIT(2)
#define TIMER_PWM_BIT                   _BIT(3)

#define TIMER_ENABLE_YES                1
#define TIMER_ENABLE_NO                 0

// user-defined means "auto-reload"
#define TIMER_MODE_USER_DEFINED         1
// free-running means "single-shot"
#define TIMER_MODE_FREE_RUNNING         0

#define TIMER_INTERRUPT_MASK_YES        1
#define TIMER_INTERRUPT_MASK_NO         0

// FIXME: no description for Timer1PWM in the regmap?
#define TIMER_PWM_ENABLE                1
#define TIMER_PWM_DISABLE               0



/***********************  function declaration  *****************************/
/**
 * @brief Get timer loadcount.
 * 
 * @param index timer index.
 * @return uint32_t loadcount value.
 */
uint32_t LL_TIMER_LoadCount_Get(uint8_t index);


/**
 * @brief Set timer loadcount.
 * 
 * @param index timer index.
 * @param loadcount loadcount value to be set.
 */
void LL_TIMER_LoadCount_Set(uint8_t index, uint32_t loadcount);


/**
 * @brief Get timer currentvlaue.
 * 
 * @param index timer index.
 * @return uint32_t currentvalue.
 */
uint32_t LL_TIMER_CurrentValue_Get(uint8_t index);


/**
 * @brief Get timer control register value.
 * 
 * @param index timer index.
 * @return uint32_t
 */
uint32_t LL_TIMER_Control_Get(uint8_t index);


/**
 * @brief Set timer control register.
 * 
 * @param index timer index.
 * @param value 
 */
void LL_TIMER_Control_Set(uint8_t index, uint8_t value);


/**
 * @brief Enable/Disable timer.
 * 
 * @param index timer index.
 * @param enable 
 */
void LL_TIMER_Enable(uint8_t index, bool enable);


/**
 * @brief Set timer mode, user-defined mode means "auto-reload", and free-running
 *  mode means "singl-shot".
 * 
 * @param index timer index.
 * @param mode 
 */
void LL_TIMER_Mode_Set(uint8_t index, uint8_t mode);


/**
 * @brief Set timer interrupt mask.
 * 
 * @param index timer index.
 * @param mask 
 */
void LL_TIMER_IntMask_Set(uint8_t index, uint8_t mask);


/**
 * @brief Enable/Disable PWM function.
 * 
 * @param index
 * @param enable 
 */
void LL_TIMER_PWM_Enable(uint8_t index, uint8_t enable);


/**
 * @brief Check whether timer is enabled or disabled.
 * 
 * @param index 
 * @return uint8_t 
 */
uint8_t LL_TIMER_EnableStat_Get(uint8_t index);


/**
 * @brief Check timer mode.
 * 
 * @param index 
 * @return uint8_t 
 */
uint8_t LL_TIMER_Mode_Get(uint8_t index);


/**
 * @brief Check timer interrupt mask.
 * 
 * @param index 
 * @return uint8_t 
 */
uint8_t LL_TIMER_IntMask_Get(uint8_t index);


/**
 * @brief Check whether timer enable PWM function or not.
 * 
 * @param index 
 * @return uint8_t 
 */
uint8_t LL_TIMER_PWMStat_Get(uint8_t index);


/**
 * @brief Clear timer interrupt.
 * 
 * @param index 
 * @return uint8_t 
 */
uint8_t LL_TIMER_INT_Clear(uint8_t index);


/**
 * @brief Get timer interrupt status.
 * 
 * @param index 
 * @return uint8_t 
 */
uint8_t LL_TIMER_IntStat_Get(uint8_t index);


/**
 * @brief Get all timer interrupt status, one bit for one timer.
 * 
 * @return uint8_t 
 */
uint8_t LL_TIMERS_IntStat_Get( void );


/**
 * @brief Clear all timer interrupt.
 * 
 * @return uint8_t 
 */
void LL_TIMERS_INT_Clear( void );


/**
 * @brief Get the raw interrupt status of all timers.
 * 
 * @return uint8_t 
 */
uint8_t LL_TIMERS_RawIntStat_Get( void );


/**
 * @brief Set loadcount2 for the specified timer.
 * 
 * @param index timer index.
 * @param loadcount2
 */
void LL_TIMER_LoadCount2_Set(uint8_t index, uint32_t loadcount2);


/**
 * @brief Get loadcount2 of the specified timer.
 * 
 * @param index timer index.
 * @return uint32_t 
 */
uint32_t LL_TIMER_LoadCount2_Get(uint8_t index);



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __LL_TIMER_H_
