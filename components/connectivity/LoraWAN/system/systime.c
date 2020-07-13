/*!
 * \file      systime.c
 *
 * \brief     System time functions implementation.
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
 *              (C)2013-2018 Semtech - STMicroelectronics
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    MCD Application Team ( STMicroelectronics International )
 */
#include <stdio.h>
#include "rtc-board.h"
#include "systime.h"

#define END_OF_FEBRUARY_LEAP                         60 //31+29
#define END_OF_JULY_LEAP                            213 //31+29+...

#define END_OF_FEBRUARY_NORM                         59 //31+28
#define END_OF_JULY_NORM                            212 //31+28+...

#define UNIX_YEAR                                    68 //1968 is leap year

//UNIX time 0 = start at 01:00:00, 01/01/1970
#define UNIX_HOUR_OFFSET                            ( ( TM_DAYS_IN_LEAP_YEAR + TM_DAYS_IN_YEAR ) * TM_SECONDS_IN_1DAY )

/*!
 * \brief Correction factors
 */
#define  DAYS_IN_MONTH_CORRECTION_NORM              ( (uint32_t )0x99AAA0 )
#define  DAYS_IN_MONTH_CORRECTION_LEAP              ( (uint32_t )0x445550 )


/* 365.25 = (366 + 365 + 365 + 365)/4 */
#define DIV_365_25( X )                               ( ( ( X ) * 91867 + 22750 ) >> 25 )

#define DIV_APPROX_86400( X )                       ( ( ( X ) >> 18 ) + ( ( X ) >> 17 ) )

#define DIV_APPROX_1000( X )                        ( ( ( X ) >> 10 ) +( ( X ) >> 16 ) + ( ( X ) >> 17 ) )

#define DIV_APPROX_60( X )                          ( ( ( X ) * 17476 ) >> 20 )

#define DIV_APPROX_61( X )                          ( ( ( X ) * 68759 ) >> 22 )

#define MODULO_7( X )                               ( ( X ) -( ( ( ( ( X ) + 1 ) * 299593 ) >> 21 ) * 7 ) )

/*!
 * \brief Calculates ceiling( X / N )
 */
#define DIVC( X, N )                                ( ( ( X ) + ( N ) -1 ) / ( N ) )

#define DIVC_BY_4( X )                              ( ( ( X ) + 3 ) >>2 )

#define DIVC_BY_2( X )                              ( ( ( X ) + 1 ) >> 1 )

static uint32_t CalendarGetMonth( uint32_t days, uint32_t year );
static void CalendarDiv86400( uint32_t in, uint32_t* out, uint32_t* remainder );
static uint32_t CalendarDiv61( uint32_t in );
static void CalendarDiv60( uint32_t in, uint32_t* out, uint32_t* remainder );

const char *WeekDayString[]={ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

SysTime_t SysTimeAdd( SysTime_t a, SysTime_t b )
{
    SysTime_t c =  { .Seconds = 0, .SubSeconds = 0 };

    c.Seconds = a.Seconds + b.Seconds;
    c.SubSeconds = a.SubSeconds + b.SubSeconds;
    if( c.SubSeconds >= 1000 )
    {
        c.Seconds++;
        c.SubSeconds -= 1000;
    }
    return c;
}

SysTime_t SysTimeSub( SysTime_t a, SysTime_t b )
{
    SysTime_t c = { .Seconds = 0, .SubSeconds = 0 };

    c.Seconds = a.Seconds - b.Seconds;
    c.SubSeconds = a.SubSeconds - b.SubSeconds;
    if( c.SubSeconds < 0 )
    {
        c.Seconds--;
        c.SubSeconds += 1000;
    }
    return c;
}

void SysTimeSet( SysTime_t sysTime )
{
    SysTime_t deltaTime;
  
    SysTime_t calendarTime = { .Seconds = 0, .SubSeconds = 0 };

    calendarTime.Seconds = RtcGetCalendarTime( ( uint16_t* )&calendarTime.SubSeconds );

    // sysTime is epoch
    deltaTime = SysTimeSub( sysTime, calendarTime );

    RtcBkupWrite( deltaTime.Seconds, ( uint32_t )deltaTime.SubSeconds );
}

SysTime_t SysTimeGet( void )
{
    SysTime_t calendarTime = { .Seconds = 0, .SubSeconds = 0 };
    SysTime_t sysTime = { .Seconds = 0, .SubSeconds = 0 };
    SysTime_t deltaTime;

    calendarTime.Seconds = RtcGetCalendarTime( ( uint16_t* )&calendarTime.SubSeconds );

    RtcBkupRead( &deltaTime.Seconds, ( uint32_t* )&deltaTime.SubSeconds );

    sysTime = SysTimeAdd( deltaTime, calendarTime );

    return sysTime;
}

SysTime_t SysTimeGetMcuTime( void )
{
    SysTime_t calendarTime = { .Seconds = 0, .SubSeconds = 0 };

    calendarTime.Seconds = RtcGetCalendarTime( ( uint16_t* )&calendarTime.SubSeconds );

    return calendarTime;
}

uint32_t SysTimeToMs( SysTime_t sysTime )
{
    SysTime_t deltaTime;
    RtcBkupRead( &deltaTime.Seconds, ( uint32_t* )&deltaTime.SubSeconds );
    SysTime_t calendarTime = SysTimeSub( sysTime, deltaTime );
    return calendarTime.Seconds * 1000 + calendarTime.SubSeconds;
}

SysTime_t SysTimeFromMs( uint32_t timeMs )
{
    uint32_t seconds = timeMs / 1000;
    SysTime_t sysTime = { .Seconds = seconds, .SubSeconds =  timeMs - seconds * 1000 };
    SysTime_t deltaTime = { 0 };
    RtcBkupRead( &deltaTime.Seconds, ( uint32_t* )&deltaTime.SubSeconds );

    return SysTimeAdd( sysTime, deltaTime );
}

uint32_t SysTimeMkTime( const struct tm* localtime )
{
    uint32_t nbdays;
    uint32_t nbsecs;
    uint32_t year = localtime->tm_year - UNIX_YEAR;
    uint32_t correctionMonth[4] =
    {
        DAYS_IN_MONTH_CORRECTION_LEAP,
        DAYS_IN_MONTH_CORRECTION_NORM,
        DAYS_IN_MONTH_CORRECTION_NORM,
        DAYS_IN_MONTH_CORRECTION_NORM
    };

    nbdays = DIVC( ( TM_DAYS_IN_YEAR * 3 + TM_DAYS_IN_LEAP_YEAR ) * year, 4 );

    nbdays += ( DIVC_BY_2( ( localtime->tm_mon ) * ( 30 + 31 ) ) -
                ( ( ( correctionMonth[year % 4] >> ( ( localtime->tm_mon ) * 2 ) ) & 0x03 ) ) );

    nbdays += ( localtime->tm_mday - 1 );

    // Convert from days to seconds
    nbsecs = nbdays * TM_SECONDS_IN_1DAY;

    nbsecs += ( ( uint32_t )localtime->tm_sec + 
                ( ( uint32_t )localtime->tm_min * TM_SECONDS_IN_1MINUTE ) +
                ( ( uint32_t )localtime->tm_hour * TM_SECONDS_IN_1HOUR ) );
    return nbsecs - UNIX_HOUR_OFFSET;
}



void SysTimeLocalTime( const uint32_t timestamp, struct tm *localtime )
{
    uint32_t correctionMonth[4] =
    {
        DAYS_IN_MONTH_CORRECTION_LEAP,
        DAYS_IN_MONTH_CORRECTION_NORM,
        DAYS_IN_MONTH_CORRECTION_NORM,
        DAYS_IN_MONTH_CORRECTION_NORM
    };
    uint32_t weekDays = 1; // Monday 1st January 1968
    uint32_t seconds;
    uint32_t minutes;
    uint32_t days;
    uint32_t divOut;
    uint32_t divReminder;

    CalendarDiv86400( timestamp + UNIX_HOUR_OFFSET, &days, &seconds );

    // Calculates seconds
    CalendarDiv60( seconds, &minutes, &divReminder );
    localtime->tm_sec = ( uint8_t )divReminder;

    // Calculates minutes and hours
    CalendarDiv60( minutes, &divOut, &divReminder);
    localtime->tm_min = ( uint8_t )divReminder;
    localtime->tm_hour = ( uint8_t )divOut;

    // Calculates year
    localtime->tm_year = DIV_365_25( days );
    days-= DIVC_BY_4( ( TM_DAYS_IN_YEAR * 3 + TM_DAYS_IN_LEAP_YEAR ) * localtime->tm_year );

    localtime->tm_yday = days;

    // Calculates month
    localtime->tm_mon = CalendarGetMonth( days, localtime->tm_year );

    // calculates weekdays
    weekDays += DIVC_BY_4( ( localtime->tm_year * 5 ) );
    weekDays += days;
    localtime->tm_wday = MODULO_7( weekDays );

    days -= ( DIVC_BY_2( ( localtime->tm_mon ) * ( 30 + 31 ) ) -
              ( ( ( correctionMonth[localtime->tm_year % 4] >> ( ( localtime->tm_mon ) * 2 ) ) & 0x03 ) ) );

    // Convert 0 to 1 indexed.
    localtime->tm_mday = days + 1;

    localtime->tm_year += UNIX_YEAR;

    localtime->tm_isdst = -1;
}

static uint32_t CalendarGetMonth( uint32_t days, uint32_t year )
{
    uint32_t month;
    if( ( year % 4 ) == 0 )
    {   /*leap year*/
        if( days < END_OF_FEBRUARY_LEAP )
        {   // January or February
            // month =  days * 2 / ( 30 + 31 );
            month = CalendarDiv61( days * 2 );
        }
        else if( days < END_OF_JULY_LEAP )
        {
            month = CalendarDiv61( ( days - END_OF_FEBRUARY_LEAP ) * 2 ) + 2;
        }
        else
        {
            month = CalendarDiv61( ( days - END_OF_JULY_LEAP ) * 2 ) + 7;
        }
    }
    else
    {
        if( days < END_OF_FEBRUARY_NORM )
        {   // January or February
            month = CalendarDiv61( days * 2 );
        }
        else if( days < END_OF_JULY_NORM )
        {
            month = CalendarDiv61( ( days - END_OF_FEBRUARY_NORM ) * 2 ) + 2;
        }
        else
        {
            month = CalendarDiv61( ( days - END_OF_JULY_NORM ) * 2 ) + 7;
        }
    }
    return month;
}

static void CalendarDiv86400( uint32_t in, uint32_t* out, uint32_t* remainder )
{
#if 0
    *remainder = in % SECONDS_IN_1DAY;
    *out       = in / SECONDS_IN_1DAY;
#else
    uint32_t outTemp = 0;
    uint32_t divResult = DIV_APPROX_86400( in );

    while( divResult >=1 )
    {
        outTemp += divResult;
        in -= divResult * 86400;
        divResult= DIV_APPROX_86400( in );
    }
    if( in >= 86400 )
    {
        outTemp += 1;
        in -= 86400;
    }

    *remainder = in;
    *out = outTemp;
#endif
}

static uint32_t CalendarDiv61( uint32_t in )
{
#if 0
    return( in / 61 );
#else
    uint32_t outTemp = 0;
    uint32_t divResult = DIV_APPROX_61( in );
    while( divResult >=1 )
    {
        outTemp += divResult;
        in -= divResult * 61;
        divResult = DIV_APPROX_61( in );
    }
    if( in >= 61 )
    {
        outTemp += 1;
        in -= 61;
    }
    return outTemp;
#endif
}

static void CalendarDiv60( uint32_t in, uint32_t* out, uint32_t* remainder )
{
#if 0
    *remainder = in % 60;
    *out       = in / 60;
#else
    uint32_t outTemp = 0;
    uint32_t divResult = DIV_APPROX_60( in );

    while( divResult >=1 )
    {
        outTemp += divResult;
        in -= divResult * 60;
        divResult = DIV_APPROX_60( in );
    }
    if( in >= 60 )
    {
        outTemp += 1;
        in -= 60;
    }
    *remainder = in;
    *out = outTemp;
#endif
}
