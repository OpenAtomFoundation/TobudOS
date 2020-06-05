/*!
 * \file      pam7q.c
 *
 * \brief     PAM7Q GPS receiver driver implementation
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
#include "utilities.h"
#include "i2c.h"
#include "pam7q.h"

extern I2c_t I2c;

static uint8_t I2cDeviceAddr = 0;

void PAM7QInit( void )
{
    PAM7QSetDeviceAddr( PAM7Q_I2C_ADDRESS );
}

bool PAM7QGetGpsData( uint8_t *nmeaString, uint8_t *nmeaStringSize, uint16_t nmeaMaxStringSize )
{
    uint8_t status;
    uint16_t pendingBytes;
    bool result = false;

    *nmeaStringSize = 0;

    status = PAM7QReadBuffer( MESSAGE_SIZE_1, nmeaString, 2 );

    if( status == SUCCESS )
    {
        // build a 16bit number
        pendingBytes = ( uint16_t )( ( nmeaString[0] << 8 ) | nmeaString[1] );

        // check for invalid length
        if( pendingBytes == 0xFFFF )
        {
            pendingBytes = 0;
        }
        // just to buffer size
        if( pendingBytes > ( nmeaMaxStringSize - 1 ) )
        {
            pendingBytes = nmeaMaxStringSize - 1;
        }

        // read pending data from GPS module
        status = PAM7QReadBuffer( PAYLOAD, nmeaString, pendingBytes );

        // make sure the string is terminated
        if( status == SUCCESS )
        {
            nmeaString[pendingBytes] = 0x00;

            // copy length indication to argument
            *nmeaStringSize = pendingBytes;

            // return success only if there is data to process
            if( pendingBytes > 0 )
            {
                result = true;
            }
        }
    }
    return result;
}

uint8_t PAM7QGetDeviceAddr( void )
{
    return I2cDeviceAddr;
}

void PAM7QSetDeviceAddr( uint8_t addr )
{
    I2cDeviceAddr = addr;
}

uint8_t PAM7QWrite( uint8_t addr, uint8_t data )
{
    return PAM7QWriteBuffer( addr, &data, 1 );
}

uint8_t PAM7QWriteBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cWriteBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}

uint8_t PAM7QRead( uint8_t addr, uint8_t *data )
{
    return PAM7QReadBuffer( addr, data, 1 );
}

uint8_t PAM7QReadBuffer( uint8_t addr, uint8_t *data, uint8_t size )
{
    return I2cReadBuffer( &I2c, I2cDeviceAddr << 1, addr, data, size );
}
