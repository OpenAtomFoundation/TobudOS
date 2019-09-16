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
 * @file     am_hal_itm.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for accessing and configuring the ARM ITM.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup itm Instrumentation Trace Macrocell (ITM)
 * @ingroup hal
 * @{
 ******************************************************************************/

#ifndef AM_HAL_ITM_H
#define AM_HAL_ITM_H

//*****************************************************************************
//
// Sync Packet Defines
//
//*****************************************************************************
#define AM_HAL_ITM_SYNC_REG             23
#define AM_HAL_ITM_SYNC_VAL             0xF8F8F8F8

//*****************************************************************************
//
// PrintF Setup
//
//*****************************************************************************
#define AM_HAL_ITM_PRINT_NUM_BYTES      1
#define AM_HAL_ITM_PRINT_NUM_REGS       1

extern uint32_t am_hal_itm_print_registers[AM_HAL_ITM_PRINT_NUM_REGS];

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_itm_delay_us(uint32_t ui32MicroSeconds);
extern void am_hal_itm_enable(void);
extern void am_hal_itm_disable(void);
extern void am_hal_itm_not_busy(void);
extern void am_hal_itm_sync_send(void);
extern void am_hal_itm_trace_port_enable(uint8_t ui8portNum);
extern void am_hal_itm_trace_port_disable(uint8_t ui8portNum);
extern bool am_hal_itm_stimulus_not_busy(uint32_t ui32StimReg);
extern void am_hal_itm_stimulus_reg_word_write(uint32_t ui32StimReg,
                                                uint32_t ui32Value);
extern void am_hal_itm_stimulus_reg_short_write(uint32_t ui32StimReg,
                                                uint16_t ui16Value);
extern void am_hal_itm_stimulus_reg_byte_write(uint32_t ui32StimReg,
                                                uint8_t ui8Value);
extern bool am_hal_itm_print_not_busy(void);
extern void am_hal_itm_print(char *pcString);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_ITM_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
