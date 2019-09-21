#ifndef __MCU_INIT_H
#define __MCU_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "tos.h"
#include "i2c.h"
#include "bh1750.h"
#include "spi.h"
#include "lcd_spi2_drv.h"


void board_init(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif
#endif /*__ __MCU_INIT_H */
