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
#ifndef _FSL_WKT_H_
#define _FSL_WKT_H_

#include "fsl_common.h"

/*!
 * @addtogroup wkt
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_WKT_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0 */
/*@}*/

/*! @brief Describes WKT clock source. */
typedef enum _wkt_clock_source
{
    kWKT_DividedFROClockSource = 0U, /*!< WKT clock sourced from the divided FRO clock */
    kWKT_LowPowerClockSource = 1U,   /*!< WKT clock sourced from the Low power clock 
                                          Use this clock, LPOSCEN bit of DPDCTRL register must be enabled */
    kWKT_ExternalClockSource = 2U,   /*!< WKT clock sourced from the Low power clock
                                          Use this clock, WAKECLKPAD_DISABLE bit of DPDCTRL register must be enabled */
} wkt_clock_source_t;

/*! @brief Describes WKT configuration structure. */
typedef struct _wkt_config
{
    wkt_clock_source_t clockSource;    /*!< External or internal clock source select */
} wkt_config_t;

/*! @brief List of WKT flags */
typedef enum _wkt_status_flags
{
    kWKT_AlarmFlag = WKT_CTRL_ALARMFLAG_MASK, /*!< Alarm flag */
} wkt_status_flags_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Ungates the WKT clock and configures the peripheral for basic operation.
 *
 * @note This API should be called at the beginning of the application using the WKT driver.
 *
 * @param base   WKT peripheral base address
 * @param config Pointer to user's WKT config structure.
 */
void WKT_Init(WKT_Type *base, const wkt_config_t *config);

/*!
 * @brief Gate the WKT clock
 *
 * @param base WKT peripheral base address
 */
void WKT_Deinit(WKT_Type *base);

/*!
 * @brief Initializes the WKT configuration sturcture.
 *
 * This function initializes the WKT configuration structure to default values. The default
 * values are as follows.
 * @code
 *   config->clockSource = kWKT_DividedFROClockSource;
 * @endcode
 *
 * @param config Pointer to the WKT configuration structure.
 * @see wdog_config_t
 */
static inline void WKT_GetDefaultConfig(wkt_config_t *config)
{
    assert(config);

    /* Select divided FRO clock. */
    config->clockSource = kWKT_DividedFROClockSource;
}

/*! @}*/

/*!
 * @name Read the counter value.
 * @{
 */

/*!
 * @brief Read actual WKT counter value.
 *
 * @param base WKT peripheral base address
 */
static inline uint32_t WKT_GetCounterValue(WKT_Type *base)
{
    /* Get the counter. */
    return (base->COUNT);
}

/*! @}*/

/*!
 * @name Status Interface
 * @{
 */

/*!
 * @brief Gets the RTC status flags
 *
 * @param base WKT peripheral base address
 *
 * @return The status flags. This is the logical OR of members of the
 *         enumeration ::wkt_status_flags_t
 */
static inline uint32_t WKT_GetStatusFlags(WKT_Type *base)
{
    return (base->CTRL & WKT_CTRL_ALARMFLAG_MASK);
}

/*!
 * @brief  Clears the WKT status flags.
 *
 * @param base WKT peripheral base address
 * @param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::wkt_status_flags_t
 */
static inline void WKT_ClearStatusFlags(WKT_Type *base, uint32_t mask)
{
    base->CTRL |= mask;
}

/*! @}*/

/*!
 * @name Timer Start and Stop
 * @{
 */

/*!
 * @brief Starts the timer counting.
 *
 * After calling this function, timer loads a count value, counts down to 0, then stops. 
 *
 * @note User can call the utility macros provided in fsl_common.h to convert to ticks
 *       Do not write to Counter register while the counting is in progress         
 *
 * @param base WKT peripheral base address.
 * @param count The value to be loaded into the WKT Count register
 */
static inline void WKT_StartTimer(WKT_Type *base, uint32_t count)
{
    /* Set the start count value */
    base->COUNT = count;
}

/*!
 * @brief Stops the timer counting.
 *
 * This function Clears the counter and stops the timer from counting.
 *
 * @param base WKT peripheral base address
 */
static inline void WKT_StopTimer(WKT_Type *base)
{
    /* Clear the counter and stop the timer immediately */
    base->CTRL = (base->CTRL & (~WKT_CTRL_ALARMFLAG_MASK)) | WKT_CTRL_CLEARCTR_MASK;
}

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_RTC_H_ */
