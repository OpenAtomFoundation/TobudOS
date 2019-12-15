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
// uups.h - Driver for the UUPS Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_UUPS_H__
#define __MSP430WARE_UUPS_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_UUPS__

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
// The following are values that can be passed to the mask parameter for
// functions: UUPS_clearInterrupt(), UUPS_enableInterrupt() and 
// UUPS_disableInterrupt() well as returned by the UUPS_getInterruptStatus() 
// function.
//
//*****************************************************************************
#define UUPS_POWER_REQUEST_IGNORE_INTERRUPT                              PREQIG
#define UUPS_POWER_UP_TIMEOUT_INTERRUPT                                  PTMOUT

//*****************************************************************************
//
// The following are values that can be passed to the powerMode parameter for
// functions: UUPS_gotoPowerMode() as well as returned by the 
// UUPS_getPowerModeStatus() function.
//
//*****************************************************************************
#define UUPS_POWERMODE_OFF                                            UPSTATE_0
#define UUPS_POWERMODE_STANDBY                                        UPSTATE_1
#define UUPS_POWERMODE_TRANSITION                                     UPSTATE_2
#define UUPS_POWERMODE_READY                                          UPSTATE_3

//*****************************************************************************
//
// The following are values that can be passed to the triggerSource parameter for
// functions: UUPS_turnOnPower().
//
//*****************************************************************************
#define UUPS_POWERUP_TRIGGER_SOURCE_USSPWRUP                      USSPWRUPSEL_0
#define UUPS_POWERUP_TRIGGER_SOURCE_RTC                           USSPWRUPSEL_1
#define UUPS_POWERUP_TRIGGER_SOURCE_TIMER                         USSPWRUPSEL_2
#define UUPS_POWERUP_TRIGGER_SOURCE_EXTERNAL                      USSPWRUPSEL_3

//*****************************************************************************
//
// The following are values that can be returned by the UUPS_isBusy() function.
//
//*****************************************************************************
#define UUPS_BUSY                                                      USS_BUSY
#define UUPS_NOT_BUSY                                                       0x0

//*****************************************************************************
//
// The following are values that can be returned by the UUPS_isLDOReady() function.
//
//*****************************************************************************
#define UUPS_LDO_READY                                                   LDORDY
#define UUPS_LDO_NOT_READY                                                  0x0

//*****************************************************************************
//
// The following are values that can be passed to the holdOffDelay parameter for functions: UUPS_setLowPowerBiasHoldOffDelay().
//
//*****************************************************************************
#define UUPS_HOLD_OFF_DELAY_ADDITIONAL_NONE                             LBHDEL_0
#define UUPS_HOLD_OFF_DELAY_ADDITIONAL_512_REFCLKS                      LBHDEL_1
#define UUPS_HOLD_OFF_DELAY_ADDITIONAL_1024_REFCLKS                     LBHDEL_2
#define UUPS_HOLD_OFF_DELAY_ADDITIONAL_1536_REFCLKS                     LBHDEL_3
#define UUPS_HOLD_OFF_DELAY_ADDITIONAL_100_US                           LBHDEL_1
#define UUPS_HOLD_OFF_DELAY_ADDITIONAL_200_US                           LBHDEL_2
#define UUPS_HOLD_OFF_DELAY_ADDITIONAL_300_US                           LBHDEL_3

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Clears UUPS interrupts.
//!
//! The UUPS interrupt source is cleared, so that it no longer asserts. If the
//! interrupt is not enabled, the corresponding bit in the RIS register bit will
//! be cleared. If the interrupt is enabled, the corresponding bit both MIS and 
//! RIS registers will be both cleared.
//!
//! \param baseAddress is the base address of the UUPS module.
//! \param mask is the logical OR of any of the following:
//!        - \b UUPS_POWER_REQUEST_IGNORE_INTERRUPT - UUPS power request ignored interrupt
//!        - \b UUPS_POWER_UP_TIMEOUT_INTERRUPT - UUPS power up time out interrupt
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_clearInterrupt(uint16_t baseAddress, uint8_t mask);

//*****************************************************************************
//
//! \brief Enables UUPS interrupts.
//!
//! Enables the indicated UUPS interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor. <b>Does not clear interrupt flags.</b>
//!
//! \param baseAddress is the base address of the UUPS module.
//! \param mask is the logical OR of any of the following:
//!        - \b UUPS_POWER_REQUEST_IGNORE_INTERRUPT - UUPS power request ignored interrupt
//!        - \b UUPS_POWER_UP_TIMEOUT_INTERRUPT - UUPS power up time out interrupt
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_enableInterrupt(uint16_t baseAddress, uint8_t mask);

//*****************************************************************************
//
//! \brief Disables selected UUPS interrupt sources.
//!
//! Disables the indicated UUPS interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor.
//!
//! \param baseAddress is the base address of the UUPS module.
//! \param mask is the logical OR of any of the following:
//!        - \b UUPS_POWER_REQUEST_IGNORE_INTERRUPT - UUPS power request ignored interrupt
//!        - \b UUPS_POWER_UP_TIMEOUT_INTERRUPT - UUPS power up time out interrupt
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_disableInterrupt(uint16_t baseAddress, uint8_t mask);

//*****************************************************************************
//
//! \brief Gets the current UUPS interrupt status.
//!
//! This returns the interrupt status for the UUPS module based on which flag
//! is passed.
//!
//! \param baseAddress is the base address of the UUPS module.
//! \param mask is the logical OR of any of the following:
//!        - \b UUPS_POWER_REQUEST_IGNORE_INTERRUPT - UUPS power request ignored interrupt
//!        - \b UUPS_POWER_UP_TIMEOUT_INTERRUPT - UUPS power up time out interrupt
//!
//! \return Logical OR of any of the following:
//!        - \b UUPS_POWER_REQUEST_IGNORE_INTERRUPT - UUPS power request ignored interrupt
//!        - \b UUPS_POWER_UP_TIMEOUT_INTERRUPT - UUPS power up time out interrupt
//
//*****************************************************************************
extern uint8_t UUPS_getInterruptStatus(uint16_t baseAddress, uint16_t mask);

//*****************************************************************************
//
//! \brief Stops USS measurement.
//!
//! This stops the USS current measurement.
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_stopMeasurement(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Turns down USS power.
//!
//! This stops the current measurement and turn off USS module.
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_turnOffPower(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Turns on USS power and PLL.
//!
//! This turns on USS power and PLL with specified power-up trigger source. When
//! an external trigger source is selected, this function has no effect.
//!
//! \param baseAddress is the base address of the UUPS module.
//! \param triggerSource is the specified power-up trigger source.
//!        Valid values are:
//!        - \b UUPS_POWERUP_TRIGGER_SOURCE_USSPWRUP
//!        - \b UUPS_POWERUP_TRIGGER_SOURCE_RTC
//!        - \b UUPS_POWERUP_TRIGGER_SOURCE_TIMER
//!        - \b UUPS_POWERUP_TRIGGER_SOURCE_EXTERNAL
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_turnOnPower(uint16_t baseAddress, uint16_t triggerSource);

//*****************************************************************************
//
//! \brief Enables USS acquistion sequencer (ASQ).
//!
//! This enable the USS acquistion sequencer (ASQ). ASQ must be enabled before
//! applying a USS_PWRREQ signal. 
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_enableASQ(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Goes to the specified power mode.
//!
//! This function makes USS go to the specified power mode.
//!
//! \param baseAddress is the base address of the UUPS module.
//! \param powerMode is the desired power mode to go to.
//!
//! \return None
//
//*****************************************************************************
//extern void UUPS_gotoPowerMode(uint8_t powerMode);

//*****************************************************************************
//
//! \brief Gets current USS power mode status.
//!
//! This gets current USS power mode status.
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return Any of the following:
//!        - \b UUPS_POWERMODE_OFF
//!        - \b UUPS_POWERMODE_READY
//!        - \b UUPS_POWERMODE_STANDBY
//!        - \b UUPS_POWERMODE_TIMEOUT
//
//*****************************************************************************
extern uint8_t UUPS_getPowerModeStatus(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Returns current USS busy status.
//!
//! This returns current USS busy status depending on whether USS is performing
//! a measurement.
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return UUPS_BUSY or UUPS_NOT_BUSY
//
//*****************************************************************************
extern uint8_t UUPS_isBusy(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Returns whether USS LDO is ready.
//!
//! This returns USS LDO ready status.
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return UUPS_LDO_READY or UUPS_LDO_NOT_READY
//
//*****************************************************************************
extern uint8_t UUPS_isLDOReady(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Sets the low power bias hold off delay.
//!
//! Defines the duration of the hold off delay for low power bias mode. Low power bias mode can be enabled from SAPH module.
//! The hold off delay is inserted from "OFF state" to "READY state" and from "STANDBY state" to "READY state".
//! Affects extra delay in Register Mode and Auto Mode.
//! Reset type: PUC
//!
//! \param baseAddress is the base address of the UUPS module.
//! \param holdOffDelay is the desired amount of additional hold off delay. 512 REFCLKS = ~100us, 1024 REFCLKS = ~200us, 1536 REFCLKS = ~300us. They map to the same values.
//!        Valid values are:
//!        - \b UUPS_HOLD_OFF_DELAY_ADDITIONAL_NONE
//!        - \b UUPS_HOLD_OFF_DELAY_ADDITIONAL_512_REFCLKS
//!        - \b UUPS_HOLD_OFF_DELAY_ADDITIONAL_1024_REFCLKS
//!        - \b UUPS_HOLD_OFF_DELAY_ADDITIONAL_1536_REFCLKS
//!        - \b UUPS_HOLD_OFF_DELAY_ADDITIONAL_100_US
//!        - \b UUPS_HOLD_OFF_DELAY_ADDITIONAL_200_US
//!        - \b UUPS_HOLD_OFF_DELAY_ADDITIONAL_300_US
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_setLowPowerBiasHoldOffDelay(uint16_t baseAddress, uint16_t holdOffDelay);

//*****************************************************************************
//
//! \brief Hold USS (and sub modules) logic in reset state.
//!
//! Useful if HSPLL gets stuck. Cycle USS into reset state with this function, and then release from reset state with UUPS_releaseUSSFromResetState.
//! USS is default released from reset, and ready for operation (still required to be powered up).
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_holdUSSInResetState(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Release USS (and sub modules) from reset state for operation.
//!
//! Useful if HSPLL gets stuck. Cycle USS into reset state with this UUPS_holdUSSInResetState, and then release from reset state with this function.
//! USS is default released from reset, and ready for operation (still required to be powered up).
//!
//! \param baseAddress is the base address of the UUPS module.
//!
//! \return None
//
//*****************************************************************************
extern void UUPS_releaseUSSFromResetState(uint16_t baseAddress);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_UUPS_H__

