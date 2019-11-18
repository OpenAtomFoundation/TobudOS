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
// sdhs.c - Driver for the SDHS Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup sdhs_api sdhs
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SDHS__
#include "sdhs.h"

#include <assert.h>

void SDHS_init(uint16_t baseAddress, SDHS_initParam *param)
{
   //Unlock Registers
   HWREG16(baseAddress + OFS_SDHSCTL3) &= ~TRIGEN;
   HWREG16(baseAddress + OFS_SDHSCTL5) &= ~SDHS_LOCK;

    //Set SDHS Control 0
    HWREG16(baseAddress + OFS_SDHSCTL0) =
        param->triggerSourceSelect
        | param->msbShift 
        | param->outputBitResolution
        | param->dataFormat
        | param->dataAlignment
        | param->interruptDelayGeneration
        | param->autoSampleStart;
    
    //Set SDHS Control 1
    HWREG16(baseAddress + OFS_SDHSCTL1) =  param->oversamplingRate;
    
    //Set SDHS Control 2
    HWREG16(baseAddress + OFS_SDHSCTL2) = 
        param->dataTransferController
        | param->windowComparator
        | param->sampleSizeCounting;
}

void SDHS_setWindowComp(uint16_t baseAddress, uint16_t highThreshold,
    uint16_t lowThreshold)
{
    uint16_t saveCTL3 = HWREG16(baseAddress + OFS_SDHSCTL3);
    uint16_t saveCTL5 = HWREG16(baseAddress + OFS_SDHSCTL5);
    
    //Unlock Registers
    HWREG16(baseAddress + OFS_SDHSCTL3) &= ~TRIGEN;
    HWREG16(baseAddress + OFS_SDHSCTL5) &= ~SDHS_LOCK;
   
    HWREG16(baseAddress + OFS_SDHSWINHITH) = highThreshold;
    HWREG16(baseAddress + OFS_SDHSWINLOTH) = lowThreshold;
    
    //Restore CTL3 and CTL5
    HWREG16(baseAddress + OFS_SDHSCTL3) = saveCTL3;
    HWREG16(baseAddress + OFS_SDHSCTL5) = saveCTL5;
}

void SDHS_setTotalSampleSize(uint16_t baseAddress, uint16_t sampleSize)
{
    uint16_t saveCTL3 = HWREG16(baseAddress + OFS_SDHSCTL3);
    uint16_t saveCTL5 = HWREG16(baseAddress + OFS_SDHSCTL5);
    
    //Unlock Registers
    HWREG16(baseAddress + OFS_SDHSCTL3) &= ~TRIGEN;
    HWREG16(baseAddress + OFS_SDHSCTL5) &= ~SDHS_LOCK;
   
    HWREG16(baseAddress + OFS_SDHSCTL2) = sampleSize - 1;
}

void SDHS_enableTrigger(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SDHSCTL3) |= TRIGEN;
}

void SDHS_disableTrigger(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SDHSCTL3) &= ~TRIGEN;
}

void SDHS_enable(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SDHSCTL4) |= SDHSON;
}

void SDHS_disable(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SDHSCTL4) &= ~SDHSON;
}

uint16_t SDHS_getInterruptStatus(uint16_t baseAddress, uint16_t interruptMask)
{
    return (HWREG16(baseAddress + OFS_SDHSRIS) & interruptMask);
}

uint16_t SDHS_getInterruptMaskStatus(uint16_t baseAddress, uint16_t interruptMask)
{
    return (HWREG16(baseAddress + OFS_SDHSIMSC) & interruptMask);
}

void SDHS_enableInterrupt(uint16_t baseAddress, uint16_t interruptMask)
{
    HWREG16(baseAddress + OFS_SDHSIMSC) |= interruptMask;
}
void SDHS_disableInterrupt(uint16_t baseAddress, uint16_t interruptMask)
{
    HWREG16(baseAddress + OFS_SDHSIMSC) &= ~interruptMask;
}

void SDHS_clearInterrupt(uint16_t baseAddress, uint16_t interruptMask)
{
    HWREG16(baseAddress + OFS_SDHSICR) |= interruptMask;
}

void SDHS_setInterrupt(uint16_t baseAddress, uint16_t interruptMask)
{
    HWREG16(baseAddress + OFS_SDHSISR) |= interruptMask;
}

void SDHS_setPGAGain(uint16_t baseAddress, uint16_t gain)
{
    assert (gain < 0x40);
    HWREG16(baseAddress + OFS_SDHSCTL6) = gain;
}

void SDHS_setModularOptimization(uint16_t baseAddress, uint16_t optimization)
{
    assert (optimization < 0x20);
    HWREG16(baseAddress + OFS_SDHSCTL7) = optimization;
}
uint16_t SDHS_getRegisterLockStatus(uint16_t baseAddress)
{
    return (HWREG16(baseAddress + OFS_SDHSCTL5) & SDHS_LOCK);
}

void SDHS_startConversion(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SDHSCTL5) |= SSTART;
}

void SDHS_endConversion(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SDHSCTL5) &= ~SSTART;
}

uint16_t SDHS_getResults(uint16_t baseAddress)
{
    return (HWREG16(baseAddress + OFS_SDHSDT));
}

void SDHS_setDTCDestinationAddress(uint16_t baseAddress, uint16_t address)
{
    HWREG16(baseAddress + OFS_SDHSDTCDA) = address;
}
#endif
//*****************************************************************************
//
//! Close the doxygen group for sdhs_api
//! @}
//
//*****************************************************************************

