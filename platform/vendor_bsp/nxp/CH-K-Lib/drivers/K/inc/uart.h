/**
  ******************************************************************************
  * @file    uart.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片UART模块的底层功能函数
  ******************************************************************************
  */
#ifndef __CH_LIB_UART_H__
#define __CH_LIB_UART_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


/* UART 硬件模块号 */
#define HW_UART0  (0x00U)  /* 芯片的UART0端口，依次类推 */
#define HW_UART1  (0x01U)
#define HW_UART2  (0x02U)
#define HW_UART3  (0x03U)
#define HW_UART4  (0x04U)
#define HW_UART5  (0x05U)

/* UART 快速初始化宏 */                      
#define UART1_RX_PE01_TX_PE00   (0x80E1U) /* 芯片的UART1端口，使用PTE1引脚为接收引脚，使用PTE0引脚为发送引脚，依次类推*/
#define UART0_RX_PF17_TX_PF18   (0xA528U)
#define UART3_RX_PE05_TX_PE04   (0x88E3U)
#define UART5_RX_PF19_TX_PF20   (0xA72DU)
#define UART5_RX_PE09_TX_PE08   (0x90E5U)
#define UART2_RX_PE17_TX_PE16   (0xA0E2U)
#define UART4_RX_PE25_TX_PE24   (0xB0E4U)
#define UART0_RX_PA01_TX_PA02   (0x8280U)
#define UART0_RX_PA15_TX_PA14   (0x9CC0U)
#define UART3_RX_PB10_TX_PB11   (0x94CBU)
#define UART0_RX_PB16_TX_PB17   (0xA0C8U)
#define UART1_RX_PC03_TX_PC04   (0x86D1U)
#define UART4_RX_PC14_TX_PC15   (0x9CD4U)
#define UART3_RX_PC16_TX_PC17   (0xA0D3U)
#define UART2_RX_PD02_TX_PD03   (0x84DAU)
#define UART0_RX_PD06_TX_PD07   (0x8CD8U)
#define UART2_RX_PF13_TX_PF14   (0x9B2AU) 
#define UART5_RX_PD08_TX_PD09   (0x90DDU) 
#define UART5_RX_PE08_TX_PE09   (0X90E5U)

/**
 * \enum UART_ParityMode_Type
 * \brief UART parity 选择
 */
typedef enum
{
    kUART_ParityDisabled = 0x0,  /**< 校验位禁止 */
    kUART_ParityEven     = 0x2,  /**< 1位 奇校验 */
    kUART_ParityOdd      = 0x3,  /**< 1位 偶校验 */
} UART_ParityMode_Type;

/**
 * \enum UART_BitPerChar_Type
 * \brief UART每帧数据位个数
 */
typedef enum 
{
    kUART_8BitsPerChar  = 0,   /**< 8-bit 数据 不包括校验位 */
    kUART_9BitsPerChar  = 1,   /**< 9-bit 数据 不包括校验位 */
} UART_BitPerChar_Type;

/**
 * \enum UART_ITDMAConfig_Type
 * \brief UART中断及DMA配置
 */
typedef enum
{
    kUART_IT_Tx,                /**< 开启每发送一帧传输完成中断 */
    kUART_DMA_Tx,               /**< 开启每发送一帧传输完成触发DMA */
    kUART_IT_Rx,                /**< 开启每接收一帧传输完成中断 */
    kUART_DMA_Rx,               /**< 开启每接收一帧传输完成触发DMA */
    kUART_IT_IdleLine,
}UART_ITDMAConfig_Type;

/**
 * \struct PIT_InitTypeDef
 * \brief UART初始化结构
 */
typedef struct
{
    uint32_t                srcClock;       ///< 时钟源频率
    uint8_t                 instance;       ///< UART 模块号 HW_UART0~HW_UART5
    uint32_t                baudrate;       ///< UART 波特率 
    UART_ParityMode_Type    parityMode;     ///< UART 校验位 
    UART_BitPerChar_Type    bitPerChar;     ///< UART 每一帧含多少位数据 
}UART_InitTypeDef;

/* UART 回调函数声明 */
typedef void (*UART_CallBackTxType)(uint16_t * pbyteToSend);
typedef void (*UART_CallBackRxType)(uint16_t byteReceived);

/* API functions */
uint8_t UART_QuickInit(uint32_t MAP, uint32_t baudrate);
void UART_Init(UART_InitTypeDef * UART_InitStruct);
void UART_DeInit(uint32_t instance);
int UART_printf(uint32_t instance, const char *format,...);
uint8_t UART_ReadByte(uint32_t instance, uint16_t *ch);
void UART_WriteByte(uint32_t instance, uint16_t ch);
void UART_SelectDebugInstance(uint32_t instance);

/* FIFO functions */
void UART_EnableTxFIFO(uint32_t instance, bool status);
void UART_EnableRxFIFO(uint32_t instance, bool status);
uint32_t UART_GetTxFIFOSize(uint32_t instance);
uint32_t UART_GetRxFIFOSize(uint32_t instance);
void UART_SetTxFIFOWatermark(uint32_t instance, uint32_t size);
void UART_SetRxFIFOWatermark(uint32_t instance, uint32_t size);

/* Interrupt and DMA functions */
void UART_CallbackTxInstall(uint32_t instance, UART_CallBackTxType AppCBFun);
void UART_CallbackRxInstall(uint32_t instance, UART_CallBackRxType AppCBFun);
void UART_ITDMAConfig(uint32_t instance, UART_ITDMAConfig_Type config, bool status);


/* DMA support */
#ifdef DMA0
/*
uint32_t UART_Init();
uint32_t UART_Config();
void UART_SendData();
uint8_t UART_GetData();
*/

void UART_DMASetCmd(uint32_t instance, uint32_t dir, bool status);
void UART_DMASendData(uint32_t instance, uint8_t *buf, uint32_t len);
void UART_DMAGetRemain(uint32_t instacne, uint32_t dir);
#endif

/* param check */
#define IS_UART_ALL_INSTANCE(INSTANCE)  (INSTANCE < ARRAY_SIZE(UART_InstanceTable))

#ifdef __cplusplus
}
#endif

#endif

