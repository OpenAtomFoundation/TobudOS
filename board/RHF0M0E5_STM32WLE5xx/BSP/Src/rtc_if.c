/*******************************************************************************
 * @file    rtc_if.c
 * @author  MCD Application Team
 * @brief   Handles the RTC
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "platform.h"
#include "rtc_if.h"
#include "stm32_lpm.h"
#include "app_system.h"
#include "stm32wlxx_ll_rtc.h"
/* External variables ---------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;
#define RtcHandle hrtc

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint32_t Rtc_Time; /* Reference time */

  RTC_TimeTypeDef RTC_Calndr_Time; /* Reference time in calendar format */

  RTC_DateTypeDef RTC_Calndr_Date; /* Reference date in calendar format */

} RtcTimerContext_t;

/* Private define ------------------------------------------------------------*/

/* MCU Wake Up Time */
#define MIN_ALARM_DELAY               3 /* in ticks */

/* subsecond number of bits */  /* Now defined in main.h via MX GUI */
//#define RTC_N_PREDIV_S                 10
//#define RTC_PREDIV_S                  ((1<<RTC_N_PREDIV_S)-1)      /* RTC_SYNCH_PREDIV; */
//#define RTC_PREDIV_A                  (1<<(15-RTC_N_PREDIV_S))-1    /* RTC_ASYNCH_PREDIV; */

/* Sub-second mask definition  */
#define RTC_ALARMSUBSECONDMASK    RTC_N_PREDIV_S<<RTC_ALRMASSR_MASKSS_Pos


/* RTC Time base in us */
#define USEC_NUMBER               1000000
#define MSEC_NUMBER               (USEC_NUMBER/1000)
#define RTC_ALARM_TIME_BASE       (USEC_NUMBER>>RTC_N_PREDIV_S)

#define COMMON_FACTOR        3
#define CONV_NUMER                (MSEC_NUMBER>>COMMON_FACTOR)
#define CONV_DENOM                (1<<(RTC_N_PREDIV_S-COMMON_FACTOR))

#define  DAYS_IN_LEAP_YEAR (uint32_t) 366

#define  DAYS_IN_YEAR      (uint32_t) 365

#define  SECONDS_IN_1DAY   (uint32_t) 86400

#define  SECONDS_IN_1HOUR   (uint32_t) 3600

#define  SECONDS_IN_1MINUTE   (uint32_t) 60

#define  MINUTES_IN_1HOUR    (uint32_t) 60

#define  HOURS_IN_1DAY      (uint32_t) 24

#define  DAYS_IN_MONTH_CORRECTION_NORM     ((uint32_t) 0x99AAA0 )
#define  DAYS_IN_MONTH_CORRECTION_LEAP     ((uint32_t) 0x445550 )


/* Calculates ceiling(X/N) */
#define DIVC(X,N)   ( ( (X) + (N) -1 ) / (N) )


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*!
 * \brief Indicates if the RTC is already Initalized or not
 */
static bool RTC_Initalized = false;

/*!
 * \brief compensates MCU wakeup time
 */

static bool McuWakeUpTimeInitialized = false;

/*!
 * \brief compensates MCU wakeup time
 */

static int16_t McuWakeUpTimeCal = 0;

/*!
 * Number of days in each month on a normal year
 */
static const uint8_t DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*!
 * Number of days in each month on a leap year
 */
static const uint8_t DaysInMonthLeapYear[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static RTC_AlarmTypeDef RTC_AlarmStructure;


/*!
 * Keep the value of the RTC timer when the RTC alarm is set
 * Set with the RTC_SetTimerContext function
 * Value is kept as a Reference to calculate alarm
 */
static RtcTimerContext_t RtcTimerContext;


/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief start wake up alarm
 * @note  alarm in RtcTimerContext.Rtc_Time + timeoutValue
 * @param timeoutValue in ticks
 * @retval none
 */
static void RTC_StartWakeUpAlarm(uint32_t timeoutValue);

/*!
 * @brief get current time from calendar in ticks
 * @param pointer to RTC_DateStruct
 * @param pointer to RTC_TimeStruct
 * @retval time in ticks
 */
static uint32_t RTC_GetCalendarValue(RTC_DateTypeDef *RTC_DateStruct, RTC_TimeTypeDef *RTC_TimeStruct);

/* Exported variables ---------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/
/**
  * @brief This function configures the source of the time base.
  * @brief  don't enable systick
  * @param TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /*Initialize the RTC services */
  RTC_Init();
  return HAL_OK;
}

/**
  * @brief Provide a tick value in millisecond measured using RTC
  * @note This function overwrites the __weak one from HAL
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
  if (RTC_Initalized == false)
  {
    return 0; /* HAL_GetTick() based on RTC cannot be used until RTC is initialised */
  }
  else
  {
    return RTC_GetTimerValue();
  }
}

/**
  * @brief This function provides delay (in ms)
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(__IO uint32_t Delay)
{
  RTC_DelayMs(Delay);   /* based on RTC */
}



/**
  * @brief  Alarm A callback.
  * @param  RtcHandle: RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *RtcHandle)
{
  //UTIL_TIMER_IRQ_Handler( );
}



void RTC_Init(void)
{
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  if (RTC_Initalized == false)
  {
    /*##-1- Configue the RTC clock soucre ######################################*/

    /* Select LSE as RTC clock source */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      while (1);
    }

    MX_RTC_Init();

#if defined (LPM_STOP_MODE_DISABLE) && (LPM_STOP_MODE_DISABLE == 0) /* LOW_POWER enabled */
    /* enable RTC ALARM EXTI (AIEC) line   */
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17); 
#elif !defined (LPM_STOP_MODE_DISABLE)
 #error LPM_STOP_MODE_DISABLE not defined
#endif

    /*Enable Direct Read of the calendar registers (not through Shadow) */
    HAL_RTCEx_EnableBypassShadow(&RtcHandle);

    /** Configure the Alarm A */
    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);

    RTC_SetTimerContext();
    RTC_Initalized = true;
  }
}


void RTC_setMcuWakeUpTime(void)
{
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;

  uint32_t now, hit;
  int16_t McuWakeUpTime;

  if ((McuWakeUpTimeInitialized == false) &&
      (HAL_NVIC_GetPendingIRQ(RTC_Alarm_IRQn) == 1))
  {
    /* warning: works ok if now is below 30 days
       it is ok since it's done once at first alarm wake-up*/
    McuWakeUpTimeInitialized = true;
    now = RTC_GetCalendarValue(&RTC_DateStruct, &RTC_TimeStruct);


    HAL_RTC_GetAlarm(&RtcHandle, &RTC_AlarmStructure, RTC_ALARM_A, RTC_FORMAT_BIN);
    hit = RTC_AlarmStructure.AlarmTime.Seconds +
          60 * (RTC_AlarmStructure.AlarmTime.Minutes +
                60 * (RTC_AlarmStructure.AlarmTime.Hours +
                      24 * (RTC_AlarmStructure.AlarmDateWeekDay)));
    hit = (hit << RTC_N_PREDIV_S) + (RTC_PREDIV_S - RTC_AlarmStructure.AlarmTime.SubSeconds);

    McuWakeUpTime = (int16_t)((now - hit));
    McuWakeUpTimeCal += McuWakeUpTime;
    APP_LOG("Cal=%d, %d\n\r", McuWakeUpTimeCal, McuWakeUpTime);
  }
}

int16_t RTC_getMcuWakeUpTime(void)
{
  return McuWakeUpTimeCal;
}


uint32_t RTC_GetMinimumTimeout(void)
{
  return (MIN_ALARM_DELAY);
}


uint32_t RTC_ms2Tick(uint32_t timeMicroSec)
{
  /*return( ( timeMicroSec / RTC_ALARM_TIME_BASE ) ); */
  return (uint32_t)((((uint64_t)timeMicroSec) * CONV_DENOM) / CONV_NUMER);
}


uint32_t RTC_Tick2ms(uint32_t tick)
{
  /*return( ( timeMicroSec * RTC_ALARM_TIME_BASE ) ); */
  return (((uint64_t)(tick) * CONV_NUMER) / CONV_DENOM);
}

uint32_t RTC_GetTimerElapsedTime(void)
{
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;

  uint32_t CalendarValue = RTC_GetCalendarValue(&RTC_DateStruct, &RTC_TimeStruct);

  return ((uint32_t)(CalendarValue - RtcTimerContext.Rtc_Time));
}

uint32_t RTC_GetTimerValue(void)
{
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;

  uint32_t CalendarValue = (uint32_t) RTC_GetCalendarValue(&RTC_DateStruct, &RTC_TimeStruct);

  return (CalendarValue);
}




void RTC_DelayMs(uint32_t delay)
{
  uint32_t delayValue = 0;
  uint32_t timeout = 0;

  delayValue = RTC_ms2Tick(delay);

  /* Wait delay ms */
  timeout = RTC_GetTimerValue();
  while (((RTC_GetTimerValue() - timeout)) < delayValue)
  {
    __NOP();
  }
}


uint32_t RTC_SetTimerContext(void)
{
  RtcTimerContext.Rtc_Time = RTC_GetCalendarValue(&RtcTimerContext.RTC_Calndr_Date, &RtcTimerContext.RTC_Calndr_Time);
  return (uint32_t) RtcTimerContext.Rtc_Time;
}


uint32_t RTC_GetTimerContext(void)
{
  return (uint32_t) RtcTimerContext.Rtc_Time;
}
/* Private functions ---------------------------------------------------------*/

/*!
 * @brief start wake up alarm
 * @note  alarm in RtcTimerContext.Rtc_Time + timeoutValue
 * @param timeoutValue in ticks
 * @retval none
 */
static void RTC_StartWakeUpAlarm(uint32_t timeoutValue)
{
  uint16_t rtcAlarmSubSeconds = 0;
  uint16_t rtcAlarmSeconds = 0;
  uint16_t rtcAlarmMinutes = 0;
  uint16_t rtcAlarmHours = 0;
  uint16_t rtcAlarmDays = 0;
  RTC_TimeTypeDef RTC_TimeStruct = RtcTimerContext.RTC_Calndr_Time;
  RTC_DateTypeDef RTC_DateStruct = RtcTimerContext.RTC_Calndr_Date;

  //RTC_StopAlarm();


  /*reverse counter */
  rtcAlarmSubSeconds =  RTC_PREDIV_S - RTC_TimeStruct.SubSeconds;
  rtcAlarmSubSeconds += (timeoutValue & RTC_PREDIV_S);
  /* convert timeout  to seconds */
  timeoutValue >>= RTC_N_PREDIV_S;  /* convert timeout  in seconds */

  /*convert microsecs to RTC format and add to 'Now' */
  rtcAlarmDays =  RTC_DateStruct.Date;
  while (timeoutValue >= SECONDS_IN_1DAY)
  {
    timeoutValue -= SECONDS_IN_1DAY;
    rtcAlarmDays++;
  }

  /* calc hours */
  rtcAlarmHours = RTC_TimeStruct.Hours;
  while (timeoutValue >= SECONDS_IN_1HOUR)
  {
    timeoutValue -= SECONDS_IN_1HOUR;
    rtcAlarmHours++;
  }

  /* calc minutes */
  rtcAlarmMinutes = RTC_TimeStruct.Minutes;
  while (timeoutValue >= SECONDS_IN_1MINUTE)
  {
    timeoutValue -= SECONDS_IN_1MINUTE;
    rtcAlarmMinutes++;
  }

  /* calc seconds */
  rtcAlarmSeconds =  RTC_TimeStruct.Seconds + timeoutValue;

  /***** correct for modulo********/
  while (rtcAlarmSubSeconds >= (RTC_PREDIV_S + 1))
  {
    rtcAlarmSubSeconds -= (RTC_PREDIV_S + 1);
    rtcAlarmSeconds++;
  }

  while (rtcAlarmSeconds >= SECONDS_IN_1MINUTE)
  {
    rtcAlarmSeconds -= SECONDS_IN_1MINUTE;
    rtcAlarmMinutes++;
  }

  while (rtcAlarmMinutes >= MINUTES_IN_1HOUR)
  {
    rtcAlarmMinutes -= MINUTES_IN_1HOUR;
    rtcAlarmHours++;
  }

  while (rtcAlarmHours >= HOURS_IN_1DAY)
  {
    rtcAlarmHours -= HOURS_IN_1DAY;
    rtcAlarmDays++;
  }

  if (RTC_DateStruct.Year % 4 == 0)
  {
    if (rtcAlarmDays > DaysInMonthLeapYear[ RTC_DateStruct.Month - 1 ])
    {
      rtcAlarmDays = rtcAlarmDays % DaysInMonthLeapYear[ RTC_DateStruct.Month - 1 ];
    }
  }
  else
  {
    if (rtcAlarmDays > DaysInMonth[ RTC_DateStruct.Month - 1 ])
    {
      rtcAlarmDays = rtcAlarmDays % DaysInMonth[ RTC_DateStruct.Month - 1 ];
    }
  }

  /* Set RTC_AlarmStructure with calculated values*/
  RTC_AlarmStructure.AlarmTime.SubSeconds = RTC_PREDIV_S - rtcAlarmSubSeconds;
  RTC_AlarmStructure.AlarmSubSecondMask  = RTC_ALARMSUBSECONDMASK;
  RTC_AlarmStructure.AlarmTime.Seconds = rtcAlarmSeconds;
  RTC_AlarmStructure.AlarmTime.Minutes = rtcAlarmMinutes;
  RTC_AlarmStructure.AlarmTime.Hours   = rtcAlarmHours;
  RTC_AlarmStructure.AlarmDateWeekDay    = (uint8_t)rtcAlarmDays;
  RTC_AlarmStructure.AlarmTime.TimeFormat   = RTC_TimeStruct.TimeFormat;
  RTC_AlarmStructure.AlarmDateWeekDaySel   = RTC_ALARMDATEWEEKDAYSEL_DATE;
  RTC_AlarmStructure.AlarmMask       = RTC_ALARMMASK_NONE;
  RTC_AlarmStructure.Alarm = RTC_ALARM_A;
  RTC_AlarmStructure.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  RTC_AlarmStructure.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

  /* Set RTC_Alarm */
  HAL_RTC_SetAlarm_IT(&RtcHandle, &RTC_AlarmStructure, RTC_FORMAT_BIN);
}


/*!
 * @brief get current time from calendar in ticks
 * @param pointer to RTC_DateStruct
 * @param pointer to RTC_TimeStruct
 * @retval time in ticks
 */
static uint32_t RTC_GetCalendarValue(RTC_DateTypeDef *RTC_DateStruct, RTC_TimeTypeDef *RTC_TimeStruct)
{
  uint32_t calendarValue = 0;
  uint32_t first_read;
  uint32_t correction;

  /* Get Time and Date*/
  HAL_RTC_GetTime(&RtcHandle, RTC_TimeStruct, RTC_FORMAT_BIN);


  /* make sure it is correct due to asynchronus nature of RTC*/
  do
  {
    first_read = LL_RTC_TIME_GetSubSecond(RTC);
    HAL_RTC_GetDate(&RtcHandle, RTC_DateStruct, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&RtcHandle, RTC_TimeStruct, RTC_FORMAT_BIN);

  }
  while (first_read != LL_RTC_TIME_GetSubSecond(RTC));

  /* calculte amount of elapsed days since 01/01/2000 */
  calendarValue = DIVC((DAYS_IN_YEAR * 3 + DAYS_IN_LEAP_YEAR) * RTC_DateStruct->Year, 4);

  correction = ((RTC_DateStruct->Year % 4) == 0) ? DAYS_IN_MONTH_CORRECTION_LEAP : DAYS_IN_MONTH_CORRECTION_NORM ;

  calendarValue += (DIVC((RTC_DateStruct->Month - 1) * (30 + 31), 2) - (((correction >> ((RTC_DateStruct->Month - 1) * 2)) & 0x3)));

  calendarValue += (RTC_DateStruct->Date - 1);

  /* convert from days to seconds */
  calendarValue *= SECONDS_IN_1DAY;

  calendarValue += ((uint32_t)RTC_TimeStruct->Seconds +
                    ((uint32_t)RTC_TimeStruct->Minutes * SECONDS_IN_1MINUTE) +
                    ((uint32_t)RTC_TimeStruct->Hours * SECONDS_IN_1HOUR)) ;

  calendarValue = (calendarValue << RTC_N_PREDIV_S) + (RTC_PREDIV_S - RTC_TimeStruct->SubSeconds);

  return (calendarValue);
}

/*!
 * \brief Get system time
 * \param [IN]   pointer to ms
 *
 * \return uint32_t seconds
 */
uint32_t HW_RTC_GetCalendarTime(uint16_t *mSeconds)
{
  RTC_TimeTypeDef RTC_TimeStruct ;
  RTC_DateTypeDef RTC_DateStruct;
  uint32_t ticks;

  uint64_t calendarValue = RTC_GetCalendarValue(&RTC_DateStruct, &RTC_TimeStruct);

  uint32_t seconds = (uint32_t)(calendarValue >> RTC_N_PREDIV_S);

  ticks = (uint32_t) calendarValue & RTC_PREDIV_S;

  *mSeconds = RTC_Tick2ms(ticks);

  return seconds;
}

void HW_RTC_BKUPWrite(uint32_t Data0, uint32_t Data1)
{
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, Data0);
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, Data1);
}

void HW_RTC_BKUPRead(uint32_t *Data0, uint32_t *Data1)
{
  *Data0 = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0);
  *Data1 = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

