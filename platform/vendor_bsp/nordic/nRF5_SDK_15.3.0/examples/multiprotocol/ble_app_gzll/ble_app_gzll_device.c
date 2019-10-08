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
/** @file
 *
 * @{
 * @ingroup ble_app_gzll_gazell_part
 */

#include "ble_app_gzll_device.h"
#include <stdint.h>
#include "nordic_common.h"
#include "app_error.h"
#include "nrf_gzll.h"
#include "nrf_gzll_error.h"
#include "ble_app_gzll_ui.h"
#include "bsp.h"
#include "bsp_config.h"

#define MAX_TX_ATTEMPTS         100  /**< Maximum number of Transmit attempts.*/
#define DUMMY_PACKET            0x80 /**< First payload.*/
#define NB_TX_WITH_SAME_PAYLOAD 3    /**< Number of times each packet is sent (this avoid changing the payload too quickly and allows to have a visible LED pattern on the receiver side).*/
#define PAYLOAD_SIZE            8    /**< Size of the payload to send over Gazell.*/
#define PIPE_TO_HOST            0    /**< Pipe number. */


static uint8_t m_gzll_packet[PAYLOAD_SIZE];


/**@brief Function for starting Gazell functionality.
 */
void gzll_app_start(void)
{
    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_init(NRF_GZLL_MODE_DEVICE));
    nrf_gzll_set_max_tx_attempts(MAX_TX_ATTEMPTS);
    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_enable());

    // Add a packet to the TX FIFO to start the data transfer.
    // Next packets to send will be added.
    m_gzll_packet[0] = DUMMY_PACKET;
    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(PIPE_TO_HOST,
                                                            m_gzll_packet,
                                                            PAYLOAD_SIZE));
}


void gzll_app_stop()
{
    // Disable gazell.
    nrf_gzll_disable();

    // Wait for Gazell to shut down.
    while (nrf_gzll_is_enabled())
    {

    }

    // Clean up after Gazell.
    NVIC_DisableIRQ(RADIO_IRQn);
    NVIC_DisableIRQ(TIMER2_IRQn);
    NVIC_DisableIRQ(SWI0_IRQn);
    NVIC_ClearPendingIRQ(RADIO_IRQn);
    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_ClearPendingIRQ(SWI0_IRQn);

}


/**@brief Callback function for Gazell Transmit Success. Adds new packet to tx fifo.
 */
void nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    static int cpt = 0;
    uint8_t    dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t   dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    // If an ACK was received, we send another packet.
    bsp_board_led_invert(BSP_LED_INDICATE_SENT_OK);

    if (tx_info.payload_received_in_ack)
    {
        // if ack was sent with payload, pop them from rx fifo.
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length));
    }

    cpt++;

    // Update transmitted data.
    if (cpt > NB_TX_WITH_SAME_PAYLOAD)
    {
        cpt = 0;

        m_gzll_packet[0] = ~(m_gzll_packet[0]);
        if (m_gzll_packet[0] == DUMMY_PACKET)
        {
            m_gzll_packet[0] = 0x00;
        }

        m_gzll_packet[0] <<= 1;
        if (m_gzll_packet[0] == 0)
        {
            m_gzll_packet[0]++;
        }

        m_gzll_packet[0] = ~(m_gzll_packet[0]);
    }

    //  Add next packet to tx fifo.
    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(pipe, m_gzll_packet, PAYLOAD_SIZE));

}


/**@brief Callback function for Gazell Transmit fail. Resends the current packet.
 */
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    uint8_t  dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;
    uint32_t err_code;

    // If the transmission failed, send a new packet.
    err_code = bsp_indication_set(BSP_INDICATE_SEND_ERROR);
    // Ignore app_timer queue overflow.
    if (err_code != NRF_ERROR_NO_MEM)
    {
        APP_ERROR_CHECK(err_code);
    }

    if (tx_info.payload_received_in_ack)
    {
        // if ack was sent with payload, pop them from rx fifo.
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length));
    }

    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(pipe, m_gzll_packet, PAYLOAD_SIZE));
}


/**@brief Callback function for Gazell Receive Data Ready. Flushes the receive's FIFO.
 */
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
    // We dont expect to receive any data in return, but if it happens we flush the RX fifo.
    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_flush_rx_fifo(pipe));
}


/**@brief Callback function for Gazell Disabled - Not needed in this example.
 */
void nrf_gzll_disabled()
{
}


/**
 * @}
 */
