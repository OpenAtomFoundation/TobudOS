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
// uups.c - Driver for the UUPS Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup uups_api uups
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_UUPS__
#include "uups.h"

#include <assert.h>

void UUPS_clearInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_UUPSICR) |= mask;
}

void UUPS_enableInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_UUPSIMSC) |= mask;
}

void UUPS_disableInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_UUPSIMSC) &= ~mask;
}

uint8_t UUPS_getInterruptStatus(uint16_t baseAddress, uint16_t mask)
{
    return HWREG16(baseAddress + OFS_UUPSRIS) & mask;
}

void UUPS_stopMeasurement(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_UUPSCTL) |= USSSTOP;
}

void UUPS_turnOffPower(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_UUPSCTL) |= USSPWRDN;
}

void UUPS_turnOnPower(uint16_t baseAddress, uint16_t triggerSource)
{
    HWREG16(baseAddress + OFS_UUPSCTL) |= (triggerSource | USSPWRUP);
}

void UUPS_enableASQ(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_UUPSCTL) |= ASQEN;
}

uint8_t UUPS_getPowerModeStatus(uint16_t baseAddress)
{
    return HWREG16(baseAddress + OFS_UUPSCTL) & UPSTATE;
}

uint8_t UUPS_isBusy(uint16_t baseAddress)
{
    return HWREG16(baseAddress + OFS_UUPSCTL) & USS_BUSY;
}

uint8_t UUPS_isLDOReady(uint16_t baseAddress)
{
    return HWREG16(baseAddress + OFS_UUPSCTL) & LDORDY;
}

void UUPS_setLowPowerBiasHoldOffDelay(uint16_t baseAddress, uint16_t holdOffDelay)
{
    HWREG16(baseAddress + OFS_UUPSCTL) &= ~LBHDEL;
    HWREG16(baseAddress + OFS_UUPSCTL) |= holdOffDelay;
}

void UUPS_holdUSSInResetState(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_UUPSCTL) |= USSSWRST;
}

void UUPS_releaseUSSFromResetState(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_UUPSCTL) &= ~USSSWRST;
}

#endif
//*****************************************************************************
//
//! Close the doxygen group for uups_api
//! @}
//
//*****************************************************************************

