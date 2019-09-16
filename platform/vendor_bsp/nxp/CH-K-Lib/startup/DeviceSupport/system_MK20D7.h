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
 * @brief Device specific configuration file for MK20D7 (header file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#ifndef SYSTEM_MK20D7_H_
#define SYSTEM_MK20D7_H_                         /**< Symbol preventing repeated inclusion */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
extern uint32_t SystemCoreClock;

/**
 * @brief Setup the microcontroller system.
 *
 * Typically this function configures the oscillator (PLL) that is part of the
 * microcontroller device. For systems with variable clock speed it also updates
 * the variable SystemCoreClock. SystemInit is called from startup_device file.
 */
void SystemInit (void);

/**
 * @brief Updates the SystemCoreClock variable.
 *
 * It must be called whenever the core clock is changed during program
 * execution. SystemCoreClockUpdate() evaluates the clock register settings and calculates
 * the current core clock.
 */
void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif  /* #if !defined(SYSTEM_MK20D7_H_) */
