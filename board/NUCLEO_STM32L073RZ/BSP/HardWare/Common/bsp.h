/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: contains all hardware driver

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/**
  ******************************************************************************
  * @file    bsp.h
  * @author  MCD Application Team
  * @brief   contains all hardware driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
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
#ifndef __BSP_H__
#define __BSP_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "HTS221.h"
#include "LPS22HB.h"
#include "LIS3MDL.h"
  
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  sensor_press_t       sensor_press;         /* pressure sensor          */
  sensor_tempnhumi_t   sensor_tempnhumi;     /* temperature and humidity */
  sensor_magn_t        sensor_magn;          /* magnetometer             */
  
  //--------------------------- accelerator and gyroscope  -------------------//
  int16_t  accel_x;                           /* in g           */
  int16_t  accel_y;                           /* in g           */
  int16_t  accel_z;                           /* in g           */
  int16_t  gyro_x;                            /* in degree/s    */
  int16_t  gyro_y;                            /* in degree/s    */
  int16_t  gyro_z;                            /* in degree/s    */
  
} sensor_data_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/**
 * @brief  initialises the sensor
 *
 * @note
 * @retval None
 */
void  BSP_Sensor_Init(void);

/**
 * @brief  sensor  read.
 *
 * @note none
 * @retval sensor_data
 */
void BSP_Sensor_Read(sensor_data_t *sensor_data);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H__ */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
