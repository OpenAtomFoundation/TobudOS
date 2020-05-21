/**
  ******************************************************************************
  * @file    stm32l0xx_hal_flash_ex2.h
  * @author  jieranzhi
  * @date    2020/04/02 14:59 CST
  * @brief   Header file of Flash EEPROM.
  ******************************************************************************
  * @attention
  *
  * this file implementes FLASH read operations for stm32l0xx series(category 5 
  * devices), to implement a morefunctions, please refer to RM0367, which could 
  * be downloaded from:
  * https://www.st.com/resource/en/reference_manual/dm00095744-ultra-low-power-
  * stm32l0x3-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf
  ******************************************************************************  
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L0xx_HAL_FLASH_EX2_H
#define __STM32L0xx_HAL_FLASH_EX2_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
   
/* Include FLASH HAL Extended module */
#include "stm32l0xx_hal_flash_ex.h"  
#include "stm32l0xx_hal_flash_ramfunc.h"
   
/* Exported functions --------------------------------------------------------*/
void HAL_FLASH_Prepare_Reading(uint8_t WaitState);
HAL_StatusTypeDef HAL_FLASH_ReadWord(uint32_t Address, uint32_t* Data);
HAL_StatusTypeDef HAL_FLASH_ReadHalfWord(uint32_t Address, uint16_t* Data);


#ifdef __cplusplus
}
#endif

#endif /* __STM32L0xx_HAL_FLASH_EX2_H */
