/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016, NXP
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
#ifndef _FSL_POWER_H_
#define _FSL_POWER_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief power driver version 2.0.0. */
#define FSL_POWER_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

typedef enum pd_bits
{
    kPDRUNCFG_PD_IRC_OUT = SYSCON_PDRUNCFG_IRCOUT_PD_MASK,
    kPDRUNCFG_PD_IRC = SYSCON_PDRUNCFG_IRC_PD_MASK,
    kPDRUNCFG_PD_FLASH = SYSCON_PDRUNCFG_FLASH_PD_MASK,
    kPDRUNCFG_PD_BOD = SYSCON_PDRUNCFG_BOD_PD_MASK,
    kPDRUNCFG_PD_ADC0 = SYSCON_PDRUNCFG_ADC_PD_MASK,
    kPDRUNCFG_PD_SYSOSC = SYSCON_PDRUNCFG_SYSOSC_PD_MASK,
    kPDRUNCFG_PD_WDT_OSC = SYSCON_PDRUNCFG_WDTOSC_PD_MASK,
    kPDRUNCFG_PD_SYSPLL = SYSCON_PDRUNCFG_SYSPLL_PD_MASK,
    kPDRUNCFG_PD_ACMP = SYSCON_PDRUNCFG_ACMP_MASK,

    /*
    This enum member has no practical meaning,it is used to avoid MISRA issue,
    user should not trying to use it.
    */
    kPDRUNCFG_ForceUnsigned = 0x80000000U,
} pd_bit_t;

/* Power mode configuration API parameter */
typedef enum _power_mode_config
{
    kPmu_Sleep = 0U,
    kPmu_Deep_Sleep = 1U,
    kPmu_PowerDown = 2U,
    kPmu_Deep_PowerDown = 3U,
} power_mode_cfg_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
* @name Power Configuration
* @{
*/

/*!
 * @brief API to enable PDRUNCFG bit in the Syscon. Note that enabling the bit powers down the peripheral
 *
 * @param en    peripheral for which to enable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_EnablePD(pd_bit_t en)
{
    SYSCON->PDRUNCFG |= en;
}

/*!
 * @brief API to disable PDRUNCFG bit in the Syscon. Note that disabling the bit powers up the peripheral
 *
 * @param en    peripheral for which to disable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_DisablePD(pd_bit_t en)
{
    SYSCON->PDRUNCFG &= ~en;
}

/*!
 * @brief API to enable deep sleep bit in the ARM Core.
 *
 * @param none
 * @return none
 */
static inline void POWER_EnableDeepSleep(void)
{
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

/*!
 * @brief API to disable deep sleep bit in the ARM Core.
 *
 * @param none
 * @return none
 */
static inline void POWER_DisableDeepSleep(void)
{
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

#ifdef __cplusplus
}
#endif

#endif /* _FSL_POWER_H_ */
