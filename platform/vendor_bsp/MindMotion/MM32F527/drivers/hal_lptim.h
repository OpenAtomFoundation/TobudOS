/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_LPTIM_H__
#define __HAL_LPTIM_H__

#include "hal_common.h"

/*!
 * @addtogroup LPTIM
 * @{
 */

/*!
 * @brief LPTIM driver version number.
 */
#define LPTIM_DRIVER_VERSION 0u /*!< LPTIM_0. */

/*!
 * @addtogroup LPTIM_STATUS
 * @{
 */
#define LPTIM_STATUS_COUNTER_OVERFLOW (1u << 0u) /*!< Status flag when LPTIM counter overflow. */
#define LPTIM_STATUS_TRIGGER_INPUT    (1u << 2u) /*!< Status flag when LPTIM trigger input. */
#define LPTIM_STATUS_COMPARE_MATCH    (1u << 1u) /*!< Status flag when LPTIM compare match. */
/*!
 * @}
 */

/*!
 * @addtogroup LPTIM_INT
 * @{
 */
#define LPTIM_INT_COUNTER_OVERFLOW    (1u << 0u) /*!< Interrupt enable when LPTIM counter overflow. */
#define LPTIM_INT_TRIGGER_INPUT       (1u << 1u) /*!< Interrupt enable when LPTIM trigger input. */
#define LPTIM_INT_COMPARE_MATCH       (1u << 2u) /*!< Interrupt enable when LPTIM compare match. */
/*!
 * @}
 */

/*!
 * @brief LPTIM counter period working mode.
 */
typedef enum
{
    LPTIM_PeriodMode_Continuous = 0u, /*!< Counter works circularly once started. */
    LPTIM_PeriodMode_OneTimeRun = 1u, /*!< Counter only works one time once started. */
} LPTIM_PeriodMode_Type;

/*!
 * @brief LPTIM work mode type.
 */
typedef enum
{
    LPTIM_WorkMode_Timer = 0u, /*!< Normal timer mode with output waveform feature. */
    LPTIM_WorkMode_Trigger = 1u, /*!< Pulse trigger counting mode. */
    LPTIM_WorkMode_WakeUpLowPower =3, /*!< Counter time out wake up MCU from low power mode. */
} LPTIM_WorkMode_Type;

/*!
 * @brief LPTIM trigger input channel type.
 */
typedef enum
{
    LPTIM_TriggerInChannel_Pin  = 0u, /*!< Select trigger pin as trigger input channel. */
    LPTIM_TriggerInChannel_Comp = 1u, /*!< Select Comparator output as trigger input channel. */
} LPTIM_TriggerInChannel_Type;

/*!
 * @brief LPTIM trigger input polarity type.
 */
typedef enum
{
    LPTIM_TriggerInPolarity_Rising  = 0u, /*!< Select rising edge as external trigger signal. */
    LPTIM_TriggerInPolarity_Falling = 1u, /*!< Select falling edge as external trigger signal. */
    LPTIM_TriggerInPolarity_RisingOrFalling = 2u, /*!< Select both the rising and falling edge as external trigger signal. */
}LPTIM_TriggerInPolarity_Type;

/*!
 * @brief LPTIM output waveform type.
 */
typedef enum
{
    LPTIM_OutWave_Square  = 0u, /*!< Select Square wave to output. */
    LPTIM_OutWave_PWM = 1u, /*!< Select PWM wave to output. */
} LPTIM_OutWave_Type;

/*!
 * @brief LPTIM output polarity type.
 */
typedef enum
{
    LPTIM_OutPolarity_Rising  = 0u, /*!< Output rising edge when the count value is equal to the compare value. */
    LPTIM_OutPolarity_Falling = 1u, /*!< Output falling edge when the count value is equal to the compare value. */
} LPTIM_OutPolarity_Type;

/*!
 * @brief LPTIM prescaler type.
 *
 * Select the prescaler of time base as the LPTIM counter clock.
 */
typedef enum
{
    LPTIM_Prescaler_1   = 0u, /*!< LPTIM prescaler 1. */
    LPTIM_Prescaler_2   = 1u, /*!< LPTIM prescaler 2. */
    LPTIM_Prescaler_4   = 2u, /*!< LPTIM prescaler 4. */
    LPTIM_Prescaler_8   = 3u, /*!< LPTIM prescaler 8. */
    LPTIM_Prescaler_16  = 4u, /*!< LPTIM prescaler 16. */
    LPTIM_Prescaler_32  = 5u, /*!< LPTIM prescaler 32. */
    LPTIM_Prescaler_64  = 6u, /*!< LPTIM prescaler 64. */
    LPTIM_Prescaler_128 = 7u, /*!< LPTIM prescaler 128. */
} LPTIM_Prescaler_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref LPTIM_Init() to initialize the LPTIM module.
 */
typedef struct
{
    uint32_t Period; /*!< Counter counting length, from 0 to period value, the value range: 0 ~ 65535.*/
    LPTIM_PeriodMode_Type PeriodMode; /*!< Specify the counter period mode. */
    LPTIM_WorkMode_Type WorkMode; /*!< Specify the LPTIM worke mode. */
    LPTIM_Prescaler_Type Prescaler; /*!< Specify the LPTIM counter time base division value. */
} LPTIM_Init_Type;

/*!
 * @brief Setup the external trigger input configuration when calling the @ref LPTIM_EnableTriggerIn().
 */
typedef struct
{
    LPTIM_TriggerInChannel_Type Channel; /*!< Specify the trigger input channel. */
    bool EnableFilter;  /*!< 'true' to enable the external tirgger pin filter, 'false' to disable the filer. */
    LPTIM_TriggerInPolarity_Type TriggerInPolarity; /*!< Specify the trigger input polarity. */
} LPTIM_TriggerInConf_Type;

/*!
 * @brief Setup the output waveform configuration when calling the @ref LPTIM_OutWaveConf_Type().
 */
typedef struct
{
    LPTIM_OutWave_Type Wave; /*!< Specify output waveform. */
    LPTIM_OutPolarity_Type OutPolarity; /*!< Specify the waveform output pin polarity. */
} LPTIM_OutWaveConf_Type;

/*!
 * @brief Initialize the LPTIM module.
 *
 * @param LPTIMx LPTIM instance.
 * @param init Pointer to the initialization structure. See to @ref LPTIM_Init_Type.
 * @return None.
 */
void LPTIM_Init(LPTIM_Type * LPTIMx, LPTIM_Init_Type * init);

/*!
 * @brief Start counting.
 *
 * @param LPTIMx LPTIM instance.
 * @return None.
 */
void LPTIM_Start(LPTIM_Type * LPTIMx);

/*!
 * @brief Stop counting immediately.
 *
 * @param LPTIMx LPTIM instance.
 * @return None.
 */
void LPTIM_Stop(LPTIM_Type * LPTIMx);

/*!
 * @brief Enable interrupts of the LPTIM module.
 *
 * @param LPTIMx LPTIM_Type instance.
 * @param interrupts Interrupt code masks. See to @ref LPTIM_INT.
 * @param enable 'true' to enable the indicated interrupts, 'false' to disable the indicated interrupts.
 * @return None.
 */
void LPTIM_EnableInterrupts(LPTIM_Type * LPTIMx, uint32_t interrupts, bool enable);

/*!
 * @brief Get the interrupts status flags of the LPTIM module.
 *
 * @param LPTIMx LPTIM_Type instance.
 * @return Interrupt status flags. See to @ref LPTIM_STATUS.
 */
uint32_t LPTIM_GetInterruptStatus(LPTIM_Type * LPTIMx);

/*!
 * @brief Clear the interrupts status flags of the LPTIM module.
 *
 * @param LPTIMx LPTIM_Type instance.
 * @param interrupts The mask codes of the indicated interrupt flags to be cleared.
 * @return None.
 */
void LPTIM_ClearInterruptStatus(LPTIM_Type * LPTIMx, uint32_t interrupts);

/*!
 * @brief Enable the external trigger input of the indicated counter.
 *
 * External trigger signal will trigger counter do addition.
 *
 * @param LPTIMx LPTIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref LPTIM_TriggerInConf_Type.
 * @return None.
 */
void LPTIM_EnableTriggerIn(LPTIM_Type * LPTIMx, LPTIM_TriggerInConf_Type * conf);

/*!
 * @brief Enable output waveform.
 *
 * The output waveform could be PWM wave or suqare wave.
 *
 * @param LPTIMx LPTIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref LPTIM_OutWaveConf_Type.
 * @return None.
 */
void LPTIM_EnableOutWave(LPTIM_Type * LPTIMx, LPTIM_OutWaveConf_Type * conf);

/*!
 * @brief Set the compare value.
 *
 * The compare value would be compared with counter value.
 *
 * @param LPTIMx LPTIM_Type instance.
 * @param value The compare value to be set, the value range: 0 ~ 65535.
 * @return none.
 */
void LPTIM_SetCompareValue(LPTIM_Type * LPTIMx, uint32_t value);

/*!
 * @brief Get the current counter value.
 *
 * @param LPTIMx LPTIM_Type instance.
 * @return The current counter value.
 */
uint32_t LPTIM_GetCounterValue(LPTIM_Type * LPTIMx);

/*!
 *@}
 */
#endif /* __HAL_TIM_H__ */
