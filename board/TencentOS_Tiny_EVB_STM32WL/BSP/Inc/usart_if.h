/**
  ******************************************************************************
  * @file   usart_if.h
  * @author MCD Application Team
  * @brief  Header file for the UART driver handling for hyperterminal communication
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

#include "usart.h"
#include "dma.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_IF_H__
#define __USART_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
* @brief  Init the UART and associated DMA.
* @param  TxCpltCallback
* @return none
*/
uint8_t vcom_Init(void (*cb)(void*));


/**
* @brief  DeInit the UART and associated DMA.
* @param  none
* @return none
*/
uint8_t vcom_DeInit(void);


/**
* @brief  send buffer @p_data of size size to vcom using DMA
* @param  p_data data to be sent
* @param  szie of buffer p_data to be sent
* @return none
*/
uint8_t vcom_Trace(uint8_t *p_data, uint16_t size);




#ifdef __cplusplus
}
#endif

#endif /* __USART_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
