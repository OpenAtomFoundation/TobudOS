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

/**@file
 *
 * @defgroup ble_sdk_app_connectivity_main main.c
 * @{
 * @ingroup ble_sdk_app_connectivity
 *
 * @brief BLE Connectivity application.
 */

#include <stdbool.h>
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "nrf_sdh.h"
#include "ser_hal_transport.h"
#include "ser_conn_handlers.h"
#include "boards.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_timer.h"
#include "ser_phy_debug_comm.h"

#if defined(APP_USBD_ENABLED) && APP_USBD_ENABLED
#include "app_usbd_serial_num.h"
#ifdef BOARD_PCA10059
#include "nrf_dfu_trigger_usb.h"
#endif
#include "app_usbd.h"

static volatile bool m_usb_started;


static void usbd_user_evt_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SUSPEND:
            break;
        case APP_USBD_EVT_DRV_RESUME:
            break;
        case APP_USBD_EVT_STARTED:
            m_usb_started = true;
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
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

static void usbd_init(void)
{
    app_usbd_serial_num_generate();
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_evt_handler
    };
    APP_ERROR_CHECK(app_usbd_init(&usbd_config));
}

static void usbd_enable(void)
{
#ifdef BOARD_PCA10059
    APP_ERROR_CHECK(nrf_dfu_trigger_usb_init());
#endif
    APP_ERROR_CHECK(app_usbd_power_events_enable()); 

    /* Process USB events until USB is started. This is related to the fact that
     * current version of softdevice does not handle USB POWER events. */
    while (m_usb_started == false)
    {
        while (app_usbd_event_queue_process())
        {
            /* Nothing to do */
        }
    }

}
#endif //APP_USBD_ENABLED

static void on_idle(void)
{

    if (!NRF_LOG_PROCESS())
    {
      // Wait for an event.
      if (nrf_sdh_is_enabled())
      {
          ret_code_t ret_code = sd_app_evt_wait();
          ASSERT((ret_code == NRF_SUCCESS) || (ret_code == NRF_ERROR_SOFTDEVICE_NOT_ENABLED));
          UNUSED_VARIABLE(ret_code);
      }
      else
      {
          // Wait for an event.
          __WFE();
          // Clear the internal event register.
          __SEV();
          __WFE();
      }
    }

#if defined(APP_USBD_ENABLED) && APP_USBD_ENABLED

    while (app_usbd_event_queue_process())
    {
        /* Nothing to do */
    }
#endif
}

/**@brief Main function of the connectivity application. */
int main(void)
{
    uint32_t err_code = NRF_SUCCESS;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("BLE connectivity started");

    bsp_board_init(BSP_INIT_LEDS);

#if (defined(SER_PHY_HCI_DEBUG_ENABLE) || defined(SER_PHY_DEBUG_APP_ENABLE))
    debug_init(NULL);
#endif

    /* Force constant latency mode to control SPI slave timing */
    NRF_POWER->TASKS_CONSTLAT = 1;

    /* Initialize scheduler queue. */
    //lint -save -e666 -e587
    APP_SCHED_INIT(SER_CONN_SCHED_MAX_EVENT_DATA_SIZE, SER_CONN_SCHED_QUEUE_SIZE);
    //lint -restore

    /* Initialize SoftDevice.
     * SoftDevice Event IRQ is not scheduled but immediately copies BLE events to the application
     * scheduler queue */

    err_code = nrf_drv_clock_init();
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED))
    {
        APP_ERROR_CHECK(err_code);
    }

    nrf_drv_clock_hfclk_request(NULL);
    while (!nrf_drv_clock_hfclk_is_running())
    {}

#if defined(APP_USBD_ENABLED) && APP_USBD_ENABLED
    usbd_init();
#endif

    /* Open serialization HAL Transport layer and subscribe for HAL Transport events. */
    err_code = ser_hal_transport_open(ser_conn_hal_transport_event_handle);
    APP_ERROR_CHECK(err_code);

#if defined(APP_USBD_ENABLED) && APP_USBD_ENABLED
    usbd_enable();
#endif

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    ser_conn_on_no_mem_handler_set(on_idle);

    /* Enter main loop. */
    for (;;)
    {
        /* Process SoftDevice events. */
        app_sched_execute();
        if (nrf_sdh_is_suspended())
        {
            // Resume pulling new events if queue utilization drops below 50%.
            if (app_sched_queue_space_get() > (SER_CONN_SCHED_QUEUE_SIZE >> 1))
            {
                nrf_sdh_resume();
            }
        }

        /* Process received packets.
         * We can NOT add received packets as events to the application scheduler queue because
         * received packets have to be processed before SoftDevice events but the scheduler queue
         * does not have priorities. */
        err_code = ser_conn_rx_process();
        APP_ERROR_CHECK(err_code);

        on_idle();
    }
}
/** @} */
