/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf52832_xxaa_service.h"
#include "ble_qiot_llsync_device.h"
#include "ble_qiot_service.h"
#include "ble_qiot_import.h"
#include "ble_qiot_config.h"

#include "flash_storage.h"

#define USER_BUTTON_BUTTON1 \
    BSP_BUTTON_0 /**< Button that will trigger the notification event with the LED Button Service */
#define USER_BUTTON_BUTTON2 \
    BSP_BUTTON_1 /**< Button that will trigger the notification event with the LED Button Service */
#define USER_BUTTON_BUTTON3 \
    BSP_BUTTON_2 /**< Button that will trigger the notification event with the LED Button Service */
#define USER_BUTTON_BUTTON4 \
    BSP_BUTTON_3 /**< Button that will trigger the notification event with the LED Button Service */

#define GAP_DEVICE_NAME "IoT"

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(100, UNIT_1_25_MS) /**< Minimum acceptable connection interval (0.5 seconds). \
                                                            */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(200, UNIT_1_25_MS) /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY     0                                /**< Slave latency. */
#define CONN_SUP_TIMEOUT  MSEC_TO_UNITS(4000, UNIT_10_MS)  /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY                                                                      \
    APP_TIMER_TICKS(20000) /**< Time from initiating event (connect or start of notification) to first time \
                              sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY \
    APP_TIMER_TICKS(                  \
        5000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3 /**< Number of attempts before giving up the connection parameter negotiation. \
                                        */

#define BUTTON_DETECTION_DELAY \
    APP_TIMER_TICKS(           \
        50) /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define DEAD_BEEF \
    0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

NRF_BLE_QWR_DEF(m_qwr); /**< Context for the Queued Write module.*/

/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void)
{
    bsp_board_init(BSP_INIT_LEDS);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)GAP_DEVICE_NAME, strlen(GAP_DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    nrf_ble_gatt_t *p_gatt;

    p_gatt              = ble_get_gatt_instance();
    ret_code_t err_code = nrf_ble_gatt_init(p_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    ret_code_t err_code;
    // ble_lbs_init_t     init     = {0};
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module that
 *          are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply
 *       setting the disconnect_on_fail config parameter, but instead we use the event
 *       handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
    ret_code_t      err_code;
    ble_priv_cfg_s *p_cfg;

    p_cfg = ble_get_priv_cfg();
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        err_code = sd_ble_gap_disconnect(p_cfg->conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code           = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);
}

void ble_iot_button1_change(uint8_t button_action)
{
    static uint8_t last_button_action = 0;

    if (last_button_action == 1 && button_action == 0) {
        ble_qiot_advertising_start();
    }
    last_button_action = button_action;
}

void ble_iot_button2_change(uint8_t button_action)
{
    static uint8_t last_button_action = 0;
    static int     button2_index      = 0;

    if (last_button_action == 1 && button_action == 0) {
        NRF_LOG_INFO("event id %d", button2_index % 3);
        ble_event_post(button2_index % 3);
        button2_index++;
    }
    last_button_action = button_action;
}

void ble_iot_button3_change(uint8_t button_action)
{
    static uint8_t last_button_action = 0;

    if (last_button_action == 1 && button_action == 0) {
        NRF_LOG_INFO("erase flash at addr 0x%X", BLE_QIOT_RECORD_FLASH_ADDR);
        fstorage_erase(BLE_QIOT_RECORD_FLASH_ADDR);
    }
    last_button_action = button_action;
}

void ble_iot_button4_change(uint8_t button_action)
{
    static uint8_t last_button_action = 0;
    static int     button4_index      = 1;

    if (last_button_action == 1 && button_action == 0) {
        if (button4_index % 2 == 0) {
            NRF_LOG_INFO("get status");
            ble_event_get_status();
        } else {
            NRF_LOG_INFO("report property");
            ble_event_report_property();
        }
        button4_index++;
    }
    last_button_action = button_action;
}

/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    // ret_code_t err_code;

    NRF_LOG_INFO("button --> %d %d", pin_no, button_action);

    switch (pin_no) {
        case USER_BUTTON_BUTTON1:
            ble_iot_button1_change(button_action);
            break;

        case USER_BUTTON_BUTTON2:
            ble_iot_button2_change(button_action);
            break;

        case USER_BUTTON_BUTTON3:
            ble_iot_button3_change(button_action);
            break;

        case USER_BUTTON_BUTTON4:
            ble_iot_button4_change(button_action);
            break;

        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
}

/**@brief Function for initializing the button handler module.
 */
static void buttons_init(void)
{
    ret_code_t err_code;

    // The array must be static because a pointer to it will be saved in the button handler module.
    static app_button_cfg_t buttons[] = {
        {USER_BUTTON_BUTTON1, false, BUTTON_PULL, button_event_handler},
        {USER_BUTTON_BUTTON2, false, BUTTON_PULL, button_event_handler},
        {USER_BUTTON_BUTTON3, false, BUTTON_PULL, button_event_handler},
        {USER_BUTTON_BUTTON4, false, BUTTON_PULL, button_event_handler},
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false) {
        nrf_pwr_mgmt_run();
    }
}

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
    leds_init();
    timers_init();
    buttons_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();

    fstorage_init();
    ble_qiot_explorer_init();
#if (1 == BLE_QIOT_BUTTON_BROADCAST)
    if (E_LLSYNC_BIND_SUCC == llsync_bind_state_get())
#endif
    {
        ble_qiot_advertising_start();
    }
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("Blinky example started.");

    // Enter main loop.
    for (;;) {
        idle_state_handle();
    }
}
