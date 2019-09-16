/**
  ******************************************************************************
  * File Name          : ETH.h
  * Description        : This file provides code for the configuration
  *                      of the ETH instances.
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
#ifndef __bsp_eth_H
#define __bsp_eth_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ethernetif.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ETH_HandleTypeDef heth;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

extern ethernetif_drv_t lan8742a_drv;

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ eth_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
