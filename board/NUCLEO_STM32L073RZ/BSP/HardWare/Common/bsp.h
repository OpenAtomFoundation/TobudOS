/**
  ******************************************************************************
  * @file    bsp.c
  * @author  jieranzhi
  * @brief   provide high level interfaces to manage the sensors on the 
  *          application, this is a modified version of the official api
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
#include <stdbool.h>
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

 // application configuration types
 typedef enum{
   DCT_IS_CONFIRM     = 0x00U,
   DCT_REPORT_PERIOD  = 0x01U,
   DCT_REPEAT_TIME    = 0x02U,
   DCT_MAGN_FULLSCALE = 0x03U,
   DCT_DEFAULT        = 0xFFU,
 }DeviceConfigType_TypeDef;

// application configuration
typedef struct
{
  uint32_t                  config_address;
  uint16_t                  report_period;
  uint8_t                   repeat_time;
  LIS3MDL_FullScaleTypeDef  magn_fullscale;
  bool                      is_confirmed;
}DeviceConfig_TypeDef;

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
void  BSP_Sensor_Init(DeviceConfig_TypeDef config);

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
