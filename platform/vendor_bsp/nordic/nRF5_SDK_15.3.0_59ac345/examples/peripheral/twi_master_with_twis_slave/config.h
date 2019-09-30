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
/**
 * @ingroup twi_master_with_twis_slave_example
 * @defgroup twi_master_with_twis_slave_example_config Example code configuration
 *
 * Configuration for the code presenting TWIS and TWI functionality.
 * @{
 */

#ifndef TWI_MASTER_WITH_TWIS_SLAVE_CONFIG_H__
#define TWI_MASTER_WITH_TWIS_SLAVE_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EEPROM_SIM_SIZE                   (320u) //!< Simulated EEPROM size.

/* Maximum number of bytes writable to this slave emulator in one sequential access including
 * address of the slave memory. Maximum allowed is 255.
 * Note that separate RAM is allocated for the data to be written to slave.
 */
#define EEPROM_SIM_SEQ_WRITE_MAX_BYTES    200

/* Maximum number of bytes writable to this slave emulator in one sequential access.
   Maximum allowed is 255.
 */
#define EEPROM_SIM_SEQ_READ_MAX_BYTES     200     //<! Number of data bytes transfer in single request

#define EEPROM_SIM_ADDR                   0x50    //!< Simulated EEPROM TWI slave address.


#define EEPROM_SIM_SCL_S         31   //!< Slave SCL pin.
#define EEPROM_SIM_SDA_S         30   //!< Slave SDA pin.

#define EEPROM_SIM_TWIS_INST     1    //!< TWIS interface used by EEPROM simulator.

/* Flash start address to load the RAM with at startup */
#define EEPROM_SIM_FLASH_ADDRESS  0x8000

/* Slave memory addressing byte length */
#define EEPROM_SIM_ADDRESS_LEN_BYTES    2

/* if EEPROM_SIM_ADDRESS_LEN_BYTES == 2, below will configure which byte is sent first by master */
/**
 * @enum address_byte_endian
 * @brief Endianness of the address byte that is received from master.
 */
typedef enum
{
    /*lint -save -e30*/
    BIG_ENDIAN = 0,   /**< MSB is sent first by master for address. */
    LITTLE_ENDIAN,    /**< LSB is sent first by master for address. */
} address_byte_endian;

#define TWI_ADDRESS_CONFIG    LITTLE_ENDIAN

/* Master Configuration */
#define MASTER_TWI_INST     0       //!< TWI interface used as a master accessing EEPROM memory.
#define UART_TX_BUF_SIZE    1024    //!< UART TX buffer size.
#define UART_RX_BUF_SIZE    32      //!< UART RX buffer size.
#define TWI_SCL_M           3       //!< Master SCL pin.
#define TWI_SDA_M           4       //!< Master SDA pin.
#define IN_LINE_PRINT_CNT   (16u)   //!< Number of data bytes printed in a single line.


/** @} */

#ifdef __cplusplus
}
#endif

#endif // TWI_MASTER_WITH_TWIS_SLAVE_CONFIG_H__
