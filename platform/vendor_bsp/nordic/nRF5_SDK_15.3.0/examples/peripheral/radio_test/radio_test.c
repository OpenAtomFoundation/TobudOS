/**
 * Copyright (c) 2009-2018 - 2019, Nordic Semiconductor ASA
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
 * @addtogroup nrf_radio_test_example_main
 * @{
 */

#include "radio_test.h"
#include <stdbool.h>
#include "nrf.h"
#include "app_util_platform.h"

#define IEEE_DEFAULT_FREQ         (5)   /**< IEEE 802.15.4 default frequency. */
#define RADIO_LENGTH_LENGTH_FIELD (8UL) /**< Length on air of the LENGTH field. */


#define IEEE_FREQ_CALC(_channel) (IEEE_DEFAULT_FREQ + \
                                  (IEEE_DEFAULT_FREQ * ((_channel) - IEEE_MIN_CHANNEL))) /**< Frequency calculation for a given channel in the IEEE 802.15.4 radio mode. */

static uint8_t m_tx_packet[RADIO_MAX_PAYLOAD_LEN];                                       /**< Buffer for the radio TX packet. */
       uint8_t g_rx_packet[RADIO_MAX_PAYLOAD_LEN];                                       /**< Buffer for the radio RX packet. */

static volatile uint8_t     m_mode;                                                      /**< Radio mode. Data rate and modulation. */
static volatile uint8_t     m_txpower;                                                   /**< Radio output power. */
static volatile uint8_t     m_channel_start;                                             /**< Radio start channel (frequency).*/
static volatile uint8_t     m_channel_end;                                               /**< Radio end channel (frequency). */
static volatile uint8_t     m_channel;                                                   /**< Radio current channel (frequency). */
volatile transmit_pattern_t g_pattern;                                                   /**< Radio transmission pattern. */
static volatile bool        m_sweep_tx;                                                  /**< Sweep TX. If true, the TX channel is sweeped. */
static volatile bool        m_sweep_rx;                                                  /**< Sweep RX. If true, the RX channel is sweeped. */

/**
 * @brief Function for initializing Timer 0 in the 24-bit timer mode with a resolution of 1 µs.
 *
 * @param[in] delay_ms Delay time in milliseconds.
 */
static void timer0_init(uint8_t delay_ms)
{
    NRF_TIMER0->TASKS_STOP = 1;

    // Create an Event-Task shortcut to clear Timer 1 on COMPARE[0] event.
    NRF_TIMER0->SHORTS    = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    NRF_TIMER0->MODE      = TIMER_MODE_MODE_Timer;
    NRF_TIMER0->BITMODE   = (TIMER_BITMODE_BITMODE_24Bit << TIMER_BITMODE_BITMODE_Pos);
    NRF_TIMER0->PRESCALER = 4; // resolution of 1 µs
    NRF_TIMER0->INTENSET  = (TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos);

    // Sample update must happen as soon as possible. The earliest possible moment is MAX_SAMPLE_LEVELS
    // ticks before changing the output duty cycle.
    NRF_TIMER0->CC[0]       = ((uint32_t)delay_ms * 1000) - 1;
    NRF_TIMER0->CC[1]       = 0;
    NRF_TIMER0->TASKS_START = 1;
}


/**
 * @brief Function for generating an 8-bit random number with the internal random generator.
 */
static uint32_t rnd8(void)
{
    NRF_RNG->EVENTS_VALRDY = 0;

    while (NRF_RNG->EVENTS_VALRDY == 0)
    {
        // Do nothing.
    }
    return NRF_RNG->VALUE;
}


/**
 * @brief Function for generating a 32-bit random number with the internal random generator.
 */
static uint32_t rnd32(void)
{
    uint8_t  i;
    uint32_t val = 0;

    for (i = 0; i < 4; i++)
    {
        val <<= 8;
        val  |= rnd8();
    }
    return val;
}


/**brief Function for setting the channel for radio.
 *
 * @param[in] mode    Radio mode.
 * @param[in] channel Radio channel.
 */
static void radio_channel_set(uint8_t mode, uint8_t channel)
{
#ifdef NRF52840_XXAA
    if (mode == RADIO_MODE_MODE_Ieee802154_250Kbit)
    {
        if (channel >= IEEE_MIN_CHANNEL && channel <= IEEE_MAX_CHANNEL)
        {
            NRF_RADIO->FREQUENCY = IEEE_FREQ_CALC(channel);
        }

        else
        {
            NRF_RADIO->FREQUENCY = IEEE_DEFAULT_FREQ;
        }

    }
    else
    {
        NRF_RADIO->FREQUENCY = channel;
    }
#else
    NRF_RADIO->FREQUENCY = channel;
#endif // NRF52840_XXAA
}


/**@brief Function for configuring the radio in every possible mode.
 *
 * @param[in] mode Radio mode.
 */
static void radio_config(uint8_t mode)
{
    // Reset Radio ramp-up time.
    NRF_RADIO->MODECNF0 &= (~RADIO_MODECNF0_RU_Msk);

    // Packet configuration:
    // Bit 25: 1 Whitening enabled
    // Bit 24: 1 Big endian,
    // 4-byte base address length (5-byte full address length),
    // 0-byte static length, max 255-byte payload .
    NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos) |
                       (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos) |
                       (4UL << RADIO_PCNF1_BALEN_Pos) |
                       (0UL << RADIO_PCNF1_STATLEN_Pos) |
                       ((sizeof(m_tx_packet) - 1) << RADIO_PCNF1_MAXLEN_Pos);
    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Disabled << RADIO_CRCCNF_LEN_Pos);

    NRF_RADIO->TXADDRESS   = 0x00UL; // Set the device address 0 to use when transmitting
    NRF_RADIO->RXADDRESSES = 0x01UL; // Enable the device address 0 to use to select which addresses to receive

    // Set the address according to the transmission pattern.
    switch (g_pattern)
    {
        case TRANSMIT_PATTERN_RANDOM:
            NRF_RADIO->PREFIX0 = rnd8();
            NRF_RADIO->BASE0   = rnd32();
            break;

        case TRANSMIT_PATTERN_11001100:
            NRF_RADIO->PREFIX0 = 0xCC;
            NRF_RADIO->BASE0   = 0xCCCCCCCC;
            break;

        case TRANSMIT_PATTERN_11110000:
            NRF_RADIO->PREFIX0 = 0xF0;
            NRF_RADIO->BASE0   = 0xF0F0F0F0;
            break;

        default:
            return;
    }


    switch (mode)
    {
     #ifdef NRF52840_XXAA
        case RADIO_MODE_MODE_Ieee802154_250Kbit:
        {
            // Packet configuration:
            // S1 size = 0 bits, S0 size = 0 bytes, payload length size = 8 bits, 32-bit preamble.
            NRF_RADIO->PCNF0 = (RADIO_LENGTH_LENGTH_FIELD << RADIO_PCNF0_LFLEN_Pos) |
                               (RADIO_PCNF0_PLEN_32bitZero << RADIO_PCNF0_PLEN_Pos) |
                               (RADIO_PCNF0_CRCINC_Exclude << RADIO_PCNF0_CRCINC_Pos);
            NRF_RADIO->PCNF1 = (IEEE_MAX_PAYLOAD_LEN << RADIO_PCNF1_MAXLEN_Pos);

            NRF_RADIO->MODECNF0    |= (RADIO_MODECNF0_RU_Fast << RADIO_MODECNF0_RU_Pos);
        } break;

        case RADIO_MODE_MODE_Ble_LR500Kbit:
        case RADIO_MODE_MODE_Ble_LR125Kbit:
        {
            // Packet configuration:
            // S1 size = 0 bits, S0 size = 0 bytes, payload length size = 8 bits, 10-bit preamble.
            NRF_RADIO->PCNF0 = (0UL << RADIO_PCNF0_S1LEN_Pos) |
                               (0UL << RADIO_PCNF0_S0LEN_Pos) |
                               (RADIO_PCNF0_PLEN_LongRange << RADIO_PCNF0_PLEN_Pos) |
                               (2UL << RADIO_PCNF0_CILEN_Pos) |
                               (3UL << RADIO_PCNF0_TERMLEN_Pos) |
                               (RADIO_PCNF0_CRCINC_Exclude << RADIO_PCNF0_CRCINC_Pos) |
                               (RADIO_LENGTH_LENGTH_FIELD << RADIO_PCNF0_LFLEN_Pos);
            NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Enabled << RADIO_PCNF1_WHITEEN_Pos) |
                               (RADIO_PCNF1_ENDIAN_Little << RADIO_PCNF1_ENDIAN_Pos) |
                               (3UL << RADIO_PCNF1_BALEN_Pos) |
                               (0UL << RADIO_PCNF1_STATLEN_Pos) |
                               ((sizeof(m_tx_packet) - 1) << RADIO_PCNF1_MAXLEN_Pos);

            // Set fast ramp-up time.
            NRF_RADIO->MODECNF0 |= (RADIO_MODECNF0_RU_Fast << RADIO_MODECNF0_RU_Pos);

            // Set CRC length; CRC calculation does not include the address field.
            NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) |
                                (RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos);
        } break;
     #endif // NRF52840_XXAA

        case RADIO_MODE_MODE_Ble_2Mbit:
        {
            // Packet configuration:
            // S1 size = 0 bits, S0 size = 0 bytes, payload length size = 8 bits, 16-bit preamble.
            NRF_RADIO->PCNF0 = (0UL << RADIO_PCNF0_S1LEN_Pos) |
                               (0UL << RADIO_PCNF0_S0LEN_Pos) |
                               (RADIO_PCNF0_PLEN_16bit << RADIO_PCNF0_PLEN_Pos) |
                               (RADIO_LENGTH_LENGTH_FIELD << RADIO_PCNF0_LFLEN_Pos);
        } break;

        default:
        {
            // Packet configuration:
            // S1 size = 0 bits, S0 size = 0 bytes, payload length size = 8 bits, 8 -bit preamble.
            NRF_RADIO->PCNF0 = (0UL << RADIO_PCNF0_S1LEN_Pos) |
                               (0UL << RADIO_PCNF0_S0LEN_Pos) |
                               (RADIO_PCNF0_PLEN_8bit << RADIO_PCNF0_PLEN_Pos) |
                               (RADIO_LENGTH_LENGTH_FIELD << RADIO_PCNF0_LFLEN_Pos);
        } break;
    }
}


/**
 * @brief Function for configuring the radio to use a random address and a 254-byte random payload.
 * The S0 and S1 fields are not used.
 *
 * param[in] mode Radio mode.
 */
static void generate_modulated_rf_packet(uint8_t mode)
{
    radio_config(mode);

    // One byte used for size, actual size is SIZE-1
#ifdef NRF52840_XXAA
    if (mode == RADIO_MODE_MODE_Ieee802154_250Kbit)
    {
        m_tx_packet[0] = IEEE_MAX_PAYLOAD_LEN - 1;
    }
    else
    {
        m_tx_packet[0] = sizeof(m_tx_packet) - 1;
    }
#else
    m_tx_packet[0] = sizeof(m_tx_packet) - 1;
#endif // NRF52840_XXAA

    // Fill payload with random data.
    for (uint8_t i = 0; i < sizeof(m_tx_packet) - 1; i++)
    {
        if (g_pattern == TRANSMIT_PATTERN_RANDOM)
            m_tx_packet[i + 1] = rnd8();
        else if (g_pattern == TRANSMIT_PATTERN_11001100)
            m_tx_packet[i + 1] = 0xCC;
        else if (g_pattern == TRANSMIT_PATTERN_11110000)
            m_tx_packet[i + 1] = 0xF0;
        else
        {
            // Do nothing.
        }
    }
    NRF_RADIO->PACKETPTR = (uint32_t)m_tx_packet;
}


/**@brief Function for disabling radio.
 */
static void radio_disable(void)
{
    NRF_RADIO->SHORTS          = 0;
    NRF_RADIO->EVENTS_DISABLED = 0;
#ifdef NRF51
    NRF_RADIO->TEST = 0;
#endif
    NRF_RADIO->TASKS_DISABLE = 1;

    while (NRF_RADIO->EVENTS_DISABLED == 0)
    {
        // Do nothing.
    }
    NRF_RADIO->EVENTS_DISABLED = 0;
}


void radio_sweep_end(void)
{
    NRF_TIMER0->TASKS_STOP = 1;
    radio_disable();
}


void radio_unmodulated_tx_carrier(uint8_t txpower, uint8_t mode, uint8_t channel)
{
    radio_disable();
    NRF_RADIO->SHORTS  = RADIO_SHORTS_READY_START_Msk;
    NRF_RADIO->TXPOWER = (txpower << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->MODE    = (mode << RADIO_MODE_MODE_Pos);

    radio_channel_set(mode, channel);

#ifdef NRF51
    NRF_RADIO->TEST = (RADIO_TEST_CONST_CARRIER_Enabled << RADIO_TEST_CONST_CARRIER_Pos) \
                      | (RADIO_TEST_PLL_LOCK_Enabled << RADIO_TEST_PLL_LOCK_Pos);
#endif
    NRF_RADIO->TASKS_TXEN = 1;
}


/**
 * @brief Function for starting the modulated TX carrier by repeatedly sending a packet with a random address and
 * a random payload.
 */
void radio_modulated_tx_carrier(uint8_t txpower, uint8_t mode, uint8_t channel)
{
    radio_disable();
    generate_modulated_rf_packet(mode);
    #ifdef NRF51
    NRF_RADIO->SHORTS = RADIO_SHORTS_END_DISABLE_Msk | RADIO_SHORTS_READY_START_Msk | \
                        RADIO_SHORTS_DISABLED_TXEN_Msk;
    #else

    switch (mode)
    {
        #ifdef NRF52840_XXAA
        case RADIO_MODE_MODE_Ieee802154_250Kbit:
        case RADIO_MODE_MODE_Ble_LR125Kbit:
        case RADIO_MODE_MODE_Ble_LR500Kbit:
        {
            NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_PHYEND_START_Msk;
        } break;
        #endif // NRF52840_XXAA

        case RADIO_MODE_MODE_Ble_1Mbit:
        case RADIO_MODE_MODE_Ble_2Mbit:
        case RADIO_MODE_MODE_Nrf_1Mbit:
        case RADIO_MODE_MODE_Nrf_2Mbit:
        default:
        #ifdef NRF52832_XXAA
        case RADIO_MODE_MODE_Nrf_250Kbit:
        #endif // NRF52832_XXAA
            {
                NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_START_Msk;
            } break;
    }
    #endif
    NRF_RADIO->TXPOWER = (txpower << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->MODE    = (mode << RADIO_MODE_MODE_Pos);

    radio_channel_set(mode, channel);

    NRF_RADIO->EVENTS_END = 0U;
    NRF_RADIO->TASKS_TXEN = 1;

    while (NRF_RADIO->EVENTS_END == 0U)
    {
        // wait
    }

}


void radio_modulated_tx_carrier_duty_cycle(uint8_t txpower,
                                           uint8_t mode,
                                           uint8_t channel,
                                           uint8_t duty_cycle)
{
    // Lookup table with time per byte in each radio MODE
    // Mapped per NRF_RADIO->MODE available on nRF5-series devices @ref <insert ref to mode register>
    static const uint8_t time_in_us_per_byte[16] =
    {8, 4, 32, 8, 4, 64, 16, 0, 0, 0, 0, 0, 0, 0, 0, 32};
    // 1 byte preamble, 5 byte address (BALEN + PREFIX), and sizeof(payload), no CRC
    const uint32_t total_payload_size     = 1 + 5 + sizeof(m_tx_packet);
    const uint32_t total_time_per_payload = time_in_us_per_byte[mode] * total_payload_size;
    // Duty cycle = 100 * Time_on / (time_on + time_off), we need to calculate "time_off" for delay.
    // In addition, the timer includes the "total_time_per_payload", so we need to add this to the total timer cycle.
    uint32_t delay_time = total_time_per_payload +
                          ((100 * total_time_per_payload -
                            (total_time_per_payload * duty_cycle)) / duty_cycle);

    CRITICAL_REGION_ENTER();
    radio_disable();
    generate_modulated_rf_packet(mode);
    // We let the TIMER start the radio transmission again.


    NRF_RADIO->SHORTS    = RADIO_SHORTS_END_DISABLE_Msk | RADIO_SHORTS_READY_START_Msk;
    NRF_RADIO->TXPOWER   = (txpower << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->MODE      = (mode << RADIO_MODE_MODE_Pos);
    NRF_RADIO->FREQUENCY = channel;

    radio_channel_set(mode, channel);

    NRF_TIMER0->TASKS_STOP  = 1;
    NRF_TIMER0->TASKS_CLEAR = 1;
    NRF_TIMER0->INTENSET    = (TIMER_INTENSET_COMPARE1_Set << TIMER_INTENSET_COMPARE1_Pos);
    NRF_TIMER0->SHORTS      =
        (TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos);
    // Clear CC[0], in case sweep TX/RX was previously active
    NRF_TIMER0->CC[0]             = 0;
    NRF_TIMER0->CC[1]             = delay_time;
    NRF_TIMER0->EVENTS_COMPARE[0] = 0;
    NRF_TIMER0->EVENTS_COMPARE[1] = 0;
    NRF_TIMER0->BITMODE           = (TIMER_BITMODE_BITMODE_24Bit << TIMER_BITMODE_BITMODE_Pos);
    NRF_TIMER0->PRESCALER         = 4; // resolution of 1 µs
    NRF_TIMER0->TASKS_START       = 1;
    CRITICAL_REGION_EXIT();
}


void radio_rx(uint8_t mode, uint8_t channel)
{
    radio_disable();

    NRF_RADIO->MODE      = (mode << RADIO_MODE_MODE_Pos);
    NRF_RADIO->SHORTS    = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_START_Msk;
    NRF_RADIO->PACKETPTR = (uint32_t)g_rx_packet;

    radio_config(mode);
    radio_channel_set(mode, channel);

    NRF_RADIO->TASKS_RXEN = 1U;
}


void radio_tx_sweep_start(uint8_t txpower,
                          uint8_t mode,
                          uint8_t channel_start,
                          uint8_t channel_end,
                          uint8_t delayms)
{
    m_txpower       = txpower;
    m_mode          = mode;
    m_channel_start = channel_start;
    m_channel       = channel_start;
    m_channel_end   = channel_end;
    m_sweep_tx      = true;
    m_sweep_rx      = false;
    timer0_init(delayms);
}


void radio_rx_sweep_start(uint8_t mode, uint8_t channel_start, uint8_t channel_end, uint8_t delayms)
{
    m_mode          = mode;
    m_channel_start = channel_start;
    m_channel       = channel_start;
    m_channel_end   = channel_end;
    m_sweep_tx      = false;
    m_sweep_rx      = true;
    timer0_init(delayms);
}


void toggle_dcdc_state(uint8_t dcdc_state)
{
#ifdef NRF52840_XXAA
    if (dcdc_state == 0)
    {
        NRF_POWER->DCDCEN0 = (NRF_POWER->DCDCEN0 == POWER_DCDCEN0_DCDCEN_Disabled) ? 1 : 0;
    }
    else if (dcdc_state == 1)
    {
        NRF_POWER->DCDCEN = (NRF_POWER->DCDCEN == POWER_DCDCEN_DCDCEN_Disabled) ? 1 : 0;
    }
    else
    {
        // Do nothing.
    }
#else
    if (dcdc_state <= 1)
    {
        NRF_POWER->DCDCEN = dcdc_state;
    }
#endif // NRF52840_XXAA
}


/**
 * @brief Function for handling the Timer 0 interrupt used for the TX or RX sweep. The carrier is started with the new channel,
 * and the channel is incremented for the next interrupt.
 */
void TIMER0_IRQHandler(void)
{
    uint8_t channel_end   = m_channel_end;
    uint8_t channel_start = m_channel_start;
    uint8_t mode          = m_mode;
    uint8_t txpower       = m_txpower;

    // COMPARE[0] handles channel sweep
    if (NRF_TIMER0->EVENTS_COMPARE[0])
    {
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;
        if (m_sweep_tx)
        {
            radio_unmodulated_tx_carrier(txpower, mode, m_channel);
        }
        else if (m_sweep_rx)
        {
            radio_rx(mode, m_channel);
        }
        else
        {
           //Do nothing
        }

        m_channel++;
        if (m_channel > channel_end)
        {
            m_channel = channel_start;
        }
    }
    // Timer sequence for function
    // radio_modulated_tx_carrier_with_delay(...)
    if (NRF_TIMER0->EVENTS_COMPARE[1])
    {
        NRF_TIMER0->EVENTS_COMPARE[1] = 0;
        if (!m_sweep_tx || !m_sweep_rx)
            NRF_RADIO->TASKS_TXEN = 1;
    }
}


/**
 * @}
 */
