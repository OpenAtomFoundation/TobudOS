/**
  ******************************************************************************
  * @file   adc_if.c
  * @author MCD Application Team
  * @brief  Read status related to the chip (battery level, VREF, chip temperature)
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

/* Includes ------------------------------------------------------------------*/
#include "adc_if.h"
#include "app_system.h"
#ifdef HAL_ADC_MODULE_ENABLED
#include "adc.h"
#endif

/* External variables ---------------------------------------------------------*/
#ifdef HAL_ADC_MODULE_ENABLED
extern ADC_HandleTypeDef    hadc1;
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*from dtatsheet*/
#define TEMPSENSOR_TYP_CAL1_V          (( int32_t)  760)        /* Internal temperature sensor, parameter V30 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define TEMPSENSOR_TYP_AVGSLOPE        (( int32_t) 2500)        /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */
#define TEMPSENSOR_CAL_VREF            ((uint32_t) 3000)        /* Vdda value with which temperature sensor has been calibrated in production (+-10 mV). */

/* Private macro -------------------------------------------------------------*/
#define __LL_ADC_CALC_VREFANALOG_VOLTAGE_TEMPORARY(__VREFINT_ADC_DATA__,\
                                         __ADC_RESOLUTION__)                   \
  (((uint32_t)(*VREFINT_CAL_ADDR) * VREFINT_CAL_VREF)                          \
    / __LL_ADC_CONVERT_DATA_RESOLUTION((__VREFINT_ADC_DATA__),                 \
                                       (__ADC_RESOLUTION__),                   \
                                       LL_ADC_RESOLUTION_12B)                  \
  )

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

static uint32_t  ADC_ReadChannels(uint32_t channel);

/* Functions Definition ------------------------------------------------------*/


/**
  * @brief This function initializes the ADC
  * @param none
  * @retval none
  */
void SYS_InitMeasurement(void)
{
  hadc1.Instance = ADC1;
}

void SYS_DeInitMeasurement(void)
{
}

int16_t SYS_GetTemperatureLevel(void)
{
  int16_t temperatureDegreeC = 6400; /*25degreeC in q7.8*/

#ifdef HAL_ADC_MODULE_ENABLED
  uint32_t measuredLevel = 0;
  uint16_t batteryLevelmV = SYS_GetBatteryLevel();

  measuredLevel = ADC_ReadChannels(ADC_CHANNEL_TEMPSENSOR);

  /* #warning "to be replaced by __LL_ADC_CALC_TEMPERATURE when calibration data will be set in prod"*/
  temperatureDegreeC = __LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(TEMPSENSOR_TYP_AVGSLOPE,
                                                            TEMPSENSOR_TYP_CAL1_V,
                                                            TEMPSENSOR_CAL1_TEMP,
                                                            batteryLevelmV,
                                                            measuredLevel,
                                                            LL_ADC_RESOLUTION_12B);
  /*to be replaced */
  APP_LOG("temp= %d\n\r", temperatureDegreeC);

  /* from int16 to q8.7*/
  temperatureDegreeC <<= 8;

#endif /* HAL_ADC_MODULE_ENABLED */

  return (int16_t) temperatureDegreeC;
}
/**
  * @brief This function return the battery level
  * @param none
  * @retval the battery level in mV
  */
uint16_t SYS_GetBatteryLevel(void)
{
  uint16_t batteryLevelmV = 3300;
#ifdef HAL_ADC_MODULE_ENABLED
  uint32_t measuredLevel = 0;

  measuredLevel = ADC_ReadChannels(ADC_CHANNEL_VREFINT);

  if (measuredLevel == 0)
  {
    batteryLevelmV = 0;
  }
  else
  {
    batteryLevelmV = (3300 * 1510) / measuredLevel;
    /* #warning "to be replaced by __LL_ADC_CALC_VREFANALOG_VOLTAGE when calibration data will be set in prod" */
  }
#endif /* HAL_ADC_MODULE_ENABLED */

  return batteryLevelmV;
}

/* Private function definition -----------------------------------------------*/
/**
  * @brief This function reads the ADC channel
  * @param Channel
  * @retval Value
  */
static uint32_t ADC_ReadChannels(uint32_t channel)
{
  uint32_t ADCxConvertedValues = 0;
#ifdef HAL_ADC_MODULE_ENABLED
  ADC_ChannelConfTypeDef sConfig = {0};

  MX_ADC1_Init();
  /** Configure Regular Channel */
  sConfig.Channel = channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }
  /** Wait for end of conversion */
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

  /* Workaround for tempsensor value*/
  if (channel == ADC_CHANNEL_TEMPSENSOR)
  {
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
      /* Start Error */
      Error_Handler();
    }
    /** Wait for end of conversion */
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  }

  /** Wait for end of conversion */
  HAL_ADC_Stop(&hadc1) ;   /* it calls also ADC_Disable() */

  ADCxConvertedValues = HAL_ADC_GetValue(&hadc1);

  HAL_ADC_DeInit(&hadc1);

#endif /* HAL_ADC_MODULE_ENABLED */
  return ADCxConvertedValues;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
