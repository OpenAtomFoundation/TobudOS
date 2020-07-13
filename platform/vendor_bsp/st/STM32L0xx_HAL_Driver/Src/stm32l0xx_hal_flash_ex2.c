/**
  ******************************************************************************
  * @file    stm32l0xx_hal_flash_eeprom.c
  * @author  jieranzhi
  * @date    2020/04/02 14:59 CST
  * @brief   implementation of Flash EEPROM operations.
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

#include "stm32l0xx_hal_flash_ex2.h"

/**
  * @brief  set the correct number of wait states, and set the clock of the memory 
            interface 
  * @note   The user must set the correct number of wait states (LATENCY bit in 
  *         the FLASH_ACR register). No control is done to verify if the frequency
  *         or the power used is correct, with respect to the number of wait 
  *         states. A wrong number of wait states can generate wrong read values 
  *         (high frequency and 0 wait states) or a long time to execute a code 
  *         (low frequency with 1 wait state).
  * @param  correct number of wait states
  */
void HAL_FLASH_Prepare_Reading(uint8_t WaitState)
{
    __HAL_RCC_MIF_CLK_ENABLE();
    __HAL_FLASH_SET_LATENCY(WaitState);
}

/**
  * @brief  Read word (4 bytes) from a specified address
  * @note   To correctly run this function, the HAL_FLASH_Prepare_Reading(...)
  *         function must be called before.
  *
  * @param  Address       Specifie the address to be read from.
  * @param  Data          Specifie the data to store the 
  * 
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_ReadWord(uint32_t Address, uint32_t* Data)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  
  /* Check the parameters */
  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address)||IS_FLASH_DATA_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  
  if(status == HAL_OK)
  {
    *Data = *(__IO uint32_t*)Address; 
  }
  return status;
}

/**
  * @brief  Read half-word (2 bytes) from a specified address
  * @note   To correctly run this function, the HAL_FLASH_Prepare_Reading(...)
  *         function must be called before.
  *
  * @param  Address       Specifie the address to be read from.
  * @param  Data          Specifie the data to store the 
  * 
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_FLASH_ReadHalfWord(uint32_t Address, uint16_t* Data)
{  
  HAL_StatusTypeDef status = HAL_ERROR;
  
  /* Check the parameters */
  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address)||IS_FLASH_DATA_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  
  if(status == HAL_OK)
  {
    *Data = *(__IO uint16_t*)Address; 
  }
  return status;
}
