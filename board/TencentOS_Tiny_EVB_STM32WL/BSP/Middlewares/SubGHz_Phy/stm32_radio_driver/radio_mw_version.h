/******************************************************************************
  * @file    radio_mw_version.h
  * @author  MCD Application Team
  * @brief   defines the radio driver version
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

#ifndef __RADIO_MW_VERSION_H__
#define __RADIO_MW_VERSION_H__

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* __SUBGH_RADIO_TYPE: 0x01  WTM32WL
                       0x61  sx126x   
                       0x72  sx1272 
                       0x76  sx1276 */ 

#define __SUBGH_RADIO_TYPE        (0x01U) /*!< [31:24] main version */
#define __SUBGH_MW_VERSION_MAIN   (0x01U) /*!< [23:16] main version */
#define __SUBGH_MW_VERSION_SUB1   (0x00U) /*!< [15:8]  sub1 version */
#define __SUBGH_MW_VERSION_SUB2    (0x00U) /*!< [7:0]  sub2 version */ 
#define __SUBGH_MW_VERSION        ((__SUBGH_RADIO_TYPE <<24)\
                                    |(__SUBGH_MW_VERSION_MAIN << 16)\
                                    |(__SUBGH_MW_VERSION_SUB1 << 8 )\
                                    |(__SUBGH_MW_VERSION_SUB2))   
   
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

#ifdef __cplusplus
}
#endif

#endif /*__RADIO_MW_VERSION_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
