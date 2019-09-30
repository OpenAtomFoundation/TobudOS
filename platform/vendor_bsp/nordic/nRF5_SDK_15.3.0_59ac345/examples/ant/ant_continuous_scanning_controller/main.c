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
/*
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "nrf.h"
#include "app_error.h"
#include "app_timer.h"
#include "bsp.h"
#include "boards.h"
#include "hardfault.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "nrf_pwr_mgmt.h"
#include "ant_channel_config.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "commands.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_TIMER_PRESCALER           0                                             /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE       2u                                            /**< Size of timer operation queues. */

#define SCAN_TIMER_TICKS            APP_TIMER_TICKS(500u)                           /**< Scan timer ticks. */

#define MAX_RETRIES                 ((uint8_t) 5)                                   /**< Max retries. */
#define MAX_DEVICES                 ((uint8_t) 16)                                  /**< Maximum number of nodes supported in this network. */

#define CONVERT_DEVICE_NUM_TO_INDEX(device_number) (device_number % MAX_DEVICES)    /**< Hash function. */

#define APP_ANT_OBSERVER_PRIO       1                                               /**< Application's ANT observer priority. You shouldn't need to modify this value. */

typedef struct
{
    uint8_t rssi;               /**< RSSI. */
    uint16_t device_number;     /**< Device number. */
}node_t;


static uint8_t m_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];  /**< Transmit buffer. */
static node_t  m_node_list[MAX_DEVICES];                     /**< List of state of all devices in the network. */

static uint8_t m_command;                                    /**< Command. */
static uint8_t m_retries;                                    /**< Retries. */
static bool    m_wait_for_timeout;                           /**< The application does not respond to buttons when this flag is true. Because each button press starts scanning, another press could be too fast and try to start another scan when the previous one is still active. */

APP_TIMER_DEF(m_scan_timer_id);                              /**< Scan timer. */


/**@brief Send command to node with highest RSSI.
 * If a global command was selected, send to all.
 */
static void command_send(void)
{
    uint16_t device_number = 0;
    uint8_t  max_rssi      = 0;
    uint32_t err_code;

    // Loop through nodes and pick highest rssi node
    for (int node = 0; node < MAX_DEVICES; node++)
    {
        if (m_node_list[node].device_number != 0)
        {
            if (m_node_list[node].rssi > max_rssi)
            {
                max_rssi      = m_node_list[node].rssi;
                device_number = m_node_list[node].device_number;
            }
        }
    }

    // Do not send the command if no device was found
    // Close scanning channel and return
    if ( device_number == 0 )
    {
        bsp_board_led_on(BSP_BOARD_LED_2);
        bsp_board_led_off(BSP_BOARD_LED_0);
        err_code = sd_ant_channel_close(ANT_SCAN_CHANNEL_NUMBER);
        APP_ERROR_CHECK(err_code);
        return;
    }
    else
    {
        bsp_board_led_off(BSP_BOARD_LED_2);
    }

    err_code = sd_ant_channel_id_set(ANT_RESPONSE_CHANNEL_NUMBER,
                                     device_number,
                                     CHAN_ID_DEV_TYPE,
                                     CHAN_ID_TRANS_TYPE);
    APP_ERROR_CHECK(err_code);

    switch ( m_command )
    {
        case COMMAND_STATE_ON:
            m_command = COMMAND_LIGHT_ON;
            break;

        case COMMAND_STATE_OFF:
            m_command = COMMAND_LIGHT_OFF;
            break;

        case COMMAND_STATE_ALL_ON:
            device_number = ADDRESS_ALL_NODES;
            m_command     = COMMAND_LIGHT_ON;
            break;

        case COMMAND_STATE_ALL_OFF:
            device_number = ADDRESS_ALL_NODES;
            m_command     = COMMAND_LIGHT_OFF;
            break;

        default:
            break; // No implementation needed
    }

    m_tx_buffer[0] = MOBILE_COMMAND_PAGE;
    m_tx_buffer[1] = 0xFF;
    m_tx_buffer[2] = device_number;
    m_tx_buffer[3] = 0xFF;
    m_tx_buffer[4] = 0xFF;
    m_tx_buffer[5] = 0xFF;
    m_tx_buffer[6] = 0xFF;
    m_tx_buffer[7] = m_command;

    err_code = sd_ant_acknowledge_message_tx(ANT_RESPONSE_CHANNEL_NUMBER,
                                             ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                             m_tx_buffer);
    APP_ERROR_CHECK(err_code);
}


/**@brief Handler for scan timer event
 * @param p_context     Pointer to context
 */
static void scan_timeout_event(void * p_context)
{
    command_send();
    m_retries          = MAX_RETRIES;
    m_wait_for_timeout = false;
}


/**@brief Start sequence for sending command over scanning channel
 *
 * @param[in] command   Command selected with button
 */
static void command_sequence_start(uint8_t command)
{
    uint32_t err_code;

    bsp_board_led_on(BSP_BOARD_LED_0);

    for (int node = 0; node < MAX_DEVICES; node++)
    {
        m_node_list[node].device_number = 0;
        m_node_list[node].rssi          = 0;
    }
    m_command = command;

    err_code = sd_ant_rx_scan_mode_start(ANT_SCAN_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_scan_timer_id, SCAN_TIMER_TICKS, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling button events.
 *
 * @param[in] evt     BSP event
 */
static void bsp_evt_handler(bsp_event_t evt)
{
    // Check if the previous operation has been completed.
    if (m_wait_for_timeout)
    {
        return;
    }

    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            bsp_board_led_off(BSP_BOARD_LED_2);
            command_sequence_start(COMMAND_STATE_ON);
            break;

        case BSP_EVENT_KEY_1:
            bsp_board_led_off(BSP_BOARD_LED_2);
            command_sequence_start(COMMAND_STATE_OFF);
            break;

        case BSP_EVENT_KEY_2:
            bsp_board_led_off(BSP_BOARD_LED_2);
            command_sequence_start(COMMAND_STATE_ALL_ON);
            break;

        case BSP_EVENT_KEY_3:
            bsp_board_led_off(BSP_BOARD_LED_2);
            command_sequence_start(COMMAND_STATE_ALL_OFF);
            break;

        default:
            break; //No implementation needed
    }

    m_wait_for_timeout = true;
}


/**@brief Function for the Timer and BSP initialization.
 */
static void utils_setup(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS,
                        bsp_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_scan_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                scan_timeout_event);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief initialize application
 */
static void continuous_scan_init()
{
    uint32_t err_code;

    // Set library config to report RSSI and Device ID
    err_code = sd_ant_lib_config_set(
        ANT_LIB_CONFIG_MESG_OUT_INC_RSSI | ANT_LIB_CONFIG_MESG_OUT_INC_DEVICE_ID);
    APP_ERROR_CHECK(err_code);

    // Configure channel 0 for scanning mode, but do not open it.
    // The scanning channel will be opened in scan mode for a short amount of time on a button press.
    ant_channel_config_t channel_config =
    {
        .channel_number    = ANT_SCAN_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_SLAVE,
        .ext_assign        = 0x00,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = 0x00,          // Wildcard
        .channel_period    = 0x00,          // Not used, since this is going to be scanning
        .network_number    = ANT_NETWORK_NUM,
    };

    err_code = ant_channel_init(&channel_config);
    APP_ERROR_CHECK(err_code);

    // Assign a second channel for sending messages on the reverse direction
    // There is no need to configure any other parameters, this channel is never opened;
    // its resources are used by ANT to send messages in the reverse direction while in
    // continuous scanning mode.
    err_code = sd_ant_channel_assign(ANT_RESPONSE_CHANNEL_NUMBER,
                                     CHANNEL_TYPE_SLAVE,
                                     ANT_NETWORK_NUM,
                                     0x00);
    APP_ERROR_CHECK(err_code);
}


/** @brief Add a newly discovered node to the m_node_list
 *
 * @param[in] device_number   The device_number of the current to be added
 * @param[in] rssi            The RSSI from the current node to be added
 */
void node_to_list_add(uint16_t device_number, uint8_t rssi)
{
    uint8_t index = MAX_DEVICES;                                // Specifed maximum total number in the array
    uint8_t start = CONVERT_DEVICE_NUM_TO_INDEX(device_number); // Start index of the array

    uint8_t node_device_number;

    for (int i = 0; i < MAX_DEVICES; i++ )
    {
        node_device_number = m_node_list[(start + i) % MAX_DEVICES].device_number;

        if ( node_device_number == device_number ) // If this node already exists, don't add it
            return;

        if (node_device_number == 0) // If we found a place to put it
        {   // Find the best place to put this node (the place closest to the hash function result)
            index = (start + i) % MAX_DEVICES;
            break;
        }
    }

    if ( index == MAX_DEVICES )
        return;

    m_node_list[index].device_number = device_number;
    m_node_list[index].rssi          = rssi;
}

/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    uint32_t err_code;
    uint8_t  message_rssi;
    uint16_t message_device_number;

    ANT_MESSAGE * p_ant_message = (ANT_MESSAGE *) &p_ant_evt->message;

    switch (p_ant_evt->event)
    {
        case EVENT_RX:
            if (p_ant_message->ANT_MESSAGE_aucPayload[0] == DEVICE_STATUS_PAGE)
            {
                bsp_board_led_invert(BSP_BOARD_LED_1);
                if (p_ant_message->ANT_MESSAGE_stExtMesgBF.bANTDeviceID &&
                    p_ant_message->ANT_MESSAGE_stExtMesgBF.bANTRssi)
                {
                    message_device_number =
                        (uint16_t)(p_ant_message->ANT_MESSAGE_aucExtData[0] |
                                   ((uint16_t)p_ant_message->ANT_MESSAGE_aucExtData[1] << 8));
                    message_rssi = p_ant_message->ANT_MESSAGE_aucExtData[5];
                    node_to_list_add(message_device_number, message_rssi);
                }
            }
            break;

        case EVENT_TRANSFER_TX_COMPLETED:
            bsp_board_led_off(BSP_BOARD_LED_0);
            bsp_board_led_off(BSP_BOARD_LED_1);
            err_code = sd_ant_channel_close(ANT_SCAN_CHANNEL_NUMBER);
            APP_ERROR_CHECK(err_code);
            break;

        case EVENT_TRANSFER_TX_FAILED:
            if (m_retries > 0)
            {
                command_send();
                m_retries--;
            }
            else
            {
                bsp_board_led_off(BSP_BOARD_LED_0);
                bsp_board_led_off(BSP_BOARD_LED_1);
                err_code = sd_ant_channel_close(ANT_SCAN_CHANNEL_NUMBER);
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break; // No implementation needed
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);

/**@brief Function for ANT stack initialization.
 */
static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);
}


/**
 *@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/* Main function */
int main(void)
{
    log_init();
    utils_setup();
    softdevice_setup();
    continuous_scan_init();

    NRF_LOG_INFO("ANT Continuous Scanning Controller example started.");

    // Enter main loop
    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}


/**
 *@}
 **/
