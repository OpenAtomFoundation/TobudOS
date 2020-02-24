/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/** \addtogroup rtc_module Working with RTC
 * The RTC driver provides the interface to configure and use the RTC
 * peripheral.
 *
 * It manages date, time, and alarms.\n
 * This timer is clocked by the 32kHz system clock, and is not impacted by
 * power management settings (PMC). To be accurate, it is better to use an
 * external 32kHz crystal instead of the internal 32kHz RC.\n
 *
 * It uses BCD format, and time can be set in AM/PM or 24h mode through a
 * configuration bit in the mode register.\n
 *
 * To update date or time, the user has to follow these few steps :
 * <ul>
 * <li>Set UPDTIM and/or UPDCAL bit(s) in RTC_CR,</li>
 * <li>Polling or IRQ on the ACKUPD bit of RTC_CR,</li>
 * <li>Clear ACKUPD bit in RTC_SCCR,</li>
 * <li>Update Time and/or Calendar values in RTC_TIMR/RTC_CALR (BCD format),</li>
 * <li>Clear UPDTIM and/or UPDCAL bit in RTC_CR.</li>
 * </ul>
 * An alarm can be set to happen on month, date, hours, minutes or seconds,
 * by setting the proper "Enable" bit of each of these fields in the Time and
 * Calendar registers.
 * This allows a large number of configurations to be available for the user.
 * Alarm occurence can be detected even by polling or interrupt.
 *
 * A check of the validity of the date and time format and values written by the user is automatically done.
 * Errors are reported through the Valid Entry Register.
 *
 * For more accurate information, please look at the RTC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref rtc.c\n
 * \ref rtc.h.\n
*/
/*@{*/
/*@}*/


/**
 * \file
 *
 * Implementation of Real Time Clock (RTC) controller.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

/* These headers were introduced in C99 by working group ISO/IEC JTC1/SC22/WG14. */
#include <stdint.h>

#include "rtc.h"
#include <board.h>
#include <utility/assert.h>
#include <utility/trace.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Sets the RTC in either 12 or 24 hour mode.
 *
 * \param mode  Hour mode.
 */
void RTC_SetHourMode(uint32_t mode)
{
    SANITY_CHECK((mode & 0xFFFFFFFE) == 0);

    TRACE_DEBUG("RTC_SetHourMode()\n\r");

    RTC->RTC_MR = mode;
}

/**
 * \brief Gets the RTC mode.
 *
 * \return Hour mode.
 */
uint32_t RTC_GetHourMode( void )
{
    uint32_t hmode;

    TRACE_DEBUG("RTC_SetHourMode()\n\r");

    hmode = RTC->RTC_MR;
    hmode &= 0xFFFFFFFE;

    return hmode;
}

/**
 * \brief Enables the selected interrupt sources of the RTC.
 *
 * \param sources  Interrupt sources to enable.
 */
void RTC_EnableIt(uint32_t sources)
{
    SANITY_CHECK((sources & ~0x1F) == 0);

    TRACE_DEBUG("RTC_EnableIt()\n\r");

    RTC->RTC_IER = sources;
}

/**
* \brief Disables the selected interrupt sources of the RTC.
*
* \param sources  Interrupt sources to disable.
*/
void RTC_DisableIt(uint32_t sources)
{
    SANITY_CHECK((sources & ~0x1F) == 0);

    TRACE_DEBUG("RTC_DisableIt()\n\r");

    RTC->RTC_IDR = sources;
}

/**
 * \brief Sets the current time in the RTC.
 *
 * \param hour    Current hour in 12 or 24 hour mode.
 * \param minute  Current minute.
 * \param second  Current second.
 * \return 0 sucess, 1 fail to set
 */
int RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    uint32_t time=0;
    uint8_t hour_bcd;
    uint8_t min_bcd;
    uint8_t sec_bcd;

    TRACE_DEBUG("RTC_SetTime(%02d:%02d:%02d)\n\r", hour, minute, second);

    /* if 12-hour mode, set AMPM bit */
    if ((RTC->RTC_MR & RTC_MR_HRMOD) == RTC_MR_HRMOD) {

        if (hour > 12) {

            hour -= 12;
            time |= RTC_TIMR_AMPM;
        }
    }
    hour_bcd  = (hour%10) | ((hour/10)<<4);
    min_bcd   = (minute%10) | ((minute/10)<<4);
    sec_bcd   = (second%10) | ((second/10)<<4);

    /* value overflow */
    if((hour_bcd & (uint8_t)(~RTC_HOUR_BIT_LEN_MASK)) |
       (min_bcd & (uint8_t)(~RTC_MIN_BIT_LEN_MASK)) |
         (sec_bcd & (uint8_t)(~RTC_SEC_BIT_LEN_MASK)))
            return 1;

    time = sec_bcd | (min_bcd << 8) | (hour_bcd<<16);

    /* Set time */
    while ((RTC->RTC_SR & RTC_SR_SEC) != RTC_SR_SEC); /* wait from previous set */
    RTC->RTC_CR |= RTC_CR_UPDTIM;
    while ((RTC->RTC_SR & RTC_SR_ACKUPD) != RTC_SR_ACKUPD);
    RTC->RTC_SCCR = RTC_SCCR_ACKCLR;
    RTC->RTC_TIMR = time;
    RTC->RTC_CR &= ~RTC_CR_UPDTIM;
    RTC->RTC_SCCR |= RTC_SCCR_SECCLR; /* clear SECENV in SCCR */

    return (int)(RTC->RTC_VER & RTC_VER_NVTIM);
}

/**
 * \brief Retrieves the current time as stored in the RTC in several variables.
 *
 * \param pHour  If not null, current hour is stored in this variable.
 * \param pMinute  If not null, current minute is stored in this variable.
 * \param pSecond  If not null, current second is stored in this variable.
 */
void RTC_GetTime(
    uint8_t *pHour,
    uint8_t *pMinute,
    uint8_t *pSecond)
{
    uint32_t time;

    TRACE_DEBUG("RTC_GetTime()\n\r");

    /* Get current RTC time */
    time = RTC->RTC_TIMR;
    while (time != RTC->RTC_TIMR) {

        time = RTC->RTC_TIMR;
    }

    /* Hour */
    if (pHour) {

        *pHour = ((time & 0x00300000) >> 20) * 10
                 + ((time & 0x000F0000) >> 16);
        if ((time & RTC_TIMR_AMPM) == RTC_TIMR_AMPM) {

            *pHour += 12;
        }
    }

    /* Minute */
    if (pMinute) {

        *pMinute = ((time & 0x00007000) >> 12) * 10
                   + ((time & 0x00000F00) >> 8);
    }

    /* Second */
    if (pSecond) {

        *pSecond = ((time & 0x00000070) >> 4) * 10
                   + (time & 0x0000000F);
    }
}

/**
 * \brief Sets a time alarm on the RTC.
 * The match is performed only on the provided variables;
 * Setting all pointers to 0 disables the time alarm.
 *
 * \note In AM/PM mode, the hour value must have bit #7 set for PM, cleared for
 * AM (as expected in the time registers).
 *
 * \param pHour  If not null, the time alarm will hour-match this value.
 * \param pMinute  If not null, the time alarm will minute-match this value.
 * \param pSecond  If not null, the time alarm will second-match this value.
 * \return 0 success, 1 fail to set
 */
int RTC_SetTimeAlarm(
    uint8_t *pHour,
    uint8_t *pMinute,
    uint8_t *pSecond)
{
    uint32_t alarm = 0;

    TRACE_DEBUG("RTC_SetTimeAlarm()\n\r");

    /* Hour */
    if (pHour) {

        alarm |= RTC_TIMALR_HOUREN | ((*pHour / 10) << 20) | ((*pHour % 10) << 16);
    }

    /* Minute */
    if (pMinute) {

        alarm |= RTC_TIMALR_MINEN | ((*pMinute / 10) << 12) | ((*pMinute % 10) << 8);
    }

    /* Second */
    if (pSecond) {

        alarm |= RTC_TIMALR_SECEN | ((*pSecond / 10) << 4) | (*pSecond % 10);
    }

    RTC->RTC_TIMALR = alarm;

    return (int)(RTC->RTC_VER & RTC_VER_NVTIMALR);
}

/**
 * \brief Retrieves the current year, month and day from the RTC.
 * Month, day and week values are numbered starting at 1.
 *
 * \param pYear  Current year (optional).
 * \param pMonth  Current month (optional).
 * \param pDay  Current day (optional).
 * \param pWeek  Current day in current week (optional).
 */
void RTC_GetDate(
    unsigned short *pYear,
    uint8_t *pMonth,
    uint8_t *pDay,
    uint8_t *pWeek)
{
    uint32_t date;

    /* Get current date (multiple reads are necessary to insure a stable value) */
    do {

        date = RTC->RTC_CALR;
    }
    while (date != RTC->RTC_CALR);

    /* Retrieve year */
    if (pYear) {

        *pYear = (((date  >> 4) & 0x7) * 1000)
                 + ((date & 0xF) * 100)
                 + (((date >> 12) & 0xF) * 10)
                 + ((date >> 8) & 0xF);
    }

    /* Retrieve month */
    if (pMonth) {

        *pMonth = (((date >> 20) & 1) * 10) + ((date >> 16) & 0xF);
    }

    /* Retrieve day */
    if (pDay) {

        *pDay = (((date >> 28) & 0x3) * 10) + ((date >> 24) & 0xF);
    }

    /* Retrieve week */
    if (pWeek) {

        *pWeek = ((date >> 21) & 0x7);
    }
}

/**
 * \brief Sets the current year, month and day in the RTC.
 * Month, day and week values must be numbered starting from 1.
 *
 * \param year  Current year.
 * \param month  Current month.
 * \param day  Current day.
 * \param week  Day number in current week.
 * \return 0 success, 1 fail to set
 */
int RTC_SetDate(
    unsigned short year,
    uint8_t month,
    uint8_t day,
    uint8_t week)
{
    uint32_t date;
    uint8_t cent_bcd;
    uint8_t year_bcd;
    uint8_t month_bcd;
    uint8_t day_bcd;
    uint8_t week_bcd;

    cent_bcd  = ((year/100)%10) | ((year/1000)<<4);
    year_bcd  = (year%10) | (((year/10)%10)<<4);
    month_bcd = ((month%10) | (month/10)<<4);
    day_bcd   = ((day%10) | (day/10)<<4);
    week_bcd  = ((week%10) | (week/10)<<4);

    /* value over flow */
    if((cent_bcd & (uint8_t)(~RTC_CENT_BIT_LEN_MASK)) |
        (year_bcd & (uint8_t)(~RTC_YEAR_BIT_LEN_MASK)) |
          (month_bcd & (uint8_t)(~RTC_MONTH_BIT_LEN_MASK)) |
            (week_bcd & (uint8_t)(~RTC_WEEK_BIT_LEN_MASK)) |
              (day_bcd & (uint8_t)(~RTC_DATE_BIT_LEN_MASK)))
                return 1;


    /* Convert values to date register value */
    date = cent_bcd |
            (year_bcd << 8) |
              (month_bcd << 16) |
                (week_bcd << 21) |
                  (day_bcd << 24);


    /* Update calendar register  */
    while ((RTC->RTC_SR & RTC_SR_SEC) != RTC_SR_SEC); /* wait from previous set */
    RTC->RTC_CR |= RTC_CR_UPDCAL;
    while ((RTC->RTC_SR & RTC_SR_ACKUPD) != RTC_SR_ACKUPD);
    RTC->RTC_SCCR = RTC_SCCR_ACKCLR;
    RTC->RTC_CALR = date;
    RTC->RTC_CR &= ~RTC_CR_UPDCAL;
    RTC->RTC_SCCR |= RTC_SCCR_SECCLR; /* clear SECENV in SCCR */

    return (int)(RTC->RTC_VER & RTC_VER_NVCAL);
}

/**
 * \brief Sets a date alarm in the RTC.
 * The alarm will match only the provided values;
 * Passing a null-pointer disables the corresponding field match.
 *
 * \param pMonth  If not null, the RTC alarm will month-match this value.
 * \param pDay  If not null, the RTC alarm will day-match this value.
 * \return 0 success, 1 fail to set
 */
int RTC_SetDateAlarm(uint8_t *pMonth, uint8_t *pDay)
{
    uint32_t alarm;

    alarm = ((pMonth) || (pDay)) ? (0) : (0x01010000);

    TRACE_DEBUG("RTC_SetDateAlarm()\n\r");

    /* Compute alarm field value */
    if (pMonth) {

        alarm |= RTC_CALALR_MTHEN | ((*pMonth / 10) << 20) | ((*pMonth % 10) << 16);
    }
    if (pDay) {

        alarm |= RTC_CALALR_DATEEN | ((*pDay / 10) << 28) | ((*pDay % 10) << 24);
    }

    /* Set alarm */
    RTC->RTC_CALALR = alarm;

    return (int)(RTC->RTC_VER & RTC_VER_NVCALALR);
}

/**
 * \brief Clear flag bits of status clear command register in the RTC.
 *
 * \param mask Bits mask of cleared events
 */
void RTC_ClearSCCR(uint32_t mask)
{
    /* Clear all flag bits in status clear command register */
    mask &= RTC_SCCR_ACKCLR | RTC_SCCR_ALRCLR | RTC_SCCR_SECCLR | \
                                    RTC_SCCR_TIMCLR | RTC_SCCR_CALCLR;

    RTC->RTC_SCCR = mask;
}

/**
 * \brief Get flag bits of status register in the RTC.
 *
 * \param mask Bits mask of Status Register
 * \return Status register & mask
 */
uint32_t RTC_GetSR(uint32_t mask)
{
    uint32_t event;

    event = RTC->RTC_SR;

    return (event & mask);
}

