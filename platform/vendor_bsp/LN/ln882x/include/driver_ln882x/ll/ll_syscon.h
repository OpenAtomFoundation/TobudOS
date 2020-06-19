#ifndef __LL_SYSCON_H_
#define __LL_SYSCON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "hal/syscon_types.h"

/**
 * @brief Real 32K period, can be used by SW to calculate PPM.
 *
 * @return uint16_t
 */
uint16_t LL_SYSCON_Get32KPeriodNs(void);

/**
 * @brief Enable of cal 32K, enable of update calibratioin result.
 *
 */
void LL_SYSCON_32KCaliEnable(void);

/**
 * @brief Set CPU sleep duration time, unit: ns.
 *
 * @param n_ns CPU sleep duration time, unit is ns.
 */
void LL_SYSCON_CPUSleepDurationEnable(unsigned long long n_ns);

/**
 * @brief Disable CPU sleep.
 *
 */
void LL_SYSCON_CPUSleepDurationDisable(void);

/**
 * @brief Get CPU real sleep time.
 *
 * @return uint64_t real sleep time, unit is ns.
 */
uint64_t LL_SYSCON_RealSleepTime(void);

/**
 * @brief Get always on idle reg value.
 *
 * @return uint32_t awo idle reg value
 */
uint32_t LL_SYSCON_IdleReg(void);

/**
 * @brief Calculate compensate time, unit is ns.
 *
 * @return uint32_t compensate time.
 */
uint32_t LL_SYSCON_CalculateCompensateNs(void);

/**
 * @brief Indicate a true lock after debounce logic.
 *
 * @return true locked.
 * @return false not locked.
 */
bool LL_SYSCON_IsSysPllLocked(void);

/**
 * @brief Set lock signal parity, debounce delay.
 *
 * @param lock_polarity 1 bit.
 * @param debounce_dly threshold to indicate a true lock.
 */
void LL_SYSCON_SysPllDebounceSet(uint8_t lock_polarity, uint8_t debounce_dly);

/**
 * @brief Select flash mode or mirror mode.
 *
 * @param mode 1 -- flash mode; 0 -- mirror mode.
 */
void LL_SYSCON_FlashOrMirrorMode(uint8_t mode);

/**
 * @brief Get boot mode.
 *
 * @return uint8_t  bootmode bit[0:2]
 */
uint8_t LL_SYSCON_GetBootMode(void);

/**
 * @brief Reset Core, only five options is valid: all, phy, rtc, wic, pmu.
 *
 * @param src one option of pmu, wic, rtc, phy, all.
 */
void LL_SYSCON_SoftwareResetCore(SYSCON_SwRst_Core src);

/**
 * @brief Select HCLK source to PLL.
 *
 */
void LL_SYSCON_ClkSwitchToPll(void);

/**
 * @brief Select HCLK source to XTAL.
 *
 */
void LL_SYSCON_ClkSwitchToXtal(void);

/**
 * @brief HCLK source select.
 *
 * @param src @see SYSTEM_CLOCK_SRC.
 */
void  LL_SYSCON_SelectSysClkSrc(SYSTEM_CLOCK_SRC src);

/**
 * @brief Set HCLK division.
 *
 * @param ahb_divider
 */
void LL_SYSCON_SetHclkDivision(uint8_t ahb_divider);

/**
 * @brief Set PCLK0 division.
 *
 * @param apb0_divider
 */
void LL_SYSCON_SetPclk0Division(uint8_t apb0_divider);

/**
 * @brief Set PCLK1 Division.
 *
 * @param apb1_divider
 */
void LL_SYSCON_SetPclk1Division(uint8_t apb1_divider);


/**
 * @brief Set hardware timer division.
 *
 * @param div clk pre-division, which is 8bits width.
 */
void LL_SYSCON_Timer1Division(uint8_t div);
void LL_SYSCON_Timer2Division(uint8_t div);
void LL_SYSCON_Timer3Division(uint8_t div);
void LL_SYSCON_Timer4Division(uint8_t div);

/**
 * @brief Clock gate enable.
 *
 * @param src core clock src.
 * @param ena 1 -- enable or 0 -- disable.
 */
void LL_SYSCON_CoreClockEnable(SYSCON_ClkGate_Core src, bool ena);

/**
 * @brief Get core clock enable status, 1 bit for 1 core clock.
 *
 * @return uint32_t ORed value, 1 bit for 1 core clock, @see SYSCON_ClkGate_Core.
 */
uint32_t LL_SYSCON_CoreClockStatus(void);

/**
 * @brief Peripheral clock enable.
 * Note: call this function one time to enable one clock.
 *
 * @param src peripheral module.
 * @param ena enable / disable.
 */
void LL_SYSCON_PeripheralClockEnable(SYSCON_ClkGate_Peripheral src, bool ena);

/**
 * @brief Get peripheral clock enable status.
 *
 * @return uint32_t ORed value, @see SYSCON_ClkGate_Peripheral.
 */
uint32_t LL_SYSCON_PeripheralClockStatus(void);

/**
 * @brief Reset peripheral module.
 * Note: call this function one time to reset one peripheral module.
 *
 * @param peri @see SYSCON_SwRst_Peripheral.
 */
void LL_SYSCON_SoftwareResetPeripheral(SYSCON_SwRst_Peripheral peri);

/**
 * @brief Set IO function.
 *
 * @param af_type alternate function type, @see GPIO_AltFunctionType.
 * @param af_io_index IO index, @see GPIO_AltFunctionIoIndex.
 * @param en enable / disable.
 */
void LL_SYSCON_FuncIOSet(GPIO_AltFunctionType af_type, GPIO_AltFunctionIoIndex af_io_index, uint8_t en);

/**
 * @brief Disable all function on all configurable IO pad.
 *
 */
void LL_SYSCON_FuncIODisableAll(void);

/**
 * @brief GPIO Pull Up
 *
 */
void LL_SYSCON_GPIO_PullUp(GPIO_Num gpio_num);

/**
 * @brief GPIO Pull Down
 *
 */
void LL_SYSCON_GPIO_PullDown(GPIO_Num gpio_num);


/**
 * @brief Select SPIS IO as pad.
 *
 * @param en 1 -- enable; 0 -- disable.
 */
void LL_SYSCON_SPIS_Enable(uint8_t en);

/**
 * @brief Select spim and csnX IO as pad.
 *
 * @param index @see SYSCON_SPIM_Index.
 */
void LL_SYSCON_SPIMEnable(SYSCON_SPIM_Index index);

/**
 * @brief Deselect SPIM and csnX IO as pad.
 *
 * @param index @see SYSCON_SPIM_Index
 */
void LL_SYSCON_SPIMDisable(SYSCON_SPIM_Index index);

/**
 * @brief Disable all SPIM.
 *
 */
void LL_SYSCON_SPIMDisableAll(void);


void LL_SYSCON_DigSel(uint16_t bitmap);

/**
 * @brief Select SDIO IO as pad.
 *
 */
void LL_SYSCON_SDIOEnable(void);

/**
 * @brief Deselect SDIO IO as pad.
 *
 */
void LL_SYSCON_SDIODisable(void);

/**
 * @brief Select SPIFlash IO as pad.
 */
void LL_SYSCON_SPIFlashEnable(void);

/**
 * @brief Deselect SPIFlash IO as pad.
 */
void LL_SYSCON_SPIFlashDisable(void);

/**
 * @brief Select I2S chnX IO as pad.
 *
 * @param index @see SYSCON_I2S_Index.
 */
void LL_SYSCON_I2SEnable(SYSCON_I2S_Index index);

/**
 * @brief Disable all I2S module.
 *
 */
void LL_SYSCON_I2SDisableAll(void);

/**
 * @brief SWD enable.
 *
 */
void LL_SYSCON_SWDEnable(void);

/**
 * @brief SWD disable.
 *
 */
void LL_SYSCON_SWDDisable(void);

/**
 * @brief DBGH enable, set GPIO20, GPIO19 to debug host.
 */
void LL_SYSCON_DBGHEnable(void);

/**
 * @brief DBHG disable, set GPIO20, GPIO19 to GPIO.
 */
void LL_SYSCON_DBGHDisable(void);


/**
 * @brief Set CPU reset request mask.
 *
 * @param mask 1 -- mask, 0 -- not masked.
 */
void LL_SYSCON_CPUResetReqMask(uint8_t mask);

/**
 * @brief PHY must be reset after TxImgCal.
 */
void LL_SYSCON_PHY_Reset(void);


void LL_SYSCON_RCO32K_Bitsel_Set(uint8_t bitsel);

uint8_t LL_SYSCON_RCO32K_Bitsel_Get(void);

void LL_SYSCON_RCO32K_Cbit_Set(uint8_t cbit);

void LL_SYSCON_PMUReg_Set(uint32_t value);

// FIXME: add to HAL layer.
void LL_SYSCON_Xtal40MCap_Set(uint8_t cap);

void LL_SYSCON_PMUCfg_Set(uint32_t value);

void LL_SYSCON_PMUAvdd_Set(uint32_t value);

void LL_SYSCON_Misc_Set(uint32_t value);
void LL_SYSCON_EXT_INTR_Enable(SYSTEM_EXT_INT_Wakeup_Index ext_int_idx, bool enable);
void LL_SYSCON_EXT_INTR_Set_Triggle_Condition(SYSTEM_EXT_INT_Wakeup_Index ext_int_idx, SYSTEM_EXT_INT_Triggle_Type triggle);
uint8_t LL_SYSCON_EXT_INTR_Stat(void);
uint8_t LL_SYSCON_EXT_INTR_Stat_Raw(void);
void LL_SYSCON_EXT_INTR_Clear(void);
void LL_SYSCON_AWO_phy_pd_en(uint8_t en);
void LL_SYSCON_AWO_phy_po_en(uint8_t en);
void LL_SYSCON_AWO_ldo18_po_en(uint8_t en);
void LL_SYSCON_AWO_sram_pd_en(uint8_t en);
void LL_SYSCON_AWO_sram_po_en(uint8_t en);
void LL_SYSCON_AWO_pmu_ret_en(uint8_t en);
void LL_SYSCON_AWO_pmu_unret_en(uint8_t en);
void LL_SYSCON_AWO_pdcmp_po(uint8_t po);
void LL_SYSCON_AWO_cpucore_retreg_po(uint8_t po);
void LL_SYSCON_AWO_idle_reg_set(uint32_t val);
void LL_SYSCON_AWO_lp_mode_awo(uint8_t lp_mode_awo);
void LL_SYSCON_AWO_clk_sel_set(uint32_t val);
void LL_SYSCON_CMP_rfreg_len(uint16_t rfreg_len);
void LL_SYSCON_CMP_rfreg_base_addr(uint16_t rfreg_base_addr);
void LL_SYSCON_CMP_rfreg_action_set(uint32_t val);
uint8_t LL_SYSCON_CMP_save_done_get(void);
void LL_SYSCON_AWO_sw_pwr_ctrl_set(uint32_t val);
uint8_t LL_SYSCON_AWO_pmu_fsm_get(void);
uint8_t LL_SYSCON_CMP_restore_done_get(void);
void LL_SYSCON_CMP_sw_clkg_set(uint32_t value);
#ifdef __cplusplus
}
#endif

#endif /* __LL_SYSCON_H_ */
