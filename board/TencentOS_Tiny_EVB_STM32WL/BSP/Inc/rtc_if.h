/**
  ******************************************************************************
  * @file   rtc_if.h
  * @author MCD Application Team
  * @brief  configuration of the rtc_if.c instances
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_IF_H__
#define __RTC_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32_timer.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/   
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */    
/*!
 * @brief Initializes the RTC timer
 * @note The timer is based on the RTC
 * @param none
 * @retval none
 */
void RTC_Init( void );





/*!
 * @brief Return the minimum timeout in ticks the RTC is able to handle
 * @param none
 * @retval minimum value for a timeout in ticks
 */
uint32_t RTC_GetMinimumTimeout( void );

/*!
 * @brief Get the RTC timer elapsed time since the last Alarm was set
 * @param none
 * @retval RTC Elapsed time in ticks
 */
uint32_t RTC_GetTimerElapsedTime(void);

/*!
 * @brief Get the RTC timer value
 * @param none
 * @retval RTC Timer value in ticks
 */
uint32_t RTC_GetTimerValue( void );

/*!
 * @brief set Time Reference set also the RTC_DateStruct and RTC_TimeStruct
 * @param none
 * @retval  Timer Reference Value in  Ticks
 */
uint32_t RTC_SetTimerContext( void );
  
/*!
 * @brief Get the RTC timer Reference
 * @param none
 * @retval Timer Value in  Ticks
 */
uint32_t RTC_GetTimerContext( void );

/*!
 * @brief RTC IRQ Handler on the RTC Alarm
 * @param none
 * @retval none
 */
void RTC_IrqHandler ( void );

/*!
 * @brief a delay of delay ms by polling RTC
 * @param delay in ms
 * @retval none
 */
void RTC_DelayMs(uint32_t delay);

/*!
 * @brief calculates the wake up time between wake up and mcu start
 * @note resolution in RTC_ALARM_TIME_BASE in timer ticks
 * @param none
 * @retval none
 */
void RTC_setMcuWakeUpTime(void);

/*!
 * @brief returns the wake up time in us
 * @param none
 * @retval wake up time in ticks
 */
int16_t RTC_getMcuWakeUpTime( void );

/*!
 * @brief converts time in ms to time in ticks
 * @param [IN] time in milliseconds
 * @retval returns time in timer ticks
 */
uint32_t RTC_ms2Tick( uint32_t timeMicroSec );

/*!
 * @brief converts time in ticks to time in ms
 * @param [IN] time in timer ticks
 * @retval returns time in timer milliseconds
 */
uint32_t RTC_Tick2ms( uint32_t tick );

/*!
 * \brief Get system time
 * \param [IN]   subSeconds in ms
 *
 * \uint32_t     seconds
 */
uint32_t HW_RTC_GetCalendarTime(uint16_t *subSeconds);

/*!
 * \brief Read from backup registers
 * \param [IN]  Data 0
 * \param [IN]  Data 1
 *
 */
void HW_RTC_BKUPRead(uint32_t *Data0, uint32_t *Data1);

/*!
 * \brief Write in backup registers
 * \param [IN]  Data 0
 * \param [IN]  Data 1
 *
 */

void HW_RTC_BKUPWrite(uint32_t Data0, uint32_t Data1);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
