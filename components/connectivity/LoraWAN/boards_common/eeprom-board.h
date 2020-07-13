/*!
 * \file      eeprom-board.h
 *
 * \brief     Target board EEPROM driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __EEPROM_BOARD_H__
#define __EEPROM_BOARD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*!
 * Writes the given buffer to the EEPROM at the specified address.
 *
 * \param[IN] addr EEPROM address to write to
 * \param[IN] buffer Pointer to the buffer to be written.
 * \param[IN] size Size of the buffer to be written.
 * \retval status [SUCCESS, FAIL]
 */
uint8_t EepromMcuWriteBuffer( uint16_t addr, uint8_t *buffer, uint16_t size );

/*!
 * Reads the EEPROM at the specified address to the given buffer.
 *
 * \param[IN] addr EEPROM address to read from
 * \param[OUT] buffer Pointer to the buffer to be written with read data.
 * \param[IN] size Size of the buffer to be read.
 * \retval status [SUCCESS, FAIL]
 */
uint8_t EepromMcuReadBuffer( uint16_t addr, uint8_t *buffer, uint16_t size );

/*!
 * Sets the device address.
 *
 * \remark Useful for I2C external EEPROMS
 *
 * \param[IN] addr External EEPROM address
 */
void EepromMcuSetDeviceAddr( uint8_t addr );

/*!
 * Gets the current device address.
 *
 * \remark Useful for I2C external EEPROMS
 *
 * \retval addr External EEPROM address
 */
uint8_t EepromMcuGetDeviceAddr( void );

#ifdef __cplusplus
}
#endif

#endif // __EEPROM_BOARD_H__
