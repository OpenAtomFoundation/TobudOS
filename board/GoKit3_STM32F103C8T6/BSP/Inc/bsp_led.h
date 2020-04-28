/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-15 20:51:38
 * @LastEditTime : 2020-02-16 00:24:14
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _BSP_LED_H
#define	_BSP_LED_H

#include "stm32f1xx_hal.h"

#define  R_MAX  255
#define  G_MAX  255
#define  B_MAX  255

#define RGB_LED_CLK_PIN                  GPIO_PIN_8             
#define RGB_LED_CLK_GPIO_PORT            GPIOB  

#define RGB_LED_SDA_PIN                  GPIO_PIN_9          
#define RGB_LED_SDA_GPIO_PORT            GPIOB        

#define RGB_LED_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

#define SCL_LOW 	HAL_GPIO_WritePin(RGB_LED_CLK_GPIO_PORT, RGB_LED_CLK_PIN, GPIO_PIN_RESET)
#define SCL_HIGH 	HAL_GPIO_WritePin(RGB_LED_CLK_GPIO_PORT, RGB_LED_CLK_PIN, GPIO_PIN_SET)

#define SDA_LOW		HAL_GPIO_WritePin(RGB_LED_SDA_GPIO_PORT, RGB_LED_SDA_PIN, GPIO_PIN_RESET)
#define SDA_HIGH	HAL_GPIO_WritePin(RGB_LED_SDA_GPIO_PORT, RGB_LED_SDA_PIN, GPIO_PIN_SET)

void rgb_led_init(void);
void rgb_led_control(uint8_t r,uint8_t g,uint8_t b);

#endif /* _BSP_LED_H */
