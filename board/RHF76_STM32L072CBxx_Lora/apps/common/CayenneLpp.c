/*!
 * \file      CayenneLpp.c
 *
 * \brief     Implements the Cayenne Low Power Protocol
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
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#include <stdint.h>

#include "utilities.h"
#include "CayenneLpp.h"

#define CAYENNE_LPP_MAXBUFFER_SIZE                  242

static uint8_t CayenneLppBuffer[CAYENNE_LPP_MAXBUFFER_SIZE];
static uint8_t CayenneLppCursor = 0;

void CayenneLppInit( void )
{
    CayenneLppCursor = 0;
}

void CayenneLppReset( void )
{
    CayenneLppCursor = 0;
}

uint8_t CayenneLppGetSize( void )
{
    return CayenneLppCursor;
}

uint8_t* CayenneLppGetBuffer( void )
{
    return CayenneLppBuffer;
}

uint8_t CayenneLppCopy( uint8_t* dst )
{
    memcpy1( dst, CayenneLppBuffer, CayenneLppCursor );

    return CayenneLppCursor;
}


uint8_t CayenneLppAddDigitalInput( uint8_t channel, uint8_t value )
{
    if( ( CayenneLppCursor + LPP_DIGITAL_INPUT_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_DIGITAL_INPUT; 
    CayenneLppBuffer[CayenneLppCursor++] = value; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddDigitalOutput( uint8_t channel, uint8_t value )
{
    if( ( CayenneLppCursor + LPP_DIGITAL_OUTPUT_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_DIGITAL_OUTPUT; 
    CayenneLppBuffer[CayenneLppCursor++] = value; 

    return CayenneLppCursor;
}


uint8_t CayenneLppAddAnalogInput( uint8_t channel, float value )
{
    if( ( CayenneLppCursor + LPP_ANALOG_INPUT_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }

    int16_t val = value * 100;
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_ANALOG_INPUT; 
    CayenneLppBuffer[CayenneLppCursor++] = val >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = val; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddAnalogOutput( uint8_t channel, float value )
{
    if( ( CayenneLppCursor + LPP_ANALOG_OUTPUT_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    int16_t val = value * 100;
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_ANALOG_OUTPUT;
    CayenneLppBuffer[CayenneLppCursor++] = val >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = val; 

    return CayenneLppCursor;
}


uint8_t CayenneLppAddLuminosity( uint8_t channel, uint16_t lux )
{
    if( ( CayenneLppCursor + LPP_LUMINOSITY_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_LUMINOSITY; 
    CayenneLppBuffer[CayenneLppCursor++] = lux >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = lux; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddPresence( uint8_t channel, uint8_t value )
{
    if( ( CayenneLppCursor + LPP_PRESENCE_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_PRESENCE; 
    CayenneLppBuffer[CayenneLppCursor++] = value; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddTemperature( uint8_t channel, float celsius )
{
    if( ( CayenneLppCursor + LPP_TEMPERATURE_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    int16_t val = celsius * 10;
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_TEMPERATURE; 
    CayenneLppBuffer[CayenneLppCursor++] = val >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = val; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddRelativeHumidity( uint8_t channel, float rh )
{
    if( ( CayenneLppCursor + LPP_RELATIVE_HUMIDITY_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_RELATIVE_HUMIDITY; 
    CayenneLppBuffer[CayenneLppCursor++] = rh * 2; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddAccelerometer( uint8_t channel, float x, float y, float z )
{
    if( ( CayenneLppCursor + LPP_ACCELEROMETER_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    int16_t vx = x * 1000;
    int16_t vy = y * 1000;
    int16_t vz = z * 1000;

    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_ACCELEROMETER; 
    CayenneLppBuffer[CayenneLppCursor++] = vx >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = vx; 
    CayenneLppBuffer[CayenneLppCursor++] = vy >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = vy; 
    CayenneLppBuffer[CayenneLppCursor++] = vz >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = vz; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddBarometricPressure( uint8_t channel, float hpa )
{
    if( ( CayenneLppCursor + LPP_BAROMETRIC_PRESSURE_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    int16_t val = hpa * 10;

    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_BAROMETRIC_PRESSURE; 
    CayenneLppBuffer[CayenneLppCursor++] = val >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = val; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddGyrometer( uint8_t channel, float x, float y, float z )
{
    if( ( CayenneLppCursor + LPP_GYROMETER_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    int16_t vx = x * 100;
    int16_t vy = y * 100;
    int16_t vz = z * 100;

    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_GYROMETER; 
    CayenneLppBuffer[CayenneLppCursor++] = vx >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = vx; 
    CayenneLppBuffer[CayenneLppCursor++] = vy >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = vy; 
    CayenneLppBuffer[CayenneLppCursor++] = vz >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = vz; 

    return CayenneLppCursor;
}

uint8_t CayenneLppAddGps( uint8_t channel, float latitude, float longitude, float meters )
{
    if( ( CayenneLppCursor + LPP_GPS_SIZE ) > CAYENNE_LPP_MAXBUFFER_SIZE )
    {
        return 0;
    }
    int32_t lat = latitude * 10000;
    int32_t lon = longitude * 10000;
    int32_t alt = meters * 100;

    CayenneLppBuffer[CayenneLppCursor++] = channel; 
    CayenneLppBuffer[CayenneLppCursor++] = LPP_GPS; 

    CayenneLppBuffer[CayenneLppCursor++] = lat >> 16; 
    CayenneLppBuffer[CayenneLppCursor++] = lat >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = lat; 
    CayenneLppBuffer[CayenneLppCursor++] = lon >> 16; 
    CayenneLppBuffer[CayenneLppCursor++] = lon >> 8; 
    CayenneLppBuffer[CayenneLppCursor++] = lon; 
    CayenneLppBuffer[CayenneLppCursor++] = alt >> 16; 
    CayenneLppBuffer[CayenneLppCursor++] = alt >> 8;
    CayenneLppBuffer[CayenneLppCursor++] = alt;

    return CayenneLppCursor;
}
