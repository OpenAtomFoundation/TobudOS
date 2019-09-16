/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
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

#include "fsl_acomp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_acomp"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the ACOMP instance from the peripheral base address.
 *
 * @param base ACOMP peripheral base address.
 * @return ACOMP instance.
 */
static uint32_t ACOMP_GetInstance(ACOMP_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Array of ACOMP peripheral base address. */
static ACOMP_Type *const s_acompBases[] = ACOMP_BASE_PTRS;
/* Clock name of ACOMP. */
static const clock_ip_name_t s_acompClock[] = ACMP_CLOCKS;

/*******************************************************************************
 * Codes
 ******************************************************************************/
static uint32_t ACOMP_GetInstance(ACOMP_Type *base)
{
    uint32_t instance = 0U;
    uint32_t acompArrayCount = (sizeof(s_acompBases) / sizeof(s_acompBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < acompArrayCount; instance++)
    {
        if (s_acompBases[instance] == base)
        {
            break;
        }
    }

    return instance;
}

void ACOMP_Init(ACOMP_Type *base, const acomp_config_t *config)
{
    assert(NULL != config);
    assert(NULL != base);

    uint32_t tmp32;

    /* Open clock gate. */
    CLOCK_EnableClock(s_acompClock[ACOMP_GetInstance(base)]);

    /* Write CTRL register. */
    tmp32 = base->CTRL & ~(ACOMP_CTRL_COMPSA_MASK | ACOMP_CTRL_HYS_MASK);
    tmp32 |= ACOMP_CTRL_HYS(config->hysteresisSelection);
    if (config->enableSyncToBusClk)
    {
        tmp32 |= ACOMP_CTRL_COMPSA_MASK;
    }
    base->CTRL = tmp32;
}

void ACOMP_Deinit(ACOMP_Type *base)
{
    /* Disable clock gate. */
    CLOCK_DisableClock(s_acompClock[ACOMP_GetInstance(base)]);
}

void ACOMP_GetDefaultConfig(acomp_config_t *config)
{
    assert(NULL != config);

    config->enableSyncToBusClk = false;
    config->hysteresisSelection = kACOMP_HysteresisNoneSelection;
}

void ACOMP_EnableInterrupts(ACOMP_Type *base, acomp_interrupt_enable_t enable)
{
#if defined(FSL_FEATURE_ACOMP_HAS_CTRL_INTENA) && FSL_FEATURE_ACOMP_HAS_CTRL_INTENA
    if (enable == kACOMP_InterruptsDisable)
    {
        base->CTRL &= ~ACOMP_CTRL_INTENA_MASK;
    }
    else
#endif /*FSL_FEATURE_ACOMP_HAS_CTRL_INTENA*/
    {
        base->CTRL = (base->CTRL & ~ACOMP_CTRL_EDGESEL_MASK) | ACOMP_CTRL_EDGESEL(enable)
#if defined(FSL_FEATURE_ACOMP_HAS_CTRL_INTENA) && FSL_FEATURE_ACOMP_HAS_CTRL_INTENA
                     | ACOMP_CTRL_INTENA_MASK
#endif /*FSL_FEATURE_ACOMP_HAS_CTRL_INTENA*/
            ;
    }
}

void ACOMP_SetLadderConfig(ACOMP_Type *base, const acomp_ladder_config_t *config)
{
    if (NULL == config)
    {
        base->LAD = 0U;
    }
    else
    {
        base->LAD =
            ACOMP_LAD_LADEN_MASK | ACOMP_LAD_LADSEL(config->ladderValue) | ACOMP_LAD_LADREF(config->referenceVoltage);
    }
}
