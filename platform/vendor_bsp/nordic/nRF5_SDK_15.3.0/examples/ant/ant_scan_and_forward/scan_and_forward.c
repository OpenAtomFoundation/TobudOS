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
 * @brief ANT Scan and Forward implementation
 *
 * @defgroup ant_scan_and_forward_example ANT Scan and Forward Demo
 * @{
 * @ingroup nrf_ant_scan_and_forward
 *
 */

#include <stdbool.h>
#include "scan_and_forward.h"
#include "deviceregistry.h"
#include "ant_parameters.h"
#include "message_cache.h"
#include "commands.h"
#include "boards.h"
#include "app_button.h"
#include "nrf_delay.h"
#include "ant_channel_config.h"
#include "ant_search_config.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"

#define ANT_SF_NETWORK_ID           ((uint8_t) 1)       /**< Network Number for Scan and Forward Application */
#define CHAN_ID_TRANS_TYPE          ((uint8_t) ((ANT_SF_NETWORK_ID << 4) | 0x05))   /**< Transmission type is comprised of ANT_SF_NETWORK_ID and 0x05 */
#define DEFAULT_CMD_PG_INTLV_PCT    30                  /**< Default percentage internal command pages should be transmitted */

#define ANT_BS_ANT_OBSERVER_PRIO    1                   /**< BS ANT observer priority. */
#define ANT_MS_ANT_OBSERVER_PRIO    1                   /**< MS ANT observer priority. */

static uint8_t m_node_address;                                          /**< Unique address of node within the network */
static uint8_t m_counter = 0;                                           /**< Index of next device */
static uint8_t m_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];             /**< Primary data transmit buffer. */
static uint8_t m_cmd_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];         /**< Command data transmit buffer. */
static uint8_t m_tx_page_counter        = 0;                            /**< Transmitted page counter. */
static uint8_t m_cache_tick_tx_counter  = 0;                            /**< Transmitted page counter for cache tick management. */
static uint8_t m_cmd_pg_intlv_pct       = DEFAULT_CMD_PG_INTLV_PCT;     /**< Percentage of the time internal command pages should be transmitted */
static bool    m_flag_hi_pri_cmd        = false;                        /**< Flag to indicate this is a new command. */

static bool             enable_optimized_command_page_priority = true;  /**< Flag to enable/disable command page pattern optimizations */
static sf_message_t     buffer[MAX_CACHE_SIZE];                         /**< Received message cache buffer. */
static message_cache_t  m_rcvd_messages =                               /**< Received message cache. */
{
    0,
    0,
    0,
    MAX_CACHE_SIZE,
    buffer
};


/**@brief DEVICE_STATUS_PAGE message is queued to send.
 *
 */
static void sf_master_beacon_message_send(void);


/**@brief Checks if a message has already been received
 *
 * @param[in]   p_buffer            Pointer to buffer with message contents
 */
static bool msg_already_received(uint8_t * p_buffer);


/** @brief Sets up and opens the background scanning channel and the master beacon channel used for the scan and forward demo.
 */
static void sf_ant_channels_setup(void)
{
    uint32_t err_code;

    const ant_channel_config_t ms_channel_config =
    {
        .channel_number    = ANT_MS_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_MASTER,
        .ext_assign        = 0x00,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = m_node_address,
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUM,
    };

    const ant_channel_config_t bs_channel_config =
    {
        .channel_number    = ANT_BS_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_SLAVE,
        .ext_assign        = EXT_PARAM_ALWAYS_SEARCH,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = 0x00,              // Wild card
        .channel_period    = 0x00,              // This is not taken into account.
        .network_number    = ANT_NETWORK_NUM,
    };

    const ant_search_config_t bs_search_config =
    {
        .channel_number        = ANT_BS_CHANNEL_NUMBER,
        .low_priority_timeout  = ANT_LOW_PRIORITY_TIMEOUT_DISABLE,
        .high_priority_timeout = ANT_HIGH_PRIORITY_SEARCH_DISABLE,
        .search_sharing_cycles = ANT_SEARCH_SHARING_CYCLES_DISABLE,
        .search_priority       = ANT_SEARCH_PRIORITY_DEFAULT,
        .waveform              = ANT_WAVEFORM_FAST,
    };

    err_code = ant_channel_init(&ms_channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = ant_channel_init(&bs_channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = ant_search_init(&bs_search_config);
    APP_ERROR_CHECK(err_code);

    // Fill tx buffer for the first frame
    sf_master_beacon_message_send();

    // Open master beacon channel
    err_code = sd_ant_channel_open(ANT_MS_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);

    // Open background scanning channel
    err_code = sd_ant_channel_open(ANT_BS_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);
}


void sf_init(void)
{
    dr_init();

    // Get the node address from the switches instead of serial number
#if defined(NODE_ID_FROM_SWITCHES)
    m_node_address = 1 + ((nrf_gpio_pin_read(BSP_SWITCH_1) << 0) |
                     (nrf_gpio_pin_read(BSP_SWITCH_2) << 1) |
                     (nrf_gpio_pin_read(BSP_SWITCH_3) << 2) |
                     (nrf_gpio_pin_read(BSP_SWITCH_4) << 3));
#else
    // Get the node address from the serial number of the device
    m_node_address = (uint8_t)(NRF_FICR->DEVICEID[0]);

    if (m_node_address == ADDRESS_ALL_NODES) // Node address 0 is not allowed
    {
        m_node_address = 1;
    }
    else if (m_node_address == RESERVED) // Node address 0xFF is not allowed
    {
        m_node_address = 0xFE;
    }
#endif

    UNUSED_PARAMETER(dr_device_add(m_node_address));

    for (int i = 0; i < ANT_STANDARD_DATA_PAYLOAD_SIZE; i++)
    {
        m_cmd_tx_buffer[i] = RESERVED;
    }

    sf_ant_channels_setup();
}


void sf_bsp_evt_handler(bsp_event_t evt)
{
    device_t * p_device;

    switch (evt)
    {
        case BSP_EVENT_KEY_0: // Turn self on
            // Update status device registry
            p_device                    = dr_device_get(m_node_address);
            p_device->application_state = STATUS_LIGHT_ON;
            p_device->last_message_sequence_received++;
            bsp_board_leds_on();
            break;

        case BSP_EVENT_KEY_1: // Turn self off
            // Update status device registry
            p_device                    = dr_device_get(m_node_address);
            p_device->application_state = STATUS_LIGHT_OFF;
            p_device->last_message_sequence_received++;
            bsp_board_leds_off();
            break;

        case BSP_EVENT_KEY_2: // Turn all on
            // Update status device registry
            p_device                    = dr_device_get(m_node_address);
            p_device->application_state = STATUS_LIGHT_ON;
            p_device->last_message_sequence_received++;
            // Update command buffer
            set_cmd_buffer_new(ADDRESS_ALL_NODES, COMMAND_LIGHT_ON, RESERVED, RESERVED);
            bsp_board_leds_on();
            break;

        case BSP_EVENT_KEY_3: // Turn all off
            // Update status device registry
            p_device                    = dr_device_get(m_node_address);
            p_device->application_state = STATUS_LIGHT_OFF;
            p_device->last_message_sequence_received++;
            // Update command buffer
            set_cmd_buffer_new(ADDRESS_ALL_NODES, COMMAND_LIGHT_OFF, RESERVED, RESERVED);
            bsp_board_leds_off();
            break;

        default:
            return; // no implementation needed
    }
}


/**@brief Processes ANT message on ANT background scanning channel
 *
 * @param[in] p_ant_evt   ANT message content.
 * @param[in] p_context     Context.
 */
void sf_background_scanner_process(ant_evt_t * p_ant_evt, void * p_context)
{
    if (p_ant_evt->channel != ANT_BS_CHANNEL_NUMBER)
    {
        return;
    }

    device_t * p_device;
    switch (p_ant_evt->event)
    {
        case EVENT_RX:

            // Device Status Page
            if (p_ant_evt->message.ANT_MESSAGE_aucPayload[DATA_PAGE_IND] == DEVICE_STATUS_PAGE)
            {
                uint8_t node            = p_ant_evt->message.ANT_MESSAGE_aucPayload[DEVICE_STATUS_NODE_IND]; // Origin
                uint8_t sequence_number = p_ant_evt->message.ANT_MESSAGE_aucPayload[DEVICE_STATUS_SEQ_NUM_IND];
                uint8_t device_state    = p_ant_evt->message.ANT_MESSAGE_aucPayload[DEVICE_STATUS_STATE_IND];

                // Has this device been seen before?
                if (dr_device_exists(node))
                {
                    // Is this a new message?
                    if (!msg_already_received(p_ant_evt->message.ANT_MESSAGE_aucPayload))
                    {
                        // Update status device registry
                        p_device = dr_device_get(node);
                        p_device->last_message_sequence_received = sequence_number;
                        p_device->application_state              = device_state;
                        m_tx_page_counter                        = dr_index_of_node_get(node) - 1; // Small optimization to send the new device info first
                    }
                }
                // First message received from this device.
                else
                {
                    // Attempt to add the new device
                    if (dr_device_add(node))
                    {
                        // Update status device registry
                        p_device                                 = dr_device_get(node);
                        p_device->application_state              = device_state;
                        p_device->last_message_sequence_received = sequence_number;
                    }
                }
            }
            // Internal Network Command Page
            else if (p_ant_evt->message.ANT_MESSAGE_aucPayload[DATA_PAGE_IND] == INTERNAL_COMMAND_PAGE)
            {
                // Is this a new message?
                if (!msg_already_received(p_ant_evt->message.ANT_MESSAGE_aucPayload))
                {
                    uint8_t node                    = p_ant_evt->message.ANT_MESSAGE_aucPayload[INTERNAL_CMD_DST_IND]; // Destination
                    uint8_t sequence_number         = p_ant_evt->message.ANT_MESSAGE_aucPayload[INTERNAL_CMD_SEQ_NUM_IND];
                    uint8_t cmd_page_interleave_pct = p_ant_evt->message.ANT_MESSAGE_aucPayload[INTERNAL_CMD_CMD_DATA1_IND];
                    uint8_t high_priority_cmd_enable= p_ant_evt->message.ANT_MESSAGE_aucPayload[INTERNAL_CMD_CMD_DATA2_IND];
                    uint8_t node_command            = p_ant_evt->message.ANT_MESSAGE_aucPayload[INTERNAL_CMD_CMD_IND];

                    // Update command buffer
                    set_cmd_buffer_seq(node, node_command, cmd_page_interleave_pct, high_priority_cmd_enable, sequence_number);

                    if (node == m_node_address || node == ADDRESS_ALL_NODES)
                    {
                        // Update status device registry
                        p_device = dr_device_get(m_node_address);
                        p_device->last_message_sequence_received++;

                        switch (node_command)
                        {
                            case COMMAND_LIGHT_OFF:
                                p_device->application_state = STATUS_LIGHT_OFF;
                                bsp_board_leds_off();
                                break;

                            case COMMAND_LIGHT_ON:
                                p_device->application_state = STATUS_LIGHT_ON;
                                bsp_board_leds_on();
                                break;
                            case COMMAND_CHG_CMD_PG_SET:
                                if (cmd_page_interleave_pct != RESERVED)
                                    m_cmd_pg_intlv_pct = cmd_page_interleave_pct;
                                if (high_priority_cmd_enable != RESERVED)
                                    enable_optimized_command_page_priority = high_priority_cmd_enable;
                                break;
                        }
                    }
                }
            }
            break;

        default:
            break;

    }
}

NRF_SDH_ANT_OBSERVER(m_ant_bs_observer, ANT_BS_ANT_OBSERVER_PRIO, sf_background_scanner_process, NULL);


static void sf_master_beacon_message_send(void)
{
    uint32_t      err_code;
    device_t    * p_device      = NULL;

    // Number of messages being transmitted per second
    uint8_t msg_tx_freq = ANT_CLOCK_FREQUENCY / CHAN_PERIOD;

    // Number of command pages required to be transmitted per second
    uint16_t req_cmd_page_count = msg_tx_freq * m_cmd_pg_intlv_pct / 100;

    // If we are sending a new command, prioritize it. Or if we are unable to send command pages.
    if (m_flag_hi_pri_cmd)
    {
    // Transmit new command for 1 second less 1 channel period
        req_cmd_page_count = msg_tx_freq - 1;
    }

    // Transmit status pages after send all of the required command pages for the channel period
    if (m_tx_page_counter % msg_tx_freq >= req_cmd_page_count)
    {
        // Unflag high priority command rotation once we have started sending status pages
        m_flag_hi_pri_cmd = false;

        // Cycle through available device numbers until we get to a registered device.
        do
        {
            if (m_counter < (MAX_DEVICES - 1))
            {
                m_counter++;
            }
            else
            {
                m_counter = 0;
            }
        }
        while (!dr_device_at_index_exists(m_counter));

        p_device = dr_device_at_index_get(m_counter);

        // Update the primary tx buffer
        m_tx_buffer[DATA_PAGE_IND] = DEVICE_STATUS_PAGE;
        m_tx_buffer[DEVICE_STATUS_NODE_IND] = p_device->node_id;
        m_tx_buffer[2] = RESERVED;
        m_tx_buffer[3] = RESERVED;
        m_tx_buffer[4] = RESERVED;
        m_tx_buffer[5] = RESERVED;
        m_tx_buffer[DEVICE_STATUS_SEQ_NUM_IND] = p_device->last_message_sequence_received;
        m_tx_buffer[DEVICE_STATUS_STATE_IND] = p_device->application_state;

        // Add the message we are transmitting to the cache
        mc_add(&m_rcvd_messages, m_tx_buffer);

        err_code = sd_ant_broadcast_message_tx(ANT_MS_CHANNEL_NUMBER,
                                               ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                               m_tx_buffer);
        APP_ERROR_CHECK(err_code);
    }
    // Send command page when we are not sending status pages
    else
    {
        // Add the message we are transmitting to the cache
        mc_add(&m_rcvd_messages, m_cmd_tx_buffer);

        err_code = sd_ant_broadcast_message_tx(ANT_MS_CHANNEL_NUMBER,
                                               ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                               m_cmd_tx_buffer);
        APP_ERROR_CHECK(err_code);
    }

    // Increment transmitted page counters
    m_tx_page_counter++;
    m_cache_tick_tx_counter++;

    // Piggy back off of the channel period to provide 1 second ticks for cache cleanup
    if (m_cache_tick_tx_counter % msg_tx_freq == 0)
    {
        mc_cleanup(&m_rcvd_messages, CACHE_TIMEOUT_S);
    }
}

/**@brief Processes ANT message on ANT master beacon channel
 *
 * @details   This function handles all events on the master beacon channel.
 *            On EVENT_TX an DEVICE_STATUS_PAGE message is queued. The format is:
 *            byte[0]   = page (0x20 = DEVICE_STATUS_PAGE)
 *            byte[1]   = Node Address (Source)
 *            byte[2]   = Reserved
 *            byte[3]   = Reserved
 *            byte[4]   = Reserved
 *            byte[5]   = Reserved
 *            byte[6]   = Sequence Number
 *            byte[7]   = Application State
 *
 *            This channel may also reseive commands sent from a mobile device such as a phone, controller, or ANTwareII in the format:
 *            byte[0]   = page (0x10 = MOBILE_COMMAND_PAGE)
 *            byte[1]   = Originating node (Set to 0xFF for mobile device)
 *            byte[2]   = Destination node (0 for all nodes)
 *            byte[3]   = Reserved (0x0F)
 *            byte[4]   = Reserved (0xFF)
 *            byte[5]   = Reserved (0xFF)
 *            byte[6]   = Reserved (0xFF)
 *            byte[7]   = Command (Turn Off: 0x00, Turn On: 0x01)
 *
 * @param[in] p_ant_evt ANT message content.
 * @param[in] p_context  Context.
 */
void sf_master_beacon_process(ant_evt_t * p_ant_evt, void * p_context)
{
    if (p_ant_evt->channel != ANT_MS_CHANNEL_NUMBER)
    {
        return;
    }

    switch (p_ant_evt->event)
    {
        case EVENT_TX:
            sf_master_beacon_message_send();
            break;

        case EVENT_RX:
            sf_external_received_message_process(p_ant_evt->message.ANT_MESSAGE_aucPayload[DATA_PAGE_IND],
                                                 p_ant_evt->message.ANT_MESSAGE_aucPayload[MOBILE_CMD_DST_IND],
                                                 p_ant_evt->message.ANT_MESSAGE_aucPayload[MOBILE_CMD_CMD_IND],
                                                 p_ant_evt->message.ANT_MESSAGE_aucPayload[MOBILE_CMD_CMD_DATA1_IND],
                                                 p_ant_evt->message.ANT_MESSAGE_aucPayload[MOBILE_CMD_CMD_DATA2_IND]);
            break;

        default:
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_ms_observer, ANT_MS_ANT_OBSERVER_PRIO, sf_master_beacon_process, NULL);


static bool msg_already_received(uint8_t * p_buffer)
{
    if (mc_is_in_cache(&m_rcvd_messages, p_buffer))
    {
        return true;
    }

    // New message, store for future comparisons
    mc_add(&m_rcvd_messages, p_buffer);

    return false;
}


void set_cmd_buffer_seq(uint8_t dst, uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t seq)
{
    m_cmd_tx_buffer[DATA_PAGE_IND] = INTERNAL_COMMAND_PAGE;
    m_cmd_tx_buffer[INTERNAL_CMD_DST_IND] = dst;
    m_cmd_tx_buffer[2] = RESERVED;
    m_cmd_tx_buffer[3] = RESERVED;
    m_cmd_tx_buffer[INTERNAL_CMD_CMD_DATA2_IND] = data2;
    m_cmd_tx_buffer[INTERNAL_CMD_CMD_DATA1_IND] = data1;
    m_cmd_tx_buffer[INTERNAL_CMD_SEQ_NUM_IND] = seq;
    m_cmd_tx_buffer[INTERNAL_CMD_CMD_IND] = cmd;

    if (enable_optimized_command_page_priority || cmd == COMMAND_CHG_CMD_PG_SET)
    {
        m_flag_hi_pri_cmd = true;   // Flag that command page should take priority over other pages

        // Small optimization to send the new command first.
        // The master channel is configured to send cmd pages first at the start of every second
        m_tx_page_counter = 0;
    }
}


void set_cmd_buffer_new(uint8_t dst, uint8_t cmd, uint8_t payload0, uint8_t payload1)
{
    set_cmd_buffer_seq(dst, cmd, payload0, payload1, m_cmd_tx_buffer[6] + 1);
}


void sf_external_received_message_process(uint8_t page, uint8_t dst, uint8_t data0, uint8_t data1, uint8_t data2)
{
    device_t    * p_device      = NULL;
    uint8_t command = data0;

    // Mobile Command Page
    switch (page)
    {
        case MOBILE_COMMAND_PAGE:

            if (dr_device_exists(dst))
            {
                p_device = dr_device_get(dst);
            }

            switch (command)
            {
                case COMMAND_LIGHT_ON:

                    set_cmd_buffer_new(dst, COMMAND_LIGHT_ON,
                                   RESERVED, RESERVED);

                    if ((p_device != NULL && p_device->node_id == m_node_address)
                                            || dst == ADDRESS_ALL_NODES)
                    {
                        p_device                    = dr_device_get(m_node_address);
                        p_device->application_state = STATUS_LIGHT_ON;
                        p_device->last_message_sequence_received++;
                        bsp_board_leds_on();
                    }
                    break;

                case COMMAND_LIGHT_OFF:

                    set_cmd_buffer_new(dst, COMMAND_LIGHT_OFF,
                                   RESERVED, RESERVED);

                    if ((p_device != NULL && p_device->node_id == m_node_address)
                                            || dst == ADDRESS_ALL_NODES)
                    {
                        p_device                    = dr_device_get(m_node_address);
                        p_device->application_state = STATUS_LIGHT_OFF;
                        p_device->last_message_sequence_received++;
                        bsp_board_leds_off();
                    }
                    break;

                case COMMAND_CHG_CMD_PG_SET :

                    set_cmd_buffer_new(dst, COMMAND_CHG_CMD_PG_SET,
                                   data1, data2);

                    if (dst == m_node_address || dst == ADDRESS_ALL_NODES)
                    {
                        if (data1 != RESERVED)
                            m_cmd_pg_intlv_pct = data1;
                        if (data2 != RESERVED)
                            enable_optimized_command_page_priority = data2;
                    }
                    break;
            }
            break;
    }
}

