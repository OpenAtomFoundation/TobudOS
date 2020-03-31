/**
  ******************************************************************************
  * @file       LPS22HB.h
  * @author     jieranzhi (the developer)
  * @update     2020/03/23 19:00 CST
  * @brief      This file contains basic functions prototypes and pre-definitions 
  *             of the register addresses
  ******************************************************************************
  * @attention
  *
  * 1. the temperature sensor embedded in the LPS22HB is intended to be embedded 
  *    temperature compensation. Therefore, in this file we DO NOT include the
  *    temperature output
  *
  * 2. on the P-NUCLEO-LRWAN3, the SDO/SA0 pad is connected to voltage supply(
  *    via a resistor), LSb is ¡®1¡¯ (address 1011101b)
  *
  * 3. for more information, please refer to the datasheet
  *    (https://www.st.com/resource/en/datasheet/dm00140895.pdf)
  *
  ******************************************************************************
  */ 
#ifndef	_LPS22HB_H_
#define	_LPS22HB_H_

#include <stm32l0xx_hal.h>

/* Registers -----------------------------------------------------------------*/
#define LPS22HB_ADDR_WR	0xBA
#define LPS22HB_ADDR_RD	0xBB

#define LPS22HB_CTRL_REG1 0x10
#define LPS22HB_CTRL_REG2 0x11
#define LPS22HB_CTRL_REG3 0x12

#define LPS22HB_RES_CONF_REG    0x1A
#define LPS22HB_STATUS_REG	0x27

#define LPS22HB_PRESS_OUT_XL    0x28
#define LPS22HB_PRESS_OUT_L     0x29
#define LPS22HB_PRESS_OUT_H     0x2A

/* Enumeration ---------------------------------------------------------------*/
/** 
  * @brief  STATUS structures definition  
  */ 
typedef enum
{
  LPS22HB_T_OR  = 0x20,  /*!< temperature overrun            */  
  LPS22HB_P_OR  = 0x10,  /*!< pressure overrun               */
  LPS22HB_T_DA  = 0x02,  /*!< new temperature data available */     
  LPS22HB_P_DA  = 0x01,  /*!< new pressure data available    */  
}LPS22HB_StatusTypeDef;

/** 
  * @brief  pressure sensor structures definition  
  */ 
typedef struct
{
  uint16_t sensitivity;                  // sensitivity per fullscale
  uint32_t pressure;                       // X-magnetic value in LSB
}sensor_press_t;

/* Functions -----------------------------------------------------------------*/
void LPS22HB_Init(void);
uint8_t LPS22HB_Get_Press(sensor_press_t* press_sensor);

#endif /*  _LPS22HB_H_ */
