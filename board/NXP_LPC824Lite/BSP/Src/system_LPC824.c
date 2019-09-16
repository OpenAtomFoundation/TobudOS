/*
** ###################################################################
**     Processors:          LPC824M201JDH20
**                          LPC824M201JHI33
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          MCUXpresso Compiler
**
**     Reference manual:    LPC82x User manual Rev.1.3  11 Aug 2017
**     Version:             rev. 1.2, 2017-06-08
**     Build:               b180207
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     The Clear BSD License
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without
**     modification, are permitted (subject to the limitations in the
**     disclaimer below) provided that the following conditions are met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**
**     * Neither the name of the copyright holder nor the names of its
**       contributors may be used to endorse or promote products derived from
**       this software without specific prior written permission.
**
**     NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
**     GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
**     HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
**     WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
**     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
**     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**     WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
**     OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
**     IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2016-08-12)
**         Initial version.
**     - rev. 1.1 (2016-11-25)
**         Update CANFD and Classic CAN register.
**         Add MAC TIMERSTAMP registers.
**     - rev. 1.2 (2017-06-08)
**         Remove RTC_CTRL_RTC_OSC_BYPASS.
**         SYSCON_ARMTRCLKDIV rename to SYSCON_ARMTRACECLKDIV.
**         Remove RESET and HALT from SYSCON_AHBCLKDIV.
**
** ###################################################################
*/

/*!
 * @file LPC824
 * @version 1.2
 * @date 2017-06-08
 * @brief Device specific configuration file for LPC824 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "fsl_device_registers.h"

/* get system pll input freq */
static uint32_t CLOCK_GetSystemPLLInClkRate(void)
{
    uint32_t freq = 0U;

    switch ((SYSCON->SYSPLLCLKSEL & SYSCON_SYSPLLCLKSEL_SEL_MASK))
    {
        /* source from external clock in */
        case 0x00U:
            freq = CLK_IRC_12MHZ;
            break;
        /* source from the IRC clock */
        case 0x01U:
            freq = CLK_OSC_IN;
            break;
        /* source from external clock clock */
        case 0x03U:
            freq = EXT_CLK_IN;
            break;

        default:
            break;
    }

    return freq;
}

/* get system pll output freq*/
static uint32_t Clock_GetPLLFreq(uint32_t PLLReg, uint32_t inputRate)
{
    uint32_t m_val = ((PLLReg & 0x1F) + 1);

    return (inputRate * m_val);
}




/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) {

#if defined(__MCUXPRESSO)
    extern void(*const g_pfnVectors[]) (void);
    SCB->VTOR = (uint32_t) &g_pfnVectors;
#else
    extern void *__Vectors;
    SCB->VTOR = (uint32_t) &__Vectors;
#endif
    SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
  SystemInitHook();
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {
  uint32_t wdt_osc = 0U;
  uint32_t irc_clk = 12000000U;

  switch ((SYSCON->WDTOSCCTRL >> 5) & 0x0F) {
    case 0:  wdt_osc =       0; break;
    case 1:  wdt_osc =  600000; break;
    case 2:  wdt_osc = 1050000; break;
    case 3:  wdt_osc = 1400000; break;
    case 4:  wdt_osc = 1750000; break;
    case 5:  wdt_osc = 2100000; break;
    case 6:  wdt_osc = 2400000; break;
    case 7:  wdt_osc = 2700000; break;
    case 8:  wdt_osc = 3000000; break;
    case 9:  wdt_osc = 3250000; break;
    case 10: wdt_osc = 3500000; break;
    case 11: wdt_osc = 3750000; break;
    case 12: wdt_osc = 4000000; break;
    case 13: wdt_osc = 4200000; break;
    case 14: wdt_osc = 4400000; break;
    case 15: wdt_osc = 4600000; break;
  }
  wdt_osc /= (((SYSCON->WDTOSCCTRL & 0x1F) + 1) << 1);

  switch (SYSCON->MAINCLKSEL & SYSCON_MAINCLKSEL_SEL_MASK)
  {
    case 0U:                                           /* IRC  */
      SystemCoreClock = irc_clk;
      break;
      case 1U:                                        /* System PLL input */
        switch (SYSCON->SYSPLLCLKSEL & 0x03) {
          case 0:                                         /* IRC */
            SystemCoreClock = irc_clk;
            break;
          case 1:                                         /* System oscillator */
            SystemCoreClock = CLK_OSC_IN;
            break;
          case 3:                                         /* CLKIN */
            SystemCoreClock = EXT_CLK_IN;
            break;
          default:
            break;
        }
      break;
    case 2U:                                          /* watchdog oscillator */
      SystemCoreClock = wdt_osc;
      break;
    case 3U:                                          /* System PLL  */
      SystemCoreClock = Clock_GetPLLFreq((SYSCON->SYSPLLCTRL & SYSCON_SYSPLLCTRL_MSEL_MASK), CLOCK_GetSystemPLLInClkRate());
      break;
    default:
      break;
  }

  SystemCoreClock /= SYSCON->SYSAHBCLKDIV;
}

/* ----------------------------------------------------------------------------
   -- SystemInitHook()
   ---------------------------------------------------------------------------- */

__attribute__ ((weak)) void SystemInitHook (void) {
  /* Void implementation of the weak function. */
}
