#ifndef __MCU_INIT_H
#define __MCU_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "usart.h"
#include "spi.h"
#include "gpio.h"
#include "lcd_spi2_drv.h"
#include "bh1750.h"
#include "tos_k.h"

void m26_power_on(void);
void board_init(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif
#endif /*__ __MCU_INIT_H */
