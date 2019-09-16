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
 * @file     am_hal_ctimer.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for accessing and configuring the CTIMER.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup ctimer Counter/Timer (CTIMER)
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_CTIMER_H
#define AM_HAL_CTIMER_H

//*****************************************************************************
//
//! @name Interrupt Status Bits
//! @brief Interrupt Status Bits for enable/disble use
//!
//! These macros may be used to set and clear interrupt bits
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_TIMERA0           AM_REG_CTIMER_INTEN_CTMRA0INT_M
#define AM_HAL_CTIMER_INT_TIMERB0           AM_REG_CTIMER_INTEN_CTMRB0INT_M
#define AM_HAL_CTIMER_INT_TIMERA1           AM_REG_CTIMER_INTEN_CTMRA1INT_M
#define AM_HAL_CTIMER_INT_TIMERB1           AM_REG_CTIMER_INTEN_CTMRB1INT_M
#define AM_HAL_CTIMER_INT_TIMERA2           AM_REG_CTIMER_INTEN_CTMRA2INT_M
#define AM_HAL_CTIMER_INT_TIMERB2           AM_REG_CTIMER_INTEN_CTMRB2INT_M
#define AM_HAL_CTIMER_INT_TIMERA3           AM_REG_CTIMER_INTEN_CTMRA3INT_M
#define AM_HAL_CTIMER_INT_TIMERB3           AM_REG_CTIMER_INTEN_CTMRB3INT_M
//! @}

//*****************************************************************************
//
//! @name Configuration options
//! @brief Configuration options for \e am_hal_ctimer_config_t
//!
//! These options are to be used with the \e am_hal_ctimer_config_t structure
//! used by \e am_hal_ctimer_config
//! @{
//
//*****************************************************************************
#define AM_HAL_CTIMER_CLK_PIN               AM_REG_CTIMER_CTRL0_TMRA0CLK(0x0)
#define AM_HAL_CTIMER_HFRC_24MHZ            AM_REG_CTIMER_CTRL0_TMRA0CLK(0x1)
#define AM_HAL_CTIMER_HFRC_3MHZ             AM_REG_CTIMER_CTRL0_TMRA0CLK(0x2)
#define AM_HAL_CTIMER_HFRC_187_5KHZ         AM_REG_CTIMER_CTRL0_TMRA0CLK(0x3)
#define AM_HAL_CTIMER_HFRC_47KHZ            AM_REG_CTIMER_CTRL0_TMRA0CLK(0x4)
#define AM_HAL_CTIMER_HFRC_12KHZ            AM_REG_CTIMER_CTRL0_TMRA0CLK(0x5)
#define AM_HAL_CTIMER_XT_32_768KHZ          AM_REG_CTIMER_CTRL0_TMRA0CLK(0x6)
#define AM_HAL_CTIMER_XT_16_384KHZ          AM_REG_CTIMER_CTRL0_TMRA0CLK(0x7)
#define AM_HAL_CTIMER_XT_2_048KHZ           AM_REG_CTIMER_CTRL0_TMRA0CLK(0x8)
#define AM_HAL_CTIMER_XT_256HZ              AM_REG_CTIMER_CTRL0_TMRA0CLK(0x9)
#define AM_HAL_CTIMER_LFRC_512HZ            AM_REG_CTIMER_CTRL0_TMRA0CLK(0xA)
#define AM_HAL_CTIMER_LFRC_32HZ             AM_REG_CTIMER_CTRL0_TMRA0CLK(0xB)
#define AM_HAL_CTIMER_LFRC_1HZ              AM_REG_CTIMER_CTRL0_TMRA0CLK(0xC)
#define AM_HAL_CTIMER_LFRC_1_16HZ           AM_REG_CTIMER_CTRL0_TMRA0CLK(0xD)
#define AM_HAL_CTIMER_RTC_100HZ             AM_REG_CTIMER_CTRL0_TMRA0CLK(0xE)
#define AM_HAL_CTIMER_HCLK                  AM_REG_CTIMER_CTRL0_TMRA0CLK(0xF)
//! @}

//*****************************************************************************
//
// Timer function macros.
//
//*****************************************************************************
#define AM_HAL_CTIMER_FN_ONCE               AM_REG_CTIMER_CTRL0_TMRA0FN(0)
#define AM_HAL_CTIMER_FN_REPEAT             AM_REG_CTIMER_CTRL0_TMRA0FN(1)
#define AM_HAL_CTIMER_FN_PWM_ONCE           AM_REG_CTIMER_CTRL0_TMRA0FN(2)
#define AM_HAL_CTIMER_FN_PWM_REPEAT         AM_REG_CTIMER_CTRL0_TMRA0FN(3)
#define AM_HAL_CTIMER_FN_CONTINUOUS         AM_REG_CTIMER_CTRL0_TMRA0FN(4)

//*****************************************************************************
//
// Half-timer options.
//
//*****************************************************************************
#define AM_HAL_CTIMER_INT_ENABLE            AM_REG_CTIMER_CTRL0_TMRA0IE_M
#define AM_HAL_CTIMER_PIN_ENABLE            AM_REG_CTIMER_CTRL0_TMRA0PE_M
#define AM_HAL_CTIMER_PIN_INVERT            AM_REG_CTIMER_CTRL0_TMRA0POL_M
#define AM_HAL_CTIMER_CLEAR                 AM_REG_CTIMER_CTRL0_TMRA0CLR_M

//*****************************************************************************
//
// Additional timer options.
//
//*****************************************************************************
#define AM_HAL_CTIMER_LINK                  AM_REG_CTIMER_CTRL0_CTLINK0_M
#define AM_HAL_CTIMER_ADC_TRIG              AM_REG_CTIMER_CTRL3_ADCEN_M

//*****************************************************************************
//
// Timer selection macros.
//
//*****************************************************************************
#define AM_HAL_CTIMER_TIMERA                0x0000FFFF
#define AM_HAL_CTIMER_TIMERB                0xFFFF0000
#define AM_HAL_CTIMER_BOTH                  0xFFFFFFFF
//! @}

//*****************************************************************************
//
// Timer configuration structure
//
//*****************************************************************************
typedef struct
{
    //
    //! Set to 1 to operate this timer as a 32-bit timer instead of two 16-bit
    //! timers.
    //
    uint32_t ui32Link;

    //
    //! Configuration options for TIMERA
    //
    uint32_t ui32TimerAConfig;

    //
    //! Configuration options for TIMERB
    //
    uint32_t ui32TimerBConfig;

}
am_hal_ctimer_config_t;

//*****************************************************************************
//
// Function pointer type for CTimer interrupt handlers.
//
//*****************************************************************************
typedef void (*am_hal_ctimer_handler_t)(void);

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_ctimer_config(uint32_t ui32TimerNumber,
                                 am_hal_ctimer_config_t *psConfig);

extern void am_hal_ctimer_config_single(uint32_t ui32TimerNumber,
                                        uint32_t ui32TimerSegment,
                                        uint32_t ui32ConfigVal);

extern void am_hal_ctimer_start(uint32_t ui32TimerNumber,
                                uint32_t ui32TimerSegment);

extern void am_hal_ctimer_stop(uint32_t ui32TimerNumber,
                               uint32_t ui32TimerSegment);

extern void am_hal_ctimer_clear(uint32_t ui32TimerNumber,
                                uint32_t ui32TimerSegment);

extern uint32_t am_hal_ctimer_read(uint32_t ui32TimerNumber,
                                   uint32_t ui32TimerSegment);

extern void am_hal_ctimer_pin_enable(uint32_t ui32TimerNumber,
                                     uint32_t ui32TimerSegment);

extern void am_hal_ctimer_pin_disable(uint32_t ui32TimerNumber,
                                      uint32_t ui32TimerSegment);

extern void am_hal_ctimer_pin_invert(uint32_t ui32TimerNumber,
                                     uint32_t ui32TimerSegment,
                                     bool bInvertOutput);

extern void am_hal_ctimer_compare_set(uint32_t ui32TimerNumber,
                                      uint32_t ui32TimerSegment,
                                      uint32_t ui32CompareReg,
                                      uint32_t ui32Value);

extern void am_hal_ctimer_period_set(uint32_t ui32TimerNumber,
                                     uint32_t ui32TimerSegment,
                                     uint32_t ui32Period,
                                     uint32_t ui32OnTime);

extern void am_hal_ctimer_adc_trigger_enable(void);
extern void am_hal_ctimer_adc_trigger_disable(void);
extern void am_hal_ctimer_int_enable(uint32_t ui32Interrupt);
extern uint32_t am_hal_ctimer_int_enable_get(void);
extern void am_hal_ctimer_int_disable(uint32_t ui32Interrupt);
extern void am_hal_ctimer_int_set(uint32_t ui32Interrupt);
extern void am_hal_ctimer_int_clear(uint32_t ui32Interrupt);
extern uint32_t am_hal_ctimer_int_status_get(bool bEnabledOnly);
extern void am_hal_ctimer_int_register(uint32_t ui32Interrupt,
                         am_hal_ctimer_handler_t pfnHandler);
extern void am_hal_ctimer_int_service(uint32_t ui32Status);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CTIMER_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
