/*!
 * \file      sx1509.c
 *
 * \brief     SX1509 IO expander driver implementation
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
#include "sx1509.h"

extern I2c_t I2c;

static uint8_t I2cDeviceAddr = 0;

static bool SX1509Initialized = false;

void SX1509Init( void )
{
    if( SX1509Initialized == false )
    {
        SX1509SetDeviceAddr( SX1509_I2C_ADDRESS );
        SX1509Initialized = true;

        SX1509Reset( );
    }
}

uint8_t SX1509Reset( )
{
    if( SX1509Write( RegReset, 0x12 ) == SUCCESS )
    {
        if( SX1509Write( RegReset, 0x34 ) == SUCCESS )
        {
            return SUCCESS;
        }
    }
    return FAIL;
}

uint8_t SX1509Write( uint8_t addr, uint8_t data )
{
    return SX1509WriteBuffer( addr, &data, 1 );
}

uint8_t SX1509WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cWriteBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}

uint8_t SX1509Read( uint8_t addr, uint8_t *data )
{
    return SX1509ReadBuffer( addr, data, 1 );
}

uint8_t SX1509ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cReadBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}

void SX1509SetDeviceAddr( uint8_t addr )
{
    I2cDeviceAddr = addr;
}

uint8_t SX1509GetDeviceAddr( void )
{
    return I2cDeviceAddr;
}
