#ifndef __MCU_INIT_H
#define __MCU_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "tos.h"

void board_init(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif
#endif /*__ __MCU_INIT_H */
