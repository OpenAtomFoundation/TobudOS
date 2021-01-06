#ifndef __MCU_INIT_H
#define __MCU_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "stm32l4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "dcmi.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "ov2640.h"
#include "lcd_2inch4.h"
#include "tos_k.h"
void board_init(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif
#endif /*__ __MCU_INIT_H */
