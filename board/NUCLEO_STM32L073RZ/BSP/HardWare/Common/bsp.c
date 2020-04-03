/**
  ******************************************************************************
  * @file    bsp.c
  * @author  jieranzhi
  * @brief   provide high level interfaces to manage the sensors on the 
  *          application
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

void  BSP_Sensor_Init(DeviceConfig_TypeDef config)
{
  /* Initialize sensors */
  HTS221_Init();
  LPS22HB_Init();
  LIS3MDL_Init();
  LIS3MDL_Set_FullScale((LIS3MDL_FullScaleTypeDef)config.magn_fullscale);
}

void BSP_Sensor_Read(sensor_data_t *sensor_data)
{
  sensor_tempnhumi_t tempnhumi_sensor;
  sensor_press_t press_sensor;
  sensor_magn_t   magn_sensor;
  
  HTS221_Get_TemperatureAndHumidity(&tempnhumi_sensor);
  LPS22HB_Get_Press(&press_sensor);
  LIS3MDL_Get_Magn(&magn_sensor);
  
  sensor_data->sensor_press = press_sensor;
  sensor_data->sensor_tempnhumi = tempnhumi_sensor;
  sensor_data->sensor_magn = magn_sensor;
}

