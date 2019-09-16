/*
 * Copyright (c) 2017, NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_common.h"
#include "fsl_reset.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.reset"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

#if ((defined(FSL_FEATURE_SOC_SYSCON_COUNT) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)) || \
     (defined(FSL_FEATURE_SOC_ASYNC_SYSCON_COUNT) && (FSL_FEATURE_SOC_ASYNC_SYSCON_COUNT > 0)))

void RESET_SetPeripheralReset(reset_ip_name_t peripheral)
{
    const uint32_t bitPos = ((uint32_t)peripheral & 0x0000FFFFu);
    const uint32_t bitMask = 1u << bitPos;

    assert(bitPos < 32u);

    /* reset register is in SYSCON */
    /* set bit */
    SYSCON->PRESETCTRL &= ~bitMask;
}

void RESET_ClearPeripheralReset(reset_ip_name_t peripheral)
{
    const uint32_t bitPos = ((uint32_t)peripheral & 0x0000FFFFu);
    const uint32_t bitMask = 1u << bitPos;

    assert(bitPos < 32u);

    /* reset register is in SYSCON */
     /* clear bit */
    SYSCON->PRESETCTRL |= bitMask;
}

void RESET_PeripheralReset(reset_ip_name_t peripheral)
{
    RESET_SetPeripheralReset(peripheral);
    RESET_ClearPeripheralReset(peripheral);
}

#endif /* FSL_FEATURE_SOC_SYSCON_COUNT || FSL_FEATURE_SOC_ASYNC_SYSCON_COUNT */
