/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ETS_UART_INTR_ENABLE()  _xt_isr_unmask(1 << ETS_UART_INUM)
#define ETS_UART_INTR_DISABLE() _xt_isr_mask(1 << ETS_UART_INUM)
#define UART_INTR_MASK          0x1ff
#define UART_LINE_INV_MASK      (0x3f<<19)

typedef enum {
    UART_WordLength_5b = 0x0,
    UART_WordLength_6b = 0x1,
    UART_WordLength_7b = 0x2,
    UART_WordLength_8b = 0x3
} UART_WordLength;

typedef enum {
    USART_StopBits_1   = 0x0,
    USART_StopBits_2   = 0x1,
} UART_StopBits;

typedef enum {
    UART0 = 0x0,
    UART1 = 0x1,
} UART_Port;

typedef enum {
    USART_Parity_None = 0x2,
    USART_Parity_Even = 0x0,
    USART_Parity_Odd  = 0x1
} UART_ParityMode;

typedef enum {
    PARITY_DIS = 0x0,
    PARITY_EN  = 0x2
} UartExistParity;

typedef enum {
    BIT_RATE_300     = 300,
    BIT_RATE_600     = 600,
    BIT_RATE_1200    = 1200,
    BIT_RATE_2400    = 2400,
    BIT_RATE_4800    = 4800,
    BIT_RATE_9600    = 9600,
    BIT_RATE_19200   = 19200,
    BIT_RATE_38400   = 38400,
    BIT_RATE_57600   = 57600,
    BIT_RATE_74880   = 74880,
    BIT_RATE_115200  = 115200,
    BIT_RATE_230400  = 230400,
    BIT_RATE_460800  = 460800,
    BIT_RATE_921600  = 921600,
    BIT_RATE_1843200 = 1843200,
    BIT_RATE_3686400 = 3686400,
} UART_BautRate; //you can add any rate you need in this range

typedef enum {
    USART_HardwareFlowControl_None    = 0x0,
    USART_HardwareFlowControl_RTS     = 0x1,
    USART_HardwareFlowControl_CTS     = 0x2,
    USART_HardwareFlowControl_CTS_RTS = 0x3
} UART_HwFlowCtrl;

//typedef enum {
//    UART_None_Inverse = 0x0,
//    UART_Rxd_Inverse  = UART_RXD_INV,
//    UART_CTS_Inverse  = UART_CTS_INV,
//    UART_Txd_Inverse  = UART_TXD_INV,
//    UART_RTS_Inverse  = UART_RTS_INV,
//} UART_LineLevelInverse;

typedef struct {
    UART_BautRate   baud_rate;
    UART_WordLength data_bits;
    UART_ParityMode parity;    // chip size in byte
    UART_StopBits   stop_bits;
    UART_HwFlowCtrl flow_ctrl;
    u8           UART_RxFlowThresh ;
//    u32          UART_InverseMask;
} UART_ConfigTypeDef;

typedef struct {
    u32 UART_IntrEnMask;
    u8  UART_RX_TimeOutIntrThresh;
    u8  UART_TX_FifoEmptyIntrThresh; //txfifo trigger level
    u8  UART_RX_FifoFullIntrThresh;  //rxfifo trigger level
} UART_IntrConfTypeDef;

//=======================================

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup UART_Driver_APIs UART Driver APIs
  * @brief UART driver APIs
  */

/** @addtogroup UART_Driver_APIs
  * @{
  */

/**  
  * @brief   Wait uart tx fifo empty, do not use it if tx flow control enabled.
  * 
  * @param   UART_Port uart_no:UART0 or UART1
  *  
  * @return  null
  */
void UART_WaitTxFifoEmpty(UART_Port uart_no); //do not use if tx flow control enabled

/**  
  * @brief   Clear uart tx fifo and rx fifo.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  *  
  * @return  null
  */
void UART_ResetFifo(UART_Port uart_no);

/**  
  * @brief  Clear uart interrupt flags.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   u32 clr_mask : To clear the interrupt bits
  *  
  * @return  null
  */
void UART_ClearIntrStatus(UART_Port uart_no, u32 clr_mask);

/**  
  * @brief   Enable uart interrupts .
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   u32 ena_mask : To enable the interrupt bits
  *  
  * @return  null
  */
void UART_SetIntrEna(UART_Port uart_no, u32 ena_mask);

/**  
  * @brief   Register an application-specific interrupt handler for Uarts interrupts.
  *
  
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   void *fn : interrupt handler for Uart interrupts.
  * @param   void *arg : interrupt handler's arg.
  *  
  * @return  null
  */
void UART_intr_handler_register(UART_Port uart_no, void *fn, void *arg);

/**  
  * @brief   Config from which serial output printf function.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  *  
  * @return  null
  */
void UART_SetPrintPort(UART_Port uart_no);

/**  
  * @brief   Config Common parameters of serial ports.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   UART_ConfigTypeDef *pUARTConfig : parameters structure
  *  
  * @return  null
  */
void UART_ParamConfig(UART_Port uart_no,  UART_ConfigTypeDef *pUARTConfig);

/**  
  * @brief   Config types of uarts.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   UART_IntrConfTypeDef *pUARTIntrConf : parameters structure
  *  
  * @return  null
  */
void UART_IntrConfig(UART_Port uart_no,  UART_IntrConfTypeDef *pUARTIntrConf);

/**  
  * @brief   Config the length of the uart communication data bits.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   UART_WordLength len : the length of the uart communication data bits
  *  
  * @return  null
  */
void UART_SetWordLength(UART_Port uart_no, UART_WordLength len);

/**  
  * @brief   Config the length of the uart communication stop bits.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   UART_StopBits bit_num : the length uart communication stop bits
  *  
  * @return  null
  */
void UART_SetStopBits(UART_Port uart_no, UART_StopBits bit_num);

/**  
  * @brief   Configure whether to open the parity.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   UART_ParityMode Parity_mode : the enum of uart parity configuration
  *  
  * @return  null
  */
void UART_SetParity(UART_Port uart_no, UART_ParityMode Parity_mode) ;

/**  
  * @brief   Configure the Baud rate.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   u32 baud_rate : the Baud rate
  *  
  * @return  null
  */
void UART_SetBaudrate(UART_Port uart_no, u32 baud_rate);

/**  
  * @brief   Configure Hardware flow control.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   UART_HwFlowCtrl flow_ctrl : Hardware flow control mode
  * @param   u8 rx_thresh : threshold of Hardware flow control
  *  
  * @return  null
  */
void UART_SetFlowCtrl(UART_Port uart_no, UART_HwFlowCtrl flow_ctrl, u8 rx_thresh);

/**  
  * @brief   Configure trigging signal of uarts.
  * 
  * @param   UART_Port uart_no : UART0 or UART1
  * @param   UART_LineLevelInverse inverse_mask : Choose need to flip the IO
  *  
  * @return  null
  */
//void UART_SetLineInverse(UART_Port uart_no, UART_LineLevelInverse inverse_mask) ;

/**  
  * @brief   An example illustrates how to configure the serial port.
  * 
  * @param   null
  *  
  * @return  null
  */
void uart_init_new(void);


void uart0_write_char(char c);
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
