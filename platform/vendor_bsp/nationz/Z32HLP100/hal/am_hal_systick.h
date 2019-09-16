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
 * @file     am_hal_systick.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for accessing and configuring the SYSTICK.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup systick System Timer (SYSTICK)
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_SYSTICK_H
#define AM_HAL_SYSTICK_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_systick_start(void);
extern void am_hal_systick_stop(void);
extern void am_hal_systick_int_enable(void);
extern void am_hal_systick_int_disable(void);
extern uint32_t am_hal_systick_int_status_get(void);
extern void am_hal_systick_reset(void);
extern void am_hal_systick_load(uint32_t ui32LoadVal);
extern uint32_t am_hal_systick_count(void);
extern uint32_t am_hal_systick_wait_ticks(uint32_t u32Ticks);
extern uint32_t am_hal_systick_delay_us(uint32_t u32NumUs);

void am_hal_systick_ms_config(void);
#ifdef __cplusplus
}
#endif

#endif // AM_HAL_SYSTICK_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
