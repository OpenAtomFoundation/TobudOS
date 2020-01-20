#ifndef __MCU_INIT_H
#define __MCU_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "usart.h"
#include "gpio.h"
#include "rng.h"
#include "tos_k.h"

void board_init(void);
void SystemClock_Config(void);
void Error_Handler(void);

#ifdef __cplusplus
}
#endif
#endif /*__ __MCU_INIT_H */
