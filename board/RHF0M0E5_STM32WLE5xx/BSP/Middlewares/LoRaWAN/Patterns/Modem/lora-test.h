/******************************************************************************
  * @file    lora_test.h
  * @author  MCD Application Team
  * @brief   lora API to drive the lora state Machine
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

#ifndef __LORA_TEST_H__
#define __LORA_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define CERTIF_PORT 224
/* Exported types ------------------------------------------------------------*/


/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief Certification Initialisation
 * @param [IN] Notification call back to the appl that a CERTIF_Process must be called
 * @retval none
 */
void CERTIF_Init(void (*CERTIF_ProcessNotify)(void));
/**
 * @brief CERTIF_Process to be called when a CERTIF_ProcessNotify callback is generated
 * @param [IN] none
 * @retval none
 */
void CERTIF_Process(void);

/* Lora Test callbacks*/
typedef struct sLoRaCertifCallback
{
  bool (*running)(void);

  void (*DownLinkIncrement)(void);

  void (*linkCheck)(MlmeConfirm_t *mlmeConfirm);

  void (*rx)(McpsIndication_t *mcpsIndication, MlmeReqJoin_t *JoinParameters);

} LoRaCertifCallback_t;

#ifdef __cplusplus
}
#endif

#endif /*__LORA_TEST_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
