/** ----------------------------------------------------------------------------
 *         Nationz Technology Software Support  -  NATIONZ  -
 * -----------------------------------------------------------------------------
 * Copyright (c) 2013гн2018, Nationz Corporation  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the disclaimer below in the documentation and/or
 * other materials provided with the distribution. 
 * 
 * Nationz's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission. 
 * 
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONZ "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
 * -----------------------------------------------------------------------------
 */

/** ****************************************************************************
 * @copyright      Nationz Co.,Ltd 
 *        Copyright (c) 2013гн2018 All Rights Reserved 
 *******************************************************************************
 * @file     am_reg_systick.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register macros for the SYSTICK module
 ******************************************************************************/
#ifndef AM_REG_SYSTICK_H
#define AM_REG_SYSTICK_H

//*****************************************************************************
//
// Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_SYSTICK_NUM_MODULES                   1
#define AM_REG_SYSTICKn(n) \
    (REG_SYSTICK_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
// Register offsets.
//
//*****************************************************************************
#define AM_REG_SYSTICK_SYSTCSR_O                     0xE000E010
#define AM_REG_SYSTICK_SYSTRVR_O                     0xE000E014
#define AM_REG_SYSTICK_SYSTCVR_O                     0xE000E018
#define AM_REG_SYSTICK_SYSTCALIB_O                   0xE000E01C

//*****************************************************************************
//
// SYSTICK_SYSTCSR - SysTick Control and Status Register.
//
//*****************************************************************************
// Returns 1 if timer counted to 0 since last time this was read.
#define AM_REG_SYSTICK_SYSTCSR_COUNTFLAG_S           16
#define AM_REG_SYSTICK_SYSTCSR_COUNTFLAG_M           0x00010000
#define AM_REG_SYSTICK_SYSTCSR_COUNTFLAG(n)          (((uint32_t)(n) << 16) & 0x00010000)

// Enables SysTick exception request. Software can use COUNTFLAG to determine if
// SysTick has ever counted to zero. 0 = counting down to zero does not assert
// the SysTick exception request; 1 = counting down to zero asserts the SysTick
// exception request.
#define AM_REG_SYSTICK_SYSTCSR_TICKINT_S             1
#define AM_REG_SYSTICK_SYSTCSR_TICKINT_M             0x00000002
#define AM_REG_SYSTICK_SYSTCSR_TICKINT(n)            (((uint32_t)(n) << 1) & 0x00000002)

// Enables the counter. 0 = counter disabled; 1 = counter enabled.
#define AM_REG_SYSTICK_SYSTCSR_ENABLE_S              0
#define AM_REG_SYSTICK_SYSTCSR_ENABLE_M              0x00000001
#define AM_REG_SYSTICK_SYSTCSR_ENABLE(n)             (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// SYSTICK_SYSTRVR - SysTick Reload Value Register.
//
//*****************************************************************************
// Value to load into the SYSTCVR register when the counter is enabled and when
// it reaches 0.
#define AM_REG_SYSTICK_SYSTRVR_RELOAD_S              0
#define AM_REG_SYSTICK_SYSTRVR_RELOAD_M              0x00FFFFFF
#define AM_REG_SYSTICK_SYSTRVR_RELOAD(n)             (((uint32_t)(n) << 0) & 0x00FFFFFF)

//*****************************************************************************
//
// SYSTICK_SYSTCVR - SysTick Current Value Register.
//
//*****************************************************************************
// Reads return the current value of the SysTick counter. A write of any value
// clears the field to 0, and also clears the SYSTCSR COUNTFLAG bit to 0.
#define AM_REG_SYSTICK_SYSTCVR_CURRENT_S             0
#define AM_REG_SYSTICK_SYSTCVR_CURRENT_M             0x00FFFFFF
#define AM_REG_SYSTICK_SYSTCVR_CURRENT(n)            (((uint32_t)(n) << 0) & 0x00FFFFFF)

//*****************************************************************************
//
// SYSTICK_SYSTCALIB - SysTick Calibration Value Register.
//
//*****************************************************************************
// Indicates whether the device provides a reference clock to the processor. 0 =
// reference clock provided; 1 = no reference clock provided. If your device
// does not provide a reference clock, the SYST_CSR.CLKSOURCE bit reads-as-one
// and ignores writes.
#define AM_REG_SYSTICK_SYSTCALIB_NOREF_S             31
#define AM_REG_SYSTICK_SYSTCALIB_NOREF_M             0x80000000
#define AM_REG_SYSTICK_SYSTCALIB_NOREF(n)            (((uint32_t)(n) << 31) & 0x80000000)

// Indicates whether the TENMS value is exact. 0 = TENMS value is exact; 1 =
// TENMS value is inexact, or not given. An inexact TENMS value can affect the
// suitability of SysTick as a software real time clock.
#define AM_REG_SYSTICK_SYSTCALIB_SKEW_S              30
#define AM_REG_SYSTICK_SYSTCALIB_SKEW_M              0x40000000
#define AM_REG_SYSTICK_SYSTCALIB_SKEW(n)             (((uint32_t)(n) << 30) & 0x40000000)

// Reload value for 10ms (100Hz) timing, subject to system clock skew errors. If
// the value reads as zero, the calibration value is not known.
#define AM_REG_SYSTICK_SYSTCALIB_TENMS_S             0
#define AM_REG_SYSTICK_SYSTCALIB_TENMS_M             0x00FFFFFF
#define AM_REG_SYSTICK_SYSTCALIB_TENMS(n)            (((uint32_t)(n) << 0) & 0x00FFFFFF)

#endif // AM_REG_SYSTICK_H
