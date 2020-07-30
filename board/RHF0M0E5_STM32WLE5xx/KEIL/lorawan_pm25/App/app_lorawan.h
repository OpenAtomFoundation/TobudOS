/**
  ******************************************************************************
 * @file    app_lorawan.h
 * @author  MCD Application Team
 * @brief  header of application of the LRWAN Middleware
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
#ifndef __APP_LORAWAN_H__
#define __APP_LORAWAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "lora.h"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
* @brief  Init Lora Application
* @param None
* @retval None
*/
void MX_LoRaWAN_Init(void);
/**
* @brief  Send Lora Data
* @param None
* @retval None
*/
void MX_LoRaWAN_Process(void);


#ifdef __cplusplus
}
#endif

#endif /*__APP_LORAWAN_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

