/**
  ******************************************************************************
  * @file    lorawan_info.c
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

/* Includes ------------------------------------------------------------------*/
#include "lorawan_info.h"
#include "LoRaMac.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
LoRaMacInfo_t LoRaMacInfoTable;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief initialises the LoraMacInfo capabilities table
  * @param none
  * @retval  none
  */
void LoraMacInfo_Init(void)
{
	LoRaMacInfoTable.Region = 0;
#ifdef  REGION_AS923
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_AS923) ;
#endif
#ifdef  REGION_AU915
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_AU915) ;
#endif
#ifdef  REGION_CN470
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_CN470) ;
#endif
#ifdef  REGION_CN470ALI
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_CN470ALI);
#endif
#ifdef  REGION_CN779
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_CN779) ;
#endif
#ifdef  REGION_EU433
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_EU433) ;
#endif
#ifdef  REGION_EU868
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_EU868) ;
#endif
#ifdef  REGION_KR920
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_KR920) ;
#endif
#ifdef  REGION_IN865
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_IN865) ;
#endif
#ifdef  REGION_US915
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_US915) ;
#endif
#ifdef  REGION_RU864
	LoRaMacInfoTable.Region |= (1<<LORAMAC_REGION_RU864) ;
#endif

#ifdef  LORAMAC_CLASSB_ENABLED
	LoRaMacInfoTable.ClassB = 1;
#else
	LoRaMacInfoTable.ClassB = 0;
#endif

return;
}

/**
  * @brief returns the pointer to the LoraMacInfo capabilities table
  * @param none
  * @retval  LoRaMacInfoTable pointer
  */
LoRaMacInfo_t* LoraMacInfo_GetPtr(void)
{
  return &LoRaMacInfoTable; 
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

