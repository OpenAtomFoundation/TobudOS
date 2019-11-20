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
// sdhs.h - Driver for the SDHS Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_SDHS_H__
#define __MSP430WARE_SDHS_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SDHS__

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

#include "inc/hw_memmap.h"

//*****************************************************************************
//
// The following are values that can be passed to the interruptMask parameter
// for functions: SDHS_getInterruptStatus(), SDHS_enableInterrupt(), 
// SDHS_disableInterrupt()
//
//*****************************************************************************
#define SDHS_INCOMPLETE_STOP_INTERRUPT                                    ISTOP
#define SDHS_WINDOW_LOW_INTERRUPT                                         WINLO
#define SDHS_WINDOW_HIGH_INTERRUPT                                        WINHI
#define SDHS_DATA_READY_INTERRUPT                                         DTRDY
#define SDHS_START_CONVERSION_TRIGGER_INTERRUPT                           SSTRG
#define SDHS_ACQUISITION_DONE_INTERRUPT                                 ACQDONE
#define SDHS_DATA_OVERFLOW_INTERRUPT                                        OVF

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the triggerSourceSelect parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_REGISTER_CONTROL_MODE                                     TRGSRC_0
#define SDHS_ASQ_CONTROL_MODE                                          TRGSRC_1

//*****************************************************************************
//
// The following are values that are returned by SDHS_getRegisterLockStatus()
//
//*****************************************************************************
#define SDHS_REGISTERS_LOCKED                                       SDHS_LOCK_1
#define SDHS_REGISTERS_UNLOCKED                                     SDHS_LOCK_0

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the msbShift parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_NO_SHIFT                                                   SHIFT_0
#define SDHS_SHIFT_LEFT_1                                               SHIFT_1
#define SDHS_SHIFT_LEFT_2                                               SHIFT_2

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the outputBitResolution parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_OUTPUT_RESOLUTION_12_BIT                                     OBR_0
#define SDHS_OUTPUT_RESOLUTION_13_BIT                                     OBR_1
#define SDHS_OUTPUT_RESOLUTION_14_BIT                                     OBR_2

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the dataFormat parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_DATA_FORMAT_TWOS_COMPLEMENT                               DFMSEL_0
#define SDHS_DATA_FORMAT_OFFSET_BINARY                                 DFMSEL_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the dataAlignment parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_DATA_ALIGNED_RIGHT                                         DALGN_0
#define SDHS_DATA_ALIGNED_LEFT                                          DALGN_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the interruptDelayGeneration parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_DELAY_SAMPLES_0                                           INTDLY_0
#define SDHS_DELAY_SAMPLES_1                                           INTDLY_1
#define SDHS_DELAY_SAMPLES_2                                           INTDLY_2
#define SDHS_DELAY_SAMPLES_3                                           INTDLY_3
#define SDHS_DELAY_SAMPLES_4                                           INTDLY_4
#define SDHS_DELAY_SAMPLES_5                                           INTDLY_5
#define SDHS_DELAY_SAMPLES_6                                           INTDLY_6
#define SDHS_DELAY_SAMPLES_7                                           INTDLY_7

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the autoSampleStart parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_AUTO_SAMPLE_START_ENABLED                              AUTOSSDIS_0
#define SDHS_AUTO_SAMPLE_START_DISABLED                             AUTOSSDIS_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the oversamplingRate parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_OVERSAMPLING_RATE_10                                         OSR_0
#define SDHS_OVERSAMPLING_RATE_20                                         OSR_1
#define SDHS_OVERSAMPLING_RATE_40                                         OSR_2
#define SDHS_OVERSAMPLING_RATE_80                                         OSR_3
#define SDHS_OVERSAMPLING_RATE_160                                        OSR_4

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the dataTransferController parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_DATA_TRANSFER_CONTROLLER_ON                               DTCOFF_0
#define SDHS_DATA_TRANSFER_CONTROLLER_OFF                              DTCOFF_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the windowComparator parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_WINDOW_COMPARATOR_DISABLE                               WINCMPEN_0
#define SDHS_WINDOW_COMPARATOR_ENABLE                                WINCMPEN_1

//*****************************************************************************
//
// The following are values that can be passed to the param parameter for
// functions: SDHS_init(); the sampleSizeCounting parameter for
// functions: SDHS_init().
//
//*****************************************************************************
#define SDHS_SMPSZ_USED                                             SMPCTLOFF_0
#define SDHS_SMPSZ_IGNORED                                          SMPCTLOFF_1

//*****************************************************************************
//
// The following are values that can be passed to the optimization parameter for
// functions: SDHS_setModularOptimization().
//
//*****************************************************************************
#define SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_77_80MHz                         0xC
#define SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_74_77MHz                         0xD
#define SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_71_74MHz                         0xE
#define SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_68_71MHz                         0xF

//*****************************************************************************
//
//! \brief Used in the SDHS_init() function as the param parameter.
//
//*****************************************************************************
typedef struct SDHS_initParam
{
    //! Trigger source select
    //! \n Valid values are:
    //! - \b SDHS_REGISTER_CONTROL_MODE [Default]
    //! - \b SDHS_ASQ_CONTROL_MODE
    uint16_t triggerSourceSelect;
    //! Selects MSB shift from filter out
    //! - \b SDHS_NO_SHIFT [Default] 
    //! - \b SDHS_SHIFT_LEFT_1 
    //! - \b SDHS_SHIFT_LEFT_2 
    uint8_t msbShift;
    //! Selects the output bit resolution
    //! \n Valid values are:
    //! - \b SDHS_OUTPUT_RESOLUTION_12_BIT [Default]
    //! - \b SDHS_OUTPUT_RESOLUTION_13_BIT
    //! - \b SDHS_OUTPUT_RESOLUTION_14_BIT
    uint16_t outputBitResolution;
    //! Selects the data format
    //! \n Valid values are:
    //! - \b SDHS_DATA_FORMAT_TWOS_COMPLEMENT [Default]
    //! - \b SDHS_DATA_FORMAT_OFFSET_BINARY
    uint16_t dataFormat;
    //! Selects the data format
    //! \n Valid values are:
    //! - \b SDHS_DATA_ALIGNED_RIGHT [Default]
    //! - \b SDHS_DATA_ALIGNED_LEFT
    uint16_t dataAlignment;
    //! Selects the data format
    //! \n Valid values are:
    //! - \b SDHS_DELAY_SAMPLES_0
    //! - \b SDHS_DELAY_SAMPLES_1 [Default]
    //! - \b SDHS_DELAY_SAMPLES_2
    //! - \b SDHS_DELAY_SAMPLES_3
    //! - \b SDHS_DELAY_SAMPLES_4
    //! - \b SDHS_DELAY_SAMPLES_5
    //! - \b SDHS_DELAY_SAMPLES_6
    //! - \b SDHS_DELAY_SAMPLES_7
    uint16_t interruptDelayGeneration;
    //! Selects the Auto Sample Start
    //! \n Valid values are:
    //! - \b SDHS_AUTO_SAMPLE_START_DISABLED [Default]
    //! - \b SDHS_AUTO_SAMPLE_START_ENABLED
    uint16_t autoSampleStart;
    //! Selects the Oversampling Rate
    //! \n Valid values are:
    //! - \b SDHS_OVERSAMPLING_RATE_10 [Default]
    //! - \b SDHS_OVERSAMPLING_RATE_20
    //! - \b SDHS_OVERSAMPLING_RATE_40
    //! - \b SDHS_OVERSAMPLING_RATE_80
    //! - \b SDHS_OVERSAMPLING_RATE_160
    uint16_t oversamplingRate;
    //! Selects the Data Transfer Controller State
    //! \n Valid values are:
    //! - \b SDHS_DATA_TRANSFER_CONTROLLER_ON [Default]
    //! - \b SDHS_DATA_TRANSFER_CONTROLLER_OFF
    uint16_t dataTransferController;
    //! Selects the Window Comparator State
    //! \n Valid values are:
    //! - \b SDHS_WINDOW_COMPARATOR_DISABLE [Default]
    //! - \b SDHS_WINDOW_COMPARATOR_ENABLE
    uint16_t windowComparator;
    //! Selects the Sample Size Counting 
    //! \n Valid values are:
    //! - \b SDHS_SMPSZ_USED [Default]
    //! - \b SDHS_SMPSZ_IGNORED
    uint16_t sampleSizeCounting;
} SDHS_initParam;

//*****************************************************************************
//
//! \brief Initializes the SDHS module
//!
//! Initializes the SDHS moduleress
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \param params is the pointer to the initialization structure
//!
//! At the end of this call the TRIGEN and SDHS_LOCK bits are reset
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_init(uint16_t baseAddress, SDHS_initParam *param);

//*****************************************************************************
//
//! \brief Returns the status of the selected  interrupt flags.
//!
//! Returns the status of the selected  interrupt flags. 
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param interruptMask
//!            Mask value is the logical OR of any of the following:
//!         Valid values are:
//!        - \b SDHS_INCOMPLETE_STOP_INTERRUPT    
//!        - \b SDHS_WINDOW_LOW_INTERRUPT
//!        - \b SDHS_WINDOW_HIGH_INTERRUPT 
//!        - \b SDHS_DATA_READY_INTERRUPT
//!        - \b SDHS_START_CONVERSION_TRIGGER_INTERRUPT
//!        - \b SDHS_ACQUISITION_DONE_INTERRUPT
//!        - \b SDHS_DATA_OVERFLOW_INTERRUPT
//! \return Logical OR of any of above valid values for interruptMask
//!         \n indicating the status of the masked flags
//!
//
//*****************************************************************************
extern uint16_t SDHS_getInterruptStatus(uint16_t baseAddress, uint16_t interruptMask);

//*****************************************************************************
//
//! \brief Returns the mask status of the selected  interrupt.
//!
//! Returns the mask status of the selected  interrupt flags. 
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param interruptMask
//!            Mask value is the logical OR of any of the following:
//!         Valid values are:
//!        - \b SDHS_INCOMPLETE_STOP_INTERRUPT
//!        - \b SDHS_WINDOW_LOW_INTERRUPT
//!        - \b SDHS_WINDOW_HIGH_INTERRUPT
//!        - \b SDHS_DATA_READY_INTERRUPT
//!        - \b SDHS_START_CONVERSION_TRIGGER_INTERRUPT
//!        - \b SDHS_ACQUISITION_DONE_INTERRUPT
//!        - \b SDHS_DATA_OVERFLOW_INTERRUPT
//! \return Logical OR of any of above valid values for interruptMask
//!         \n indicating the status of the masked flags
//!
//
//*****************************************************************************
extern uint16_t SDHS_getInterruptMaskStatus(uint16_t baseAddress, uint16_t interruptMask);

//*****************************************************************************
//
//! \brief Clears SDHS selected interrupt flags.
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param interruptMask
//!            Mask value is the logical OR of any of the following:
//!         Valid values are:
//!        - \b SDHS_INCOMPLETE_STOP_INTERRUPT
//!        - \b SDHS_WINDOW_LOW_INTERRUPT
//!        - \b SDHS_WINDOW_HIGH_INTERRUPT
//!        - \b SDHS_DATA_READY_INTERRUPT
//!        - \b SDHS_START_CONVERSION_TRIGGER_INTERRUPT
//!        - \b SDHS_ACQUISITION_DONE_INTERRUPT
//!        - \b SDHS_DATA_OVERFLOW_INTERRUPT
//!
//! Modified registers are \b SDHSICR 
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_clearInterrupt(uint16_t baseAddress, uint16_t interruptMask);

//*****************************************************************************
//
//! \brief Enable SDHS selected interrupt masks.
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param interruptMask
//!            Mask value is the logical OR of any of the following:
//!         Valid values are:
//!        - \b SDHS_WINDOW_LOW_INTERRUPT
//!        - \b SDHS_WINDOW_HIGH_INTERRUPT
//!        - \b SDHS_DATA_READY_INTERRUPT
//!        - \b SDHS_START_CONVERSION_TRIGGER_INTERRUPT
//!        - \b SDHS_ACQUISITION_DONE_INTERRUPT
//!        - \b SDHS_DATA_OVERFLOW_INTERRUPT
//!
//! Modified registers are SDHSIMSC 
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_enableInterrupt(uint16_t baseAddress, uint16_t interruptMask);

//*****************************************************************************
//
//! \brief Disable SDHS selected interrupt masks.
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param interruptMask 
//!            Mask value is the logical OR of any of the following:
//!         Valid values are:
//!        - \b SDHS_WINDOW_LOW_INTERRUPT
//!        - \b SDHS_WINDOW_HIGH_INTERRUPT 
//!        - \b SDHS_DATA_READY_INTERRUPT
//!        - \b SDHS_START_CONVERSION_TRIGGER_INTERRUPT
//!        - \b SDHS_ACQUISITION_DONE_INTERRUPT
//!        - \b SDHS_DATA_OVERFLOW_INTERRUPT
//!
//! Modified registers are SDHSIMSC 
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_disableInterrupt(uint16_t baseAddress, uint16_t interruptMask);

//*****************************************************************************
//
//! \brief Set SDHS selected interrupts.
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param interruptMask 
//!            Mask value is the logical OR of any of the following:
//!         Valid values are:
//!        - \b SDHS_WINDOW_LOW_INTERRUPT
//!        - \b SDHS_WINDOW_HIGH_INTERRUPT 
//!        - \b SDHS_DATA_READY_INTERRUPT
//!        - \b SDHS_START_CONVERSION_TRIGGER_INTERRUPT
//!        - \b SDHS_ACQUISITION_DONE_INTERRUPT
//!        - \b SDHS_DATA_OVERFLOW_INTERRUPT
//!
//! Modified registers are SDHSISR 
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_setInterrupt(uint16_t baseAddress, uint16_t interruptMask);

//*****************************************************************************
//
//! \brief Sets the high and low threshold for the window comparator feature.
//!
//! Sets the high and low threshold for the window comparator feature.
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param highThreshold is the upper bound that could trip an interrupt for
//!        the window comparator.
//! \param lowThreshold is the lower bound that could trip on interrupt for the
//!        window comparator.
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_setWindowComp(uint16_t baseAddress, uint16_t highThreshold,
    uint16_t lowThreshold);

//*****************************************************************************
//
//! \brief Sets total sample size
//!
//! Note that SDHSCTL2.SMPSZ includes the samples skipped by SDHSCTL0.INTDLY:
//! - The total number of samples SDHS generates = sampleSize + 1.
//! - The number of samples SDHS generates via SDHSDT register = sampleSize - INTDLY + 1.
//! If sampleSize - INTDLY + 1 <= 0, then no data output to SDHSDT register
//!
//! \param baseAddress is the base address of the SDHS module.
//! \param sampleSize is the total number of samples SDHS generates + 1
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_setTotalSampleSize(uint16_t baseAddress, uint16_t sampleSize);

//*****************************************************************************
//
//! \brief Enables the SDHS Trigger.
//!
//! Enables the SDHS Trigger.
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_enableTrigger(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Disables the SDHS Trigger.
//!
//! Disables the SDHS Trigger.
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_disableTrigger(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Turn On Module
//!
//! Turn On Module
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_enable(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Start Conversion
//!
//! Start Conversion
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_startConversion(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief End Conversion
//!
//! End Conversion
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_endConversion(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Set PGA Gain 
//!
//! Set PGA Gain 
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//!    \param gain is the PGA Gain control value
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_setPGAGain(uint16_t baseAddress, uint16_t gain);

//*****************************************************************************
//
//! \brief Set Modular Optimization
//!
//! Set Modular Optimization. Upper bound of optimization parameter label is non-inclusive.
//! For example, if your PLL output frequency is 77 MHz, you should select
//! SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_77_80MHz for optimization parameter.
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//!    \param optimization is the Modular Optimization value
//!         Valid values are:
//!        - \b SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_77_80MHz
//!        - \b SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_74_77MHz
//!        - \b SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_71_74MHz
//!        - \b SDHS_OPTIMIZE_PLL_OUTPUT_FREQUENCY_68_71MHz [Default]
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_setModularOptimization(uint16_t baseAddress, uint16_t optimization);

//*****************************************************************************
//
//! \brief Turn Off Module
//!
//! Turn Off Module
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_disable(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Returns the contents of the Data Conversion Register
//!
//! Returns the contents of the Data Conversion Register.
//!    The sign bit is extended up to the bit 15.
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return The contents of the Data Conversion Register
//
//*****************************************************************************
extern uint16_t SDHS_getResults(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Returns the lock status of the registers
//!
//! Returns the lock status of the registers
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \return \b SDHS_REGISTERS_LOCKED or \b SDHS_REGISTERS_UNLOCKED
//
//*****************************************************************************
extern uint16_t SDHS_getRegisterLockStatus(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Sets the DTC destination address
//!
//! Sets the DTC Destination Address
//!
//! \param baseAddress is the base address of the SDHS module.
//!
//! \param address is the destination address to be set
//!
//! \return None
//
//*****************************************************************************
extern void SDHS_setDTCDestinationAddress(uint16_t baseAddress, uint16_t address);

//*****************************************************************************
//
// The following are deprecated APIs.
//
//*****************************************************************************
#define SDHS_turnOnModuleinStandaloneMode                           SDHS_enable
#define SDHS_turnOffModuleinStandaloneMode                         SDHS_disable
#define SDHS_startConversioninStandaloneMode               SDHS_startConversion

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_SDHS_H__

