#include "stm8l15x.h"
#include "rtc.h"

StruCLOCK clock;

const uint8_t MonthLength[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void RTC_Setting_Init(void)
{
    RTC_InitTypeDef RTC_InitStr;
    RTC_TimeTypeDef RTC_TimeStr;
    RTC_DateTypeDef RTC_DateStr;

    //RTC Clock Init
    CLK_LSEConfig(CLK_LSE_ON); //Open LSE Clock 32.768K
    while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET) //wait is ready
        ;
    RTC_DeInit();
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1); //select LSE no div 32.768K
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE); 
    RTC_WakeUpCmd(DISABLE);


    RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
    RTC_InitStr.RTC_AsynchPrediv = 0x7f; //32.768K/(127+1)=256
    RTC_InitStr.RTC_SynchPrediv = 0x0ff; //256/(255+1)=1
    RTC_Init(&RTC_InitStr);

    //Write default time 18.1.1 00:00:00
    RTC_DateStructInit(&RTC_DateStr);
    RTC_DateStr.RTC_Date = 1;
    RTC_DateStr.RTC_Month = RTC_Month_January;
    RTC_DateStr.RTC_Year = 18;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);

    RTC_TimeStructInit(&RTC_TimeStr);
    RTC_TimeStr.RTC_Hours = 0;
    RTC_TimeStr.RTC_Minutes = 0;
    RTC_TimeStr.RTC_Seconds = 0;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);

}

void RTC_Get_Time(void)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    RTC_GetTime(RTC_Format_BIN, &time); //get the time
    RTC_GetDate(RTC_Format_BIN, &date); //get the date

    clock.cHour = time.RTC_Hours;
    clock.cMinute = time.RTC_Minutes;
    clock.cSecond = time.RTC_Seconds;

    clock.cYear = date.RTC_Year;
    clock.cMonth = date.RTC_Month;
    clock.cDay = date.RTC_Date;
    clock.cWeek = date.RTC_WeekDay;
}

