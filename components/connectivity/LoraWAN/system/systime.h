/*!
 * \file      systime.h
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
#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "time.h"

/*!
 * \brief Days, Hours, Minutes and seconds of systime.h
 */
#define TM_DAYS_IN_LEAP_YEAR                        ( ( uint32_t )  366U )
#define TM_DAYS_IN_YEAR                             ( ( uint32_t )  365U )
#define TM_SECONDS_IN_1DAY                          ( ( uint32_t )86400U )
#define TM_SECONDS_IN_1HOUR                         ( ( uint32_t ) 3600U )
#define TM_SECONDS_IN_1MINUTE                       ( ( uint32_t )   60U )
#define TM_MINUTES_IN_1HOUR                         ( ( uint32_t )   60U )
#define TM_HOURS_IN_1DAY                            ( ( uint32_t )   24U )


/*!
 * \brief Months of systime.h
 */
#define TM_MONTH_JANUARY                            ( ( uint8_t ) 0U )
#define TM_MONTH_FEBRUARY                           ( ( uint8_t ) 1U )
#define TM_MONTH_MARCH                              ( ( uint8_t ) 2U )
#define TM_MONTH_APRIL                              ( ( uint8_t ) 3U )
#define TM_MONTH_MAY                                ( ( uint8_t ) 4U )
#define TM_MONTH_JUNE                               ( ( uint8_t ) 5U )
#define TM_MONTH_JULY                               ( ( uint8_t ) 6U )
#define TM_MONTH_AUGUST                             ( ( uint8_t ) 7U )
#define TM_MONTH_SEPTEMBER                          ( ( uint8_t ) 8U )
#define TM_MONTH_OCTOBER                            ( ( uint8_t ) 9U )
#define TM_MONTH_NOVEMBER                           ( ( uint8_t )10U )
#define TM_MONTH_DECEMBER                           ( ( uint8_t )11U )

/*!
 * \brief Week days of systime.h
 */
#define TM_WEEKDAY_SUNDAY                           ( ( uint8_t )0U )
#define TM_WEEKDAY_MONDAY                           ( ( uint8_t )1U )
#define TM_WEEKDAY_TUESDAY                          ( ( uint8_t )2U )
#define TM_WEEKDAY_WEDNESDAY                        ( ( uint8_t )3U )
#define TM_WEEKDAY_THURSDAY                         ( ( uint8_t )4U )
#define TM_WEEKDAY_FRIDAY                           ( ( uint8_t )5U )
#define TM_WEEKDAY_SATURDAY                         ( ( uint8_t )6U )

/*!
 * \brief Number of seconds elapsed between Unix and GPS epoch
 */
#define UNIX_GPS_EPOCH_OFFSET                       315964800

/*!
 * \brief Structure holding the system time in seconds and milliseconds.
 */
typedef struct SysTime_s
{
    uint32_t Seconds;
    int16_t  SubSeconds;
}SysTime_t;

/*!
 * \brief Adds 2 SysTime_t values
 *
 * \param a Value
 * \param b Value to added
 *
 * \retval result Addition result (SysTime_t value)
 */
SysTime_t SysTimeAdd( SysTime_t a, SysTime_t b );

/*!
 * \brief Subtracts 2 SysTime_t values
 *
 * \param a Value
 * \param b Value to be subtracted
 *
 * \retval result Subtraction result (SysTime_t value)
 */
SysTime_t SysTimeSub( SysTime_t a, SysTime_t b );

/*!
 * \brief Sets new system time
 *
 * \param  sysTime    New seconds/sub-seconds since UNIX epoch origin
 */
void SysTimeSet( SysTime_t sysTime );

/*!
 * \brief Gets current system time
 *
 * \retval sysTime    Current seconds/sub-seconds since UNIX epoch origin
 */
SysTime_t SysTimeGet( void );

/*!
 * \brief Gets current MCU system time
 *
 * \retval sysTime    Current seconds/sub-seconds since Mcu started
 */
SysTime_t SysTimeGetMcuTime( void );

/*!
 * Converts the given SysTime to the equivalent RTC value in milliseconds
 *
 * \param [IN] sysTime System time to be converted
 * 
 * \retval timeMs The RTC converted time value in ms
 */
uint32_t SysTime2Ms( SysTime_t sysTime );

/*!
 * \brief Convert a calendar time into time since UNIX epoch as a uint32_t.
 *
 * \param [IN] localtime Pointer to the object containing the calendar time
 * \retval     timestamp The calendar time as seconds since UNIX epoch.
 */
uint32_t SysTimeMkTime( const struct tm* localtime );

/*!
 * \brief Converts a given time in seconds since UNIX epoch into calendar time.
 *
 * \param [IN]  timestamp The time since UNIX epoch to convert into calendar time.
 * \param [OUT] localtime Pointer to the calendar time object which will contain
                          the result of the conversion.
 */
void SysTimeLocalTime( const uint32_t timestamp, struct tm *localtime );

#ifdef __cplusplus
}
#endif

#endif // __SYS_TIME_H__
