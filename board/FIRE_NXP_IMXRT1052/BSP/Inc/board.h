#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME                    "YH i.MX RT1052 Board"

/* 调试串口定义的信息 */
#define BOARD_DEBUG_UART_TYPE         DEBUG_CONSOLE_DEVICE_TYPE_LPUART
#define BOARD_DEBUG_UART_BASEADDR     (uint32_t) LPUART1
#define BOARD_DEBUG_UART_INSTANCE     1U

#define BOARD_DEBUG_UART_CLK_FREQ     BOARD_DebugConsoleSrcFreq()

#define BOARD_UART_IRQ                LPUART1_IRQn
#define BOARD_UART_IRQ_HANDLER        LPUART1_IRQHandler

#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE     (115200U)
#endif /* BOARD_DEBUG_UART_BAUDRATE */

/*! @brief FLASH空间大小 */
#define BOARD_FLASH_SIZE    (0x2000000U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
uint32_t BOARD_DebugConsoleSrcFreq(void);

void BOARD_InitDebugConsole(void);

void BOARD_ConfigMPU(void);
  
void CopyAndUseRAMVectorTable(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
