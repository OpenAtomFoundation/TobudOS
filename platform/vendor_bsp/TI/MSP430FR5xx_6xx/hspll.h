/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// hspll.h - Driver for the HSPLL Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_HSPLL_H__
#define __MSP430WARE_HSPLL_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_HSPLL__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// The following are values that can be returned by the 
// HSPLL_getInterruptStatus(), HSPLL_getInterruptMaskStatus() API
//
//*****************************************************************************
#define HSPLL_PLL_STATE_UNCHANGED               PLLUNLOCK_0
#define HSPLL_PLL_STATE_CHANGED_LOCK_TO_UNLOCK  PLLUNLOCK_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: HSPLL_init(); the frequency parameter for
// functions: HSPLL_init().

//
//*****************************************************************************
#define HSPLL_LESSER_OR_EQUAL_TO_6MHZ           PLLINFREQ_0
#define HSPLL_GREATER_THAN_6MHZ                 PLLINFREQ_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: HSPLL_init(); the lockStatus parameter for
// functions: HSPLL_init(). As well as returned by the 
// HSPLL_isLocked() function.
//
//*****************************************************************************
#define HSPLL_UNLOCKED                          PLL_LOCK_0
#define HSPLL_LOCKED                            PLL_LOCK_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: HSPLL_xtalInit(); the oscillatorType parameter for
// functions: HSPLL_xtalInit().
//
//*****************************************************************************
#define HSPLL_XTAL_GATING_COUNTER_LENGTH_4096   OSCTYPE_0
#define HSPLL_XTAL_GATING_COUNTER_LENGTH_512    OSCTYPE_1
#define HSPLL_XTAL_OSCTYPE_XTAL                 OSCTYPE__XTAL
#define HSPLL_XTAL_OSCTYPE_CERAMIC              OSCTYPE__CERAMIC

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: HSPLL_xtalInit(); the xtlOutput parameter for
// functions: HSPLL_xtalInit().
//
//*****************************************************************************
#define HSPLL_XTAL_OUTPUT_DISABLE               XTOUTOFF_1
#define HSPLL_XTAL_OUTPUT_ENABLE                XTOUTOFF_0

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: HSPLL_xtalInit(); the oscillatorEnable parameter for
// functions: HSPLL_xtalInit().
//
//*****************************************************************************
#define HSPLL_XTAL_ENABLE                       USSXTEN_1
#define HSPLL_XTAL_DISABLE                      USSXTEN_0

//*****************************************************************************
//
// The following are values that can be returned by HSPLL_getOscillatorStatus()
//
//*****************************************************************************
#define HSPLL_OSCILLATOR_STABILIZED             OSCSTATE_0
#define HSPLL_OSCILLATOR_NOT_STABILIZED         OSCSTATE_1
#define HSPLL_OSCILLATOR_NOT_STARTED            OSCSTATE_0
#define HSPLL_OSCILLATOR_STARTED                OSCSTATE_1

//*****************************************************************************
//
//! \brief Used in the HSPLL_init() function as the param parameter.
//
//*****************************************************************************
typedef struct HSPLL_initParam
{
    //! PLL Multiplier
    //! \n Valid values are  16 thru 39. Default value is 16
    //! \n Alternative valid values are any OR of PLLM_x_H bits
    uint16_t multiplier;
    //! Selects MSB shift from filter out
    //! - \b HSPLL_LESSER_OR_EQUAL_TO_6MHZ [Default] 
    //! - \b HSPLL_GREATER_THAN_6MHZ 
    uint16_t frequency;
    //! Selects the output bit resolution
    //! \n Valid values are:
    //! - \b HSPLL_UNLOCKED [Default]
    //! - \b HSPLL_LOCKED
    uint16_t lockStatus;
} HSPLL_initParam;

//*****************************************************************************
//
//! \brief Used in the HSPLL_xtalInit() function as the param parameter.
//
//*****************************************************************************
typedef struct HSPLL_xtalInitParam
{
    //! Selects the oscillator type
    //! \n Valid values are:
    //! - \b HSPLL_XTAL_GATING_COUNTER_LENGTH_4096 [Default]
    //! - \b HSPLL_XTAL_GATING_COUNTER_LENGTH_512
    //! - \b HSPLL_XTAL_OSCTYPE_XTAL [Default]
    //! - \b HSPLL_XTAL_OSCTYPE_CERAMIC
    uint16_t oscillatorType;
    //! Disables/Enables the oscillator output
    //! \n Valid values are:
    //! - \b HSPLL_XTAL_OUTPUT_DISABLE [Default]
    //! - \b HSPLL_XTAL_OUTPUT_ENABLE
    uint16_t xtlOutput;
    //! Selects the Auto Sample Start
    //! \n Valid values are:
    //! - \b HSPLL_XTAL_DISABLE [Default]
    //! - \b HSPLL_XTAL_ENABLE
    uint16_t oscillatorEnable;
} HSPLL_xtalInitParam;

//*****************************************************************************
//
//! \brief Initializes the HSPLL module
//!
//! Initializes the HSPLL module
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! \param params is the pointer to the initialization structure
//!
//! \return None
//
//*****************************************************************************
extern void HSPLL_init(uint16_t baseAddress, HSPLL_initParam *param);

//*****************************************************************************
//
//! \brief Initializes the HSPLL XTAL module
//!
//! Initializes the HSPLL XTAL module
//!
//! \param baseAddress is the base address of the HSPLL XTAL module.
//!
//! \param params is the pointer to the initialization structure
//!
//! \return None
//
//*****************************************************************************
extern void HSPLL_xtalInit(uint16_t baseAddress, HSPLL_xtalInitParam *param);

//*****************************************************************************
//
//! \brief Returns the status of the selected  interrupt flags.
//!
//! Returns the status of the selected  interrupt flag. 
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! \return \b HSPLL_PLL_STATE_UNCHANGED or 
//!         \b HSPLL_PLL_STATE_CHANGED_LOCK_TO_UNLOCK
//!
//
//*****************************************************************************
extern uint16_t HSPLL_getInterruptStatus(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Returns the mask status of the selected  interrupt flags.
//!
//! Returns the mask status of the selected  interrupt flag. 
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! \return \b HSPLL_PLL_STATE_UNCHANGED or 
//!         \b HSPLL_PLL_STATE_CHANGED_LOCK_TO_UNLOCK
//!
//
//*****************************************************************************
extern uint16_t HSPLL_getInterruptMaskStatus(uint16_t baseAddress);

//*****************************************************************************
//
///! \brief Enable HSPLL PLLUNLOCK interrupt.
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! Modified registers are HSPLLIMSC 
//!
//! \return None
//
//*****************************************************************************
extern void HSPLL_enableInterrupt(uint16_t baseAddress);

//*****************************************************************************
//
///! \brief Disable HSPLL PLLUNLOCK interrupt.
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! Modified registers are HSPLLIMSC 
//!
//! \return None
//
//*****************************************************************************
extern void HSPLL_disableInterrupt(uint16_t baseAddress);

//*****************************************************************************
//
///! \brief Clear HSPLL PLLUNLOCK interrupt.
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! Modified registers are HSPLLICR 
//!
//! \return None
//
//*****************************************************************************
extern void HSPLL_clearInterrupt(uint16_t baseAddress);

//*****************************************************************************
//
///! \brief Set HSPLL PLLUNLOCK interrupt.
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! Modified registers are HSPLLISR 
//!
//! \return None
//
//*****************************************************************************
extern void HSPLL_setInterrupt(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Returns the oscillator status
//!
//! Returns the oscillator status
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! \return \b HSPLL_OSCILLATOR_NOT_STARTED or 
//!         \b HSPLL_OSCILLATOR_STARTED
//!
//
//*****************************************************************************
extern uint16_t HSPLL_getOscillatorStatus(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Returns the PLL status
//!
//! Returns the PLL status
//!
//! \param baseAddress is the base address of the HSPLL module.
//!
//! \return \b HSPLL_UNLOCKED or 
//!         \b HSPLL_LOCKED
//!
//
//*****************************************************************************
extern uint16_t HSPLL_isLocked(uint16_t baseAddress);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_HSPLL_H__

