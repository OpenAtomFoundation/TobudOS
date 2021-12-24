#ifndef __MCU_INIT_H
#define __MCU_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "tos_k.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_gpio.h"

void board_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ __MCU_INIT_H */
