/**
  ******************************************************************************
  * @file       LSM6DS3.h
  * @author     jieranzhi (the developer)
  * @update     2020/03/23 19:00 CST
  * @brief      This file contains basic functions prototypes and pre-definitions 
  *             of the register addresses
  ******************************************************************************
  * @attention
  *
  * 1. the temperature sensor embedded in the LSM6DS3 is intended to be embedded 
  *    temperature compensation. Therefore, in this file we DO NOT include the
  *    temperature output
  *
  * 2. on the P-NUCLEO-LRWAN3, the SDO/SA0 pad is connected to voltage supply(
  *    via a resistor), LSb is ¡®1¡¯ (address 1011101b), and the sensor uses 
  *    connection mode 1
  *
  * 3. for more information, please refer to the datasheet
  *    (https://www.st.com/resource/en/datasheet/lsm6ds3.pdf)
  *
  ******************************************************************************
  */ 
#ifndef	_LSM6DS3_H_
#define	_LSM6DS3_H_

#include <stm32l0xx_hal.h>

/* Registers -----------------------------------------------------------------*/
#define LSM6DS3_ADDR_WR	        0xD6
#define LSM6DS3_ADDR_RD	        0xD7

#define LSM6DS3_CTRL1_XL        0x10
#define LSM6DS3_CTRL2_G         0x11
#define LSM6DS3_CTRL3_C         0x12
#define LSM6DS3_CTRL4_C         0x13
#define LSM6DS3_CTRL5_C         0x14
#define LSM6DS3_CTRL6_C         0x15
#define LSM6DS3_CTRL7_G         0x16
#define LSM6DS3_CTRL8_XL        0x17
#define LSM6DS3_CTRL9_XL        0x18
#define LSM6DS3_CTRL10_C        0x19

#define LSM6DS3_STATUS_REG	0x1E

#define LSM6DS3_OUTX_L_G        0x22
#define LSM6DS3_OUTX_H_G        0x23
#define LSM6DS3_OUTY_L_G        0x24
#define LSM6DS3_OUTY_H_G        0x25
#define LSM6DS3_OUTZ_L_G        0x26
#define LSM6DS3_OUTZ_H_G        0x27

#define LSM6DS3_OUTX_L_XL       0x28
#define LSM6DS3_OUTX_H_XL       0x29
#define LSM6DS3_OUTY_L_XL       0x30
#define LSM6DS3_OUTY_H_XL       0x31
#define LSM6DS3_OUTZ_L_XL       0x32
#define LSM6DS3_OUTZ_H_XL       0x33

#define LSM6DS3_STEP_COUNTER_L  0x4B
#define LSM6DS3_STEP_COUNTER_H  0x4C

#define LSM6DS3_TAP_CFG         0x58



/* Enumeration ---------------------------------------------------------------*/
/** 
  * @brief  STATUS structures definition  
  */ 
typedef enum
{
  LSM6DS3_T_DA  = 0x04,  /*!< temperature data available     */  
  LSM6DS3_G_DA  = 0x02,  /*!< gyro data available            */
  LSM6DS3_XL_DA = 0x01,  /*!< acceleration data available    */
}LSM6DS3_StatusTypeDef;

typedef enum
{
  ACCEL_FULLSCALE_2  = 0x00U,
  ACCEL_FULLSCALE_16 = 0x04U,
  ACCEL_FULLSCALE_4  = 0x08U,
  ACCEL_FULLSCALE_8  = 0x0CU,
}LSM6DS3_AccelFullscaleTypeDef;

typedef enum
{
  GYRO_FULLSCALE_125  = 0x02U,
  GYRO_FULLSCALE_250  = 0x00U,
  GYRO_FULLSCALE_500  = 0x04U,
  GYRO_FULLSCALE_1000 = 0x08U,
  GYRO_FULLSCALE_2000 = 0x0CU,
}LSM6DS3_GyroFullscaleTypeDef;

/** 
  * @brief  motion sensor structures definition  
  */ 
typedef struct
{
  uint8_t  accelFullscale;
  uint8_t  gyroFullscale;
  uint16_t accelSensitivity;  /*!< the sensitivity is 1000x its actual value */  
  uint16_t gyroSensitivity;   /*!< the sensitivity is 1000x its actual value */  
  uint16_t gyroX;
  uint16_t gyroY;
  uint16_t gyroZ;
  uint16_t accelX;
  uint16_t accelY;
  uint16_t accelZ;
  uint16_t stepCount;
}sensor_motion_t;

/* Functions -----------------------------------------------------------------*/
void LSM6DS3_Init(void);
void LSM6DS3_Set_Accel_FullScale(LSM6DS3_AccelFullscaleTypeDef fullscale);
void LSM6DS3_Set_Gyro_FullScale(LSM6DS3_GyroFullscaleTypeDef fullscale);
void LSM6DS3_Set_Accel_FullScale_Num(uint8_t fullscale_num);
void LSM6DS3_Set_Gyro_FullScale_Num(uint8_t fullscale_num);
uint8_t LSM6DS3_Get_Accel_FullScale_Num(LSM6DS3_AccelFullscaleTypeDef fullscale);
uint16_t LSM6DS3_Get_Gyro_FullScale_Num(LSM6DS3_GyroFullscaleTypeDef fullscale);
LSM6DS3_AccelFullscaleTypeDef LSM6DS3_Get_Accel_FullScale(void);
LSM6DS3_GyroFullscaleTypeDef LSM6DS3_Get_Gyro_FullScale(void);
uint16_t LSM6DS3_Get_Accel_Sensitivity(LSM6DS3_AccelFullscaleTypeDef fullscale);
uint32_t LSM6DS3_Get_Gyro_Sensitivity(LSM6DS3_GyroFullscaleTypeDef fullscale);
uint8_t LSM6DS3_Get_Sensor_Config(sensor_motion_t* sensor_motion);
uint8_t LSM6DS3_Get_Accel(sensor_motion_t* sensor_motion);
uint8_t LSM6DS3_Get_Gyro(sensor_motion_t* sensor_motion);
uint8_t LSM6DS3_Get_Step(sensor_motion_t* sensor_motion);

#endif /*  _LSM6DS3_H_ */
