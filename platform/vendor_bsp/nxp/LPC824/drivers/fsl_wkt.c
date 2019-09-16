/*
 * The Clear BSD License
 * Copyright 2018 NXP
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

#include "fsl_wkt.h"
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.wkt"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Gets the instance from the base address
 *
 * @param base WKT peripheral base address
 *
 * @return The WKT instance
 */
static uint32_t WKT_GetInstance(WKT_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to WKT bases for each instance. */
static WKT_Type *const s_wktBases[] = WKT_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to WKT clocks for each instance. */
static const clock_ip_name_t s_wktClocks[] = WKT_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! @brief Pointers to WKT resets for each instance. */
static const reset_ip_name_t s_wktResets[] = WKT_RSTS_N;

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t WKT_GetInstance(WKT_Type *base)
{
    uint32_t instance;
    uint32_t wktArrayCount = (sizeof(s_wktBases) / sizeof(s_wktBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < wktArrayCount; instance++)
    {
        if (s_wktBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < wktArrayCount);

    return instance;
}

void WKT_Init(WKT_Type *base, const wkt_config_t *config)
{
    assert(config);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the WKT peripheral clock */
    CLOCK_EnableClock(s_wktClocks[WKT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Reset the module */
    RESET_PeripheralReset(s_wktResets[WKT_GetInstance(base)]);

    /* Clear wake-up or alarm timer flag */
    WKT_ClearStatusFlags(WKT, kWKT_AlarmFlag);

#if defined(FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK) && FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK
    if (config->clockSource == kWKT_ExternalClockSource)
    {
        /* Select external clock source */
        base->CTRL |= WKT_CTRL_SEL_EXTCLK_MASK;
    }
    else
    {
#endif /* FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK */
        base->CTRL &= ~(
#if defined(FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK) && FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK
                         WKT_CTRL_SEL_EXTCLK_MASK | 
#endif /* FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK */
                         WKT_CTRL_CLKSEL_MASK);
        /* Select divided FRO clock or Low power clock */
        base->CTRL |= WKT_CTRL_CLKSEL(config->clockSource);
#if defined(FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK) && FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK
    }
#endif /* FSL_FEATURE_WKT_HAS_CTRL_SEL_EXTCLK */
}

void WKT_Deinit(WKT_Type *base)
{
    /* Stop the timer */
    WKT_StopTimer(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Gate the module clock */
    CLOCK_DisableClock(kCLOCK_Wkt);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}
