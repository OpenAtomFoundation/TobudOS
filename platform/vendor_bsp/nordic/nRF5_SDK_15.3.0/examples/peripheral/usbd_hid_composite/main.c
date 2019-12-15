/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_drv_power.h"

#include "app_timer.h"
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_hid_mouse.h"
#include "app_usbd_hid_kbd.h"
#include "app_usbd_dummy.h"
#include "app_error.h"
#include "bsp.h"

#include "bsp_cli.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/**
 * @brief CLI interface over UART
 */
NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 64, 16);
NRF_CLI_DEF(m_cli_uart,
            "uart_cli:~$ ",
            &m_cli_uart_transport.transport,
            '\r',
            4);

/**
 * @brief Enable USB power detection
 */
#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION true
#endif

/**
 * @brief Enable HID mouse class
 */
#define CONFIG_HAS_MOUSE    1

/**
 * @brief Enable HID keyboard class
 */
#define CONFIG_HAS_KBD      1

/**
 * @brief Mouse button count
 */
#define CONFIG_MOUSE_BUTTON_COUNT 2

/**
 * @brief Mouse speed (value sent via HID when board button is pressed).
 */
#define CONFIG_MOUSE_MOVE_STEP (3)

/**
 * @brief Mouse move repeat time in milliseconds
 */
#define CONFIG_MOUSE_MOVE_TIME_MS (5)

/**
 * @brief Letter to be sent on LETTER button
 *
 * @sa BTN_KBD_LETTER
 */
#define CONFIG_KBD_LETTER APP_USBD_HID_KBD_G

/**
 * @brief Propagate SET_PROTOCOL command to other HID instance
 */
#define PROPAGATE_PROTOCOL  0


#define LED_CAPSLOCK       (BSP_BOARD_LED_0) /**< CAPSLOCK */
#define LED_NUMLOCK        (BSP_BOARD_LED_1) /**< NUMLOCK */
#define LED_HID_REP        (BSP_BOARD_LED_2) /**< Changes its state if any HID report was received or transmitted */
#define LED_USB_START      (BSP_BOARD_LED_3) /**< The USBD library has been started and the bus is not in SUSPEND state */

#define BTN_MOUSE_X_POS    0
#define BTN_MOUSE_LEFT     1
#define BTN_KBD_SHIFT      2
#define BTN_KBD_LETTER     3

/**
 * @brief Additional key release events
 *
 * This example needs to process release events of used buttons
 */
enum {
    BSP_USER_EVENT_RELEASE_0 = BSP_EVENT_KEY_LAST + 1, /**< Button 0 released */
    BSP_USER_EVENT_RELEASE_1,                          /**< Button 1 released */
    BSP_USER_EVENT_RELEASE_2,                          /**< Button 2 released */
    BSP_USER_EVENT_RELEASE_3,                          /**< Button 3 released */
    BSP_USER_EVENT_RELEASE_4,                          /**< Button 4 released */
    BSP_USER_EVENT_RELEASE_5,                          /**< Button 5 released */
    BSP_USER_EVENT_RELEASE_6,                          /**< Button 6 released */
    BSP_USER_EVENT_RELEASE_7,                          /**< Button 7 released */
};

/**
 * @brief USB composite interfaces
 */
#define APP_USBD_INTERFACE_MOUSE 0
#define APP_USBD_INTERFACE_KBD   1

/**
 * @brief User event handler, HID mouse
 */
static void hid_mouse_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                      app_usbd_hid_user_event_t event);

/**
 * @brief User event handler, HID keyboard
 */
static void hid_kbd_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_hid_user_event_t event);

/*lint -save -e26 -e64 -e123 -e505 -e651*/

/**
 * @brief Global HID mouse instance
 */
APP_USBD_HID_MOUSE_GLOBAL_DEF(m_app_hid_mouse,
                              APP_USBD_INTERFACE_MOUSE,
                              NRF_DRV_USBD_EPIN1,
                              CONFIG_MOUSE_BUTTON_COUNT,
                              hid_mouse_user_ev_handler,
                              APP_USBD_HID_SUBCLASS_BOOT
);

APP_USBD_DUMMY_GLOBAL_DEF(m_app_mouse_dummy, APP_USBD_INTERFACE_MOUSE);

/**
 * @brief Global HID keyboard instance
 */
APP_USBD_HID_KBD_GLOBAL_DEF(m_app_hid_kbd,
                            APP_USBD_INTERFACE_KBD,
                            NRF_DRV_USBD_EPIN2,
                            hid_kbd_user_ev_handler,
                            APP_USBD_HID_SUBCLASS_BOOT
);
APP_USBD_DUMMY_GLOBAL_DEF(m_app_kbd_dummy, APP_USBD_INTERFACE_KBD);

/*lint -restore*/

/**
 * @brief Timer to repeat mouse move
 */
APP_TIMER_DEF(m_mouse_move_timer);


static void kbd_status(void)
{
    if(app_usbd_hid_kbd_led_state_get(&m_app_hid_kbd, APP_USBD_HID_KBD_LED_NUM_LOCK))
    {
        bsp_board_led_on(LED_NUMLOCK);
    }
    else
    {
        bsp_board_led_off(LED_NUMLOCK);
    }

    if(app_usbd_hid_kbd_led_state_get(&m_app_hid_kbd, APP_USBD_HID_KBD_LED_CAPS_LOCK))
    {
        bsp_board_led_on(LED_CAPSLOCK);
    }
    else
    {
        bsp_board_led_off(LED_CAPSLOCK);
    }
}

/**
 * @brief Class specific event handler.
 *
 * @param p_inst    Class instance.
 * @param event     Class specific event.
 * */
static void hid_mouse_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                      app_usbd_hid_user_event_t event)
{
    UNUSED_PARAMETER(p_inst);
    switch (event) {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
            /* No output report defined for HID mouse.*/
            ASSERT(0);
            break;
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
            bsp_board_led_invert(LED_HID_REP);
            break;
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
            UNUSED_RETURN_VALUE(hid_mouse_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_kbd_on_set_protocol(&m_app_hid_kbd, APP_USBD_HID_USER_EVT_SET_BOOT_PROTO);
#endif
            break;
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
            UNUSED_RETURN_VALUE(hid_mouse_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_kbd_on_set_protocol(&m_app_hid_kbd, APP_USBD_HID_USER_EVT_SET_REPORT_PROTO);
#endif
            break;
        default:
            break;
    }
}

/**
 * @brief Class specific event handler.
 *
 * @param p_inst    Class instance.
 * @param event     Class specific event.
 * */
static void hid_kbd_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_hid_user_event_t event)
{
    UNUSED_PARAMETER(p_inst);
    switch (event) {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
            /* Only one output report IS defined for HID keyboard class. Update LEDs state. */
            bsp_board_led_invert(LED_HID_REP);
            kbd_status();
            break;
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
            bsp_board_led_invert(LED_HID_REP);
            break;
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
            UNUSED_RETURN_VALUE(hid_kbd_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_mouse_on_set_protocol(&m_app_hid_mouse, APP_USBD_HID_USER_EVT_SET_BOOT_PROTO);
#endif
            break;
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
            UNUSED_RETURN_VALUE(hid_kbd_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_mouse_on_set_protocol(&m_app_hid_mouse, APP_USBD_HID_USER_EVT_SET_REPORT_PROTO);
#endif
            break;
        default:
            break;
    }
}


/**
 * @brief USBD library specific event handler.
 *
 * @param event     USBD library event.
 * */
static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SOF:
            break;
        case APP_USBD_EVT_DRV_SUSPEND:
            app_usbd_suspend_req(); // Allow the library to put the peripheral into sleep mode
            bsp_board_leds_off();
            break;
        case APP_USBD_EVT_DRV_RESUME:
            bsp_board_led_on(LED_USB_START);
            kbd_status(); /* Restore LED state - during SUSPEND all LEDS are turned off */
            break;
        case APP_USBD_EVT_STARTED:
            bsp_board_led_on(LED_USB_START);
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            bsp_board_leds_off();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            NRF_LOG_INFO("USB power detected");

            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            NRF_LOG_INFO("USB ready");
            app_usbd_start();
            break;
        default:
            break;
    }
}


static void mouse_move_timer_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    UNUSED_RETURN_VALUE(app_usbd_hid_mouse_x_move(&m_app_hid_mouse, CONFIG_MOUSE_MOVE_STEP));
}

static void bsp_event_callback(bsp_event_t ev)
{
    switch ((unsigned int)ev)
    {
        case CONCAT_2(BSP_EVENT_KEY_, BTN_MOUSE_X_POS):
            UNUSED_RETURN_VALUE(app_usbd_hid_mouse_x_move(&m_app_hid_mouse, CONFIG_MOUSE_MOVE_STEP));
            UNUSED_RETURN_VALUE(app_timer_start(m_mouse_move_timer, APP_TIMER_TICKS(CONFIG_MOUSE_MOVE_TIME_MS), NULL));
            break;
        case CONCAT_2(BSP_USER_EVENT_RELEASE_, BTN_MOUSE_X_POS):
            UNUSED_RETURN_VALUE(app_timer_stop(m_mouse_move_timer));
            break;

        case CONCAT_2(BSP_EVENT_KEY_, BTN_MOUSE_LEFT):
            UNUSED_RETURN_VALUE(app_usbd_hid_mouse_button_state(&m_app_hid_mouse, 0, true));
            break;
        case CONCAT_2(BSP_USER_EVENT_RELEASE_, BTN_MOUSE_LEFT):
            UNUSED_RETURN_VALUE(app_usbd_hid_mouse_button_state(&m_app_hid_mouse, 0, false));
            break;

        case CONCAT_2(BSP_EVENT_KEY_, BTN_KBD_SHIFT):
            UNUSED_RETURN_VALUE(app_usbd_hid_kbd_modifier_state_set(&m_app_hid_kbd, APP_USBD_HID_KBD_MODIFIER_LEFT_SHIFT, true));
            break;
        case CONCAT_2(BSP_USER_EVENT_RELEASE_, BTN_KBD_SHIFT):
            UNUSED_RETURN_VALUE(app_usbd_hid_kbd_modifier_state_set(&m_app_hid_kbd, APP_USBD_HID_KBD_MODIFIER_LEFT_SHIFT, false));
            break;

        case CONCAT_2(BSP_EVENT_KEY_, BTN_KBD_LETTER):
            UNUSED_RETURN_VALUE(app_usbd_hid_kbd_key_control(&m_app_hid_kbd, CONFIG_KBD_LETTER, true));
            break;
        case CONCAT_2(BSP_USER_EVENT_RELEASE_, BTN_KBD_LETTER):
            UNUSED_RETURN_VALUE(app_usbd_hid_kbd_key_control(&m_app_hid_kbd, CONFIG_KBD_LETTER, false));
            break;

        default:
            return; // no implementation needed
    }
}

/**
 * @brief Auxiliary internal macro
 *
 * Macro used only in @ref init_bsp to simplify the configuration
 */
#define INIT_BSP_ASSIGN_RELEASE_ACTION(btn)                      \
    APP_ERROR_CHECK(                                             \
        bsp_event_to_button_action_assign(                       \
            btn,                                                 \
            BSP_BUTTON_ACTION_RELEASE,                           \
            (bsp_event_t)CONCAT_2(BSP_USER_EVENT_RELEASE_, btn)) \
    )

static void init_bsp(void)
{
    ret_code_t ret;
    ret = bsp_init(BSP_INIT_BUTTONS, bsp_event_callback);
    APP_ERROR_CHECK(ret);

    INIT_BSP_ASSIGN_RELEASE_ACTION(BTN_MOUSE_X_POS);
    INIT_BSP_ASSIGN_RELEASE_ACTION(BTN_MOUSE_LEFT );
    INIT_BSP_ASSIGN_RELEASE_ACTION(BTN_KBD_SHIFT  );
    INIT_BSP_ASSIGN_RELEASE_ACTION(BTN_KBD_LETTER );

    /* Configure LEDs */
    bsp_board_init(BSP_INIT_LEDS);
}

static void init_cli(void)
{
    ret_code_t ret;
    ret = bsp_cli_init(bsp_event_callback);
    APP_ERROR_CHECK(ret);
    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    ret = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);
    ret = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(ret);
}

int main(void)
{
    ret_code_t ret;
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler,
    };

    ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);

    nrf_drv_clock_lfclk_request(NULL);
    while(!nrf_drv_clock_lfclk_is_running())
    {
        /* Just waiting */
    }

    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    ret = app_timer_create(&m_mouse_move_timer, APP_TIMER_MODE_REPEATED, mouse_move_timer_handler);
    APP_ERROR_CHECK(ret);

    init_bsp();
    init_cli();

    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_inst_mouse;
#if CONFIG_HAS_MOUSE
    class_inst_mouse = app_usbd_hid_mouse_class_inst_get(&m_app_hid_mouse);
#else
    class_inst_mouse = app_usbd_dummy_class_inst_get(&m_app_mouse_dummy);
#endif
    ret = app_usbd_class_append(class_inst_mouse);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_inst_kbd;
#if CONFIG_HAS_KBD
    class_inst_kbd = app_usbd_hid_kbd_class_inst_get(&m_app_hid_kbd);
#else
    class_inst_kbd = app_usbd_dummy_class_inst_get(&m_app_kbd_dummy);
#endif
    ret = app_usbd_class_append(class_inst_kbd);
    APP_ERROR_CHECK(ret);

    NRF_LOG_INFO("USBD HID composite example started.");
    
    if (USBD_POWER_DETECTION)
    {
        ret = app_usbd_power_events_enable();
        APP_ERROR_CHECK(ret);
    }
    else
    {
        NRF_LOG_INFO("No USB power detection enabled\r\nStarting USB now");

        app_usbd_enable();
        app_usbd_start();
    }

    while (true)
    {
        while (app_usbd_event_queue_process())
        {
            /* Nothing to do */
        }
        nrf_cli_process(&m_cli_uart);

        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        /* Sleep CPU only if there was no interrupt since last loop processing */
        __WFE();
    }
}
