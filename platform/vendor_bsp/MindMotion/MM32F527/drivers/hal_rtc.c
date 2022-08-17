/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_rtc.h"

/* Initialize RTC. */
void RTC_Init(RTC_Type * RTCx, RTC_Init_Type * init)
{
    uint32_t temp = ( (init->ClockFreq / init->CountFreq) - 1u);
    RTC_PutPrescalerValue(RTCx, temp);
}

/* Get the current status flags of the RTC module. */
uint32_t RTC_GetStatus(RTC_Type * RTCx)
{
    return RTCx->CRL;
}

/* Clear the status flag of the RTC module. */
void RTC_ClearStatus(RTC_Type * RTCx, uint32_t status)
{
    RTCx->CRL &= ~status;
}

/* Set the counter hopping frequency, open configration module before put data into RTC register. */
void RTC_PutPrescalerValue(RTC_Type * RTCx, uint32_t div)
{
    RTCx->CRL |= RTC_CRL_CNF_MASK;  /* Enable the configuration mode. */
    RTCx->PRLH = div >> 16u;        /* Setup the upper 16-bit value of prescaler, RTC_PRL[19:0] = (fRTC_CLK / fSC_CLK ) - 1u. */
    RTCx->PRLL = div;               /* Setup the lower 16-bit value of prescaler. */
    RTCx->CRL &= ~RTC_CRL_CNF_MASK; /* Disable the configuration mode. */
}

/* Configure the value at which the counter starts counting. */
void RTC_PutCounterValue(RTC_Type * RTCx, uint32_t cnt)
{
    RTCx->CRL |= RTC_CRL_CNF_MASK;  /* Enable the configuration mode. */
    RTCx->CNTH = cnt >> 16u;        /* Setup the upper 16-bit value of counter. */
    RTCx->CNTL = cnt;               /* Setup the lower 16-bit value of counter. */
    RTCx->CRL &= ~RTC_CRL_CNF_MASK; /* Disable the configuration mode. */
}

/* Configure alarm clock response time. */
void RTC_PutAlarmValue(RTC_Type * RTCx, uint32_t alarm)
{
    RTCx->CRL |= RTC_CRL_CNF_MASK;  /* Enable the configuration mode. */
    RTCx->ALRH = alarm >> 16u;      /* Setup the upper 16-bit value of alarm counter. */
    RTCx->ALRL = alarm;             /* Setup the lower 16-bit value of alarm counter. */
    RTCx->CRL &= ~RTC_CRL_CNF_MASK; /* Disable the configuration mode. */
}

/* Get the current value from counter of RTC module. */
uint32_t RTC_GetCounterValue(RTC_Type * RTCx)
{
    return ( (RTCx->CNTH << 16u) | RTCx->CNTL );
}

/* Get the alarm clock response time */
uint32_t RTC_GetAlarmValue(RTC_Type * RTCx)
{
    return ( (RTCx->ALRH << 16u) | RTCx->ALRL);
}

/* Enable RTC interrupt of RTC module. */
void RTC_EnableInterrupts(RTC_Type * RTCx, uint32_t interrupts, bool enable)
{
    if (enable)
    {
        RTCx->CRH |= interrupts;
    }
    else
    {
        RTCx->CRH &= ~interrupts;
    }
}

/* Get the interrupts status flags of the RTC module. */
uint32_t RTC_GetInterruptStatus(RTC_Type * RTCx)
{
    return (RTCx->CRL & RTCx->CRH);
}

/* Clear the status of RTC interrupt. */
void RTC_ClearInterruptStatus(RTC_Type * RTCx, uint32_t interrupts)
{
    RTCx->CRL &= ~interrupts;
}

/* Judging whether the current year is a leap year, an ordinary leap year or a century leap year. */
bool RTC_JudgeLeapYear(uint16_t years)
{
    if (years % 4u == 0u)
    {
        if (years % 100u == 0u)
        {
            if (years % 400u == 0u)
            {
                return true;  /* Century leap year. */
            }
            else
            {
            	return false;
            }
        }
        else
        {
        	return true; /* Ordinary leap year. */
        }
    }
    else
    {
        return false;
    }
}

/* Month correction table, used for calculation of month. */
const uint8_t month_table[12u] = {31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u};

/* Setup initialization time, calculate the year, month, day, hour, minute and second as the total number of seconds. */
void RTC_SetTime(RTC_Type * RTCx, RTC_Time_Type * time)
{
    /* Calculate the total number of seconds of the current configuration time. */
    uint32_t seccnt = 0u;
    /* Calculate the number of seconds from the lowest years to the current setup years. */
    for (uint16_t years = time->Lyears; years < time->Years; years++)
    {
        if ( RTC_JudgeLeapYear(years) )
        {
            seccnt += 31622400u;  /* The number of seconds in leap year is 31622400. */
        }
        else
        {
            seccnt += 31536000u;  /* The number of seconds in normal year is 31622400. */
        }
    }

    /* Add up the seconds of the previous month. */
    time->Months -= 1u;  /* The month count starts from 0 instead of 1, so current months - 1. */
    for (uint16_t months = 0u; months < time->Months; months++)
    {
        seccnt += (uint32_t)month_table[months] * 86400u;  /* Calculate the number of seconds of months, the total number of seconds in a day is 86400. */
        if ( ( RTC_JudgeLeapYear(time->Years) ) && (months == 1u) )  /* The time is in a leap year and february, add the number of seconds in one day. */
        {
            seccnt += 86400u;  /* The number of seconds in day is 86400. */
        }
    }
    /* Add up the seconds of the previous date. */
    seccnt += (uint32_t)(time->Days - 1u) * 86400u;  /* The day set for initialization is less than 24 hours, which needs to be subtracted by one day. */
    seccnt += (uint32_t)(time->Hours) * 3600u;  /* There are 3600 seconds in a hour. */
    seccnt += (uint32_t)(time->Mins) * 60u;   /* There are 60 seconds in a minute. */
    seccnt += (time->Secs);

    /* Configrate counter value. */
    RTC_PutCounterValue(RTCx, seccnt);  /* Put data into counter. */
}

/* Calculate the total number of seconds as year, month, day, hour, minute and second.. */
void RTC_CalcTime(RTC_Type * RTCx, RTC_Time_Type * time)
{
    uint32_t count = RTC_GetCounterValue(RTCx);  /* Get current seconds count. */

    /* Calculated in days. */
    uint16_t years = time->Lyears;
    uint32_t days = count / 86400u;

    for (; days >= 365u; days -= 365u)
    {
        if ( RTC_JudgeLeapYear(years) )  /* Determine whether it is a leap year. */
        {
            if (days >= 366u)
            {
                days -= 1u;
            }
            else
            {
                break;
            }
        }
        years++;
    }
    time->Years = years;  /* Get current years. */

    uint16_t months = 0u;
    for (; days >= 28u; days -= 28u)
    {
        if ( ( true == RTC_JudgeLeapYear(time->Years) ) && (months == 1u) )  /* The time is February of leap year. */
        {
            if (days >= 29u)
            {
                days -= 1u;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (days >= month_table[months])  /* Reach the maximum number of days in the current month. */
            {
                days = days - month_table[months] + 28u;
            }
            else
            {
                break;
            }
        }
        months++;
    }
    time->Months = months + 1u;  /* Get current months. */
    time->Days   = days   + 1u;   /* Get current days. */
    time->Hours  = ( count % 86400u) / 3600u;  /* Get current hours. */
    time->Mins   = ((count % 86400u) % 3600u) / 60u;  /* Get current minutes. */
    time->Secs   = ((count % 86400u) % 3600u) % 60u;  /* Get current seconds. */
}

/* Setup the alarm response time. */
void RTC_SetAlarm(RTC_Type * RTCx, RTC_Time_Type * time)
{
    uint32_t value = RTC_GetCounterValue(RTCx);  /* Get the current total number of seconds. */
    RTC_PutAlarmValue(RTCx, value + time->AlarmTime);  /* Set alarm respond time. */
}

/* EOF. */

