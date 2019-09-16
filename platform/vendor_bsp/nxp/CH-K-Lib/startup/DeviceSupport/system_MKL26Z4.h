/*
** ###################################################################
**     Processors:          MKL26Z128VFM4
**                          MKL26Z64VFM4
**                          MKL26Z32VM4
**                          MKL26Z128VFT4
**                          MKL26Z64VFT4
**                          MKL26Z32VFT4
**                          MKL26Z256VLH4
**                          MKL26Z128VLH4
**                          MKL26Z64VLH4
**                          MKL26Z32VLH4
**                          MKL26Z256VLK4
**                          MKL26Z256VLL4
**                          MKL26Z128VLL4
**                          MKL26Z256VMC4
**                          MKL26Z128VMC4
**
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    KL26P121M48SF4RM, Rev.2, Dec 2012
**     Version:             rev. 1.2, 2013-04-12
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright: 2013 Freescale, Inc. All Rights Reserved.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2012-12-12)
**         Initial version.
**     - rev. 1.1 (2013-04-05)
**         Changed start of doxygen comment.
**     - rev. 1.2 (2013-04-12)
**         SystemInit function fixed for clock configuration 1.
**         Name of the interrupt num. 31 updated to reflect proper function.
**
** ###################################################################
*/

/*!
 * @file MKL26Z4
 * @version 1.2
 * @date 2013-04-12
 * @brief Device specific configuration file for MKL26Z4 (header file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#ifndef SYSTEM_MKL26Z4_H_
#define SYSTEM_MKL26Z4_H_                        /**< Symbol preventing repeated inclusion */

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

#endif  /* #if !defined(SYSTEM_MKL26Z4_H_) */
