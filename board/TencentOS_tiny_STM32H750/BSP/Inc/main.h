/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define E53_GPIO1_Pin GPIO_PIN_5
#define E53_GPIO1_GPIO_Port GPIOB
#define W_DISABLE_Pin GPIO_PIN_5
#define W_DISABLE_GPIO_Port GPIOK
#define HDMI_RST_Pin GPIO_PIN_4
#define HDMI_RST_GPIO_Port GPIOK
#define HDM_INT_Pin GPIO_PIN_3
#define HDM_INT_GPIO_Port GPIOK
#define DCMI_PWDN_Pin GPIO_PIN_7
#define DCMI_PWDN_GPIO_Port GPIOD
#define DCMI_RST_Pin GPIO_PIN_10
#define DCMI_RST_GPIO_Port GPIOA
#define USER_LED_Pin GPIO_PIN_13
#define USER_LED_GPIO_Port GPIOC
#define DMIC_CLK_Pin GPIO_PIN_8
#define DMIC_CLK_GPIO_Port GPIOA
#define SD_CD_Pin GPIO_PIN_7
#define SD_CD_GPIO_Port GPIOG
#define LCD_BL_Pin GPIO_PIN_12
#define LCD_BL_GPIO_Port GPIOI
#define WLAN_GPIO_Pin GPIO_PIN_1
#define WLAN_GPIO_GPIO_Port GPIOC
#define E53_GPIO4_Pin GPIO_PIN_2
#define E53_GPIO4_GPIO_Port GPIOA
#define E53_GPIO2_Pin GPIO_PIN_12
#define E53_GPIO2_GPIO_Port GPIOD
#define E53_GPIO3_Pin GPIO_PIN_13
#define E53_GPIO3_GPIO_Port GPIOD
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOJ
#define E53_GPIO5_Pin GPIO_PIN_3
#define E53_GPIO5_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOJ
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
