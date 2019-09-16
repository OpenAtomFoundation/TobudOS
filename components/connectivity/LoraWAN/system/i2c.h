/*!
 * \file      i2c.h
 *
 * \brief     I2C driver implementation
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
#ifndef __I2C_H__
#define __I2C_H__

#include "gpio.h"

/*!
 * I2C peripheral ID
 */
typedef enum
{
    I2C_1,
    I2C_2,
}I2cId_t;

/*!
 * I2C object type definition
 */
typedef struct
{
    I2cId_t I2cId;
    Gpio_t Scl;
    Gpio_t Sda;
}I2c_t;

/*!
 * \brief Initializes the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 * \param [IN] scl  I2C Scl pin name to be used
 * \param [IN] sda  I2C Sda pin name to be used
 */
void I2cInit( I2c_t *obj, I2cId_t i2cId, PinNames scl, PinNames sda );

/*!
 * \brief DeInitializes the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 */
void I2cDeInit( I2c_t *obj );

/*!
 * \brief Reset the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 */
void I2cResetBus( I2c_t *obj );

/*!
 * \brief Write data to the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             data address
 * \param [IN] data             data to write
 */
uint8_t I2cWrite( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t data );

/*!
 * \brief Write data buffer to the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             data address
 * \param [IN] buffer           data buffer to write
 * \param [IN] size             number of bytes to write
 */
uint8_t I2cWriteBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size );

/*!
 * \brief Read data from the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             data address
 * \param [OUT] data            data to read
 */
uint8_t I2cRead( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *data );

/*!
 * \brief Read data buffer from the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             data address
 * \param [OUT] buffer          data buffer to read
 * \param [IN] size             number of data bytes to read
 */
uint8_t I2cReadBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size );

#endif // __I2C_H__
