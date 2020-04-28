/**
  ******************************************************************************
  * @file    stm8l15x_rtc.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the RTC 
  *          firmware library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************  
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_RTC_H
#define __STM8L15x_RTC_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup RTC
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/
/** @defgroup RTC_Exported_Types
  * @{
  */

/** @defgroup RTC_Weekdays
  * @{
  */ 
typedef enum
{
  RTC_Weekday_Monday      =  ((uint8_t)0x01), /*!< WeekDay is Monday */
  RTC_Weekday_Tuesday     =  ((uint8_t)0x02), /*!< WeekDay is Tuesday */
  RTC_Weekday_Wednesday   =  ((uint8_t)0x03), /*!< WeekDay is Wednesday */
  RTC_Weekday_Thursday    =  ((uint8_t)0x04), /*!< WeekDay is Thursday */
  RTC_Weekday_Friday      =  ((uint8_t)0x05), /*!< WeekDay is Friday */
  RTC_Weekday_Saturday    =  ((uint8_t)0x06), /*!< WeekDay is Saturday */
  RTC_Weekday_Sunday      =  ((uint8_t)0x07)  /*!< WeekDay is Sunday */
}
RTC_Weekday_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Months
  * @{
  */ 
typedef enum
{
  RTC_Month_January   =  ((uint8_t)0x01), /*!< Month is January */
  RTC_Month_February  =  ((uint8_t)0x02), /*!< Month is February */
  RTC_Month_March     =  ((uint8_t)0x03), /*!< Month is March */
  RTC_Month_April     =  ((uint8_t)0x04), /*!< Month is April */
  RTC_Month_May       =  ((uint8_t)0x05), /*!< Month is May */
  RTC_Month_June      =  ((uint8_t)0x06), /*!< Month is June */
  RTC_Month_July      =  ((uint8_t)0x07), /*!< Month is July */
  RTC_Month_August    =  ((uint8_t)0x08), /*!< Month is August */
  RTC_Month_September =  ((uint8_t)0x09), /*!< Month is September */
  RTC_Month_October   =  ((uint8_t)0x10), /*!< Month is October */
  RTC_Month_November  =  ((uint8_t)0x11), /*!< Month is November */
  RTC_Month_December  =  ((uint8_t)0x12)  /*!< Month is December */
}
RTC_Month_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Hour_Format
  * @{
  */ 
typedef enum
{
  RTC_HourFormat_24  = ((uint8_t)0x00), /*!< Hour Format is 24H */
  RTC_HourFormat_12  = ((uint8_t)0x40)  /*!< Hour Format is 12H (using AM/PM) */
}
RTC_HourFormat_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Time
  * @{
  */ 
typedef enum
{
  RTC_H12_AM     = ((uint8_t)0x00), /*!< AM/PM notation is AM or 24 hour format */
  RTC_H12_PM     = ((uint8_t)0x40)  /*!< AM/PM notation is PM  */
}
RTC_H12_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Alarm_WeekDay_Selection
  * @{
  */ 
typedef enum
{
  RTC_AlarmDateWeekDaySel_Date     = ((uint8_t)0x00), /*!< Date/WeekDay selection is Date */
  RTC_AlarmDateWeekDaySel_WeekDay  = ((uint8_t)0x40)  /*!< Date/WeekDay selection is WeekDay */
}
RTC_AlarmDateWeekDaySel_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Alarm_Mask
  * @{
  */ 
typedef enum
{
  RTC_AlarmMask_None         =  ((uint8_t)0x00), /*!< Alarm Masks disabled */
  RTC_AlarmMask_Seconds      =  ((uint8_t)0x80), /*!< Alarm Seconds Mask */
  RTC_AlarmMask_Minutes      =  ((uint8_t)0x40), /*!< Alarm Minutes Mask */
  RTC_AlarmMask_Hours        =  ((uint8_t)0x20), /*!< Alarm Hours Mask */
  RTC_AlarmMask_DateWeekDay  =  ((uint8_t)0x10), /*!< Alarm Date/WeekDay Mask */
  RTC_AlarmMask_All          =  ((uint8_t)0xF0)  /*!< Alarm All Mask are enabled */
}
RTC_AlarmMask_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Sub_Second_Alarm_Mask
  * @{
  */ 
typedef enum
{
  RTC_AlarmSubSecondMask_All      =  ((uint8_t)0x00), /*!< All Alarm SS fields are masked. There is no comparison on sub seconds for Alarm */
  RTC_AlarmSubSecondMask_None     =  ((uint8_t)0x0F), /*!< SS[14:0] are compared and must match to activate alarm. */
  RTC_AlarmSubSecondMask_SS14_1   =  ((uint8_t)0x01), /*!< SS[14:1] are not used in Alarm comparison. Only SS[0] is compared. */
  RTC_AlarmSubSecondMask_SS14_2   =  ((uint8_t)0x02), /*!< SS[14:2] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_3   =  ((uint8_t)0x03), /*!< SS[14:3] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_4   =  ((uint8_t)0x04), /*!< SS[14:4] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_5   =  ((uint8_t)0x05), /*!< SS[14:5] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_6   =  ((uint8_t)0x06), /*!< SS[14:6] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_7   =  ((uint8_t)0x07), /*!< SS[14:7] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_8   =  ((uint8_t)0x08), /*!< SS[14:8] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_9   =  ((uint8_t)0x09), /*!< SS[14:9] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_10  =  ((uint8_t)0x0A), /*!< SS[14:10] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_11  =  ((uint8_t)0x0B), /*!< SS[14:11] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_12  =  ((uint8_t)0x0C), /*!< SS[14:12] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14_13  =  ((uint8_t)0x0D), /*!< SS[14:13] are not used in Alarm comparison. Only SS[1:0] are compared */
  RTC_AlarmSubSecondMask_SS14     =  ((uint8_t)0x0E) /*!< SS[14] is not used in Alarm comparison. Only SS[13:0] are compared */
}
RTC_AlarmSubSecondMask_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Wakeup_Clock
  * @{
  */ 
typedef enum
{
  RTC_WakeUpClock_RTCCLK_Div16    = ((uint8_t)0x00), /*!< (RTC clock) div 16*/
  RTC_WakeUpClock_RTCCLK_Div8     = ((uint8_t)0x01), /*!< (RTC clock) div 8*/
  RTC_WakeUpClock_RTCCLK_Div4     = ((uint8_t)0x02), /*!< (RTC clock) div 4*/
  RTC_WakeUpClock_RTCCLK_Div2     = ((uint8_t)0x03), /*!< (RTC clock) div 2*/
  RTC_WakeUpClock_CK_SPRE_16bits  = ((uint8_t)0x04), /*!< CK SPRE with a counter from 0x0000 to 0xFFFF */
  RTC_WakeUpClock_CK_SPRE_17bits  = ((uint8_t)0x06)  /*!< CK SPRE with a counter from 0x10000 to 0x1FFFF */
}
RTC_WakeUpClock_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Output_Selection
  * @{
  */ 
typedef enum
{
  RTC_Output_Disable = ((uint8_t)0x00), /*!< RTC Alternate function Output is disabled  */
  RTC_Output_Alarm   = ((uint8_t)0x20), /*!< RTC Alternate function Output is the Alarm A event*/
  RTC_Output_WakeUp  = ((uint8_t)0x60)  /*!< RTC Alternate function Output is the WakeUp event */
}
RTC_Output_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Output_Polarity
  * @{
  */ 
typedef enum
{
  RTC_OutputPolarity_High  = ((uint8_t)0x00), /*!< RTC Alternate function Output Polarity is High  */
  RTC_OutputPolarity_Low   = ((uint8_t)0x10)  /*!< RTC Alternate function Output Polarity is Low  */
}
RTC_OutputPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Calibration_Output
  * @{
  */ 
typedef enum
{
  RTC_CalibOutput_512Hz  = ((uint8_t)0x00), /*!< RTC Calibration Output is 512Hz */
  RTC_CalibOutput_1Hz    = ((uint8_t)0x80)  /*!< RTC Calibration Output is 1Hz */
}
RTC_CalibOutput_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_DayLight_Saving
  * @{
  */ 
typedef enum
{
  RTC_DayLightSaving_SUB1H  = ((uint8_t)0x02), /*!< Substract 1 hour to the current Time (Winter Time Adjustment) */
  RTC_DayLightSaving_ADD1H  = ((uint8_t)0x01)  /*!< Add 1 hour to the current Time (Summer Time Adjustment) */
}
RTC_DayLightSaving_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Store_Operation
  * @{
  */ 
typedef enum
{
  RTC_StoreOperation_Set   = ((uint8_t)0x04), /*!< Store Operation Set */
  RTC_StoreOperation_Reset = ((uint8_t)0x00)  /*!< Store Operation Reset */
}
RTC_StoreOperation_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Input_Parameter_Format
  * @{
  */ 
typedef enum
{
  RTC_Format_BIN  = ((uint8_t)0x00), /*!< Binary Format is used  */
  RTC_Format_BCD  = ((uint8_t)0x01)  /*!< BCD Format is used  */
}
RTC_Format_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Flags
  * @{
  */ 
typedef enum
{
  RTC_FLAG_TAMP3F  = ((uint16_t)0x0080), /*!< TAMPER3 detection  Flag. If set, tamper detection event is detected on tamper input 3 */
  RTC_FLAG_TAMP2F  = ((uint16_t)0x0040), /*!< TAMPER2 detection  Flag. If set, tamper detection event is detected on tamper input 2 */
  RTC_FLAG_TAMP1F  = ((uint16_t)0x0020), /*!< TAMPER1 detection  Flag. If set, tamper detection event is detected on tamper input 1 */

  RTC_FLAG_WUTF    = ((uint16_t)0x0004), /*!< Wake up Timer Flag. If set, the Wake Up down counter reaches 0 */
  RTC_FLAG_ALRAF   = ((uint16_t)0x0001), /*!< Alarm A Flag. If set, the Time/Date registers matches Alarm A registers */

  RTC_FLAG_INITF   = ((uint16_t)0x4000), /*!< Initialisation Flag. If set,Calender registers has been initialized  */
  RTC_FLAG_RSF     = ((uint16_t)0x2000), /*!< Registers Synchronization Flag. If set,Calender registers synchronized  */
  RTC_FLAG_INITS   = ((uint16_t)0x1000), /*!< Initialisation Status Flag. If set,Calender  update is allowed  */

  RTC_FLAG_SHPF    = ((uint16_t)0x0800), /*!< Shift operation pending Flag. This flag is set by hardware as soon as a shift operation is
                                              initiated by a write to the RTC_SHIFTRL. It is cleared by hardware as soon as the corresponding
                                              shift operation has completed.  */

  RTC_FLAG_WUTWF   = ((uint16_t)0x0400), /*!< Wake up Timer write Flag. If set, Wake up Timer update is allowed */
  RTC_FLAG_RECALPF = ((uint16_t)0x0200), /*!<   Recalibration pending Flag, The status flag RECALPF is automatically set to ‘1’ when software
                                                writes to the register RTC_CALRL, indicating that the RTC_CALRx registers are blocked.
                                                When the new calibration settings are taken into account, this Flag returns by hardware to ‘0’. */
  RTC_FLAG_ALRAWF  = ((uint16_t)0x0100)  /*!< Alarm  A write Flag. If set, Alarm A update is allowed   */
}
RTC_Flag_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Interrupts
  * @{
  */ 
typedef enum
{
  RTC_IT_WUT  = ((uint16_t)0x0040),  /*!< Wake up Timer Interrupt */
  RTC_IT_ALRA = ((uint16_t)0x0010),  /*!< Alarm A Interrupt */
  RTC_IT_TAMP = ((uint16_t)0x0F01)   /*!< Tamper Interrupt */
}
RTC_IT_TypeDef;

/**
  * @}
  */

/** @defgroup RTC_Tamper_Level
  * @{
  */ 
typedef enum
{
  RTC_TamperLevel_Low    = ((uint8_t)0x00), /*!< Tamper staying low triggers a tamper detection event. */
  RTC_TamperLevel_High   = ((uint8_t)0x54)  /*!< Tamper staying high triggers a tamper detection event. */
}
RTC_TamperLevel_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Tamper_State
  * @{
  */ 
typedef enum
{
  RTC_TamperState_Disable  = ((uint8_t)0x00), /*!< Tamper State is Disable */
  RTC_TamperState_Enable   = ((uint8_t)0x01)  /*!< Tamper State is Enable  */
}
RTC_TamperState_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Tamper_definition
  * @{
  */ 
typedef enum
{
  RTC_Tamper_1 = ((uint8_t)0x02),  /*!< Tamper 1 selection */
  RTC_Tamper_2 = ((uint8_t)0x08),  /*!< Tamper 2 selection */
  RTC_Tamper_3 = ((uint8_t)0x20)   /*!< Tamper 3 selection */
}
RTC_Tamper_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Tamper_Precharge_Duration
  * @{
  */ 
typedef enum
{
  RTC_TamperPrechargeDuration_None    = ((uint8_t)0x80),  /*!< Tamper pins are not precharged before sampling */
  RTC_TamperPrechargeDuration_1RTCCLK = ((uint8_t)0x00),  /*!< Tamper pins are precharged before sampling during 1 RTCCLK cycle  */
  RTC_TamperPrechargeDuration_2RTCCLK = ((uint8_t)0x20),  /*!< Tamper pins are precharged before sampling during 2 RTCCLK cycles */
  RTC_TamperPrechargeDuration_4RTCCLK = ((uint8_t)0x40),  /*!< Tamper pins are precharged before sampling during 4 RTCCLK cycles */
  RTC_TamperPrechargeDuration_8RTCCLK = ((uint8_t)0x60)   /*!< Tamper pins are precharged before sampling during 8 RTCCLK cycles */
}
RTC_TamperPrechargeDuration_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Tamper_Filter
  * @{
  */ 
typedef enum
{
  RTC_TamperFilter_1Sample    = ((uint8_t)0x00), /*!< Tamper is activated after 1 sample at the active level */
  RTC_TamperFilter_2Sample    = ((uint8_t)0x08), /*!< Tamper is activated after 2 consecutive samples at the active level. */
  RTC_TamperFilter_4Sample    = ((uint8_t)0x10), /*!< Tamper is activated after 4 consecutive samples at the active level. */
  RTC_TamperFilter_8Sample    = ((uint8_t)0x18)  /*!< Tamper is activated after 8 consecutive samples at the active level. */
}
RTC_TamperFilter_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Tamper_Sampling_Frequency
  * @{
  */ 
typedef enum
{
  RTC_TamperSamplingFreq_RTCCLK_Div32768  = ((uint8_t)0x00), /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 32768 */
  RTC_TamperSamplingFreq_RTCCLK_Div16384  = ((uint8_t)0x01), /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 16384 */
  RTC_TamperSamplingFreq_RTCCLK_Div8192   = ((uint8_t)0x02), /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 8192  */
  RTC_TamperSamplingFreq_RTCCLK_Div4096   = ((uint8_t)0x03), /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 4096  */
  RTC_TamperSamplingFreq_RTCCLK_Div2048   = ((uint8_t)0x04), /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 2048  */
  RTC_TamperSamplingFreq_RTCCLK_Div1024   = ((uint8_t)0x05), /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 1024  */
  RTC_TamperSamplingFreq_RTCCLK_Div512    = ((uint8_t)0x06), /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 512   */
  RTC_TamperSamplingFreq_RTCCLK_Div256    = ((uint8_t)0x07)  /*!< Tamper inputs are sampled with a frequency =  RTCCLK / 256   */
}
RTC_TamperSamplingFreq_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Shift_Add_1s
  * @{
  */ 
typedef enum
{
  RTC_ShiftAdd1S_Set    = ((uint8_t)0x80), /*!< Add 1 Second */
  RTC_ShiftAdd1S_Reset  = ((uint8_t)0x00)  /*!< Do not Add 1 Second */
}
RTC_ShiftAdd1S_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Smooth_Calibration_Period
  * @{
  */ 
typedef enum
{
  RTC_SmoothCalibPeriod_32sec  = ((uint8_t)0x00), /*!<  if RTCCLK = 32768 Hz, Smooth calibration period is 32s,  else 2exp20 RTCCLK seconds */
  RTC_SmoothCalibPeriod_16sec  = ((uint8_t)0x20), /*!<  if RTCCLK = 32768 Hz, Smooth calibration period is 16s, else 2exp19 RTCCLK seconds */
  RTC_SmoothCalibPeriod_8sec   = ((uint8_t)0x40)  /*!<  if RTCCLK = 32768 Hz, Smooth calibration period is 8s, else 2exp18 RTCCLK seconds */
}
RTC_SmoothCalibPeriod_TypeDef;

/**
  * @}
  */
  
/** @defgroup RTC_Smooth_Calibration_Pulses
  * @{
  */ 
typedef enum
{
  RTC_SmoothCalibPlusPulses_Set    = ((uint8_t)0x80), /*!<  The number of RTCCLK pulses added during a X -second window =  Y  - CALM[8:0].  
                                                             with Y = 512, 256, 128 when X = 32, 16, 8 */
  RTC_SmoothCalibPlusPulses_Reset  = ((uint8_t)0x00)  /*!<  The number of RTCCLK pulses substituted during a 32-second window =  CALM[8:0]. */

}
RTC_SmoothCalibPlusPulses_TypeDef;

/**
  * @}
  */

/**
  * @brief RTC Init structure definition
  */

typedef struct
{

  RTC_HourFormat_TypeDef RTC_HourFormat; /*!< RTC Hour Format */

  uint8_t RTC_AsynchPrediv;   /*!< RTC Asynchronous Predivider.
                                   This parameter can be any value from 
                                   0x00 to 0x7F.*/

  uint16_t RTC_SynchPrediv;   /*!< RTC Synchronous Predivider.
                                   This parameter can be any value from 
                                   0x0000 to 0x7FFF.*/
}
RTC_InitTypeDef;


/**
  * @brief RTC Time structure definition
  */
typedef struct
{
  uint8_t RTC_Hours;       /*!< RTC Hours.
                            If Binary format is selected :
                               - If RTC_Hour format is RTC_Hour format_12, 
                                 this parameter can be any value from 1 to 12.
                               - If RTC_Hour format is RTC_Hour format_24, 
                                 this parameter can be any value from 0 to 23.
                            If BCD format is selected :
                               - If RTC_Hour format is RTC_Hour format_12, 
                                 this parameter can be any BCD value from 
                                 0x01 to 0x12.
                               - If RTC_Hour format is RTC_Hour format_24, 
                                 this parameter can be any BCD value  from 
                                 0x00 to 0x23.*/
  uint8_t RTC_Minutes;     /*!< RTC Minutes.
                             If Binary format is selected, this parameter can
                                be any value from 0 to 59.
                             If BCD format is selected, this parameter can
                                be any BCD value from 0x00 to 0x59.*/
  uint8_t RTC_Seconds;     /*!< RTC Seconds.
                             If Binary format is selected, this parameter can
                                be any value from 0 to 59.
                             If BCD format is selected, this parameter can
                                be any BCD value from 0x00 to 0x59.*/
  RTC_H12_TypeDef RTC_H12;  /*!< RTC 12-hour clock period (AM/PM) */
}
RTC_TimeTypeDef;

/**
  * @brief RTC Date structure definition
  */
typedef struct
{
  RTC_Weekday_TypeDef  RTC_WeekDay; /*!< The RTC Calender Weekday. */

  RTC_Month_TypeDef RTC_Month;      /*!< The RTC Calender Month. */

  uint8_t RTC_Date;                 /*!< The RTC Calender Date.
                                      If Binary format is selected, this 
                                         parameter can be any value from 1 to 31.
                                      If BCD format is selected, this parameter
                                         can be any BCD value from 0x01 to 0x31.*/

  uint8_t RTC_Year;                 /*!< The RTC Calender Date.
                                      If Binary format is selected, this parameter
                                         can be any value from 0 to 99.
                                      If BCD format is selected, this parameter
                                         can be any BCD value from 0x00 to 0x99.*/
}
RTC_DateTypeDef;

/**
  * @brief RTC Alarm structure definition
  */
typedef struct
{
  RTC_TimeTypeDef RTC_AlarmTime;                           /*!< RTC Alarm Time */

  uint8_t RTC_AlarmMask;                                   /*!< The RTC Alarm Fields Masks. */

  RTC_AlarmDateWeekDaySel_TypeDef RTC_AlarmDateWeekDaySel; /*!< The RTC Alarm Date/WeekDay selection. */

  uint8_t RTC_AlarmDateWeekDay;                            /*!< The RTC Alarm Date/WeekDay value.
                                                               - If RTC Alarm Date/WeekDay selection is Date
                                                                and if If Binary format is selected, this
                                                                parameter can be any value from 1 to 31.
                                                               - If RTC Alarm Date/WeekDay selection is WeekDay,
                                                                 this parameter can be one of the
                                                                 @ref RTC_Weekday_TypeDef enumeration.*/
}
RTC_AlarmTypeDef;
  /**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/** @defgroup RTC_Exported_Macros
* @{
*/

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender Hour format
  */

#define IS_RTC_HOUR_FORMAT(FORMAT) (((FORMAT) == RTC_HourFormat_12) || \
                                    ((FORMAT) == RTC_HourFormat_24))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Asynchronous Predivider
  */
#define IS_RTC_ASYNCH_PREDIV(PREDIV) ((PREDIV) <= 0x7F)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Synchronous Predivider
  */
#define IS_RTC_SYNCH_PREDIV(PREDIV) ((PREDIV) <= 0x7FFF)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender Hours value (format 12h)
  */
#define IS_RTC_HOUR12_MAX(HOUR) ((HOUR) <= (uint8_t)12)
#define IS_RTC_HOUR12_MIN(HOUR) ((HOUR) > (uint8_t)0)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender Hours value (format 24h)
  */
#define IS_RTC_HOUR24(HOUR) ((HOUR) <= 23)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender minutes value
  */
#define IS_RTC_MINUTES(MINUTES) ((MINUTES) <= 59)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender seconds value
  */
#define IS_RTC_SECONDS(SECONDS) ((SECONDS) <= 59)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender H12 mode
  */
#define IS_RTC_H12(PM) (((PM) == (RTC_H12_AM)) || ((PM) == (RTC_H12_PM)))


/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender Year value
  */
#define IS_RTC_YEAR(YEAR) ((YEAR) <= 99)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender month value
  */
#define IS_RTC_MONTH_MAX(MONTH) ((MONTH) <= (uint8_t)12)
#define IS_RTC_MONTH_MIN(MONTH) ((MONTH) >= (uint8_t)1)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender Date value
  */
#define IS_RTC_DATE_MAX(DATE) ((DATE) <= (uint8_t)31)
#define IS_RTC_DATE_MIN(DATE) ((DATE) >= (uint8_t)1)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Calender Week day value
  */
#define IS_RTC_WEEKDAY(WEEKDAY) (((WEEKDAY) ==   RTC_Weekday_Monday)    || \
                                 ((WEEKDAY) ==   RTC_Weekday_Tuesday)   || \
                                 ((WEEKDAY) ==   RTC_Weekday_Wednesday) || \
                                 ((WEEKDAY) ==   RTC_Weekday_Thursday)  || \
                                 ((WEEKDAY) ==   RTC_Weekday_Friday)    || \
                                 ((WEEKDAY) ==   RTC_Weekday_Saturday)  || \
                                 ((WEEKDAY) ==   RTC_Weekday_Sunday))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Alarm Date/week day  selection
  */
#define IS_RTC_ALARM_DATEWEEKDAY_SEL(SEL) (((SEL) == RTC_AlarmDateWeekDaySel_Date) || \
                                           ((SEL) == RTC_AlarmDateWeekDaySel_WeekDay))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Alarm Mask
  */
#define IS_RTC_ALARM_MASK(MASK) (((MASK) & 0x0F) == (uint8_t)(RESET))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        wakeup clock source
  */
#define IS_RTC_WAKEUP_CLOCK(CLOCK) (((CLOCK) == RTC_WakeUpClock_RTCCLK_Div16) || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div8)  || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div4)  || \
                                    ((CLOCK) == RTC_WakeUpClock_RTCCLK_Div2)  || \
                                    ((CLOCK) == RTC_WakeUpClock_CK_SPRE_16bits) || \
                                    ((CLOCK) == RTC_WakeUpClock_CK_SPRE_17bits))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Output selection
  */
#define IS_RTC_OUTPUT_SEL(SEL) (((SEL) == RTC_Output_Disable) || \
                                ((SEL) == RTC_Output_Alarm) || \
                                ((SEL) == RTC_Output_WakeUp))
/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Output polarity
  */
#define IS_RTC_OUTPUT_POL(POL) (((POL) == RTC_OutputPolarity_High) || \
                                ((POL) == RTC_OutputPolarity_Low))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Daylight saving mode
  */
#define IS_RTC_DAYLIGHT_SAVING(SAVE) (((SAVE) ==  RTC_DayLightSaving_SUB1H) || \
                                      ((SAVE) == RTC_DayLightSaving_ADD1H))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Store Operation status
  */
#define IS_RTC_STORE_OPERATION(OP) (((OP) ==  RTC_StoreOperation_Set) || \
                                    ((OP) == RTC_StoreOperation_Reset))


/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        format (bin/bcd) for data user insert
  */
#define IS_RTC_FORMAT(FORMAT) (((FORMAT) == RTC_Format_BIN) || \
                               ((FORMAT) == RTC_Format_BCD))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Flag to get
  */
#define IS_RTC_GET_FLAG(FLAG) (((FLAG) == RTC_FLAG_WUTF)  || \
                               ((FLAG) == RTC_FLAG_ALRAF) || \
                               ((FLAG) == RTC_FLAG_INITF) || \
                               ((FLAG) == RTC_FLAG_RSF)   || \
                               ((FLAG) == RTC_FLAG_INITS) || \
                               ((FLAG) == RTC_FLAG_WUTWF) || \
                               ((FLAG) == RTC_FLAG_TAMP3F)|| \
                               ((FLAG) == RTC_FLAG_TAMP2F)|| \
                               ((FLAG) == RTC_FLAG_TAMP1F)|| \
                               ((FLAG) == RTC_FLAG_SHPF)  || \
                               ((FLAG) == RTC_FLAG_RECALPF) || \
                               ((FLAG) == RTC_FLAG_ALRAWF))

/* RTC_FLAG_ALRAWF is for Medium Density only but supported by High Density 
   Devices*/

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Flag to clear
  */
#define RTC_FLAG_CLRBLE (RTC_FLAG_RSF | RTC_FLAG_ALRAF | RTC_FLAG_WUTF| RTC_FLAG_TAMP1F|RTC_FLAG_TAMP2F|RTC_FLAG_TAMP3F)
#define IS_RTC_CLEAR_FLAG(FLAG) (((FLAG) != RESET) && ((uint16_t)((FLAG) & (uint16_t)(~(RTC_FLAG_CLRBLE))) == RESET))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *       Interrupt to configure
  */
#define IS_RTC_CONFIG_IT(IT) (((uint16_t)(IT) != (uint8_t)RESET) && ((uint16_t)((uint16_t)(IT) & (uint16_t)(~(uint16_t)(RTC_IT_WUT|RTC_IT_ALRA|RTC_IT_TAMP))) == (uint8_t)RESET))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Interrupt to get
  */
#define IS_RTC_GET_IT(IT) (((IT) == RTC_IT_WUT) || \
                           ((IT) == RTC_IT_ALRA)|| \
                           ((IT) == RTC_IT_TAMP) )

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Interrupt to clear
  */
#define IS_RTC_CLEAR_IT(IT)  (((IT) != RESET) && ((uint16_t)((IT) & (uint16_t)(~(uint16_t)(RTC_IT_WUT|RTC_IT_ALRA|RTC_IT_TAMP))) == RESET))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Tamper Levels
  */
#define IS_RTC_TAMPER_LEVEL(LEVEL) (((LEVEL) == RTC_TamperLevel_Low) || \
                                    ((LEVEL) == RTC_TamperLevel_High))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *       Tamper
  */
#define NOT_CORRECT_TAMPER  (uint8_t)~(uint8_t)( RTC_Tamper_1 | \
                                                 RTC_Tamper_2 | \
                                                 RTC_Tamper_3)
#define IS_RTC_TAMPER(TAMPER) (((uint8_t)((TAMPER) & NOT_CORRECT_TAMPER) == RESET) && ((TAMPER) != RESET))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Tampers Filter
  */
#define IS_RTC_TAMPER_FILTER(SEL) (((SEL) == RTC_TamperFilter_1Sample) || \
                                   ((SEL) == RTC_TamperFilter_2Sample) || \
                                   ((SEL) == RTC_TamperFilter_4Sample) || \
                                   ((SEL) == RTC_TamperFilter_8Sample))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Tampers Sampling Frequencies
  */
#define IS_RTC_TAMPER_SAMPLING_FREQ(SEL) ((SEL) <= RTC_TamperSamplingFreq_RTCCLK_Div256)


/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Tampers Pins precharge duration
  */
#define IS_RTC_TAMPER_PINS_PRECHAR_DURATION(SEL) (((SEL) == RTC_TamperPrechargeDuration_None) || \
                                                  ((SEL) == RTC_TamperPrechargeDuration_1RTCCLK) || \
                                                  ((SEL) == RTC_TamperPrechargeDuration_2RTCCLK) || \
                                                  ((SEL) == RTC_TamperPrechargeDuration_4RTCCLK) || \
                                                  ((SEL) == RTC_TamperPrechargeDuration_8RTCCLK))

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Smooth calibration period
  */
#define  IS_RTC_SMOOTH_CALIB_PERIOD(SEL) (((SEL) == RTC_SmoothCalibPeriod_32sec) || \
                                          ((SEL) == RTC_SmoothCalibPeriod_16sec) || \
                                          ((SEL) == RTC_SmoothCalibPeriod_8sec))
    
/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Smooth calibration Plus pulses
  */
#define  IS_RTC_SMOOTH_CALIB_PLUS(SEL) (((SEL) == RTC_SmoothCalibPlusPulses_Set) || \
                                        ((SEL) == RTC_SmoothCalibPlusPulses_Reset))
/**
  * @brief Macro used by the assert_param function in order to check the used  
  *        Smooth calibration Minus  pulses
  */
#define  IS_RTC_SMOOTH_CALIB_MINUS(VALUE) ((VALUE) <= 0x01FF)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Output Selection
  */
#define  IS_RTC_OUTPUT_SELECT(SEL) (((SEL) == RTC_Output_Disable) || \
                                    ((SEL) == RTC_Output_Alarm) || \
                                    ((SEL) == RTC_Output_WakeUp))

/**
  * @brief Macro used by the assert_param function in order to check the 
  *        used calibration Output Selection
  */
#define  IS_RTC_CALOUTPUT_SELECT(SEL) (((SEL) == RTC_CalibOutput_512Hz) || \
                                       ((SEL) == RTC_CalibOutput_1Hz))
                                       
/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Alarm sub second value
  */
#define IS_RTC_ALARM_SS_VALUE(SS) ((SS) <= 0x7FFF)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        Alarm sub second mask
  */
#define IS_RTC_ALARM_SS_MASK(MASK) ((MASK) <= 0x0F)

/**
  * @brief Macro used by the assert_param function in order to check the used 
  *        fraction of seconds to sub
  */
#define IS_RTC_SHIFT_SUBFS(FS) ((FS) <= 0x7FFF)

/**
  * @brief Macro used by the assert_param function in order to check the 
  *        parameter of 1 second to add
  */
#define IS_RTC_SHIFT_ADD1S(VAL) (((VAL) == RTC_ShiftAdd1S_Set) || \
                                 ((VAL) == RTC_ShiftAdd1S_Reset))
                                 
/**
  * @}
  */
/* Exported functions ------------------------------------------------------- */

/*  Function used to set the RTC configuration to the default reset state *****/ 
ErrorStatus RTC_DeInit(void);

/* Initialization and Configuration functions *********************************/ 
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);
void RTC_WriteProtectionCmd(FunctionalState NewState);
ErrorStatus RTC_EnterInitMode(void);
void RTC_ExitInitMode(void);
ErrorStatus RTC_WaitForSynchro(void);
void RTC_RatioCmd(FunctionalState NewState);
void RTC_BypassShadowCmd(FunctionalState NewState);

/* Time and Date configuration functions **************************************/ 
ErrorStatus RTC_SetTime(RTC_Format_TypeDef RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_GetTime(RTC_Format_TypeDef RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
uint16_t RTC_GetSubSecond(void);
ErrorStatus RTC_SetDate(RTC_Format_TypeDef RTC_Format, RTC_DateTypeDef* RTC_DateStruct);
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct);
void RTC_GetDate(RTC_Format_TypeDef RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

/* Alarm configuration functions  *********************************************/ 
void RTC_SetAlarm(RTC_Format_TypeDef RTC_Format,  RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_GetAlarm(RTC_Format_TypeDef RTC_Format,  RTC_AlarmTypeDef* RTC_AlarmStruct);
ErrorStatus RTC_AlarmCmd(FunctionalState NewState);
ErrorStatus RTC_AlarmSubSecondConfig(uint16_t RTC_AlarmSubSecondValue,
                                     RTC_AlarmSubSecondMask_TypeDef RTC_AlarmSubSecondMask);

/* WakeUp Timer configuration functions ***************************************/ 
void RTC_WakeUpClockConfig(RTC_WakeUpClock_TypeDef RTC_WakeUpClock);
void RTC_SetWakeUpCounter(uint16_t RTC_WakeupCounter);
uint16_t RTC_GetWakeUpCounter(void);
ErrorStatus RTC_WakeUpCmd(FunctionalState NewState);

/* Daylight Saving configuration functions ************************************/ 
void RTC_DayLightSavingConfig(RTC_DayLightSaving_TypeDef RTC_DayLightSaving,
                              RTC_StoreOperation_TypeDef RTC_StoreOperation);
RTC_StoreOperation_TypeDef  RTC_GetStoreOperation(void);

/* Output pin Configuration function ******************************************/ 
void RTC_OutputConfig(RTC_Output_TypeDef RTC_Output,
                      RTC_OutputPolarity_TypeDef RTC_OutputPolarity);

/* Shift control  synchronisation function ************************************/ 
ErrorStatus RTC_SynchroShiftConfig(RTC_ShiftAdd1S_TypeDef RTC_ShiftAdd1S,
                                   uint16_t RTC_ShiftSubFS);

/* Smooth Calibration functions  **********************************************/
ErrorStatus RTC_SmoothCalibConfig(RTC_SmoothCalibPeriod_TypeDef RTC_SmoothCalibPeriod,
                                  RTC_SmoothCalibPlusPulses_TypeDef RTC_SmoothCalibPlusPulses,
                                  uint16_t RTC_SmouthCalibMinusPulsesValue);

/* Calibration configuration functions ****************************************/ 
void RTC_CalibOutputConfig(RTC_CalibOutput_TypeDef RTC_CalibOutput);
void RTC_CalibOutputCmd(FunctionalState NewState);

/* Tampers configuration functions ********************************************/ 
void RTC_TamperLevelConfig(RTC_Tamper_TypeDef RTC_Tamper,
                           RTC_TamperLevel_TypeDef RTC_TamperLevel);
void RTC_TamperFilterConfig(RTC_TamperFilter_TypeDef RTC_TamperFilter);
void RTC_TamperSamplingFreqConfig(RTC_TamperSamplingFreq_TypeDef RTC_TamperSamplingFreq);
void RTC_TamperPinsPrechargeDuration(RTC_TamperPrechargeDuration_TypeDef RTC_TamperPrechargeDuration);
void RTC_TamperCmd(RTC_Tamper_TypeDef RTC_Tamper,
                   FunctionalState NewState);

/* Interrupts and flags management functions **********************************/ 
void RTC_ITConfig(RTC_IT_TypeDef RTC_IT, FunctionalState NewState);
FlagStatus RTC_GetFlagStatus(RTC_Flag_TypeDef RTC_FLAG);
void RTC_ClearFlag(RTC_Flag_TypeDef RTC_FLAG);
ITStatus RTC_GetITStatus(RTC_IT_TypeDef RTC_IT);
void RTC_ClearITPendingBit(RTC_IT_TypeDef RTC_IT);

#endif /*__STM8L15x_RTC_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
