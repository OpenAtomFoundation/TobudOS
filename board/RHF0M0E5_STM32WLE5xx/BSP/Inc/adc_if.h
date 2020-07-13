/**
  ******************************************************************************
  * @file   adc_if.h
  * @author MCD Application Team
  * @brief  API and configuration of the adc_if.c instances
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
#ifndef __ADC_IF_H__
#define __ADC_IF_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "platform.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define VDDA_VREFINT_CAL            ((uint32_t) 3000)
#define BAT_CR2032                  ((uint32_t) 3000)
#define VDD_BAT                     BAT_CR2032
#define VDD_MIN                     1800

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/*!
 * @brief  Initializes the ADC input
 * @param  none
 * @retval none
 */
void SYS_InitMeasurement(void);

/*!
 * @brief DeInitializes the ADC
 * @param  none
 * @retval none
 */
void SYS_DeInitMeasurement(void);

/*!
 * @brief  Get the current temperature
 * @param  none
 * @retval value temperature in degreeCelcius( q7.8 )
 */
int16_t SYS_GetTemperatureLevel(void);


/*!
 * @brief Get the current battery level
 * @param  none
 * @retval value battery level in linear scale
 */
uint16_t SYS_GetBatteryLevel(void);


#ifdef __cplusplus
}
#endif

#endif /* __ADC_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

