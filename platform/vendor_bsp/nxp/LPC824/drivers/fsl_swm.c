/*
 * The Clear BSD License
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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

#include "fsl_swm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.swm"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void SWM_SetMovablePinSelect(SWM_Type *base, swm_select_movable_t func, swm_port_pin_type_t swm_port_pin)
{
    /* Check arguments */
    assert(NULL != base);
    assert(func < kSWM_MOVABLE_NUM_FUNCS);

    uint32_t temp;
    uint32_t pinassign = 0;
    uint32_t shifter = 0;

    pinassign = func / 4;
    shifter = (func % 4) * 8;

    temp = base->PINASSIGN_DATA[pinassign];
    temp &= ~(0xFF << (shifter));
    temp |= (swm_port_pin << shifter);
    base->PINASSIGN_DATA[pinassign] = temp;
}

#if (defined(FSL_FEATURE_SWM_HAS_PINASSIGNFIXED0_REGISTER) && (FSL_FEATURE_SWM_HAS_PINASSIGNFIXED0_REGISTER == 1))
void SWM_SetFixedMovablePinSelect(SWM_Type *base,
                                  swm_select_fixed_movable_t func,
                                  swm_fixed_port_pin_type_t swm_port_pin)
{
    /* Check arguments */
    assert(NULL != base);
    assert(func < kSWM_PINASSINGNFIXED_MOVABLE_NUM_FUNCS);

    uint32_t temp;
    uint32_t shifter = 0;

    shifter = func * 2;

    temp = base->PINASSIGNFIXED0;
    temp &= ~(0x03 << (shifter));
    temp |= (swm_port_pin << shifter);
    base->PINASSIGNFIXED0 = temp;
}
#endif /* FSL_FEATURE_SWM_HAS_PINASSIGNFIXED0_REGISTER */

void SWM_SetFixedPinSelect(SWM_Type *base, swm_select_fixed_pin_t func, bool enable)
{
    /* Check arguments */
    assert(NULL != base);
    assert((func > 0) || func < kSWM_FIXEDPIN_NUM_FUNCS);

    if (enable)
    {
        if (!(func & 0x80000000U))
        {
            base->PINENABLE0 &= ~(func);
        }
#if (defined(FSL_FEATURE_SWM_HAS_PINENABLE1_REGISTER) && (FSL_FEATURE_SWM_HAS_PINENABLE1_REGISTER == 1))
        else
        {
            base->PINENABLE1 &= ~(func);
        }
#endif /* FSL_FEATURE_SWM_HAS_PINENABLE1_REGISTER */
    }
    else
    {
        if (!(func & 0x80000000U))
        {
            base->PINENABLE0 |= (func);
        }
#if (defined(FSL_FEATURE_SWM_HAS_PINENABLE1_REGISTER) && (FSL_FEATURE_SWM_HAS_PINENABLE1_REGISTER == 1))
        else
        {
            base->PINENABLE1 |= (func);
        }
#endif /* FSL_FEATURE_SWM_HAS_PINENABLE1_REGISTER */
    }
}
