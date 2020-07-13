/*!
 * \file      mma8451.h
 *
 * \brief     MMA8451 Accelerometer driver implementation
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
#include <stdbool.h>
#include "utilities.h"
#include "i2c.h"
#include "mma8451.h"

extern I2c_t I2c;

static uint8_t I2cDeviceAddr = 0;

static bool MMA8451Initialized = false;

/*!
 * \brief Writes a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MMA8451Write( uint8_t addr, uint8_t data );

/*!
 * \brief Writes a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \param [IN]: size
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MMA8451WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Reads a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MMA8451Read( uint8_t addr, uint8_t *data );

/*!
 * \brief Reads a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \param [IN]: size
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MMA8451ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Sets the I2C device slave address
 *
 * \param [IN]: addr
 */
void MMA8451SetDeviceAddr( uint8_t addr );

/*!
 * \brief Gets the I2C device slave address
 *
 * \retval: addr Current device slave address
 */
uint8_t MMA8451GetDeviceAddr( void );

uint8_t MMA8451Init( void )
{
    uint8_t regVal = 0;

    MMA8451SetDeviceAddr( MMA8451_I2C_ADDRESS );

    if( MMA8451Initialized == false )
    {
        MMA8451Initialized = true;

        MMA8451Read( MMA8451_ID, &regVal );
        if( regVal != 0x1A )   // Fixed Device ID Number = 0x1A
        {
            return FAIL;
        }
        MMA8451Reset( );

        // INT pins on this chip default to push-pull output
        // set them to open drain.
        MMA8451Write( MMA8451_CTRL_REG3, 0x01 );
        MMA8451OrientDetect( );
    }
    return SUCCESS;
}


uint8_t MMA8451Reset( )
{
    if( MMA8451Write( 0x2B, 0x40 ) == SUCCESS ) // Reset the MMA8451 with CTRL_REG2
    {
        return SUCCESS;
    }
    return FAIL;
}

uint8_t MMA8451Write( uint8_t addr, uint8_t data )
{
    return MMA8451WriteBuffer( addr, &data, 1 );
}

uint8_t MMA8451WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cWriteBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}

uint8_t MMA8451Read( uint8_t addr, uint8_t *data )
{
    return MMA8451ReadBuffer( addr, data, 1 );
}

uint8_t MMA8451ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cReadBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}

void MMA8451SetDeviceAddr( uint8_t addr )
{
    I2cDeviceAddr = addr;
}

uint8_t MMA8451GetDeviceAddr( void )
{
    return I2cDeviceAddr;
}

uint8_t MMA8451GetOrientation( void )
{
    uint8_t orientation = 0;

    MMA8451Read( MMA8451_PL_STATUS, &orientation );
    return orientation;
}

void MMA8451OrientDetect( void )
{
    uint8_t ctrlReg1 = 0;
    uint8_t tmpReg = 0;

    // Set device in standby mode
    MMA8451Read( MMA8451_CTRL_REG1, &ctrlReg1 );
    MMA8451Write( MMA8451_CTRL_REG1, ctrlReg1 & 0xFE );

    // Set the data rate to 50 Hz
    MMA8451Write( MMA8451_CTRL_REG1, ctrlReg1 | 0x20 );

    // Set enable orientation detection.
    MMA8451Read( MMA8451_PL_CFG, &tmpReg );
    MMA8451Write( MMA8451_PL_CFG, tmpReg | 0x40 );

    // Enable orientation interrupt
    MMA8451Write( MMA8451_CTRL_REG4, 0x10 );

    // Select orientation interrupt pin INT1
    MMA8451Write( MMA8451_CTRL_REG5, 0x10 );

    // Set the debounce counter 5 -> 100 ms at 50 Hz
    MMA8451Write( MMA8451_PL_COUNT, 0x05 );

    // Set device in active mode
    MMA8451Read( MMA8451_CTRL_REG1, &ctrlReg1 );
    MMA8451Write( MMA8451_CTRL_REG1, ctrlReg1 | 0x01 );
}
