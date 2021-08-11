 /*******************************************************************************
  * @file    radio_conf.h
  * @author  MCD Application Team
  * @brief   driver sx1276 board (sx1276mb1mas for EU and sx1276mb1las for US)
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_CONF_H__
#define __RADIO_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* Exported types ------------------------------------------------------------*/   
/* Defines   ------------------------*/
#define RADIO_RX_BUF_SIZE       255

/* Function mapping */
#define RADIO_DELAY_MS                          HAL_Delay
#define RADIO_MEMSET8( dest, value, size )      UTIL_MEM_cpy_8( dest, value, size) 
   
#define LET_SUBGHZ_MW_USING_DGB_LINE1_PIN
#define LET_SUBGHZ_MW_USING_DGB_LINE2_PIN

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
   
#ifdef __cplusplus
}
#endif

#endif /* __RADIO_CONF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
