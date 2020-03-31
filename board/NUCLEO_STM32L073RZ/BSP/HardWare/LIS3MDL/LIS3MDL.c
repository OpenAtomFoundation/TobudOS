/**
  ******************************************************************************
  * @file       LIS3MDL.c
  * @author     jieranzhi
  * @update     2020/03/23 19:00 CST
  * @brief      This file provides code for the LIS3MDL Initialization 
  *             and data output codes.
  ******************************************************************************
  * @attention
  *
  * 1. this code is used as one of the examples in TencentOS_tiny project, it's 
  *    just a simple implementation of the sensor functionalities, to implement 
  *    more functions, please refer to the datasheet or the official software 
  *    package provided by ST (STM32CubeExpansion_LRWAN_V1.3.1)
  *
  * 2. in this file the host MCU will to read the output persistently, which is 
  *    NOT of power efficient.
  *
  ******************************************************************************
  */

#include <LIS3MDL.h>
#include <i2c.h>

// initialization of LIS3MDL
void LIS3MDL_Init()
{
  uint8_t cmd = 0;
                 
  // enable temperature sensor(temperature compensation);X and Y axes operative 
  // mode(Medium-performance); Output data rate(10Hz); disable high data rate
  // disable Self-test
  cmd = 0xB0;
  HAL_I2C_Mem_Write(&hi2c1, LIS3MDL_ADDR_WR, LIS3MDL_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
                    
  // Full-scale:+/-4gauss; Reboot memory(normal)
  cmd = 0x00;
  HAL_I2C_Mem_Write(&hi2c1, LIS3MDL_ADDR_WR, LIS3MDL_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // disable lowpower mode; Operating mode (continuous mode)
  cmd = 0x00;
  HAL_I2C_Mem_Write(&hi2c1, LIS3MDL_ADDR_WR, LIS3MDL_CTRL_REG3, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // Z-axis operative mode (Medium-performance)
  cmd = 0x04;
  HAL_I2C_Mem_Write(&hi2c1, LIS3MDL_ADDR_WR, LIS3MDL_CTRL_REG4, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // disable FAST READ; data block update(continuous)
  cmd = 0x40;
  HAL_I2C_Mem_Write(&hi2c1, LIS3MDL_ADDR_WR, LIS3MDL_CTRL_REG5, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
}

// get fullscale configuration
LIS3MDL_FullScaleTypeDef LIS3MDL_Get_FullScale()
{
  uint8_t fullscale;
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &fullscale, 1, 0xFFFF);
  fullscale = (fullscale<<1)>>6;
  return (LIS3MDL_FullScaleTypeDef)fullscale;
}

// set fullscale of the sensor
void LIS3MDL_Set_FullScale(LIS3MDL_FullScaleTypeDef fullscale)
{
  uint8_t ctrl_reg2_value;
  uint8_t fullscale_config = (uint8_t)fullscale;
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &ctrl_reg2_value, 1, 0xFFFF);
  fullscale_config = (ctrl_reg2_value&0x9F)|(fullscale_config<<5);
  HAL_I2C_Mem_Write(&hi2c1, LIS3MDL_ADDR_WR, LIS3MDL_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &fullscale_config, 1, 0xFFFF);
}

// calculate the sensitivity per fullscale
uint16_t LIS3MDL_Get_Sensitivity(LIS3MDL_FullScaleTypeDef fullscale)
{
  uint16_t sensitivity = 1;
  switch(fullscale)
  {
    case FULLSCALE_4:{
      sensitivity = 6842;
      break;
    }
    case FULLSCALE_8:{
      sensitivity = 3421;
      break;
    }
    case FULLSCALE_12:{
      sensitivity = 2281;
      break;
    }
    case FULLSCALE_16:{
      sensitivity = 1711;
      break;
    }
    default:{
      sensitivity = 1;
    }
  }
  return sensitivity;
}

// start a new acquisition by enabling the one-shot bit in the LIS3MDL_CTRL_REG2
// and read the magnetic field from the sensor
uint8_t LIS3MDL_Get_Magn(sensor_magn_t* magn_sensor)
{
  uint8_t status_dat = 0;
  uint8_t magn_x_out_l = 0;
  uint8_t magn_x_out_h = 0;
  uint8_t magn_y_out_l = 0;
  uint8_t magn_y_out_h = 0;
  uint8_t magn_z_out_l = 0;
  uint8_t magn_z_out_h = 0;
  
  // get fullscale and sensitivity
  LIS3MDL_FullScaleTypeDef fullscale = LIS3MDL_Get_FullScale();
  magn_sensor->fullscale = (uint8_t)fullscale;
  magn_sensor->sensitivity = LIS3MDL_Get_Sensitivity(fullscale);
  
  // wait until the data is ready
  while((status_dat&LIS3MDL_ZYXDA) != LIS3MDL_ZYXDA)
  {
    HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status_dat, 1, 0xFFFF);
  }
  
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_X_OUT_L, I2C_MEMADD_SIZE_8BIT, &magn_x_out_l, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_X_OUT_H, I2C_MEMADD_SIZE_8BIT, &magn_x_out_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_Y_OUT_L, I2C_MEMADD_SIZE_8BIT, &magn_y_out_l, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_Y_OUT_H, I2C_MEMADD_SIZE_8BIT, &magn_y_out_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_Z_OUT_L, I2C_MEMADD_SIZE_8BIT, &magn_z_out_l, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LIS3MDL_ADDR_RD, LIS3MDL_Z_OUT_H, I2C_MEMADD_SIZE_8BIT, &magn_z_out_h, 1, 0xFFFF);
  
  magn_sensor->magn_x = (uint16_t)magn_x_out_h<<8|magn_x_out_l;
  magn_sensor->magn_y = (uint16_t)magn_y_out_h<<8|magn_y_out_l;
  magn_sensor->magn_z = (uint16_t)magn_z_out_h<<8|magn_z_out_l;
  
  return 0;
}
