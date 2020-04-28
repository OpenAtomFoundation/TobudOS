#ifndef _HAL_SYSCON_H_
#define _HAL_SYSCON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "syscon_types.h"

/**
 * @brief Real 32K period, can be used by SW to calculate PPM.
 *
 * @return uint16_t
 */
uint16_t HAL_SYSCON_Get32KPeriodNs(void);

/**
 * @brief Enable of cal 32K, enable of update calibratioin result.
 *
 */
void HAL_SYSCON_32KCaliEnable(void);

/**
 * @brief Set CPU sleep duration time, unit: ns.
 *
 * @param n_ns CPU sleep duration time, unit is ns.
 */
void HAL_SYSCON_CPUSleepDurationEnable(unsigned long long n_ns);

/**
 * @brief Disable CPU sleep.
 *
 */
void HAL_SYSCON_CPUSleepDurationDisable(void);

/**
 * @brief Get CPU real sleep time.
 *
 * @return uint64_t real sleep time, unit is ns.
 */
uint64_t HAL_SYSCON_RealSleepTime(void);

/**
 * @brief Get always on idle reg value.
 *
 * @return uint32_t awo idle reg value
 */
uint32_t HAL_SYSCON_IdleReg(void);


/**
 * @brief Calculate compensate time, unit is ns.
 *
 * @return uint32_t compensate time.
 */
uint32_t HAL_SYSCON_CalculateCompensateNs(void);

/**
 * @brief Indicate a true lock after debounce logic.
 *
 * @return true locked.
 * @return false not locked.
 */
bool HAL_SYSCON_IsSysPllLocked(void);

/**
 * @brief Set lock signal parity, debounce delay.
 *
 * @param lock_polarity 1 bit.
 * @param debounce_dly threshold to indicate a true lock.
 */
void HAL_SYSCON_SysPllDebounceSet(uint8_t lock_polarity, uint8_t debounce_dly);

/**
 * @brief Select flash mode or mirror mode.
 *
 * @param mode 1 -- flash mode; 0 -- mirror mode.
 */
void HAL_SYSCON_FlashOrMirrorMode(uint8_t mode);

/**
 * @brief Get boot mode.
 *
 * @return uint8_t 0 -- flash mode; 1 -- uart mode.
 */
uint8_t HAL_SYSCON_GetBootMode(void);

/**
 * @brief Reset Core, only five options is valid: all, phy, rtc, wic, pmu.
 *
 * @param src one option of pmu, wic, rtc, phy, all.
 */
void HAL_SYSCON_SoftwareResetCore(SYSCON_SwRst_Core src);

/**
 * @brief Select HCLK source to PLL.
 *
 */
void HAL_SYSCON_ClkSwitchToPll(void);

/**
 * @brief Select HCLK source to XTAL.
 *
 */
void HAL_SYSCON_ClkSwitchToXtal(void);

/**
 * @brief Set HCLK division.
 *
 * @param ahb_divider
 */
void HAL_SYSCON_SetHclkDivision(uint8_t ahb_divider);

/**
 * @brief Set PCLK0 division.
 *
 * @param apb0_divider
 */
void HAL_SYSCON_SetPclk0Division(uint8_t apb0_divider);

/**
 * @brief Set PCLK1 Division.
 *
 * @param apb1_divider
 */
void HAL_SYSCON_SetPclk1Division(uint8_t apb1_divider);

/**
 * @brief Clock gate enable.
 *
 * @param src core clock src.
 * @param ena 1 -- enable or 0 -- disable.
 */
void HAL_SYSCON_CoreClockEnable(SYSCON_ClkGate_Core src, bool ena);

/**
 * @brief Get core clock enable status, 1 bit for 1 core clock.
 *
 * @return uint32_t ORed value, 1 bit for 1 core clock, @see SYSCON_ClkGate_Core.
 */
uint32_t HAL_SYSCON_CoreClockStatus(void);

/**
 * @brief Peripheral clock enable.
 * Note: call this function one time to enable one clock.
 *
 * @param src peripheral module.
 * @param ena enable / disable.
 */
void HAL_SYSCON_PeripheralClockEnable(SYSCON_ClkGate_Peripheral src, bool ena);

/**
 * @brief Get peripheral clock enable status.
 *
 * @return uint32_t ORed value, @see SYSCON_ClkGate_Peripheral.
 */
uint32_t HAL_SYSCON_PeripheralClockStatus(void);

/**
 * @brief Reset peripheral module.
 * Note: call this function one time to reset one peripheral module.
 *
 * @param peri @see SYSCON_SwRst_Peripheral.
 */
void HAL_SYSCON_SoftwareResetPeripheral(SYSCON_SwRst_Peripheral peri);

/**
 * @brief Set IO function.
 *
 * @param af_type function type, @see GPIO_AltFunctionType.
 * @param af_io_idx IO index, @see GPIO_AltFunctionIoIndex.
 * @param en enable / disable.
 */
void HAL_SYSCON_FuncIOSet(GPIO_AltFunctionType af_type, GPIO_AltFunctionIoIndex af_io_idx, uint8_t en);

/**
 * @brief Disable all function on all configurable IO pad.
 *
 */
void HAL_SYSCON_FuncIODisableAll(void);


/**
 * @brief GPIO Pull Up
 *
 */
void HAL_SYSCON_GPIO_PullUp(GPIO_Num gpio_num);

/**
 * @brief GPIO Pull Down
 *
 */
void HAL_SYSCON_GPIO_PullDown(GPIO_Num gpio_num);


/**
 * @brief Select SPIS as IO pad.
 *
 * @param en 1 -- enable; 0 -- disable.
 */
void HAL_SYSCON_SPIS_Enable(uint8_t en);

/**
 * @brief Select spim and csnX IO as pad.
 *
 * @param index @see SYSCON_SPIM_Index.
 */
void HAL_SYSCON_SPIMEnable(SYSCON_SPIM_Index index);

/**
 * @brief Deselect SPIM and csnX IO as pad.
 *
 * @param index @see SYSCON_SPIM_Index
 */
void HAL_SYSCON_SPIMDisable(SYSCON_SPIM_Index index);

/**
 * @brief Disable all SPIM.
 *
 */
void HAL_SYSCON_SPIMDisableAll(void);

/**
 * @brief Select SDIO IO as pad.
 *
 */
void HAL_SYSCON_SDIOEnable(void);

/**
 * @brief Deselect SDIO IO as pad.
 *
 */
void HAL_SYSCON_SDIODisable(void);

/**
 * @brief Select/Deselect SPIFlash as pad.
 *
 * @param enable
 */
void HAL_SYSCON_SPIFlashEnable(uint8_t enable);

/**
 * @brief Select I2S chnX IO as pad.
 *
 * @param index @see SYSCON_I2S_Index.
 */
void HAL_SYSCON_I2SEnable(SYSCON_I2S_Index index);

/**
 * @brief Disable all I2S module.
 *
 */
void HAL_SYSCON_I2SDisable(void);

/**
 * @brief SWD enable.
 *
 */
void HAL_SYSCON_SWDEnable(void);

/**
 * @brief SWD disable.
 *
 */
void HAL_SYSCON_SWDDisable(void);

/**
 * @brief DBGH enable.
 *
 */
void HAL_SYSCON_DBGHEnable(void);

/**
 * @brief DBHG disable.
 *
 */
void HAL_SYSCON_DBGHDisable(void);


/**
 * @brief HCLK source select.
 *
 * @param src @see SYSTEM_CLOCK_SRC.
 */
void HAL_SYSCON_SelectSysClkSrc(SYSTEM_CLOCK_SRC src);

/**
 * @brief Set CPU reset request mask.
 *
 * @param mask 1 -- mask, 0 -- not masked.
 */
void HAL_SYSCON_CPUResetReqMask(uint8_t mask);

/**
 * @brief PHY must be reset after TxImgCal.
 */

void HAL_SysconPhyReset(void);
/**
 * @brief  Reset the mcu by software
 *
 * @param  none
 *
 * @note   use the 3.5 version of the firmware library. 
 */
void HAL_SYSCON_SoftReset(void);


void HAL_SYSCON_EXT_INTR_Enable(SYSTEM_EXT_INT_Wakeup_Index ext_int_idx, bool enable);
void HAL_SYSCON_EXT_INTR_Set_Triggle_Condition(SYSTEM_EXT_INT_Wakeup_Index ext_int_idx, SYSTEM_EXT_INT_Triggle_Type triggle);
uint8_t HAL_SYSCON_EXT_INTR_Stat(void);
uint8_t HAL_SYSCON_EXT_INTR_Stat_Raw(void);
void HAL_SYSCON_EXT_INTR_Clear(void);
void HAL_SYSCON_RCO32K_Bitsel_Set(uint8_t bitsel);
void HAL_SYSCON_RCO32K_Cbit_Set(uint8_t cbit);
void HAL_SYSCON_Xtal40MCap_Set(uint8_t cap);
void HAL_SYSCON_PmuCfg(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_SYSCON_H_ */
