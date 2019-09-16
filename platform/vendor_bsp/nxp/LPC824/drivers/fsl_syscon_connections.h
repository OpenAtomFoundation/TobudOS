/*
 * The Clear BSD License
 * Copyright (c) 2018, NXP Semiconductors, Inc.
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

#ifndef _FSL_SYSCON_CONNECTIONS_
#define _FSL_SYSCON_CONNECTIONS_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.syscon_connections"
#endif

/*!
 * @addtogroup syscon
 * @{
 */

/*!
 * @name Syscon multiplexing connections
 * @{
 */

/*! @brief Periphinmux IDs */
#define PINTSEL_ID 0x178U
#define SYSCON_SHIFT 20U

/*! @brief SYSCON connections type */
typedef enum _syscon_connection_t
{
    /*!< Pin Interrupt. */
    kSYSCON_GpioPort0Pin0ToPintsel = 0U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin1ToPintsel = 1U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin2ToPintsel = 2U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin3ToPintsel = 3U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin4ToPintsel = 4U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin5ToPintsel = 5U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin6ToPintsel = 6U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin7ToPintsel = 7U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin8ToPintsel = 8U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin9ToPintsel = 9U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin10ToPintsel = 10U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin11ToPintsel = 11U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin12ToPintsel = 12U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin13ToPintsel = 13U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin14ToPintsel = 14U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin15ToPintsel = 15U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin16ToPintsel = 16U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin17ToPintsel = 17U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin18ToPintsel = 18U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin19ToPintsel = 19U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin20ToPintsel = 20U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin21ToPintsel = 21U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin22ToPintsel = 22U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin23ToPintsel = 23U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin24ToPintsel = 24U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin25ToPintsel = 25U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin26ToPintsel = 26U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin27ToPintsel = 27U + (PINTSEL_ID << SYSCON_SHIFT),
    kSYSCON_GpioPort0Pin28ToPintsel = 28U + (PINTSEL_ID << SYSCON_SHIFT),
} syscon_connection_t;

/*@}*/

#endif /* _FSL_SYSCON_CONNECTIONS_ */
