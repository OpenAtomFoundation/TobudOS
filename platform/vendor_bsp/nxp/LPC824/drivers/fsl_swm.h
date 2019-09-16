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
#ifndef _FSL_SWM_H_
#define _FSL_SWM_H_

#include "fsl_swm_connections.h"

/*!
 * @addtogroup swm
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_SWM_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0 */
/*@}*/

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Assignment of digital peripheral functions to pins.
 *
 * This function will selects a pin (designated by its GPIO port and
 * bit numbers) to a function.
 *
 * @param base SWM peripheral base address.
 * @param func any function name that is movable.
 * @param swm_port_pin any pin which has a GPIO port number and bit number.
 */
void SWM_SetMovablePinSelect(SWM_Type *base, swm_select_movable_t func, swm_port_pin_type_t swm_port_pin);

#if (defined(FSL_FEATURE_SWM_HAS_PINASSIGNFIXED0_REGISTER) && (FSL_FEATURE_SWM_HAS_PINASSIGNFIXED0_REGISTER == 1))
/*!
 * @brief Assignment of digital peripheral functions to pins.
 *
 * This function will selects a pin (designated by its GPIO port and
 * bit numbers) to a function.
 *
 * @param base SWM peripheral base address.
 * @param func any function name that is movable.
 * @param swm_port_pin any pin which has a GPIO port number and bit number.
 */
void SWM_SetFixedMovablePinSelect(SWM_Type *base, swm_select_fixed_movable_t func, swm_fixed_port_pin_type_t swm_port_pin);
#endif /* FSL_FEATURE_SWM_HAS_PINASSIGNFIXED0_REGISTER */

/*!
 * @brief Enable the fixed-pin function.
 *
 * This function will enables a fixed-pin function in PINENABLE0 or PINENABLE1.
 *
 * @param base SWM peripheral base address.
 * @param func any function name that is fixed pin.
 * @param enable enable or disable.
 */
void SWM_SetFixedPinSelect(SWM_Type *base, swm_select_fixed_pin_t func, bool enable);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_SWM_H_ */
