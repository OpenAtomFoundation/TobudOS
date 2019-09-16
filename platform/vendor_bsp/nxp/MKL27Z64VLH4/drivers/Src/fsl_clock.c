/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2017 , NXP
 * All rights reserved.
 *
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
 * o Neither the name of copyright holder nor the names of its
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

#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.clock"
#endif

#if (defined(OSC) && !(defined(OSC0)))
#define OSC0 OSC
#endif

#define MCG_HIRC_FREQ (48000000U)
#define MCG_LIRC_FREQ1 (2000000U)
#define MCG_LIRC_FREQ2 (8000000U)

#define MCG_S_CLKST_VAL ((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT)
#define MCG_SC_FCRDIV_VAL ((MCG->SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT)
#define MCG_MC_LIRC_DIV2_VAL ((MCG->MC & MCG_MC_LIRC_DIV2_MASK) >> MCG_MC_LIRC_DIV2_SHIFT)
#define MCG_C2_IRCS_VAL ((MCG->C2 & MCG_C2_IRCS_MASK) >> MCG_C2_IRCS_SHIFT)

#define SIM_CLKDIV1_OUTDIV1_VAL ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)
#define SIM_CLKDIV1_OUTDIV4_VAL ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> SIM_CLKDIV1_OUTDIV4_SHIFT)
#define SIM_SOPT1_OSC32KSEL_VAL ((SIM->SOPT1 & SIM_SOPT1_OSC32KSEL_MASK) >> SIM_SOPT1_OSC32KSEL_SHIFT)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* External XTAL0 (OSC0) clock frequency. */
uint32_t g_xtal0Freq;
/* External XTAL32K clock frequency. */
uint32_t g_xtal32Freq;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the current MCG_Lite LIRC_CLK frequency in Hz.
 *
 * This function will return the LIRC_CLK value in frequency(Hz) based
 * on current MCG_Lite configurations and settings. It is an internal function.
 *
 * @return MCG_Lite LIRC_CLK frequency.
 */
static uint32_t CLOCK_GetLircClkFreq(void);

/*!
 * @brief Get RANGE value based on OSC frequency.
 *
 * To setup external crystal oscillator, must set the register bits RANGE base
 * on the crystal frequency. This function returns the RANGE base on the input
 * frequency. This is an internal function.
 *
 * @return RANGE value.
 */
static uint8_t CLOCK_GetOscRangeFromFreq(uint32_t freq);

/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t CLOCK_GetLircClkFreq(void)
{
    static const uint32_t lircFreqs[] = {MCG_LIRC_FREQ1, MCG_LIRC_FREQ2};

    /* Check whether the LIRC is enabled. */
    if ((MCG->C1 & MCG_C1_IRCLKEN_MASK) || (kMCGLITE_ClkSrcLirc == MCG_S_CLKST_VAL))
    {
        return lircFreqs[MCG_C2_IRCS_VAL];
    }
    else
    {
        return 0U;
    }
}

static uint8_t CLOCK_GetOscRangeFromFreq(uint32_t freq)
{
    uint8_t range;

    if (freq <= 39063U)
    {
        range = 0U;
    }
    else if (freq <= 8000000U)
    {
        range = 1U;
    }
    else
    {
        range = 2U;
    }

    return range;
}

uint32_t CLOCK_GetOsc0ErClkFreq(void)
{
    if (OSC0->CR & OSC_CR_ERCLKEN_MASK)
    {
        /* Please call CLOCK_SetXtal0Freq base on board setting before using OSC0 clock. */
        assert(g_xtal0Freq);
        return g_xtal0Freq;
    }
    else
    {
        return 0U;
    }
}

uint32_t CLOCK_GetEr32kClkFreq(void)
{
    uint32_t freq;

    switch (SIM_SOPT1_OSC32KSEL_VAL)
    {
        case 0U: /* OSC 32k clock  */
            freq = (CLOCK_GetOsc0ErClkFreq() == 32768U) ? 32768U : 0U;
            break;
        case 2U: /* RTC 32k clock  */
            /* Please call CLOCK_SetXtal32Freq base on board setting before using XTAL32K/RTC_CLKIN clock. */
            assert(g_xtal32Freq);
            freq = g_xtal32Freq;
            break;
        case 3U: /* LPO clock      */
            freq = LPO_CLK_FREQ;
            break;
        default:
            freq = 0U;
            break;
    }
    return freq;
}

uint32_t CLOCK_GetPlatClkFreq(void)
{
    return CLOCK_GetOutClkFreq() / (SIM_CLKDIV1_OUTDIV1_VAL + 1);
}

uint32_t CLOCK_GetFlashClkFreq(void)
{
    uint32_t freq;

    freq = CLOCK_GetOutClkFreq() / (SIM_CLKDIV1_OUTDIV1_VAL + 1);
    freq /= (SIM_CLKDIV1_OUTDIV4_VAL + 1);

    return freq;
}

uint32_t CLOCK_GetBusClkFreq(void)
{
    uint32_t freq;

    freq = CLOCK_GetOutClkFreq() / (SIM_CLKDIV1_OUTDIV1_VAL + 1);
    freq /= (SIM_CLKDIV1_OUTDIV4_VAL + 1);

    return freq;
}

uint32_t CLOCK_GetCoreSysClkFreq(void)
{
    return CLOCK_GetOutClkFreq() / (SIM_CLKDIV1_OUTDIV1_VAL + 1);
}

uint32_t CLOCK_GetFreq(clock_name_t clockName)
{
    uint32_t freq;

    switch (clockName)
    {
        case kCLOCK_CoreSysClk:
        case kCLOCK_PlatClk:
            freq = CLOCK_GetOutClkFreq() / (SIM_CLKDIV1_OUTDIV1_VAL + 1);
            break;
        case kCLOCK_BusClk:
        case kCLOCK_FlashClk:
            freq = CLOCK_GetOutClkFreq() / (SIM_CLKDIV1_OUTDIV1_VAL + 1);
            freq /= (SIM_CLKDIV1_OUTDIV4_VAL + 1);
            break;
        case kCLOCK_Er32kClk:
            freq = CLOCK_GetEr32kClkFreq();
            break;
        case kCLOCK_Osc0ErClk:
            freq = CLOCK_GetOsc0ErClkFreq();
            break;
        case kCLOCK_McgInternalRefClk:
            freq = CLOCK_GetInternalRefClkFreq();
            break;
        case kCLOCK_McgPeriphClk:
        case kCLOCK_McgIrc48MClk:
            freq = CLOCK_GetPeriphClkFreq();
            break;
        case kCLOCK_LpoClk:
            freq = LPO_CLK_FREQ;
            break;
        default:
            freq = 0U;
            break;
    }

    return freq;
}

void CLOCK_SetSimConfig(sim_clock_config_t const *config)
{
    SIM->CLKDIV1 = config->clkdiv1;
    CLOCK_SetEr32kClock(config->er32kSrc);
}

bool CLOCK_EnableUsbfs0Clock(clock_usb_src_t src, uint32_t freq)
{
    bool ret = true;

    CLOCK_DisableClock(kCLOCK_Usbfs0);

    if (kCLOCK_UsbSrcExt == src)
    {
        SIM->SOPT2 &= ~SIM_SOPT2_USBSRC_MASK;
    }
    else
    {
        SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK;
    }

    CLOCK_EnableClock(kCLOCK_Usbfs0);

    if (kCLOCK_UsbSrcIrc48M == src)
    {
        USB0->CLK_RECOVER_IRC_EN = 0x03U;
        USB0->CLK_RECOVER_CTRL |= USB_CLK_RECOVER_CTRL_CLOCK_RECOVER_EN_MASK;
    }

    return ret;
}

uint32_t CLOCK_GetInternalRefClkFreq(void)
{
    uint8_t divider1 = MCG_SC_FCRDIV_VAL;
    uint8_t divider2 = MCG_MC_LIRC_DIV2_VAL;
    /* LIRC internal reference clock is selected*/
    return CLOCK_GetLircClkFreq() >> (divider1 + divider2);
}

uint32_t CLOCK_GetPeriphClkFreq(void)
{
    /* Check whether the HIRC is enabled. */
    if ((MCG->MC & MCG_MC_HIRCEN_MASK) || (kMCGLITE_ClkSrcHirc == MCG_S_CLKST_VAL))
    {
        return MCG_HIRC_FREQ;
    }
    else
    {
        return 0U;
    }
}

uint32_t CLOCK_GetOutClkFreq(void)
{
    uint32_t freq;

    switch (MCG_S_CLKST_VAL)
    {
        case kMCGLITE_ClkSrcHirc:
            freq = MCG_HIRC_FREQ;
            break;
        case kMCGLITE_ClkSrcLirc:
            freq = CLOCK_GetLircClkFreq() >> MCG_SC_FCRDIV_VAL;
            break;
        case kMCGLITE_ClkSrcExt:
            /* Please call CLOCK_SetXtal0Freq base on board setting before using OSC0 clock. */
            assert(g_xtal0Freq);
            freq = g_xtal0Freq;
            break;
        default:
            freq = 0U;
            break;
    }

    return freq;
}

mcglite_mode_t CLOCK_GetMode(void)
{
    mcglite_mode_t mode;

    switch (MCG_S_CLKST_VAL)
    {
        case kMCGLITE_ClkSrcHirc: /* HIRC */
            mode = kMCGLITE_ModeHirc48M;
            break;
        case kMCGLITE_ClkSrcLirc: /* LIRC */
            if (kMCGLITE_Lirc2M == MCG_C2_IRCS_VAL)
            {
                mode = kMCGLITE_ModeLirc2M;
            }
            else
            {
                mode = kMCGLITE_ModeLirc8M;
            }
            break;
        case kMCGLITE_ClkSrcExt: /* EXT  */
            mode = kMCGLITE_ModeExt;
            break;
        default:
            mode = kMCGLITE_ModeError;
            break;
    }

    return mode;
}

status_t CLOCK_SetMcgliteConfig(mcglite_config_t const *targetConfig)
{
    assert(targetConfig);

    /*
     * If switch between LIRC8M and LIRC2M, need to switch to HIRC mode first,
     * because could not switch directly.
     */
    if ((kMCGLITE_ClkSrcLirc == MCG_S_CLKST_VAL) && (kMCGLITE_ClkSrcLirc == targetConfig->outSrc) &&
        (MCG_C2_IRCS_VAL != targetConfig->ircs))
    {
        MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(kMCGLITE_ClkSrcHirc);
        while (kMCGLITE_ClkSrcHirc != MCG_S_CLKST_VAL)
        {
        }
    }

    /* Set configuration now. */
    MCG->SC = MCG_SC_FCRDIV(targetConfig->fcrdiv);
    MCG->MC = MCG_MC_HIRCEN(targetConfig->hircEnableInNotHircMode) | MCG_MC_LIRC_DIV2(targetConfig->lircDiv2);
    MCG->C2 = (MCG->C2 & ~MCG_C2_IRCS_MASK) | MCG_C2_IRCS(targetConfig->ircs);
    MCG->C1 = MCG_C1_CLKS(targetConfig->outSrc) | targetConfig->irclkEnableMode;

    /*
     * If external oscillator used and MCG_Lite is set to EXT mode, need to
     * wait for the OSC stable.
     */
    if ((MCG->C2 & MCG_C2_EREFS0_MASK) && (kMCGLITE_ClkSrcExt == targetConfig->outSrc))
    {
        while (!(MCG->S & MCG_S_OSCINIT0_MASK))
        {
        }
    }

    /* Wait for clock source change completed. */
    while (targetConfig->outSrc != MCG_S_CLKST_VAL)
    {
    }

    return kStatus_Success;
}

void CLOCK_InitOsc0(osc_config_t const *config)
{
    uint8_t range = CLOCK_GetOscRangeFromFreq(config->freq);

    OSC_SetCapLoad(OSC0, config->capLoad);
    OSC_SetExtRefClkConfig(OSC0, &config->oscerConfig);

    MCG->C2 = ((MCG->C2 & MCG_C2_IRCS_MASK) | MCG_C2_RANGE0(range) | (uint8_t)config->workMode);

    if ((kOSC_ModeExt != config->workMode) && (OSC0->CR & OSC_CR_ERCLKEN_MASK))
    {
        /* Wait for stable. */
        while (!(MCG->S & MCG_S_OSCINIT0_MASK))
        {
        }
    }
}

void CLOCK_DeinitOsc0(void)
{
    OSC0->CR = 0U;
    MCG->C2 &= MCG_C2_IRCS_MASK;
}
