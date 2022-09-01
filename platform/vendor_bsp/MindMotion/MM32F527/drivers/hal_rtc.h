/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_RTC_H__
#define __HAL_RTC_H__

#include "hal_common.h"
#include "hal_rcc.h"

/*!
 * @addtogroup RTC
 * @{
 */

/*!
 * @addtogroup RTC_STATUS
 * @{
 */
#define RTC_STATUS_OPERATION           RTC_CRL_RTOFF_MASK    /*!< Status flag when write operation of RTC register is completed. */
#define RTC_STATUS_SYNC                RTC_CRL_RSF_MASK      /*!< Status flag when registers synchronized. */
/*!
 * @}
 */

/*!
 * @addtogroup RTC_INT
 * @{
 */
#define RTC_INT_SEC                   RTC_CRL_SECF_MASK     /*!< Interrupt enable when seconds interrupt generation or overflow of prescaler. After the flag is generated, RTC counter + 1. */
#define RTC_INT_ALARM                 RTC_CRL_ALRF_MASK     /*!< Interrupt enable when the alarm interrupt is generated, in other words, the value of the alarm counter is the same as the RTC counter. */
#define RTC_INT_OVERFLOW              RTC_CRL_OWF_MASK      /*!< Interrupt enable when overflow interrupt generaton or programmable counter overflow. */
/*!
 * @}
 */

/*!
 * @brief Define the callback function called when the RTC interrupt is done.
 */
typedef void (*RTC_Callback_1_Type)(void *param);

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref RTC_Init() to initialize the RTC module.
 */
typedef struct
{
    uint32_t              ClockFreq;    /*!< Used to calculate the overload value of prescaler, RTC_PRL[19:0] = (ClockFreq / CountFreq) - 1u. */
    uint32_t              CountFreq;    /*!< Used to set the clock hopping frequency of the counter, and the maximum frequency should not exceed 1 second. */
} RTC_Init_Type;

/*!
 * @brief This type of structure instance is used to set and get time when set time and get current time.
 */
typedef struct
{
    uint8_t    Secs;           /*!< Specify the clock seconds. */
    uint8_t    Mins;           /*!< Specify the clock minutes. */
    uint8_t    Hours;          /*!< Specify the clock hours. */
    uint8_t    Days;           /*!< Specify the clock days. */
    uint8_t    Months;         /*!< Specify the clock months. */
    uint16_t   Years;          /*!< Specify the clock years. */
    uint16_t   Lyears;         /*!< Specify term of lowest years. */
    uint32_t   AlarmTime;      /*!< Specify the number of seconds that alarm clock will respond. */
} RTC_Time_Type;

/*!
 * @brief Initialize the RTC module.
 *
 * @param RTCx RTC instance.
 * @param init Pointer to the initialization structure. See to @ref RTC_Init_Type.
 * @return None.
 */
void RTC_Init(RTC_Type * RTCx, RTC_Init_Type * init);

/*!
 * @brief Get the current status flags of the RTC module.
 *
 * @param RTCx RTC instance.
 * @return Status flags. See to @ref RTC_STATUS.
 */
uint32_t RTC_GetStatus(RTC_Type * RTCx);

/*!
 * @brief Clear the status flags of the RTC module.
 *
 * @param RTCx RTC instance.
 * @param status The mask code of the indicated flags to be clear. See to @ref RTC_STATUS.
 * @return None.
 */
void RTC_ClearStatus(RTC_Type * RTCx, uint32_t status);

/*!
 * @brief Configure the prescaler reload value.
 * 
 * The value of div determines the hopping frequency of the counter.
 * div = (ClockFreq / CountFreq) - 1u, CountFreq is hopping frequency of the counter, the counter jumps once every second at most.
 * The counter maximum jumps once every second, so the maximum CountFreq is no more than 1.
 *
 * @param RTCx RTC instance.
 * @param div Data value to be send into the prescaler register.
 * @return None.
 */
void RTC_PutPrescalerValue(RTC_Type * RTCx, uint32_t div);

/*!
 * @brief Configure the value at which the counter starts counting.
 *
 * The counter starts counting from value of cnt.
 *
 * @param RTCx RTC instance.
 * @param cnt The value of counter starts counting.
 * @return None.
 */
void RTC_PutCounterValue(RTC_Type * RTCx, uint32_t cnt);

/*!
 * @brief Configure alarm clock response time.
 *
 * @param RTCx RTC instance.
 * @param alarm Data value to be send into the alarm register.
 * @return None.
 */
void RTC_PutAlarmValue(RTC_Type * RTCx, uint32_t alarm);

/*!
 * @brief Get the current value from counter.
 *
 * @param RTCx RTC instance.
 * @return The data value from counter.
 */
uint32_t RTC_GetCounterValue(RTC_Type * RTCx);

/*!
 * @brief Get the value the alarm clock will respond to.
 *
 * @param RTCx RTC instance.
 * @return The value the alarm clock will respond to.
 */
uint32_t RTC_GetAlarmValue(RTC_Type * RTCx);

/*!
 * @brief Enabel interrupt of the RTC module.
 *
 * @param RTCx RTC instance.
 * @param interrupts Interrupt code masks. See to @ref RTC_INT.
 * @param enable 'true' to enable the indicated interrupts, 'false' to disable the indicated interrupts.
 * @return None.
 */
void RTC_EnableInterrupts(RTC_Type * RTCx, uint32_t interrupts, bool enable);

/*!
 * @brief Get the interrupts status flags of the RTC module.
 *
 * @param RTCx RTC instance.
 * @return Interrupt status flags. See to @ref RTC_INT.
 */
uint32_t RTC_GetInterruptStatus(RTC_Type * RTCx);

/*!
 * @brief Clear the interrupts status flags of the RTC module.
 *
 * @param RTCx RTC instance.
 * @param interrupts The mask codes of the indicated interrupt flags to be cleared. See to @ref RTC_INT.
 * @return None.
 */
void RTC_ClearInterruptStatus(RTC_Type * RTCx, uint32_t interrupts);

/*!
 * @brief Setup the initialization time of the RTC module.
 *
 * Use RTC_PutCounterValue(), calculate the year, month, day, hour, minute and second as the total number of seconds.
 *
 * @param RTCx RTC instance.
 * @param time Pointer to the time structure. See to @ref RTC_Time_Type.
 * @return 'true' to set time succeess, 'false' to set time failed.
 */
void RTC_SetTime(RTC_Type * RTCx, RTC_Time_Type * time);

/*!
 * @brief Calculate and get current time.
 *
 * Use RTC_GetCounterValue(), calculate the total number of seconds as year, month, day, hour, minute and second.
 *
 * @param RTCx RTC instance.
 * @param time Pointer to the time structure. See to @ref RTC_Time_Type.
 * @return None.
 */
void RTC_CalcTime(RTC_Type * RTCx, RTC_Time_Type * time);

/*!
 * @brief Set alarm time of the RTC module.
 *
 * Use RTC_GetCounterValue() and RTC_PutAlarmValue()
 * Get the current total seconds, plus the alarm response time, and put it into the alarm register.
 *
 * @param RTCx RTC instance.
 * @param time Pointer to the time structure. See to @ref RTC_Time_Type.
 * @return 'true' to set alarm time succeess, 'false' to set alarm time failed.
 */
void RTC_SetAlarm(RTC_Type * RTCx, RTC_Time_Type * time);

/*!
 *@}
 */

#endif /* __HAL_RTC_H__ */

