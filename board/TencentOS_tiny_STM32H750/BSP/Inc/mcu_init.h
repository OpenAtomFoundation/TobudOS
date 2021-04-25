#ifndef __MCU_INIT_H
#define __MCU_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"
#include "dac.h"
#include "dcmi.h"
#include "hdmi_cec.h"
#include "i2c.h"
#include "i2s.h"
#include "ltdc.h"
#include "quadspi.h"
#include "sai.h"
#include "sdmmc.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fmc.h"

#include "tos_k.h"

void board_init(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __MCU_INIT_H */

