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

/**
 * \file
 *
 * Interface for Real Time Clock (RTC) controller.
 *
 */

#ifndef RTC_H
#define RTC_H

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

#define RTC_HOUR_BIT_LEN_MASK   0x3F
#define RTC_MIN_BIT_LEN_MASK    0x7F
#define RTC_SEC_BIT_LEN_MASK    0x7F
#define RTC_CENT_BIT_LEN_MASK   0x7F
#define RTC_YEAR_BIT_LEN_MASK   0xFF
#define RTC_MONTH_BIT_LEN_MASK  0x1F
#define RTC_DATE_BIT_LEN_MASK   0x3F
#define RTC_WEEK_BIT_LEN_MASK   0x07

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern void RTC_SetHourMode(uint32_t mode);

extern uint32_t RTC_GetHourMode( void ) ;

extern void RTC_EnableIt(uint32_t sources);

extern void RTC_DisableIt(uint32_t sources);

extern int RTC_SetTime(
    uint8_t hour,
    uint8_t minute,
    uint8_t second);

extern void RTC_GetTime(
    uint8_t *pHour,
    uint8_t *pMinute,
    uint8_t *pSecond);

extern int RTC_SetTimeAlarm(
    uint8_t *pHour,
    uint8_t *pMinute,
    uint8_t *pSecond);

extern void RTC_GetDate(
    unsigned short *pYear,
    uint8_t *pMonth,
    uint8_t *pDay,
    uint8_t *pWeek);

extern int RTC_SetDate(
    unsigned short year,
    uint8_t month,
    uint8_t day,
    uint8_t week);

extern int RTC_SetDateAlarm(uint8_t *pMonth, uint8_t *pDay);

extern void RTC_ClearSCCR(uint32_t mask);

extern uint32_t RTC_GetSR(uint32_t mask);

#endif /* #ifndef RTC_H */

