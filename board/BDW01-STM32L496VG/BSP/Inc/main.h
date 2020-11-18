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
#include "stm32l4xx_hal.h"

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
void _Error_Handler(char *file, int line);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOE
#define LED5_Pin GPIO_PIN_3
#define LED5_GPIO_Port GPIOE
#define SENSOR_EN_Pin GPIO_PIN_5
#define SENSOR_EN_GPIO_Port GPIOE
#define SENSOR_INT1_Pin GPIO_PIN_6
#define SENSOR_INT1_GPIO_Port GPIOE
#define SENSOR_INT2_Pin GPIO_PIN_13
#define SENSOR_INT2_GPIO_Port GPIOC
#define GPS_ON_Pin GPIO_PIN_1
#define GPS_ON_GPIO_Port GPIOB
#define BD_EN_Pin GPIO_PIN_2
#define BD_EN_GPIO_Port GPIOB
#define GPS_RST_Pin GPIO_PIN_7
#define GPS_RST_GPIO_Port GPIOE
#define NB_RST_Pin GPIO_PIN_8
#define NB_RST_GPIO_Port GPIOE
#define NB_WKUP_Pin GPIO_PIN_9
#define NB_WKUP_GPIO_Port GPIOE
#define NB_EN_Pin GPIO_PIN_11
#define NB_EN_GPIO_Port GPIOE
#define SEL1_Pin GPIO_PIN_12
#define SEL1_GPIO_Port GPIOE
#define SEL2_Pin GPIO_PIN_13
#define SEL2_GPIO_Port GPIOE
#define SEL3_Pin GPIO_PIN_14
#define SEL3_GPIO_Port GPIOE
#define KEY4_Pin GPIO_PIN_15
#define KEY4_GPIO_Port GPIOE
#define OLED_RST_Pin GPIO_PIN_12
#define OLED_RST_GPIO_Port GPIOB
#define OLED_EN_Pin GPIO_PIN_15
#define OLED_EN_GPIO_Port GPIOB
#define AU_CTR_Pin GPIO_PIN_8
#define AU_CTR_GPIO_Port GPIOD
#define AU_RST_Pin GPIO_PIN_13
#define AU_RST_GPIO_Port GPIOD
#define AU_EN_Pin GPIO_PIN_15
#define AU_EN_GPIO_Port GPIOD
#define CHARGE_STATE_Pin GPIO_PIN_6
#define CHARGE_STATE_GPIO_Port GPIOC
#define CARD_EN_Pin GPIO_PIN_7
#define CARD_EN_GPIO_Port GPIOC
#define PCIE_RST_Pin GPIO_PIN_8
#define PCIE_RST_GPIO_Port GPIOA
#define PCIE_EN_Pin GPIO_PIN_9
#define PCIE_EN_GPIO_Port GPIOA
#define CARD_END5_Pin GPIO_PIN_5
#define CARD_END5_GPIO_Port GPIOD
#define KEY3_Pin GPIO_PIN_6
#define KEY3_GPIO_Port GPIOD
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_8
#define LED3_GPIO_Port GPIOB
#define IR_OUT_Pin GPIO_PIN_9
#define IR_OUT_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_0
#define LED2_GPIO_Port GPIOE
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
