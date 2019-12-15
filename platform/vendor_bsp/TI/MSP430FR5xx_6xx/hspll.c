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
// hspll.c - Driver for the HSPLL Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup hspll_api hspll
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_HSPLL__
#include "hspll.h"

#include <assert.h>

void HSPLL_init(uint16_t baseAddress, HSPLL_initParam *param)
{
    assert (param->multiplier > 15);
    assert (param->multiplier < 40);
    
    HWREG16(baseAddress + OFS_HSPLLCTL) =
        (param->multiplier << 10)
        | param->frequency 
        | param->lockStatus;
}

void HSPLL_xtalInit(uint16_t baseAddress, HSPLL_xtalInitParam *param)
{    
    HWREG16(baseAddress + OFS_HSPLLUSSXTLCTL) =
        param->oscillatorType
        | param->xtlOutput
        | param->oscillatorEnable;
}

uint16_t HSPLL_getInterruptStatus(uint16_t baseAddress)
{
    return (HWREG16(baseAddress + OFS_HSPLLRIS) & PLLUNLOCK);
}

uint16_t HSPLL_getInterruptMaskStatus(uint16_t baseAddress)
{
    return (HWREG16(baseAddress + OFS_HSPLLIMSC) & PLLUNLOCK);
}

void HSPLL_enableInterrupt(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_HSPLLIMSC) |= PLLUNLOCK;
}

void HSPLL_disableInterrupt(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_HSPLLIMSC) &= ~PLLUNLOCK;
}

void HSPLL_clearInterrupt(uint16_t baseAddress)        
{
    HWREG16(baseAddress + OFS_HSPLLICR) |= PLLUNLOCK;
}

void HSPLL_setInterrupt(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_HSPLLISR) |= PLLUNLOCK;
}

uint16_t HSPLL_getOscillatorStatus(uint16_t baseAddress)
{
    return (HWREG16(baseAddress + OFS_HSPLLUSSXTLCTL) & OSCSTATE);
}

uint16_t HSPLL_isLocked(uint16_t baseAddress)
{
    return (HWREG16(baseAddress + OFS_HSPLLCTL) & PLL_LOCK);
}


#endif
//*****************************************************************************
//
//! Close the doxygen group for hspll_api
//! @}
//
//*****************************************************************************

