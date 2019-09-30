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
 * @defgroup iot_sdk_icmp_app main.c
 * @{
 * @ingroup iot_sdk_app_ipv6
 * @brief This file contains the source code for Nordic's IPv6 ICMPv6 sample application.
 *
 * This example demonstrates how Nordic's IPv6 stack can be used for sending and receiving ICMPv6 packets.
 * Sending one of four packets is triggered by user buttons on kit. LEDs blinking indicates that response has been received.
 *
 * All available packets are:
 * - Neighbor Solicitation                              | Button 1
 * - Router Solicitation                                | Button 2
 * - Echo Request to all node multi-cast address ff02::1 | Button 3
 * - Echo Request to remote node (Router)               | Button 4
 *
 * Note: In order to get response for Router Solicitation, peer node has to act as a Router.
 *
 * Below is MSC explaining the data flow at ICMPv6 level.
 *
 *    +------------------+                                                +------------------+
 *    | Node             |                                                | Router           |
 *    |(this application)|                                                |                  |
 *    +------------------+                                                +------------------+
 *           |                                                                   |
 *           |                                                                   |
 *        ---|------------------------- Interface is UP  ------------------------|---
 *           |                                                                   |
 *        ---|-------------------- Button 1 has been pushed ---------------------|---
 *           |                                                                   |
 *           |[ICMPv6 Neighbor Solicitation to router's link-local address]      |
 *           |------------------------------------------------------------------>|
 *           |                                                                   |
 *           |       [ICMPv6 Neighbor Advertisement to node's link-local address]|
 *           |<------------------------------------------------------------------|
 *           |                                                                   |
 *     LEDs BLINKING                                                             |
 *           |                                                                   |
 *        ---|-------------------- Button 2 has been pushed ---------------------|---
 *           |                                                                   |
 *           |[ICMPv6 Router Solicitation to all routers multi-cast address]     |
 *           |------------------------------------------------------------------>|
 *           |                                                                   |
 *           |         [ICMPv6 Router Advertisement to node's link-local address]|
 *           |<------------------------------------------------------------------|
 *           |                                                                   |
 *     LEDs BLINKING                                                             |
 *           |                                                                   |
 *        ---|-------------------- Button 3 has been pushed ---------------------|---
 *           |                                                                   |
 *           |[ICMPv6 Echo Request to all nodes multi-cast address]              |
 *           |------------------------------------------------------------------>|
 *           |                                                                   |
 *           |                [ICMPv6 Echo Response to node's link-local address]|
 *           |<------------------------------------------------------------------|
 *           |                                                                   |
 *     LEDs BLINKING                                                             |
 *           |                                                                   |
 *        ---|-------------------- Button 4 has been pushed ---------------------|---
 *           |                                                                   |
 *           |[ICMPv6 Echo Request to router's link-local address]               |
 *           |------------------------------------------------------------------>|
 *           |                                                                   |
 *           |                [ICMPv6 Echo Response to node's link-local address]|
 *           |<------------------------------------------------------------------|
 *           |                                                                   |
 *     LEDs BLINKING                                                             |
 *           |                                                                   |
 *           -                                                                   -
 */
#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nordic_common.h"
#include "sdk_config.h"
#include "nrf_sdm.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "app_button.h"
#include "ipv6_api.h"
#include "icmp6_api.h"
#include "udp_api.h"
#include "iot_timer.h"
#include "ipv6_medium.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define CONNECTABLE_MODE_LED          BSP_LED_0_MASK                                                /**< LED to indicate that device is in connectable mode. */
#define CONNECTED_LED                 BSP_LED_1_MASK                                                /**< LED to indicate that device is connected. */
#define RESPONSE_LED_1                BSP_LED_2_MASK                                                /**< LED to indicate ICMP response is received. */
#define RESPONSE_LED_2                BSP_LED_3_MASK                                                /**< LED to indicate ICMP response is received. */

#define NS_BUTTON_PIN_NO              BSP_BUTTON_0                                                  /**< Button used to send Neighbor Solicitation message. */
#define RS_BUTTON_PIN_NO              BSP_BUTTON_1                                                  /**< Button used to send Router Solicitation message. */
#define PING_ALL_BUTTON_PIN_NO        BSP_BUTTON_2                                                  /**< Button used to send Echo Request to all device. */
#define PING_PEER_BUTTON_PIN_NO       BSP_BUTTON_3                                                  /**< Button used to send Echo Request to peer device. */

#define SCHED_QUEUE_SIZE              16                                                            /**< Maximum number of events in the scheduler queue. */
#define SCHED_MAX_EVENT_DATA_SIZE     192                                                           /**< Maximum size of scheduler events. */


#define DEAD_BEEF                     0xDEADBEEF                                                    /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define MAX_LENGTH_FILENAME           128                                                           /**< Max length of filename to copy for the debug error handler. */

#define LED_BLINK_INTERVAL_MS         400                                                           /**< Timeout between LEDs toggling. */
#define APP_MAX_LED_TOGGLE            5                                                             /**< Number of LEDs toggling after successful operation. */

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

typedef enum
{
    LEDS_INACTIVE = 0,
    LEDS_CONNECTABLE_MODE,
    LEDS_IPV6_IF_DOWN,
    LEDS_IPV6_IF_UP,
    LEDS_BLINK_MODE
} display_state_t;

eui64_t                       eui64_local_iid;                                                      /**< Local EUI64 value that is used as the IID for SLAAC. */
static iot_interface_t      * mp_interface = NULL;                                                  /**< Pointer to IoT interface if any. */
static ipv6_medium_instance_t m_ipv6_medium;
static display_state_t        m_disp_state = LEDS_INACTIVE;                                         /**< Board LED display state. */
static uint8_t                m_blink_count = 0;                                                    /**< Number of LED flashes after successful operation. */
static bool                   m_led_feedback_enabled = false;                                       /**< If true, incoming ICMP packets are notified to the user. */

APP_TIMER_DEF(m_iot_timer_tick_src_id);                                                             /**< App timer instance used to update the IoT timer wall clock. */

/**@brief IPv6 well-known addresses. */
static const ipv6_addr_t      all_node_multicast_addr   = {{0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}};
static const ipv6_addr_t      all_router_multicast_addr = {{0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}};

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
    LEDS_CONFIGURE(LEDS_MASK);

    // Set LEDs off.
    LEDS_OFF(LEDS_MASK);
}


/**@brief Timer callback used for controlling board LEDs to represent application state.
 */
static void blink_timeout_handler(iot_timer_time_in_ms_t wall_clock_value)
{
    UNUSED_PARAMETER(wall_clock_value);

    static display_state_t previous_display_state;
#ifdef COMMISSIONING_ENABLED
    static bool id_mode_previously_enabled;
#endif // COMMISSIONING_ENABLED

#ifdef COMMISSIONING_ENABLED
    if (m_identity_mode_active == false)
    {
#endif // COMMISSIONING_ENABLED
    switch (m_disp_state)
    {
        case LEDS_INACTIVE:
        {
            LEDS_OFF(LEDS_MASK);
            break;
        }
        case LEDS_CONNECTABLE_MODE:
        {
            LEDS_INVERT(CONNECTABLE_MODE_LED);
            LEDS_OFF((CONNECTED_LED | RESPONSE_LED_1 | RESPONSE_LED_2));
            break;
        }
        case LEDS_IPV6_IF_DOWN:
        {
            LEDS_ON(CONNECTABLE_MODE_LED);
            LEDS_OFF((CONNECTED_LED | RESPONSE_LED_1 | RESPONSE_LED_2));
            break;
        }
        case LEDS_IPV6_IF_UP:
        {
            LEDS_ON(CONNECTED_LED);
            LEDS_OFF((CONNECTABLE_MODE_LED | RESPONSE_LED_1 | RESPONSE_LED_2));
            break;
        }
        case LEDS_BLINK_MODE:
        {
            if (previous_display_state != LEDS_BLINK_MODE)
            {
                LEDS_OFF(RESPONSE_LED_1 | RESPONSE_LED_2);
            }
            else
            {
                ++m_blink_count;
                if (m_blink_count < APP_MAX_LED_TOGGLE)
                {
                    LEDS_INVERT(RESPONSE_LED_1 | RESPONSE_LED_2);
                }
                else
                {
                    LEDS_OFF(RESPONSE_LED_1 | RESPONSE_LED_2);
                    m_disp_state = LEDS_IPV6_IF_UP;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
#ifdef COMMISSIONING_ENABLED
    }
#endif // COMMISSIONING_ENABLED
    previous_display_state = m_disp_state;

#ifdef COMMISSIONING_ENABLED
    if ((id_mode_previously_enabled == false) && (m_identity_mode_active == true))
    {
        LEDS_ON(BSP_LED_0_MASK | BSP_LED_2_MASK);
        LEDS_OFF(BSP_LED_1_MASK | BSP_LED_3_MASK);
    }

    if ((id_mode_previously_enabled == true) && (m_identity_mode_active == true))
    {
        LEDS_INVERT(LEDS_MASK);
    }

    if ((id_mode_previously_enabled == true) && (m_identity_mode_active == false))
    {
        LEDS_OFF(LEDS_MASK);
    }

    id_mode_previously_enabled = m_identity_mode_active;
#endif // COMMISSIONING_ENABLED
}


/**@brief Function for indicating receive of ICMP response by blinking the LEDs.
 */
static void led_blinking_start()
{
    m_blink_count = 0;
    m_disp_state  = LEDS_BLINK_MODE;
    m_led_feedback_enabled = false;
}


/**@brief Function for handling button events.
 *
 * @param[in]   pin_no         The pin number of the button pressed.
 * @param[in]   button_action  The action performed on button.
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    uint32_t                   err_code;
    ipv6_addr_t                src_addr;
    ipv6_addr_t                dest_addr;
    icmp6_ns_param_t           ns_param;
    iot_pbuffer_t            * p_buffer;
    iot_pbuffer_alloc_param_t  pbuff_param;

    // Check if interface is UP.
    if (mp_interface == NULL)
    {
#ifdef COMMISSIONING_ENABLED
        if ((button_action == APP_BUTTON_PUSH) && (pin_no == PING_PEER_BUTTON_PIN_NO))
        {
            APPL_LOG("Erasing all commissioning settings from persistent storage...");
            commissioning_settings_clear();
            return;
        }
#endif // COMMISSIONING_ENABLED
        return;
    }

    // Create Link Local address.
    IPV6_CREATE_LINK_LOCAL_FROM_EUI64(&src_addr, mp_interface->local_addr.identifier);

    if (button_action == APP_BUTTON_PUSH)
    {
        m_led_feedback_enabled = true;

        switch (pin_no)
        {
            case NS_BUTTON_PIN_NO:
            {
                APPL_LOG("Sending Neighbour Solicitation to peer!");

                ns_param.add_aro      = true;
                ns_param.aro_lifetime = 1000;

                // Unicast address.
                IPV6_CREATE_LINK_LOCAL_FROM_EUI64(&ns_param.target_addr,
                                                  mp_interface->peer_addr.identifier);

                // Send Neighbor Solicitation to all nodes.
                err_code = icmp6_ns_send(mp_interface,
                                         &src_addr,
                                         &ns_param.target_addr,
                                         &ns_param);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case RS_BUTTON_PIN_NO:
            {
                APPL_LOG("Sending Router Solicitation to all routers FF02::2!");

                // Send Router Solicitation to all routers.
                err_code = icmp6_rs_send(mp_interface,
                                         &src_addr,
                                         &all_router_multicast_addr);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case PING_ALL_BUTTON_PIN_NO:
            {
                APPL_LOG("Ping all remote nodes using FF02::1 address!");

                pbuff_param.flags  = PBUFFER_FLAG_DEFAULT;
                pbuff_param.type   = ICMP6_PACKET_TYPE;
                pbuff_param.length = ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET + 10;

                // Allocate packet buffer.
                err_code = iot_pbuffer_allocate(&pbuff_param, &p_buffer);
                APP_ERROR_CHECK(err_code);

                // Fill payload of Echo Request with 'A' letters.
                memset(p_buffer->p_payload + ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET, 'A', 10);

                // Send Echo Request to all nodes.
                err_code = icmp6_echo_request(mp_interface, &src_addr, &all_node_multicast_addr, p_buffer);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case PING_PEER_BUTTON_PIN_NO:
            {
                APPL_LOG("Ping peer device!");

                pbuff_param.flags  = PBUFFER_FLAG_DEFAULT;
                pbuff_param.type   = ICMP6_PACKET_TYPE;
                pbuff_param.length = ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET + 10;

                // Allocate packet buffer.
                err_code = iot_pbuffer_allocate(&pbuff_param, &p_buffer);
                APP_ERROR_CHECK(err_code);

                // Unicast address.
                IPV6_CREATE_LINK_LOCAL_FROM_EUI64(&dest_addr,
                                                  mp_interface->peer_addr.identifier);

                // Fill payload of Echo Request with 'B' letters.
                memset(p_buffer->p_payload + ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET, 'A', 10);

                // Send Echo Request to all nodes.
                err_code = icmp6_echo_request(mp_interface, &src_addr, &dest_addr, p_buffer);
                APP_ERROR_CHECK(err_code);
                break;
            }

            default:
            {
                m_led_feedback_enabled = false;
                break;
            }
        }
    }
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


/**@brief Function for initializing the IoT Timer. */
static void iot_timer_init(void)
{
    uint32_t err_code;

    static const iot_timer_client_t list_of_clients[] =
    {
        {blink_timeout_handler,   LED_BLINK_INTERVAL_MS},
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


/**@brief Function for the Button initialization.
 *
 * @details Initializes all Buttons used by this application.
 */
static void buttons_init(void)
{
    uint32_t err_code;

    static app_button_cfg_t buttons[] =
    {
        {NS_BUTTON_PIN_NO,          false, BUTTON_PULL, button_event_handler},
        {RS_BUTTON_PIN_NO,          false, BUTTON_PULL, button_event_handler},
        {PING_ALL_BUTTON_PIN_NO,    false, BUTTON_PULL, button_event_handler},
        {PING_PEER_BUTTON_PIN_NO,   false, BUTTON_PULL, button_event_handler}
    };

    #define BUTTON_DETECTION_DELAY APP_TIMER_TICKS(50)

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
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
    APPL_LOG("Got ICMP6 Application Handler Event on interface %p", p_interface);

    APP_ERROR_CHECK(process_result);

    APPL_LOG("\tSrc IPv6 Address: ");
    APPL_ADDR(p_ip_header->srcaddr);
    APPL_LOG("\tDst IPv6 Address: ");
    APPL_ADDR(p_ip_header->destaddr);
    APPL_LOG("\tProcess result = 0x%08lX\r\n", process_result);

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

            if (m_led_feedback_enabled == true)
            {
                // Start blinking LEDs to indicate that Echo Reply has been received.
                led_blinking_start();
            }

            break;
        case ICMP6_TYPE_ROUTER_SOLICITATION:
            APPL_LOG("ICMP6 Message Type = Router Solicitation");
            break;
        case ICMP6_TYPE_ROUTER_ADVERTISEMENT:
            APPL_LOG("ICMP6 Message Type = Router Advertisement");

            if (m_led_feedback_enabled == true)
            {
                // Start blinking LEDs to indicate that Echo Reply has been received.
                led_blinking_start();
            }

            break;
        case ICMP6_TYPE_NEIGHBOR_SOLICITATION:
            APPL_LOG("ICMP6 Message Type = Neighbor Solicitation");
            break;
        case ICMP6_TYPE_NEIGHBOR_ADVERTISEMENT:
            APPL_LOG("ICMP6 Message Type = Neighbor Advertisement");

            if (m_led_feedback_enabled == true)
            {
                // Start blinking LEDS to indicate that Echo Reply has been received.
                led_blinking_start();
            }

            break;
        default:
            break;
    }

    return NRF_SUCCESS;
}


/**@brief Function for starting connectable mode.
 */
static void connectable_mode_enter(void)
{
    uint32_t err_code = ipv6_medium_connectable_mode_enter(m_ipv6_medium.ipv6_medium_instance_id);
    APP_ERROR_CHECK(err_code);

    APPL_LOG("Physical layer in connectable mode.");
    m_disp_state = LEDS_CONNECTABLE_MODE;
}


/**@brief IP Stack interface events handler. */
void ip_app_handler(iot_interface_t * p_interface, ipv6_event_t * p_event)
{
    APPL_LOG("Got IP Application Handler Event on interface %p", p_interface);

    switch (p_event->event_id)
    {
        case IPV6_EVT_INTERFACE_ADD:
#ifdef COMMISSIONING_ENABLED
            commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_STOP_RESET);
#endif // COMMISSIONING_ENABLED
            APPL_LOG("New interface added!");
            mp_interface = p_interface;

            m_disp_state = LEDS_IPV6_IF_UP;

            break;
        case IPV6_EVT_INTERFACE_DELETE:
#ifdef COMMISSIONING_ENABLED
            commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_START);
#endif // COMMISSIONING_ENABLED
            APPL_LOG("Interface removed!");
            mp_interface = NULL;

            m_led_feedback_enabled = false;
            m_disp_state = LEDS_IPV6_IF_DOWN;

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


static void on_ipv6_medium_evt(ipv6_medium_evt_t * p_ipv6_medium_evt)
{
    switch (p_ipv6_medium_evt->ipv6_medium_evt_id)
    {
        case IPV6_MEDIUM_EVT_CONN_UP:
        {
            APPL_LOG("Physical layer connected.");
            m_disp_state = LEDS_IPV6_IF_DOWN;
            break;
        }
        case IPV6_MEDIUM_EVT_CONN_DOWN:
        {
            APPL_LOG("Physical layer disconnected.");
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
void commissioning_id_mode_cb(mode_control_cmd_t control_command)
{
    switch (control_command)
    {
        case CMD_IDENTITY_MODE_ENTER:
        {
            LEDS_OFF(LEDS_MASK);
            m_identity_mode_active = true;

            break;
        }
        case CMD_IDENTITY_MODE_EXIT:
        {
            m_identity_mode_active = false;
            LEDS_OFF(LEDS_MASK);

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

    APPL_LOG("Do power_off on failure: %s.",
             m_power_off_on_failure ? "true" : "false");
}
#endif // COMMISSIONING_ENABLED


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    // Initialize.
    scheduler_init();
    leds_init();
    timers_init();
    log_init();
    buttons_init();
    iot_timer_init();

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
    APPL_LOG("Application started.");

    // Start execution.
    connectable_mode_enter();

    // Enter main loop.
    for (;;)
    {
        /* Execute event schedule */
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
