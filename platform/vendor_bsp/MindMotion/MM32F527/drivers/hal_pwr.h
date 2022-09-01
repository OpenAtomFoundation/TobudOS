/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_PWR_H__
#define __HAL_PWR_H__

#include "hal_common.h"

/*!
 * @addtogroup PWR
 * @{
 */

/*!
 * @brief PWR driver version number.
 */
#define PWR_DRIVER_VERSION 0u /*!< PWR_0. */

/*!
 * @addtogroup PWR_WAKEUP_PIN
 * @{
 */
#define PWR_WAKEUP_PIN_0  (1u << 0u) /*!< Wake up pin 0 selected. */
#define PWR_WAKEUP_PIN_1  (1u << 1u) /*!< Wake up pin 1 selected. */
#define PWR_WAKEUP_PIN_2  (1u << 2u) /*!< Wake up pin 2 selected. */
#define PWR_WAKEUP_PIN_3  (1u << 3u) /*!< Wake up pin 3 selected. */
#define PWR_WAKEUP_PIN_4  (1u << 4u) /*!< Wake up pin 4 selected. */
#define PWR_WAKEUP_PIN_5  (1u << 5u) /*!< Wake up pin 5 selected. */
/*!
 * @}
 */

/*!
 * @addtogroup PWR_STATUS
 * @{
 */
#define PWR_STATUS_LOWPOWERRUN    (1u << 0u) /*!< Low power run mode flag. */
#define PWR_STATUS_STANDBYRESET   (1u << 1u) /*!< Standby reset flag. */
/*!
 * @}
 */

/*!
 * @brief PWR standby wake up delay type.
 */
typedef enum
{
    PWR_StandbyWakeUpDelay_2 = 1u, /*!< 2 LSI period selected. */
    PWR_StandbyWakeUpDelay_3 = 2u, /*!< 3 LSI period selected. */
    PWR_StandbyWakeUpDelay_4 = 3u, /*!< 4 LSI period selected. */
    PWR_StandbyWakeUpDelay_5 = 4u, /*!< 5 LSI period selected. */
    PWR_StandbyWakeUpDelay_6 = 5u, /*!< 6 LSI period selected. */
    PWR_StandbyWakeUpDelay_7 = 6u, /*!< 7 LSI period selected. */
    PWR_StandbyWakeUpDelay_8 = 7u, /*!< 8 LSI period selected. */
} PWR_StandbyWakeUpDelay_Type;

/*!
 * @brief PWR standby mode wake up pin trigger input type.
 */
typedef enum
{
    PWR_StandbyWakeUpPinTriggerIn_RisingEdge  = 0u, /*!< Enable the wake up pin rising edge trigger. */
    PWR_StandbyWakeUpPinTriggerIn_FallingEdge = 1u, /*!< Enable the wake up pin falling edge trigger. */
} PWR_StandbyWakeUpPinTriggerIn_Type;

/*!
 * @brief This type of structure instance is used to keep setting when calling.
 */
typedef struct
{
    uint32_t Pins; /*!< Specify wake up pins. See to @ref PWR_WAKEUP_PIN. */
    PWR_StandbyWakeUpPinTriggerIn_Type TrgIn; /*!< @ref Specify wake up pin trigger input type. See to @ref PWR_StandbyWakeUpPinTriggerIn_Type. */
} PWR_StandbyWakeUpPinConf_Type;

/*!
 * @brief This type of structure instance is used to keep setting when calling.
 */
typedef struct
{
    uint32_t Pin; /*!< Specify one of the PWR_WAKEUP_PIN_0, PWR_WAKEUP_PIN_1, and PWR_WAKEUP_PIN_2 for filtering. */
    PWR_StandbyWakeUpPinTriggerIn_Type TrgIn; /*!< @ref Specify wake up pin trigger input type. See to @ref PWR_StandbyWakeUpPinTriggerIn_Type. */
    uint32_t CountValue; /*!< Specify filter count value. */
} PWR_StandbyWakeUpPinFilter0Conf_Type;

/*!
 * @brief This type of structure instance is used to keep setting when calling.
 */
typedef struct
{
    uint32_t Pin; /*!< Specify one of PWR_WAKEUP_PIN_3, PWR_WAKEUP_PIN_4, and PWR_WAKEUP_PIN_5 for filtering. */
    PWR_StandbyWakeUpPinTriggerIn_Type TrgIn; /*!< @ref Specify wake up pin trigger input type. See to @ref PWR_StandbyWakeUpPinTriggerIn_Type. */
    uint32_t CountValue; /*!< Specify filter count value. */
} PWR_StandbyWakeUpPinFilter1Conf_Type;

/*!
 * @brief Enable low power run mode.
 *
 * @param PWRx PWR instance.
 * @param enable 'true' to enable the mode, 'false' to disable the mode.
 * @return None.
 */
void PWR_EnableLowPowerRunMode(PWR_Type * PWRx, bool enable);

/*!
 * @brief Enter stop mode.
 *
 * @param PWRx PWR instance.
 * @param deep_stop 'true' to enable the deep stop mode, 'false' to disable the deep stop mode.
 * @return None.
 */
void PWR_EnterStopMode(PWR_Type * PWRx, bool deep_stop);

/*!
 * @brief Enable standby mode wake up pin.
 *
 * @param PWRx PWR instance.
 * @param init Pointer to the initialization structure. See to @ref PWR_StandbyWakeUpPinConf_Type.
 * @return None.
 */
void PWR_EnableStandbyWakeUpPin(PWR_Type * PWRx, PWR_StandbyWakeUpPinConf_Type * conf);

/*!
 * @brief Enable standby mode wake up pin filter 0.
 *
 * @param PWRx PWR instance.
 * @param init Pointer to the initialization structure. See to @ref PWR_StandbyWakeUpPinFilter0Conf_Type.
 * @return None.
 */
void PWR_EnableStandbyWakeUpPinFilter0(PWR_Type * PWRx,  PWR_StandbyWakeUpPinFilter0Conf_Type * conf);

/*!
 * @brief Enable standby mode wake up pin filter 1.
 *
 * @param PWRx PWR instance.
 * @param init Pointer to the initialization structure. See to @ref PWR_StandbyWakeUpPinFilter1Conf_Type.
 * @return None.
 */
void PWR_EnableStandbyWakeUpPinFilter1(PWR_Type * PWRx,  PWR_StandbyWakeUpPinFilter1Conf_Type * conf);

/*!
 * @brief Enter standby mode.
 *
 * @param PWRx PWR instance.
 * @param Delay Specify standby mode wake up delay value. See to @ref PWR_StandbyWakeUpDelay_Type.
 * @return None.
 */
void PWR_EnterStandbyMode(PWR_Type * PWRx, PWR_StandbyWakeUpDelay_Type delay);

/*!
 * @brief Get the current status flags of PWR module.
 *
 * @param PWRx PWR instance.
 * @return PWR status flags. See to @ref PWR_STATUS.
 */
uint32_t PWR_GetStatus(PWR_Type * PWRx);

/*!
 * @brief Clear PWR status flags.
 *
 * @param PWRx PWR instance.
 * @param flags PWR status flags. See to @ref PWR_STATUS.
 * @return None.
 */
void PWR_ClearStatus(PWR_Type * PWRx, uint32_t flags);

/*!
 *@}
 */

#endif /* __HAL_PWR_H__ */
