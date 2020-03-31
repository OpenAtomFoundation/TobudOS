/**
  ******************************************************************************
  * @file       LIS3MDL.h
  * @author     jieranzhi (the developer)
  * @update     2020/03/23 19:00 CST
  * @brief      This file contains basic functions prototypes and pre-definitions 
  *             of the register addresses
  ******************************************************************************
  * @attention
  *
  * 1. the temperature sensor embedded in the LIS3MDL is intended to be embedded 
  *    temperature compensation. Therefore, in this file we DO NOT include the
  *    temperature output
  *
  * 2. on the P-NUCLEO-LRWAN3, the SDO/SA0 pad is connected to voltage supply(
  *    via a resistor), LSb is ¡®1¡¯ (address 1011101b)
  *
  * 3. for more information, please refer to the datasheet
  *    (https://www.st.com/resource/en/datasheet/lis3mdl.pdf)
  *
  ******************************************************************************
  */ 
#ifndef	_LIS3MDL_H_
#define	_LIS3MDL_H_

#include <stm32l0xx_hal.h>

/* Registers -----------------------------------------------------------------*/
#define LIS3MDL_ADDR_WR	0x3C
#define LIS3MDL_ADDR_RD	0x3D

#define LIS3MDL_CTRL_REG1 0x20
#define LIS3MDL_CTRL_REG2 0x21
#define LIS3MDL_CTRL_REG3 0x22
#define LIS3MDL_CTRL_REG4 0x23
#define LIS3MDL_CTRL_REG5 0x24

#define LIS3MDL_STATUS_REG	0x27

#define LIS3MDL_X_OUT_L         0x28
#define LIS3MDL_X_OUT_H         0x29
#define LIS3MDL_Y_OUT_L         0x2A
#define LIS3MDL_Y_OUT_H         0x2B
#define LIS3MDL_Z_OUT_L         0x2C
#define LIS3MDL_Z_OUT_H         0x2D

/* Enumeration ---------------------------------------------------------------*/
/** 
  * @brief  STATUS structures definition  
  */ 
typedef enum
{
  LIS3MDL_ZYXOR  = 0x80U,  /*!< XYZ-axis data overrun       */  
  LIS3MDL_ZOR    = 0x40U,  /*!< Z-axis data overrun         */
  LIS3MDL_YOR    = 0x20U,  /*!< Y-axis data overrun         */     
  LIS3MDL_XOR    = 0x10U,  /*!< X-axis data overrun         */  
  LIS3MDL_ZYXDA  = 0x08U,  /*!< XYZ-axis data available     */  
  LIS3MDL_ZDA    = 0x04U,  /*!< Z-axis data available       */
  LIS3MDL_YDA    = 0x02U,  /*!< Y-axis data available       */     
  LIS3MDL_XDA    = 0x01U,  /*!< X-axis data available       */ 
}LIS3MDL_StatusTypeDef;

/** 
  * @brief  STATUS structures definition  
  */ 
typedef enum
{
  FULLSCALE_4  = 0x00U,
  FULLSCALE_8  = 0x01U,
  FULLSCALE_12 = 0x02U,
  FULLSCALE_16 = 0x03U
}LIS3MDL_FullScaleTypeDef;

/** 
  * @brief  magnetic sensor structures definition  
  */ 
typedef struct
{
  uint8_t  fullscale;                    // fullscale of magnetometer
  uint16_t sensitivity;                  // sensitivity per fullscale
  uint16_t magn_x;                       // X-magnetic value in LSB
  uint16_t magn_y;                       // Y-magnetic value in LSB
  uint16_t magn_z;                       // Z-magnetic value in LSB
}sensor_magn_t;

/* Functions -----------------------------------------------------------------*/
void LIS3MDL_Init(void);
void LIS3MDL_Set_FullScale(LIS3MDL_FullScaleTypeDef fullscale);
LIS3MDL_FullScaleTypeDef LIS3MDL_Get_FullScale(void);
uint16_t LIS3MDL_Get_Sensitivity(LIS3MDL_FullScaleTypeDef fullscale);
uint8_t LIS3MDL_Get_Magn(sensor_magn_t* magn_sensor);

#endif /*  _LIS3MDL_H_ */
