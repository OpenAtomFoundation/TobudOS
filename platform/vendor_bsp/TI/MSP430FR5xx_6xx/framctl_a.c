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
// framctl_a.c - Driver for the framctl_a Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup framctl_a_api framctl_a
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_FRCTL_A__
#include "framctl_a.h"

#include <assert.h>

void FRAMCtl_A_write8(uint8_t *dataPtr,
    uint8_t *framPtr,
    uint16_t numberOfBytes
    )
{
    while (numberOfBytes > 0)
    {
        //Write to Fram
        *framPtr++ = *dataPtr++;
        numberOfBytes--;
    }
}

void FRAMCtl_A_write16(uint16_t *dataPtr,uint16_t *framPtr,
		uint16_t numberOfWords)
{
    while (numberOfWords > 0)
    {
        //Write to Fram
        *framPtr++ = *dataPtr++;
        numberOfWords--;
    }
}

void FRAMCtl_A_write32(uint32_t *dataPtr,uint32_t *framPtr,
		uint16_t count)
{
    while (count > 0)
    {
        //Write to Fram
        *framPtr++ = *dataPtr++;
        count--;
    }
}

void FRAMCtl_A_fillMemory32 (uint32_t value,
    uint32_t *framPtr,
    uint16_t count
    )
{
    while (count> 0)
    {
        //Write to Fram
        *framPtr++ = value;
        count--;
    }
}

void FRAMCtl_A_enableInterrupt (uint8_t interruptMask)
{
    uint8_t temp = HWREG8(FRCTL_A_BASE + OFS_FRCTL0_L);

    // Clear lock in FRAM control registers
    HWREG16(FRCTL_A_BASE + OFS_FRCTL0) = FRCTLPW | temp;

    // Enable user selected interrupt sources
    HWREG16(FRCTL_A_BASE + OFS_GCCTL0) |= interruptMask;
}

uint8_t FRAMCtl_A_getInterruptStatus(uint16_t interruptFlagMask)
{
    return ( HWREG16(FRCTL_A_BASE + OFS_GCCTL1) & interruptFlagMask );
}

void FRAMCtl_A_disableInterrupt(uint16_t interruptMask)
{
    uint8_t temp = HWREG8(FRCTL_A_BASE + OFS_FRCTL0_L);

    //Clear lock in FRAM control registers
    HWREG16(FRCTL_A_BASE + OFS_FRCTL0) = FRCTLPW | temp;

    HWREG16(FRCTL_A_BASE + OFS_GCCTL0) &= ~(interruptMask);
}

void FRAMCtl_A_clearInterrupt(uint16_t interruptFlagMask)
{
    uint8_t temp = HWREG8(FRCTL_A_BASE + OFS_FRCTL0_L);

    //Clear lock in FRAM control registers
    HWREG16(FRCTL_A_BASE + OFS_FRCTL0) = FRCTLPW | temp;

    HWREG16(FRCTL_A_BASE + OFS_GCCTL1) &= ~interruptFlagMask;
}

void FRAMCtl_A_configureWaitStateControl(uint8_t waitState )
{
    uint8_t temp = HWREG8(FRCTL_A_BASE + OFS_FRCTL0_L);
    
    temp &= ~NWAITS_15;
    temp |= waitState;
    HWREG16(FRCTL_A_BASE + OFS_FRCTL0) = FRCTLPW | temp;
}

void FRAMCtl_A_delayPowerUpFromLPM(uint8_t delayStatus)
{
    uint8_t temp = HWREG8(FRCTL_A_BASE + OFS_FRCTL0_L);

    // Clear lock in FRAM control registers
    HWREG16(FRCTL_A_BASE + OFS_FRCTL0) = FRCTLPW | temp;

    HWREG8(FRCTL_A_BASE + OFS_GCCTL0_L) &= ~FRPWR;
    HWREG8(FRCTL_A_BASE + OFS_GCCTL0_L) |= delayStatus;
}

void FRAMCtl_A_enableWriteProtection(void)
{
    uint8_t temp = HWREG8(FRCTL_A_BASE + OFS_FRCTL0_L);
    temp |= WPROT;
    HWREG16(FRCTL_A_BASE + OFS_FRCTL0) = FRCTLPW | temp;
}

void FRAMCtl_A_disableWriteProtection(void)
{
    uint8_t temp = HWREG8(FRCTL_A_BASE + OFS_FRCTL0_L);
    temp &= ~WPROT;
    HWREG16(FRCTL_A_BASE + OFS_FRCTL0) = FRCTLPW | temp;
}

#endif
//*****************************************************************************
//
//! Close the doxygen group for framctl_a_api
//! @}
//
//*****************************************************************************
