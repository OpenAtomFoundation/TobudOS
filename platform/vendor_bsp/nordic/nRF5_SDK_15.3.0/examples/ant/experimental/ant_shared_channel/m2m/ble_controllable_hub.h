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
/** @file
 *
 * @defgroup ble_ant_srv_ach ANT Controllable Hub Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief Controllable Hub Service module.
 */

#ifndef BLE_ACHS_H__
#define BLE_ACHS_H__

#include <stdint.h>
#include "ble.h"
#include "asc_pages.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACH_CTRLPT_NACK_PROC_ALREADY_IN_PROGRESS   (BLE_GATT_STATUS_ATTERR_APP_BEGIN + 0)
#define ACH_CTRLPT_NACK_CCCD_IMPROPERLY_CONFIGURED (BLE_GATT_STATUS_ATTERR_APP_BEGIN + 1)

#define ACH_OP_COMMAND                    0                             /**< Opcode for cmd of a peripheral ID. */

#define ACH_CMD_REPORTING_MODE_OFF        0                             /**< Opcode representing the ANT generic command 32735. */
#define ACH_CMD_REPORTING_MODE_ON         1                             /**< Opcode representing the ANT generic command 32734. */
#define ACH_CMD_REPORTING_MODE_WARNINGS   2                             /**< Opcode representing the ANT generic command 32733. */

#define ACH_CMD_PERI_ON                   3                             /**< Opcode representing relayed hub page (240) to turn on a given peripheral. */
#define ACH_CMD_PERI_OFF                  4                             /**< Opcode representing relayed hub page (240) to turn off a given peripheral. */

#define ACH_CMD_GROUP_ASSIGN              5                             /**< Opcode representing relayed hub page (240) to assign a peripheral to a group. */

#define ACH_CMD_GROUP_ON                  6                             /**< */
#define ACH_CMD_GROUP_OFF                 7                             /**< */

typedef struct
{
    uint8_t op_code;
    uint8_t cmd[7];
} ble_ach_command_t;

/**@brief ANT Controllable Hub Service event type. */
typedef enum
{
    BLE_ACHS_EVT_NOTIFICATION_ENABLED,                                  /**< ANT Controllable Hub report notification enabled event.  */
    BLE_ACHS_EVT_NOTIFICATION_DISABLED                                  /**< ANT Controllable Hub report notification disabled event. */
} ble_achs_evt_type_t;

/**@brief ANT Controllable Hub Service event. */
typedef struct
{
    ble_achs_evt_type_t evt_type;                                       /**< Type of event. */
} ble_achs_evt_t;

// declaration of the ble_achs_t type.
typedef struct ble_achs_s ble_achs_t;

/**@brief ANT Controllable Hub Service event handler type. */
typedef void (*ble_achs_evt_handler_t) (ble_achs_t * p_achs, ble_achs_evt_t * p_evt);

/**@brief ANT Controllable Hub Service init structure. This contains all options and data
*         needed for initialization of the service. */
typedef struct
{
    ble_achs_evt_handler_t       evt_handler;                           /**< Event handler to be called for handling events in the ANT Controllable Hub Service. */
} ble_achs_init_t;

/**@brief ANT Controllable Hub Service structure. This contains various status information for
 *        the service. */
struct ble_achs_s
{
    ble_achs_evt_handler_t       evt_handler;                           /**< Event handler to be called for handling events in the ANT Controllable Hub Service. */
    uint16_t                     conn_handle;                           /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
};

/**@brief ANT Controllable Hub Service report structure. */
typedef struct
{
    uint8_t  page_num;
#ifdef TWO_BYTE_SHARED_ADDRESS
    uint16_t shared_address;
#else
    uint8_t  shared_address;
#endif
    uint16_t master_id;
    uint8_t  value;
} ble_achs_report_t;

/**@brief Function for initializing the ANT Controllable Hub Service.
 *
 * @param[out]  p_achs      ANT Controllable Hub Service structure. This structure will have to
 *                          be supplied by the application. It will be initialized by this function,
 *                          and will later be used to identify this particular service instance.
 * @param[in]   p_achs_init Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_achs_init(ble_achs_t * p_achs, const ble_achs_init_t * p_achs_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the ANT Controllable Hub
 *          Service.
 *
 * @param[in]   p_achs     ANT Controllable Hub Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_achs_on_ble_evt(ble_achs_t * p_achs, ble_evt_t const * p_ble_evt);

/**@brief Function for sending ANT controllable hub report if notification has been enabled.
 *
 * @details The application calls this function after having received an ANT Controllable Hub
 *          Service report. If notification has been enabled, the report data is encoded
 *          and sent to the client.
 *
 * @param[in]   p_achs         Cycling Speed and Cadence Service structure.
 * @param[in]   p_report       Pointer to new ANT controllable hub report.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_achs_report_send(ble_achs_t * p_achs, ble_achs_report_t * p_report);


/**@brief Function to get the ANT controllable hub event bitfield.
 *
 * @note After using this function and checking for an event, be sure to clear that event immediately.
 *       Returns event codes as defined in asc_events.h
 *
 * @return A copy of the current event bitfield.
 */
uint32_t ble_achs_events_get(void);

/**@brief Function to clear an event from the BLE controllable hub event bitfield.
 *
 * @param[in] event     The event to be cleared from the BLE controllable hub event bitfield.
 */
void ble_achs_event_clear(uint32_t event);

/**@brief Gets the last command received over the ble connection.
 *
 * @return A copy of the last command received over the ble connection.
 */
asc_command_data_t ble_achs_get_last_command(void);



#ifdef __cplusplus
}
#endif

#endif // BLE_ACHS_H
