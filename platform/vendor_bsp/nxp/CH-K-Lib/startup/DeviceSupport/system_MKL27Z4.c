/*
** ###################################################################
**     Processors:          MKL27Z256VFM4
**                          MKL27Z128VFM4
**                          MKL27Z256VFT4
**                          MKL27Z128VFT4
**                          MKL27Z256VLH4
**                          MKL27Z128VLH4
**                          MKL27Z256VMP4
**                          MKL27Z128VMP4
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          GNU C Compiler - CodeSourcery Sourcery G++
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    KL27P64M48SF6RM, Rev.3, Aug 2014
**     Version:             rev. 1.3, 2014-08-21
**     Build:               b140821
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MKL27Z4
**
**     Copyright (c) 1997 - 2014 Freescale Semiconductor, Inc.
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
**     - rev. 1.0 (2014-03-27)
**         Initial version.
**     - rev. 1.1 (2014-05-26)
**         I2S registers TCR2/RCR2 and others were changed.
**         FLEXIO register FLEXIO_VERID has now bitfields: FEATURE, MINOR, MAJOR.
**         Names of the bitfields of the FLEXIO_SHIFTBUF have been changed to the appropriate register name e.g.: FLEXIO_SHIFTBUFBBS_SHIFTBUFBBS.
**         Peripheral_BASES macros has been changed to Peripheral_BASE_PTRS, e.g.: ADC_BASES to ADC_BASE_PTRS.
**         Clock configuration for high range external oscillator has been added.
**         RFSYS module access has been added.
**     - rev. 1.2 (2014-07-10)
**         GPIO - Renamed modules PTA,PTB,PTC,PTD,PTE to GPIOA,GPIOB,GPIOC,GPIOD,GPIOE.
**         UART0 - UART0 module renamed to UART2.
**         I2S - removed MDR register.
**     - rev. 1.3 (2014-08-21)
**         UART2 - Removed ED register.
**         UART2 - Removed MODEM register.
**         UART2 - Removed IR register.
**         UART2 - Removed PFIFO register.
**         UART2 - Removed CFIFO register.
**         UART2 - Removed SFIFO register.
**         UART2 - Removed TWFIFO register.
**         UART2 - Removed TCFIFO register.
**         UART2 - Removed RWFIFO register.
**         UART2 - Removed RCFIFO register.
**         USB - Removed bitfield REG_EN in CLK_RECOVER_IRC_EN register.
**         SIM - Changed bitfield value MCGIRCLK to LIRC_CLK of bitfield CLKOUTSEL in SOPT2 register.
**         SIM - Removed bitfield DIEID in SDID register.
**
** ###################################################################
*/

/*!
 * @file MKL27Z4
 * @version 1.3
 * @date 2014-08-21
 * @brief Device specific configuration file for MKL27Z4 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MKL27Z4.h"



/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) {

#if (DISABLE_WDOG)
  /* Disable the COP module */
  /* SIM_COPC: COPCLKSEL=0,COPDBGEN=0,COPSTPEN=0,COPT=0,COPCLKS=0,COPW=0 */
  SIM->COPC = (uint32_t)0x00u;
#endif /* (DISABLE_WDOG) */
#if (CLOCK_SETUP == 0)
  /* SIM->SOPT2: USBSRC=0 */
  SIM->SOPT2 &= ~SIM_SOPT2_USBSRC_MASK; /* USB_CLKIN is clock source for USB FS  (applicable only for derivatived with USB)*/
  /* SIM->CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                 SIM_CLKDIV1_OUTDIV4(0x01); /* Update system prescalers */
  /* MCG->SC: FCRDIV=1 */
  MCG->SC = MCG_SC_FCRDIV(0x01);       /* Set the LIRC1 divider*/
  /* MCG->MC: HIRC=0,LIRC_DIV2=0 */
  MCG->MC = MCG_MC_LIRC_DIV2(0x00);    /* Set the LIRC2 divider*/
  /* OSC0->CR: ERCLKEN=0,EREFSTEN=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0->CR = (uint32_t)0x00u;         /* Disable External reference */
  /* MCG->C2: RANGE0=0,HGO0=0,EREFS0=0,IRCS=1 */
  MCG->C2 = MCG_C2_IRCS_MASK;         /* Enable LIRC 8MHz */
  /* Switch to LIRC 8MHz Mode */
  /* MCG->C1: CLKS=1,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = MCG_C1_CLKS(0x01) |
            MCG_C1_IRCLKEN_MASK;       /* Enable LIRC and select LIRC as a clock source */
  while((MCG->S & MCG_S_CLKST_MASK) != 0x04u) {} /* Check that the clock source is the LIRC clock. */
#elif (CLOCK_SETUP == 1)
  /* SIM->SOPT2: USBSRC=0 */
  SIM->SOPT2 &= ~SIM_SOPT2_USBSRC_MASK; /* USB_CLKIN is clock source for USB FS  (applicable only for derivatived with USB)*/
  /* SIM->CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                 SIM_CLKDIV1_OUTDIV4(0x01); /* Set system prescalers */
  /* MCG->SC: FCRDIV=1 */
  MCG->SC = MCG_SC_FCRDIV(0x01);       /* LIRC1 divider not used - leave the default value*/
  /* MCG->MC: HIRC=1,LIRC_DIV2=0 */
  MCG->MC = MCG_MC_HIRCEN_MASK;    /* Enable HIRC clock source*/
  /* OSC0->CR: ERCLKEN=0,EREFSTEN=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0->CR = (uint32_t)0x00u;         /* Disable External reference */
  /* MCG->C2: RANGE0=0,HGO0=0,EREFS0=0,IRCS=1 */
  MCG->C2 = MCG_C2_IRCS_MASK;         /* Not used - leave default value */
  /* Switch to HIRC Mode */
  /* MCG->C1: CLKS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = MCG_C1_CLKS(0x00) |
            MCG_C1_IRCLKEN_MASK;       /* Leave LIRC enabled and select HIRC as a clock source */
  while((MCG->S & MCG_S_CLKST_MASK) != 0x00u) {} /* Check that the clock source is the HIRC clock. */
#elif (CLOCK_SETUP == 2)
  /* SIM->SOPT2: USBSRC=0 */
  SIM->SOPT2 &= ~SIM_SOPT2_USBSRC_MASK; /* USB_CLKIN is clock source for USB FS  (applicable only for derivatived with USB)*/
  /* SIM->SCGC5: PORTA=1 */
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; /* Enable clock gate for port to enable pin routing */
  /* SIM->CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                 SIM_CLKDIV1_OUTDIV4(0x00); /* Set system prescalers */
  /* PORTA_PCR18: ISF=0,MUX=0 */
  PORTA->PCR[18] &= (uint32_t)~(uint32_t)(PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07));
  /* PORTA_PCR19: ISF=0,MUX=0 */
  PORTA->PCR[19] &= (uint32_t)~(uint32_t)(PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07));
  /* MCG->SC: FCRDIV=1 */
  MCG->SC = MCG_SC_FCRDIV(0x01);       /* LIRC1 divider not used - leave the default value*/
  /* MCG->MC: HIRC=0,LIRC_DIV2=0 */
  MCG->MC = MCG_MC_LIRC_DIV2(0x00);    /* Not used - leave the default value */
  /* MCG->C2: RANGE0=0,HGO0=0,EREFS0=1,IRCS=1 */
  MCG->C2 = MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK;   /* Select external crystal, low range, low power, for LIRC - leave default value */
  /* OSC0->CR: ERCLKEN=1,EREFSTEN=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0->CR = OSC_CR_ERCLKEN_MASK;         /* Enable External reference */
  /* Switch to EXT Mode */
  /* MCG->C1: CLKS=0,IRCLKEN=0,IREFSTEN=0 */
  MCG->C1 = MCG_C1_CLKS(0x02);       /* Disable LIRC and select EXT as a clock source */
  while((MCG->S & MCG_S_CLKST_MASK) != 0x08u) {} /* Check that the clock source is the EXT clock. */
#elif (CLOCK_SETUP == 3)
  /* SIM->SOPT2: USBSRC=0 */
  SIM->SOPT2 &= ~SIM_SOPT2_USBSRC_MASK; /* USB_CLKIN is clock source for USB FS  (applicable only for derivatived with USB)*/
  /* MCG->MC: HIRC=1 */
  MCG->MC |= MCG_MC_HIRCEN_MASK;    /* Enable HIRC clock source*/
  /* MCG->C1: CLKS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = MCG_C1_CLKS(0x00) |
            MCG_C1_IRCLKEN_MASK;       /* Leave LIRC enabled and select HIRC as a clock source */
  while((MCG->S & MCG_S_CLKST_MASK) != 0x00u) {} /* Check that the clock source is the HIRC clock. */
  /* SIM->CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                 SIM_CLKDIV1_OUTDIV4(0x01); /* Set system prescalers */
  /* MCG->SC: FCRDIV=0 */
  MCG->SC = MCG_SC_FCRDIV(0x00);       /* Set the LIRC1 divider to 1*/
  /* MCG->MC: HIRC=0,LIRC_DIV2=0 */
  MCG->MC = MCG_MC_LIRC_DIV2(0x00);    /* Set the LIRC2 divider to 1 */
  /* OSC0->CR: ERCLKEN=0,EREFSTEN=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0->CR = (uint32_t)0x00u;         /* Disable External reference */
  /* MCG->C2: RANGE0=0,HGO0=0,EREFS0=0,IRCS=0 */
  MCG->C2 = (uint32_t)0x00u;         /* Enable LIRC 2MHz */
  /* Switch to LIRC 2MHz Mode */
  /* MCG->C1: CLKS=1,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = MCG_C1_CLKS(0x01) |
            MCG_C1_IRCLKEN_MASK;       /* Enable LIRC and select LIRC as a clock source */
  while((MCG->S & MCG_S_CLKST_MASK) != 0x04u) {} /* Check that the clock source is the LIRC clock. */
#elif (CLOCK_SETUP == 4)
  /* SIM->SOPT2: USBSRC=1 */
  SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK; /* Internal 48 MHz oscillator (IRC48) is clock source for USB FS */
  /* SIM->CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                 SIM_CLKDIV1_OUTDIV4(0x01); /* Set system prescalers */
  /* MCG->SC: FCRDIV=1 */
  MCG->SC = MCG_SC_FCRDIV(0x01);       /* LIRC1 divider not used - leave the default value*/
  /* MCG->MC: HIRC=1,LIRC_DIV2=0 */
  MCG->MC = MCG_MC_HIRCEN_MASK;    /* Enable HIRC clock source*/
  /* OSC0->CR: ERCLKEN=0,EREFSTEN=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0->CR = (uint32_t)0x00u;         /* Disable External reference */
  /* MCG->C2: RANGE0=0,HGO0=0,EREFS0=0,IRCS=1 */
  MCG->C2 = MCG_C2_IRCS_MASK;         /* Not used - leave default value */
  /* Switch to HIRC Mode */
  /* MCG->C1: CLKS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = MCG_C1_CLKS(0x00) |
            MCG_C1_IRCLKEN_MASK;       /* Leave LIRC enabled and select HIRC as a clock source */
  while((MCG->S & MCG_S_CLKST_MASK) != 0x00u) {} /* Check that the clock source is the HIRC clock. */
#elif (CLOCK_SETUP == 5)
  /* SIM->SOPT2: USBSRC=0 */
  SIM->SOPT2 &= ~SIM_SOPT2_USBSRC_MASK; /* USB_CLKIN is clock source for USB FS  (applicable only for derivatived with USB)*/
  /* SIM->SCGC5: PORTA=1 */
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; /* Enable clock gate for port to enable pin routing */
  /* SIM->CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                 SIM_CLKDIV1_OUTDIV4(0x01); /* Set system prescalers */
  /* PORTA_PCR18: ISF=0,MUX=0 */
  PORTA->PCR[18] &= (uint32_t)~(uint32_t)(PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07));
  /* PORTA_PCR19: ISF=0,MUX=0 */
  PORTA->PCR[19] &= (uint32_t)~(uint32_t)(PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07));
  /* MCG->SC: FCRDIV=1 */
  MCG->SC = MCG_SC_FCRDIV(0x01);       /* LIRC1 divider not used - leave the default value*/
  /* MCG->MC: HIRC=0,LIRC_DIV2=0 */
  MCG->MC = MCG_MC_LIRC_DIV2(0x00);    /* Not used - leave the default value */
  /* MCG->C2: RANGE0=1,HGO0=0,EREFS0=1,IRCS=1 */
  MCG->C2 = MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK | MCG_C2_RANGE0(1);   /* Select external crystal, high range, low power, for LIRC - leave default value */
  /* OSC0->CR: ERCLKEN=1,EREFSTEN=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0->CR = OSC_CR_ERCLKEN_MASK;         /* Enable External reference */
  /* Switch to EXT Mode */
  /* MCG->C1: CLKS=0,IRCLKEN=0,IREFSTEN=0 */
  MCG->C1 = MCG_C1_CLKS(0x02);       /* Disable LIRC and select EXT as a clock source */
  while((MCG->S & MCG_S_CLKST_MASK) != 0x08u) {} /* Check that the clock source is the EXT clock. */
#endif

}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {

  uint32_t ICSOUTClock;                                                        /* Variable to store output clock frequency of the ICS module */
  uint8_t Divider;

  if ((MCG->S & MCG_S_CLKST_MASK) == 0x04u) {
    /* LIRC reference clock is selected */
    ICSOUTClock = CPU_INT_SLOW_CLK_HZ;
    Divider = (uint8_t)(1u << ((MCG->SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT));
    ICSOUTClock = (ICSOUTClock / Divider);  /* Calculate the divided LIRC clock */
  } else if ((MCG->S & MCG_S_CLKST_MASK) == 0x0u) {
    /* HIRC reference clock is selected */
    ICSOUTClock = CPU_INT_FAST_CLK_HZ;
  } else if ((MCG->S & MCG_S_CLKST_MASK) == 0x80u) {
    /* External reference clock is selected */
    ICSOUTClock = CPU_XTAL_CLK_HZ;
  } else {
    /* Reserved value */
    return;
  }
  SystemCoreClock = (ICSOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)));

}
