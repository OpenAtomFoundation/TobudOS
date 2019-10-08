/**
 * Copyright (c) 2016 - 2019 Nordic Semiconductor ASA and Luxoft Global Operations Gmbh.
 *
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
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
#include "config.h"
#include "ral_irq_handlers.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_clock.h"

#include "fsm.h"
#include "uart.h"
#include "mac_mcps_data.h"
#include "sys_init.h"
#include "sys_task_scheduler.h"
#include "boards.h"

static uint8_t __ALIGN(ALIGN_VALUE) m_heap[CONFIG_POOL_SIZE];
static const size_t m_heap_size = CONFIG_POOL_SIZE;

static void out_of_memory_callback(const void * data);
static void memory_freed_callback(const void * data);

static sys_event_desc_t m_out_of_memory_desc =
{
    .event_id = SYS_EVENT_OUT_OF_MEMORY,
    .callback = out_of_memory_callback,
};

static sys_event_desc_t m_memory_freed_desc =
{
    .event_id = SYS_EVENT_MEMORY_FREED,
    .callback = memory_freed_callback,
};

void mcps_data_ind(mcps_data_ind_t * p_ind)
{
    static uint8_t    app_rx_counter = 0;
    static bool       first_frame = true;
    sequence_number_t next_counter_val = app_rx_counter + 1;
    sequence_number_t rx_counter = 0;

    bool addresses_match = p_ind->dst_pan_id == CONFIG_PAN_ID &&
                           p_ind->dst_addr_mode == MAC_ADDR_SHORT &&
                           p_ind->dst_addr.short_address == CONFIG_DEVICE_SHORT_ADDRESS &&
                           p_ind->src_pan_id == CONFIG_PAN_ID &&
                           p_ind->src_addr_mode == MAC_ADDR_SHORT &&
                           p_ind->src_addr.short_address == CONFIG_OTHER_ADDRESS;

    rx_counter = p_ind->msdu.p_payload[0];
    if ((rx_counter != app_rx_counter || first_frame) && addresses_match && (p_ind->msdu_length > 0))
    {
        if (first_frame == false)
        {
            ASSERT_INFO(next_counter_val == rx_counter,"Number of lost frames: %d", rx_counter - next_counter_val);
            (void)next_counter_val;
        }
        first_frame = false;

        app_rx_counter = rx_counter;

        fsm_event_data_t data =
        {
            .mcps_data_ind = p_ind,
        };
        fsm_event_post(E_RADIO_RX_DONE, &data);
    }
    else
    {
        mac_mem_msdu_free(&p_ind->msdu);
    }
}

static void out_of_memory_callback(const void * data)
{
    LEDS_ON(BIT(CONFIG_ERROR_PIN));
}

static void memory_freed_callback(const void * data)
{
    LEDS_OFF(BIT(CONFIG_ERROR_PIN));
}

/**@brief   Application task initialization for GPIO test.
 */
static void app_task_init(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);

    sys_init(m_heap, m_heap_size);

    sys_event_subscribe(&m_out_of_memory_desc);
    sys_event_subscribe(&m_memory_freed_desc);

    uart_init();
    fsm_init();
    fsm_event_post(E_START_CONFIG, NULL);
}

/**@brief   Application task handler for GPIO test.
 */
void app_task_handler(void)
{
    fsm_event_scheduler_run();
}

static void clock_init(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    ASSERT((err_code == NRF_SUCCESS) || (err_code == NRF_ERROR_MODULE_ALREADY_INITIALIZED));

    nrf_drv_clock_hfclk_request(NULL);
    while (!nrf_drv_clock_hfclk_is_running())
    {
        // spin lock
    }

    nrf_drv_clock_lfclk_request(NULL);
    while (!nrf_drv_clock_lfclk_is_running())
    {
        // spin lock
    }
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    ral_irq_handler_import();
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("Wireless UART example started.");

    app_task_init();
    sys_task_post(APP_TASK_ID);

    clock_init();

    while (true)
    {
        sys_task_run();
    }
}

