/**
 * Copyright (c) 2013 - 2019, Nordic Semiconductor ASA
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
 * @defgroup iot_sdk_tcp_server main.c
 * @{
 * @ingroup iot_sdk_app_lwip
 * @brief This file contains the source code for LwIP TCP Server sample application.
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
#include "lwip/init.h"
#include "lwip/inet6.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
/*lint -save -e607 */
#include "lwip/tcp.h"
/*lint -restore */
#include "lwip/timers.h"
#include "nrf_platform_port.h"
#include "app_util_platform.h"
#include "iot_timer.h"
#include "ipv6_medium.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define SCHED_QUEUE_SIZE                    16                                                      /**< Maximum number of events in the scheduler queue. */
#define SCHED_MAX_EVENT_DATA_SIZE           192                                                     /**< Maximum size of scheduler events. */

#define LED_ONE                             BSP_LED_0_MASK                                          /**< Is on when device is advertising. */
#define LED_TWO                             BSP_LED_1_MASK                                          /**< Is on when device is connected. */
#define LED_THREE                           BSP_LED_2_MASK
#define LED_FOUR                            BSP_LED_3_MASK
#define TCP_CONNECTED_LED                   BSP_LED_2_MASK                                          /**< Is on when a TCP connection is established. */
#define DISPLAY_LED_0                       BSP_LED_0_MASK                                          /**< LED used for displaying mod 4 of data payload received on UDP port. */
#define DISPLAY_LED_1                       BSP_LED_1_MASK                                          /**< LED used for displaying mod 4 of data payload received on UDP port. */
#define DISPLAY_LED_2                       BSP_LED_2_MASK                                          /**< LED used for displaying mod 4 of data payload received on UDP port. */
#define DISPLAY_LED_3                       BSP_LED_3_MASK                                          /**< LED used for displaying mod 4 of data payload received on UDP port. */
#define ALL_APP_LED                        (BSP_LED_0_MASK | BSP_LED_1_MASK | \
                                            BSP_LED_2_MASK | BSP_LED_3_MASK)                        /**< Define used for simultaneous operation of all application LEDs. */

#ifdef COMMISSIONING_ENABLED
#define ERASE_BUTTON_PIN_NO                 BSP_BUTTON_3                                            /**< Button used to erase commissioning settings. */
#endif // COMMISSIONING_ENABLED

#define LWIP_SYS_TICK_MS                    100                                                     /**< Interval for timer used as trigger to send. */
#define LED_BLINK_INTERVAL_MS               300                                                     /**< LED blinking interval. */

#define DEAD_BEEF                           0xDEADBEEF                                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define MAX_LENGTH_FILENAME                 128                                                     /**< Max length of filename to copy for the debug error handler. */

#define TCP_SERVER_PORT                     9000                                                    /**< TCP server listen port number. */
#define TCP_DATA_SIZE                       8                                                       /**< UDP Data size sent on remote. */

#define APP_ENABLE_LOGS                     1                                                       /**< Enable logs in the application. */

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
    TCP_STATE_IDLE,
    TCP_STATE_REQUEST_CONNECTION,
    TCP_STATE_CONNECTED,
    TCP_STATE_DATA_TX_IN_PROGRESS,
    TCP_STATE_DISCONNECTED
}tcp_state_t;

APP_TIMER_DEF(m_iot_timer_tick_src_id);                                                             /**< System Timer used to service CoAP and LWIP periodically. */
eui64_t                                     eui64_local_iid;                                        /**< Local EUI64 value that is used as the IID for*/
static ipv6_medium_instance_t               m_ipv6_medium;
static struct tcp_pcb                     * mp_tcp_port;                                            /**< TCP Port to listen on. */
static tcp_state_t                          m_tcp_state;                                            /**< TCP State information. */

#ifdef COMMISSIONING_ENABLED
static bool                                 m_power_off_on_failure = false;
static bool                                 m_identity_mode_active;
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
    // Configure application LED pins.
    LEDS_CONFIGURE(ALL_APP_LED);

    // Turn off all LED on initialization.
    LEDS_OFF(ALL_APP_LED);
}


#ifdef COMMISSIONING_ENABLED
/**@brief Timer callback used for controlling board LEDs to represent application state.
 *
 */
static void blink_timeout_handler(iot_timer_time_in_ms_t wall_clock_value)
{
    UNUSED_PARAMETER(wall_clock_value);
    static bool id_mode_previously_enabled;

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
}
#endif // COMMISSIONING_ENABLED


/**@brief Function to TCP port.
 *
 * @details Function to close the TCP port and reset any information on the port.
 */
static void tcp_port_close(struct tcp_pcb * p_pcb)
{
    m_tcp_state = TCP_STATE_REQUEST_CONNECTION;

    //Reset all information set on and/or callback registered for the  port.
    tcp_arg(p_pcb, NULL);
    tcp_sent(p_pcb, NULL);
    tcp_recv(p_pcb, NULL);
    tcp_err(p_pcb, NULL);
    tcp_poll(p_pcb, NULL, 0);

    UNUSED_VARIABLE(tcp_close(p_pcb));

    LEDS_OFF((DISPLAY_LED_0 | DISPLAY_LED_1 | DISPLAY_LED_2 | DISPLAY_LED_3));
    LEDS_ON(LED_TWO);
}


/**@brief TCP Port Write complete callback.
 *
 * @details Callback registered to be notified of the write complete event on the TCP port.
 *          In case write complete is notified with 'zero' length, port is closed.
 *
 * @param[in]   p_arg     Receive argument set on the port.
 * @param[in]   p_pcb     PCB identifier of the port.
 * @param[in]   len       Length of data written successfully.
 *
 * @retval ERR_OK.
 */
static err_t tcp_write_complete(void           * p_arg,
                                struct tcp_pcb * p_pcb,
                                u16_t            len)
{
    UNUSED_PARAMETER(p_arg);
    UNUSED_PARAMETER(p_pcb);

    if (len != 0)
    {
        //Write complete, reset the state to connected from transmit pending.
        m_tcp_state = TCP_STATE_CONNECTED;
    }
    else
    {
        //Something is not right on the port, close the port.
        tcp_port_close(mp_tcp_port);
    }
    return ERR_OK;
}


/**@brief Send test data on the port.
 *
  * @details Sends TCP data in Request of size 8 in format described in description above.
 *
 * @param[in]   p_pcb     PCB identifier of the port.
 */
static void tcp_send_data(struct tcp_pcb * p_pcb, uint32_t sequence_number)
{
    err_t  err = ERR_OK;

    if (m_tcp_state != TCP_STATE_DATA_TX_IN_PROGRESS)
    {
        //Register callback to get notification of data reception is complete.
        tcp_sent(p_pcb, tcp_write_complete);
        uint8_t tcp_data[TCP_DATA_SIZE];

        tcp_data[0] = (uint8_t )((sequence_number >> 24) & 0x000000FF);
        tcp_data[1] = (uint8_t )((sequence_number >> 16) & 0x000000FF);
        tcp_data[2] = (uint8_t )((sequence_number >> 8)  & 0x000000FF);
        tcp_data[3] = (uint8_t )(sequence_number         & 0x000000FF);

        tcp_data[4] = 'P';
        tcp_data[5] = 'o';
        tcp_data[6] = 'n';
        tcp_data[7] = 'g';

        //Enqueue data for transmission.
        err = tcp_write(p_pcb, tcp_data, TCP_DATA_SIZE, 1);

        if (err != ERR_OK)
        {
            APPL_LOG ("Failed to send TCP packet, reason %d", err);
        }
        else
        {
            m_tcp_state = TCP_STATE_DATA_TX_IN_PROGRESS;
        }
    }
    else
    {
        //Wait for TX to be complete.
    }
}


/**@brief Callback registered for receiving data on the TCP Port.
 *
 * @param[in]   p_arg     Receive argument set on the TCP port.
 * @param[in]   p_pcb     TCP PCB on which data is received.
 * @param[in]   p_buffer  Buffer with received data.
 * @param[in]   err       Event result indicating error associated with the receive,
 *                        if any, else ERR_OK.
 */
err_t tcp_recv_data_handler(void           * p_arg,
                            struct tcp_pcb * p_pcb,
                            struct pbuf    * p_buffer,
                            err_t            err)
{
    APPL_LOG (">> TCP Data.");

    //Check event result before proceeding.
    if (err == ERR_OK)
    {
        uint8_t *p_data = p_buffer->payload;

        if (p_buffer->len == TCP_DATA_SIZE)
        {
            uint32_t sequence_number = 0;

            sequence_number  = ((p_data[0] << 24) & 0xFF000000);
            sequence_number |= ((p_data[1] << 16) & 0x00FF0000);
            sequence_number |= ((p_data[2] << 8)  & 0x0000FF00);
            sequence_number |= (p_data[3]         & 0x000000FF);

            LEDS_OFF(ALL_APP_LED);

            if (sequence_number & 0x00000001)
            {
                LEDS_ON(DISPLAY_LED_0);
            }
            if (sequence_number & 0x00000002)
            {
                LEDS_ON(DISPLAY_LED_1);
            }
            if (sequence_number & 0x00000004)
            {
                LEDS_ON(DISPLAY_LED_2);
            }
            if (sequence_number & 0x00000008)
            {
                LEDS_ON(DISPLAY_LED_3);
            }

            //Send Response
            tcp_send_data(p_pcb, sequence_number);
        }
        else
        {
            APPL_LOG ("TCP data received in incorrect format.");
        }
        // All is good with the data received, process it.
        tcp_recved(p_pcb, p_buffer->tot_len);
        UNUSED_VARIABLE(pbuf_free(p_buffer));
    }
    else
    {
        //Free the buffer in case its not NULL.
        if (p_buffer != NULL)
        {
            UNUSED_VARIABLE(pbuf_free(p_buffer));
        }

        //Something is not right with the port, close the port.
        tcp_port_close(mp_tcp_port);
    }

    return ERR_OK;
}


/**@brief Callback registered To be notified of errors on the TCP ports.
 *
 * @param[in]   p_arg     Receive argument set on the port.
 * @param[in]   err       Indication of nature of error that occurred on the port.
 */
static void tcp_error_handler(void *p_arg, err_t err)
{
   LWIP_UNUSED_ARG(p_arg);
   LWIP_UNUSED_ARG(err);

   tcp_port_close(mp_tcp_port);
}


/**@brief TCP Poll Callback.
 *
 * @details Callback registered for TCP port polling, currently does nothing.
 *
 * @param[in]   p_arg     Receive argument set on the port.
 * @param[in]   p_pcb     PCB identifier of the port.
 *
 * @retval ERR_OK.
 */
static err_t tcp_connection_poll(void *p_arg, struct tcp_pcb *p_pcb)
{
    LWIP_UNUSED_ARG(p_arg);
    LWIP_UNUSED_ARG(p_pcb);

    return ERR_OK;
}


/**@brief TCP Accept Callback.
 *
 * @details Callback registered to be notified of client connect request and accept it.
 *
 * @param[in]   p_arg     Receive argument set on the port.
 * @param[in]   p_pcb     PCB identifier of the port.
 * @param[in]   err       Event result notifying if the client connection was processed
 *                        successfully in the stack or not.
 *
 * @retval ERR_OK.
 */
static err_t tcp_port_accept(void           * p_arg,
                             struct tcp_pcb * p_pcb,
                             err_t            err)
{
   LWIP_UNUSED_ARG(p_arg);

   if (err == ERR_OK)
   {
       tcp_setprio(p_pcb, TCP_PRIO_MIN);
       tcp_arg(p_pcb, NULL);
       tcp_recv(p_pcb, tcp_recv_data_handler);
       tcp_err(p_pcb, tcp_error_handler);
       tcp_poll(p_pcb, tcp_connection_poll, 0);

       /*lint -save -e548 */
       tcp_accepted(mp_tcp_port);
       /*lint -restore */

       m_tcp_state = TCP_STATE_CONNECTED;
       LEDS_ON(TCP_CONNECTED_LED);
   }
   else
   {
       ASSERT(0);
   }

   return ERR_OK;
}


/**@brief Setup TCP Port.
 *
 * @details Set up TCP Port and register receive callback.
 */
static void tcp_port_setup(void)
{
    ip6_addr_t any_addr;
    ip6_addr_set_any(&any_addr);

    mp_tcp_port = tcp_new_ip6();

    if (mp_tcp_port != NULL)
    {
        err_t err = tcp_bind(mp_tcp_port, &any_addr, TCP_SERVER_PORT);
        APP_ERROR_CHECK(err);
        mp_tcp_port = tcp_listen_with_backlog(mp_tcp_port, 1);
        if (mp_tcp_port != NULL)
        {
            tcp_accept(mp_tcp_port, tcp_port_accept);
        }
    }
    else
    {
        ASSERT(0);
    }
}


/**@brief Function for initializing IP stack.
 *
 * @details Initialize the IP Stack and its driver.
 */
static void ip_stack_init(void)
{
    uint32_t err_code;

    err_code = ipv6_medium_eui64_get(m_ipv6_medium.ipv6_medium_instance_id,
                                     &eui64_local_iid);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_mem_init();
    APP_ERROR_CHECK(err_code);

    //Initialize LwIP stack.
    lwip_init();

    //Initialize LwIP stack driver.
    err_code = nrf_driver_init();
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


/**@brief Timer callback used for periodic servicing of LwIP protocol timers.
 *
 * @details Timer callback used for periodic servicing of LwIP protocol timers.
 *
 * @param[in]   p_context   Pointer used for passing context. No context used in this application.
 */
static void system_timer_callback(iot_timer_time_in_ms_t wall_clock_value)
{
    UNUSED_VARIABLE(wall_clock_value);
    sys_check_timeouts();
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
        {system_timer_callback,   LWIP_SYS_TICK_MS},
#ifdef COMMISSIONING_ENABLED
        {blink_timeout_handler,   LED_BLINK_INTERVAL_MS},
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


/**@brief Function to handle interface up event. */
void nrf_driver_interface_up(iot_interface_t const * p_interface)
{
    UNUSED_PARAMETER(p_interface);

#ifdef COMMISSIONING_ENABLED
    commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_STOP_RESET);
#endif // COMMISSIONING_ENABLED

    APPL_LOG ("IPv6 interface up.");

    sys_check_timeouts();

    m_tcp_state = TCP_STATE_REQUEST_CONNECTION;

    LEDS_OFF(LED_ONE);
    LEDS_ON(LED_TWO);
}


/**@brief Function to handle interface down event. */
void nrf_driver_interface_down(iot_interface_t const * p_interface)
{
    UNUSED_PARAMETER(p_interface);

#ifdef COMMISSIONING_ENABLED
    commissioning_joining_mode_timer_ctrl(JOINING_MODE_TIMER_START);
#endif // COMMISSIONING_ENABLED

    APPL_LOG ("IPv6 interface down.");

    LEDS_OFF((DISPLAY_LED_0 | DISPLAY_LED_1 | DISPLAY_LED_2 | DISPLAY_LED_3));
    LEDS_ON(LED_ONE);

    m_tcp_state = TCP_STATE_DISCONNECTED;
}


/**@brief Function for starting connectable mode.
 */
static void connectable_mode_enter(void)
{
    uint32_t err_code = ipv6_medium_connectable_mode_enter(m_ipv6_medium.ipv6_medium_instance_id);
    APP_ERROR_CHECK(err_code);

    APPL_LOG("Physical layer in connectable mode.");
    LEDS_OFF(LED_TWO);
    LEDS_ON(LED_ONE);
}


static void on_ipv6_medium_evt(ipv6_medium_evt_t * p_ipv6_medium_evt)
{
    switch (p_ipv6_medium_evt->ipv6_medium_evt_id)
    {
        case IPV6_MEDIUM_EVT_CONN_UP:
        {
            APPL_LOG("Physical layer: connected.");
            LEDS_OFF(LED_ONE);
            LEDS_ON(LED_TWO);
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


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    //Initialize.
    log_init();
    leds_init();
    scheduler_init();
    timers_init();
    iot_timer_init();

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
    tcp_port_setup();

    APPL_LOG("Application started.");

    //Start execution.
    connectable_mode_enter();

    //Enter main loop.
    for (;;)
    {
        //Execute event schedule.
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
