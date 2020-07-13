/**
  ******************************************************************************
  * @file       LPS22HB.c
  * @author     jieranzhi
  * @update     2020/03/23 19:00 CST
  * @brief      This file provides code for the LPS22HB Initialization 
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

#include <LPS22HB.h>
#include <i2c.h>

// initialization of LPS22HB
void LPS22HB_Init()
{
  uint8_t cmd = 0;
  
  // reset the Low-power mode configuration
  cmd = 0x00;
  HAL_I2C_Mem_Write(&hi2c1, LPS22HB_ADDR_WR, LPS22HB_RES_CONF_REG, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
                    
  // setup data rate(power down, 000); disable lowpass filter (we use one-shot in this case); Block data update(continuous)
  cmd = 0x00;
  HAL_I2C_Mem_Write(&hi2c1, LPS22HB_ADDR_WR, LPS22HB_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
                    
  // reboot mode(normal); disable FIFO; enable IF_ADD_INC; enable I2C; software reset mode(normal); one-shot mode(idle)
  cmd = 0x10;
  HAL_I2C_Mem_Write(&hi2c1, LPS22HB_ADDR_WR, LPS22HB_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  
  // interupt control: set to default value (0x00)
  cmd = 0x00;
  HAL_I2C_Mem_Write(&hi2c1, LPS22HB_ADDR_WR, LPS22HB_CTRL_REG3, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
}

// enable one-shot to start a new acquisition (conversion)
static void LPS22HB_Start()
{
  uint8_t cmd = 0;
  HAL_I2C_Mem_Read(&hi2c1, LPS22HB_ADDR_RD, LPS22HB_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
  cmd |= 0x01;
  HAL_I2C_Mem_Write(&hi2c1, LPS22HB_ADDR_WR, LPS22HB_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFFFF);
} 

// start a new acquisition by enabling the one-shot bit in the LPS22HB_CTRL_REG2
// and read the pressure from the sensor
uint8_t LPS22HB_Get_Press(sensor_press_t* press_sensor)
{
  uint8_t status_dat = 0;
  uint8_t pressure_out_l = 0;
  uint8_t pressure_out_h = 0;
  uint8_t pressure_out_xl = 0;
  press_sensor->sensitivity = 4096;
  
  LPS22HB_Start();
  while((status_dat&LPS22HB_P_DA) != LPS22HB_P_DA)
  {
    HAL_I2C_Mem_Read(&hi2c1, LPS22HB_ADDR_RD, LPS22HB_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &status_dat, 1, 0xFFFF);
  }
  
  HAL_I2C_Mem_Read(&hi2c1, LPS22HB_ADDR_RD, LPS22HB_PRESS_OUT_XL, I2C_MEMADD_SIZE_8BIT, &pressure_out_xl, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LPS22HB_ADDR_RD, LPS22HB_PRESS_OUT_L, I2C_MEMADD_SIZE_8BIT, &pressure_out_l, 1, 0xFFFF);
  HAL_I2C_Mem_Read(&hi2c1, LPS22HB_ADDR_RD, LPS22HB_PRESS_OUT_H, I2C_MEMADD_SIZE_8BIT, &pressure_out_h, 1, 0xFFFF);
  
  press_sensor->pressure = (((uint32_t)pressure_out_h<<16) | ((uint16_t)pressure_out_l<<8) | pressure_out_xl);
  return 0;
}
