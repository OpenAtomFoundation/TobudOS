/*!
 * \file      gps.c
 *
 * \brief     GPS driver implementation
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
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "utilities.h"
#include "board.h"
#include "rtc-board.h"
#include "gps-board.h"
#include "gps.h"

#define TRIGGER_GPS_CNT                             10

/* Various type of NMEA data we can receive with the Gps */
const char NmeaDataTypeGPGGA[] = "GPGGA";
const char NmeaDataTypeGPGSA[] = "GPGSA";
const char NmeaDataTypeGPGSV[] = "GPGSV";
const char NmeaDataTypeGPRMC[] = "GPRMC";

/* Value used for the conversion of the position from DMS to decimal */
const int32_t MaxNorthPosition = 8388607;       // 2^23 - 1
const int32_t MaxSouthPosition = 8388608;       // -2^23
const int32_t MaxEastPosition = 8388607;        // 2^23 - 1
const int32_t MaxWestPosition = 8388608;        // -2^23

NmeaGpsData_t NmeaGpsData;

static double HasFix = false;
static double Latitude = 0;
static double Longitude = 0;

static int32_t LatitudeBinary = 0;
static int32_t LongitudeBinary = 0;

static int16_t Altitude = ( int16_t )0xFFFF;

static uint32_t PpsCnt = 0;

bool PpsDetected = false;

void GpsPpsHandler( bool *parseData )
{
    PpsDetected = true;
    PpsCnt++;
    *parseData = false;

    if( PpsCnt >= TRIGGER_GPS_CNT )
    {
        PpsCnt = 0;
        *parseData = true;
    }
}

void GpsInit( void )
{
    PpsDetected = false;
    GpsMcuInit( );
}

void GpsStart( void )
{
    GpsMcuStart( );
}

void GpsStop( void )
{
    GpsMcuStop( );
}

void GpsProcess( void )
{
    GpsMcuProcess( );
}

bool GpsGetPpsDetectedState( void )
{
    bool state = false;

    CRITICAL_SECTION_BEGIN( );
    state = PpsDetected;
    PpsDetected = false;
    CRITICAL_SECTION_END( );
    return state;
}

bool GpsHasFix( void )
{
    return HasFix;
}

void GpsConvertPositionIntoBinary( void )
{
    long double temp;

    if( Latitude >= 0 ) // North
    {
        temp = Latitude * MaxNorthPosition;
        LatitudeBinary = temp / 90;
    }
    else                // South
    {
        temp = Latitude * MaxSouthPosition;
        LatitudeBinary = temp / 90;
    }

    if( Longitude >= 0 ) // East
    {
        temp = Longitude * MaxEastPosition;
        LongitudeBinary = temp / 180;
    }
    else                // West
    {
        temp = Longitude * MaxWestPosition;
        LongitudeBinary = temp / 180;
    }
}

void GpsConvertPositionFromStringToNumerical( void )
{
    int i;

    double valueTmp1;
    double valueTmp2;
    double valueTmp3;
    double valueTmp4;

    // Convert the latitude from ASCII to uint8_t values
    for( i = 0 ; i < 10 ; i++ )
    {
        NmeaGpsData.NmeaLatitude[i] = NmeaGpsData.NmeaLatitude[i] & 0xF;
    }
    // Convert latitude from degree/minute/second (DMS) format into decimal
    valueTmp1 = ( double )NmeaGpsData.NmeaLatitude[0] * 10.0 + ( double )NmeaGpsData.NmeaLatitude[1];
    valueTmp2 = ( double )NmeaGpsData.NmeaLatitude[2] * 10.0 + ( double )NmeaGpsData.NmeaLatitude[3];
    valueTmp3 = ( double )NmeaGpsData.NmeaLatitude[5] * 1000.0 + ( double )NmeaGpsData.NmeaLatitude[6] * 100.0 +
                ( double )NmeaGpsData.NmeaLatitude[7] * 10.0 + ( double )NmeaGpsData.NmeaLatitude[8];

    Latitude = valueTmp1 + ( ( valueTmp2 + ( valueTmp3 * 0.0001 ) ) / 60.0 );

    if( NmeaGpsData.NmeaLatitudePole[0] == 'S' )
    {
        Latitude *= -1;
    }

    // Convert the longitude from ASCII to uint8_t values
    for( i = 0 ; i < 10 ; i++ )
    {
        NmeaGpsData.NmeaLongitude[i] = NmeaGpsData.NmeaLongitude[i] & 0xF;
    }
    // Convert longitude from degree/minute/second (DMS) format into decimal
    valueTmp1 = ( double )NmeaGpsData.NmeaLongitude[0] * 100.0 + ( double )NmeaGpsData.NmeaLongitude[1] * 10.0 + ( double )NmeaGpsData.NmeaLongitude[2];
    valueTmp2 = ( double )NmeaGpsData.NmeaLongitude[3] * 10.0 + ( double )NmeaGpsData.NmeaLongitude[4];
    valueTmp3 = ( double )NmeaGpsData.NmeaLongitude[6] * 1000.0 + ( double )NmeaGpsData.NmeaLongitude[7] * 100;
    valueTmp4 = ( double )NmeaGpsData.NmeaLongitude[8] * 10.0 + ( double )NmeaGpsData.NmeaLongitude[9];

    Longitude = valueTmp1 + ( valueTmp2 / 60.0 ) + ( ( ( valueTmp3 + valueTmp4 ) * 0.0001 ) / 60.0 );

    if( NmeaGpsData.NmeaLongitudePole[0] == 'W' )
    {
        Longitude *= -1;
    }
}


uint8_t GpsGetLatestGpsPositionDouble( double *lati, double *longi )
{
    uint8_t status = FAIL;
    if( HasFix == true )
    {
        status = SUCCESS;
    }
    else
    {
        GpsResetPosition( );
    }
    *lati = Latitude;
    *longi = Longitude;
    return status;
}

uint8_t GpsGetLatestGpsPositionBinary( int32_t *latiBin, int32_t *longiBin )
{
    uint8_t status = FAIL;

    CRITICAL_SECTION_BEGIN( );
    if( HasFix == true )
    {
        status = SUCCESS;
    }
    else
    {
        GpsResetPosition( );
    }
    *latiBin = LatitudeBinary;
    *longiBin = LongitudeBinary;
    CRITICAL_SECTION_END( );
    return status;
}

int16_t GpsGetLatestGpsAltitude( void )
{
    CRITICAL_SECTION_BEGIN( );
    if( HasFix == true )
    {
        Altitude = atoi( NmeaGpsData.NmeaAltitude );
    }
    else
    {
        Altitude = ( int16_t )0xFFFF;
    }
    CRITICAL_SECTION_END( );

    return Altitude;
}

/*!
 * Calculates the checksum for a NMEA sentence
 *
 * Skip the first '$' if necessary and calculate checksum until '*' character is
 * reached (or buffSize exceeded).
 *
 * \retval chkPosIdx Position of the checksum in the sentence
 */
int32_t GpsNmeaChecksum( int8_t *nmeaStr, int32_t nmeaStrSize, int8_t * checksum )
{
    int i = 0;
    uint8_t checkNum = 0;

    // Check input parameters
    if( ( nmeaStr == NULL ) || ( checksum == NULL ) || ( nmeaStrSize <= 1 ) )
    {
        return -1;
    }

    // Skip the first '$' if necessary
    if( nmeaStr[i] == '$' )
    {
        i += 1;
    }

    // XOR until '*' or max length is reached
    while( nmeaStr[i] != '*' )
    {
        checkNum ^= nmeaStr[i];
        i += 1;
        if( i >= nmeaStrSize )
        {
            return -1;
        }
    }

    // Convert checksum value to 2 hexadecimal characters
    checksum[0] = Nibble2HexChar( checkNum / 16 ); // upper nibble
    checksum[1] = Nibble2HexChar( checkNum % 16 ); // lower nibble

    return i + 1;
}

/*!
 * Calculate the checksum of a NMEA frame and compare it to the checksum that is
 * present at the end of it.
 * Return true if it matches
 */
static bool GpsNmeaValidateChecksum( int8_t *serialBuff, int32_t buffSize )
{
    int32_t checksumIndex;
    int8_t checksum[2]; // 2 characters to calculate NMEA checksum

    checksumIndex = GpsNmeaChecksum( serialBuff, buffSize, checksum );

    // could we calculate a verification checksum ?
    if( checksumIndex < 0 )
    {
        return false;
    }

    // check if there are enough char in the serial buffer to read checksum
    if( checksumIndex >= ( buffSize - 2 ) )
    {
        return false;
    }

    // check the checksum
    if( ( serialBuff[checksumIndex] == checksum[0] ) && ( serialBuff[checksumIndex + 1] == checksum[1] ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t GpsParseGpsData( int8_t *rxBuffer, int32_t rxBufferSize )
{
    uint8_t i = 1;
    uint8_t j = 0;
    uint8_t fieldSize = 0;

    if( rxBuffer[0] != '$' )
    {
        GpsMcuInvertPpsTrigger( );
        return FAIL;
    }

    if( GpsNmeaValidateChecksum( rxBuffer, rxBufferSize ) == false )
    {
        return FAIL;
    }

    fieldSize = 0;
    while( rxBuffer[i + fieldSize++] != ',' )
    {
        if( fieldSize > 6 )
        {
            return FAIL;
        }
    }
    for( j = 0; j < fieldSize; j++, i++ )
    {
        NmeaGpsData.NmeaDataType[j] = rxBuffer[i];
    }
    // Parse the GPGGA data
    if( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPGGA, 5 ) == 0 )
    {
        // NmeaUtcTime
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 11 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaUtcTime[j] = rxBuffer[i];
        }
        // NmeaLatitude
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 10 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLatitude[j] = rxBuffer[i];
        }
        // NmeaLatitudePole
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLatitudePole[j] = rxBuffer[i];
        }
        // NmeaLongitude
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 11 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLongitude[j] = rxBuffer[i];
        }
        // NmeaLongitudePole
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLongitudePole[j] = rxBuffer[i];
        }
        // NmeaFixQuality
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaFixQuality[j] = rxBuffer[i];
        }
        // NmeaSatelliteTracked
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 3 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaSatelliteTracked[j] = rxBuffer[i];
        }
        // NmeaHorizontalDilution
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 6 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaHorizontalDilution[j] = rxBuffer[i];
        }
        // NmeaAltitude
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 8 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaAltitude[j] = rxBuffer[i];
        }
        // NmeaAltitudeUnit
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaAltitudeUnit[j] = rxBuffer[i];
        }
        // NmeaHeightGeoid
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 8 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaHeightGeoid[j] = rxBuffer[i];
        }
        // NmeaHeightGeoidUnit
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaHeightGeoidUnit[j] = rxBuffer[i];
        }

        GpsFormatGpsData( );
        return SUCCESS;
    }
    else if ( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPRMC, 5 ) == 0 )
    {
        // NmeaUtcTime
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 11 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaUtcTime[j] = rxBuffer[i];
        }
        // NmeaDataStatus
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaDataStatus[j] = rxBuffer[i];
        }
        // NmeaLatitude
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 10 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLatitude[j] = rxBuffer[i];
        }
        // NmeaLatitudePole
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLatitudePole[j] = rxBuffer[i];
        }
        // NmeaLongitude
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 11 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLongitude[j] = rxBuffer[i];
        }
        // NmeaLongitudePole
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 2 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaLongitudePole[j] = rxBuffer[i];
        }
        // NmeaSpeed
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 8 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaSpeed[j] = rxBuffer[i];
        }
        // NmeaDetectionAngle
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 8 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaDetectionAngle[j] = rxBuffer[i];
        }
        // NmeaDate
        fieldSize = 0;
        while( rxBuffer[i + fieldSize++] != ',' )
        {
            if( fieldSize > 8 )
            {
                return FAIL;
            }
        }
        for( j = 0; j < fieldSize; j++, i++ )
        {
            NmeaGpsData.NmeaDate[j] = rxBuffer[i];
        }

        GpsFormatGpsData( );
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}

void GpsFormatGpsData( void )
{
    if( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPGGA, 5 ) == 0 )
    {
        HasFix = ( NmeaGpsData.NmeaFixQuality[0] > 0x30 ) ? true : false;
    }
    else if ( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPRMC, 5 ) == 0 )
    {
        HasFix = ( NmeaGpsData.NmeaDataStatus[0] == 0x41 ) ? true : false;
    }
    GpsConvertPositionFromStringToNumerical( );
    GpsConvertPositionIntoBinary( );
}

void GpsResetPosition( void )
{
    Altitude = ( int16_t )0xFFFF;
    Latitude = 0;
    Longitude = 0;
    LatitudeBinary = 0;
    LongitudeBinary = 0;
}
