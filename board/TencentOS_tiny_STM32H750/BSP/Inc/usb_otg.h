/**
  ******************************************************************************
  * @file    usb_otg.h
  * @brief   This file contains all the function prototypes for
  *          the usb_otg.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#ifndef __USB_OTG_H__
#define __USB_OTG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
extern HCD_HandleTypeDef hhcd_USB_OTG_HS;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USB_OTG_FS_HCD_Init(void);
void MX_USB_OTG_HS_HCD_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USB_OTG_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
