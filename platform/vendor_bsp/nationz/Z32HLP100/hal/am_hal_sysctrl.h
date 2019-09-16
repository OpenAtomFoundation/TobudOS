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
 * @file     am_hal_sysctrl.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for interfacing with the M4F system control registers
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup sysctrl System Control (SYSCTRL)
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_SYSCTRL_H
#define AM_HAL_SYSCTRL_H

//*****************************************************************************
//
// Definitions for sleep mode parameter
//
//*****************************************************************************
#define AM_HAL_SYSCTRL_SLEEP_DEEP       true
#define AM_HAL_SYSCTRL_SLEEP_NORMAL     false

//*****************************************************************************
//
// Macros to make intrinsics a little easier.
//
//*****************************************************************************
#if defined(__ARMCC_VERSION)
#define am_wfi() __wfi();

#elif defined(__IAR_SYSTEMS_ICC__)
#define am_wfi() asm("    wfi")

#else
#define am_wfi() __asm("    wfi")

#endif

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_sysctrl_sleep(bool bSleepDeep);
extern void am_hal_sysctrl_sleep_auto(void);
extern void am_hal_sysctrl_fpu_enable(void);
extern void am_hal_sysctrl_fpu_disable(void);
extern void am_hal_sysctrl_fpu_stacking_enable(bool bLazy);
extern void am_hal_sysctrl_fpu_stacking_disable(void);
extern void am_hal_sysctrl_aircr_reset(void);
#ifdef __cplusplus
}
#endif

#endif // AM_HAL_SYSCTRL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

