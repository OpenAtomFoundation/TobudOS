/**
  ******************************************************************************
  * @file    uart.h
  * @author  YANDLD
  * @version V2.6
  * @date    2015.6.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_KL_UART_H__
#define __CH_LIB_KL_UART_H__

#include <stdint.h>
#include <stdbool.h>

/*!< UART 硬件模块号 */
#define HW_UART0  (0x00U)
#define HW_UART1  (0x01U)
#define HW_UART2  (0x02U)


/*!< UART 快速初始化宏 */                      
#define UART1_RX_PE01_TX_PE00   (0x000080E1U)
#define UART2_RX_PE17_TX_PE16   (0x0000A0E2U)
#define UART0_RX_PE21_TX_PE20   (0x0000A920U)
#define UART2_RX_PE23_TX_PE22   (0x0000AD22U)
#define UART0_RX_PA01_TX_PA02   (0x00008280U)
#define UART0_RX_PA15_TX_PA14   (0x00009CC0U)
#define UART1_RX_PA18_TX_PA19   (0x0000A4C1U)
#define UART0_RX_PB16_TX_PB17   (0x0000A0C8U)
#define UART2_RX_PD02_TX_PD03   (0x000084DAU)
#define UART2_RX_PD04_TX_PD05   (0x000088DAU)
#define UART0_RX_PD06_TX_PD07   (0x00008CD8U)

typedef enum
{
    kUART_IntTx,
    kUART_IntRx,
    kUART_IntIdleLine,
}UART_Int_t;

typedef enum
{
    kUART_DMATx,
    kUART_DMARx,
}UART_DMA_t;


/*!< API functions */
uint32_t UART_Init(uint32_t MAP, uint32_t baudrate);
uint8_t UART_GetChar(uint32_t instance, uint8_t *ch);
void UART_PutChar(uint32_t instance, uint8_t ch);
uint32_t UART_SetIntMode(uint32_t instance, UART_Int_t mode, bool val);
uint32_t UART_SetDMAMode(uint32_t instance, UART_DMA_t mode, bool val);
void UART_SetBaudRate(uint32_t instance, uint32_t baud);



#if (CHLIB_DMA_SUPPORT == 1)
uint32_t UART_DMASend(uint32_t instance, uint32_t dmaChl, uint8_t *buf, uint32_t len);
uint32_t UART_DMAGetRemain(uint32_t instance);
#endif


#endif


