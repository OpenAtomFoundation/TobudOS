/*
** ###################################################################
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          GNU C Compiler - CodeSourcery Sourcery G++
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    K22P121M120SF8RM, Rev. 1, March 24, 2014
**     Version:             rev. 1.5, 2014-04-30
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright: 2014 Freescale, Inc.
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
**     - rev. 1.0 (2013-09-17)
**         Initial version.
**     - rev. 1.1 (2013-10-29)
**         Definition of BITBAND macros updated to support peripherals with 32-bit acces disabled.
**     - rev. 1.2 (2013-12-20)
**         Update according to reference manual rev. 0.6,
**     - rev. 1.3 (2014-02-06)
**         Update according to reference manual rev. 0.61,
**     - rev. 1.4 (2014-02-10)
**         The declaration of clock configurations has been moved to separate header file system_MK22F25612.h
**     - rev. 1.5 (2014-04-30)
**         Update of MCM and USB modules according to the RM rev. 1.
**         Update of system and startup files.
**         Module access macro module_BASES replaced by module_BASE_PTRS.
**
** ###################################################################
*/

/*!
 * @file MK22F25612
 * @version 1.5
 * @date 2014-04-30
 * @brief Device specific configuration file for MK22F25612 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MK22F25612.h"



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
  /* WDOG->UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* Key 1 */
  /* WDOG->UNLOCK: WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* Key 2 */
  /* WDOG->STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,??=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = WDOG_STCTRLH_BYTESEL(0x00) |
                 WDOG_STCTRLH_WAITEN_MASK |
                 WDOG_STCTRLH_STOPEN_MASK |
                 WDOG_STCTRLH_ALLOWUPDATE_MASK |
                 WDOG_STCTRLH_CLKSRC_MASK |
                 0x0100U;
#endif /* (DISABLE_WDOG) */
#ifdef RTC_CR_VALUE
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
  if ((RTC_CR & RTC_CR_OSCE_MASK) == 0u) { /* Only if the OSCILLATOR is not already enabled */
    RTC_CR = (uint32_t)((RTC_CR & (uint32_t)~(uint32_t)(RTC_CR_SC2P_MASK | RTC_CR_SC4P_MASK | RTC_CR_SC8P_MASK | RTC_CR_SC16P_MASK)) | (uint32_t)RTC_CR_VALUE);
    RTC_CR |= (uint32_t)RTC_CR_OSCE_MASK;
    RTC_CR &= (uint32_t)~(uint32_t)RTC_CR_CLKO_MASK;
  }
#endif

  /* Power mode protection initialization */
#ifdef SMC_PMPROT_VALUE
  SMC->PMPROT = SMC_PMPROT_VALUE;
#endif

  /* High speed run mode enable */
  if (((SMC_PMCTRL_VALUE) & SMC_PMCTRL_RUNM_MASK) == SMC_PMCTRL_RUNM(0x03U)) {
    SMC->PMCTRL = (uint8_t)((SMC_PMCTRL_VALUE) & (SMC_PMCTRL_RUNM_MASK)); /* Enable HSRUN mode */
    while(SMC->PMSTAT != 0x80U) {      /* Wait until the system is in HSRUN mode */
    }
  }
  /* System clock initialization */
  /* Internal reference clock trim initialization */
#if defined(SLOW_TRIM_ADDRESS)
  if ( *((uint8_t*)SLOW_TRIM_ADDRESS) != 0xFFU) {                              /* Skip if non-volatile flash memory is erased */
    MCG->C3 = *((uint8_t*)SLOW_TRIM_ADDRESS);
  #endif /* defined(SLOW_TRIM_ADDRESS) */
  #if defined(SLOW_FINE_TRIM_ADDRESS)
    MCG->C4 = (MCG->C4 & ~(MCG_C4_SCFTRIM_MASK)) | ((*((uint8_t*) SLOW_FINE_TRIM_ADDRESS)) & MCG_C4_SCFTRIM_MASK);
  #endif
  #if defined(FAST_TRIM_ADDRESS)
    MCG->C4 = (MCG->C4 & ~(MCG_C4_FCTRIM_MASK)) |((*((uint8_t*) FAST_TRIM_ADDRESS)) & MCG_C4_FCTRIM_MASK);
  #endif
  #if defined(FAST_FINE_TRIM_ADDRESS)
    MCG->C2 = (MCG->C2 & ~(MCG_C2_FCFTRIM_MASK)) | ((*((uint8_t*)FAST_TRIM_ADDRESS)) & MCG_C2_FCFTRIM_MASK);
  #endif /* defined(FAST_FINE_TRIM_ADDRESS) */
#if defined(SLOW_TRIM_ADDRESS)
  }
  #endif /* defined(SLOW_TRIM_ADDRESS) */

  /* Set system prescalers and clock sources */
  SIM->CLKDIV1 = SIM_CLKDIV1_VALUE;    /* Set system prescalers */
  SIM->SOPT1 = ((SIM->SOPT1) & (uint32_t)(~(SIM_SOPT1_OSC32KSEL_MASK))) | ((SIM_SOPT1_VALUE) & (SIM_SOPT1_OSC32KSEL_MASK)); /* Set 32 kHz clock source (ERCLK32K) */
  SIM->SOPT2 = ((SIM->SOPT2) & (uint32_t)(~(SIM_SOPT2_PLLFLLSEL_MASK))) | ((SIM_SOPT2_VALUE) & (SIM_SOPT2_PLLFLLSEL_MASK)); /* Selects the high frequency clock for various peripheral clocking options. */
#if (defined(MCG_MODE_FEI) || defined(MCG_MODE_FBI) || defined(MCG_MODE_BLPI))
  /* Set MCG and OSC */
#if  ((((OSC_CR_VALUE) & OSC_CR_ERCLKEN_MASK) != 0x00) || ((((MCG_C5_VALUE) & MCG_C5_PLLCLKEN0_MASK) != 0x00) && (((MCG_C7_VALUE) & MCG_C7_OSCSEL_MASK) == 0x00)))
  /* SIM_SCGC5: PORTA=1 */
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
  /* PORTA_PCR18: ISF=0,MUX=0 */
  PORTA_PCR18 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
  if (((MCG_C2_VALUE) & MCG_C2_EREFS_MASK) != 0x00) {
  /* PORTA_PCR19: ISF=0,MUX=0 */
  PORTA_PCR19 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
  }
#endif
  MCG->SC = MCG_SC_VALUE;              /* Set SC (fast clock internal reference divider) */
  MCG->C1 = MCG_C1_VALUE;              /* Set C1 (clock source selection, FLL ext. reference divider, int. reference enable etc.) */
  /* Check that the source of the FLL reference clock is the requested one. */
  if (((MCG_C1_VALUE) & MCG_C1_IREFS_MASK) != 0x00U) {
    while((MCG->S & MCG_S_IREFST_MASK) == 0x00U) {
    }
  } else {
    while((MCG->S & MCG_S_IREFST_MASK) != 0x00U) {
    }
  }
  MCG->C2 = (MCG->C2 & (uint8_t)(~(MCG_C2_FCFTRIM_MASK))) | (MCG_C2_VALUE & (uint8_t)(~(MCG_C2_LP_MASK))); /* Set C2 (freq. range, ext. and int. reference selection etc. excluding trim bits; low power bit is set later) */
  MCG->C4 = ((MCG_C4_VALUE) & (uint8_t)(~(MCG_C4_FCTRIM_MASK | MCG_C4_SCFTRIM_MASK))) | (MCG->C4 & (MCG_C4_FCTRIM_MASK | MCG_C4_SCFTRIM_MASK)); /* Set C4 (FLL output; trim values not changed) */
  OSC->CR = OSC_CR_VALUE;              /* Set OSC_CR (OSCERCLK enable, oscillator capacitor load) */
  MCG->C7 = MCG_C7_VALUE;              /* Set C7 (OSC Clock Select) */
  #if defined(MCG_MODE_BLPI)
  /* BLPI specific */
  MCG->C2 |= (MCG_C2_LP_MASK);         /* Disable FLL and PLL in bypass mode */
  #endif

#else /* MCG_MODE */
  /* Set MCG and OSC */
#if  (((OSC_CR_VALUE) & OSC_CR_ERCLKEN_MASK) != 0x00) || (((MCG_C7_VALUE) & MCG_C7_OSCSEL_MASK) == 0x00)
  /* SIM_SCGC5: PORTA=1 */
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
  /* PORTA_PCR18: ISF=0,MUX=0 */
  PORTA_PCR18 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
  if (((MCG_C2_VALUE) & MCG_C2_EREFS_MASK) != 0x00) {
  /* PORTA_PCR19: ISF=0,MUX=0 */
  PORTA_PCR19 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
  }
#endif
  MCG->SC = MCG_SC_VALUE;              /* Set SC (fast clock internal reference divider) */
  MCG->C2 = (MCG->C2 & (uint8_t)(~(MCG_C2_FCFTRIM_MASK))) | (MCG_C2_VALUE & (uint8_t)(~(MCG_C2_LP_MASK))); /* Set C2 (freq. range, ext. and int. reference selection etc. excluding trim bits; low power bit is set later) */
  OSC->CR = OSC_CR_VALUE;              /* Set OSC_CR (OSCERCLK enable, oscillator capacitor load) */
  MCG->C7 = MCG_C7_VALUE;              /* Set C7 (OSC Clock Select) */
  #if defined(MCG_MODE_PEE)
  MCG->C1 = (MCG_C1_VALUE) | MCG_C1_CLKS(0x02); /* Set C1 (clock source selection, FLL ext. reference divider, int. reference enable etc.) - PBE mode*/
  #else
  MCG->C1 = MCG_C1_VALUE;              /* Set C1 (clock source selection, FLL ext. reference divider, int. reference enable etc.) */
  #endif
  if (((MCG_C2_VALUE) & MCG_C2_EREFS_MASK) != 0U) {
    while((MCG->S & MCG_S_OSCINIT0_MASK) == 0x00U) { /* Check that the oscillator is running */
    }
  }
  /* Check that the source of the FLL reference clock is the requested one. */
  if (((MCG_C1_VALUE) & MCG_C1_IREFS_MASK) != 0x00U) {
    while((MCG->S & MCG_S_IREFST_MASK) == 0x00U) {
    }
  } else {
    while((MCG->S & MCG_S_IREFST_MASK) != 0x00U) {
    }
  }
  MCG->C4 = ((MCG_C4_VALUE)  & (uint8_t)(~(MCG_C4_FCTRIM_MASK | MCG_C4_SCFTRIM_MASK))) | (MCG->C4 & (MCG_C4_FCTRIM_MASK | MCG_C4_SCFTRIM_MASK)); /* Set C4 (FLL output; trim values not changed) */
#endif /* MCG_MODE */

  /* Common for all MCG modes */

  /* PLL clock can be used to generate clock for some devices regardless of clock generator (MCGOUTCLK) mode. */
  MCG->C5 = (MCG_C5_VALUE) & (uint8_t)(~(MCG_C5_PLLCLKEN0_MASK)); /* Set C5 (PLL settings, PLL reference divider etc.) */
  MCG->C6 = (MCG_C6_VALUE) & (uint8_t)~(MCG_C6_PLLS_MASK); /* Set C6 (PLL select, VCO divider etc.) */
  if ((MCG_C5_VALUE) & MCG_C5_PLLCLKEN0_MASK) {
    MCG->C5 |= MCG_C5_PLLCLKEN0_MASK;  /* PLL clock enable in mode other than PEE or PBE */
  }
  /* BLPE, PEE and PBE MCG mode specific */

#if defined(MCG_MODE_BLPE)
  MCG->C2 |= (MCG_C2_LP_MASK);         /* Disable FLL and PLL in bypass mode */
#elif (defined(MCG_MODE_PBE) || defined(MCG_MODE_PEE))
  MCG->C6 |= (MCG_C6_PLLS_MASK);       /* Set C6 (PLL select, VCO divider etc.) */
  while((MCG->S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until PLL is locked*/
  }
  #if defined(MCG_MODE_PEE)
  MCG->C1 &= (uint8_t)~(MCG_C1_CLKS_MASK);
  #endif
#endif
#if (defined(MCG_MODE_FEI) || defined(MCG_MODE_FEE))
  while((MCG->S & MCG_S_CLKST_MASK) != 0x00U) { /* Wait until output of the FLL is selected */
  }
#elif (defined(MCG_MODE_FBI) || defined(MCG_MODE_BLPI))
  while((MCG->S & MCG_S_CLKST_MASK) != 0x04U) { /* Wait until internal reference clock is selected as MCG output */
  }
#elif (defined(MCG_MODE_FBE) || defined(MCG_MODE_PBE) || defined(MCG_MODE_BLPE))
  while((MCG->S & MCG_S_CLKST_MASK) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
  }
#elif defined(MCG_MODE_PEE)
  while((MCG->S & MCG_S_CLKST_MASK) != 0x0CU) { /* Wait until output of the PLL is selected */
  }
#endif
  if (((SMC_PMCTRL_VALUE) & SMC_PMCTRL_RUNM_MASK) == SMC_PMCTRL_RUNM(0x02U)) {
    SMC->PMCTRL = (uint8_t)((SMC_PMCTRL_VALUE) & (SMC_PMCTRL_RUNM_MASK)); /* Enable VLPR mode */
    while(SMC->PMSTAT != 0x04U) {      /* Wait until the system is in VLPR mode */
    }
  }
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {

  uint32_t MCGOUTClock;                                                        /* Variable to store output clock frequency of the MCG module */
  uint16_t Divider;

  if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) {
    /* Output of FLL or PLL is selected */
    if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) {
      /* FLL is selected */
      if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) {
        /* External reference clock is selected */
        switch (MCG->C7 & MCG_C7_OSCSEL_MASK) {
        case 0x00u:
          MCGOUTClock = CPU_XTAL_CLK_HZ;                                       /* System oscillator drives MCG clock */
          break;
        case 0x01u:
          MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                    /* RTC 32 kHz oscillator drives MCG clock */
          break;
        case 0x02u:
        default:
          MCGOUTClock = CPU_INT_IRC_CLK_HZ;                                              /* IRC 48MHz oscillator drives MCG clock */
        }
        if ((MCG->C2 & MCG_C2_RANGE_MASK) != 0x0u) {
          switch (MCG->C1 & MCG_C1_FRDIV_MASK) {
          case MCG_C1_FRDIV(0x07):
            Divider = 1536;
            break;
          case MCG_C1_FRDIV(0x06):
            Divider = 1280;
            break;
          default:
            Divider = (uint16_t)(32u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
          }
        } else {/* ((MCG->C2 & MCG_C2_RANGE_MASK) != 0x0u) */
          Divider = (uint16_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
        }
        MCGOUTClock = (MCGOUTClock / Divider);  /* Calculate the divided FLL reference clock */
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
    switch (MCG->C7 & MCG_C7_OSCSEL_MASK) {
    case 0x00u:
      MCGOUTClock = CPU_XTAL_CLK_HZ;                                           /* System oscillator drives MCG clock */
      break;
    case 0x01u:
      MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                        /* RTC 32 kHz oscillator drives MCG clock */
      break;
    case 0x02u:
    default:
      MCGOUTClock = CPU_INT_IRC_CLK_HZ;                                              /* IRC 48MHz oscillator drives MCG clock */
    }
  } else { /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
    /* Reserved value */
    return;
  } /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
  SystemCoreClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)));

}
