#include "ln88xx.h"
#include "hal/syscon_types.h"
#include "hal/hal_syscon.h"
#include "ll/ll_syscon.h"
#include "hal/hal_common.h"


/**
 * @brief Real 32K period, can be used by SW to calculate PPM.
 *
 * @return uint16_t
 */
uint16_t HAL_SYSCON_Get32KPeriodNs(void)
{
    return LL_SYSCON_Get32KPeriodNs();
}


/**
 * @brief Enable of cal 32K, enable of update calibratioin result.
 *
 */
void HAL_SYSCON_32KCaliEnable(void)
{
    LL_SYSCON_32KCaliEnable();
}


/**
 * @brief Set CPU sleep duration time, unit: ns.
 *
 * @param n_ns CPU sleep duration time, unit is ns.
 */
void HAL_SYSCON_CPUSleepDurationEnable(unsigned long long n_ns)
{
    LL_SYSCON_CPUSleepDurationEnable(n_ns);
}


/**
 * @brief Disable CPU sleep.
 *
 */
void HAL_SYSCON_CPUSleepDurationDisable(void)
{
    LL_SYSCON_CPUSleepDurationDisable();
}


/**
 * @brief Get CPU real sleep time.
 *
 * @return uint64_t real sleep time, unit is ns.
 */
uint64_t HAL_SYSCON_RealSleepTime(void)
{
    return LL_SYSCON_RealSleepTime();
}

/**
 * @brief Get always on idle reg value.
 *
 * @return uint32_t awo idle reg value
 */
uint32_t HAL_SYSCON_IdleReg(void)
{
    return LL_SYSCON_IdleReg();
}


/**
 * @brief Calculate compensate time, unit is ns.
 *
 * @return uint32_t compensate time.
 */
uint32_t HAL_SYSCON_CalculateCompensateNs(void)
{
    return LL_SYSCON_CalculateCompensateNs();
}


/**
 * @brief Indicate a true lock after debounce logic.
 *
 * @return true locked.
 * @return false not locked.
 */
bool HAL_SYSCON_IsSysPllLocked(void)
{
    return LL_SYSCON_IsSysPllLocked();
}


/**
 * @brief Set lock signal parity, debounce delay.
 *
 * @param lock_polarity 1 bit.
 * @param debounce_dly threshold to indicate a true lock.
 */
void HAL_SYSCON_SysPllDebounceSet(uint8_t lock_polarity, uint8_t debounce_dly)
{
    LL_SYSCON_SysPllDebounceSet(lock_polarity, debounce_dly);
}

/**
 * @brief Select flash mode or mirror mode.
 *
 * @param mode 1 -- flash mode; 0 -- mirror mode.
 */
void HAL_SYSCON_FlashOrMirrorMode(uint8_t mode)
{
    LL_SYSCON_FlashOrMirrorMode(mode);
}

/**
 * @brief Get boot mode.
 *
 * @return uint8_t  bootmode bit[0:2]
 */
uint8_t HAL_SYSCON_GetBootMode(void)
{
    return LL_SYSCON_GetBootMode();
}


/**
 * @brief Reset Core, only five options is valid: all, phy, rtc, wic, pmu.
 *
 * @param src one option of pmu, wic, rtc, phy, all.
 */
void HAL_SYSCON_SoftwareResetCore(SYSCON_SwRst_Core src)
{
    assert_param(IS_SWRST_CORE(src));

    LL_SYSCON_SoftwareResetCore(src);
}


/**
 * @brief Select HCLK source to PLL.
 *
 */
void HAL_SYSCON_ClkSwitchToPll(void)
{
    LL_SYSCON_ClkSwitchToPll();
}


/**
 * @brief Select HCLK source to XTAL.
 *
 */
void HAL_SYSCON_ClkSwitchToXtal(void)
{
    LL_SYSCON_ClkSwitchToXtal();
}


/**
 * @brief Set HCLK division.
 *
 * @param ahb_divider
 */
void HAL_SYSCON_SetHclkDivision(uint8_t ahb_divider)
{
    assert_param( (ahb_divider - 1) <= 0x0F );

    LL_SYSCON_SetHclkDivision(ahb_divider);
}


/**
 * @brief Set PCLK0 division.
 *
 * @param apb0_divider
 */
void HAL_SYSCON_SetPclk0Division(uint8_t apb0_divider)
{
    assert_param( (apb0_divider - 1) <= 0x0F );

    LL_SYSCON_SetPclk0Division(apb0_divider);
}


/**
 * @brief Set PCLK1 Division.
 *
 * @param apb1_divider
 */
void HAL_SYSCON_SetPclk1Division(uint8_t apb1_divider)
{
    assert_param( (apb1_divider - 1) <= 0x0F );

    LL_SYSCON_SetPclk1Division(apb1_divider);
}


/**
 * @brief Clock gate enable.
 *
 * @param src core clock src.
 * @param ena 1 -- enable or 0 -- disable.
 */
void HAL_SYSCON_CoreClockEnable(SYSCON_ClkGate_Core src, bool ena)
{
    assert_param(IS_CLKGATE_CORE(src));

    LL_SYSCON_CoreClockEnable(src, ena);
}


/**
 * @brief Get core clock enable status, 1 bit for 1 core clock.
 *
 * @return uint32_t ORed value, 1 bit for 1 core clock, @see SYSCON_ClkGate_Core.
 */
uint32_t HAL_SYSCON_CoreClockStatus(void)
{
    return LL_SYSCON_CoreClockStatus();
}


/**
 * @brief Peripheral clock enable.
 * Note: call this function one time to enable one clock.
 *
 * @param src peripheral module.
 * @param ena enable / disable.
 */
void HAL_SYSCON_PeripheralClockEnable(SYSCON_ClkGate_Peripheral src, bool ena)
{
    assert_param(IS_CLKGATE_PERIPHERAL(src));

    LL_SYSCON_PeripheralClockEnable(src, ena);
}


/**
 * @brief Get peripheral clock enable status.
 *
 * @return uint32_t ORed value, @see SYSCON_ClkGate_Peripheral.
 */
uint32_t HAL_SYSCON_PeripheralClockStatus(void)
{
    return LL_SYSCON_PeripheralClockStatus();
}


/**
 * @brief Reset peripheral module.
 * Note: call this function one time to reset one peripheral module.
 *
 * @param peri @see SYSCON_SwRst_Peripheral.
 */
void HAL_SYSCON_SoftwareResetPeripheral(SYSCON_SwRst_Peripheral peri)
{
    assert_param(IS_SWRST_PERIPHERAL(peri));

    LL_SYSCON_SoftwareResetPeripheral(peri);
}


/**
 * @brief Set IO function.
 *
 * @param af_type function type, @see GPIO_AltFunctionType.
 * @param af_io_idx IO index, @see GPIO_AltFunctionIoIndex.
 * @param en enable / disable.
 */
void HAL_SYSCON_FuncIOSet(GPIO_AltFunctionType af_type, GPIO_AltFunctionIoIndex af_io_idx, uint8_t en)
{
    assert_param(IS_GPIO_AF_TYPE(af_type));
    assert_param(IS_GPIO_AF_IO_INDEX(af_io_idx));

    LL_SYSCON_FuncIOSet(af_type, af_io_idx, en);
}


/**
 * @brief Disable all function on all configurable IO pad.
 *
 */
void HAL_SYSCON_FuncIODisableAll(void)
{
    LL_SYSCON_FuncIODisableAll();
}

/**
 * @brief GPIO Pull Up
 *
 */
void HAL_SYSCON_GPIO_PullUp(GPIO_Num gpio_num){
    LL_SYSCON_GPIO_PullUp(gpio_num);
};

/**
 * @brief GPIO Pull Down
 *
 */
void HAL_SYSCON_GPIO_PullDown(GPIO_Num gpio_num){
    LL_SYSCON_GPIO_PullDown(gpio_num);
};



/**
 * @brief Select SPIS as IO pad.
 *
 * @param en 1 -- enable; 0 -- disable.
 */
void HAL_SYSCON_SPIS_Enable(uint8_t en)
{
    LL_SYSCON_SPIS_Enable(en);
}

/**
 * @brief Select spim and csnX IO as pad.
 *
 * @param index @see SYSCON_SPIM_Index.
 */
void HAL_SYSCON_SPIMEnable(SYSCON_SPIM_Index index)
{
    assert_param(IS_SPIM_INDEX(index));

    LL_SYSCON_SPIMEnable(index);
}

/**
 * @brief Deselect SPIM and csnX IO as pad.
 *
 * @param index @see SYSCON_SPIM_Index
 */
void HAL_SYSCON_SPIMDisable(SYSCON_SPIM_Index index)
{
    assert_param(IS_SPIM_INDEX(index));

    LL_SYSCON_SPIMDisable(index);
}

/**
 * @brief Disable all SPIM.
 *
 */
void HAL_SYSCON_SPIMDisableAll(void)
{
    LL_SYSCON_SPIMDisableAll();
}


/**
 * @brief Select SDIO IO as pad.
 *
 */
void HAL_SYSCON_SDIOEnable(void)
{
    LL_SYSCON_SDIOEnable();
}


/**
 * @brief Deselect SDIO IO as pad.
 *
 */
void HAL_SYSCON_SDIODisable(void)
{
    LL_SYSCON_SDIODisable();
}

/**
 * @brief Select/Deselect SPIFlash as pad.
 *
 * @param enable
 */
void HAL_SYSCON_SPIFlashEnable(uint8_t enable)
{
    if (enable) {
        LL_SYSCON_SPIFlashEnable();
    } else {
        LL_SYSCON_SPIFlashDisable();
    }
}

/**
 * @brief Select I2S chnX IO as pad.
 *
 * @param index @see SYSCON_I2S_Index.
 */
void HAL_SYSCON_I2SEnable(SYSCON_I2S_Index index)
{
    assert_param(IS_I2S_INDEX(index));

    LL_SYSCON_I2SEnable(index);
}


/**
 * @brief Disable all I2S module.
 *
 */
void HAL_SYSCON_I2SDisable(void)
{
    LL_SYSCON_I2SDisableAll();
}


/**
 * @brief SWD enable.
 *
 */
void HAL_SYSCON_SWDEnable(void)
{
    LL_SYSCON_SWDEnable();
}


/**
 * @brief SWD disable.
 *
 */
void HAL_SYSCON_SWDDisable(void)
{
    LL_SYSCON_SWDDisable();
}


/**
 * @brief DBGH enable.
 *
 */
void HAL_SYSCON_DBGHEnable(void)
{
    LL_SYSCON_DBGHEnable();
}


/**
 * @brief DBHG disable.
 *
 */
void HAL_SYSCON_DBGHDisable(void)
{
    LL_SYSCON_DBGHDisable();
}



/**
 * @brief HCLK source select.
 *
 * @param src @see SYSTEM_CLOCK_SRC.
 */
void HAL_SYSCON_SelectSysClkSrc(SYSTEM_CLOCK_SRC src)
{
    assert_param(IS_CLOCK_SRC(src));

    LL_SYSCON_SelectSysClkSrc(src);
}


/**
 * @brief Set CPU reset request mask.
 *
 * @param mask 1 -- mask, 0 -- not masked.
 */
void HAL_SYSCON_CPUResetReqMask(uint8_t mask)
{
    LL_SYSCON_CPUResetReqMask(mask);
}


/**
 * @brief PHY must be reset after TxImgCal.
 */

void HAL_SysconPhyReset(void)
{
	LL_SYSCON_PHY_Reset();
}

void HAL_SYSCON_EXT_INTR_Enable(SYSTEM_EXT_INT_Wakeup_Index ext_int_idx, bool enable)
{
    LL_SYSCON_EXT_INTR_Enable(ext_int_idx, enable);
}
void HAL_SYSCON_EXT_INTR_Set_Triggle_Condition(SYSTEM_EXT_INT_Wakeup_Index ext_int_idx, SYSTEM_EXT_INT_Triggle_Type triggle)
{
    LL_SYSCON_EXT_INTR_Set_Triggle_Condition(ext_int_idx, triggle);
}
uint8_t HAL_SYSCON_EXT_INTR_Stat(void)
{
    return LL_SYSCON_EXT_INTR_Stat();
}
uint8_t HAL_SYSCON_EXT_INTR_Stat_Raw(void)
{
    return LL_SYSCON_EXT_INTR_Stat_Raw();
}
void HAL_SYSCON_EXT_INTR_Clear(void)
{
    LL_SYSCON_EXT_INTR_Clear();
}


/**
 * @brief  Reset the mcu by software
 *
 * @param  none
 *
 * @note   use the 3.5 version of the firmware library. 
 */
void HAL_SYSCON_SoftReset(void)
{
    //cpu reset request unmask
    HAL_SYSCON_CPUResetReqMask(0);
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset(); // 复位
}
void HAL_SYSCON_RCO32K_Bitsel_Set(uint8_t bitsel)
{
    LL_SYSCON_RCO32K_Bitsel_Set(bitsel);
}
void HAL_SYSCON_RCO32K_Cbit_Set(uint8_t cbit)
{
    LL_SYSCON_RCO32K_Cbit_Set(cbit);
}

void HAL_SYSCON_Xtal40MCap_Set(uint8_t cap)
{
    LL_SYSCON_Xtal40MCap_Set(cap);
}

void HAL_SYSCON_PmuCfg(void)
{  
    LL_SYSCON_PMUReg_Set(0x8B050900);// (BUCK)  r_vtrim: bit[9:8]=0b01 //00: 1.195 / 01: 1.202  / 10: 1.210 / 11: 1.218
    //LL_SYSCON_PMUCfg_Set(0x20001603);// o_digldo_norvdd: bit[10:8]=0b100; o_digldo_retvdd: bit[14:12]=0b001
    LL_SYSCON_PMUCfg_Set(0x310746fd);// o_digldo_norvdd: bit[10:8]=0b100; o_digldo_retvdd: bit[14:12]=0b110
    LL_SYSCON_Misc_Set(0x00002C0D);

    LL_SYSCON_PMUAvdd_Set(0x00055DB6);// Avdd & PLL & RF  voltage  55B75
}
void HAL_SYSCON_AWO_phy_pd_en(uint8_t en)
{
    LL_SYSCON_AWO_phy_pd_en(en);
}
void HAL_SYSCON_AWO_phy_po_en(uint8_t en)
{
    LL_SYSCON_AWO_phy_po_en(en);
}
void HAL_SYSCON_AWO_ldo18_po_en(uint8_t en)
{
    LL_SYSCON_AWO_ldo18_po_en(en);
}
void HAL_SYSCON_AWO_sram_pd_en(uint8_t en)
{
    LL_SYSCON_AWO_sram_pd_en(en);
}
void HAL_SYSCON_AWO_sram_po_en(uint8_t en)
{
    LL_SYSCON_AWO_sram_po_en(en);
}

void HAL_SYSCON_AWO_pmu_ret_en(uint8_t en)
{
    LL_SYSCON_AWO_pmu_ret_en(en);
}
void HAL_SYSCON_AWO_pmu_unret_en(uint8_t en)
{
    LL_SYSCON_AWO_pmu_unret_en(en);
}
void HAL_SYSCON_AWO_pdcmp_po(uint8_t po)
{
    LL_SYSCON_AWO_pdcmp_po(po);
}

void HAL_SYSCON_AWO_cpucore_retreg_po(uint8_t po)
{
    LL_SYSCON_AWO_cpucore_retreg_po(po);
}
void HAL_SYSCON_AWO_idle_reg_set(uint32_t val)
{
    LL_SYSCON_AWO_idle_reg_set(val);
}

void HAL_SYSCON_AWO_lp_mode_awo(uint8_t lp_mode_awo)
{
    LL_SYSCON_AWO_lp_mode_awo(lp_mode_awo);
}

void HAL_SYSCON_AWO_clk_sel_set(uint32_t val)
{
    LL_SYSCON_AWO_clk_sel_set(val);
}

void HAL_SYSCON_CMP_rfreg_len(uint16_t rfreg_len)
{
    LL_SYSCON_CMP_rfreg_len(rfreg_len);
}
void HAL_SYSCON_CMP_rfreg_base_addr(uint16_t rfreg_base_addr)
{
    LL_SYSCON_CMP_rfreg_base_addr(rfreg_base_addr);
}

void HAL_SYSCON_CMP_rfreg_action_set(uint32_t val)
{
    LL_SYSCON_CMP_rfreg_action_set(val);
}

uint8_t HAL_SYSCON_CMP_save_done_get(void)
{
    return LL_SYSCON_CMP_save_done_get();
}

void HAL_SYSCON_AWO_sw_pwr_ctrl_set(uint32_t value)
{
    LL_SYSCON_AWO_sw_pwr_ctrl_set(value);
}

uint8_t HAL_SYSCON_AWO_pmu_fsm_get(void)
{
    return LL_SYSCON_AWO_pmu_fsm_get();
}

uint8_t HAL_SYSCON_CMP_restore_done_get(void)
{
    return LL_SYSCON_CMP_restore_done_get();
}

void HAL_SYSCON_CMP_sw_clkg_set(uint32_t value)
{
    LL_SYSCON_CMP_sw_clkg_set(value);
}

