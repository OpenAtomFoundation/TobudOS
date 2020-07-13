#ifndef __HAL_RTC_H_
#define __HAL_RTC_H_
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

//////////////////////////////  Data type and Macros  /////////////////////////

/**
 * @brief Set whether the counter should wrap when a match occurs instead of waiting until the maximum count is reached.
 */
typedef enum 
{
    RTC_WRAP_DISABLE = 0,
    RTC_WRAP_ENABLE,
} RTC_WrapEn;

/**
 * @brief Set the int mask of rtc module.
 */
typedef enum 
{
    RTC_INT_UNMASK = 0,
    RTC_INT_MASK,
} RTC_IntMask;

/**
 * @brief Enable or disable the interrupt of rtc.
 */
typedef enum 
{
    RTC_INT_DISABLE = 0,
    RTC_INT_ENABLE,
} RTC_IntEn;


typedef enum 
{
    RTC_INT_STATUS_INACTIVE = 0,
    RTC_INT_STATUS_ACTIVE
} RTC_IntStatus;

/**
 * @brief Enable or disable counter of rtc.
 */
typedef enum 
{
    RTC_DISABLE = 0,
    RTC_ENABLE
} RTC_Enable;

/**
 * @brief Struct to define parameter of RTC. 
 */
typedef struct 
{
    uint32_t counter_match;     ///**< counter_match: 32bit. When RTC start counting from counter_load to counter_match a interrupt will occur if interrupt is enabled. */
    uint32_t counter_load;      ///**< counter_match: 32bit. Set from where the rtc counter start counting from */
    RTC_WrapEn wrap_en;         ///**< wrap_en: Set whether the counter should wrap when a match occurs instead of waiting until the maximum count is reached. */
    RTC_IntMask int_mask;       ///**< int_mask: mask the interrupt or not */
    RTC_IntEn int_en;           ///**< int_en: enable the interrupt or not */
} RTC_InitTypeDef;


//////////////////////////////  Function Declaration  /////////////////////////
/**
 * @brief  Get the current value of the internal counter.
 * @return return the interal counter of rtc.
 */
uint32_t HAL_RTC_GetCurValue(void);

/**
 * @brief  Set the match counter of rtc.
 * @param  matchCounter: the match counter of rtc. 
 *         The rtc counter always count forward. when the load counter reach the match counter an interrupt will trigger.
 * @return This function has no return value.
 */
void HAL_RTC_SetMatchCounter(uint32_t matchCounter);

/**
 * @brief  Set the load counter of rtc.
 * @param  loadCounter: the load counter of rtc. It is the value where rtc start counting forward
 * @return This function has no return value.
 */
void HAL_RTC_SetLoadCounter(uint32_t loadCounter);

/**
 * @brief  To force the counter to wrap when a match occurs
 * @param  en: 1 to enable a wrap, 0 to disable a wrap
 * @return This function has no return value.
 */
void HAL_RTC_Wrap(RTC_WrapEn wrapEn);

/**
 * @brief  Enable the RTC counter.
 * @param  en: 1 to enable the counter, 0 to disable the counter
 * @return This function has no return value.
 */
void HAL_RTC_Enable(RTC_Enable enable);

/**
 * @brief  Set whether to mask the rtc interrupt or not
 * @param  en: 1 to mask the interrupt, 0 to unmask the interrupt
 * @return This function has no return value.
 */
void HAL_RTC_IntMask(RTC_IntMask mask);

/**
 * @brief  Set whether to enable the rtc interrupt or not
 * @param  en: 1 to enable the interrupt, 0 to disable the interrupt
 * @return This function has no return value.
 */
void HAL_RTC_IntEnable(RTC_IntEn enable);

/**
 * @brief  Get the status of rtc interrupt, after the mask
 * @return The interrupt status after the int mask. 1 means interrupt is active, 0 means interrupt is inactive.
 */
RTC_IntStatus HAL_RTC_IntStatus(void);

/**
 * @brief  Get the raw status of rtc interrupt, before the mask
 * @return The interrupt status before the int mask. 1 means interrupt is active, 0 means interrupt is inactive.
 */
RTC_IntStatus HAL_RTC_IntRawStatus(void);

/**
 * @brief  Clear rtc interrupt
 * @return This function has no return value.
 */
void HAL_RTC_ClearInt(void);

/**
 * @brief Initialize the RTC
 * @param config: Struct to define parameter of RTC. 
 * @return This function has no return value.
 */
void HAL_RTC_Init(RTC_InitTypeDef config);
uint32_t HAL_RTC_GetMatchCounter(void);

#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // __HAL_RTC_H_
