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

#ifndef __FSL_DAC_H__
#define __FSL_DAC_H__

#include "fsl_common.h"

/*!
 * @addtogroup lpc_dac
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief DAC driver version 2.0.0. */
#define LPC_DAC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*!
* @brief The DAC settling time.
*/
typedef enum _dac_settling_time
{
    kDAC_SettlingTimeIs1us = 0U,  /*!< The settling time of the DAC is 1us max, and the maximum
                                       current is 700 mA. This allows a maximum update rate of 1 MHz. */
    kDAC_SettlingTimeIs25us = 1U, /*!< The settling time of the DAC is 2.5us and the maximum current
                                       is 350uA. This allows a maximum update rate of 400 kHz. */
} dac_settling_time_t;

/*!
* @brief The configuration of DAC.
*/
typedef struct _dac_config
{
    dac_settling_time_t settlingTime; /*!< The settling times are valid for a capacitance load on the DAC_OUT pin not
                                           exceeding 100 pF. A load impedance value greater than that value will cause
                                           settling time longer than the specified time. One or more graphs of load
                                           impedance vs. settling time will be included in the final data sheet. */
} dac_config_t;

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
* @brief Initialize the DAC module.
*
* @param base DAC peripheral base address.
* @param config The pointer to configuration structure. Please refer to "dac_config_t" structure.
*/
void DAC_Init(DAC_Type *base, const dac_config_t *config);

/*!
* @brief De-Initialize the DAC module.
*
* @param base DAC peripheral base address.
*/
void DAC_Deinit(DAC_Type *base);

/*!
* @brief Initializes the DAC user configuration structure.
*
* This function initializes the user configuration structure to a default value. The default values are as follows.
* @code
*   config->settlingTime = kDAC_SettlingTimeIs1us;
* @endcode
* @param config Pointer to the configuration structure. See "dac_config_t".
*/
void DAC_GetDefaultConfig(dac_config_t *config);

/*!
* @brief Enable/Diable double-buffering feature. Notice: Disabling the double-buffering feature will disable counter
* opreation.
*        If double-buffering feature is disabled, any writes to the CR address will go directly to the CR register.
*        If double-buffering feature is enabled, any write to the CR register will only load the pre-buffer,
*        which shares its register address with the CR register. The CR itself will be loaded from the pre-buffer
*        whenever the counter reaches zero and the DMA request is set.
*
* @param base DAC peripheral base address.
* @param enable Enable or disable the feature.
*/
void DAC_EnableDoubleBuffering(DAC_Type *base, bool enable);

/*!
* @brief Write DAC output value into CR register or pre-buffer. The DAC output voltage is VALUE*((VREFP)/1024).
*
* @param base DAC peripheral base address.
* @param value Setting the value for items in the buffer. 10-bits are available.
*/
void DAC_SetBufferValue(DAC_Type *base, uint32_t value);

/*!
* @brief Write DAC counter value into CNTVAL register.
*        When the counter is enabled bit, the 16-bit counter will begin counting down, at the rate selected by PCLK,
*        from the value programmed into the DACCNTVAL register. The counter is decremented Each time the counter reaches
*        zero, the counter will be reloaded by the value of DACCNTVAL and the DMA request bit INT_DMA_REQ will be set in
*        hardware.
*
* @param base DAC peripheral basic address.
* @param value Setting the value for items in the counter. 16-bits are available.
*/
void DAC_SetCounterValue(DAC_Type *base, uint32_t value);

#if defined(FSL_FEATURE_DAC_HAS_CTRL_DMA_ENA) && FSL_FEATURE_DAC_HAS_CTRL_DMA_ENA
/*!
* @breif Enable/Disable the DMA access.
*
* @param base DAC peripheral base address.
* @param enable Enable or disable the feature.
*/
static inline void DAC_EnableDMA(DAC_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= DAC_CTRL_DMA_ENA_MASK;
    }
    else
    {
        base->CTRL &= ~DAC_CTRL_DMA_ENA_MASK;
    }
}
#endif /* FSL_FEATURE_DAC_HAS_CTRL_DMA_ENA */

/*!
* @breif Enable/Disable the counter operation.
*
* @param base DAC peripheral base address.
* @param enable Enable or disable the feature.
*/
static inline void DAC_EnableCounter(DAC_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= DAC_CTRL_CNT_ENA_MASK;
    }
    else
    {
        base->CTRL &= ~DAC_CTRL_CNT_ENA_MASK;
    }
}

/*!
* @brief Get the status flag of DMA or interrupt request.
*
* @param base DAC peripheral base address.
* @return If return 'true', it means DMA request or interrupt occurs.
*         If return 'false', it means DMA request or interrupt doesn't occur.
*/
static inline bool DAC_GetDMAInterruptRequestFlag(DAC_Type *base)
{
#if defined(FSL_FEATURE_DAC_HAS_CTRL_DMA_ENA) && FSL_FEATURE_DAC_HAS_CTRL_DMA_ENA
    if (0U != (DAC_CTRL_INT_DMA_REQ_MASK & base->CTRL))
#else
    if (0U != (DAC_CTRL_INT_CPU_REQ_MASK & base->CTRL))
#endif /* FSL_FEATURE_DAC_HAS_CTRL_DMA_ENA */
    {
        return true;
    }
    else
    {
        return false;
    }
}

#if defined(__cplusplus)
}
#endif

/* @} */

#endif /* __FSL_DAC_H__ */
