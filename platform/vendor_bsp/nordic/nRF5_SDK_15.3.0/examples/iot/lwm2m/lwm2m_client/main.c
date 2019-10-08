/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
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
 * @defgroup ble_sdk_app_lwm2m_client_main main.c
 * @{
 * @ingroup ble_sdk_lwm2m_client
 * @brief LWM2M client main file.
 *
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
#include "mem_manager.h"
#include "nrf_delay.h"
#include "iot_common.h"
#include "ipv6_api.h"
#include "icmp6_api.h"
#include "udp_api.h"
#include "iot_timer.h"
#include "coap_api.h"
#include "coap_option.h"
#include "lwm2m_api.h"
#include "lwm2m_objects_tlv.h"
#include "ipso_objects_tlv.h"
#include "addr_parse.h"
#include "ipv6_medium.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define USE_SECURITY 1

#if (USE_SECURITY == 1)
#define BOOTSTRAP_LOCAL_PORT            5684                                                  /**< Port to be used as local port for bootstrapping. */
#define SERVER_LOCAL_PORT               BOOTSTRAP_LOCAL_PORT                                  /**< Port to be used as local port for server registration. */
#else
#define BOOTSTRAP_LOCAL_PORT            5683                                                  /**< Port to be used as local port for bootstrapping. */
#define SERVER_LOCAL_PORT               BOOTSTRAP_LOCAL_PORT                                  /**< Port to be used as local port for server registration. */
#endif

#if (USE_SECURITY == 1)
#define BOOTSTRAP_URI  "coaps://[2001:db8::1]:5684"                                           /**< Server URI to the bootstrap server when using security (DTLS). */
#else
#define BOOTSTRAP_URI  "coap://[2001:db8::1]:5683"                                            /**< Server URI to the bootstrap server when not used with security (DTLS). */
#endif

#define LWM2M_SERVERS_TOTAL_NUM         2                                                     /**< Total number of LWM2M servers including BS server. */

#define CLIENT_UUID                     "0a18de70-0ce0-4570-bce9-7f5895db6c70"                /**< UUID of the device. */

#define SCHED_MAX_EVENT_DATA_SIZE       16                                                    /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE                192                                                   /**< Maximum number of events in the scheduler queue. */

#define LED_ONE                         BSP_LED_0_MASK
#define LED_TWO                         BSP_LED_1_MASK
#define LED_THREE                       BSP_LED_2_MASK
#define LED_FOUR                        BSP_LED_3_MASK
#define ALL_APP_LED                    (BSP_LED_0_MASK | BSP_LED_1_MASK | \
                                        BSP_LED_2_MASK | BSP_LED_3_MASK)                      /**< Define used for simultaneous operation of all application LEDs. */

#define IPSO_LED                        LED_FOUR

#define BUTTON_ONE                      BSP_BUTTON_0
#define BUTTON_TWO                      BSP_BUTTON_1
#define BUTTON_THREE                    BSP_BUTTON_2
#ifdef COMMISSIONING_ENABLED
#define ERASE_BUTTON_PIN_NO             BSP_BUTTON_3                                          /**< Button used to erase commissioning settings. */
#endif // COMMISSIONING_ENABLED

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50)

#define LED_BLINK_INTERVAL_MS           300                                                   /**< LED blinking interval. */
#define COAP_TICK_INTERVAL_MS           1000                                                  /**< Interval between periodic callbacks to CoAP module. */
#define LWM2M_TICK_INTERVAL_MS          3000                                                  /**< Interval between periodic bootstrap or server registration attempts. */

#define LWM2M_RETRY_COUNT_MAX           5                                                     /**< Max number of bootstrap attempts. */
#define SECURITY_SERVER_URI_SIZE_MAX    32                                                    /**< Max size of server URIs. */

#define BOOTSTRAP_SECURITY_INSTANCE_IDX 0                                                     /**< Index of bootstrap security instance. */
#define SERVER_SECURITY_INSTANCE_IDX    1                                                     /**< Index of server security instance. */
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

static ipv6_medium_instance_t              m_ipv6_medium;
eui64_t                                    eui64_local_iid;                                   /**< Local EUI64 value that is used as the IID for*/

static const ipv6_addr_t      local_routers_multicast_addr = {{
        0xFF, 0x02, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02
    }
};                                                                                            /**< Multi-cast address of all routers on the local network segment. */

APP_TIMER_DEF(m_iot_timer_tick_src_id);                                                       /**< App timer instance used to update the IoT timer wall clock. */

static char m_security_server_uri[2][SECURITY_SERVER_URI_SIZE_MAX] = {BOOTSTRAP_URI, ""};

static lwm2m_remote_t                      m_bootstrap_remote;                                /**< Remote structure for bootstrap server. */
static lwm2m_remote_t                      m_server_remote;                                   /**< Remote structure for server. */
static lwm2m_server_config_t               m_server_conf;                                     /**< Server configuration structure. */
static lwm2m_client_identity_t             m_client_id;                                       /**< Client ID structure to hold the client's UUID. */
static bool                                m_use_dtls[2]            = {false, false};         /**< Array to keep track of which of the connections to bootstrap server and server is using a secure link. */
static display_state_t                     m_display_state          = LEDS_INACTIVE;          /**< Board LED display state. */
static bool                                m_bootstrapped           = false;                  /**< Variable holding the state of whether the devices is completed bootstrapping or not. */
static bool                                m_bootstrap_retry        = false;                  /**< Variable holding the state of whether the devices is should retry bootstrapping. */
static bool                                m_register_retry         = false;                  /**< Variable holding the state of whether the devices is should retry registration with the server. */
static uint8_t                             m_bootstrap_retry_count  = 0;                      /**< Variable holding the number of attempts to try to bootstrap before giving up. */
static uint8_t                             m_register_retry_count   = 0;                      /**< Variable holding the number of attempts to try register to the server before giving up. */
static uint8_t *                           mp_link_format_string    = NULL;                   /**< Pointer to hold a link format string across a button press initiated registration and retry. */
static uint32_t                            m_link_format_string_len = 0;                      /**< Length of the link format string that is used in registration attempts. */

// Objects
static lwm2m_object_prototype_t            m_object_security;                                 /**< LWM2M security base object. */
static lwm2m_object_prototype_t            m_object_server;                                   /**< LWM2M server base object. */
static lwm2m_object_prototype_t            m_object_digital_out;                              /**< IPSO digital output base object. */
static lwm2m_object_prototype_t            m_bootstrap_server;                                /**< Named object to be used as callback object when bootstrap is completed. */

static char m_bootstrap_object_alias_name[] = "bs";                                           /**< Name of the bootstrap complete object. */

// Instances
static lwm2m_server_t                      m_instance_server;                                 /**< Server object instance to be filled by the bootstrap server. */
static lwm2m_security_t                    m_instance_security[2];                            /**< Security object instances. Index 0 is always bootstrap instance. */
static ipso_digital_output_t               m_instance_digital_out;                            /**< IPSO digital output object instance. */
static char                                m_ipso_digital_output_led4_app_type[] = "LED4";    /**< Application type string to be used by the IPSO digital output instance. */

#ifdef COMMISSIONING_ENABLED
static bool                                m_power_off_on_failure = false;
static bool                                m_identity_mode_active;
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


/**@brief Function for handling LWM2M periodically bootstrap attempts.
*/
static void bootstrap_retry_tick(iot_timer_time_in_ms_t wall_clock_value)
{

    if (m_bootstrap_retry == true)
    {
        if (m_bootstrap_retry_count < LWM2M_RETRY_COUNT_MAX)
        {
            uint32_t err_code = lwm2m_bootstrap(&m_bootstrap_remote, &m_client_id, BOOTSTRAP_LOCAL_PORT);
            if (err_code == NRF_SUCCESS)
            {
                m_bootstrap_retry = false;
            }

            if (err_code == NRF_TLS_HANDSHAKE_IN_PROGRESS)
            {
                // delay lwm2m_bootstrapping. Try again by using iot_timer to do re-attempts.
                m_bootstrap_retry_count++;
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        }
    }

    if (m_register_retry == true &&
        m_bootstrapped == true)
    {
        if (m_register_retry_count < LWM2M_RETRY_COUNT_MAX)
        {
            uint32_t err_code = lwm2m_register(&m_server_remote,
                                               &m_client_id,
                                               &m_server_conf,
                                               SERVER_LOCAL_PORT,
                                               mp_link_format_string,
                                               (uint16_t)m_link_format_string_len);
            if (err_code == NRF_SUCCESS)
            {
                m_register_retry = false;
            }

            if (err_code == NRF_TLS_HANDSHAKE_IN_PROGRESS)
            {
                // Delay LWM2M server registration. Try again by using iot_timer to do re-attempts.
                m_register_retry_count++;
            }
            else
            {
                // An unexpected error occurred, clean up.
                if (mp_link_format_string != NULL)
                {
                    // No more attempts, clean up.
                    nrf_free(mp_link_format_string);
                    mp_link_format_string = NULL;
                }

                APP_ERROR_CHECK(err_code);
            }
        }
        else
        {
            if (mp_link_format_string != NULL)
            {
                // No more attempts, clean up.
                nrf_free(mp_link_format_string);
                mp_link_format_string = NULL;
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
        {bootstrap_retry_tick,    LWM2M_TICK_INTERVAL_MS},
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


/**@brief Function for handling interface events from the IP stack. */
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
}


/**@brief Application implementation of the root handler interface.
 *
 * @details This function is not bound to any object or instance. It will be called from
 *          LWM2M upon an action on the root "/" URI path. During bootstrap it is expected
 *          to get a DELETE operation on this URI.
 */
uint32_t lwm2m_coap_handler_root(uint8_t op_code, coap_message_t * p_request)
{
    (void)lwm2m_respond_with_code(COAP_CODE_202_DELETED, p_request);

    // Delete any existing objects or instances if needed.

    return NRF_SUCCESS;
}


/**@brief LWM2M notification handler. */
uint32_t lwm2m_notification(lwm2m_notification_type_t type,
                            lwm2m_remote_t *          p_remote,
                            uint8_t                   coap_code)
{
    APPL_LOG("Got LWM2M notifcation %d ", type);

    if (type == LWM2M_NOTIFCATION_TYPE_REGISTER)
    {
        // We have successfully registered, free up the allocated link format string.
        if (mp_link_format_string != NULL)
        {
            // No more attempts, clean up.
            nrf_free(mp_link_format_string);
            mp_link_format_string = NULL;
        }
    }

    return NRF_SUCCESS;
}


/**@brief Callback function for the named bootstrap complete object. */
uint32_t bootstrap_object_callback(lwm2m_object_prototype_t * p_object,
                                   uint16_t                   instance_id,
                                   uint8_t                    op_code,
                                   coap_message_t *           p_request)
{
    m_bootstrapped = true;
    LEDS_ON(LED_THREE);

    (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);

    return NRF_SUCCESS;
}


/**@brief Callback function for IPSO digital output instances. */
uint32_t ipso_instance_callback(lwm2m_instance_prototype_t * p_instance,
                                uint16_t                     resource_id,
                                uint8_t                      op_code,
                                coap_message_t *             p_request)
{
    ipso_digital_output_t * ipso_dig_out = (ipso_digital_output_t *)p_instance;

    APPL_LOG("lwm2m: ipso_instance_callback, instance: %u, resource id: %u",
             p_instance->instance_id,
             resource_id);

    switch (resource_id)
    {
        case IPSO_RR_ID_DIGITAL_OUTPUT_STATE:
        {
            switch (op_code)
            {
                case LWM2M_OPERATION_CODE_READ:
                {
                    uint8_t dig_out_state = ipso_dig_out->digital_output_state + '0'; // Convert to ASCII
                    (void)lwm2m_respond_with_payload(&dig_out_state, 1,  p_request);
                    break;
                }

                case LWM2M_OPERATION_CODE_WRITE:
                {
                    bool dig_out_state = (bool)(p_request->p_payload[0] - '0');
                    ipso_dig_out->digital_output_state = dig_out_state;

                    if (ipso_dig_out->digital_output_state == true)
                    {
                        if (ipso_dig_out->digital_output_polarity == (bool)IPSO_RR_ID_DIGITAL_OUTPUT_POLARITY_NORMAL)
                        {
                            LEDS_ON(IPSO_LED);
                        }
                        else
                        {
                            LEDS_OFF(IPSO_LED);
                        }
                    }
                    else
                    {
                        if (ipso_dig_out->digital_output_polarity == (bool)IPSO_RR_ID_DIGITAL_OUTPUT_POLARITY_NORMAL)
                        {
                            LEDS_OFF(IPSO_LED);
                        }
                        else
                        {
                            LEDS_ON(IPSO_LED);
                        }
                    }

                    (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);
                    break;
                }

                default:
                {
                    (void)lwm2m_respond_with_code(COAP_CODE_405_METHOD_NOT_ALLOWED, p_request);
                    break;
                }
            }
            break;
        }

        case IPSO_RR_ID_DIGITAL_OUTPUT_POLARITY:
        {
            switch (op_code)
            {
                case LWM2M_OPERATION_CODE_READ:
                {
                    uint8_t dig_out_polarity = ipso_dig_out->digital_output_polarity + '0'; // Convert to ASCII
                    (void)lwm2m_respond_with_payload(&dig_out_polarity, 1,  p_request);
                    break;
                }

                case LWM2M_OPERATION_CODE_WRITE:
                {
                    bool dig_out_polarity = (bool)(p_request->p_payload[0] - '0');
                    ipso_dig_out->digital_output_polarity = dig_out_polarity;
                   (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);
                    break;
                }

                default:
                    break;
            }

            break;
        }

        case IPSO_RR_ID_APPLICATION_TYPE:
        {
            switch (op_code)
            {
                case LWM2M_OPERATION_CODE_READ:
                {
                   (void)lwm2m_respond_with_payload((uint8_t *)ipso_dig_out->application_type.p_val,
                                                    ipso_dig_out->application_type.len,
                                                    p_request);
                    break;
                }

                case LWM2M_OPERATION_CODE_WRITE:
                {
                    (void)lwm2m_respond_with_code(COAP_CODE_405_METHOD_NOT_ALLOWED, p_request);
                    break;
                }

                default:
                    break;
            }

            break;
        }

        case LWM2M_INVALID_RESOURCE:
        {
            // This is a callback to the instance, not a specific resource.
            switch (op_code)
            {
                case LWM2M_OPERATION_CODE_READ:
                {
                    uint8_t  buffer[100];
                    uint32_t buffer_size = sizeof(buffer);

                    (void)ipso_tlv_ipso_digital_output_encode(buffer, &buffer_size, ipso_dig_out);

                    (void)lwm2m_respond_with_payload(buffer,
                                                     buffer_size,
                                                     p_request);
                    break;
                }

                case LWM2M_OPERATION_CODE_WRITE:
                {
                    (void)ipso_tlv_ipso_digital_output_decode(ipso_dig_out,
                                                              p_request->p_payload,
                                                              p_request->payload_len);

                    // As decode will overwrite the lwm2m_string_t we need to set it manually.
                    // strlen to not count the 0-terminator.
                    uint8_t str_size = strlen(m_ipso_digital_output_led4_app_type);
                    if (ipso_dig_out->application_type.len < str_size)
                    {
                        str_size = ipso_dig_out->application_type.len;
                    }

                    memset(m_ipso_digital_output_led4_app_type, 0, sizeof(m_ipso_digital_output_led4_app_type));
                    memcpy(m_ipso_digital_output_led4_app_type,
                           ipso_dig_out->application_type.p_val,
                           str_size);

                    // Point to the static buffer again.
                    ipso_dig_out->application_type.p_val = m_ipso_digital_output_led4_app_type;
                    ipso_dig_out->application_type.len   = str_size;

                   (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);
                    break;
                }

                default:
                    break;

            }

            break;
        }

        default:
        {
            (void)lwm2m_respond_with_code(COAP_CODE_501_NOT_IMPLEMENTED, p_request);
            break;
        }
    }

    return NRF_SUCCESS;
}


/**@brief Callback function for LWM2M server instances. */
uint32_t server_instance_callback(lwm2m_instance_prototype_t * p_instance,
                                  uint16_t                     resource_id,
                                  uint8_t                      op_code,
                                  coap_message_t *             p_request)
{
    APPL_LOG("lwm2m: server_instance_callback");

    uint32_t err_code = NRF_SUCCESS;

    if (op_code == LWM2M_OPERATION_CODE_WRITE)
    {
        // Remember to make a copy of lwm2m_string_t and lwm2m_opaque_t you want to keep.
        // They will be freed after this callback.
        (void)lwm2m_tlv_server_decode(&m_instance_server,
                                      p_request->p_payload,
                                      p_request->payload_len);

        (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);
    }

    return err_code;
}


/**@brief Callback function for LWM2M server objects. */
uint32_t server_object_callback(lwm2m_object_prototype_t * p_object,
                                uint16_t                   instance_id,
                                uint8_t                    op_code,
                                coap_message_t *           p_request)
{
    APPL_LOG("lwm2m: server_object_callback");

    uint32_t err_code = NRF_SUCCESS;

    if (op_code == LWM2M_OPERATION_CODE_WRITE)
    {
        // Remember to make a copy of lwm2m_string_t and lwm2m_opaque_t you want to keep.
        // They will be freed after this callback.
        (void)lwm2m_tlv_server_decode(&m_instance_server,
                                      p_request->p_payload,
                                      p_request->payload_len);



        m_instance_server.proto.instance_id = instance_id;
        m_instance_server.proto.object_id   = p_object->object_id;
        m_instance_server.proto.callback    = server_instance_callback;

        // Cast the instance to its prototype and add it.
        // This application only supports one instance.
        (void)lwm2m_coap_handler_instance_delete((lwm2m_instance_prototype_t *)&m_instance_server);
        (void)lwm2m_coap_handler_instance_add((lwm2m_instance_prototype_t *)&m_instance_server);

        (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);
    }

    return err_code;
}


/**@brief Callback function for LWM2M security instances. */
uint32_t security_instance_callback(lwm2m_instance_prototype_t * p_instance,
                                    uint16_t                     resource_id,
                                    uint8_t                      op_code,
                                    coap_message_t *             p_request)
{
    APPL_LOG("lwm2m: security_instance_callback");

    if (op_code == LWM2M_OPERATION_CODE_WRITE)
    {
        uint16_t instance_id = p_instance->instance_id;

        // Remember to make a copy of lwm2m_string_t and lwm2m_opaque_t you want to keep.
        // They will be freed after this callback
        uint32_t err_code = lwm2m_tlv_security_decode(&m_instance_security[instance_id],
                                                      p_request->p_payload,
                                                      p_request->payload_len);
        APP_ERROR_CHECK(err_code);

        // Copy the URI. Can be changed to handle all resources in the instance.
        if (m_instance_security[instance_id].server_uri.len < SECURITY_SERVER_URI_SIZE_MAX)
        {
            // Copy the lwm2m_string_t server_uri into a application allocated memory space
            // and point to that one.
            memset(m_security_server_uri[instance_id], 0, SECURITY_SERVER_URI_SIZE_MAX);

            memcpy(m_security_server_uri[instance_id],
                   m_instance_security[instance_id].server_uri.p_val,
                   m_instance_security[instance_id].server_uri.len);

            m_instance_security[instance_id].server_uri.p_val =
                (char *)m_security_server_uri[instance_id];

            (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);
        }
        else
        {
            // URI was to long to be copied.
            (void)lwm2m_respond_with_code(COAP_CODE_400_BAD_REQUEST, p_request);
        }
    }

    return NRF_SUCCESS;
}


/**@brief Callback function for LWM2M object instances. */
uint32_t security_object_callback(lwm2m_object_prototype_t * p_object,
                                  uint16_t                   instance_id,
                                  uint8_t                    op_code,
                                  coap_message_t *           p_request)
{
    APPL_LOG("lwm2m: security_object_callback, instance %u", instance_id);

    if (op_code == LWM2M_OPERATION_CODE_WRITE)
    {
        // Remember to make a copy of lwm2m_string_t and lwm2m_opaque_t you want to keep.
        // They will be freed after this callback
        uint32_t err_code = lwm2m_tlv_security_decode(&m_instance_security[instance_id],
                                                      p_request->p_payload,
                                                      p_request->payload_len);
        APP_ERROR_CHECK(err_code);

        m_instance_security[instance_id].proto.instance_id = instance_id;
        m_instance_security[instance_id].proto.object_id   = p_object->object_id;
        m_instance_security[instance_id].proto.callback    = security_instance_callback;

        // Copy the URI. Can be changed to handle all resources in the instance.
        if (m_instance_security[instance_id].server_uri.len < SECURITY_SERVER_URI_SIZE_MAX)
        {
            // Copy the lwm2m_string_t server_uri into a application allocated memory space
            // and point to that one.
            memset(m_security_server_uri[instance_id], 0, SECURITY_SERVER_URI_SIZE_MAX);

            memcpy(m_security_server_uri[instance_id],
                   m_instance_security[instance_id].server_uri.p_val,
                   m_instance_security[instance_id].server_uri.len);

            m_instance_security[instance_id].server_uri.p_val =
                (char *)m_security_server_uri[instance_id];

            // Cast the instance to its prototype and add it to the coap handler to become a
            // public instance. We can only have one so we delete the first if any.
            (void)lwm2m_coap_handler_instance_delete(
                (lwm2m_instance_prototype_t *)&m_instance_security[instance_id]);

            (void)lwm2m_coap_handler_instance_add(
                (lwm2m_instance_prototype_t *)&m_instance_security[instance_id]);

            (void)lwm2m_respond_with_code(COAP_CODE_204_CHANGED, p_request);
        }
        else
        {
            // URI was to long to be copied.
            (void)lwm2m_respond_with_code(COAP_CODE_400_BAD_REQUEST, p_request);
        }
    }

    return NRF_SUCCESS;
}


/**@brief LWM2M initialization.
 *
 * @details The function will register all implemented base objects as well as initial registration
 *          of existing instances. If bootstrap is not performed, the registration to the server
 *          will use what is initialized in this function.
 */
static void lwm2m_setup(void)
{
    (void)lwm2m_init();

    m_bootstrap_server.object_id    = LWM2M_NAMED_OBJECT;
    m_bootstrap_server.callback     = bootstrap_object_callback;
    m_bootstrap_server.p_alias_name = m_bootstrap_object_alias_name;
    (void)lwm2m_coap_handler_object_add(&m_bootstrap_server);

    // Add security support.
    m_object_security.object_id = LWM2M_OBJ_SECURITY;
    m_object_security.callback = security_object_callback;
    (void)lwm2m_coap_handler_object_add(&m_object_security);

    // Add server support.
    m_object_server.object_id = LWM2M_OBJ_SERVER;
    m_object_server.callback = server_object_callback;
    (void)lwm2m_coap_handler_object_add(&m_object_server);

    // Add digital output object support.
    m_object_digital_out.object_id = IPSO_SO_ID_DIGITAL_OUTPUT;
    (void)lwm2m_coap_handler_object_add(&m_object_digital_out);

    // Initialize the instances.
    lwm2m_instance_security_init(&m_instance_security[BOOTSTRAP_SECURITY_INSTANCE_IDX]);
    m_instance_security[BOOTSTRAP_SECURITY_INSTANCE_IDX].proto.instance_id = 0;

    lwm2m_instance_security_init(&m_instance_security[SERVER_SECURITY_INSTANCE_IDX]);
    m_instance_security[SERVER_SECURITY_INSTANCE_IDX].proto.instance_id = 1;

    lwm2m_instance_server_init(&m_instance_server);
    m_instance_server.proto.instance_id = 1;

    ipso_instance_digital_output_init(&m_instance_digital_out);
    m_instance_digital_out.proto.instance_id = 1;

    // Set default state.
    m_instance_digital_out.digital_output_state    = false;

    // Set default polarity.
    m_instance_digital_out.digital_output_polarity = IPSO_RR_ID_DIGITAL_OUTPUT_POLARITY_NORMAL;

    // Configure LED4 string in the IPSO instance.
    m_instance_digital_out.application_type.p_val  = m_ipso_digital_output_led4_app_type;
    m_instance_digital_out.application_type.len    = strlen(m_ipso_digital_output_led4_app_type);

    // Set callbacks.
    m_instance_digital_out.proto.callback = ipso_instance_callback;

    // Add predefined instances.
    (void)lwm2m_coap_handler_instance_add((lwm2m_instance_prototype_t *)&m_instance_digital_out);

    // Set client ID.
    memcpy(&m_client_id.value.uuid[0], CLIENT_UUID, LWM2M_CLIENT_ID_TYPE_UUID);
    m_client_id.type = LWM2M_CLIENT_ID_TYPE_UUID;
}


/**@brief Function for handling button events.
 *
 * @param[in]   pin_no         The pin number of the button pressed.
 * @param[in]   button_action  The action performed on button.
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    uint32_t err_code;

    if (button_action == APP_BUTTON_PUSH)
    {
        switch (pin_no)
        {
            case BUTTON_ONE:
            {
                // Bootstrap the device.
                m_bootstrapped = false;

                err_code = addr_parse_uri(m_bootstrap_remote.addr,
                                          &m_bootstrap_remote.port_number,
                                          &m_use_dtls[BOOTSTRAP_SECURITY_INSTANCE_IDX],
                                          m_security_server_uri[BOOTSTRAP_SECURITY_INSTANCE_IDX],
                                          strlen(m_security_server_uri[BOOTSTRAP_SECURITY_INSTANCE_IDX]));
                APP_ERROR_CHECK(err_code);

                if (m_use_dtls[BOOTSTRAP_SECURITY_INSTANCE_IDX] == true)
                {
                    APPL_LOG("SECURE session (bootstrap)");
                    static char key_identity[] = "identity0";
                    static char key_secret[]   = "topsecret0";

                    static nrf_tls_preshared_key_t m_preshared_key;

                    m_preshared_key.p_identity = (uint8_t *)key_identity;
                    m_preshared_key.identity_len = strlen(key_identity);
                    m_preshared_key.p_secret_key = (uint8_t *)key_secret;
                    m_preshared_key.secret_key_len = strlen(key_secret);

                    nrf_tls_key_settings_t key_settings;
                    key_settings.p_psk = &m_preshared_key;

                    // Allocate DTLS instance.
                    err_code = coap_security_setup(BOOTSTRAP_LOCAL_PORT,
                                                   NRF_TLS_ROLE_CLIENT,
                                                   &m_bootstrap_remote,
                                                   &key_settings);
                    APP_ERROR_CHECK(err_code);
                }
                else
                {
                    APPL_LOG("NON-SECURE session (bootstrap)");
                }

                err_code = lwm2m_bootstrap(&m_bootstrap_remote, &m_client_id, BOOTSTRAP_LOCAL_PORT);
                if (err_code == NRF_TLS_HANDSHAKE_IN_PROGRESS)
                {
                    // Delay LWM2M bootstrapping. Try again by using iot_timer to do re-attempts.
                    m_bootstrap_retry = true;
                }
                else
                {
                    APP_ERROR_CHECK(err_code);
                }

                break;
            }

            case BUTTON_TWO:
            {
                // Register to the server provided during bootstrap.
                if (m_bootstrapped == true)
                {
                    err_code = addr_parse_uri(m_server_remote.addr,
                                              &m_server_remote.port_number,
                                              &m_use_dtls[SERVER_SECURITY_INSTANCE_IDX],
                                              m_instance_security[SERVER_SECURITY_INSTANCE_IDX].server_uri.p_val,
                                              m_instance_security[SERVER_SECURITY_INSTANCE_IDX].server_uri.len);
                    APP_ERROR_CHECK(err_code);

                    if (m_use_dtls[SERVER_SECURITY_INSTANCE_IDX] == true)
                    {
                        APPL_LOG("SECURE session (register)");

                        static char key_identity[] = "identity";
                        static char key_secret[]   = "topsecret";

                        static nrf_tls_preshared_key_t m_preshared_key;

                        m_preshared_key.p_identity = (uint8_t *)key_identity;
                        m_preshared_key.identity_len = strlen(key_identity);
                        m_preshared_key.p_secret_key = (uint8_t *)key_secret;
                        m_preshared_key.secret_key_len = strlen(key_secret);

                        nrf_tls_key_settings_t key_settings;
                        key_settings.p_psk = &m_preshared_key;

                        // Allocate DTLS instance.
                        err_code = coap_security_setup(SERVER_LOCAL_PORT,
                                                       NRF_TLS_ROLE_CLIENT,
                                                       &m_server_remote,
                                                       &key_settings);
                        APP_ERROR_CHECK(err_code);
                    }
                    else
                    {
                        APPL_LOG("NON-SECURE session (register)");
                    }

                    // Initialize server configuration structure.
                    memset(&m_server_conf, 0, sizeof(lwm2m_server_config_t));
                    m_server_conf.lifetime = 1000;

                    // Dry run the link format generation, to check how much memory that is needed.
                    err_code = lwm2m_coap_handler_gen_link_format(NULL, (uint16_t *)&m_link_format_string_len);
                    APP_ERROR_CHECK(err_code);

                    // Allocate the needed amount of memory.
                    err_code = nrf_mem_reserve(&mp_link_format_string, &m_link_format_string_len);
                    APP_ERROR_CHECK(err_code);

                    // Render the link format string.
                    err_code = lwm2m_coap_handler_gen_link_format(mp_link_format_string, (uint16_t *)&m_link_format_string_len);
                    APP_ERROR_CHECK(err_code);

                    err_code = lwm2m_register(&m_server_remote,
                                              &m_client_id,
                                              &m_server_conf,
                                              SERVER_LOCAL_PORT,
                                              mp_link_format_string,
                                              (uint16_t)m_link_format_string_len);
                    if (err_code == NRF_TLS_HANDSHAKE_IN_PROGRESS)
                    {
                        // Delay LWM2M server registration. Try again by using iot_timer to do re-attempts.
                        m_register_retry = true;
                    }
                    else
                    {
                        APP_ERROR_CHECK(err_code);
                    }
                }

                break;
            }

            case BUTTON_THREE:
            {
                // Destroy the secure session if any.
                if (m_bootstrapped == true)
                {
                    if (m_use_dtls[BOOTSTRAP_SECURITY_INSTANCE_IDX] == true)
                    {
                        err_code = coap_security_destroy(BOOTSTRAP_LOCAL_PORT, &m_bootstrap_remote);
                        APP_ERROR_CHECK(err_code);
                    }

                    if (m_use_dtls[SERVER_SECURITY_INSTANCE_IDX] == true)
                    {
                        err_code = coap_security_destroy(SERVER_LOCAL_PORT, &m_server_remote);
                        APP_ERROR_CHECK(err_code);
                    }
                }
                break;
            }

#ifdef COMMISSIONING_ENABLED
            case ERASE_BUTTON_PIN_NO:
            {
                APPL_LOG("Erasing all commissioning settings from persistent storage...");
                commissioning_settings_clear();

                break;
            }
#endif // COMMISSIONING_ENABLED

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
        {BUTTON_ONE,          false, BUTTON_PULL, button_event_handler},
        {BUTTON_TWO,          false, BUTTON_PULL, button_event_handler},
        {BUTTON_THREE,        false, BUTTON_PULL, button_event_handler},
#ifdef COMMISSIONING_ENABLED
        {ERASE_BUTTON_PIN_NO, false, BUTTON_PULL, button_event_handler}
#endif // COMMISSIONING_ENABLED
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons),
                               BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_button_enable();
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
    uint32_t err_code = NRF_SUCCESS;

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

#if (BOOTSTRAP_LOCAL_PORT == SERVER_LOCAL_PORT)
    // If bootstrap server and server is using the same port we can
    // only register the shared port once.
    coap_port_t local_port_list[COAP_PORT_COUNT] =
    {
        {.port_number = BOOTSTRAP_LOCAL_PORT}
    };
#else
    // If bootstrap server and server is using different port we can
    // register the ports individually.
    coap_port_t local_port_list[COAP_PORT_COUNT] =
    {
        {.port_number = BOOTSTRAP_LOCAL_PORT},
        {.port_number = SERVER_LOCAL_PORT}
    };
#endif

    // Verify that the port count defined is sdk_config.h is matching the one configured for coap_init.
    APP_ERROR_CHECK_BOOL(((sizeof(local_port_list)) / (sizeof(coap_port_t))) == COAP_PORT_COUNT);

    coap_transport_init_t port_list;
    port_list.p_port_table = &local_port_list[0];

    err_code = coap_init(17, &port_list);
    APP_ERROR_CHECK(err_code);

    lwm2m_setup();

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
            // Sleep waiting for an application event.
            err_code = sd_app_evt_wait();
            APP_ERROR_CHECK(err_code);
        }
    }
}

/**
 * @}
 */
