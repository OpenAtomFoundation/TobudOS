/*
 * Copyright 2017 NXP
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
 * o Neither the name of the copyright holder nor the names of its
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

#include "fsl_dac.h"
#include "fsl_clock.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_dac"
#endif


/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array of GPIO peripheral base address. */
static DAC_Type *const s_dacBases[] = DAC_BASE_PTRS;
/* Array of GPIO clock name. */
static const clock_ip_name_t s_dacClocks[] = DAC_CLOCKS;

/*******************************************************************************
* Prototypes
******************************************************************************/

/*!
* @brief Gets the DAC instance according to the DAC base.
*
* @param base DAC peripheral base pointer.
* @retval DAC instance.
*/
static uint32_t DAC_GetInstance(DAC_Type *base);

/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t DAC_GetInstance(DAC_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_dacBases); instance++)
    {
        if (s_dacBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_dacBases));

    return instance;
}

void DAC_Init(DAC_Type *base, const dac_config_t *config)
{
    assert(config != NULL);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable clock. */
    CLOCK_EnableClock(s_dacClocks[DAC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    base->CR &= ~DAC_CR_BIAS_MASK;
    base->CR |= DAC_CR_BIAS(config->settlingTime);
}

void DAC_Deinit(DAC_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable the clock. */
    CLOCK_DisableClock(s_dacClocks[DAC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

void DAC_GetDefaultConfig(dac_config_t *config)
{
    assert(config != NULL);

    config->settlingTime = kDAC_SettlingTimeIs1us;
}

void DAC_EnableDoubleBuffering(DAC_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= DAC_CTRL_DBLBUF_ENA_MASK;
        base->CTRL |= DAC_CTRL_CNT_ENA_MASK;
    }
    else
    {
        base->CTRL &= ~DAC_CTRL_DBLBUF_ENA_MASK;
        base->CTRL &= ~DAC_CTRL_CNT_ENA_MASK;
    }
}

void DAC_SetBufferValue(DAC_Type *base, uint32_t value)
{
    assert(value < 1024U);

    base->CR = (base->CR & ~DAC_CR_VALUE_MASK) | DAC_CR_VALUE(value);
}

void DAC_SetCounterValue(DAC_Type *base, uint32_t value)
{
    assert(value < 65536U);

    base->CNTVAL &= ~DAC_CNTVAL_VALUE_MASK;
    base->CNTVAL |= DAC_CNTVAL_VALUE(value);
}
