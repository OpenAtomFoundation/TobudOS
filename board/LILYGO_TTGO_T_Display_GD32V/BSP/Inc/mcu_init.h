#ifndef __MCU_INIT_H
#define __MCU_INIT_H

#include "gd32vf103.h"
#include "usart.h"


#define LEDR_GPIO_PORT  GPIOC
#define LEDG_GPIO_PORT  GPIOA
#define LEDB_GPIO_PORT  GPIOA

#define LEDR_PIN        GPIO_PIN_13
#define LEDG_PIN        GPIO_PIN_1
#define LEDB_PIN        GPIO_PIN_2

void board_init();

#endif //__MCU_INIT_H
