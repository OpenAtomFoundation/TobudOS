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
#ifndef ASC_DEVICE_REGISTRY_H__
#define ASC_DEVICE_REGISTRY_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**@todo add two byte address support */

////////////////////////////////////////////////////////////////////////////////
// Module Description
////////////////////////////////////////////////////////////////////////////////

/*
 * This module is responsible for management of the device registry used to store information about peripheral devices that have registered wih the hub
 */


////////////////////////////////////////////////////////////////////////////////
// Public Definitions
////////////////////////////////////////////////////////////////////////////////
#define MAX_DEVICES                             ((uint8_t) 0x0F)
#define INVALID_SHARED_ADDRESS                  ((uint8_t) 0x00)

#define DEVICEREGISTRY_EVENT_DEVICE_ADDED       ((uint32_t)0x00000001)
#define DEVICEREGISTRY_EVENT_DEVICE_REMOVED     ((uint32_t)0x00000002)

////////////////////////////////////////////////////////////////////////////////
// Public structs
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint32_t    shared_address;
    uint32_t    serial_number;
    uint16_t    model_number;
    uint8_t     hw_revision;
    uint8_t     sw_revision;
    uint8_t     missed_polls;
    uint8_t     poll_count;
} asc_device_t;

typedef struct
{
    uint8_t         count;                         //Total number of registered devices
    uint32_t        event_flags;
    uint8_t         highest_registered_address;    //The highest valued address that is registered
    asc_device_t    devices[MAX_DEVICES];
} asc_device_registry_t;


////////////////////////////////////////////////////////////////////////////////
// Public Function Prototypes
////////////////////////////////////////////////////////////////////////////////
void deviceregistry_clear_event(asc_device_registry_t * const p_registry, uint32_t event);

void deviceregistry_setup(asc_device_registry_t * p_registry);

uint8_t deviceregistry_get_next_free_shared_address(asc_device_registry_t * p_registry);

uint8_t deviceregistry_get_first_registered_shared_address(asc_device_registry_t * p_registry);

uint8_t deviceregistry_get_next_registered_shared_address(asc_device_registry_t * p_registry, uint8_t previous_shared_address);

bool deviceregistry_add_device(asc_device_registry_t * p_registry, asc_device_t* p_device);

bool deviceregistry_remove_device(asc_device_registry_t * p_registry, uint8_t shared_address);

bool deviceregistry_is_full(asc_device_registry_t * p_registry);

asc_device_t* deviceregistry_get_device(asc_device_registry_t * p_registry, uint8_t shared_address);

bool deviceregistry_is_device_registered(asc_device_registry_t * p_registry, uint8_t shared_address);


#ifdef __cplusplus
}
#endif

#endif /* ASC_DEVICE_REGISTRY_H__ */

