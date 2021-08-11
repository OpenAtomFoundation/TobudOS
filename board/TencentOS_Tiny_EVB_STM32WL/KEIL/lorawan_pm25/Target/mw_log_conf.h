/**
 ******************************************************************************
 * @file    mw_log_conf.h
 * @author  MCD Application Team
 * @brief   Interface layer CM4 System to MBMUX (Mailbox Multiplexer)
 *******************************************************************************
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
#ifndef __MW_LOG_CONF_H__
#define __MW_LOG_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "app_conf.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

#if LOG_LEVEL == 0
	#define LOG_PRINT      printf 
#elif LOG_LEVEL == 1
	#define LOG_PRINT(...)    do{printf("<<line:%d ;function:%s >>\r\n"\
								,__LINE__, __FUNCTION__);\
								printf(__VA_ARGS__);}while(0);
#elif LOG_LEVEL == 2
	#define LOG_PRINT(...)    do{printf("<<<%s ->line:%d ;function:%s >>>\r\n"\
								,__FILE__,__LINE__, __FUNCTION__);\
								printf(__VA_ARGS__);}while(0);
#endif

#if defined (MW_LOG_ENABLED) && (MW_LOG_ENABLED == 1)
	#define PPRINTF     printf 
	#define TPRINTF     printf 
	#define PRINTF      printf 		
	#define MW_LOG 	 LOG_PRINT 																						
#else
	#define PPRINTF(...)
	#define TPRINTF(...)
	#define PRINTF(...)
	#define MW_LOG(...)
#endif

/* Defines -------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */



#ifdef __cplusplus
}
#endif

#endif /*__MW_LOG_CONF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
