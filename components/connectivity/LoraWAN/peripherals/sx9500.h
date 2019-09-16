/*!
 * \file      sx9500.h
 *
 * \brief     SX9500 proximity sensor driver implementation
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
#ifndef __SX9500_H__
#define __SX9500_H__

#include <stdint.h>

#define SX9500_I2C_ADDRESS                          0x28

#define SX9500_REG_IRQSRC                           0x00
#define SX9500_REG_STAT                             0x01
#define SX9500_REG_IRQMSK                           0x03
#define SX9500_REG_PROXCTRL0                        0x06
#define SX9500_REG_PROXCTRL1                        0x07
#define SX9500_REG_PROXCTRL2                        0x08
#define SX9500_REG_PROXCTRL3                        0x09
#define SX9500_REG_PROXCTRL4                        0x0A
#define SX9500_REG_PROXCTRL5                        0x0B
#define SX9500_REG_PROXCTRL6                        0x0C
#define SX9500_REG_PROXCTRL7                        0x0D
#define SX9500_REG_PROXCTRL8                        0x0E
#define SX9500_REG_SENSORSEL                        0x20
#define SX9500_REG_USEMSB                           0x21
#define SX9500_REG_USELSB                           0x22
#define SX9500_REG_AVGMSB                           0x23
#define SX9500_REG_AVGLSB                           0x24
#define SX9500_REG_DIFFMSB                          0x25
#define SX9500_REG_DIFFLSB                          0x26
#define SX9500_REG_OFFSETMSB                        0x27
#define SX9500_REG_OFFSETLSB                        0x28
#define SX9500_REG_RESET                            0x7F

#define SX9500_RESET_CMD                            0xDE

uint8_t SX9500Init( void );

/*!
 * \brief Resets the device
 *
 * \retval status [OK, ERROR, UNSUPPORTED]
 */
uint8_t SX9500Reset( void );

/*!
 * \brief Writes a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \retval status [OK, ERROR, UNSUPPORTED]
 */
uint8_t SX9500Write( uint8_t addr, uint8_t data );

/*!
 * \brief Writes a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \param [IN]: size
 * \retval status [OK, ERROR, UNSUPPORTED]
 */
uint8_t SX9500WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Reads a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \retval status [OK, ERROR, UNSUPPORTED]
 */
uint8_t SX9500Read( uint8_t addr, uint8_t *data );

/*!
 * \brief Reads a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \param [IN]: size
 * \retval status [OK, ERROR, UNSUPPORTED]
 */
uint8_t SX9500ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Sets the I2C device slave address
 *
 * \param [IN]: addr
 */
void SX9500SetDeviceAddr( uint8_t addr );

/*!
 * \brief Gets the I2C device slave address
 *
 * \retval: addr Current device slave address
 */
uint8_t SX9500GetDeviceAddr( void );

/*!
 * \brief Goes into a loop until a successful capacitive proximity detection
 */
void SX9500LockUntilDetection( void );

#endif // __SX1509_H__
