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
/** @file
 *
 * @defgroup nfc_uart_tag_example_main main.c
 * @{
 * @ingroup nfc_uart_tag_example
 * @brief The application main file of NFC UART Tag example.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "app_error.h"
#include "app_fifo.h"
#include "app_uart.h"
#include "boards.h"
#include "nfc_t4t_lib.h"
#include "sdk_config.h"

#include "nrf_log_default_backends.h"
#include "nrf_log_ctrl.h"
#include "nrf_log.h"

#define MAX_APDU_LEN      0xF0   /**< Maximal APDU length, Adafruit limitation. */
#define HEADER_FIELD_SIZE 1      /**< Header field size. */

static app_fifo_t m_nfc_rx_fifo; /**< FIFO instance for data that is received from NFC. */
static app_fifo_t m_nfc_tx_fifo; /**< FIFO instance for data that will be transmitted over NFC. */


/**
 * @brief Callback function for handling UART errors.
 */
void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


/**
 * @brief Function for flushing all FIFOs that are used by the application.
 */
static void fifos_flush(void)
{
    UNUSED_RETURN_VALUE(app_uart_flush());
    UNUSED_RETURN_VALUE(app_fifo_flush(&m_nfc_rx_fifo));
    UNUSED_RETURN_VALUE(app_fifo_flush(&m_nfc_tx_fifo));
}


/**
 * @brief Callback function for handling NFC events.
 */
static void nfc_callback(void          * context,
                         nfc_t4t_event_t event,
                         const uint8_t * data,
                         size_t          dataLength,
                         uint32_t        flags)
{
    ret_code_t err_code;
    uint32_t   resp_len;

    static uint8_t  apdu_buf[APDU_BUFF_SIZE]; // Buffer for APDU data
    static uint32_t apdu_len = 0;             // APDU length

    (void)context;

    switch (event)
    {
        case NFC_T4T_EVENT_FIELD_ON:
            NRF_LOG_INFO("NFC Tag has been selected. UART transmission can start...");
            bsp_board_led_on(BSP_BOARD_LED_0);

            // Flush all FIFOs. Data that was collected from UART channel before selecting
            // the tag is discarded.
            fifos_flush();
            break;

        case NFC_T4T_EVENT_FIELD_OFF:
            NRF_LOG_INFO("NFC field lost. Data from UART will be discarded...");
            bsp_board_leds_off();
            break;

        case NFC_T4T_EVENT_DATA_IND:
            if (apdu_len + dataLength > APDU_BUFF_SIZE)
            {
                APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
            }
            memcpy(apdu_buf + apdu_len, data, dataLength);
            apdu_len += dataLength;

            if (flags != NFC_T4T_DI_FLAG_MORE)
            {
                // Store data in NFC RX FIFO if payload is present.
                if (apdu_len > HEADER_FIELD_SIZE)
                {
                    NRF_LOG_DEBUG("NFC RX data length: %d", apdu_len);
                    uint32_t buff_size;

                    apdu_len -= HEADER_FIELD_SIZE;
                    buff_size = apdu_len;
                    err_code  = app_fifo_write(&m_nfc_rx_fifo,
                                               apdu_buf + HEADER_FIELD_SIZE,
                                               &buff_size);
                    if ((buff_size != apdu_len) || (err_code == NRF_ERROR_NO_MEM))
                    {
                        NRF_LOG_WARNING("NFC RX FIFO buffer overflow");
                    }
                }
                apdu_len = 0;

                // Check if there is any data in NFC TX FIFO that needs to be transmitted.
                resp_len = MIN(APDU_BUFF_SIZE - HEADER_FIELD_SIZE, MAX_APDU_LEN);
                if (app_fifo_read(&m_nfc_tx_fifo, apdu_buf + HEADER_FIELD_SIZE, &resp_len) ==
                    NRF_ERROR_NOT_FOUND)
                {
                    resp_len = 0;
                }

                if (resp_len > 0)
                {
                    NRF_LOG_DEBUG("NFC TX data length: %d", resp_len);
                }

                // Send the response PDU over NFC.
                err_code = nfc_t4t_response_pdu_send(apdu_buf, resp_len + HEADER_FIELD_SIZE);
                APP_ERROR_CHECK(err_code);

                bsp_board_led_off(BSP_BOARD_LED_1);
            }
            else
            {
                bsp_board_led_on(BSP_BOARD_LED_1);
            }
            break;

        default:
            break;
    }
}


/**
 * @brief Function for initializing UART module.
 */
static ret_code_t uart_init(void)
{
    ret_code_t err_code;

    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUFF_SIZE,
                       UART_TX_BUFF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    return err_code;
}


/**
 * @brief Function for initializing FIFO instances that are used to exchange data over NFC.
 */
static ret_code_t nfc_fifos_init(void)
{
    ret_code_t err_code;

    static uint8_t nfc_rx_buff[NFC_RX_BUFF_SIZE]; // Buffer for NFC RX FIFO instance
    static uint8_t nfc_tx_buff[NFC_TX_BUFF_SIZE]; // Buffer for NFC TX FIFO instance

    err_code = app_fifo_init(&m_nfc_rx_fifo, nfc_rx_buff, sizeof(nfc_rx_buff));
    VERIFY_SUCCESS(err_code);

    err_code = app_fifo_init(&m_nfc_tx_fifo, nfc_tx_buff, sizeof(nfc_tx_buff));
    return err_code;
}

/**
 *@brief Function for initializing logging.
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
    ret_code_t err_code;

    log_init();

    /* Configure LED-pins as outputs */
    bsp_board_init(BSP_INIT_LEDS);

    /* Set up UART */
    err_code = uart_init();
    APP_ERROR_CHECK(err_code);

    /* Set up FIFOs for data that are transfered over NFC */
    err_code = nfc_fifos_init();
    APP_ERROR_CHECK(err_code);

    /* Set up NFC */
    err_code = nfc_t4t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("NFC UART Tag example started.");

    /* Start sensing NFC field */
    err_code = nfc_t4t_emulation_start();
    APP_ERROR_CHECK(err_code);

    while (1)
    {
        uint8_t msg_chunk;

        // Transmit NFC data with UART.
        while (app_fifo_get(&m_nfc_rx_fifo, &msg_chunk) == NRF_SUCCESS)
        {
            while (app_uart_put(msg_chunk) != NRF_SUCCESS);
        }

        // Handle data that come from UART.
        while (app_uart_get(&msg_chunk) == NRF_SUCCESS)
        {
            if (app_fifo_put(&m_nfc_tx_fifo, msg_chunk) != NRF_SUCCESS)
            {
                NRF_LOG_WARNING("NFC TX FIFO buffer overflow");
            }
        }

        NRF_LOG_FLUSH();
    }
}


/** @} */
