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
 * @defgroup iot_sdk_dns6_app main.c
 * @{
 * @ingroup iot_sdk_app_ipv6
 * @brief This file contains the source code for Nordic's IPv6 DNS6 sample application.
 *
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
#include "nrf_delay.h"
#include "iot_common.h"
#include "iot_timer.h"
#include "ipv6_api.h"
#include "icmp6_api.h"
#include "dns6_api.h"
#include "ipv6_medium.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define SCHED_MAX_EVENT_DATA_SIZE           16                                                      /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE                    192                                                     /**< Maximum number of events in the scheduler queue. */

#define LED_ONE                         BSP_LED_0_MASK
#define LED_TWO                         BSP_LED_1_MASK
#define LED_THREE                       BSP_LED_2_MASK
#define LED_FOUR                        BSP_LED_3_MASK

#define START_BUTTON_PIN_NO             BSP_BUTTON_0                                                /**< Button used to start application state machine. */
#define STOP_BUTTON_PIN_NO              BSP_BUTTON_1                                                /**< Button used to stop application state machine. */
#ifdef COMMISSIONING_ENABLED
#define ERASE_BUTTON_PIN_NO             BSP_BUTTON_3                                                /**< Button used to erase commissioning settings. */
#endif // COMMISSIONING_ENABLED

#define LED_BLINK_INTERVAL_MS           500                                                         /**< LED blinking interval. */
#define APP_STATE_INTERVAL              APP_TIMER_TICKS(1500)                                       /**< Interval for executing application state machine. */

#define APP_RTR_SOLICITATION_DELAY      1000                                                        /**< Time before host sends an initial solicitation in ms. */

#define DEAD_BEEF                       0xDEADBEEF                                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define MAX_LENGTH_FILENAME             128                                                         /**< Max length of filename to copy for the debug error handler. */

#define APP_HOSTNAME                    "example.com"                                               /**< Host name to perform DNS resolution and Echo Ping. */
#define APP_DNS_LOCAL_PORT              0x8888                                                      /**< UDP Port number of local DNS Resolver. */
#define APP_DNS_SERVER_PORT             0x0035                                                      /**< UDP Port number of DNS Server. */
#define APP_DNS_SERVER_ADDR             {{0x20, 0x01, 0x48, 0x60, 0x48, 0x60, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88}}          /**< IPv6 address of DNS Server. */

#define APP_MAX_ECHO_REQUEST_RTR        3                                                           /**< Maximum echo request retransmission before application goes back to querying state. */

#define APP_ENABLE_LOGS                 1                                                           /**< Enable logs in the application. */

#if (APP_ENABLE_LOGS == 1)

#define APPL_LOG  NRF_LOG_INFO
#define APPL_DUMP NRF_LOG_RAW_HEXDUMP_INFO
#define APPL_ADDR IPV6_ADDRESS_LOG

#else // APP_ENABLE_LOGS

#define APPL_LOG(...)
#define APPL_DUMP(...)
#define APPL_ADDR(...)

#endif // APP_ENABLE_LOGS

/**@brief Application's state. */
typedef enum
{
    APP_STATE_IDLE = 0,
    APP_STATE_QUERYING,
    APP_STATE_RESOLVING,
    APP_STATE_PINGING
} app_state_t;

/**@brief LED's indication state. */
typedef enum
{
    LEDS_INACTIVE = 0,
    LEDS_CONNECTABLE_MODE,
    LEDS_IPV6_IF_DOWN,
    LEDS_IPV6_IF_UP
} display_state_t;

APP_TIMER_DEF(m_app_timer);                                                                         /**< Timer instance used for application state machine. */
APP_TIMER_DEF(m_iot_timer_tick_src_id);                                                             /**< App timer instance used to update the IoT timer wall clock. */
static ipv6_medium_instance_t m_ipv6_medium;
eui64_t                       eui64_local_iid;                                                      /**< Local EUI64 value that is used as the IID for*/
static iot_interface_t      * mp_interface = NULL;                                                  /**< Pointer to IoT interface if any. */
static ipv6_addr_t            m_hostname_address;                                                   /**< IPv6 address of given hostname. */
static uint32_t               m_echo_req_retry_count;                                               /**< Number of echo request retransmission. */
static display_state_t        m_display_state = LEDS_INACTIVE;                                      /**< Board LED display state. */
static volatile app_state_t   m_app_state = APP_STATE_IDLE;                                         /**< State of application state machine. */

/**@brief Addresses used in sample application. */
static const ipv6_addr_t      m_local_routers_multicast_addr = {{0xFF, 0x02, 0x00, 0x00,
                                                                 0x00, 0x00, 0x00, 0x00,
                                                                 0x00, 0x00, 0x00, 0x00,
                                                                 0x00, 0x00, 0x00, 0x02}};          /**< Multi-cast address of all routers on the local network segment. */

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


/**@brief Timer callback used for controlling board LEDs to represent application state.
 *
 * @param[in]   p_context   Pointer used for passing context. No context used in this application.
 */
static void blink_timeout_handler(iot_timer_time_in_ms_t wall_clock_value)
{
    UNUSED_PARAMETER(wall_clock_value);

#ifdef COMMISSIONING_ENABLED
    static bool id_mode_previously_enabled;

    if (m_identity_mode_active == false)
    {
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
            LEDS_OFF((LED_THREE | LED_FOUR));
            LEDS_ON(LED_ONE);
            LEDS_INVERT(LED_TWO);
            break;
        }

        case LEDS_IPV6_IF_UP:
        {
            LEDS_OFF(LED_ONE);
            LEDS_ON(LED_TWO);
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


/**@brief Function for handling button events.
 *
 * @param[in]   pin_no         The pin number of the button pressed.
 * @param[in]   button_action  The action performed on button.
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    uint32_t err_code;
#ifdef COMMISSIONING_ENABLED
    if ((button_action == APP_BUTTON_PUSH) && (pin_no == ERASE_BUTTON_PIN_NO))
    {
        APPL_LOG("Erasing all commissioning settings from persistent storage...");
        commissioning_settings_clear();
        return;
    }
#endif // COMMISSIONING_ENABLED

    // Check if interface is UP.
    if (mp_interface == NULL)
    {
        return;
    }

    if (button_action == APP_BUTTON_PUSH)
    {
        switch (pin_no)
        {
            case START_BUTTON_PIN_NO:
            {
                APPL_LOG("Start button has been pushed.");

                // Change application state in case being in IDLE state.
                if (m_app_state == APP_STATE_IDLE)
                {
                    m_app_state = APP_STATE_QUERYING;

                    // Start application state machine timer.
                    err_code = app_timer_start(m_app_timer, APP_STATE_INTERVAL, NULL);
                    APP_ERROR_CHECK(err_code);
                }

                break;
            }
            case STOP_BUTTON_PIN_NO:
            {
                APPL_LOG("Stop button has been pushed.");
                LEDS_OFF((LED_THREE | LED_FOUR));

                // Back to IDLE state.
                m_app_state = APP_STATE_IDLE;

                // Stop application state machine timer.
                err_code = app_timer_stop(m_app_timer);
                APP_ERROR_CHECK(err_code);
                break;
            }

            default:
              break;
        }
    }
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
        {START_BUTTON_PIN_NO, false, BUTTON_PULL, button_event_handler},
        {STOP_BUTTON_PIN_NO,  false, BUTTON_PULL, button_event_handler},
#ifdef COMMISSIONING_ENABLED
        {ERASE_BUTTON_PIN_NO, false, BUTTON_PULL, button_event_handler}
#endif // COMMISSIONING_ENABLED
    };

    #define BUTTON_DETECTION_DELAY APP_TIMER_TICKS(50)

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}


/**@brief DNS6 module event handler.
 *
 * @details Callback registered with the DNS6 module to receive asynchronous events from
 *          the module for registering query.
 */
static void app_dns_handler(uint32_t      process_result,
                            const char  * p_hostname,
                            ipv6_addr_t * p_addr,
                            uint16_t      addr_count)
{
    uint32_t index;

    if (m_app_state != APP_STATE_RESOLVING)
    {
        // Exit if it's not in resolving state.
        return;
    }

    APPL_LOG("DNS Response for hostname: %s, with %d IPv6 addresses and status 0x%08lX",
    p_hostname, addr_count, process_result);

    if (process_result == NRF_SUCCESS)
    {
        for (index = 0; index < addr_count; index++)
        {
            APPL_LOG("[%ld] IPv6 Address: ", index);
            APPL_ADDR(p_addr[index]);

            // Store only first given address, but print all of them.
            if (index == 0)
            {
                memcpy(m_hostname_address.u8, p_addr[0].u8, IPV6_ADDR_SIZE);

                // Change application state.
                m_app_state = APP_STATE_PINGING;

                // Turn on LED_THREE
                LEDS_ON(LED_THREE);
            }
        }
    }
    else
    {
        // Start application state machine from beginning.
        m_app_state = APP_STATE_QUERYING;
    }
}


/**@brief Application state machine used for sending DNS Query and Echo Request.
 *
 * @param[in]   p_context   Pointer used for passing context. No context used in this application.
 */
static void app_fsm(void * p_context)
{
    uint32_t                   err_code;
    iot_pbuffer_t            * p_buffer;
    iot_pbuffer_alloc_param_t  pbuff_param;
    ipv6_addr_t                src_addr;

    switch (m_app_state)
    {
      case APP_STATE_QUERYING:
          APPL_LOG("Application in querying state.");
          // Set number of retransmission to 0.
          m_echo_req_retry_count = 0;

          // Turn off LED_THREE and LED_FOUR
          LEDS_OFF((LED_THREE | LED_FOUR));

          // Find all IPv6 address corresponds to APP_HOSTNAME domain.
          err_code = dns6_query(APP_HOSTNAME, app_dns_handler);
          APP_ERROR_CHECK(err_code);

          // Change state to resolving.
          m_app_state = APP_STATE_RESOLVING;
        break;
      case APP_STATE_RESOLVING:
          APPL_LOG("Application in resolving state.");
          // Wait for DNS response.
          break;
      case APP_STATE_PINGING:
          APPL_LOG("Application in pinging state.");

          if (m_echo_req_retry_count < APP_MAX_ECHO_REQUEST_RTR)
          {
              pbuff_param.flags  = PBUFFER_FLAG_DEFAULT;
              pbuff_param.type   = ICMP6_PACKET_TYPE;
              pbuff_param.length = ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET + 10;

              // Allocate packet buffer.
              err_code = iot_pbuffer_allocate(&pbuff_param, &p_buffer);
              APP_ERROR_CHECK(err_code);

              // Fill payload of Echo Request with 'A' letters.
              memset(p_buffer->p_payload + ICMP6_ECHO_REQUEST_PAYLOAD_OFFSET, 'A', 10);

              // Find proper source address for given destination one.
              err_code = ipv6_address_find_best_match(&mp_interface, &src_addr, &m_hostname_address);
              APP_ERROR_CHECK(err_code);

              // Send Echo Request to all nodes.
              err_code = icmp6_echo_request(mp_interface, &src_addr, &m_hostname_address, p_buffer);
              APP_ERROR_CHECK(err_code);

              // Increase retransmission number.
              m_echo_req_retry_count++;
          }
          else
          {
              APPL_LOG("Failed, no Echo Response received.");
              // Go to initial state.
              m_app_state = APP_STATE_QUERYING;
          }
          break;
      default:
        break;
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
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    APP_ERROR_CHECK(app_timer_init());

    // Create timer to control application state machine.
    err_code = app_timer_create(&m_app_timer, APP_TIMER_MODE_REPEATED,
                                app_fsm);
    APP_ERROR_CHECK(err_code);

    // Initialize timer instance as a tick source for IoT timer.
    err_code = app_timer_create(&m_iot_timer_tick_src_id,
                                APP_TIMER_MODE_REPEATED,
                                iot_timer_tick_callback);
    APP_ERROR_CHECK(err_code);
}


static void ip_app_handler(iot_interface_t * p_interface, ipv6_event_t * p_event)
{
    uint32_t    err_code;
    ipv6_addr_t src_addr;

    APPL_LOG("Got IP Application Handler Event on interface 0x%p", p_interface);

    switch (p_event->event_id)
    {
        case IPV6_EVT_INTERFACE_ADD:
#ifdef COMMISSIONING_ENABLED
            commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_STOP_RESET);
#endif // COMMISSIONING_ENABLED
            APPL_LOG("New interface added!");
            mp_interface = p_interface;

            m_display_state = LEDS_IPV6_IF_UP;

            APPL_LOG("Sending Router Solicitation to all routers!");

            // Create Link Local addresses
            IPV6_CREATE_LINK_LOCAL_FROM_EUI64(&src_addr, p_interface->local_addr.identifier);

            // Delay first solicitation due to possible restriction on other side.
            nrf_delay_ms(APP_RTR_SOLICITATION_DELAY);

            // Send Router Solicitation to all routers.
            err_code = icmp6_rs_send(p_interface,
                                     &src_addr,
                                     &m_local_routers_multicast_addr);
            APP_ERROR_CHECK(err_code);
        break;

        case IPV6_EVT_INTERFACE_DELETE:
#ifdef COMMISSIONING_ENABLED
            commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_START);
#endif // COMMISSIONING_ENABLED
            APPL_LOG("Interface removed!");
            mp_interface = NULL;

            m_display_state = LEDS_IPV6_IF_DOWN;

            // Stop application state machine timer.
            m_app_state = APP_STATE_IDLE;
            err_code = app_timer_stop(m_app_timer);
            APP_ERROR_CHECK(err_code);
            break;

        case IPV6_EVT_INTERFACE_RX_DATA:
            APPL_LOG("Got unsupported protocol data!");
            break;

        default:
            // Unknown event. Should not happen.
            break;
    }
}


/**@brief ICMP6 module event handler.
 *
 * @details Callback registered with the ICMP6 module to receive asynchronous events from
 *          the module, if the ICMP6_ENABLE_ALL_MESSAGES_TO_APPLICATION constant is not zero
 *          or the ICMP6_ENABLE_ND6_MESSAGES_TO_APPLICATION constant is not zero.
 */
uint32_t icmp6_handler(iot_interface_t * p_interface,
                       ipv6_header_t   * p_ip_header,
                       icmp6_header_t  * p_icmp_header,
                       uint32_t          process_result,
                       iot_pbuffer_t   * p_rx_packet)
{
    APPL_LOG("Got ICMP6 Application Handler Event on interface 0x%p", p_interface);

    APPL_LOG("Source IPv6 Address: ");
    APPL_ADDR(p_ip_header->srcaddr);
    APPL_LOG("Destination IPv6 Address: ");
    APPL_ADDR(p_ip_header->destaddr);
    APPL_LOG("Process result = 0x%08lx", process_result);

    switch (p_icmp_header->type)
    {
        case ICMP6_TYPE_ECHO_REPLY:
            APPL_LOG("ICMP6 Message Type = Echo Reply");

            if (m_app_state != APP_STATE_IDLE)
            {
                // Invert LED_FOUR.
                LEDS_INVERT(LED_FOUR);
            }

            // Reset echo request retransmission number.
            m_echo_req_retry_count = 0;
            break;
        default:
            break;
    }

    return NRF_SUCCESS;
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


/**@brief Function for initializing the IoT Timer. */
static void ip_stack_timer_init(void)
{
    uint32_t err_code;

    static const iot_timer_client_t list_of_clients[] =
    {
        {blink_timeout_handler,   LED_BLINK_INTERVAL_MS},
        {dns6_timeout_process,    SEC_TO_MILLISEC(DNS6_RETRANSMISSION_INTERVAL)},
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


/**@brief Function for initializing IP stack.
 *
 * @details Initialize the IP Stack.
 */
static void dns_client_init(void)
{
    uint32_t    err_code;

    // Configure DNS client and server parameters.
    dns6_init_t dns_init_param =
    {
        .local_src_port  = APP_DNS_LOCAL_PORT,
        .dns_server      =
        {
            .port = APP_DNS_SERVER_PORT,
            .addr = APP_DNS_SERVER_ADDR
        }
    };

    err_code = dns6_init(&dns_init_param);
    APP_ERROR_CHECK(err_code);
}


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


/**@brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    // Initialize
    log_init();
    scheduler_init();
    leds_init();
    timers_init();
    buttons_init();

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

    ip_stack_init();
    ip_stack_timer_init();
    dns_client_init();

    APPL_LOG("Application started.");

    // Start execution
    connectable_mode_enter();

    // Enter main loop
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
