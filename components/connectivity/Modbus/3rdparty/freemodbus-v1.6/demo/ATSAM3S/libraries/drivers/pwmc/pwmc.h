/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 *
 * \par Purpose
 *
 * Interface for configuration the Pulse Width Modulation Controller (PWM) peripheral.
 *
 * \par Usage
 *
 *    -# Configures PWM clocks A & B to run at the given frequencies using
 *       \ref PWMC_ConfigureClocks().
 *    -# Configure PWMC channel using \ref PWMC_ConfigureChannel(), \ref PWMC_ConfigureChannelExt()
 *       \ref PWMC_SetPeriod(), \ref PWMC_SetDutyCycle() and \ref PWMC_SetDeadTime().
 *    -# Enable & disable channel using \ref PWMC_EnableChannel() and
 *       \ref PWMC_DisableChannel().
 *    -# Enable & disable the period interrupt for the given PWM channel using
 *       \ref PWMC_EnableChannelIt() and \ref PWMC_DisableChannelIt().
 *    -# Enable & disable the selected interrupts sources on a PWMC peripheral
 *       using  \ref PWMC_EnableIt() and \ref PWMC_DisableIt().
 *    -# Control syncronous channel using \ref PWMC_ConfigureSyncChannel(),
 *       \ref PWMC_SetSyncChannelUpdatePeriod() and \ref PWMC_SetSyncChannelUpdateUnlock().
 *    -# Control PWM override output using \ref PWMC_SetOverrideValue(),
 *       \ref PWMC_EnableOverrideOutput() and \ref PWMC_DisableOverrideOutput().
 *    -# \ref Send data through the transmitter using \ref PWMC_WriteBuffer().
 *
 */

#ifndef PWMC_H
#define PWMC_H

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <board.h>

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/* PWM sub-bit field definition. begin */
/* TODO: remove them when AT91SAM3S4.h have the definitions. */
// PWM_CMRx
#define 	PWM_CMR0_CPRE_MCKA                 (0xB) // (PWMC_CH)
#define 	PWM_CMR0_CPRE_MCKB                 (0xC) // (PWMC_CH)
#define 	PWM_CMR1_CPRE_MCKA                 (0xB) // (PWMC_CH)
#define 	PWM_CMR2_CPRE_MCKA                 (0xB) // (PWMC_CH)
// PWM_SCM
#define 	PWM_SCM_UPDM_MODE0                (0x0 << 16) // (PWMC) Manual write of data and manual trigger of the update
#define 	PWM_SCM_UPDM_MODE1                (0x1 << 16) // (PWMC) Manual write of data and automatic trigger of the update
#define 	PWM_SCM_UPDM_MODE2                (0x2 << 16) // (PWMC) Automatic write of data and automatic trigger of the update
/* end */

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern void PWMC_ConfigureChannel(
    uint8_t channel,
    uint32_t prescaler,
    uint32_t alignment,
    uint32_t polarity);

extern void PWMC_ConfigureChannelExt(
    uint8_t channel,
    uint32_t prescaler,
    uint32_t alignment,
    uint32_t polarity,
    uint32_t countEventSelect,
    uint32_t DTEnable,
    uint32_t DTHInverte,
    uint32_t DTLInverte);

extern void PWMC_ConfigureClocks
    (uint32_t clka,
     uint32_t clkb,
     uint32_t mck);

extern void PWMC_SetPeriod(uint8_t channel, uint16_t period);

extern void PWMC_SetDutyCycle(uint8_t channel, uint16_t duty);

extern void PWMC_SetDeadTime(uint8_t channel, uint16_t timeH, uint16_t timeL);

extern void PWMC_ConfigureSyncChannel(
    uint32_t channels,
    uint32_t updateMode,
    uint32_t requestMode,
    uint32_t requestComparisonSelect);

extern void PWMC_SetSyncChannelUpdatePeriod(uint8_t period);

extern void PWMC_SetSyncChannelUpdateUnlock(void);

extern void PWMC_EnableChannel(uint8_t channel);

extern void PWMC_DisableChannel(uint8_t channel);

extern void PWMC_EnableChannelIt(uint8_t channel);

extern void PWMC_DisableChannelIt(uint8_t channel);

extern void PWMC_EnableIt(uint32_t sources1, uint32_t sources2);

extern void PWMC_DisableIt(uint32_t sources1, uint32_t sources2);

extern uint8_t PWMC_WriteBuffer(Pwm *pwmc,
    void *buffer,
    uint32_t length);

extern void PWMC_SetOverrideValue(uint32_t value);

extern void PWMC_EnableOverrideOutput(uint32_t value, uint32_t sync);

extern void PWMC_DisableOverrideOutput(uint32_t value, uint32_t sync);

extern void PWMC_SetFaultMode(uint32_t mode);

extern void PWMC_FaultClear(uint32_t fault);

extern void PWMC_SetFaultProtectionValue(uint32_t value);

extern void PWMC_EnableFaultProtection(uint32_t value);

extern void PWMC_ConfigureComparisonUnit(uint32_t x, uint32_t value, uint32_t mode);

extern void PWMC_ConfigureEventLineMode(uint32_t x, uint32_t mode);

#endif /* #ifndef PWMC_H */

