#ifndef __HAL_WDT_H_
#define __HAL_WDT_H_

#ifdef __cplusplus
    extern "C" {
#endif // __cpluscplus

#include "types.h"

/******************************  Data Type and Macros  **********************/
/**
 * @brief Watch dog has two reset mode.
 */
typedef enum wdt_resetmode
{
    WDT_RESET_DIRECT = 0,           ///**< WDT_RESET_DIRECT: watch dog will directly reset the mcu as soon as watch dog triggerred */
    WDT_RESET_IRQ = 1               ///**< WDT_RESET_IRQ: watch dog will first trigger an interrupt. if we don't reset watch dog after another period of time, it will reset mcu */
} WDT_ResetMode;

/**
 * @brief Define the pulse width to reset the mcu
 */
typedef enum wdt_resetpulse
{
    WDT_RESET_PULSE_2_PCLK = 0,
    WDT_RESET_PULSE_4_PCLK,
    WDT_RESET_PULSE_8_PCLK,
    WDT_RESET_PULSE_16_PCLK,
    WDT_RESET_PULSE_32_PCLK,
    WDT_RESET_PULSE_64_PCLK,
    WDT_RESET_PULSE_128_PCLK,
    WDT_RESET_PULSE_256_PCLK,
    WDT_RESET_PULSE_MAX = 0x8
} WDT_ResetPulse;

/**
 * @brief Define the how many pclk to wait before wdt int trigger. For example WDT_PERIOD_3FF means the period wdt int trigger is 0x3ff cycles
 */
typedef enum wdt_timeoutperiod
{
    WDT_PERIOD_FF       = 0,    // 8ms @40MHz
    WDT_PERIOD_1FF      = 1,    // 16ms @40MHz
    WDT_PERIOD_3FF      = 2,    // 32ms @40MHz
    WDT_PERIOD_7FF      = 3,    // 64ms @40MHz
    WDT_PERIOD_FFF      = 4,    // 128ms @40MHz
    WDT_PERIOD_1FFF     = 5,    // 256ms @40MHz
    WDT_PERIOD_3FFF     = 6,    // 512ms @40MHz
    WDT_PERIOD_7FFF     = 7,    // 1024ms @40MHz
    WDT_PERIOD_FFFF     = 8,    // 2048ms @40MHz
    WDT_PERIOD_1FFFF    = 9,    // 4096ms @40MHz
    WDT_PERIOD_3FFFF    = 10,   // 8192ms @40MHz
    WDT_PERIOD_7FFFF    = 11,   // 16384ms @40MHz
    WDT_PERIOD_FFFFF    = 12,   // 32768ms @40MHz
    WDT_PERIOD_1FFFFF   = 13,   // 65536ms @40MHz
    WDT_PERIOD_3FFFFF   = 14,   // 131072ms @40MHz
    WDT_PERIOD_7FFFFF   = 15    // 262144ms @40MHz
} WDT_TimeoutPeriod;


/**
 * @brief Struct to define parameter of watch dog. 
 */
typedef struct wdt_inittypedef
{
		WDT_ResetMode   mode;
		WDT_ResetPulse  pulse_width;
		WDT_TimeoutPeriod period;
} WDT_InitTypeDef;


#define IS_WDT_RESET_MODE(mode)         ( (mode) == WDT_RESET_DIRECT || (mode) == WDT_RESET_IRQ )
#define IS_WDT_RESET_PULSE(pulse)       ( (pulse) == WDT_RESET_PULSE_2_PCLK      || \
                                          (pulse) == WDT_RESET_PULSE_4_PCLK      || \
                                          (pulse) == WDT_RESET_PULSE_8_PCLK      || \
                                          (pulse) == WDT_RESET_PULSE_16_PCLK     || \
                                          (pulse) == WDT_RESET_PULSE_32_PCLK     || \
                                          (pulse) == WDT_RESET_PULSE_64_PCLK     || \
                                          (pulse) == WDT_RESET_PULSE_128_PCLK    || \
                                          (pulse) == WDT_RESET_PULSE_256_PCLK    || \
                                          (pulse) == WDT_RESET_PULSE_MAX )

#define IS_WDT_TIMEOUT_PERIOD(period)   ( (period) == WDT_PERIOD_FF     || \
                                          (period) == WDT_PERIOD_1FF    || \
                                          (period) == WDT_PERIOD_3FF    || \
                                          (period) == WDT_PERIOD_7FF    || \
                                          (period) == WDT_PERIOD_FFF    || \
                                          (period) == WDT_PERIOD_1FFF   || \
                                          (period) == WDT_PERIOD_3FFF   || \
                                          (period) == WDT_PERIOD_7FFF   || \
                                          (period) == WDT_PERIOD_FFFF   || \
                                          (period) == WDT_PERIOD_1FFFF  || \
                                          (period) == WDT_PERIOD_3FFFF  || \
                                          (period) == WDT_PERIOD_7FFFF  || \
                                          (period) == WDT_PERIOD_FFFFF  || \
                                          (period) == WDT_PERIOD_1FFFFF || \
                                          (period) == WDT_PERIOD_3FFFFF || \
                                          (period) == WDT_PERIOD_7FFFFF )


/******************************  Function Declaration  **********************/
/**
 * @brief Enable the watch dog. Once enabled it cannot be disabled.
 * @return This function has no return value.
 */
void HAL_WDT_Enable(void);

/**
 * @brief Set the watch dog timeout period
 * @param  period: the period to trigger an wdt interrupt. For example WDT_PERIOD_3FF means the period wdt int trigger is 0x3ff cycles
 * @return This function has no return value.
 */
void HAL_WDT_SetTimeoutPeriod(WDT_TimeoutPeriod period);

/**
 * @brief Clears the watchdog interrupt. This can be used to clear the interrupt without restarting the watchdog counter.
 * @return return 1 when an interrupt is triggered.
 */
uint8_t HAL_WDT_ClrIRQ(void);

/**
 * @brief Get the current value of the internal counter
 * @return return the current value of the internal counter.
 */
uint16_t HAL_WDT_CurrentCounter(void);

/**
 * @brief To restart the WDT counter (and clear WDT interrupt.)
 * @return This function has no return value.
 */
void HAL_WDT_Restart(void);

/**
 * @brief To get the interrupt status of the WDT.
 * @return return the interrupt status. 0 means interrupt is inactive, 1 means interrupt is active.
 */
uint8_t HAL_WDT_IntStatus(void);

/**
 * @brief Initialize the watch dog
 * @param config: Struct to define parameter of watch dog. 
 * @return This function has no return value.
 */
void HAL_WDT_Init(WDT_InitTypeDef config);


#ifdef __cplusplus
}
#endif // __cpluscplus
#endif /* __HAL_WDT_H_ */
