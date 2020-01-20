/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * How to setup clock using clock driver functions:
 *
 * 1. Call CLOCK_InitXXXPLL() to configure corresponding PLL clock.
 *
 * 2. Call CLOCK_InitXXXpfd() to configure corresponding PLL pfd clock.
 *
 * 3. Call CLOCK_SetMux() to configure corresponding clock source for target clock out.
 *
 * 4. Call CLOCK_SetDiv() to configure corresponding clock divider for target clock out.
 *
 * 5. Call CLOCK_SetXtalFreq() to set XTAL frequency based on board settings.
 *
 */

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v4.0
processor: MIMXRT1052xxxxx
package_id: MIMXRT1052DVL6B
mcu_data: i_mx_1_0
processor_version: 0.0.0
board: IMXRT1050-EVKB
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockRUN();
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockRUN
called_from_default_init: true
outputs:
- {id: AHB_CLK_ROOT.outFreq, value: 600 MHz}
- {id: CAN_CLK_ROOT.outFreq, value: 40 MHz}
- {id: CKIL_SYNC_CLK_ROOT.outFreq, value: 32.768 kHz}
- {id: CLK_1M.outFreq, value: 1 MHz}
- {id: CSI_CLK_ROOT.outFreq, value: 12 MHz}
- {id: ENET_125M_CLK.outFreq, value: 2.4 MHz}
- {id: ENET_25M_REF_CLK.outFreq, value: 1.2 MHz}
- {id: FLEXIO1_CLK_ROOT.outFreq, value: 30 MHz}
- {id: FLEXIO2_CLK_ROOT.outFreq, value: 30 MHz}
- {id: FLEXSPI_CLK_ROOT.outFreq, value: 37.5 MHz}
- {id: IPG_CLK_ROOT.outFreq, value: 150 MHz}
- {id: LCDIF_CLK_ROOT.outFreq, value: 67.5 MHz}
- {id: LPI2C_CLK_ROOT.outFreq, value: 60 MHz}
- {id: LPSPI_CLK_ROOT.outFreq, value: 105.6 MHz}
- {id: LVDS1_CLK.outFreq, value: 1.2 GHz}
- {id: PERCLK_CLK_ROOT.outFreq, value: 75 MHz}
- {id: PLL7_MAIN_CLK.outFreq, value: 24 MHz}
- {id: SAI1_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SAI2_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SAI3_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SEMC_CLK_ROOT.outFreq, value: 75 MHz}
- {id: SPDIF0_CLK_ROOT.outFreq, value: 30 MHz}
- {id: TRACE_CLK_ROOT.outFreq, value: 352/3 MHz}
- {id: UART_CLK_ROOT.outFreq, value: 80 MHz}
- {id: USDHC1_CLK_ROOT.outFreq, value: 198 MHz}
- {id: USDHC2_CLK_ROOT.outFreq, value: 198 MHz}
settings:
- {id: CCM.AHB_PODF.scale, value: '1', locked: true}
- {id: CCM.ARM_PODF.scale, value: '2', locked: true}
- {id: CCM.LPSPI_PODF.scale, value: '5', locked: true}
- {id: CCM.PERCLK_PODF.scale, value: '2', locked: true}
- {id: CCM.SEMC_PODF.scale, value: '8'}
- {id: CCM.TRACE_PODF.scale, value: '3', locked: true}
- {id: CCM_ANALOG.PLL1_BYPASS.sel, value: CCM_ANALOG.PLL1}
- {id: CCM_ANALOG.PLL1_PREDIV.scale, value: '1', locked: true}
- {id: CCM_ANALOG.PLL1_VDIV.scale, value: '50', locked: true}
- {id: CCM_ANALOG.PLL2.denom, value: '1', locked: true}
- {id: CCM_ANALOG.PLL2.num, value: '0', locked: true}
- {id: CCM_ANALOG.PLL2_BYPASS.sel, value: CCM_ANALOG.PLL2_OUT_CLK}
- {id: CCM_ANALOG.PLL2_PFD0_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD0}
- {id: CCM_ANALOG.PLL2_PFD1_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD1}
- {id: CCM_ANALOG.PLL2_PFD2_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD2}
- {id: CCM_ANALOG.PLL2_PFD3_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD3}
- {id: CCM_ANALOG.PLL3_BYPASS.sel, value: CCM_ANALOG.PLL3}
- {id: CCM_ANALOG.PLL3_PFD0_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD0}
- {id: CCM_ANALOG.PLL3_PFD1_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD1}
- {id: CCM_ANALOG.PLL3_PFD2_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD2}
- {id: CCM_ANALOG.PLL3_PFD3_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD3}
- {id: CCM_ANALOG.PLL4.denom, value: '50'}
- {id: CCM_ANALOG.PLL4.div, value: '47'}
- {id: CCM_ANALOG.PLL5.denom, value: '1'}
- {id: CCM_ANALOG.PLL5.div, value: '40'}
- {id: CCM_ANALOG.PLL5.num, value: '0'}
- {id: CCM_ANALOG_PLL_USB1_POWER_CFG, value: 'Yes'}
sources:
- {id: XTALOSC24M.OSC.outFreq, value: 24 MHz, enabled: true}
- {id: XTALOSC24M.RTC_OSC.outFreq, value: 32.768 kHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
const clock_arm_pll_config_t armPllConfig_BOARD_BootClockRUN = {
    .loopDivider = 88, /* PLL loop divider, Fout = Fin * 44 */
};
const clock_sys_pll_config_t sysPllConfig_BOARD_BootClockRUN = {
    .loopDivider = 1,                         /* PLL loop divider, Fout = Fin * ( 20 + loopDivider*2 + numerator / denominator ) */
    .numerator = 0,                           /* 30 bit numerator of fractional loop divider */
    .denominator = 1,                         /* 30 bit denominator of fractional loop divider */
};
const clock_usb_pll_config_t usb1PllConfig_BOARD_BootClockRUN = {
    .loopDivider = 0, /* PLL loop divider, Fout = Fin * 20 */
};
/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
    /* Init RTC OSC clock frequency. */
    CLOCK_SetRtcXtalFreq(32768U);
    /* Set XTAL 24MHz clock frequency. */
    CLOCK_SetXtalFreq(24000000U);
    /* Setting PeriphClk2Mux and PeriphMux to provide stable clock before PLLs are initialed */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 1); /* Set PERIPH_CLK2 MUX to OSC */
    CLOCK_SetMux(kCLOCK_PeriphMux, 1);     /* Set PERIPH_CLK MUX to PERIPH_CLK2 */
    /* Setting the VDD_SOC to 1.5V. It is necessary to config AHB to 600Mhz. */
    DCDC->REG3 = (DCDC->REG3 & (~DCDC_REG3_TRG_MASK)) | DCDC_REG3_TRG(0x12);
    /* Waiting for DCDC_STS_DC_OK bit is asserted */
    while (DCDC_REG0_STS_DC_OK_MASK != (DCDC_REG0_STS_DC_OK_MASK & DCDC->REG0))
    {
    }
    /* Init ARM PLL. */
    CLOCK_InitArmPll(&armPllConfig_BOARD_BootClockRUN);
    /* Init System PLL. */
#ifndef SKIP_SYSCLK_INIT
    CLOCK_InitSysPll(&sysPllConfig_BOARD_BootClockRUN);
#endif
    /* Init Usb1 PLL. */
#if !(defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1))
    CLOCK_InitUsb1Pll(&usb1PllConfig_BOARD_BootClockRUN);
#endif
    /* Enbale Audio PLL output. */
    CCM_ANALOG->PLL_AUDIO |= CCM_ANALOG_PLL_AUDIO_ENABLE_MASK;
    /* Enbale Video PLL output. */
    CCM_ANALOG->PLL_VIDEO |= CCM_ANALOG_PLL_VIDEO_ENABLE_MASK;
    /* Enable ENET PLL output. */
    CCM_ANALOG->PLL_ENET |= CCM_ANALOG_PLL_ENET_ENABLE_MASK;
    CCM_ANALOG->PLL_ENET |= CCM_ANALOG_PLL_ENET_ENET_25M_REF_EN_MASK;
    /* Set periph clock2 clock source. */
    CLOCK_SetMux(kCLOCK_PeriphClk2Mux, 0);
    /* Set PERIPH_CLK2_PODF. */
    CLOCK_SetDiv(kCLOCK_PeriphClk2Div, 0);
    /* Set periph clock source. */
    CLOCK_SetMux(kCLOCK_PeriphMux, 0);
    /* Set AHB_PODF. */
    CLOCK_SetDiv(kCLOCK_AhbDiv, 0);
    /* Set IPG_PODF. */
    CLOCK_SetDiv(kCLOCK_IpgDiv, 3);
    /* Set ARM_PODF. */
    CLOCK_SetDiv(kCLOCK_ArmDiv, 1);
    /* Set preperiph clock source. */
    CLOCK_SetMux(kCLOCK_PrePeriphMux, 3);
    /* Set PERCLK_PODF. */
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 1);
    /* Set per clock source. */
    CLOCK_SetMux(kCLOCK_PerclkMux, 0);
    /* Set Usdhc1 clock source. */
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 0);
    /* Set USDHC1_PODF. */
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 1);
    /* Set Usdhc2 clock source. */
    CLOCK_SetMux(kCLOCK_Usdhc2Mux, 0);
    /* Set USDHC2_PODF. */
    CLOCK_SetDiv(kCLOCK_Usdhc2Div, 1);
#ifndef SKIP_SYSCLK_INIT
    /* Set Semc alt clock source. */
    CLOCK_SetMux(kCLOCK_SemcAltMux, 0);
    /* Set Semc clock source. */
    CLOCK_SetMux(kCLOCK_SemcMux, 1);
    /* Set SEMC_PODF. */
    CLOCK_SetDiv(kCLOCK_SemcDiv, 1);
#endif
#if !(defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1))
    /* Set Flexspi clock source. */
    CLOCK_SetMux(kCLOCK_FlexspiMux, 0);
    /* Set FLEXSPI_PODF. */
    CLOCK_SetDiv(kCLOCK_FlexspiDiv, 1);
#endif
    /* Set Csi clock source. */
    CLOCK_SetMux(kCLOCK_CsiMux, 0);
    /* Set CSI_PODF. */
    CLOCK_SetDiv(kCLOCK_CsiDiv, 1);
    /* Set Lpspi clock source. */
    CLOCK_SetMux(kCLOCK_LpspiMux, 2);
    /* Set LPSPI_PODF. */
    CLOCK_SetDiv(kCLOCK_LpspiDiv, 4);
    /* Set Trace clock source. */
    CLOCK_SetMux(kCLOCK_TraceMux, 2);
    /* Set TRACE_PODF. */
    CLOCK_SetDiv(kCLOCK_TraceDiv, 2);
    /* Set Sai1 clock source. */
    CLOCK_SetMux(kCLOCK_Sai1Mux, 0);
    /* Set SAI1_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, 3);
    /* Set SAI1_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai1Div, 1);
    /* Set Sai2 clock source. */
    CLOCK_SetMux(kCLOCK_Sai2Mux, 0);
    /* Set SAI2_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai2PreDiv, 3);
    /* Set SAI2_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai2Div, 1);
    /* Set Sai3 clock source. */
    CLOCK_SetMux(kCLOCK_Sai3Mux, 0);
    /* Set SAI3_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Sai3PreDiv, 3);
    /* Set SAI3_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Sai3Div, 1);
    /* Set Lpi2c clock source. */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0);
    /* Set LPI2C_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 0);
    /* Set Can clock source. */
    CLOCK_SetMux(kCLOCK_CanMux, 2);
    /* Set CAN_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_CanDiv, 1);
    /* Set Uart clock source. */
    CLOCK_SetMux(kCLOCK_UartMux, 0);
    /* Set UART_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_UartDiv, 0);
    /* Set Lcdif pre clock source. */
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 5);
    /* Set LCDIF_PRED. */
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 1);
    /* Set LCDIF_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_LcdifDiv, 3);
    /* Set Spdif clock source. */
    CLOCK_SetMux(kCLOCK_SpdifMux, 3);
    /* Set SPDIF0_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Spdif0PreDiv, 1);
    /* Set SPDIF0_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Spdif0Div, 7);
    /* Set Flexio1 clock source. */
    CLOCK_SetMux(kCLOCK_Flexio1Mux, 3);
    /* Set FLEXIO1_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Flexio1PreDiv, 1);
    /* Set FLEXIO1_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Flexio1Div, 7);
    /* Set Flexio2 clock source. */
    CLOCK_SetMux(kCLOCK_Flexio2Mux, 3);
    /* Set FLEXIO2_CLK_PRED. */
    CLOCK_SetDiv(kCLOCK_Flexio2PreDiv, 1);
    /* Set FLEXIO2_CLK_PODF. */
    CLOCK_SetDiv(kCLOCK_Flexio2Div, 7);
    /* Set Pll3 sw clock source. */
    CLOCK_SetMux(kCLOCK_Pll3SwMux, 0);
    /* Set lvds1 clock source. */
    CCM_ANALOG->MISC1 =
        (CCM_ANALOG->MISC1 & (~CCM_ANALOG_MISC1_LVDS1_CLK_SEL_MASK)) | CCM_ANALOG_MISC1_LVDS1_CLK_SEL(0);
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
}
