/**
  ******************************************************************************
  * @file       LSM6DS3.c
  * @author     jieranzhi
  * @update     2020/03/23 19:00 CST
  * @brief      This file provides code for the LSM6DS3 Initialization 
  *             and data output codes.
  ******************************************************************************
  * @attention
  *
  * 1. this code is used as one of the examples in TencentOS_tiny project, it's 
  *    just a simple implementation of the sensor functionalities, to implement 
  *    more functions, please refer to the datasheet or the official software 
  *    package provided by ST (STM32CubeExpansion_LRWAN_V1.3.1)
  *
  * 2. in this file the host MCU need to read the output persistently, which is 
  *    not of power efficient, to achieve better power consumption performance,  
  *    it is recommended to use FIFO.
  *
  ******************************************************************************
  */

#include <LSM6DS3.h>
#include <i2c.h>

// initialization of LSM6DS3
void LSM6DS3_Init()
{
  uint8_t cmd = 0;

  // ODR: 12.5Hz, fs: 4g, BWZ: 50Hz
  cmd = 0x1B;
  HAL_I2C_Mem_Write(&hi2c1, LSM6DS3_ADDR_WR, LSM6DS3_CTRL1_XL, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // ODR: 12.5Hz, fs: 250dps
  cmd = 0x10;
  HAL_I2C_Mem_Write(&hi2c1, LSM6DS3_ADDR_WR, LSM6DS3_CTRL2_G, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // High performance: disabled to save power 
  cmd = 0x10;
  HAL_I2C_Mem_Write(&hi2c1, LSM6DS3_ADDR_WR, LSM6DS3_CTRL6_C, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // High performance: disabled to save power 
  cmd = 0x80;
  HAL_I2C_Mem_Write(&hi2c1, LSM6DS3_ADDR_WR, LSM6DS3_CTRL7_G, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // timestamp output: enable, pedometer algorithm: enabled
  cmd = 0xC0;
  HAL_I2C_Mem_Write(&hi2c1, LSM6DS3_ADDR_WR, LSM6DS3_TAP_CFG, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
}

void LSM6DS3_Set_Accel_FullScale(LSM6DS3_AccelFullscaleTypeDef fullscale)
{
  uint8_t ctrl_reg1_value;
  uint8_t fullscale_config = (uint8_t)fullscale;
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_CTRL1_XL, I2C_MEMADD_SIZE_8BIT, &ctrl_reg1_value, 1, 0xFFFF);
  fullscale_config = (ctrl_reg1_value&0xF1)|(fullscale_config&0x0E);
  HAL_I2C_Mem_Write(&hi2c1, LSM6DS3_ADDR_WR, LSM6DS3_CTRL1_XL, I2C_MEMADD_SIZE_8BIT, &fullscale_config, 1, 0xFFFF);
}

void LSM6DS3_Set_Gyro_FullScale(LSM6DS3_GyroFullscaleTypeDef fullscale)
{
  uint8_t ctrl_reg2_value;
  uint8_t fullscale_config = (uint8_t)fullscale;
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_CTRL2_G, I2C_MEMADD_SIZE_8BIT, &ctrl_reg2_value, 1, 0xFFFF);
  fullscale_config = (ctrl_reg2_value&0xF1)|(fullscale_config&0x0E);
  HAL_I2C_Mem_Write(&hi2c1, LSM6DS3_ADDR_WR, LSM6DS3_CTRL2_G, I2C_MEMADD_SIZE_8BIT, &fullscale_config, 1, 0xFFFF);
}

void LSM6DS3_Set_Accel_FullScale_Num(uint8_t fullscale_num)
{
  LSM6DS3_AccelFullscaleTypeDef fullscale = ACCEL_FULLSCALE_2;
  switch(fullscale_num)
  {  
  case 2:
    {
      fullscale = ACCEL_FULLSCALE_2;
      break;
    }
  case 16:
    {
      fullscale = ACCEL_FULLSCALE_16;
      break;
    }
  case 4:
    {
      fullscale = ACCEL_FULLSCALE_4;
      break;
    }
  case 8:
    {
      fullscale = ACCEL_FULLSCALE_8;
      break;
    }
  default:
    {
      break;
    }
  }
  LSM6DS3_Set_Accel_FullScale(fullscale);
}

void LSM6DS3_Set_Gyro_FullScale_Num(uint8_t fullscale_num)
{  
  LSM6DS3_GyroFullscaleTypeDef fullscale = GYRO_FULLSCALE_250;
  switch(fullscale_num)
  {  
  case 125:
    {
      fullscale = GYRO_FULLSCALE_125;
      break;
    }
  case 250:
    {
      fullscale = GYRO_FULLSCALE_250;
      break;
    }
  case 500:
    {
      fullscale = GYRO_FULLSCALE_500;
      break;
    }
  case 1000:
    {
      fullscale = GYRO_FULLSCALE_1000;
      break;
    }
  case 2000:
    {
      fullscale = GYRO_FULLSCALE_2000;
      break;
    }
  default:
    {
      break;
    }
  }
  LSM6DS3_Set_Gyro_FullScale(fullscale);
}

uint8_t LSM6DS3_Get_Accel_FullScale_Num(LSM6DS3_AccelFullscaleTypeDef fullscale)
{
  uint8_t fullscale_num = 1;
  switch(fullscale)
  {  
  case ACCEL_FULLSCALE_2:
    {
      fullscale_num = 2;
      break;
    }
  case ACCEL_FULLSCALE_16:
    {
      fullscale_num = 16;
      break;
    }
  case ACCEL_FULLSCALE_4:
    {
      fullscale_num = 4;
      break;
    }
  case ACCEL_FULLSCALE_8:
    {
      fullscale_num = 8;
      break;
    }
  default:
    {
      break;
    }
  }
  return fullscale_num;
}

uint16_t LSM6DS3_Get_Gyro_FullScale_Num(LSM6DS3_GyroFullscaleTypeDef fullscale)
{
  uint16_t fullscale_num = 1;
  switch(fullscale)
  {  
  case GYRO_FULLSCALE_125:
    {
      fullscale_num = 125;
      break;
    }
  case GYRO_FULLSCALE_250:
    {
      fullscale_num = 250;
      break;
    }
  case GYRO_FULLSCALE_500:
    {
      fullscale_num = 500;
      break;
    }
  case GYRO_FULLSCALE_1000:
    {
      fullscale_num = 1000;
      break;
    }
  case GYRO_FULLSCALE_2000:
    {
      fullscale_num = 2000;
      break;
    }
  default:
    {
      break;
    }
  }
  return fullscale_num;
}

LSM6DS3_AccelFullscaleTypeDef LSM6DS3_Get_Accel_FullScale()
{
  uint8_t fullscale;
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_CTRL1_XL, I2C_MEMADD_SIZE_8BIT, &fullscale, 1, 0xFFFF);
  fullscale = (fullscale<<1)>>6;
  return (LSM6DS3_AccelFullscaleTypeDef)fullscale;
}

LSM6DS3_GyroFullscaleTypeDef LSM6DS3_Get_Gyro_FullScale()
{
  uint8_t fullscale;
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_CTRL2_G, I2C_MEMADD_SIZE_8BIT, &fullscale, 1, 0xFFFF);
  fullscale = fullscale&0x02;
  if(fullscale == 0x00) fullscale = fullscale&0x0C;
  return (LSM6DS3_GyroFullscaleTypeDef)fullscale;
}

uint16_t LSM6DS3_Get_Accel_Sensitivity(LSM6DS3_AccelFullscaleTypeDef fullscale)
{
  uint16_t sensitivity = 1;
  switch(fullscale)
  {
    case ACCEL_FULLSCALE_2:{
      sensitivity = 61;
      break;
    }
    case ACCEL_FULLSCALE_4:{
      sensitivity = 122;
      break;
    }
    case ACCEL_FULLSCALE_8:{
      sensitivity = 244;
      break;
    }
    case ACCEL_FULLSCALE_16:{
      sensitivity = 488;
      break;
    }
    default:{
      sensitivity = 1;
    }
  }
  return sensitivity;
}

uint32_t LSM6DS3_Get_Gyro_Sensitivity(LSM6DS3_GyroFullscaleTypeDef fullscale)
{
  uint32_t sensitivity = 1;
  switch(fullscale)
  {
    case GYRO_FULLSCALE_125:{
      sensitivity = 4375;
      break;
    }
    case GYRO_FULLSCALE_250:{
      sensitivity = 8750;
      break;
    }
    case GYRO_FULLSCALE_500:{
      sensitivity = 17500;
      break;
    }
    case GYRO_FULLSCALE_1000:{
      sensitivity = 35000;
      break;
    }
    case GYRO_FULLSCALE_2000:{
      sensitivity = 70000;
      break;
    }
    default:{
      sensitivity = 1;
    }
  }
  return sensitivity;
}

uint8_t LSM6DS3_Get_Sensor_Config(sensor_motion_t* sensor_motion)
{
  LSM6DS3_AccelFullscaleTypeDef accel_fullscale = LSM6DS3_Get_Accel_FullScale();
  sensor_motion->accelFullscale = LSM6DS3_Get_Accel_FullScale_Num(accel_fullscale);
  sensor_motion->accelSensitivity = LSM6DS3_Get_Accel_Sensitivity(accel_fullscale);
  
  LSM6DS3_GyroFullscaleTypeDef gyro_fullscale = LSM6DS3_Get_Gyro_FullScale();
  sensor_motion->gyroFullscale = LSM6DS3_Get_Gyro_FullScale_Num(gyro_fullscale);
  sensor_motion->gyroSensitivity = LSM6DS3_Get_Gyro_Sensitivity(gyro_fullscale);
  
  return 0;
}

uint8_t LSM6DS3_Get_Accel(sensor_motion_t* sensor_motion)
{
  uint8_t accelx_h, accelx_l, accely_h, accely_l, accelz_h, accelz_l;
  uint8_t status_dat = 0;
  
  while((status_dat&LSM6DS3_XL_DA) != LSM6DS3_XL_DA)
  {
    HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status_dat, 1, 0xFFFF);
  }
  
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTX_H_XL, I2C_MEMADD_SIZE_8BIT, &accelx_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTX_L_XL, I2C_MEMADD_SIZE_8BIT, &accelx_l, 1, 0xFFFF);
  sensor_motion->accelX = (uint16_t)accelx_h<<8|accelx_l;
  
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTY_H_XL, I2C_MEMADD_SIZE_8BIT, &accely_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTY_L_XL, I2C_MEMADD_SIZE_8BIT, &accely_l, 1, 0xFFFF);
  sensor_motion->accelY = (uint16_t)accely_h<<8|accely_l;

  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTZ_H_XL, I2C_MEMADD_SIZE_8BIT, &accelz_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTZ_L_XL, I2C_MEMADD_SIZE_8BIT, &accelz_l, 1, 0xFFFF);
  sensor_motion->accelZ = (uint16_t)accelz_h<<8|accelz_l;
  
  return 0;
}

uint8_t LSM6DS3_Get_Gyro(sensor_motion_t* sensor_motion)
{
  uint8_t gyrox_h, gyrox_l, gyroy_h, gyroy_l, gyroz_h, gyroz_l;
  uint8_t status_dat = 0;
  
  while((status_dat&LSM6DS3_G_DA) != LSM6DS3_G_DA)
  {
    HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status_dat, 1, 0xFFFF);
  }
  
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTX_H_G, I2C_MEMADD_SIZE_8BIT, &gyrox_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTX_L_G, I2C_MEMADD_SIZE_8BIT, &gyrox_l, 1, 0xFFFF);
  sensor_motion->gyroX = (uint16_t)gyrox_h<<8|gyrox_l;
  
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTY_H_G, I2C_MEMADD_SIZE_8BIT, &gyroy_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTY_L_G, I2C_MEMADD_SIZE_8BIT, &gyroy_l, 1, 0xFFFF);
  sensor_motion->gyroY = (uint16_t)gyroy_h<<8|gyroy_l;

  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTZ_H_G, I2C_MEMADD_SIZE_8BIT, &gyroz_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_OUTZ_L_G, I2C_MEMADD_SIZE_8BIT, &gyroz_l, 1, 0xFFFF);
  sensor_motion->gyroZ = (uint16_t)gyroz_h<<8|gyroz_l;
  
  return 0;
}

uint8_t LSM6DS3_Get_Step(sensor_motion_t* sensor_motion)
{
  uint8_t step_h, step_l;
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_STEP_COUNTER_H, I2C_MEMADD_SIZE_8BIT, &step_h, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LSM6DS3_ADDR_RD, LSM6DS3_STEP_COUNTER_L, I2C_MEMADD_SIZE_8BIT, &step_l, 1, 0xFFFF);
  sensor_motion->stepCount = (uint16_t)step_h<<8|step_l;
  return 0;
}
