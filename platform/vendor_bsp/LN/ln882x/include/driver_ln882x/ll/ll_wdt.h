#ifndef __LL_WDT_H_
#define __LL_WDT_H_
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

/******************************  Data Type and Macros  **********************/
#define WDT_RESETMODE_DIRECT              0
#define WDT_RESETMODE_IRQ                 1

#define WDT_RESETPULSE_2PCLK_CYCLES       0x00
#define WDT_RESETPULSE_4PCLK_CYCLES       0x01
#define WDT_RESETPULSE_8PCLK_CYCLES       0x02
#define WDT_RESETPULSE_16PCLK_CYCLES      0x03
#define WDT_RESETPULSE_32PCLK_CYCLES      0x04
#define WDT_RESETPULSE_64PCLK_CYCLES      0x05
#define WDT_RESETPULSE_128PCLK_CYCLES     0x06
#define WDT_RESETPULSE_256PCLK_CYCLES     0x07


#define WDT_TIMEOUT_PERIOD_FF             0x00
#define WDT_TIMEOUT_PERIOD_1FF            0x01
#define WDT_TIMEOUT_PERIOD_3FF            0x02
#define WDT_TIMEOUT_PERIOD_7FF            0x03
#define WDT_TIMEOUT_PERIOD_1FFF           0x05
#define WDT_TIMEOUT_PERIOD_3FFF           0x06
#define WDT_TIMEOUT_PERIOD_7FFF           0x07
#define WDT_TIMEOUT_PERIOD_FFFF           0x08
#define WDT_TIMEOUT_PERIOD_1FFFF          0x09
#define WDT_TIMEOUT_PERIOD_3FFFF          0x0A
#define WDT_TIMEOUT_PERIOD_7FFFF          0x0B
#define WDT_TIMEOUT_PERIOD_FFFFF          0x0C
#define WDT_TIMEOUT_PERIOD_1FFFFF         0x0D
#define WDT_TIMEOUT_PERIOD_3FFFFF         0x0E
#define WDT_TIMEOUT_PERIOD_7FFFFF         0x0F


/******************************  Function Declaration  **********************/
/**
 * @brief Enable the watch dog. Once enabled it cannot be disabled.
 * @return This function has no return value.
 */
void LL_WDT_Enable(void);

/**
 * @brief Set the watch dog timeout period
 * @param  period: the period to trigger an wdt interrupt. For example WDT_PERIOD_3FF means the period wdt int trigger is 0x3ff cycles
 * @return This function has no return value.
 */
void LL_WDT_TimeoutPeriod(uint8_t period);

/**
 * @brief Clears the watchdog interrupt. This can be used to clear the interrupt without restarting the watchdog counter.
 * @return return 1 when an interrupt is triggered.
 */
uint8_t LL_WDT_ClrIRQ(void);

/**
 * @brief Get the current value of the internal counter
 * @return return the current value of the internal counter.
 */
uint16_t LL_WDT_CurrentCounter(void);

/**
 * @brief To restart the WDT counter (and clear WDT interrupt.)
 * @return This function has no return value.
 */
void LL_WDT_Restart(void);

/**
 * @brief To get the interrupt status of the WDT.
 * @return return the interrupt status. 0 means interrupt is inactive, 1 means interrupt is active.
 */
uint8_t LL_WDT_IntStatus(void);

/**
 * @brief Reset mode.
 * 
 * @param mode 
 */
void LL_WDT_ResetModeSet(uint8_t mode);

/**
 * @brief Reset pulse length.
 * 
 * @param pulse_width 
 */
void LL_WDT_ResetPulseWidthSet(uint8_t pulse_width);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* __LL_WDT_H_ */

