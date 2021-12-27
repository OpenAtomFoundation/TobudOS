/**
  ******************************************************************************
  * @file    lorawan_conf.h
  * @author  MCD Application Team
  * @brief   configures lorawan instances
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
#ifndef __LORAWAN_CONF_H__
#define __LORAWAN_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32_systime.h"
#include "app_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Defines ------------------------------------------------------------------*/
#ifndef CRITICAL_SECTION_BEGIN
#define CRITICAL_SECTION_BEGIN( )      UTILS_ENTER_CRITICAL_SECTION( )  
#endif
#ifndef CRITICAL_SECTION_END  
#define CRITICAL_SECTION_END( )        UTILS_EXIT_CRITICAL_SECTION( )  
#endif

/* Region ------------------------------------*/
/* the region listed here will be linked in the  MW code */
/* the applic (on app_conf.h) shall just configure one region at the time */
//#define REGION_AS923
//#define REGION_AU915
#define REGION_CN470
#define REGION_CN470ALI
//#define REGION_CN779
//#define REGION_EU433
#define REGION_EU868
//#define REGION_KR920
//#define REGION_IN865
#define REGION_US915
//#define REGION_RU864




#ifdef LORAMAC_CLASSB_ENABLED
/* CLASS B LSE crystall calibration*/
  /*!
 * \brief Temperature coefficient of the clock source
 */
#define RTC_TEMP_COEFFICIENT                            ( -0.035 )

/*!
 * \brief Temperature coefficient deviation of the clock source
 */
#define RTC_TEMP_DEV_COEFFICIENT                        ( 0.0035 )

/*!
 * \brief Turnover temperature of the clock source
 */
#define RTC_TEMP_TURNOVER                               ( 25.0 )

/*!
 * \brief Turnover temperature deviation of the clock source
 */
#define RTC_TEMP_DEV_TURNOVER                           ( 5.0 )
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LORAWAN_CONF_H__ */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

