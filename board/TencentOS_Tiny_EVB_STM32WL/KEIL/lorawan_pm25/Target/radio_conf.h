/*******************************************************************************
 * @file    radio_conf.h
 * @author  MCD Application Team
 * @brief   Header of radio configuration
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_CONF_H__
#define __RADIO_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "mw_log_conf.h"

#include "radio_driver.h"
#include "app_conf.h"
#include "stm32_mem.h"
	
#ifndef CRITICAL_SECTION_BEGIN
#define CRITICAL_SECTION_BEGIN( )      UTILS_ENTER_CRITICAL_SECTION( )  
#endif
#ifndef CRITICAL_SECTION_END  
#define CRITICAL_SECTION_END( )        UTILS_EXIT_CRITICAL_SECTION( )  
#endif
/* Exported types ------------------------------------------------------------*/
/* Defines   ------------------------*/
#define RADIO_RX_BUF_SIZE       255

//#define LET_SUBGHZ_MW_USING_DGB_LINE1_PIN
//#define LET_SUBGHZ_MW_USING_DGB_LINE2_PIN

/* Function mapping */
#define RADIO_DELAY_MS                          HAL_Delay
#define RADIO_MEMSET8( dest, value, size )      UTIL_MEM_set_8(dest, value, size )


/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* __RADIO_CONF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
