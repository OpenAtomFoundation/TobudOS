/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2014
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
/**@file
 * @defgroup asc_events
 * @{
 * @ingroup ant_auto_shared_channel
 *
 * @brief Header file containing definitions of Auto Shared Channel events
 */

#ifndef ASC_EVENT_H__
#define ASC_EVENT_H__

#include <stdint.h>
#include "compiler_abstraction.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup asc_events/Event Definitions
 *
 * @brief Describes the possible events that an ASC device may call
 *
 * @{
 */
#define EVENT_ASC_STATE_CHANGED         ((uint32_t)0x00000001)  /**< Event flag to indicate that the ASC device has changed its state. */
#define EVENT_ASC_DEVICE_IN_WRONG_STATE ((uint32_t)0x00000002)  /**< Event flag to indicate that an illegal attempt was made to change the state of the ASC device. */
#define EVENT_ASC_LIGHT_STATE_CHANGED   ((uint32_t)0x00000004)  /**< Event flag to indicate that the ASC device's light state has changed. */
#define EVENT_ASC_UPDATE_RECEIVED       ((uint32_t)0x00000008)  /**< Event flag to indicate that an update was received from an ASC device. */
#define EVENT_ASC_REQUEST_RECEIVED      ((uint32_t)0x00000010)  /**< Event flag to indicate that a request was received from an ASC device. */
#define EVENT_ASC_COMMAND_RECEIVED      ((uint32_t)0x00000020)  /**< Event flag to indicate that a request was received from an ASC device. */
#define EVENT_ASC_DATA_TIMEOUT          ((uint32_t)0x00000040)  /**< Event flag to indicate that the ASC device's data timeout has occurred. */
/** @} */


/** @brief Sets the ASC event flag with the event value specified.
 *
 * @param[out] p_asc_event_flags    Pointer to the flag bitfield to modify.
 *
 * @param[in]  event                The event value to set in the event flags bitfield.
 */
static void __INLINE asc_event_set(uint32_t * p_asc_event_flags, uint32_t event)
{
    *p_asc_event_flags |= event;
}


/** @brief Clears the ASC event flag with the event value specified.
 *
 * @param[out] p_asc_event_flags    Pointer to the flag bitfield to modify.
 *
 * @param[in]  event                The event value to clear from the event flags bitfield.
 */
static void __INLINE asc_event_clear(uint32_t * p_asc_event_flags, uint32_t event)
{
    *p_asc_event_flags &= ~event;
}


#ifdef __cplusplus
}
#endif

#endif //ASC_EVENT_H__

/** @} */
