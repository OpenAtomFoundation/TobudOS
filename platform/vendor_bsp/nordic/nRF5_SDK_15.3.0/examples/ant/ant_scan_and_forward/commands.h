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
 * @brief Commands used in ANT Scan and Forward
 *
 * @defgroup ant_scan_and_forward_example ANT Scan and Forward Demo
 * @{
 * @ingroup nrf_ant_scan_and_forward
 *
 */

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#ifdef __cplusplus
extern "C" {
#endif

// Page numbers

#define DATA_PAGE_IND                   0
#define MOBILE_COMMAND_PAGE             ((uint8_t) 0x10)    /**< Data page for sending control commands */
#define DEVICE_STATUS_PAGE              ((uint8_t) 0x20)    /**< Data page for sending the device status */
#define INTERNAL_COMMAND_PAGE           ((uint8_t) 0x30)    /**< Data page for sending control commands inside the network */

// Page Indices for Device Status Data Page

#define DEVICE_STATUS_NODE_IND          1
#define DEVICE_STATUS_SEQ_NUM_IND       6
#define DEVICE_STATUS_STATE_IND         7

// Page Indices for Internal Command Data Page

#define INTERNAL_CMD_DST_IND            1
#define INTERNAL_CMD_SEQ_NUM_IND        6
#define INTERNAL_CMD_CMD_IND            7
#define INTERNAL_CMD_CMD_DATA1_IND      5
#define INTERNAL_CMD_CMD_DATA2_IND      4

// Page Indices for Mobile Command Data Page

#define MOBILE_CMD_DST_IND              2
#define MOBILE_CMD_CMD_IND              7
#define MOBILE_CMD_CMD_DATA1_IND        5                   /**< Configuration Command - Command Page Rotation % */
#define MOBILE_CMD_CMD_DATA2_IND        4                   /**< Configuration Command - Command Page Hi Priority */

// Page Indices for BLE Command Page

#define BLE_CMD_DST_IND                 1
#define BLE_CMD_DATA0_IND               2
#define BLE_CMD_DATA1_IND               3
#define BLE_CMD_DATA2_IND               4

// Device status

#define STATUS_LIGHT_OFF                ((uint8_t) 0x00)    /**< Device light is currently off */
#define STATUS_LIGHT_ON                 ((uint8_t) 0x01)    /**< Device light is currently on */

// Device command - Light

#define COMMAND_LIGHT_OFF               ((uint8_t) 0x00)    /**< Turn off device light */
#define COMMAND_LIGHT_ON                ((uint8_t) 0x01)    /**< Turn on device light */

// Device command - Configuration

#define COMMAND_CHG_CMD_PG_SET          ((uint8_t) 0x03)    /**< Change the device's internal page rotation */
#define SUBCOMMAND_ENABLE_HI_PRI_CMD    ((uint8_t) 0x01)    /**< Enable the high priority command page feature */
#define SUBCOMMAND_DISABLE_HI_PRI_CMD   ((uint8_t) 0x00)    /**< Disable the high priority command page feature */

#define ADDRESS_ALL_NODES               ((uint8_t) 0x00)                /**< Send command to all nodes  */

#define RESERVED                        ((uint8_t) 0xFF)                /**< Reserved/Invalid value  */


#ifdef __cplusplus
}
#endif

#endif

/**
 *@}
 **/
