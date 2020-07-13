#ifndef __LL_RTC_H_
#define __LL_RTC_H_

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

//////////////////////////////  Data type and Macros  /////////////////////////
#define LL_RTC_WRAP_DISABLE         0
#define LL_RTC_WRAP_ENABLE          1

#define LL_RTC_INT_UNMASK           0
#define LL_RTC_INT_MASK             1

#define LL_RTC_INT_DISABLE          0
#define LL_RTC_INT_ENABLE           1

#define LL_RTC_INT_STATUS_INACTIVE  0
#define LL_RTC_INT_STATUS_ACTIVE    1

#define LL_RTC_DISABLE              0
#define LL_RTC_ENABLE               1


//////////////////////////////  Function Declaration  /////////////////////////

/**
 * @brief  Get the current value of the internal counter.
 * @return return the interal counter of rtc.
 */
uint32_t LL_RTC_GetCurValue(void);

/**
 * @brief  Set the match counter of rtc.
 * @param  matchCounter: the match counter of rtc. 
 *         The rtc counter always count forward. when the load counter reach the match counter an interrupt will trigger.
 * @return This function has no return value.
 */
void LL_RTC_SetMatchCounter(uint32_t matchCounter);
uint32_t LL_RTC_GetMatchCounter(void);

/**
 * @brief  Set the load counter of rtc.
 * @param  loadCounter: the load counter of rtc. It is the value where rtc start counting forward
 * @return This function has no return value.
 */
void LL_RTC_SetLoadCounter(uint32_t loadCounter);

/**
 * @brief  To force the counter to wrap when a match occurs
 * @param  en: 1 to enable a wrap, 0 to disable a wrap
 * @return This function has no return value.
 */
void LL_RTC_Wrap(uint8_t en);

/**
 * @brief  Enable the RTC counter.
 * @param  en: 1 to enable the counter, 0 to disable the counter
 * @return This function has no return value.
 */
void LL_RTC_Enable(uint8_t en);

/**
 * @brief  Set whether to mask the rtc interrupt or not
 * @param  en: 1 to mask the interrupt, 0 to unmask the interrupt
 * @return This function has no return value.
 */
void LL_RTC_IntMask(uint8_t en);

/**
 * @brief  Set whether to enable the rtc interrupt or not
 * @param  en: 1 to enable the interrupt, 0 to disable the interrupt
 * @return This function has no return value.
 */
void LL_RTC_IntEnable(uint8_t en);

/**
 * @brief  Get the status of rtc interrupt, after the mask
 * @return The interrupt status after the int mask. 1 means interrupt is active, 0 means interrupt is inactive.
 */
uint8_t LL_RTC_IntStatus(void);

/**
 * @brief  Get the raw status of rtc interrupt, before the mask
 * @return The interrupt status before the int mask. 1 means interrupt is active, 0 means interrupt is inactive.
 */
uint8_t LL_RTC_IntRawStatus(void);

/**
 * @brief  Clear rtc interrupt
 * @return This function has no return value.
 */
void LL_RTC_ClearInt(void);




#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __LL_RTC_H_
