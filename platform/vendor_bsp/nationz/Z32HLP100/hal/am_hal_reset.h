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
 * @file     am_hal_reset.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Hardware abstraction layer for the Reset Generator module.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup wdt Watchdog Timer (RSTGEN)
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_RSTGEN_H
#define AM_HAL_RSTGEN_H

//*****************************************************************************
//
//! @name Reset Generator Configuration
//! @brief These macros may be used to set the reset generator's configuration.
//! @{
//
//*****************************************************************************
#define AM_HAL_RESET_CFG_WDT_RESET_ENABLE       (AM_REG_RSTGEN_CFG_WDREN(1))
// Brown out high (2.1v) reset enable.
#define AM_HAL_RESET_CFG_BOD_HIGH_RESET_ENABLE  (AM_REG_RSTGEN_CFG_BODHREN(1))
//! @}

//*****************************************************************************
//
//! @name Reset Generator Status Bit Masks
//! @brief These macros may be used to determine which type(s) of resets have
//!        been seen.
//! @{
//
//*****************************************************************************
// Reset was initiated by a Watchdog Timer Reset.
#define AM_HAL_RESET_STAT_WDT (AM_REG_RSTGEN_STAT_WDRSTAT_M)

// Reset was a initiated by Debugger Reset.
#define AM_HAL_RESET_STAT_DEBUG (AM_REG_RSTGEN_STAT_DBGRSTAT_M)

// Reset was a initiated by Software POI Reset.
#define AM_HAL_RESET_STAT_POI (AM_REG_RSTGEN_STAT_POIRSTAT_M)

// Reset was a initiated by Software POR or AIRCR Reset.
#define AM_HAL_RESET_STAT_SOFTWARE (AM_REG_RSTGEN_STAT_SWRSTAT_M)

// Reset was initiated by a Brown-Out Reset.
#define AM_HAL_RESET_STAT_BOD (AM_REG_RSTGEN_STAT_BORSTAT_M)

// Reset was initiated by a Power Cycle
#define AM_HAL_RESET_STAT_POWER_CYCLE (AM_REG_RSTGEN_STAT_PORSTAT_M)

// Reset was initiated by an External Reset.
#define AM_HAL_RESET_STAT_EXTERNAL (AM_REG_RSTGEN_STAT_EXRSTAT_M)
//! @}

#ifdef __cplusplus
extern "C"
{
#endif
//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_reset_init(uint32_t ui32Config);
extern void am_hal_reset_por(void);
extern void am_hal_reset_poi(void);
extern uint32_t am_hal_reset_status_get(void);
extern void am_hal_reset_status_clear(void);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_RSTGEN_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
