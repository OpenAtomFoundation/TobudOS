/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "service_if.h"
#include <stdint.h>
#include "nrf_log.h"


/* Static variables for each service will be generated here
//static ble_dummy_service_t       m_dummy;


// One handler function per service will be generated for handling service events.
static void on_rns_evt(ble_dummy_service_t     * p_dummy_service,
                       ble_dummy_service_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_dummy_SERVICE_EVT_NOTIFICATION_ENABLED:
            NRF_LOG_INFO("[Bluetooth_IF]: DUMMY evt NOTIFICATION_ENABLED. ");
            break;
        case BLE_dummy_SERVICE_EVT_NOTIFICATION_DISABLED:
            NRF_LOG_INFO("[Bluetooth_IF]: DUMMY evt NOTIFICATION_DISABLED. ");
            break;
        case BLE_dummy_SERVICE_dummy_EVT_CCCD_WRITE:
            NRF_LOG_INFO("[Bluetooth_IF]: DUMMY evt CCCD_WRITE.");
            break;
        default:
            // No implementation needed.
            break;
    }
}
*/

// Init function will have generated one init block per service
uint32_t bluetooth_init(void)
{
    return NRF_SUCCESS;
}

// Generic handler function for ble events will call each service's individual on_ble_evt function
void bluetooth_on_ble_evt(ble_evt_t const * p_ble_evt)
{

}
