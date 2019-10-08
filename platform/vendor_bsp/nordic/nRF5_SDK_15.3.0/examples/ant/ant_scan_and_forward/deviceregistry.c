/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2015
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
 * @brief Device registry used in the scan and forward demo
 *
 * @defgroup ant_scan_and_forward_example ANT Scan and Forward Demo
 * @{
 * @ingroup nrf_ant_scan_and_forward
 *
 */

#include "deviceregistry.h"
#include "commands.h"

#define NODE_ID_TO_INDEX_CONVERT(node_id) (node_id % MAX_DEVICES)   /**< Hash function */

static deviceregistry_t device_registry;                            /**< Device resistry of all nodes */

/** @brief Initializes entries in the device registry to invalid
 *
 * @param[in] p_device Pointer to device to initialize
 */
static void dr_init_device(device_t * p_device)
{
    p_device->node_id                        = NODE_ID_INVALID;
    p_device->ant_channel                    = ANT_CHANNEL_NOT_CONNECTED;
    p_device->last_message_sequence_received = 0;
    p_device->application_state              = 0;
}


void dr_init(void)
{
    device_registry.count = 0;

    for (int i = 0; i < MAX_DEVICES; i++)
    {
        dr_init_device(&device_registry.devices[i]);
    }
}


bool dr_device_add(uint8_t node_id)
{
    uint8_t index = MAX_DEVICES;                       // Specifed maximum total number in the array
    uint8_t start = NODE_ID_TO_INDEX_CONVERT(node_id); // Start index of the array

    uint8_t id;

    for (int i = 0; i < MAX_DEVICES; i++ )
    {
        id = device_registry.devices[(start + i) % MAX_DEVICES].node_id;

        if (id == node_id) // If this node already exists, return false.
        {
            return false;
        }

        if (id == NODE_ID_INVALID) // If we found a place to put it
        {   // Find the best place to put this node (the place closest to the hash function result)
            index = (start + i) % MAX_DEVICES;
            break;
        }
    }

    if (index == MAX_DEVICES)
    {
        return false;
    }

    device_registry.devices[index].node_id = node_id;
    device_registry.count++;
    return true;
}


bool dr_device_remove(uint8_t node_id)
{
    uint8_t index = NODE_ID_TO_INDEX_CONVERT(node_id);
    uint8_t start = index;

    for (int i = 0; i < MAX_DEVICES; i++ )
    {
        if (device_registry.devices[(start + i) % MAX_DEVICES].node_id == node_id)
        {
            // Remove the device
            dr_init_device(&device_registry.devices[index]);
            device_registry.count--;
            return true;
        }
        else
            index++;
    }
    return false;
}


bool dr_is_full(void)
{
    return (device_registry.count >= MAX_DEVICES);
}


device_t * dr_device_get(uint8_t node_id)
{
    uint8_t index = NODE_ID_TO_INDEX_CONVERT(node_id);
    uint8_t start = index;

    for (int i = 0; i < MAX_DEVICES; i++ )
    {
        if (device_registry.devices[(start + i) % MAX_DEVICES].node_id == node_id)
        {
            return &(device_registry.devices[(start + i) % MAX_DEVICES]);
        }
        else
        {
            index++;
        }
    }
    return NULL; // Doesnt exist
}


device_t * dr_device_at_index_get(uint8_t index)
{
    return &(device_registry.devices[index]);
}


bool dr_device_exists(uint8_t node_id)
{
    uint8_t start = NODE_ID_TO_INDEX_CONVERT(node_id);

    for (int i = 0; i < MAX_DEVICES; i++ )
    {
        if (device_registry.devices[(start + i) % MAX_DEVICES].node_id == node_id)
        {
            return true;
        }
    }
    return false;
}


bool dr_device_at_index_exists(uint8_t index)
{
    return (device_registry.devices[index].node_id != NODE_ID_INVALID ? true : false);
}


uint8_t dr_index_of_node_get(uint8_t node_id)
{
    uint8_t start = NODE_ID_TO_INDEX_CONVERT(node_id);

    for (int i = 0; i < MAX_DEVICES; i++ )
    {
        if (device_registry.devices[(start + i) % MAX_DEVICES].node_id == node_id)
        {
            return (start + i) % MAX_DEVICES;
        }
    }
    return 0;
}


/**
 *@}
 **/
