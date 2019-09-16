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

#include "tos.h"

void board_init(void);
void SystemClock_Config(void);
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif
#endif /*__ __MCU_INIT_H */
