/******************************************************************************
* File:             gpio.h
*
* Author:           iysheng@163.com  
* Created:          04/14/22 
*****************************************************************************/

#ifndef __GPIO_H__
#define __GPIO_H__
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gd32f3x0_libopt.h"

#define LED_Pin       GPIO_PIN_1
#define LED_GPIO_Port GPIOA

/**
  * @brief GPIO inint
  * @param void: 
  * retval N/A.
  */
void board_gpio_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__PINOUT_H__ */
