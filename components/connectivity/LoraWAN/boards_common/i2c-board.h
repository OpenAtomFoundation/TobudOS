/*!
 * \file      i2c-board.h
 *
 * \brief     Target board I2C driver implementation
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
#ifndef __I2C_BOARD_H__
#define __I2C_BOARD_H__

#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"

/*!
 * Operation Mode for the I2C
 */
typedef enum
{
    MODE_I2C = 0,
    MODE_SMBUS_DEVICE,
    MODE_SMBUS_HOST
}I2cMode;

/*!
 * I2C signal duty cycle
 */
typedef enum
{
    I2C_DUTY_CYCLE_2 = 0,
    I2C_DUTY_CYCLE_16_9
}I2cDutyCycle;

/*!
 * I2C select if the acknowledge in after the 7th or 10th bit
 */
typedef enum
{
    I2C_ACK_ADD_7_BIT = 0,
    I2C_ACK_ADD_10_BIT
}I2cAckAddrMode;

/*!
 * Internal device address size
 */
typedef enum
{
    I2C_ADDR_SIZE_8 = 0,
    I2C_ADDR_SIZE_16,
}I2cAddrSize;

/*!
 * \brief Initializes the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 * \param [IN] scl  I2C Scl pin name to be used
 * \param [IN] sda  I2C Sda pin name to be used
 */
void I2cMcuInit( I2c_t *obj, I2cId_t i2cId, PinNames scl, PinNames sda );

/*!
 * \brief Initializes the I2C object and MCU peripheral
 *
 * \param [IN] obj              I2C object
 * \param [IN] mode             Mode of operation for the I2C Bus
 * \param [IN] dutyCycle        Signal duty cycle
 * \param [IN] I2cAckEnable     Enable or Disable to ack
 * \param [IN] AckAddrMode      7bit or 10 bit addressing
 * \param [IN] I2cFrequency     I2C bus clock frequency
 */
void I2cMcuFormat( I2c_t *obj, I2cMode mode, I2cDutyCycle dutyCycle, bool I2cAckEnable, I2cAckAddrMode AckAddrMode, uint32_t I2cFrequency );

/*!
 * \brief DeInitializes the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 */
void I2cMcuDeInit( I2c_t *obj );

/*!
 * \brief Reset the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 */
void I2cMcuResetBus( I2c_t *obj );

/*!
 * \brief Write data buffer to the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             data address
 * \param [IN] buffer           data buffer to write
 * \param [IN] size             number of data bytes to write
 */
uint8_t I2cMcuWriteBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size );

/*!
 * \brief Read data buffer from the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             data address
 * \param [IN] buffer           data buffer to read
 * \param [IN] size             number of data bytes to read
 */
uint8_t I2cMcuReadBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer, uint16_t size );

/*!
 * \brief Waits until the given device is in standby mode
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 */
uint8_t I2cMcuWaitStandbyState( I2c_t *obj, uint8_t deviceAddr );

/*!
 * \brief Sets the internal device address size
 *
 * \param [IN] obj              I2C object
 * \param [IN] addrSize         Internal address size
 */
void I2cSetAddrSize( I2c_t *obj, I2cAddrSize addrSize );

#endif // __I2C_BOARD_H__
