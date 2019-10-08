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

/**@file
 *
 * @defgroup ant_sdk_app_connectivity_main main.c
 * @{
 * @ingroup ant_sdk_app_connectivity
 *
 * @brief BLE Connectivity application.
 */

#include <stdbool.h>
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "ser_hal_transport.h"
#include "ser_conn_handlers.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define NRF_LOG_MODULE_NAME CONN
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#include "ser_phy_debug_comm.h"

/**@brief Main function of the connectivity application. */
int main(void)
{
    uint32_t err_code = NRF_SUCCESS;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("ANT connectivity started");

#if ( defined(SER_PHY_HCI_DEBUG_ENABLE) || defined(SER_PHY_DEBUG_APP_ENABLE))
    debug_init(NULL);
#endif

    /* Force constant latency mode to control SPI slave timing */
    NRF_POWER->TASKS_CONSTLAT = 1;

    /* Initialize scheduler queue. */
    APP_SCHED_INIT(SER_CONN_SCHED_MAX_EVENT_DATA_SIZE, SER_CONN_SCHED_QUEUE_SIZE);
    /* Initialize SoftDevice.
     * SoftDevice Event IRQ is not scheduled but immediately copies ANT events to the application
     * scheduler queue */

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    /* Open serialization HAL Transport layer and subscribe for HAL Transport events. */
    err_code = ser_hal_transport_open(ser_conn_hal_transport_event_handle);
    APP_ERROR_CHECK(err_code);

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

        nrf_gpio_cfg_output(18);
        nrf_gpio_pin_set(18);

        (void)NRF_LOG_PROCESS();

        /* Sleep waiting for an application event. */
        err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}
/** @} */
