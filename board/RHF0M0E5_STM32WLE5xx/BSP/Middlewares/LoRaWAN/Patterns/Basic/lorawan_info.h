/**
  ******************************************************************************
  * @file    lorawan_info.h
  * @author  MCD Application Team
  * @brief   To give info to the application about LoraWAN configuration
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
#ifndef __LORAWAN_INFO_H__
#define __LORAWAN_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/*!
 * To give info to the application about LoraWAN capability 
 * it can depend how it has been compiled (e.g. compiled regions ...)
 * Params should be better uint32_t foe easier alignment with info_table concept
 */
typedef struct
{
  uint32_t Region;   /*!< Combination of regions compiled on MW  */ 
  uint32_t ClassB;  /*!< 0: not compiled in Mw, 1 : compiled in MW  */ 
}LoRaMacInfo_t;


/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief initialises the LoraMacInfo table
  * @param none
  * @retval  none
  */
void LoraMacInfo_Init(void);

/**
  * @brief returns the pointer to the LoraMacInfo table
  * @param none
  * @retval  LoRaMacInfoTable pointer
  */
LoRaMacInfo_t* LoraMacInfo_GetPtr(void);

#ifdef __cplusplus
}
#endif

#endif // __LORAWAN_INFO_H__

