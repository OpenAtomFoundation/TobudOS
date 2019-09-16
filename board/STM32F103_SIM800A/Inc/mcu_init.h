#ifndef __MCU_INIT_H_
#define __MCU_INIT_H_

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include "hal_spi_flash.h"
#include "usb.h"

#include "cmsis_os.h"

#ifdef __cplusplus
 extern "C" {
#endif

void board_init(void);
void SystemClock_Config(void);
void sim800a_power_on(void);
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MCU_INIT_H_ */
