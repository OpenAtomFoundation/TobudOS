/*!
 * \file      sx9500.c
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
#include <stdbool.h>
#include "utilities.h"
#include "i2c.h"
#include "sx9500.h"

extern I2c_t I2c;

static uint8_t I2cDeviceAddr = 0;

static bool SX9500Initialized = false;

uint8_t SX9500Init( void )
{
    uint8_t regVal = 0;

    SX9500SetDeviceAddr( SX9500_I2C_ADDRESS );

    if( SX9500Initialized == false )
    {
        SX9500Initialized = true;

        SX9500Read( SX9500_REG_PROXCTRL0, &regVal );
        if( regVal != 0x0F )
        {
            return FAIL;
        }

        SX9500Reset( );
    }
    return SUCCESS;
}

uint8_t SX9500Reset( )
{
    if( SX9500Write( SX9500_REG_RESET, SX9500_RESET_CMD ) == SUCCESS )
    {
        return SUCCESS;
    }
    return FAIL;
}

uint8_t SX9500Write( uint8_t addr, uint8_t data )
{
    return SX9500WriteBuffer( addr, &data, 1 );
}

uint8_t SX9500WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cWriteBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}

uint8_t SX9500Read( uint8_t addr, uint8_t *data )
{
    return SX9500ReadBuffer( addr, data, 1 );
}

uint8_t SX9500ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cReadBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}

void SX9500SetDeviceAddr( uint8_t addr )
{
    I2cDeviceAddr = addr;
}

uint8_t SX9500GetDeviceAddr( void )
{
    return I2cDeviceAddr;
}

void SX9500LockUntilDetection( void )
{
    uint8_t val = 0;

    SX9500Write( SX9500_REG_RESET, SX9500_RESET_CMD );
    SX9500Read( SX9500_REG_IRQSRC, &val );
    SX9500Read( SX9500_REG_STAT, &val );

    SX9500Write( SX9500_REG_PROXCTRL0, 0x0F );
    SX9500Write( SX9500_REG_PROXCTRL1, 0x43 );
    SX9500Write( SX9500_REG_PROXCTRL2, 0x77 );
    SX9500Write( SX9500_REG_PROXCTRL3, 0x01 );
    SX9500Write( SX9500_REG_PROXCTRL4, 0x30 );
    SX9500Write( SX9500_REG_PROXCTRL5, 0x0F );
    SX9500Write( SX9500_REG_PROXCTRL6, 0x04 );
    SX9500Write( SX9500_REG_PROXCTRL7, 0x40 );
    SX9500Write( SX9500_REG_PROXCTRL8, 0x00 );
    SX9500Write( SX9500_REG_IRQMSK, 0x60 );

    val = 0;

    while( ( val & 0xF0 ) == 0x00 )
    {
        SX9500Read( SX9500_REG_STAT, &val );
    }

    SX9500Read( SX9500_REG_STAT, &val );
    SX9500Read( SX9500_REG_IRQSRC, &val );
}
