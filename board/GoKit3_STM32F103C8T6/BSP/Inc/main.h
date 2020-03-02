/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define RGB_POWER_Pin GPIO_PIN_0
#define RGB_POWER_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_1
#define KEY3_GPIO_Port GPIOA
#define KEY3_EXTI_IRQn EXTI1_IRQn
#define Arduino_A2_Pin GPIO_PIN_4
#define Arduino_A2_GPIO_Port GPIOA
#define OLED_D0_Pin GPIO_PIN_5
#define OLED_D0_GPIO_Port GPIOA
#define OLED_D1_Pin GPIO_PIN_7
#define OLED_D1_GPIO_Port GPIOA
#define Arduino_A3_Pin GPIO_PIN_0
#define Arduino_A3_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_1
#define LED4_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_10
#define KEY1_GPIO_Port GPIOB
#define KEY1_EXTI_IRQn EXTI15_10_IRQn
#define OLED_DC_Pin GPIO_PIN_11
#define OLED_DC_GPIO_Port GPIOB
#define B_CS_Pin GPIO_PIN_12
#define B_CS_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_8
#define KEY2_GPIO_Port GPIOA
#define KEY2_EXTI_IRQn EXTI9_5_IRQn
#define OLED_CS_Pin GPIO_PIN_15
#define OLED_CS_GPIO_Port GPIOA
#define DHT11_DATA_Pin GPIO_PIN_3
#define DHT11_DATA_GPIO_Port GPIOB
#define Motor_IA_Pin GPIO_PIN_4
#define Motor_IA_GPIO_Port GPIOB
#define Motor_IB_Pin GPIO_PIN_5
#define Motor_IB_GPIO_Port GPIOB
#define OLED_RST_Pin GPIO_PIN_6
#define OLED_RST_GPIO_Port GPIOB
#define IR_OUT_Pin GPIO_PIN_7
#define IR_OUT_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_9
#define SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
