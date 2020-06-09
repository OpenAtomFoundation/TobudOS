/**
  ******************************************************************************
  * @file    stm32_mem.c
  * @author  MCD Application Team
  * @brief   standard memory operation
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
 
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stm32_mem.h"
   
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

void UTIL_MEM_cpy_8( void *dst, const void *src, uint16_t size )
{
  uint8_t* dst8= (uint8_t *) dst;
  uint8_t* src8= (uint8_t *) src;

  while( size-- )
    {
        *dst8++ = *src8++;
    }
}

void UTIL_MEM_cpyr_8( void *dst, const void *src, uint16_t size )
{
    uint8_t* dst8= (uint8_t *) dst;
    uint8_t* src8= (uint8_t *) src;
    
    dst8 = dst8 + ( size - 1 );
    while( size-- )
    {
        *dst8-- = *src8++;
    }
}

void UTIL_MEM_set_8( void *dst, uint8_t value, uint16_t size )
{
  uint8_t* dst8= (uint8_t *) dst;
  while( size-- )
  {
    *dst8++ = value;
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
