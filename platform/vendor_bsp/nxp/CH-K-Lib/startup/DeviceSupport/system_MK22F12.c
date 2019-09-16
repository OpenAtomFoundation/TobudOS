/*
** ###################################################################
**     Processors:          MK22FX512VLH12
**                          MK22FN1M0VLH12
**                          MK22FX512VLK12
**                          MK22FN1M0VLK12
**                          MK22FX512VLL12
**                          MK22FN1M0VLL12
**                          MK22FX512VLQ12
**                          MK22FN1M0VLQ12
**                          MK22FX512VMC12
**                          MK22FN1M0VMC12
**                          MK22FX512VMD12
**                          MK22FN1M0VMD12
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          GNU C Compiler - CodeSourcery Sourcery G++
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manuals:   K22P64M120SF5RM, Rev.4, November 2014
**                          K22P800M120SF5RM, Rev.4, November 2014
**                          K22P100M120SF5RM, Rev.4, November 2014
**                          K22P144M120SF5RM, Rev.4, November 2014
**                          K22P121M120SF5RM, Rev.4, November 2014
**
**     Version:             rev. 1.9, 2015-05-25
**     Build:               b150528
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright (c) 2015 Freescale Semiconductor, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2012-06-06)
**         Initial version.
**     - rev. 1.1 (2012-11-12)
**         Update according to reference manual rev.1, draft B
**     - rev. 1.2 (2012-12-04)
**         Update according to reference manual rev.1
**     - rev. 1.3 (2013-03-11)
**         System initialization updated to add 120MHz clock option.
**     - rev. 1.4 (2013-04-05)
**         Changed start of doxygen comment.
**     - rev. 1.5 (2013-05-16)
**         Update according to reference manual rev.2
**     - rev. 1.6 (2013-10-29)
**         Definition of BITBAND macros updated to support peripherals with 32-bit acces disabled.
**         Access restriction of some registers fixed.
**     - rev. 1.7 (2014-02-18)
**         UART0 module - LON registers removed.
**     - rev. 1.8 (2015-02-19)
**         update of SystemInit() imlementation
**         Module access macro module_BASES replaced by module_BASE_PTRS.
**         Register accessor macros added to the memory map.
**         Renamed interrupt vector Watchdog to WDOG_EWM, LPTimer to LPTMR0 and LLW to LLWU
**     - rev. 1.9 (2015-05-25)
**         Update according to reference manual rev.4
**
** ###################################################################
*/

/*!
 * @file MK22F12
 * @version 1.9
 * @date 2015-05-25
 * @brief Device specific configuration file for MK22F12 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MK22F12.h"



/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) {
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    /* set CP10, CP11 Full Access */
#endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */
#if (DISABLE_WDOG)
  /* Disable the WDOG module */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = (uint16_t)0xC520u;     /* Key 1 */
  /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK  = (uint16_t)0xD928u;    /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */
#if (CLOCK_SETUP == 0)
  /* SIM->CLKDIV1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=1,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = (uint32_t)0x00110000u; /* Update system prescalers */
  /* Switch to FEI Mode */
  /* MCG->C1: CLKS=0,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (uint8_t)0x06u;
  /* MCG->C2: ??=0,??=0,RANGE0=0,HGO=0,EREFS=0,LP=0,IRCS=0 */
  MCG->C2 = (uint8_t)0x00u;
  /* MCG_C4: DMX32=0,DRST_DRS=1 */
  MCG->C4 = (uint8_t)((MCG->C4 & (uint8_t)~(uint8_t)0xC0u) | (uint8_t)0x20u);
  /* MCG->C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV0=0 */
  MCG->C5 = (uint8_t)0x00u;
  /* MCG->C6: LOLIE=0,PLLS=0,CME=0,VDIV0=0 */
  MCG->C6 = (uint8_t)0x00u;
  while((MCG->S & MCG_S_IREFST_MASK) == 0u) { /* Check that the source of the FLL reference clock is the internal reference clock. */
  }
  while((MCG->S & 0x0Cu) != 0x00u) {    /* Wait until output of the FLL is selected */
  }
#elif ((CLOCK_SETUP == 1) || (CLOCK_SETUP == 3))
  #if (CLOCK_SETUP == 1)
  /* SIM->CLKDIV1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=1,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = (uint32_t)0x01330000u; /* Update system prescalers */
  #else
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=4,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = (uint32_t)0x01440000u; /* Update system prescalers */
  #endif
  /* Switch to FBE Mode */
  /* MCG->C2: ??=0,??=0,RANGE0=2,HGO=0,EREFS=1,LP=0,IRCS=0 */
  MCG->C2 = (uint8_t)0x24u;
  /* OSC->CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC->CR = (uint8_t)0x00u;
  /* MCG->C7: OSCSEL=0 */
  MCG->C7 = (uint8_t)0x00u;
  /* MCG->C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (uint8_t)0x9Au;
  /* MCG->C4: DMX32=0,DRST_DRS=0 */
  MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
  /* MCG->C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV0=1 */
  MCG->C5 = (uint8_t)0x01u;
  #if (CLOCK_SETUP == 1)
  /* MCG->C6: LOLIE=0,PLLS=0,CME=0,VDIV0=1 */
  MCG->C6 = (uint8_t)0x01u;
  #else
  /* MCG->C6: LOLIE=0,PLLS=0,CME=0,VDIV0=6 */
  MCG->C6 = (uint8_t)0x06u;
  #endif
  while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u) { /* Check that the oscillator is running */
  }
  while((MCG->S & MCG_S_IREFST_MASK) != 0u) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((MCG->S & 0x0Cu) != 0x08u) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to PBE Mode */
  #if (CLOCK_SETUP == 1)
  /* MCG->C6: LOLIE=0,PLLS=1,CME=0,VDIV0=1 */
  MCG->C6 = (uint8_t)0x41u;
  #else
  /* MCG->C6: LOLIE=0,PLLS=1,CME=0,VDIV0=6 */
  MCG->C6 = (uint8_t)0x46u;
  #endif
  while((MCG->S & MCG_S_PLLST_MASK) == 0u) { /* Wait until the source of the PLLS clock has switched to the PLL */
  }
  while((MCG->S & MCG_S_LOCK0_MASK) == 0u) { /* Wait until locked */
  }
  /* Switch to PEE Mode */
  /* MCG->C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (uint8_t)0x1Au;
  while((MCG->S & 0x0Cu) != 0x0Cu) {    /* Wait until output of the PLL is selected */
  }
  while((MCG->S & MCG_S_LOCK0_MASK) == 0u) { /* Wait until locked */
  }
#elif (CLOCK_SETUP == 2)
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=1,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = (uint32_t)0x00110000u; /* Update system prescalers */
  /* Switch to FBE Mode */
  /* OSC->CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC->CR = (uint8_t)0x00u;
  /* MCG->C7: OSCSEL=0 */
  MCG->C7 = (uint8_t)0x00u;
  /* MCG->C2: ??=0,??=0,RANGE0=2,HGO=0,EREFS=1,LP=0,IRCS=0 */
  MCG->C2 = (uint8_t)0x24u;
  /* MCG->C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (uint8_t)0x9Au;
  /* MCG->C4: DMX32=0,DRST_DRS=0 */
  MCG->C4 &= (uint8_t)~(uint8_t)0xE0u;
  /* MCG->C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV0=0 */
  MCG->C5 = (uint8_t)0x00u;
  /* MCG->C6: LOLIE=0,PLLS=0,CME=0,VDIV0=0 */
  MCG->C6 = (uint8_t)0x00u;
  while((MCG->S & MCG_S_OSCINIT0_MASK) == 0u) { /* Check that the oscillator is running */
  }
  while((MCG->S & MCG_S_IREFST_MASK) != 0u) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((MCG->S & 0x0Cu) != 0x08u) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to BLPE Mode */
  /* MCG->C2: ??=0,??=0,RANGE0=2,HGO=0,EREFS=1,LP=0,IRCS=0 */
  MCG->C2 = (uint8_t)0x24u;
#endif
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {
  uint32_t MCGOUTClock;                                                        /* Variable to store output clock frequency of the MCG module */
  uint8_t Divider;

  if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) {
    /* Output of FLL or PLL is selected */
    if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) {
      /* FLL is selected */
      if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) {
        /* External reference clock is selected */
        if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
          MCGOUTClock = CPU_XTAL_CLK_HZ;                                       /* System oscillator drives MCG clock */
        } else { /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
          MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                    /* RTC 32 kHz oscillator drives MCG clock */
        } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
        Divider = (uint8_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
        MCGOUTClock = (MCGOUTClock / Divider);  /* Calculate the divided FLL reference clock */
        if ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) {
          MCGOUTClock /= 32u;                                                  /* If high range is enabled, additional 32 divider is active */
        } /* ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) */
      } else { /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
        MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                     /* The slow internal reference clock is selected */
      } /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
      /* Select correct multiplier to calculate the MCG output clock  */
      switch (MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) {
        case 0x0u:
          MCGOUTClock *= 640u;
          break;
        case 0x20u:
          MCGOUTClock *= 1280u;
          break;
        case 0x40u:
          MCGOUTClock *= 1920u;
          break;
        case 0x60u:
          MCGOUTClock *= 2560u;
          break;
        case 0x80u:
          MCGOUTClock *= 732u;
          break;
        case 0xA0u:
          MCGOUTClock *= 1464u;
          break;
        case 0xC0u:
          MCGOUTClock *= 2197u;
          break;
        case 0xE0u:
          MCGOUTClock *= 2929u;
          break;
        default:
          break;
      }
    } else { /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
      /* PLL is selected */
      Divider = (1u + (MCG->C5 & MCG_C5_PRDIV0_MASK));
      MCGOUTClock = (uint32_t)(CPU_XTAL_CLK_HZ / Divider);                     /* Calculate the PLL reference clock */
      Divider = ((MCG->C6 & MCG_C6_VDIV0_MASK) + 24u);
      MCGOUTClock *= Divider;                       /* Calculate the MCG output clock */
    } /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x40u) {
    /* Internal reference clock is selected */
    if ((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u) {
      MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                       /* Slow internal reference clock selected */
    } else { /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
      MCGOUTClock = CPU_INT_FAST_CLK_HZ / (1 << ((MCG->SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT));  /* Fast internal reference clock selected */
    } /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u) {
    /* External reference clock is selected */
    if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
      MCGOUTClock = CPU_XTAL_CLK_HZ;                                           /* System oscillator drives MCG clock */
    } else { /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
      MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                        /* RTC 32 kHz oscillator drives MCG clock */
    } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
  } else { /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
    /* Reserved value */
    return;
  } /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
  SystemCoreClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)));
}
