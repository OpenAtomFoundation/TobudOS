
#ifndef __LED_H
#define __LED_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOB


void led0_toggle(void);
void led1_toggle(void);


#ifdef __cplusplus
}
#endif
#endif

