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
// framctl_a.h - Driver for the FRAMCTL_A Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_FRAMCTL_A_H__
#define __MSP430WARE_FRAMCTL_A_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_FRCTL_A__

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
// The following are values that can be passed to the interruptMask parameter
// for functions: FRAMCtl_A_enableInterrupt(), and
// FRAMCtl_A_disableInterrupt().
//
//*****************************************************************************
#define FRAMCTL_A_PUC_ON_UNCORRECTABLE_BIT                             UBDRSTEN
#define FRAMCTL_A_UNCORRECTABLE_BIT_INTERRUPT                             UBDIE
#define FRAMCTL_A_CORRECTABLE_BIT_INTERRUPT                               CBDIE
#define FRAMCTL_A_ACCESS_TIME_ERROR_INTERRUPT                           ACCTEIE
#define FRAMCTL_A_WRITE_PROTECTION_INTERRUPT                               WPIE

//*****************************************************************************
//
// The following are values that can be passed to the interruptFlagMask
// parameter for functions: FRAMCtl_A_getInterruptStatus(), and
// FRAMCtl_A_clearInterrupt() as well as returned by the
// FRAMCtl_A_getInterruptStatus() function.
//
//*****************************************************************************
#define FRAMCTL_A_ACCESS_TIME_ERROR_FLAG                               ACCTEIFG
#define FRAMCTL_A_UNCORRECTABLE_BIT_FLAG                                 UBDIFG
#define FRAMCTL_A_CORRECTABLE_BIT_FLAG                                   CBDIFG
#define FRAMCTL_A_WRITE_PROTECTION_FLAG                                   WPIFG

//*****************************************************************************
//
// The following are values that can be passed to the waitState parameter for
// functions: FRAMCtl_A_configureWaitStateControl().
//
//*****************************************************************************
#define FRAMCTL_A_ACCESS_TIME_CYCLES_0                                 NWAITS_0
#define FRAMCTL_A_ACCESS_TIME_CYCLES_1                                 NWAITS_1
#define FRAMCTL_A_ACCESS_TIME_CYCLES_2                                 NWAITS_2
#define FRAMCTL_A_ACCESS_TIME_CYCLES_3                                 NWAITS_3
#define FRAMCTL_A_ACCESS_TIME_CYCLES_4                                 NWAITS_4
#define FRAMCTL_A_ACCESS_TIME_CYCLES_5                                 NWAITS_5
#define FRAMCTL_A_ACCESS_TIME_CYCLES_6                                 NWAITS_6
#define FRAMCTL_A_ACCESS_TIME_CYCLES_7                                 NWAITS_7
#define FRAMCTL_A_ACCESS_TIME_CYCLES_8                                 NWAITS_8
#define FRAMCTL_A_ACCESS_TIME_CYCLES_9                                 NWAITS_9
#define FRAMCTL_A_ACCESS_TIME_CYCLES_10                               NWAITS_10
#define FRAMCTL_A_ACCESS_TIME_CYCLES_11                               NWAITS_11
#define FRAMCTL_A_ACCESS_TIME_CYCLES_12                               NWAITS_12
#define FRAMCTL_A_ACCESS_TIME_CYCLES_13                               NWAITS_13
#define FRAMCTL_A_ACCESS_TIME_CYCLES_14                               NWAITS_14
#define FRAMCTL_A_ACCESS_TIME_CYCLES_15                               NWAITS_15

//*****************************************************************************
//
// The following are values that can be passed to the delayStatus parameter for
// functions: FRAMCtl_A_delayPowerUpFromLPM().
//
//*****************************************************************************
#define FRAMCTL_A_DELAY_FROM_LPM_ENABLE                                    0x00
#define FRAMCTL_A_DELAY_FROM_LPM_DISABLE                                  FRPWR

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Write data into the fram memory in byte format.
//!
//! \param dataPtr is the pointer to the data to be written
//! \param framPtr is the pointer into which to write the data
//! \param numberOfBytes is the number of bytes to be written
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_write8(uint8_t *dataPtr,
                             uint8_t *framPtr,
                             uint16_t numberOfBytes);

//*****************************************************************************
//
//! \brief Write data into the fram memory in word format.
//!
//! \param dataPtr is the pointer to the data to be written
//! \param framPtr is the pointer into which to write the data
//! \param numberOfWords is the number of words to be written
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_write16(uint16_t *dataPtr,
                              uint16_t *framPtr,
                              uint16_t numberOfWords);

//*****************************************************************************
//
//! \brief Write data into the fram memory in long format, pass by reference
//!
//! \param dataPtr is the pointer to the data to be written
//! \param framPtr is the pointer into which to write the data
//! \param count is the number of 32 bit words to be written
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_write32(uint32_t *dataPtr,
                              uint32_t *framPtr,
                              uint16_t count);

//*****************************************************************************
//
//! \brief Write data into the fram memory in long format, pass by value
//!
//! \param value is the value to written to FRAMCTL_A memory
//! \param framPtr is the pointer into which to write the data
//! \param count is the number of 32 bit addresses to fill
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_fillMemory32(uint32_t value,
                                   uint32_t *framPtr,
                                   uint16_t count);

//*****************************************************************************
//
//! \brief Enables selected FRAMCtl_A interrupt sources.
//!
//! Enables the indicated FRAMCtl_A interrupt sources.  Only the sources that
//! are enabled can be reflected to the processor interrupt; disabled sources
//! have no effect on the processor. Does not clear interrupt flags.
//!
//! \param interruptMask is the bit mask of the memory buffer interrupt sources
//!        to be disabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b FRAMCTL_A_PUC_ON_UNCORRECTABLE_BIT - Enable PUC reset if
//!           FRAMCtl_A uncorrectable bit error detected.
//!        - \b FRAMCTL_A_UNCORRECTABLE_BIT_INTERRUPT - Interrupts when an
//!           uncorrectable bit error is detected.
//!        - \b FRAMCTL_A_CORRECTABLE_BIT_INTERRUPT - Interrupts when a
//!           correctable bit error is detected.
//!        - \b FRAMCTL_A_ACCESS_TIME_ERROR_INTERRUPT - Interrupts when an
//!           access time error occurs.
//!        - \b FRAMCTL_A_WRITE_PROTECTION_INTERRUPT - Interrupts when
//!           detecting a write access to FRAM.
//!
//! Modified bits of \b GCCTL0 register and bits of \b FRCTL0 register.
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_enableInterrupt(uint8_t interruptMask);

//*****************************************************************************
//
//! \brief Returns the status of the selected FRAMCtl_A interrupt flags.
//!
//! \param interruptFlagMask is a bit mask of the interrupt flags status to be
//!        returned.
//!        Mask value is the logical OR of any of the following:
//!        - \b FRAMCTL_A_ACCESS_TIME_ERROR_FLAG - Interrupt flag is set if a
//!           wrong setting for NPRECHG and NACCESS is set and FRAMCtl_A access
//!           time is not hold.
//!        - \b FRAMCTL_A_UNCORRECTABLE_BIT_FLAG - Interrupt flag is set if an
//!           uncorrectable bit error has been detected in the FRAMCtl_A memory
//!           error detection logic.
//!        - \b FRAMCTL_A_CORRECTABLE_BIT_FLAG - Interrupt flag is set if a
//!           correctable bit error has been detected and corrected in the
//!           FRAMCtl_A memory error detection logic.
//!        - \b FRAMCTL_A_WRITE_PROTECTION_FLAG - Interrupt flag is set if a
//!           write access to FRAM memory
//!
//! \return Logical OR of any of the following:
//!         - \b FRAMCTL_A_ACCESS_TIME_ERROR_FLAG Interrupt flag is set if a
//!         wrong setting for NPRECHG and NACCESS is set and FRAMCtl_A access
//!         time is not hold.
//!         - \b FRAMCTL_A_UNCORRECTABLE_BIT_FLAG Interrupt flag is set if an
//!         uncorrectable bit error has been detected in the FRAMCtl_A memory
//!         error detection logic.
//!         - \b FRAMCTL_A_CORRECTABLE_BIT_FLAG Interrupt flag is set if a
//!         correctable bit error has been detected and corrected in the
//!         FRAMCtl_A memory error detection logic.
//!         - \b FRAMCTL_A_WRITE_PROTECTION_FLAG Interrupt flag is set if a
//!         write access to FRAM memory
//!         \n indicating the status of the masked flags
//
//*****************************************************************************
extern uint8_t FRAMCtl_A_getInterruptStatus(uint16_t interruptFlagMask);

//*****************************************************************************
//
//! \brief Disables selected FRAMCtl_A interrupt sources.
//!
//! Disables the indicated FRAMCtl_A interrupt sources.  Only the sources that
//! are enabled can be reflected to the processor interrupt; disabled sources
//! have no effect on the processor.
//!
//! \param interruptMask is the bit mask of the memory buffer interrupt sources
//!        to be disabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b FRAMCTL_A_PUC_ON_UNCORRECTABLE_BIT - Enable PUC reset if
//!           FRAMCtl_A uncorrectable bit error detected.
//!        - \b FRAMCTL_A_UNCORRECTABLE_BIT_INTERRUPT - Interrupts when an
//!           uncorrectable bit error is detected.
//!        - \b FRAMCTL_A_CORRECTABLE_BIT_INTERRUPT - Interrupts when a
//!           correctable bit error is detected.
//!        - \b FRAMCTL_A_ACCESS_TIME_ERROR_INTERRUPT - Interrupts when an
//!           access time error occurs.
//!        - \b FRAMCTL_A_WRITE_PROTECTION_INTERRUPT - Interrupts when
//!           detecting a write access to FRAM.
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_disableInterrupt(uint16_t interruptMask);

//*****************************************************************************
//
//! \brief Clears selected FRAMCtl_A interrupt status flag.
//!
//! Clears the indicated FRAMCtl_A interrupt status flag. These interrupt
//! status flag can also be cleared through reading the system reset vector
//! word SYSRSTIV.
//!
//! \param interruptFlagMask is a bit mask of the interrupt flags status to be
//!        cleared.
//!        Mask value is the logical OR of any of the following:
//!        - \b FRAMCTL_A_ACCESS_TIME_ERROR_FLAG - Interrupt flag is set if a
//!           wrong setting for NPRECHG and NACCESS is set and FRAMCtl_A access
//!           time is not hold.
//!        - \b FRAMCTL_A_UNCORRECTABLE_BIT_FLAG - Interrupt flag is set if an
//!           uncorrectable bit error has been detected in the FRAMCtl_A memory
//!           error detection logic.
//!        - \b FRAMCTL_A_CORRECTABLE_BIT_FLAG - Interrupt flag is set if a
//!           correctable bit error has been detected and corrected in the
//!           FRAMCtl_A memory error detection logic.
//!        - \b FRAMCTL_A_WRITE_PROTECTION_FLAG - Interrupt flag is set if a
//!           write access to FRAM memory
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_clearInterrupt(uint16_t interruptFlagMask);

//*****************************************************************************
//
//! \brief Configures the access time of the FRAMCtl_A module
//!
//! Configures the access time of the FRAMCtl_A module.
//!
//! \param waitState defines the number of CPU cycles required for access time
//!        defined in the datasheet
//!        Valid values are:
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_0
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_1
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_2
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_3
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_4
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_5
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_6
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_7
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_8
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_9
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_10
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_11
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_12
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_13
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_14
//!        - \b FRAMCTL_A_ACCESS_TIME_CYCLES_15
//!
//! Modified bits are \b NWAITS of \b GCCTL0 register.
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_configureWaitStateControl(uint8_t waitState);

//*****************************************************************************
//
//! \brief Configures when the FRAMCtl_A module will power up after LPM exit
//!
//! Configures when the FRAMCtl_A module will power up after LPM exit. The
//! module can either wait until the first FRAM access to power up or power up
//! immediately after leaving LPM. If FRAM power is disabled, the FRAM memory
//! remains in inactive mode until the FRAM memory is actually accessed. If
//! FRAM power is enabled, the FRAM wil be immediately powered up (active
//! mode).
//!
//! \param delayStatus chooses if FRAMCTL_A should power up instantly with LPM
//!        exit or to wait until first FRAMCTL_A access after LPM exit
//!        Valid values are:
//!        - \b FRAMCTL_A_DELAY_FROM_LPM_ENABLE
//!        - \b FRAMCTL_A_DELAY_FROM_LPM_DISABLE
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_delayPowerUpFromLPM(uint8_t delayStatus);

//*****************************************************************************
//
//! \brief Enables FRAM write protection
//!
//! This function enables FRAM write protection and protect entire FRAM memory
//! from unintended write. It should be used as temporary protection. The
//! permanent FRAM memory protection should be done via MPU segments related
//! APIs.
//!
//!
//! Modified bits are \b WPROT of \b FRCTL0 register.
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_enableWriteProtection(void);

//*****************************************************************************
//
//! \brief Disables FRAM write protection
//!
//! Disables the FRAM write protection. Writing to FRAM memory is allowed.
//!
//!
//! Modified bits are \b WPROT of \b FRCTL0 register.
//!
//! \return None
//
//*****************************************************************************
extern void FRAMCtl_A_disableWriteProtection(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_FRAMCTL_A_H__
