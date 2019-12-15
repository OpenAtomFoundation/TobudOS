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
#include "asc_device_registry.h"

////////////////////////////////////////////////////////////////////////////////
// Private Definitions
////////////////////////////////////////////////////////////////////////////////
#define CAPABILITIES_UNSET                      ((uint8_t) 0x00)
#define CONVERT_ADDRESS_TO_INDEX(ucAddress)     (ucAddress - 1)
#define CONVERT_INDEX_TO_ADDRESS(ucIndex)       (ucIndex + 1)

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////


/** @brief Sets the event flag with the event value specified.
 *
 * @param[in] p_registry    The registry to set the event flag on.
 *
 * @param[in] event         The event value to be set.
 */
static void deviceregistry_set_event(asc_device_registry_t * const p_registry, uint32_t event)
{
    p_registry->event_flags |= event;
}

////////////////////////////////////////////////////////////////////////////////
// deviceregistry_update_highest_registered_address
//
// Updates the highest_registered_address value by searching the registry from
// the top down for the first used address.
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
////////////////////////////////////////////////////////////////////////////////
static void deviceregistry_update_highest_registered_address(asc_device_registry_t * const p_registry)
{
    uint32_t index = CONVERT_ADDRESS_TO_INDEX(MAX_DEVICES);
    while (p_registry->devices[index].shared_address == INVALID_SHARED_ADDRESS)
    {
        if (index == 0)
        {
            break;
        }
        index--;
    }
    p_registry->highest_registered_address = CONVERT_INDEX_TO_ADDRESS(index);
}

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
void deviceregistry_clear_event(asc_device_registry_t * const p_registry, uint32_t event)
{
    p_registry->event_flags &= ~event;
}

void deviceregistry_setup(asc_device_registry_t * const p_registry)
{
    p_registry->count = 0;
    p_registry->highest_registered_address = 0;
    p_registry->event_flags = 0;

    uint32_t index = 0;
    while (index <= MAX_DEVICES)
    {
//lint --e{661} Possible access of out-of-bounds pointer (1 beyond end of data) by operator
        p_registry->devices[index].shared_address = INVALID_SHARED_ADDRESS;
        index++;
    }
}
////////////////////////////////////////////////////////////////////////////////
// deviceregistry_getnextfreesharedaddress
//
// Returns the next free shared address in the device registry
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
//
// Returns:
//    shared_address - lowest free shared address in the registry that is greater than the highest registered address.
////////////////////////////////////////////////////////////////////////////////
uint8_t deviceregistry_get_next_free_shared_address(asc_device_registry_t * const p_registry)
{
   if (p_registry->highest_registered_address < MAX_DEVICES) //Get the next free shared address that has not yet been used
   {
        return p_registry->highest_registered_address + 1;
   }
   else if (p_registry->count < MAX_DEVICES)  //All shared addresses used at least once, check if there exists a shared address we can recycle
   {
      uint32_t i;
      //Scroll through entire registry until we find a free shared address
      for (i = 0; i < MAX_DEVICES; i++)
      {
         if (p_registry->devices[i].shared_address == INVALID_SHARED_ADDRESS)
         {
            return CONVERT_INDEX_TO_ADDRESS(i);
         }
      }
   }

   return INVALID_SHARED_ADDRESS;
}

////////////////////////////////////////////////////////////////////////////////
// deviceregistry_get_first_registered_shared_address
//
// Returns shared address of the first registered device in the registry
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
//
// Returns:
//    next_address - shared address of the first registerd device
////////////////////////////////////////////////////////////////////////////////
uint8_t deviceregistry_get_first_registered_shared_address(asc_device_registry_t * const p_registry)
{
   bool address_found = false;
   uint8_t next_address = INVALID_SHARED_ADDRESS;
   uint32_t index = 0;

   //Get the first registered shared address we see
   while ((index < MAX_DEVICES) && !address_found)
   {
      if (p_registry->devices[index].shared_address != INVALID_SHARED_ADDRESS)
      {
         next_address = CONVERT_INDEX_TO_ADDRESS(index);
         address_found = true;
      }
      else
      {
         index++;
      }
   }

   return next_address;
}

////////////////////////////////////////////////////////////////////////////////
// deviceregistry_get_next_registered_shared_address
//
// Returns the next registered shared address in the device registry
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
//    previous_shared_address -unsigned char which is the past shared address that was registered
//
// Returns:
//    next_address - next registered shared address in the registry
////////////////////////////////////////////////////////////////////////////////
uint8_t deviceregistry_get_next_registered_shared_address(asc_device_registry_t* p_registry, uint8_t previous_shared_address)
{
    bool address_found = false;
    uint8_t next_address = INVALID_SHARED_ADDRESS;
    uint32_t index = CONVERT_ADDRESS_TO_INDEX(previous_shared_address);

    //Get the next shared address that has been registered
    //TODO - Implement an improved iterator that doesn't need to scan the entire register to check whether there is a device left to return
    index++;
    while ((index < MAX_DEVICES) && !address_found)
    {
        if (p_registry->devices[index].shared_address != INVALID_SHARED_ADDRESS)
        {
            next_address = CONVERT_INDEX_TO_ADDRESS(index);
            address_found = true;
        }
        else
        {
            index++;
        }
    }

    return next_address;
}

////////////////////////////////////////////////////////////////////////////////
// deviceregistry_add_device
//
// Sends requested command to a group of peripheral devices
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
//    shared_address -unsigned char which is the shared address of the device to delete
//
// Returns:
//    true - device was successfully removed
//    false - device was not successfully removed
////////////////////////////////////////////////////////////////////////////////
bool deviceregistry_add_device(asc_device_registry_t * const p_registry, asc_device_t * p_device)
{
    bool success = false;
    uint32_t index = CONVERT_ADDRESS_TO_INDEX(p_device->shared_address);
    //Check that the shared address is not already in use
    if (p_registry->devices[index].shared_address == INVALID_SHARED_ADDRESS)
    {
        //Add the device
        p_registry->devices[index].shared_address = p_device->shared_address;
        p_registry->devices[index].serial_number = p_device->serial_number;
        p_registry->devices[index].hw_revision = p_device->hw_revision;
        p_registry->devices[index].sw_revision = p_device->sw_revision;
        p_registry->devices[index].model_number = p_device->model_number;
        if (p_device->shared_address > p_registry->highest_registered_address)
        {
          p_registry->highest_registered_address = p_device->shared_address;
        }
        p_registry->count++;
        deviceregistry_set_event(p_registry, DEVICEREGISTRY_EVENT_DEVICE_ADDED);
        success = true;
    }
    return success;
}

////////////////////////////////////////////////////////////////////////////////
// deviceregistry_remove_device
//
// Removes a specified device from a device registry
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
//    shared_address -unsigned char which is the shared address of the device to delete
//
// Returns:
//    true - device was successfully removed
//    false - device was not successfully removed
////////////////////////////////////////////////////////////////////////////////
bool deviceregistry_remove_device(asc_device_registry_t * const p_registry, uint8_t shared_address)
{
    bool success = false;
    uint32_t index = CONVERT_ADDRESS_TO_INDEX(shared_address);
    //Check that the device exists for us to remove
    if (p_registry->devices[index].shared_address != INVALID_SHARED_ADDRESS)
    {
        //Remove the device
        p_registry->devices[index].shared_address = INVALID_SHARED_ADDRESS;
        p_registry->devices[index].hw_revision = 0;
        p_registry->devices[index].serial_number = 0;
        p_registry->devices[index].sw_revision = 0;
        p_registry->devices[index].model_number = 0;
        p_registry->devices[index].missed_polls = 0;
        p_registry->devices[index].poll_count = 0;
        p_registry->count--;

        deviceregistry_set_event(p_registry, DEVICEREGISTRY_EVENT_DEVICE_REMOVED);
        success = true;
    }

    deviceregistry_update_highest_registered_address(p_registry);
    return success;
}

////////////////////////////////////////////////////////////////////////////////
// deviceregistry_is_full
//
// Checks whether the device registry is full
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
//
// Returns:
//    true - the registry is full
//    false - the registry is not full
////////////////////////////////////////////////////////////////////////////////
bool deviceregistry_is_full(asc_device_registry_t * const p_registry)
{
    if (p_registry->count >= MAX_DEVICES)
    {
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// DeviceRegistry_GetDevice
//
// Returns a pointer the the registered device with a specified shared address.
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in
//    shared_address -unsigned char which is the shared address of the device to return
//
// Returns:
//    a pointer to the device which has a shared address that matches ucSharedAddress
////////////////////////////////////////////////////////////////////////////////
asc_device_t* deviceregistry_get_device(asc_device_registry_t * const p_registry, uint8_t shared_address)
{
   return &(p_registry->devices[CONVERT_ADDRESS_TO_INDEX(shared_address)]);
}

////////////////////////////////////////////////////////////////////////////////
// DeviceRegistry_IsDeviceRegistered
//
// Indicates whether a shared address is currently registered or not.
//
// Parameters:
//    p_registry - pointer to the device registry in which the registered devices are contained in.
//    shared_address -unsigned char which is the shared address of the device to search for.
//
// Returns:
//    True is the shared address is registered, false otherwise.
////////////////////////////////////////////////////////////////////////////////
bool deviceregistry_is_device_registered(asc_device_registry_t * const p_registry, uint8_t shared_address)
{
    return ((p_registry->devices[CONVERT_ADDRESS_TO_INDEX(shared_address)]).shared_address != INVALID_SHARED_ADDRESS);
}
