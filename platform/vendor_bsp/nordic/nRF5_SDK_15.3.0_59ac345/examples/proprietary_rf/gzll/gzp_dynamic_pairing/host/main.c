/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
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
/**
 * This project requires a running counterpart project, which is either of the following:
 *
 * 1) nRF24Lxx device running the gzll_device_w_dynamic_pairing example from the
 * compatible version of the nRFgo SDK
 *
 * 2) nRF5x device running the gzp_device_dynamic_pairing_example example.
 *
 * The application listens for packets continuously, monitoring for pairing
 * requests, as well as normal user data.
 *
 * The Gazell pairing library uses pipe 0 and pipe 1 for encrypted communication.
 * The application will grant any request for a host ID, thus granting pairing.
 * Unencrypted packets can be received on pipe 2.
 *
 * When DATA is received, the contents of the first payload byte
 * are output on GPIO Port LEDS.
 *
 */
#include "nrf_gzll.h"
#include "nrf_gzp.h"
#include "nrf_ecb.h"
#include "bsp.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_gzll_error.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/*****************************************************************************/
/** @name Configuration */
/*****************************************************************************/
#define UNENCRYPTED_DATA_PIPE     2   ///< Pipes 0 and 1 are reserved for GZP pairing and data. See nrf_gzp.h.
#define NRF_GZLLDE_RXPERIOD_DIV_2 504 ///< RXPERIOD/2 on LU1 = timeslot period on nRF5x.

/**
 * @brief Initialize the BSP modules.
 */
static void ui_init(void)
{
    uint32_t err_code;

    // Initialize application timer.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = bsp_init(BSP_INIT_LEDS, NULL);
    APP_ERROR_CHECK(err_code);

    // Set up logger
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    bsp_board_init(BSP_INIT_LEDS);
}


/**
 * @brief Function to control LED outputs.
 *
 * @param[in] val Desirable state of the LEDs.
 */
static void output_present(uint8_t val)
{
    uint32_t i;

    for (i = 0; i < LEDS_NUMBER; i++)
    {
        if (val & (1 << i))
        {
            bsp_board_led_on(i);
        }
        else
        {
            bsp_board_led_off(i);
        }
    }
}


/*****************************************************************************/
/**
 * @brief Main function.
 *
 * @return ANSI required int return type.
 */
/*****************************************************************************/
int main(void)
{
    // Debug helper variables
    uint32_t length;

    // Data and acknowledgement payloads
    uint8_t payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];

    // Set up the user interface (buttons and LEDs)
    ui_init();

    // Initialize the Gazell Link Layer
    bool result_value = nrf_gzll_init(NRF_GZLL_MODE_HOST);
    GAZELLE_ERROR_CODE_CHECK(result_value);

    result_value = nrf_gzll_set_timeslot_period(NRF_GZLLDE_RXPERIOD_DIV_2); // Half RX period on an nRF24Lxx device
    GAZELLE_ERROR_CODE_CHECK(result_value);

    // Initialize the Gazell Pairing Library
    gzp_init();
    result_value = nrf_gzll_set_rx_pipes_enabled(nrf_gzll_get_rx_pipes_enabled() |
                                                 (1 << UNENCRYPTED_DATA_PIPE));
    GAZELLE_ERROR_CODE_CHECK(result_value);

    gzp_pairing_enable(true);

    result_value = nrf_gzll_enable();
    GAZELLE_ERROR_CODE_CHECK(result_value);

    NRF_LOG_INFO("Gazell dynamic pairing example started. Host mode.");
    NRF_LOG_FLUSH();


    for (;;)
    {
        gzp_host_execute();

        // If a Host ID request received
        if (gzp_id_req_received())
        {
            // Always grant a request
            gzp_id_req_grant();
        }

        length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

        if (nrf_gzll_get_rx_fifo_packet_count(UNENCRYPTED_DATA_PIPE))
        {
            if (nrf_gzll_fetch_packet_from_rx_fifo(UNENCRYPTED_DATA_PIPE, payload, &length))
            {
                output_present(payload[0]);
            }
        }
        else if (gzp_crypt_user_data_received())
        {
            if (gzp_crypt_user_data_read(payload, (uint8_t *)&length))
            {
                output_present(payload[0]);
            }
        }
    }
}


