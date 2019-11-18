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
// mtif.h - Driver for the MTIF Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_MTIF_H__
#define __MSP430WARE_MTIF_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_MTIF__

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
// The following are values that can be passed to the freq parameter for
// functions: MTIF_setPulseGenPulseGridFreq
//
//*****************************************************************************
#define MTIF_PULSE_GRID_FREQUENCY_8HZ                                    PGFS_0
#define MTIF_PULSE_GRID_FREQUENCY_16HZ                                   PGFS_1
#define MTIF_PULSE_GRID_FREQUENCY_32HZ                                   PGFS_2
#define MTIF_PULSE_GRID_FREQUENCY_64HZ                                   PGFS_3
#define MTIF_PULSE_GRID_FREQUENCY_128HZ                                  PGFS_4
#define MTIF_PULSE_GRID_FREQUENCY_256HZ                                  PGFS_5
#define MTIF_PULSE_GRID_FREQUENCY_512HZ                                  PGFS_6
#define MTIF_PULSE_GRID_FREQUENCY_1024HZ                                 PGFS_7

//*****************************************************************************
//
// The following are values that can be passed to the num parameter for
// functions: MTIF_setPulseGenCountNum
//
//*****************************************************************************
#define MTIF_KVAL_BIT0                                                    KVAL0
#define MTIF_KVAL_BIT1                                                    KVAL1
#define MTIF_KVAL_BIT2                                                    KVAL2
#define MTIF_KVAL_BIT3                                                    KVAL3
#define MTIF_KVAL_BIT4                                                    KVAL4
#define MTIF_KVAL_BIT5                                                    KVAL5
#define MTIF_KVAL_BIT6                                                    KVAL6

//*****************************************************************************
//
// The following are values that can be returned by the
// MTIF_isPulseKCountUpdated() function.
//
//*****************************************************************************
#define MTIF_K_COUNT_IS_UPDATED                                           PKUA
#define MTIF_K_COUNT_IS_NOT_UPDATED                                        0x0

//*****************************************************************************
//
// The following are values that can be returned by the
// MTIF_isPulseGridFreqUpdated() function.
//
//*****************************************************************************
#define MTIF_PULSE_GRID_FREQUENCY_IS_UPDATED                              PGUA
#define MTIF_PULSE_GRID_FREQUENCY_IS_NOT_UPDATED                           0x0

//*****************************************************************************
//
// The following are values that can be returned by the
// MTIF_getPulseCounterOverflow() function.
//
//*****************************************************************************
#define MTIF_PULSE_COUNTER_OVERFLOW                                       PCOFL
#define MTIF_PULSE_COUNTER_NOT_OVERFLOW                                     0x0

//*****************************************************************************
//
// The following are values that can be returned by the
// MTIF_isPulseCounterReadReady() function.
//
//*****************************************************************************
#define MTIF_PULSE_COUNTER_READY_TO_READ                                   PCRA
#define MTIF_PULSE_COUNTER_NOT_READY_TO_READ                                0x0

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Enables Pulse Generator.
//!
//! Enable the PG sub module
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_enablePulseGen(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Disables Pulse Generator.
//!
//! Disable the PG sub module
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_disablePulseGen(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse Generator pulse counter clear.
//!
//! Clears the pulse generator. (PGEN has to be set to one to perform a clear).
//! Note!: A clear request is being latched and released after the clear
//! is executed. MTIF_disablePulseGen (PCEN=0) and LFXTOFF=1 will prevent that.
//! The clear occurs then after the clock is reenabled. Clear reset by PUC.
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_clearPulseGenCounter(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse Generator pulse grid frequency select.
//!
//! This value determines at which time grid pulses are generated.
//! The pulse generator frame frequency is an 1/256th of this
//! (PGEN has to be one to perform a change). Reset by PUC.
//!
//! \param baseAddress is the base address of the MTIF module.
//! \param freq is the specified pulse grid frequency select.
//!        Valid values are:
//!        - \b MTIF_PULSE_GRID_FREQUENCY_8HZ
//!        - \b MTIF_PULSE_GRID_FREQUENCY_16HZ
//!        - \b MTIF_PULSE_GRID_FREQUENCY_32HZ
//!        - \b MTIF_PULSE_GRID_FREQUENCY_64HZ
//!        - \b MTIF_PULSE_GRID_FREQUENCY_128HZ
//!        - \b MTIF_PULSE_GRID_FREQUENCY_256HZ
//!        - \b MTIF_PULSE_GRID_FREQUENCY_512HZ
//!        - \b MTIF_PULSE_GRID_FREQUENCY_1024HZ [Default]
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setPulseGenPulseGridFreq(uint16_t baseAddress, uint8_t freq);

//*****************************************************************************
//
//! \brief Set Pulse Count Number.
//!
//! This register value determines how many pulses are generated withing
//! 256 periods of the pulse grid frequency(with password protection as in PGCNF).
//! PGEN has to be one to perform a change. Reset by PUC.
//!
//! \param baseAddress is the base address of the MTIF module.
//! \param num is the logical OR of any of the following:
//!        - \b MTIF_KVAL_BIT0
//!        - \b MTIF_KVAL_BIT1
//!        - \b MTIF_KVAL_BIT2
//!        - \b MTIF_KVAL_BIT3
//!        - \b MTIF_KVAL_BIT4
//!        - \b MTIF_KVAL_BIT5
//!        - \b MTIF_KVAL_BIT6
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setPulseGenCountNum(uint16_t baseAddress, uint8_t num);

//*****************************************************************************
//
//! \brief Pulse K-Count Update Request.
//!
//! The update of KVAL occurs during the frequency grid slot
//! 0xff (e.g. in the last 4ms of a second with a pulse grid frequency of
//! 256Hz). Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_clearPulseKCountUpdateRequest(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse Grid Frequency Update Request.
//!
//! The update of PGFS occurs during the frequency grid
//! slot 0xff (e.g. in the last 4ms of an second with an pulse grid
//! frequency of 256Hz). Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_clearPulseGridFreqUpdateRequest(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse K-Count Update Acknowledge.
//!
//! This acknowledges a PCUR directly after the K-values has been updated.
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setPulseKCountUpdateAck(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Is Pulse K-Count Updated?
//!
//! This checks whether Pulse K-Count has been updated.
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return Valid values are:
//!        - \b MTIF_K_COUNT_IS_UPDATED
//!        - \b MTIF_K_COUNT_IS_NOT_UPDATED
//
//*****************************************************************************
extern uint16_t MTIF_isPulseKCountUpdated(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse Grid Frequency Update Acknowledge.
//!
//! This acknowledges a PGUR directly after the PGFS has been updated.
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setPulseGridFreqUpdateAck(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Is Pulse Pulse Grid Frequency Updated?
//!
//! This checks whether Pulse Grid Frequency has been updated.
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return Valid values are:
//!        - \b MTIF_PULSE_GRID_FREQUENCY_IS_UPDATED
//!        - \b MTIF_PULSE_GRID_FREQUENCY_IS_NOT_UPDATED
//
//*****************************************************************************
extern uint16_t MTIF_isPulseGridFreqUpdated(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief PC sub module enable.
//!
//! This bit enables the PC sub module when set to one. Reset type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_enablePulseCounter(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief PC sub module disable.
//!
//! This bit disables the PC sub module when set to zero. Reset type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_disablePulseCounter(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse counter clear.
//!
//! Clears the pulse counter (PC has to be disabled to perform a clear).
//! Note!: A clear request is being latched and released after the clear is executed.
//! LFXTOFF=1 and PCEN=0 will prevent that. The clear occurs then after the clock
//! is reenabled. This bit is for triggering only; it's state cannot be read back
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_clearPulseCounter(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse Counter value register.
//!
//! Get count value from the pulse counter.
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return 16-bit count value from pulse counter
//
//*****************************************************************************
extern uint16_t MTIF_getPulseCount(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse Counter Read Request.
//!
//! Requests an update of PCR read register from the actual counter.
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setPulseCounterReadRequest(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse counter read acknowledge.
//!
//! This acknowledges the update of the PCR register as response to
//! the PCRR read request. Note!: A read request is being latched.
//! LFXTOFF=1 and PCEN=0 will prevent that.The read will then be performed and
//! acknowledged after the clock is reenabled. Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setPulseCounterReadAck(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Is Pulse Counter value ready to read?.
//!
//! This function will let you know if Pulse Counter value is updated and ready
//! to read as a response to the Pulse Counter Read Request. Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return Valid values are:
//!        - \b MTIF_PULSE_COUNTER_READY_TO_READ
//!        - \b MTIF_PULSE_COUNTER_NOT_READY_TO_READ
//
//*****************************************************************************
extern uint16_t MTIF_isPulseCounterReadReady(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Pulse counter overflow.
//!
//! This bit indicates an overflow of the pulse counter when its value changes since
//! the last read request procedure. It is basically the 17th bit of the counter
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return Valid values are:
//!        - \b MTIF_PULSE_COUNTER_OVERFLOW
//!        - \b MTIF_PULSE_COUNTER_NOT_OVERFLOW
//
//*****************************************************************************
extern uint16_t MTIF_getPulseCounterOverflow(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Acknowledge pulse counter overflow.
//!
//! This function acknowledges an overflow of the pulse counter since
//! the last read request procedure.
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_ackPulseCounterOverflow(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Enables test port output.
//!
//! Enable the test pulse output. Reset type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_enableTestPortOutput(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Disables test port output.
//!
//! Disable the test pulse output. Reset type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_disableTestPortOutput(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Enables test port input.
//!
//! Enable the test input port. Reset type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_enableTestPortInput(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Disables test port input.
//!
//! Disable the test input port. Reset type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_disableTestPortInput(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Test port input select for pulse counter, sourced from pulse generator.
//!
//! Pulse generator is used as input for test port of pulse counter
//! Reset Type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setPulseGeneratorAsPulseCounterInput(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Test port input select for pulse counter, sourced from test port
//! input terminal.
//!
//! Test port input terminal is used as input for test port of pulse counter
//! Reset Type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_setTestPortInputTerminalAsPulseCounterInput(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Test port terminal enable activation by software.
//!
//! The test port output is enabled solely by TPOE (enabled if TPOE=1) test port
//! output enabled.
//! Reset Type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_enableTestPortTerminalActivationBySW(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Test port terminal enable activation by software and hardware.
//!
//! The test port output requires both TPOE (enabled if TPOE=1) test port
//! output enabled, and MTPE pin to be high, to be enabled.
//! Reset Type: POR
//!
//! \param baseAddress is the base address of the MTIF module.
//!
//! \return None
//
//*****************************************************************************
extern void MTIF_enableTestPortTerminalActivationBySWAndHW(uint16_t baseAddress);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_MTIF_H__

