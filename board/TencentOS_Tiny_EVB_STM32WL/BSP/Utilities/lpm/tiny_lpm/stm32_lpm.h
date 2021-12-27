/**
 ******************************************************************************
 * @file    stm32_lpm.h
 * @author  MCD Application Team
 * @brief   Header for stm32_lpm.c module
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. 
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the 
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_LPM_H
#define __STM32_LPM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

typedef uint32_t  UTIL_LPM_bm_t;

typedef enum
{
  UTIL_LPM_ENABLE=0,
  UTIL_LPM_DISABLE,
} UTIL_LPM_State_t;

typedef enum
{
  UTIL_LPM_SLEEPMODE,
  UTIL_LPM_STOPMODE,
  UTIL_LPM_OFFMODE,
} UTIL_LPM_Mode_t;

/*!
 * \brief LPM driver definition
 */
struct UTIL_LPM_Driver_s
{
   void (* EnterSleepMode) ( void );
   void (* ExitSleepMode) ( void );
   void (* EnterStopMode) ( void );
   void (* ExitStopMode) ( void );
   void (* EnterOffMode) ( void );
   void (* ExitOffMode) ( void );
};

/*!
 *
 * \Remark : This structure is defined and initialized in the specific platform 
 *           power implementation
 */
extern const struct UTIL_LPM_Driver_s UTIL_PowerDriver;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief  This API Initializes the LPM resources.
 * @param  None
 * @retval None
 */
void UTIL_LPM_Init( void );

/**
 * @brief  This API Un-Initializes the LPM resources.
 * @param  None
 * @retval None
 */
void UTIL_LPM_DeInit( void );

/**
 * @brief  This API returns the Low Power Mode selected that will be applied when the system will enter low power mode
 *         if there is no update between the time the mode is read with this API and the time the system enters
 *         low power mode.
 * @param  None
 * @retval UTIL_LPM_Mode_t
 */
UTIL_LPM_Mode_t UTIL_LPM_GetMode( void );

/**
 * @brief  This API notifies the low power manager if the specified user allows the Stop mode or not.
 *         The default mode selection for all users is Stop Mode enabled
 * @param  lpm_id_bm: identifier of the user ( 1 bit per user )
 * @param  state: Specify whether StopMode is allowed or not by this user
 * @retval None
 */
void UTIL_LPM_SetStopMode( UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state );

/**
 * @brief  This API notifies the low power manager if the specified user allows the Off mode or not.
 *         The default mode selection for all users is Off mode enabled
 * @param  lpm_id_bm: identifier of the user ( 1 bit per user )
 * @param  state: Specify whether OffMode is allowed or not by this user
 * @retval None
 */
void UTIL_LPM_SetOffMode( UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state );

/**
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before entering Low Power Mode
 * @param  None
 * @retval None
 */
void UTIL_LPM_EnterLowPower( void );

#ifdef __cplusplus
}
#endif

#endif /*__STM32_LPM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
