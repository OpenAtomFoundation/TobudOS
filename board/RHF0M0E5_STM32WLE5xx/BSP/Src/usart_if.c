/**
  ******************************************************************************
  * @file  : usart_if.c
  * @author  MCD Application Team
  * @brief   interfaces UART MX driver for hyperterminal communication
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

/* Includes ------------------------------------------------------------------*/
#include "usart_if.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Uart Handle */
extern UART_HandleTypeDef huart1;
#define UartHandle huart1
uint8_t charRx;

/* Private function prototypes -----------------------------------------------*/
/**
* @brief  TX complete callback
* @param  None
* @return None
*/
static void (*TxCpltCallback)(void*);
/**
* @brief  RX complete callback
* @param  char sent by user
* @return None
*/
static void (*RxCpltCallback)(uint8_t *rxChar, uint16_t size, uint8_t error);

/* Functions Definition ------------------------------------------------------*/

uint8_t vcom_Init(void (*cb)(void *))
{
  TxCpltCallback = cb;
	
  MX_USART1_UART_Init();
  return 0;
}

uint8_t vcom_DeInit(void)
{
  /*##-1- Reset peripherals ##################################################*/
//  __HAL_RCC_USART2_FORCE_RESET();
//  __HAL_RCC_USART2_RELEASE_RESET();

  /*##-2- MspDeInit ##################################################*/
//  HAL_UART_MspDeInit(&UartHandle);

  /*##-3- Disable the NVIC for DMA ###########################################*/
  /* temorary while waiting CR 50840: MX implementation of  MX_DMA_DeInit() */
  /* For the time being user should change mannualy the channel according to the MX settings */
  /* USER CODE BEGIN 1 */
//  HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);
//  
  return 0;
  /* USER CODE END 1 */
}

uint8_t vcom_Trace(uint8_t *p_data, uint16_t size)
{
//  HAL_UART_Transmit_DMA(&UartHandle, p_data, size);
	HAL_UART_Transmit(&UartHandle, p_data, size, 1000);
  return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
