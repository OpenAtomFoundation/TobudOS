/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
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
#include <stdlib.h>
#include "nrf_cli.h"
#include "radio_test.h"

#ifdef NRF52832_XXAA
    #define TOGGLE_DCDC_HELP \
    "Toggle DCDC state <state>, if state = 1 then DC/DC converter is enabled"
#else
    #define TOGGLE_DCDC_HELP                                       \
    "Toggle DCDC state <state>, "                                  \
    "if state = 1 then toggle DC/DC REG1 state, or if state = 0 "  \
    "then toggle DC/DC REG0 state"
#endif

extern volatile transmit_pattern_t g_pattern;                           /**< Radio transmission pattern. */
extern          uint8_t            g_rx_packet[RADIO_MAX_PAYLOAD_LEN];  /**< Buffer for the radio RX packet. */ 

static uint8_t  m_mode          = RADIO_MODE_MODE_Ble_1Mbit;  /**< Radio mode. Data rate and modulation. */
static uint8_t  m_txpower       = RADIO_TXPOWER_TXPOWER_0dBm; /**< Radio output power. */
static uint8_t  m_channel_start = 0;                          /**< Radio start channel (frequency). */
static uint8_t  m_channel_end   = 80;                         /**< Radio end channel (frequency). */
static uint32_t m_delay_ms      = 10;                         /**< Delay time in milliseconds. */
static uint32_t m_duty_cycle    = 50;                         /**< Duty cycle. */
static bool     m_sweep         = false;                      /**< If true, RX or TX channel is sweeped. */


/**
 * @brief Function for printing the received payload.
 *
 * @param[in] mode       Radio mode. Data rate and modulation.
 * @param[in] p_cli      Pointer to the CLI instance.
 */
 static void radio_print_rx(nrf_cli_t const * p_cli, uint8_t mode)
{
    size_t size;
#ifdef NRF52840_XXAA
    if (mode == RADIO_MODE_MODE_Ieee802154_250Kbit)
    {
        size = IEEE_MAX_PAYLOAD_LEN;
    }
    else
    {
        size = sizeof(g_rx_packet);
    }
#else
    size = sizeof(g_rx_packet);
#endif // NRF52840_XXAA

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Received payload:\r\n");

    for (uint32_t i = 0; i < size; i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Data: %d\r\n", g_rx_packet[i]);
    }
};


#ifdef NRF52840_XXAA
static void ieee_channel_check(nrf_cli_t const * p_cli, uint8_t channel)
{
    if (m_mode == RADIO_MODE_MODE_Ieee802154_250Kbit)
    {
        if ((channel < IEEE_MIN_CHANNEL) || (channel > IEEE_MAX_CHANNEL))
        {
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_ERROR, 
                            "For %s mode channel must be between %d and %d.\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Ieee802154_250Kbit),
                            IEEE_MIN_CHANNEL,
                            IEEE_MAX_CHANNEL);

            nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Channel set to %d.\r\n", IEEE_MIN_CHANNEL);
        }

    }
}
#endif // NRF52840_XXAA


static void cmd_start_channel_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count.\r\n", argv[0]);
        return;
    }

    uint32_t channel;

    channel = atoi(argv[1]);

    if (channel > 80)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Channel must be between 0 and 80.\r\n");
        return;
    }

    m_channel_start = (uint8_t)channel;

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Start channel set to: %d.\r\n", channel);
}


static void cmd_end_channel_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count.\r\n", argv[0]);
        return;
    }

    uint32_t channel;

    channel = atoi(argv[1]);

    if (channel > 80)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Channel must be between 0 and 80.\r\n");
        return;
    }

    m_channel_end = (uint8_t)channel;

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "End channel set to: %d.\r\n", channel);
}


static void cmd_time_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count.\r\n", argv[0]);
        return;
    }

    uint32_t time;

    time = atoi(argv[1]);

    if (time > 99)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Delay time must be between 0 and 99 ms.\r\n");
        return;
    }

    m_delay_ms = time;

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Delay time set to: %d.\r\n", time);
}


static void cmd_cancel(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    radio_sweep_end();
}


static void cmd_data_rate_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count.\r\n", argv[0]);
        return;
    }

    if (argc == 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Uknown argument: %s.\r\n", argv[1]);
    }
}


static void cmd_tx_carrier_start(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (m_sweep)
    {
        radio_sweep_end();
        m_sweep = false;
    }

#ifdef NRF52840_XXAA
    ieee_channel_check(p_cli, m_channel_start);
#endif //NRF52840_XXAA 
    radio_unmodulated_tx_carrier(m_txpower, m_mode, m_channel_start);

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Start the TX carrier.\r\n");
}


static void cmd_tx_modulated_carrier_start(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (m_sweep)
    {
        radio_sweep_end();
        m_sweep = false;
    }

#ifdef NRF52840_XXAA
    ieee_channel_check(p_cli, m_channel_start);
#endif //NRF52840_XXAA 
    radio_modulated_tx_carrier(m_txpower, m_mode, m_channel_start);

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Start the modulated TX carrier.\r\n");
}


static void cmd_duty_cycle_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count.\r\n", argv[0]);
        return;
    }

    uint32_t duty_cycle;

    duty_cycle = atoi(argv[1]);

    if (duty_cycle > 100)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Duty cycle must be between 1 and 99.\r\n");
        return;
    }

    m_duty_cycle = duty_cycle;

#ifdef NRF52840_XXAA
    ieee_channel_check(p_cli, m_channel_start);
#endif //NRF52840_XXAA 
    radio_modulated_tx_carrier_duty_cycle(m_txpower, m_mode, m_channel_start, (uint8_t)m_duty_cycle);
}


static void cmd_output_power_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count.\r\n", argv[0]);
    }

    if (argc == 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Uknown argument: %s.\r\n", argv[1]);
    }
}


#ifdef NRF52840_XXAA
static void cmd_pos8dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Pos8dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos8dBm));
}


static void cmd_pos7dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Pos7dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos7dBm));
}


static void cmd_pos6dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Pos6dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos6dBm));
}


static void cmd_pos5dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Pos5dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos5dBm));
}


static void cmd_pos2dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Pos2dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos2dBm));
}


#endif // NRF52840_XXAA


static void cmd_pos3dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Pos3dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos3dBm));
}


static void cmd_pos4dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Pos4dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos4dBm));
}


static void cmd_pos0dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_0dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n", 
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_0dBm));
}


static void cmd_neg4dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Neg4dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg4dBm));
}


static void cmd_neg8dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Neg8dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg8dBm));
}


static void cmd_neg12dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Neg12dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg12dBm));
}


static void cmd_neg16dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Neg16dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg16dBm));
}


static void cmd_neg20dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Neg20dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg20dBm));
}


static void cmd_neg30dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Neg30dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg30dBm));
}


static void cmd_neg40dbm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_txpower = RADIO_TXPOWER_TXPOWER_Neg40dBm;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "TX power: %s\r\n",
                    STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg40dBm));
}


static void cmd_transmit_pattern_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count.\r\n", argv[0]);
        return;
    }

    if (argc == 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Uknown argument: %s.\r\n", argv[1]);
    }
}


static void cmd_print(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Parameters:\r\n");

    switch (m_mode)
    {
#ifdef NRF52832_XXAA
        case RADIO_MODE_MODE_Nrf_250Kbit:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Nrf_250Kbit));
            break;

#endif // NRF52832_XXAA
        case RADIO_MODE_MODE_Nrf_1Mbit:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Nrf_1Mbit));
            break;

        case RADIO_MODE_MODE_Nrf_2Mbit:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Nrf_2Mbit));
            break;

        case RADIO_MODE_MODE_Ble_1Mbit:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Ble_1Mbit));
            break;

        case RADIO_MODE_MODE_Ble_2Mbit:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Ble_2Mbit));
            break;

#ifdef NRF52840_XXAA
        case RADIO_MODE_MODE_Ble_LR125Kbit:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Ble_LR125Kbit));
            break;

        case RADIO_MODE_MODE_Ble_LR500Kbit:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Ble_LR500Kbit));
            break;

        case RADIO_MODE_MODE_Ieee802154_250Kbit:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate: %s\r\n",
                            STRINGIFY_(RADIO_MODE_MODE_Ieee802154_250Kbit));
            break;

#endif // NRF52840_XXAA
        default:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "Data rate unknown or deprecated: %lu\n\r", 
                            m_mode);
            break;
    }

    switch (m_txpower)
    {
#ifdef NRF52840_XXAA
        case RADIO_TXPOWER_TXPOWER_Pos8dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos8dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Pos7dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos7dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Pos6dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos6dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Pos5dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos5dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Pos2dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos2dBm));
            break;

#endif // NRF52840_XXAA
        case RADIO_TXPOWER_TXPOWER_Pos4dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos4dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Pos3dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Pos3dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_0dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_0dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Neg4dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg4dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Neg8dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg8dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Neg12dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg12dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Neg16dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg16dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Neg20dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg20dBm));
            break;

        case RADIO_TXPOWER_TXPOWER_Neg40dBm:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power: %s\r\n",
                            STRINGIFY_(RADIO_TXPOWER_TXPOWER_Neg40dBm));
            break;

        default:
            nrf_cli_fprintf(p_cli, 
                            NRF_CLI_INFO, 
                            "TX power unknown: %d", 
                            m_txpower);
            break;
    }

    switch (g_pattern)
    {
        case TRANSMIT_PATTERN_RANDOM:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_INFO,
                            "Transmission pattern: %s\r\n",
                            STRINGIFY_(TRANSMIT_PATTERN_RANDOM));
            break;

        case TRANSMIT_PATTERN_11110000:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_INFO,
                            "Transmission pattern: %s\r\n",
                            STRINGIFY_(TRANSMIT_PATTERN_11110000));
            break;

        case TRANSMIT_PATTERN_11001100:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_INFO,
                            "Transmission pattern: %s\r\n",
                            STRINGIFY_(TRANSMIT_PATTERN_11110000));
            break;

        default:
            nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Transmission pattern unknown: %d", g_pattern);
            break;
    }

nrf_cli_fprintf(p_cli, 
                NRF_CLI_INFO, 
                "Start Channel:\t%lu\r\n"
                "End Channel:\t%lu\r\n"
                "Time on each channel: %lu ms\r\n"
                "Duty cycle:\t%lu percent\r\n",
                m_channel_start,
                m_channel_end,
                m_delay_ms,
                m_duty_cycle);
}


static void cmd_rx_sweep_start(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    radio_rx_sweep_start(m_mode, m_channel_start, m_channel_end, (uint8_t)m_delay_ms);
    m_sweep = true;

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "RX sweep\r\n");
}


static void cmd_tx_sweep_start(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    radio_tx_sweep_start(m_txpower, m_mode, m_channel_start, m_channel_end, (uint8_t)m_delay_ms);
    m_sweep = true;

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "TX sweep\r\n");
}


static void cmd_rx_start(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (m_sweep)
    {
        radio_sweep_end();
        m_sweep = false;
    }

 #ifdef NRF52840_XXAA
    ieee_channel_check(p_cli, m_channel_start);
#endif //NRF52840_XXAA 

    memset(g_rx_packet, 0, sizeof(g_rx_packet));

    radio_rx(m_mode, m_channel_start);
}


static void cmd_nrf_1mbit(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_mode = RADIO_MODE_MODE_Nrf_1Mbit;
    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Data rate: %s\r\n", STRINGIFY_(RADIO_MODE_MODE_Nrf_1Mbit));
}


static void cmd_nrf_2mbit(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_mode = RADIO_MODE_MODE_Nrf_2Mbit;
    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Data rate: %s\r\n", STRINGIFY_(RADIO_MODE_MODE_Nrf_2Mbit));
}


#ifdef  NRF52832_XXAA
static void cmd_nrf_250kbit(nrf_cli_t const * p_cli, size_t argc, char * * argv)
{
    m_mode = RADIO_MODE_MODE_Nrf_250Kbit;
    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Data rate: %s\r\n", STRINGIFY(RADIO_MODE_MODE_Nrf_250Kbit));
}


#endif // NRF52832_XXAA


static void cmd_ble_1mbit(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_mode = RADIO_MODE_MODE_Ble_1Mbit;
    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Data rate: %s\r\n", STRINGIFY_(RADIO_MODE_MODE_Ble_1Mbit));
}


static void cmd_ble_2mbit(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_mode = RADIO_MODE_MODE_Ble_2Mbit;
    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "Data rate: %s\r\n", STRINGIFY_(RADIO_MODE_MODE_Ble_2Mbit));
}


#ifdef NRF52840_XXAA
static void cmd_ble_lr125kbit(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_mode = RADIO_MODE_MODE_Ble_LR125Kbit;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "Data rate: %s\r\n",
                    STRINGIFY_(RADIO_MODE_MODE_Ble_LR125Kbit));
}


static void cmd_ble_lr500kbit(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_mode = RADIO_MODE_MODE_Ble_LR500Kbit;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "Data rate: %s\r\n",
                    STRINGIFY_(RADIO_MODE_MODE_Ble_LR500Kbit));
}


static void cmd_ble_ieee(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    m_mode = RADIO_MODE_MODE_Ieee802154_250Kbit;
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "Data rate: %s\r\n",
                    STRINGIFY_(RADIO_MODE_MODE_Ieee802154_250Kbit));
}


#endif // NRF52840_XXAA


static void cmd_pattern_random(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    g_pattern = TRANSMIT_PATTERN_RANDOM;
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_INFO,
                    "Transmission pattern: %s.\r\n",
                    STRINGIFY_(TRANSMIT_PATTERN_RANDOM));
}


static void cmd_pattern_11110000(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    g_pattern = TRANSMIT_PATTERN_11110000;
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_INFO,
                    "Transmission pattern: %s.\r\n",
                    STRINGIFY_(TRANSMIT_PATTERN_11110000));
}


static void cmd_pattern_11001100(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    g_pattern = TRANSMIT_PATTERN_11001100;
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_INFO,
                    "Transmission pattern: %s.\r\n",
                    STRINGIFY_(TRANSMIT_PATTERN_11001100));
}


static void cmd_toggle_dc(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameters count\r\n", argv[0]);
    }

    uint32_t state = atoi(argv[1]);

    if (state > 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Invalid DCDC value provided\n\r");
    }

    toggle_dcdc_state((uint8_t)state);

#ifdef NRF52840_XXAA
    uint32_t dcdcen = NRF_POWER->DCDCEN;

    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "DCDC REG0 state %lu\r\n",
                    "DCDC REG1 state %lu\r\n",
                     "Write '0' to toggle state of DCDC REG0\r\n",
                     "Write '1' to toggle state of DCDC REG1",
                    NRF_POWER->DCDCEN0,
                    dcdcen);
#else
    nrf_cli_fprintf(p_cli, 
                    NRF_CLI_INFO, 
                    "DCDC state %lu\r\n",
                    "Write '1' to enable, '0' to disable\r\n",
                    NRF_POWER->DCDCEN);
#endif
}


static void cmd_print_payload(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    radio_print_rx(p_cli, m_mode);
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_data_rate)
{
    NRF_CLI_CMD(nrf_1Mbit, NULL, "1 Mbit/s Nordic proprietary radio mode", cmd_nrf_1mbit),
    NRF_CLI_CMD(nrf_2Mbit, NULL, "2 Mbit/s Nordic proprietary radio mode", cmd_nrf_2mbit),
    #ifdef NRF52832_XXAA
    NRF_CLI_CMD(nrf_250Kbit, NULL, "250 kbit/s Nordic proprietary radio mode", cmd_nrf_250kbit),
    #endif // NRF52832_XXAA
    NRF_CLI_CMD(ble_1Mbit, NULL, "1 Mbit/s Bluetooth Low Energy", cmd_ble_1mbit),
    NRF_CLI_CMD(ble_2Mbit, NULL, "2 Mbit/s Bluetooth Low Energy", cmd_ble_2mbit),

    #ifdef NRF52840_XXAA
    NRF_CLI_CMD(ble_lr250Kbit,
                NULL,
                "Long range 125 kbit/s TX, 125 kbit/s and 500 kbit/s RX",
                cmd_ble_lr125kbit),
    NRF_CLI_CMD(ble_lr500Kbit,
                NULL,
                "Long range 500 kbit/s TX, 125 kbit/s and 500 kbit/s RX",
                cmd_ble_lr500kbit),
    NRF_CLI_CMD(ieee802154_250Kbit, NULL, "IEEE 802.15.4-2006 250 kbit/s", cmd_ble_ieee),
    #endif // NRF52840_XXAA

    NRF_CLI_SUBCMD_SET_END
};


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_output_power)
{
#ifdef NRF52840_XXAA
    NRF_CLI_CMD(pos8dBm, NULL, "TX power: +8 dBm", cmd_pos8dbm),
    NRF_CLI_CMD(pos7dBm, NULL, "TX power: +7 dBm", cmd_pos7dbm),
    NRF_CLI_CMD(pos6dBm, NULL, "TX power: +6 dBm", cmd_pos6dbm),
    NRF_CLI_CMD(pos5dBm, NULL, "TX power: +5 dBm", cmd_pos5dbm),
    NRF_CLI_CMD(pos2dBm, NULL, "TX power: +2 dBm", cmd_pos2dbm),
#endif // NRF52840_XXAA
    NRF_CLI_CMD(pos3dBm, NULL, "TX power: +3 dBm", cmd_pos3dbm),
    NRF_CLI_CMD(pos4dBm, NULL, "TX power: +4 dBm", cmd_pos4dbm),
    NRF_CLI_CMD(pos0dBm, NULL, "TX power: 0 dBm", cmd_pos0dbm),
    NRF_CLI_CMD(neg4dBm, NULL, "TX power: -4 dBm", cmd_neg4dbm),
    NRF_CLI_CMD(neg8dbm, NULL, "TX power: -8 dBm", cmd_neg8dbm),
    NRF_CLI_CMD(neg12dbm, NULL, "TX power: -12 dBm", cmd_neg12dbm),
    NRF_CLI_CMD(neg16dbm, NULL, "TX power: -16 dBm", cmd_neg16dbm),
    NRF_CLI_CMD(neg20dbm, NULL, "TX power: -20 dBm", cmd_neg20dbm),
    NRF_CLI_CMD(neg30dbm, NULL, "TX power: -30 dBm", cmd_neg30dbm),
    NRF_CLI_CMD(neg40dbm, NULL, "TX power: -40 dBm", cmd_neg40dbm),
    NRF_CLI_SUBCMD_SET_END
};


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_transmit_pattern)
{
    NRF_CLI_CMD(pattern_random, NULL, "Set the transmission pattern to random.", cmd_pattern_random),
    NRF_CLI_CMD(pattern_11110000, NULL, "Set the transmission pattern to 11110000.", cmd_pattern_11110000),
    NRF_CLI_CMD(pattern_11001100, NULL, "Set the transmission pattern to 10101010.", cmd_pattern_11001100),
    NRF_CLI_SUBCMD_SET_END
};


NRF_CLI_CMD_REGISTER(start_channel,
                     NULL,
                     "Start the channel for the sweep or the channel for the constant carrier <channel>",
                     cmd_start_channel_set);
NRF_CLI_CMD_REGISTER(end_channel, NULL, "End the channel for the sweep <channel>", cmd_end_channel_set);
NRF_CLI_CMD_REGISTER(time_on_channel,
                     NULL,
                     "Time on each channel (between 1 ms and 99 ms) <time>",
                     cmd_time_set);
NRF_CLI_CMD_REGISTER(cancel, NULL, "Cancel the sweep or the carrier", cmd_cancel);
NRF_CLI_CMD_REGISTER(data_rate, &m_sub_data_rate, "Set data rate <sub_cmd>", cmd_data_rate_set);
NRF_CLI_CMD_REGISTER(start_tx_carrier, NULL, "Start the TX carrier", cmd_tx_carrier_start);
NRF_CLI_CMD_REGISTER(start_tx_modulated_carrier,
                     NULL,
                     "Start the modulated TX carrier",
                     cmd_tx_modulated_carrier_start);
NRF_CLI_CMD_REGISTER(output_power,
                     &m_sub_output_power,
                     "Output power set <sub_cmd>",
                     cmd_output_power_set);
NRF_CLI_CMD_REGISTER(transmit_pattern,
                     &m_sub_transmit_pattern,
                     "Set the transmission pattern",
                     cmd_transmit_pattern_set);
NRF_CLI_CMD_REGISTER(start_duty_cycle_modulated_tx,
                     NULL,
                     "Duty cycle in percent (two decimal digits, between 01 and 99) <duty_cycle>",
                     cmd_duty_cycle_set);
NRF_CLI_CMD_REGISTER(parameters_print, NULL, "Print current delay, channel and so on", cmd_print);
NRF_CLI_CMD_REGISTER(start_rx_sweep, NULL, "Start RX sweep", cmd_rx_sweep_start);
NRF_CLI_CMD_REGISTER(start_tx_sweep, NULL, "Start TX sweep", cmd_tx_sweep_start);
NRF_CLI_CMD_REGISTER(start_rx, NULL, "Start RX", cmd_rx_start);
NRF_CLI_CMD_REGISTER(toggle_dcdc_state, NULL, TOGGLE_DCDC_HELP, cmd_toggle_dc);
NRF_CLI_CMD_REGISTER(print_rx, NULL, "Print received payload", cmd_print_payload);
