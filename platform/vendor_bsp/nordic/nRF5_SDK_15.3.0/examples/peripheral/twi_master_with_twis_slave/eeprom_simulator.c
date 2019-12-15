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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "config.h"
#include "eeprom_simulator.h"
#include "nrf_drv_twis.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"

/**
 * @ingroup eeprom_simulator
 * @defgroup eeprom_simulator_ivars EEPROM simulator internal variables
 *
 * Internal variables required for the module to work.
 * @{
 */

    /**
     * @brief Memory array.
     *
     * Place that will simulate the memory.
     */
    static uint8_t m_memory[EEPROM_SIM_SIZE];

    /**
     * @brief Current memory address.
     *
     * Memory pointer for any operation that will be processed on memory array.
     */
    static uint16_t m_addr = 0;

    /**
     * @brief Receive buffer.
     *
     * Receiving buffer must contain the address and EEPROM_SIM_SEQ_WRITE_MAX_BYTES bytes of data.
     */
    static uint8_t m_rxbuff[EEPROM_SIM_ADDRESS_LEN_BYTES + EEPROM_SIM_SEQ_WRITE_MAX_BYTES];

    /**
     * @brief Internal error flag.
     *
     * This flag is set if any communication error is detected.
     * It can be cleared by calling the @ref eeprom_simulator_error_get function.
     */
    static bool m_error_flag;

    /**
     * @brief TWIS instance.
     *
     * TWIS driver instance used by this EEPROM simulator.
     */
    static const nrf_drv_twis_t m_twis = NRF_DRV_TWIS_INSTANCE(EEPROM_SIM_TWIS_INST);

    /**
     * @brief EEPROM simulator configuration.
     *
     * When the emulator is initialized, the configuration is stored here.
     */
    static eeprom_sim_config_t m_config;


/** @} */

/**
 * @ingroup eeprom_simulator
 * @defgroup eeprom_simulator_ifunc EEPROM simulator internal functions
 *
 * Internal auxiliary functions.
 * @{
 */
    /**
     * @brief Set the current address pointer.
     *
     * Sets address for the next operation on the memory array.
     * @param addr Address to set.
     */
    static void ees_setAddr(uint16_t addr)
    {
        m_addr = addr;
    }

    /**
     * @brief Perform a write operation on the memory array.
     *
     * Write a single byte into the memory array using @ref m_addr as a write pointer.
     * @param data Data to be written.
     */
    static void ees_write(uint8_t data)
    {
        if (m_addr >= sizeof(m_memory))
            m_addr = 0;
        m_memory[m_addr++] = data;
    }

    /**
     * @brief Start after the WRITE command.
     *
     * Function sets pointers for TWIS to receive data.
     * WRITE command does not write directly to memory array.
     * Temporary receive buffer is used.
     * @sa m_rxbuff
     */
    static void ees_writeBegin(void)
    {
        (void)nrf_drv_twis_rx_prepare(&m_twis, m_rxbuff, sizeof(m_rxbuff));
    }

    /**
     * @brief Finalize the WRITE command.
     *
     * Call this function when the write command is finished.
     * It sets a memory pointer and writes all received data to the memory array.
     */
    static void ees_writeEnd(size_t cnt)
    {
        if (cnt > (EEPROM_SIM_ADDRESS_LEN_BYTES - 1))
        {
            size_t n;
#if (EEPROM_SIM_ADDRESS_LEN_BYTES == 1)
            ees_setAddr(m_rxbuff[0]);
#endif

#if (EEPROM_SIM_ADDRESS_LEN_BYTES == 2)
            uint16_t rxbuff;

            if (TWI_ADDRESS_CONFIG == LITTLE_ENDIAN)
            {
                rxbuff = ((m_rxbuff[1] << 8 ) | (m_rxbuff[0]));
            }
            else
            {
                rxbuff = ((m_rxbuff[0] << 8 ) | (m_rxbuff[1]));
            }

            ees_setAddr(rxbuff);
#endif



            for (n=EEPROM_SIM_ADDRESS_LEN_BYTES; n<cnt; ++n)
            {
                ees_write(m_rxbuff[n]);
            }
        }
    }

    /**
     * @brief Start after the READ command.
     *
     * Function sets pointers for TWIS to transmit data from the current address to the end of memory.
     */
    static void ees_readBegin(void)
    {
        uint32_t read_len = EEPROM_SIM_SEQ_READ_MAX_BYTES;
        volatile uint16_t temp_addr = m_addr;

        if (m_addr + EEPROM_SIM_SEQ_READ_MAX_BYTES > sizeof(m_memory))
        {
            read_len = EEPROM_SIM_SIZE - m_addr;
            temp_addr = 0;
        }

        (void) nrf_drv_twis_tx_prepare(&m_twis, m_memory + m_addr, read_len);
        m_addr = temp_addr;

    }

    /**
     * @brief Finalize the READ command.
     *
     * Call this function when the read command is finished.
     * It adjusts the current m_addr pointer.
     * @param cnt Number of bytes read.
     */
    static void ees_readEnd(size_t cnt)
    {
        m_addr += cnt;
    }


    /**
     * @brief Event processing.
     *
     *
     */
    static void twis_event_handler(nrf_drv_twis_evt_t const * const p_event)
    {
        switch (p_event->type)
        {
        case TWIS_EVT_READ_REQ:
            if (p_event->data.buf_req)
            {
                ees_readBegin();
            }
            break;
        case TWIS_EVT_READ_DONE:
            ees_readEnd(p_event->data.tx_amount);
            break;
        case TWIS_EVT_WRITE_REQ:
            if (p_event->data.buf_req)
            {
                ees_writeBegin();
            }
            break;
        case TWIS_EVT_WRITE_DONE:
            ees_writeEnd(p_event->data.rx_amount);
            break;

        case TWIS_EVT_READ_ERROR:
        case TWIS_EVT_WRITE_ERROR:
        case TWIS_EVT_GENERAL_ERROR:
            m_error_flag = true;
            break;
        default:
            break;
        }
    }

/** @} */



ret_code_t eeprom_simulator_init(void)
{
    ret_code_t ret;

    m_config.slave_addr                = EEPROM_SIM_ADDR;
    m_config.scl_s                     = EEPROM_SIM_SCL_S;
    m_config.sda_s                     = EEPROM_SIM_SDA_S;
    m_config.flash_start_addr          = EEPROM_SIM_FLASH_ADDRESS;

    /* Initialize RAM with contents of flash */
    for (uint32_t n=0; n<EEPROM_SIM_SIZE; ++n)
    {
        m_memory[n] = *(uint8_t *)(m_config.flash_start_addr + n);
    }
    m_addr = 0;

    const nrf_drv_twis_config_t config =
    {
        .addr               = {m_config.slave_addr, 0},
        .scl                = m_config.scl_s,
        .scl_pull           = NRF_GPIO_PIN_PULLUP,
        .sda                = m_config.sda_s,
        .sda_pull           = NRF_GPIO_PIN_PULLUP,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    /* Init TWIS */
    do
    {
        ret = nrf_drv_twis_init(&m_twis, &config, twis_event_handler);
        if (NRF_SUCCESS != ret)
        {
            break;
        }

        nrf_drv_twis_enable(&m_twis);
    }while (0);
    return ret;
}

bool eeprom_simulator_error_check(void)
{
    return m_error_flag;
}

uint32_t eeprom_simulator_error_get_and_clear(void)
{
    uint32_t ret = nrf_drv_twis_error_get_and_clear(&m_twis);
    m_error_flag = false;
    return ret;
}






