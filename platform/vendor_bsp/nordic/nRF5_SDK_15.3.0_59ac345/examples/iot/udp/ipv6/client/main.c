/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
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
/** @file
 *
 * @defgroup iot_sdk_app_nrf_udp_client main.c
 * @{
 * @ingroup iot_sdk_app_nrf
 * @brief This file contains the source code for nRF UDP Client sample application.
 *
 * This application is intended to work with the Nordic UDP Server sample application.
 * The Server is identified with its address, SERVER_IPV6_ADDRESS. Check TX_PORT and RX_PORT
 * for port configuration.
 *
 * The application sends Echo Requests to the Server to determine whether it's reachable.
 * Once an Echo Response is received, the application starts to send UDP packets to the peer.
 * The payload of the UDP packets is 20 bytes long and has the following format:
 *  +----------------------------+---------------------------+
 *  |  Packet sequence number    |  16 bytes of random data  |
 *  | in uint32 format (4 bytes) |                           |
 *  +----------------------------+---------------------------+
 *
 * The Server is expected to send back the UDP packets without any modification.
 * Each transmitted packet is stored until it's received back or until the buffer is full.
 *
 * The buffer of transmitted packets is of PACKET_BUFFER_LEN long. If it gets full,
 * the transmission of UDP packets stop and the buffer is emptied. The packet sequence
 * number is set to zero. Echo Requests are sent to the Server to determine whether it
 * is reachable. When an Echo Response is received, transmission of UDP packets starts again.
 *
 * The operation of the application is reflected by two LEDs on the board:
 *
 * +-----------+-----------+
 * |   LED 1   |   LED 2   |
 * +-----------+-----------+---------------------------------------------------+
 * | Blinking  |    Off    |       Device advertising as BLE peripheral.       |
 * +-----------+-----------+---------------------------------------------------+
 * |    On     |    Off    |     BLE link established, IPv6 interface down.    |
 * +-----------+-----------+---------------------------------------------------+
 * |    On     | Blinking  |         IPv6 interface up, Echo Requests          |
 * |           |           |              are sent to the Server.              |
 * +-----------+-----------+---------------------------------------------------+
 * | Blinking  |    On     |        UDP packets are sent to the Server.        |
 * +-----------+-----------+---------------------------------------------------+
 * |    On     |    On     |       Assertion failure in the application.       |
 * +-----------+-----------+---------------------------------------------------+
 *
 * The MSC below illustrates the data flow. The UDP transmissions are triggered by a timer,
 * reception is asynchronous.
 *
 *       +------------------+                                              +------------------+
 *       | UDP Client       |                                              | UDP Server       |
 *       |(this application)|                                              |                  |
 *       +------------------+                                              +------------------+
 *                |                                                                 |
 *                |                                                                 |
 *                |                                                         Listening on RX_PORT
 *                |                                                                 |
 *                |                                                                 |
 *                |[ICMPv6 Echo Request]                                            |
 *                |---------------------------------------------------------------->|
 *                |                                                                 |
 *                |                                          [ICMPv6 Echo Response] |
 *                |<----------------------------------------------------------------|
 *                |                                                                 |
 *                |[UDP Payload : [Sequence Number 1 | Data ]]                      |
 *                |---------------------------------------------------------------->|
 *                |                                                                 |
 *  Transmitted UDP payload stored                                                  |
 *                |                                                                 |
 *                |                                                                 |
 *                |                     [UDP Payload : [Sequence Number 1 | Data ]] |
 *                |<----------------------------------------------------------------|
 *                |                                                                 |
 * Received UDP payload is matched                                                  |
 *    against stored payload(s)                                                     |
 *                |                                                                 |
 *  Sequence number is incremented                                                  |
 *                |                                                                 |
 *                |[UDP Payload : [Sequence Number 2 | Data ]]                      |
 *                |---------------------------------------------------------------->|
 *                |                                                                 |
 *  Transmitted UDP payload stored                                                  |
 *                |                                                                 |
 *                |                                                                 |
 *                |                     [UDP Payload : [Sequence Number 2 | Data ]] |
 *                |<----------------------------------------------------------------|
 *                |                                                                 |
 * Received UDP payload is matched                                                  |
 *    against stored payload(s)                                                     |
 *                |                                                                 |
 *  Sequence number is incremented                                                  |
 *                |                 .                                               |
 *                |                 .                                               |
 *                |                 .                                               |
 *                |                                                                 |
 *                -                                                                 -
 */

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nordic_common.h"
#include "sdk_config.h"
#include "nrf_sdm.h"
#include "app_scheduler.h"
#include "app_button.h"
#include "app_timer.h"
#include "app_util.h"
#include "ipv6_api.h"
#include "icmp6_api.h"
#include "udp_api.h"
#include "iot_timer.h"
#include "ipv6_medium.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define SCHED_MAX_EVENT_DATA_SIZE     16                                                            /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE              192                                                           /**< Maximum number of events in the scheduler queue. */

#define LED_ONE                       BSP_LED_0_MASK
#define LED_TWO                       BSP_LED_1_MASK
#define LED_THREE                     BSP_LED_2_MASK
#define LED_FOUR                      BSP_LED_3_MASK

#ifdef COMMISSIONING_ENABLED
#define ERASE_BUTTON_PIN_NO           BSP_BUTTON_3                                                  /**< Button used to erase commissioning settings. */
#endif // COMMISSIONING_ENABLED

/** Modify SERVER_IPV6_ADDRESS according to your setup.
 *  The address provided below is a place holder.  */
#define SERVER_IPV6_ADDRESS             0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01              /**< IPv6 address of the server node. */

#define UDP_PORT                      9000                                                          /**< Port for transmission of UDP packets. */

#define TX_INTERVAL_MS                400                                                           /**< Interval between sending packets to the peer. */
#define LED_BLINK_INTERVAL_MS         300                                                           /**< LED blinking interval. */

#define PACKET_BUFFER_LEN             5                                                             /**< Number of stored transmitted UDP6 packets. */
#define TEST_PACKET_NUM_LEN           4                                                             /**< Length of UDP6 packet sequence number, in bytes. */
#define TEST_PACKET_DATA_LEN          16                                                            /**< Length of test data in UDP6 packet payload, in bytes. */
#define TEST_PACKET_PAYLOAD_LEN       TEST_PACKET_NUM_LEN + TEST_PACKET_DATA_LEN                    /**< Full length of UDP6 packet payload. */

#define INVALID_PACKET_SEQ_NUMBER     0                                                             /**< Zero not used as a packet sequence number. */

#define DEAD_BEEF                     0xDEADBEEF                                                    /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define MAX_LENGTH_FILENAME           128                                                           /**< Max length of filename to copy for the debug error handler. */

#define APP_ENABLE_LOGS               1                                                             /**< Enable logs in the application. */

#if (APP_ENABLE_LOGS == 1)

#define APPL_LOG  NRF_LOG_INFO
#define APPL_DUMP NRF_LOG_RAW_HEXDUMP_INFO
#define APPL_ADDR IPV6_ADDRESS_LOG

#else // APP_ENABLE_LOGS

#define APPL_LOG(...)
#define APPL_DUMP(...)
#define APPL_ADDR(...)

#endif // APP_ENABLE_LOGS

typedef struct
{
    uint8_t packet_seq_num[TEST_PACKET_NUM_LEN];
    uint8_t packet_data[TEST_PACKET_DATA_LEN];
} test_packet_payload_t;

typedef enum
{
    APP_STATE_IPV6_IF_DOWN = 1,
    APP_STATE_IPV6_IF_UP,
    APP_STATE_PEER_REACHABLE
} tx_app_state_t;

typedef enum
{
    LEDS_INACTIVE = 0,
    LEDS_CONNECTABLE_MODE,
    LEDS_IPV6_IF_DOWN,
    LEDS_TX_ECHO_REQUEST,
    LEDS_TX_UDP_PACKET
} display_state_t;

APP_TIMER_DEF(m_iot_timer_tick_src_id);                                                             /**< App timer instance used to update the IoT timer wall clock. */
static ipv6_medium_instance_t m_ipv6_medium;
eui64_t                       eui64_local_iid;                                                      /**< Local EUI64 value that is used as the IID for*/
static udp6_socket_t          m_udp_socket;                                                         /**< UDP socket used for reception and transmission. */
static uint32_t               m_pkt_seq_num = 0;                                                    /**< Transmitted UDP sequence number in payload. */
static const uint32_t         m_invalid_pkt_seq_num = INVALID_PACKET_SEQ_NUMBER;                    /**< Variable storing constant invalid packet sequence number. */
static tx_app_state_t         m_node_state = APP_STATE_IPV6_IF_DOWN;                                /**< Application inner state. */
static display_state_t        m_display_state = LEDS_INACTIVE;                                      /**< Board LED display state. */
static uint8_t                m_packet_buffer[PACKET_BUFFER_LEN][TEST_PACKET_PAYLOAD_LEN];          /**< Buffer for storing transmitted packets. */

#ifdef COMMISSIONING_ENABLED
static bool                    m_power_off_on_failure = false;
static bool                    m_identity_mode_active;
#endif // COMMISSIONING_ENABLED


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    // Configure LEDs.
    LEDS_CONFIGURE((LED_ONE | LED_TWO | LED_THREE | LED_FOUR));

    // Turn LEDs off.
    LEDS_OFF((LED_ONE | LED_TWO | LED_THREE | LED_FOUR));
}


/**@brief Function for getting the index number of a stored packet or the index number of an empty
 *        slot in the buffer.
 *
 * @details Returns the index number of a stored packet with the provided packet sequence number.
 *          Use with INVALID_PACKET_SEQ_NUMBER as input to get the index number of the first
 *          empty slot in the buffer.
 *
 * @param[out] p_index_number         Index number of the first packet with matching sequence
 *                                    number.
 * @param[in]  pkt_seq_num            Packet sequence number.
 *
 * @retval     NRF_SUCCESS            Matching slot found in the buffer.
 * @retval     NRF_ERROR_NOT_FOUND    No match found in the buffer.
 */
static uint32_t get_packet_buffer_index(uint32_t * p_index_number, uint32_t * pkt_seq_num)
{
    uint32_t i;
    for (i = 0; i < PACKET_BUFFER_LEN; i++)
    {
        uint32_t rx_sequence_num = uint32_decode(&m_packet_buffer[i][0]);
        if (rx_sequence_num == *pkt_seq_num)
        {
            *p_index_number = i;
            return NRF_SUCCESS;
        }
    }
    *p_index_number = 0;
    return NRF_ERROR_NOT_FOUND;
}


/**@brief Timer callback used for transmitting Echo Request and UDP6 packets depending on
 *        application state.
 *
 * @param[in]   p_context   Pointer used for passing context. No context used in this application.
 */
static void tx_timeout_handler(iot_timer_time_in_ms_t wall_clock_value)
{
    UNUSED_PARAMETER(wall_clock_value);

    uint32_t err_code;

    switch (m_node_state)
    {
        case APP_STATE_IPV6_IF_DOWN:
        {
            return;
        }
        case APP_STATE_IPV6_IF_UP:
        {
            APPL_LOG("Ping remote node.");

            iot_pbuffer_alloc_param_t   pbuff_param;
            iot_pbuffer_t             * p_buffer;

            pbuff_param.flags  = PBUFFER_FLAG_DEFAULT;
            pbuff_param.type   = ICMP6_PACKET_TYPE;
            pbuff_param.length = ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET + 10;

            // Allocate packet buffer.
            err_code = iot_pbuffer_allocate(&pbuff_param, &p_buffer);
            APP_ERROR_CHECK(err_code);

            ipv6_addr_t dest_ipv6_addr;
            memcpy(&dest_ipv6_addr.u8[0], (uint8_t[]){SERVER_IPV6_ADDRESS}, IPV6_ADDR_SIZE);

            iot_interface_t * p_interface;
            ipv6_addr_t       src_ipv6_addr;
            err_code = ipv6_address_find_best_match(&p_interface,
                                                    &src_ipv6_addr,
                                                    &dest_ipv6_addr);
            APP_ERROR_CHECK(err_code);

            memset(p_buffer->p_payload + ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET, 'A', 10);

            // Send Echo Request to peer.
            err_code = icmp6_echo_request(p_interface, &src_ipv6_addr, &dest_ipv6_addr, p_buffer);
            APP_ERROR_CHECK(err_code);

            break;
        }
        case APP_STATE_PEER_REACHABLE:
        {
            uint32_t ind_buff = 0;
            err_code = get_packet_buffer_index(&ind_buff, (uint32_t *)&m_invalid_pkt_seq_num);
            if (err_code == NRF_ERROR_NOT_FOUND)
            {
                // Buffer of expected packets full, checking if peer is reachable.
                APPL_LOG("%ld packets transmitted, %d packets lost. Resetting counter.",
                             m_pkt_seq_num, PACKET_BUFFER_LEN);
                m_node_state    = APP_STATE_IPV6_IF_UP;
                m_display_state = LEDS_TX_ECHO_REQUEST;
                m_pkt_seq_num = 0;
                memset(&m_packet_buffer[0][0], 0x00, sizeof(m_packet_buffer));
                return;
            }

            ++m_pkt_seq_num;
            if (m_pkt_seq_num == INVALID_PACKET_SEQ_NUMBER)
            {
                ++m_pkt_seq_num;
            }

            test_packet_payload_t packet;
            uint8_t encoded_seq_num[TEST_PACKET_NUM_LEN] = {0, };;
            UNUSED_VARIABLE(uint32_encode(m_pkt_seq_num, &encoded_seq_num[0]));
            // The first 4 bytes of the payload is the packet sequence number.
            memcpy(&packet.packet_seq_num[0], &encoded_seq_num[0], TEST_PACKET_NUM_LEN);
            // The rest of the payload is random bytes.
            do
            {
                err_code = sd_rand_application_vector_get(&packet.packet_data[0],
                                                          sizeof(packet.packet_data));
            }
            while (err_code == NRF_ERROR_SOC_RAND_NOT_ENOUGH_VALUES);
            APP_ERROR_CHECK(err_code);

            iot_pbuffer_alloc_param_t   pbuff_param;
            iot_pbuffer_t             * p_buffer;

            pbuff_param.flags  = PBUFFER_FLAG_DEFAULT;
            pbuff_param.type   = UDP6_PACKET_TYPE;
            pbuff_param.length = TEST_PACKET_PAYLOAD_LEN;

            // Allocate packet buffer.
            err_code = iot_pbuffer_allocate(&pbuff_param, &p_buffer);
            APP_ERROR_CHECK(err_code);

            memcpy(p_buffer->p_payload, &packet.packet_seq_num[0], TEST_PACKET_NUM_LEN);
            memcpy(p_buffer->p_payload+TEST_PACKET_NUM_LEN, &packet.packet_data[0], TEST_PACKET_DATA_LEN);

            ipv6_addr_t dest_ipv6_addr;
            memset(&dest_ipv6_addr, 0x00, sizeof(ipv6_addr_t));
            memcpy(&dest_ipv6_addr.u8[0], (uint8_t[]){SERVER_IPV6_ADDRESS}, IPV6_ADDR_SIZE);

            // Transmit UDP6 packet.
            err_code = udp6_socket_sendto(&m_udp_socket, &dest_ipv6_addr, UDP_PORT, p_buffer);
            APP_ERROR_CHECK(err_code);

            APPL_LOG("Transmitted UDP packet sequence number: %ld", m_pkt_seq_num);

            // Store sent packet amongst expected packets.
            memcpy(&m_packet_buffer[ind_buff][0], &packet.packet_seq_num[0], TEST_PACKET_NUM_LEN);
            memcpy(&m_packet_buffer[ind_buff][TEST_PACKET_NUM_LEN], &packet.packet_data[0], TEST_PACKET_DATA_LEN);

            break;
        }
        default:
        {
            break;
        }
    }
}


/**@brief Timer callback used for controlling board LEDs to represent application state.
 *
 * @param[in]   p_context   Pointer used for passing context. No context used in this application.
 */
static void blink_timeout_handler(iot_timer_time_in_ms_t wall_clock_value)
{
    UNUSED_PARAMETER(wall_clock_value);

#ifdef COMMISSIONING_ENABLED
    static bool id_mode_previously_enabled;
#endif // COMMISSIONING_ENABLED

    switch (m_display_state)
    {
        case LEDS_INACTIVE:
        {
            LEDS_OFF((LED_ONE | LED_TWO));
            break;
        }
        case LEDS_CONNECTABLE_MODE:
        {
            LEDS_INVERT(LED_ONE);
            LEDS_OFF(LED_TWO);
            break;
        }
        case LEDS_IPV6_IF_DOWN:
        {
            LEDS_ON(LED_ONE);
            LEDS_OFF(LED_TWO);
            break;
        }
        case LEDS_TX_ECHO_REQUEST:
        {
            LEDS_ON(LED_ONE);
            LEDS_INVERT(LED_TWO);
            break;
        }
        case LEDS_TX_UDP_PACKET:
        {
            LEDS_INVERT(LED_ONE);
            LEDS_ON(LED_TWO);
            break;
        }
        default:
        {
            break;
        }
    }

#ifdef COMMISSIONING_ENABLED
    if ((id_mode_previously_enabled == false) && (m_identity_mode_active == true))
    {
        LEDS_OFF(LED_THREE | LED_FOUR);
    }

    if ((id_mode_previously_enabled == true) && (m_identity_mode_active == true))
    {
        LEDS_INVERT(LED_THREE | LED_FOUR);
    }

    if ((id_mode_previously_enabled == true) && (m_identity_mode_active == false))
    {
        LEDS_OFF(LED_THREE | LED_FOUR);
    }

    id_mode_previously_enabled = m_identity_mode_active;
#endif // COMMISSIONING_ENABLED
}


/**@brief Function for initializing the IoT Timer. */
static void iot_timer_init(void)
{
    uint32_t err_code;

    static const iot_timer_client_t list_of_clients[] =
    {
        {blink_timeout_handler,   LED_BLINK_INTERVAL_MS},
        {tx_timeout_handler,      TX_INTERVAL_MS},
#ifdef COMMISSIONING_ENABLED
        {commissioning_time_tick, SEC_TO_MILLISEC(COMMISSIONING_TICK_INTERVAL_SEC)}
#endif // COMMISSIONING_ENABLED
    };

    // The list of IoT Timer clients is declared as a constant.
    static const iot_timer_clients_list_t iot_timer_clients =
    {
        (sizeof(list_of_clients) / sizeof(iot_timer_client_t)),
        &(list_of_clients[0]),
    };

    // Passing the list of clients to the IoT Timer module.
    err_code = iot_timer_client_list_set(&iot_timer_clients);
    APP_ERROR_CHECK(err_code);

    // Starting the app timer instance that is the tick source for the IoT Timer.
    err_code = app_timer_start(m_iot_timer_tick_src_id,
                               APP_TIMER_TICKS(IOT_TIMER_RESOLUTION_IN_MS),
                               NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for updating the wall clock of the IoT Timer module.
 */
static void iot_timer_tick_callback(void * p_context)
{
    UNUSED_VARIABLE(p_context);
    uint32_t err_code = iot_timer_update();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    uint32_t err_code;
    // Initialize timer module.
    APP_ERROR_CHECK(app_timer_init());

    // Initialize timer instance as a tick source for IoT timer.
    err_code = app_timer_create(&m_iot_timer_tick_src_id,
                                APP_TIMER_MODE_REPEATED,
                                iot_timer_tick_callback);
    APP_ERROR_CHECK(err_code);
}


/**@brief Callback handler to receive data on UDP port.
 *
 * @param[in]   p_socket         Socket identifier.
 * @param[in]   p_ip_header      IPv6 header containing source and destination addresses.
 * @param[in]   p_udp_header     UDP header identifying local and remote endpoints.
 * @param[in]   process_result   Result of data reception, there could be possible errors like
 *                               invalid checksum etc.
 * @param[in]   iot_pbuffer_t    Packet buffer containing the received data packet.
 *
 * @retval      NRF_SUCCESS      Indicates received data was handled successfully, else an error
 *                               code indicating reason for failure.
 */
uint32_t rx_udp_port_app_handler(const udp6_socket_t * p_socket,
                                 const ipv6_header_t * p_ip_header,
                                 const udp6_header_t * p_udp_header,
                                 uint32_t              process_result,
                                 iot_pbuffer_t       * p_rx_packet)
{
    // APPL_LOG("Got UDP6 data on socket 0x%08lx", p_socket->socket_id);

    APP_ERROR_CHECK(process_result);

    // Print PORTs
    // APPL_LOG("UDP Destination port: %lx", HTONS(p_udp_header->destport));
    // APPL_LOG("UDP Source port: %lx",      HTONS(p_udp_header->srcport));

    uint32_t rx_sequence_num = uint32_decode(&p_rx_packet->p_payload[0]);
    uint32_t ind_buff = 0;
    uint32_t err_code = get_packet_buffer_index(&ind_buff, &rx_sequence_num);
    if (err_code == NRF_ERROR_NOT_FOUND)
    {
        // Received packet sequence number is not found amongst expected packets.
        return NRF_SUCCESS;
    }

    if (0 == memcmp(p_rx_packet->p_payload, &m_packet_buffer[ind_buff][0], TEST_PACKET_PAYLOAD_LEN))
    {
        // If received packet is as expected, free slot in buffer.
        memset(&m_packet_buffer[ind_buff][0], 0x00, TEST_PACKET_PAYLOAD_LEN);
    }

    return NRF_SUCCESS;
}


/**@brief ICMP6 module event handler.
 *
 * @details Callback registered with the ICMP6 module to receive asynchronous events from
 *          the module, if the ICMP6_ENABLE_ALL_MESSAGES_TO_APPLICATION constant is not zero
 *          or the ICMP6_ENABLE_ND6_MESSAGES_TO_APPLICATION constant is not zero.
 */
uint32_t icmp6_handler(iot_interface_t  * p_interface,
                       ipv6_header_t    * p_ip_header,
                       icmp6_header_t   * p_icmp_header,
                       uint32_t           process_result,
                       iot_pbuffer_t    * p_rx_packet)
{
    APPL_LOG("Got ICMP6 Application Handler Event on interface 0x%p", p_interface);

    APPL_LOG("Source IPv6 Address: ");
    APPL_ADDR(p_ip_header->srcaddr);
    APPL_LOG("Destination IPv6 Address: ");
    APPL_ADDR(p_ip_header->destaddr);
    APPL_LOG("Process result = 0x%08lx", process_result);

    switch (p_icmp_header->type)
    {
        case ICMP6_TYPE_DESTINATION_UNREACHABLE:
            APPL_LOG("ICMP6 Message Type = Destination Unreachable Error");
            break;
        case ICMP6_TYPE_PACKET_TOO_LONG:
            APPL_LOG("ICMP6 Message Type = Packet Too Long Error");
            break;
        case ICMP6_TYPE_TIME_EXCEED:
            APPL_LOG("ICMP6 Message Type = Time Exceed Error");
            break;
        case ICMP6_TYPE_PARAMETER_PROBLEM:
            APPL_LOG("ICMP6 Message Type = Parameter Problem Error");
            break;
        case ICMP6_TYPE_ECHO_REQUEST:
            APPL_LOG("ICMP6 Message Type = Echo Request");
            break;
        case ICMP6_TYPE_ECHO_REPLY:
            APPL_LOG("ICMP6 Message Type = Echo Reply");

            ipv6_addr_t server_ipv6_addr;
            memset(&server_ipv6_addr, 0, sizeof(ipv6_addr_t));
            memcpy(&server_ipv6_addr.u8[0], (uint8_t []){SERVER_IPV6_ADDRESS}, IPV6_ADDR_SIZE);

            if (0 == memcmp(&p_ip_header->srcaddr.u8[0], &server_ipv6_addr.u8[0], IPV6_ADDR_SIZE))
            {
                // Echo Response received from peer, start sending UDP6 packets.
                m_node_state    = APP_STATE_PEER_REACHABLE;
                m_display_state = LEDS_TX_UDP_PACKET;
            }

            break;
        case ICMP6_TYPE_ROUTER_SOLICITATION:
            APPL_LOG("ICMP6 Message Type = Router Solicitation");
            break;
        case ICMP6_TYPE_ROUTER_ADVERTISEMENT:
            APPL_LOG("ICMP6 Message Type = Router Advertisement");
            break;
        case ICMP6_TYPE_NEIGHBOR_SOLICITATION:
            APPL_LOG("ICMP6 Message Type = Neighbor Solicitation");
            break;
        case ICMP6_TYPE_NEIGHBOR_ADVERTISEMENT:
            APPL_LOG("ICMP6 Message Type = Neighbor Advertisement");
            break;
        default:
            break;
    }

    return NRF_SUCCESS;
}


/**@brief IP stack event handler.
 *
 * @details Callback registered with the ICMP6 to receive asynchronous events from the module.
 */
void ip_app_handler(iot_interface_t * p_interface, ipv6_event_t * p_event)
{
    uint32_t err_code;

    APPL_LOG("Got IP Application Handler Event on interface 0x%p", p_interface);

    switch (p_event->event_id)
    {
        case IPV6_EVT_INTERFACE_ADD:
#ifdef COMMISSIONING_ENABLED
            commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_STOP_RESET);
#endif // COMMISSIONING_ENABLED
            APPL_LOG("New interface added!");

            err_code = udp6_socket_allocate(&m_udp_socket);
            APP_ERROR_CHECK(err_code);

            err_code = udp6_socket_bind(&m_udp_socket, IPV6_ADDR_ANY, HTONS(UDP_PORT));
            APP_ERROR_CHECK(err_code);

            err_code = udp6_socket_recv(&m_udp_socket, rx_udp_port_app_handler);
            APP_ERROR_CHECK(err_code);

            memset(&m_packet_buffer[0][0], 0x00, sizeof(m_packet_buffer));
            m_display_state = LEDS_TX_ECHO_REQUEST;
            m_node_state    = APP_STATE_IPV6_IF_UP;

            break;
        case IPV6_EVT_INTERFACE_DELETE:
            m_node_state    = APP_STATE_IPV6_IF_DOWN;
            m_display_state = LEDS_IPV6_IF_DOWN;

#ifdef COMMISSIONING_ENABLED
            commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_START);
#endif // COMMISSIONING_ENABLED

            APPL_LOG("Interface removed!");

            err_code = udp6_socket_free(&m_udp_socket);
            APP_ERROR_CHECK(err_code);

            memset(&m_packet_buffer[0][0], 0x00, sizeof(m_packet_buffer));

            break;
        case IPV6_EVT_INTERFACE_RX_DATA:
            APPL_LOG("Got unsupported protocol data!");
            break;

        default:
            //Unknown event. Should not happen.
            break;
    }
}


/**@brief Function for initializing IP stack.
 *
 * @details Initialize the IP Stack.
 */
static void ip_stack_init(void)
{
    uint32_t    err_code;
    ipv6_init_t init_param;

    err_code = ipv6_medium_eui64_get(m_ipv6_medium.ipv6_medium_instance_id,
                                     &eui64_local_iid);
    APP_ERROR_CHECK(err_code);

    init_param.p_eui64       = &eui64_local_iid;
    init_param.event_handler = ip_app_handler;

    err_code = ipv6_init(&init_param);
    APP_ERROR_CHECK(err_code);

    err_code = icmp6_receive_register(icmp6_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting connectable mode.
 */
static void connectable_mode_enter(void)
{
    uint32_t err_code = ipv6_medium_connectable_mode_enter(m_ipv6_medium.ipv6_medium_instance_id);
    APP_ERROR_CHECK(err_code);

    APPL_LOG("Physical layer in connectable mode.");
    m_display_state = LEDS_CONNECTABLE_MODE;
}


static void on_ipv6_medium_evt(ipv6_medium_evt_t * p_ipv6_medium_evt)
{
    switch (p_ipv6_medium_evt->ipv6_medium_evt_id)
    {
        case IPV6_MEDIUM_EVT_CONN_UP:
        {
            APPL_LOG("Physical layer: connected.");
            m_display_state = LEDS_IPV6_IF_DOWN;
            break;
        }
        case IPV6_MEDIUM_EVT_CONN_DOWN:
        {
            APPL_LOG("Physical layer: disconnected.");
            connectable_mode_enter();
            break;
        }
        default:
        {
            break;
        }
    }
}


static void on_ipv6_medium_error(ipv6_medium_error_t * p_ipv6_medium_error)
{
    // Do something.
}


#ifdef COMMISSIONING_ENABLED
/**@brief Function for handling button events.
 *
 * @param[in]   pin_no         The pin number of the button pressed.
 * @param[in]   button_action  The action performed on button.
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
#ifdef COMMISSIONING_ENABLED
    if ((button_action == APP_BUTTON_PUSH) && (pin_no == ERASE_BUTTON_PIN_NO))
    {
        APPL_LOG("Erasing all commissioning settings from persistent storage...");
        commissioning_settings_clear();
        return;
    }
#endif // COMMISSIONING_ENABLED
}


/**@brief Function for the Button initialization.
 *
 * @details Initializes all Buttons used by this application.
 */
static void buttons_init(void)
{
    uint32_t err_code;

    static app_button_cfg_t buttons[] =
    {
#ifdef COMMISSIONING_ENABLED
        {ERASE_BUTTON_PIN_NO,        false, BUTTON_PULL, button_event_handler}
#endif // COMMISSIONING_ENABLED
    };

    #define BUTTON_DETECTION_DELAY APP_TIMER_TICKS(50)

    err_code = app_button_init(buttons,
                               ARRAY_SIZE(buttons),
                               BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}
#endif // COMMISSIONING_ENABLED


#ifdef COMMISSIONING_ENABLED
void commissioning_id_mode_cb(mode_control_cmd_t control_command)
{
    switch (control_command)
    {
        case CMD_IDENTITY_MODE_ENTER:
        {
            LEDS_OFF(LED_THREE | LED_FOUR);
            m_identity_mode_active = true;

            break;
        }
        case CMD_IDENTITY_MODE_EXIT:
        {
            m_identity_mode_active = false;
            LEDS_OFF((LED_THREE | LED_FOUR));

            break;
        }
        default:
        {

            break;
        }
    }
}


void commissioning_power_off_cb(bool power_off_on_failure)
{
    m_power_off_on_failure = power_off_on_failure;

    APPL_LOG("Commissioning: do power_off on failure: %s.",
             m_power_off_on_failure ? "true" : "false");
}
#endif // COMMISSIONING_ENABLED


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    // Initialize.
    log_init();
    scheduler_init();
    leds_init();
    timers_init();

#ifdef COMMISSIONING_ENABLED
    buttons_init();
#endif // COMMISSIONING_ENABLED

    static ipv6_medium_init_params_t ipv6_medium_init_params;
    memset(&ipv6_medium_init_params, 0x00, sizeof(ipv6_medium_init_params));
    ipv6_medium_init_params.ipv6_medium_evt_handler    = on_ipv6_medium_evt;
    ipv6_medium_init_params.ipv6_medium_error_handler  = on_ipv6_medium_error;
#ifdef COMMISSIONING_ENABLED
    ipv6_medium_init_params.commissioning_id_mode_cb   = commissioning_id_mode_cb;
    ipv6_medium_init_params.commissioning_power_off_cb = commissioning_power_off_cb;
#endif // COMMISSIONING_ENABLED

    err_code = ipv6_medium_init(&ipv6_medium_init_params,
                                IPV6_MEDIUM_ID_BLE,
                                &m_ipv6_medium);
    APP_ERROR_CHECK(err_code);

    eui48_t ipv6_medium_eui48;
    err_code = ipv6_medium_eui48_get(m_ipv6_medium.ipv6_medium_instance_id,
                                     &ipv6_medium_eui48);

    ipv6_medium_eui48.identifier[EUI_48_SIZE - 1] = 0x00;

    err_code = ipv6_medium_eui48_set(m_ipv6_medium.ipv6_medium_instance_id,
                                     &ipv6_medium_eui48);
    APP_ERROR_CHECK(err_code);

    // Initialize IP Stack.
    ip_stack_init();

    // Initialize IoT timer module.
    iot_timer_init();

    APPL_LOG("Application started.");

    // Start execution.
    connectable_mode_enter();

    // Enter main loop.
    for (;;)
    {
        app_sched_execute();

        if (NRF_LOG_PROCESS() == false)
        {
            // Sleep waiting for an application event.
            err_code = sd_app_evt_wait();
            APP_ERROR_CHECK(err_code);
        }
    }
}

/**
 * @}
 */
