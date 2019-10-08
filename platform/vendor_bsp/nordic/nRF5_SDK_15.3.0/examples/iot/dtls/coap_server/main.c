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
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
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
#include "nrf_delay.h"
#include "iot_common.h"
#include "ipv6_api.h"
#include "icmp6_api.h"
#include "udp_api.h"
#include "iot_timer.h"
#include "coap_api.h"
#include "ipv6_medium.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define LOCAL_PORT_NUM                  5684                                                  /**< CoAP default port number. */

#define SCHED_MAX_EVENT_DATA_SIZE       16                                                    /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE                192                                                   /**< Maximum number of events in the scheduler queue. */

#define LED_ONE                         BSP_LED_0_MASK
#define LED_TWO                         BSP_LED_1_MASK
#define LED_THREE                       BSP_LED_2_MASK
#define LED_FOUR                        BSP_LED_3_MASK

#ifdef COMMISSIONING_ENABLED
#define ERASE_BUTTON_PIN_NO             BSP_BUTTON_3                                          /**< Button used to erase commissioning settings. */
#endif // COMMISSIONING_ENABLED

#define COMMAND_OFF                     0x30
#define COMMAND_ON                      0x31
#define COMMAND_TOGGLE                  0x32

#define LED_BLINK_INTERVAL_MS           300                                                   /**< LED blinking interval. */
#define COAP_TICK_INTERVAL_MS           1000                                                  /**< Interval between periodic callbacks to CoAP module. */

#define APP_RTR_SOLICITATION_DELAY      500                                                   /**< Time before host sends an initial solicitation in ms. */

#define DEAD_BEEF                       0xDEADBEEF                                            /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define MAX_LENGTH_FILENAME             128                                                   /**< Max length of filename to copy for the debug error handler. */

#define APP_ENABLE_LOGS                 1                                                     /**< Enable logs in the application. */

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
} display_state_t;

static ipv6_medium_instance_t m_ipv6_medium;
eui64_t                       eui64_local_iid;                                                /**< Local EUI64 value that is used as the IID for*/
static const ipv6_addr_t      local_routers_multicast_addr = {{0xFF, 0x02, 0x00, 0x00,
                                                               0x00, 0x00, 0x00, 0x00,
                                                               0x00, 0x00, 0x00, 0x00,
                                                               0x00, 0x00, 0x00, 0x02}};      /**< Multi-cast address of all routers on the local network segment. */

APP_TIMER_DEF(m_iot_timer_tick_src_id);                                                       /**< App timer instance used to update the IoT timer wall clock. */
static uint8_t                m_well_known_core[100];
static display_state_t        m_display_state = LEDS_INACTIVE;                                /**< Board LED display state. */
static const char             m_lights_name[] = "lights";
static const char             m_led3_name[]   = "led3";
static const char             m_led4_name[]   = "led4";

#ifdef COMMISSIONING_ENABLED
static bool                    m_power_off_on_failure = false;
static bool                    m_identity_mode_active;
#endif // COMMISSIONING_ENABLED

static const uint8_t identity[] = {0x43, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x5f, 0x69, 0x64, 0x65, 0x6e, 0x74, 0x69, 0x74, 0x79};
static const uint8_t shared_secret[] = {0x73, 0x65, 0x63, 0x72, 0x65, 0x74, 0x50, 0x53, 0x4b};

static nrf_tls_preshared_key_t m_preshared_key = {
    .p_identity     = &identity[0],
    .p_secret_key   = &shared_secret[0],
    .identity_len   = 15,
    .secret_key_len = 9
};

static nrf_tls_key_settings_t m_dtls_keys = {
    .p_psk = &m_preshared_key
};


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


/**@brief Function for handling CoAP periodically time ticks.
*/
static void app_coap_time_tick(iot_timer_time_in_ms_t wall_clock_value)
{
    // Pass a tick to coap in order to re-transmit any pending messages.
    (void)coap_time_tick();
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

    // Create a sys timer.
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
        {app_coap_time_tick,      COAP_TICK_INTERVAL_MS},
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
    return;
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
        {ERASE_BUTTON_PIN_NO, false, BUTTON_PULL, button_event_handler}
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
            m_display_state = LEDS_IPV6_IF_UP;

            APPL_LOG("Sending Router Solicitation to all routers!");

            // Create Link Local addresses
            IPV6_CREATE_LINK_LOCAL_FROM_EUI64(&src_addr, p_interface->local_addr.identifier);

            // Delay first solicitation due to possible restriction on other side.
            nrf_delay_ms(APP_RTR_SOLICITATION_DELAY);

            // Send Router Solicitation to all routers.
            err_code = icmp6_rs_send(p_interface,
                                     &src_addr,
                                     &local_routers_multicast_addr);
            APP_ERROR_CHECK(err_code);
            break;

        case IPV6_EVT_INTERFACE_DELETE:
#ifdef COMMISSIONING_ENABLED
            commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_START);
#endif // COMMISSIONING_ENABLED
            APPL_LOG("Interface removed!");
            m_display_state = LEDS_IPV6_IF_DOWN;

            // Free all DTLS sessions on the server port.
            UNUSED_VARIABLE(coap_security_destroy(LOCAL_PORT_NUM, NULL));

            break;

        case IPV6_EVT_INTERFACE_RX_DATA:
            APPL_LOG("Got unsupported protocol data!");
            break;

        default:
            //Unknown event. Should not happen.
            break;
    }
}


void well_known_core_callback(coap_resource_t * p_resource, coap_message_t * p_request)
{
    coap_message_conf_t response_config;
    memset(&response_config, 0x00, sizeof(coap_message_conf_t));

    if (p_request->header.type == COAP_TYPE_NON)
    {
        response_config.type = COAP_TYPE_NON;
    }
    else if (p_request->header.type == COAP_TYPE_CON)
    {
        response_config.type = COAP_TYPE_ACK;
    }

    // PIGGY BACKED RESPONSE
    response_config.code = COAP_CODE_205_CONTENT;
    // Copy message ID.
    response_config.id = p_request->header.id;
    // Set local port number to use.
    response_config.port.port_number = LOCAL_PORT_NUM;
    // Copy token.
    memcpy(&response_config.token[0], &p_request->token[0], p_request->header.token_len);
    // Copy token length.
    response_config.token_len = p_request->header.token_len;

    coap_message_t * p_response;
    uint32_t err_code = coap_message_new(&p_response, &response_config);
    APP_ERROR_CHECK(err_code);

    err_code = coap_message_remote_addr_set(p_response, &p_request->remote);
    APP_ERROR_CHECK(err_code);

    memcpy(&p_response->remote, &p_request->remote, sizeof(coap_remote_t));

    err_code = coap_message_opt_uint_add(p_response, COAP_OPT_CONTENT_FORMAT,
                                         COAP_CT_APP_LINK_FORMAT);
    APP_ERROR_CHECK(err_code);

    err_code = coap_message_payload_set(p_response, m_well_known_core,
                                        strlen((char *)m_well_known_core));
    APP_ERROR_CHECK(err_code);

    uint32_t msg_handle;
    err_code = coap_message_send(&msg_handle, p_response);
    APP_ERROR_CHECK(err_code);

    err_code = coap_message_delete(p_response);
    APP_ERROR_CHECK(err_code);
}


/**@brief Create representation of value based on content format.
*/
static void led_value_get(uint32_t led_mask, coap_content_type_t content_type, char ** str)
{
    if (content_type == COAP_CT_APP_JSON)
    {
        static char response_str_led3_true[15] = {"{\"led3\": True}\0"};
        static char response_str_led3_false[16] = {"{\"led3\": False}\0"};

        static char response_str_led4_true[15] = {"{\"led4\": True}\0"};
        static char response_str_led4_false[16] = {"{\"led4\": False}\0"};

        if ((bool)LED_IS_ON(led_mask) == true)
        {
            if (led_mask == LED_THREE)
            {
                *str = response_str_led3_true;
            }
            else
            {
                *str = response_str_led4_true;
            }
        }
        else
        {
            if (led_mask == LED_THREE)
            {
                *str = response_str_led3_false;
            }
            else
            {
                *str = response_str_led4_false;
            }
        }
    }
    else // For all other use plain text
    {
        static char response_str[2];
        memset(response_str, '\0', sizeof(response_str));
        sprintf(response_str, "%d", (bool)LED_IS_ON(led_mask));
        *str = response_str;
    }
}


static void handle_led_request(uint32_t led_mask, coap_message_t * p_request, coap_resource_t * p_resource)
{
    coap_message_conf_t response_config;
    memset(&response_config, 0x00, sizeof(coap_message_conf_t));

    if (p_request->header.type == COAP_TYPE_NON)
    {
        response_config.type = COAP_TYPE_NON;
    }
    else if (p_request->header.type == COAP_TYPE_CON)
    {
        response_config.type = COAP_TYPE_ACK;
    }

    // PIGGY BACKED RESPONSE
    response_config.code = COAP_CODE_405_METHOD_NOT_ALLOWED;
    // Copy message ID.
    response_config.id = p_request->header.id;
    // Set local port number to use.
    response_config.port.port_number = LOCAL_PORT_NUM;
    // Copy token.
    memcpy(&response_config.token[0], &p_request->token[0], p_request->header.token_len);
    // Copy token length.
    response_config.token_len = p_request->header.token_len;

    coap_message_t * p_response;
    uint32_t err_code = coap_message_new(&p_response, &response_config);
    APP_ERROR_CHECK(err_code);

    err_code = coap_message_remote_addr_set(p_response, &p_request->remote);
    APP_ERROR_CHECK(err_code);

    // Handle request.
    switch (p_request->header.code)
    {
        case COAP_CODE_GET:
        {
            p_response->header.code = COAP_CODE_205_CONTENT;

            // Select the first common content-type between the resource and the CoAP client.
            coap_content_type_t ct_to_use;
            err_code = coap_message_ct_match_select(&ct_to_use, p_request, p_resource);
            if (err_code != NRF_SUCCESS)
            {
                // None of the accept content formats are supported in this resource endpoint.
                p_response->header.code = COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT;
                p_response->header.type = COAP_TYPE_RST;
            }
            else
            {
                // Set response payload to actual LED state.
                char * response_str;
                led_value_get(led_mask, ct_to_use, &response_str);
                err_code = coap_message_payload_set(p_response, response_str, strlen(response_str));
                APP_ERROR_CHECK(err_code);
            }
            break;
        }

        case COAP_CODE_PUT:
        {
            p_response->header.code = COAP_CODE_204_CHANGED;

            // Change LED state according to request.
            switch (p_request->p_payload[0])
            {
                case COMMAND_ON:
                {
                    LEDS_ON(led_mask);
                    break;
                }
                case COMMAND_OFF:
                {
                    LEDS_OFF(led_mask);
                    break;
                }
                case COMMAND_TOGGLE:
                {
                    LEDS_INVERT(led_mask);
                    break;
                }
                default:
                {
                    p_response->header.code = COAP_CODE_400_BAD_REQUEST;
                    break;
                }
            }
            break;
        }

        default:
        {
            p_response->header.code = COAP_CODE_405_METHOD_NOT_ALLOWED;
            break;
        }
    }

    uint32_t msg_handle;
    err_code = coap_message_send(&msg_handle, p_response);
    APP_ERROR_CHECK(err_code);

    err_code = coap_message_delete(p_response);
    APP_ERROR_CHECK(err_code);
}


static void led3_callback(coap_resource_t * p_resource, coap_message_t * p_request)
{
    handle_led_request(LED_THREE, p_request, p_resource);
}


static void led4_callback(coap_resource_t * p_resource, coap_message_t * p_request)
{
    handle_led_request(LED_FOUR, p_request, p_resource);
}


static void coap_endpoints_init(void)
{
    uint32_t err_code;

    static coap_resource_t root;
    err_code = coap_resource_create(&root, "/");
    APP_ERROR_CHECK(err_code);

    static coap_resource_t well_known;
    err_code = coap_resource_create(&well_known, ".well-known");
    APP_ERROR_CHECK(err_code);
    err_code = coap_resource_child_add(&root, &well_known);
    APP_ERROR_CHECK(err_code);

    static coap_resource_t core;
    err_code = coap_resource_create(&core, "core");
    APP_ERROR_CHECK(err_code);

    core.permission = COAP_PERM_GET;
    core.callback   = well_known_core_callback;

    err_code = coap_resource_child_add(&well_known, &core);
    APP_ERROR_CHECK(err_code);

    static coap_resource_t lights;
    err_code = coap_resource_create(&lights, m_lights_name);
    APP_ERROR_CHECK(err_code);
    err_code = coap_resource_child_add(&root, &lights);
    APP_ERROR_CHECK(err_code);

    static coap_resource_t led3;
    err_code = coap_resource_create(&led3, m_led3_name);
    APP_ERROR_CHECK(err_code);

    led3.permission      = (COAP_PERM_GET | COAP_PERM_PUT);
    led3.callback        = led3_callback;
    led3.ct_support_mask = COAP_CT_MASK_APP_JSON | COAP_CT_MASK_PLAIN_TEXT;

    err_code = coap_resource_child_add(&lights, &led3);
    APP_ERROR_CHECK(err_code);

    static coap_resource_t led4;
    err_code = coap_resource_create(&led4, m_led4_name);
    APP_ERROR_CHECK(err_code);

    led4.permission      = (COAP_PERM_GET | COAP_PERM_PUT);
    led4.callback        = led4_callback;
    led4.ct_support_mask = COAP_CT_MASK_APP_JSON | COAP_CT_MASK_PLAIN_TEXT;
    err_code = coap_resource_child_add(&lights, &led4);
    APP_ERROR_CHECK(err_code);

    uint16_t size = sizeof(m_well_known_core);
    err_code = coap_resource_well_known_generate(m_well_known_core, &size);
    APP_ERROR_CHECK(err_code);
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


static void coap_error_handler(uint32_t error_code, coap_message_t * p_message)
{
    // If any response fill the p_response with a appropriate response message.
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

    ip_stack_init();

    /** Table of ports used for CoAP server and/or client endpoints. */
    const coap_port_t coap_ports[COAP_PORT_COUNT] =
    {
        {
            .port_number = LOCAL_PORT_NUM
        }
    };

    coap_transport_init_t transport_param =
    {
       .p_port_table = (coap_port_t *)&coap_ports[0]
    };

    err_code = coap_init(17, &transport_param);
    APP_ERROR_CHECK(err_code);

    // Register server to listen on secure port 5684.
    err_code = coap_security_setup(LOCAL_PORT_NUM,
                                   NRF_TLS_ROLE_SERVER,
                                   NULL,
                                   &m_dtls_keys);
    APP_ERROR_CHECK(err_code);

    err_code = coap_error_handler_register(coap_error_handler);
    APP_ERROR_CHECK(err_code);

    coap_endpoints_init();

    iot_timer_init();

    APPL_LOG("Application started.");

    // Start execution
    connectable_mode_enter();

    // Enter main loop
    for (;;)
    {
        app_sched_execute();

        if (NRF_LOG_PROCESS() == false)
        {
            err_code = sd_app_evt_wait();
            APP_ERROR_CHECK(err_code);
        }
    }
}

/**
 * @}
 */
