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
 * @defgroup nfc_uart_poller_example_main main.c
 * @{
 * @ingroup nfc_uart_poller_example
 * @brief The application main file of NFC UART poller example.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "app_error.h"
#include "app_uart.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "sdk_macros.h"
#include "sdk_config.h"

#include "adafruit_pn532.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define SEL_RES_CASCADE_BIT_NUM         3     /**< Number of Cascade bit within SEL_RES byte. */
#define SEL_RES_TAG_PLATFORM_MASK       0x60  /**< Mask of Tag Platform bit group within SEL_RES byte. */
#define SEL_RES_TAG_PLATFORM_BIT_OFFSET 5     /**< Offset of the Tag Platform bit group within SEL_RES byte. */

#define MAX_APDU_LEN                    0xF0  /**< Maximal APDU length, Adafruit limitation. */
#define HEADER_FIELD_SIZE               1     /**< Header field size. */
#define HEADER_TOKEN                    'S'   /**< Header field value. */

/**
 * @brief Possible Tag Types.
 */
typedef enum
{
    NFC_T2T = 0x00,      /**< Type 2 Tag Platform. */
    NFC_T4T = 0x01,      /**< Type 4A Tag Platform. */
    NFC_TT_NOT_SUPPORTED /**< Tag Type not supported. */
} nfc_tag_type_t;


/**
 * @brief Callback function for handling UART errors.
 */
static void uart_error_handle(app_uart_evt_t * p_event)
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
 * @brief Function for exchanging UART data over NFC between NFC tag and poller.
 *
 * This function is used for exchanging UART data over NFC at the poller side. When NFC tag
 * leaves poller field, this function returns.
 *
 * @retval  NRF_ERROR_NOT_FOUND  If data exchange with the tag fails
 */
static ret_code_t uart_data_over_nfc_exchange(void)
{
    ret_code_t err_code = NRF_SUCCESS;
    uint8_t    resp_len = MAX_APDU_LEN;
    uint8_t    ind      = 0;

    static uint8_t apdu_buff[APDU_BUFF_SIZE]; //Buffer for APDU data.

    // Flush UART buffers. Data that was collected from UART channel before selecting the
    // tag is discarded.
    UNUSED_RETURN_VALUE(app_uart_flush());

    apdu_buff[0] = HEADER_TOKEN;
    while (1)
    {
        // Handle incoming UART data.
        ind = HEADER_FIELD_SIZE;
        while (app_uart_get(&apdu_buff[ind]) == NRF_SUCCESS)
        {
            ind++;
            if (ind == MIN(MAX_APDU_LEN, APDU_BUFF_SIZE))
            {
                break;
            }
        }

        // Send the received UART data over NFC.
        resp_len = MAX_APDU_LEN;
        err_code = adafruit_pn532_in_data_exchange(apdu_buff,
                                                   ind,
                                                   apdu_buff,
                                                   &resp_len);

        // Error is returned when NFC field is lost.
        if (err_code != NRF_SUCCESS)
        {
            break;
        }

        if (ind > HEADER_FIELD_SIZE)
        {
            NRF_LOG_DEBUG("NFC TX data length: %d", (ind - HEADER_FIELD_SIZE));
        }

        if (resp_len > HEADER_FIELD_SIZE)
        {
            NRF_LOG_DEBUG("NFC RX data length: %d", (resp_len - HEADER_FIELD_SIZE));
        }

        // Send the received NFC data over UART.
        ind = HEADER_FIELD_SIZE;
        while (ind < resp_len)
        {
            while (app_uart_put(apdu_buff[ind]) != NRF_SUCCESS);
            ind++;
        }
        NRF_LOG_FLUSH();
    }
    return NRF_ERROR_NOT_FOUND;
}


/**
 * @brief Function for identifying Tag Platform Type.
 *
 * @param[in] sel_res  SEL_RES byte containg the information about Tag Platform Type.
 *
 * @return  Tag Platform Type (see @ref nfc_tag_type_t)
 */
static nfc_tag_type_t tag_type_identify(uint8_t sel_res)
{
    uint8_t platform_config;

    // Check if Cascade bit in SEL_RES response is cleared. Cleared bit indicates that NFCID1 complete.
    if (!IS_SET(sel_res, SEL_RES_CASCADE_BIT_NUM))
    {
        // Extract platform configuration from SEL_RES response.
        platform_config = (sel_res & SEL_RES_TAG_PLATFORM_MASK) >> SEL_RES_TAG_PLATFORM_BIT_OFFSET;
        if (platform_config < NFC_TT_NOT_SUPPORTED)
        {
            return (nfc_tag_type_t) platform_config;
        }
    }

    return NFC_TT_NOT_SUPPORTED;
}


/**
 * @brief Function for detecting a Tag, verifying its Type and exchanging data with it.
 *
 * This function waits for a Tag to appear in the field. When a Tag is detected, this function
 * checks if it is Type 4 Tag and starts to exchange data with it.
 *
 * @retval     NRF_ERROR_NOT_FOUND       If there is no tag near poller or data exchange with the tag fails.
 * @retval     NRF_ERROR_NOT_SUPPORTED   If the detected Tag's platform is not Type 4 Tag.
 */
static ret_code_t tag_detect_and_data_exchange(void)
{
    ret_code_t     err_code;
    nfc_a_tag_info tag_info;

    // Detect a NFC-A Tag in the field and initiate a communication. This function activates
    // the NFC RF field. If a Tag is present, basic information about detected Tag is returned
    // in tag info structure.
    err_code = adafruit_pn532_nfc_a_target_init(&tag_info, TAG_DETECT_TIMEOUT);

    if (err_code != NRF_SUCCESS)
    {
        return NRF_ERROR_NOT_FOUND;
    }

    nfc_tag_type_t tag_type = tag_type_identify(tag_info.sel_res);
    switch (tag_type)
    {
        case NFC_T4T:
            NRF_LOG_INFO("Type 4 Tag selected. UART transmission can start...");
            return uart_data_over_nfc_exchange();

        case NFC_T2T:
            // Intentional fallthrough.
        default:
            return NRF_ERROR_NOT_SUPPORTED;
    }
}


/**
 * @brief Function for initializing UART module.
 *
 * @retval  NRF_SUCCESS  If UART module was correctly initialized.
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
 * @brief Function for turning off NFC field and waiting specified time after it.
 *
 * @retval  NRF_SUCCESS  If the NFC Field was turned off.
 */
static ret_code_t nfc_turn_off_and_delay(void)
{
    ret_code_t err_code;

    // Turn off the NFC field.
    err_code = adafruit_pn532_field_off();
    nrf_delay_ms(TAG_AFTER_FIELD_OFF_DELAY);
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

int main(void)
{
    ret_code_t err_code;

    log_init();

    bsp_board_init(BSP_INIT_LEDS);
    err_code = adafruit_pn532_init(false);
    APP_ERROR_CHECK(err_code);

    err_code = uart_init();
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("NFC UART Poller example started.");

    while (1)
    {
        err_code = tag_detect_and_data_exchange();
        switch (err_code)
        {
            case NRF_ERROR_NOT_FOUND:
                NRF_LOG_INFO("No Tag found.");
                // No delay here as we want to search for another tag immediately.
                break;

            case NRF_ERROR_NOT_SUPPORTED:
                NRF_LOG_INFO("Tag Type not supported.");
                err_code = nfc_turn_off_and_delay();
                APP_ERROR_CHECK(err_code);
                break;
        }

        NRF_LOG_FLUSH();
    }
}


/** @} */

