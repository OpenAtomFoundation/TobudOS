/*
 * The Clear BSD License
 * Copyright (c) 2018, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
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
 * How to set up clock using clock driver functions:
 *
 * 1. Setup clock sources.
 *
 * 2. Set up all dividers.
 *
 * 3. Set up all selectors to provide selected clocks.
 */

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v4.1
processor: LPC824
package_id: LPC824M201JHI33
mcu_data: ksdk2_0
processor_version: 0.0.1
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

#include "fsl_power.h"
#include "fsl_clock.h"
#include "clock_config.h"
#include "tos_k.h"
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
    BOARD_BootClockIRC12M();
}

/*******************************************************************************
 ******************** Configuration BOARD_BootClockIRC12M **********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockIRC12M
called_from_default_init: true
outputs:
- {id: ADC_clock.outFreq, value: 12 MHz}
- {id: I2C0_clock.outFreq, value: 12 MHz}
- {id: I2C1_clock.outFreq, value: 12 MHz}
- {id: I2C2_clock.outFreq, value: 12 MHz}
- {id: LowPower_clock.outFreq, value: 10 kHz}
- {id: SPI0_clock.outFreq, value: 12 MHz}
- {id: SPI1_clock.outFreq, value: 12 MHz}
- {id: SYSPLL_clock.outFreq, value: 12 MHz}
- {id: System_clock.outFreq, value: 12 MHz}
- {id: divto750k_clock.outFreq, value: 750 kHz}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/*******************************************************************************
 * Variables for BOARD_BootClockIRC12M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockIRC12M configuration
 ******************************************************************************/
void BOARD_BootClockIRC12M(void)
{
    /*!< Set up the clock sources */
    /*!< Set up IRC */
    POWER_DisablePD(kPDRUNCFG_PD_IRC_OUT); /*!< Ensure FRO is on  */
    POWER_DisablePD(kPDRUNCFG_PD_IRC);     /*!< Ensure FRO is on  */
    CLOCK_Select(kSYSPLL_From_Irc);        /*!< set IRC to pll select */
    clock_sys_pll_t config;
    config.src = kCLOCK_SysPllSrcIrc;          /*!< set pll src  */
    config.targetFreq = 12000000U;             /*!< set pll target freq */
    CLOCK_InitSystemPll(&config);              /*!< set parameters */
    CLOCK_SetMainClkSrc(kCLOCK_MainClkSrcIrc); /*!< slecte irc for main clock */
    CLOCK_Select(kCLKOUT_From_Irc);            /*!< slecte IRC for CLKOUT */
    CLOCK_SetCoreSysClkDiv(1U);
    /*!< Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKIRC12M_CORE_CLOCK;
}

/*******************************************************************************
 ******************** Configuration BOARD_BootClockPll24M **********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockPll24M
outputs:
- {id: ADC_clock.outFreq, value: 24 MHz}
- {id: I2C0_clock.outFreq, value: 24 MHz}
- {id: I2C1_clock.outFreq, value: 24 MHz}
- {id: I2C2_clock.outFreq, value: 24 MHz}
- {id: LowPower_clock.outFreq, value: 10 kHz}
- {id: SPI0_clock.outFreq, value: 24 MHz}
- {id: SPI1_clock.outFreq, value: 24 MHz}
- {id: SYSPLL_clock.outFreq, value: 24 MHz}
- {id: System_clock.outFreq, value: 24 MHz}
- {id: divto750k_clock.outFreq, value: 750 kHz}
settings:
- {id: SYSCON.MAINCLKSEL.sel, value: SYSCON.PLL}
- {id: SYSCON.M_MULT.scale, value: '2'}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/*******************************************************************************
 * Variables for BOARD_BootClockPll24M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockPll24M configuration
 ******************************************************************************/
void BOARD_BootClockPll24M(void)
{
    /*!< Set up the clock sources */
    /*!< Set up IRC */
    POWER_DisablePD(kPDRUNCFG_PD_IRC_OUT); /*!< Ensure FRO is on  */
    POWER_DisablePD(kPDRUNCFG_PD_IRC);     /*!< Ensure FRO is on  */
    CLOCK_Select(kSYSPLL_From_Irc);        /*!< set IRC to pll select */
    clock_sys_pll_t config;
    config.src = kCLOCK_SysPllSrcIrc;             /*!< set pll src  */
    config.targetFreq = 24000000U;                /*!< set pll target freq */
    CLOCK_InitSystemPll(&config);                 /*!< set parameters */
    CLOCK_SetMainClkSrc(kCLOCK_MainClkSrcSysPll); /*!< slecte syspll for main clock */
    CLOCK_Select(kCLKOUT_From_Irc);               /*!< slecte IRC for CLKOUT */
    CLOCK_SetCoreSysClkDiv(1U);
    /*!< Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKPLL24M_CORE_CLOCK;
}
void SysTick_Handler(void)
{
  if(tos_knl_is_running())
  {
      tos_knl_irq_enter();
      tos_tick_handler();
      tos_knl_irq_leave();
  }
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}
