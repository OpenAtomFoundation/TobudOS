/*
** ###################################################################
**     Processors:          MK20DX64VLH7
**                          MK20DX128VLH7
**                          MK20DX256VLH7
**                          MK20DX64VLK7
**                          MK20DX128VLK7
**                          MK20DX256VLK7
**                          MK20DX128VLL7
**                          MK20DX256VLL7
**                          MK20DX64VMB7
**                          MK20DX128VMB7
**                          MK20DX256VMB7
**                          MK20DX128VML7
**                          MK20DX256VML7
**
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    Kxx (P1 silicon) Sub-Family Reference Manual Rev. 0, draft A Oct 2011
**     Version:             rev. 1.0, 2012-01-15
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright: 2012 Freescale Semiconductor, Inc. All Rights Reserved.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2012-01-15)
**         Initial public version.
**
** ###################################################################
*/

/**
 * @file MK20D7
 * @version 1.0
 * @date 2012-01-15
 * @brief Device specific configuration file for MK20D7 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MK20D7.h"

  #define DEFAULT_SYSTEM_CLOCK            20971520u /* Default System clock value */
  #define DISABLE_WDOG                            1

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) {
#if (DISABLE_WDOG)  // 15.02.2012 Added by ARM 
  /* Disable the WDOG module */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = (uint16_t)0xC520u;     /* Key 1 */
  /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK  = (uint16_t)0xD928u;    /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {
}
