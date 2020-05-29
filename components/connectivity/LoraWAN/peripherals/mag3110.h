/*!
 * \file      mag3110.h
 *
 * \brief     MAG3110 Magnetometer driver implementation
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
#ifndef __MAG3110_H__
#define __MAG3110_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*!
 * MAG3110 I2C address
 */
#define MAG3110_I2C_ADDRESS                             0x0E

/*!
 * MAG3110 Registers
 */
#define MAG3110_ID                                      0x07

/*!
 * \brief Initializes the device
 *
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MAG3110Init( void );

/*!
 * \brief Resets the device
 *
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MAG3110Reset( void );

/*!
 * \brief Writes a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MAG3110Write( uint8_t addr, uint8_t data );

/*!
 * \brief Writes a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \param [IN]: size
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MAG3110WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Reads a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MAG3110Read( uint8_t addr, uint8_t *data );

/*!
 * \brief Reads a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \param [IN]: size
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MAG3110ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Sets the I2C device slave address
 *
 * \param [IN]: addr
 */
void MAG3110SetDeviceAddr( uint8_t addr );

/*!
 * \brief Gets the I2C device slave address
 *
 * \retval: addr Current device slave address
 */
uint8_t MAG3110GetDeviceAddr( void );

#ifdef __cplusplus
}
#endif

#endif // __MAG3110_H__
