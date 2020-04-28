#ifndef __HAL_TIMER_H_
#define __HAL_TIMER_H_

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/**
 * @brief There are totally 4 hardware timers.
 *
 */
typedef enum
{
    TIMER_1 = 1,
    TIMER_2,
    TIMER_3,
    TIMER_4
} TIMER_Index;


/**
 * @brief Mask the timer interrupt or not.
 *
 */
typedef enum
{
    TIMER_MASKED_NO = 0,
    TIMER_MASKED_YES
} TIMER_Mask;


/**
 * @brief Choose the running mode of timer.
          user_defined: Use this mode if you want a fixed, timed interrupt.
          free_running: Use this mode if you want a single timed interrupt.
 */
typedef enum
{
    TIMER_MODE_FREERUNNING = 0,     /**< TIMER_MODE_FREE_RUNNING: Use this mode if you want a single timed interrupt. */
    TIMER_MODE_USERDEFINED          /**< TIMER_MODE_USER_DEFINED: Use this mode if you want a fixed, timed interrupt. */
} TIMER_Mode;


/**
 * @brief Enable or Disable the timer
 */
typedef enum
{
    TIMER_DISABLE = 0,
    TIMER_ENABLE
} TIMER_Enable;


/**
 * @brief This is the struct to initialize the timer module.
 */
typedef struct
{
    TIMER_Index index;      /**< index: select which timer to use */
    TIMER_Mask mask;        /**< mask: mask or unmask the timer interrupt */
    TIMER_Mode mode;        /**< mode: select the running mode of timer, free-running or user define mode */
    uint32_t user_freq;     /**< user_freq: user-defined timer clock frequency fx, which must meet the request:  321.5KHz <= fx <= 80MHz */
} TIMER_InitTypeDef;


#define IS_TIMER_INDEX(TimerX)      ((TimerX) == TIMER_1 || (TimerX) == TIMER_2 || (TimerX) == TIMER_3 || (TimerX) == TIMER_4)
#define IS_TIMER_MASK(mask)         ((mask) == TIMER_MASKED_NO || (mask) == TIMER_MASKED_YES)
#define IS_TIMER_MODE(mode)         ((mode) == TIMER_MODE_USER_DEFINED || (mode) == TIMER_MODE_FREE_RUNNING)
#define IS_TIMER_ENABLE(enable)     ((enable) == TIMER_ENABLE || (enable) == TIMER_DISABLE)

///////////////////////////   function declaration  ///////////////////////////
/**
 * @brief Initialize the timer according to the input param config.
 * @param  config: the structure to initialize the timer, user_freq must meet the requierement: [312.5KHz, 80MHz]
 * @return This function has no return.
 */
void HAL_TIMER_Init(TIMER_InitTypeDef config);


/**
 * @brief Set the loadcounter of the timer
 * @param  index: define which timer to operate.
 * @param  count: the counter is 24bit.
 * @return This function has no return.
 */
void HAL_TIMER_LoadCount_Set(TIMER_Index index, uint32_t count);


/**
 * @brief Get timer loadcount.
 *
 * @param index timer index.
 * @return uint32_t loadcount of this timer.
 */
uint32_t HAL_TIMER_LoadCount_Get(TIMER_Index index);


/**
 * @brief Enable or disable the timer. When enabled, the counter starts counting down from the loadcounter to 0
 * @param  index: define which timer to operate.
 * @param  en: Enable or disable the timer.
 * @return This function has no return.
 */
void HAL_TIMER_Enable(TIMER_Index index, TIMER_Enable en);


/**
 * @brief Get the interrupt status of specific timer.
 * @param  index: define which timer to operate.
 * @return return 1 means a timer interrupt is active, else inactive.
 */
uint8_t HAL_TIMER_Int_Status(TIMER_Index index);


/**
 * @brief Clear the interrupt status of specific timer.
 * @param  index: define which timer to operate.
 * @return This function has no return.
 */
void HAL_TIMER_Int_Clr(TIMER_Index index);


/**
 * @brief Get the current counter of specific timer.
 * @param  index: define which timer to operate.
 * @return It return the current count value.
 */
uint32_t HAL_TIMER_Current_Value(TIMER_Index index);


/**
 * @brief Configure and enable the pwm function of timer. Timer should be initialize before using this function. For details please refer to examples.
          And iomux of pwm should be configed by user. Timer use pwm0-pwm3 in function io
 * @param  index: define which timer to operate.
 * @param  high_count: 24bit counter to define high period of pwm
 * @param  low_count: 24bit counter to define low period of pwm
 * @return This function has no return.
 */
void HAL_TIMER_PWM_Enable(TIMER_Index index, uint32_t high_count, uint32_t low_count);


/**
 * @brief Disable the Timer PWM
 * @param  index: define which timer to operate.
 * @return This function has no return.
 */
void HAL_TIMER_PWM_Disable(TIMER_Index index);


/**
 * @brief Get all timers interrupt status.
 *
 * @return uint8_t 4bit value, one bit for a timer interrupt status after masked operation.
 */
uint8_t HAL_TIMERS_IntStat_Get(void);


/**
 * @brief Clear all timers interrupt.
 *
 */
void HAL_TIMERS_INT_Clear(void);


/**
 * @brief Get the raw interrupt status of all timers.
 *
 * @return uint8_t
 */
uint8_t HAL_TIMERS_RawIntStat_Get(void);


/**
 * @brief Set loadcount2 for the specified timer.
 *
 * @param index
 * @param loadCount2
 */
void HAL_TIMER_LoadCount2_Set(TIMER_Index index, uint32_t loadCount2);


/**
 * @brief Get loadcount2 of the specified timer.
 *
 * @param index Timer index.
 * @return uint32_t loadcount2 value.
 */
uint32_t HAL_TIMER_LoadCount2_Get(TIMER_Index index);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HAL_TIMRE_H_ */
