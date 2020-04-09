#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#include "ll/ll_uart.h"
#include "hal/hal_common.h"


/****************************  UART Instances: UART  ***********************/
#define IS_UART_INSTANCE( INSTANCE ) ( (INSTANCE == UART0) || (INSTANCE == UART1) )

#define IS_UART_IT_ENABLE_BITMAP( IT_ENABLE ) ( (IT_ENABLE == UART_INT_EN_RECEIVE_DATA_AVAILABLE)  || \
                                                (IT_ENABLE == UART_INT_EN_TRANSMIT_HOLD_REG_EMPTY) || \
                                                (IT_ENABLE == UART_INT_EN_RECEIVER_LINE_STATUS)    || \
                                                (IT_ENABLE == UART_INT_EN_MODEM_STATUS)            || \
                                                (IT_ENABLE == UART_INT_EN_PROGRAMMABLE_THRE) )

        
/**
 * @brief Uart interrupt bit map
 */
typedef enum
{
    UART_INT_EN_RECEIVE_DATA_AVAILABLE  = 0x01,
    UART_INT_EN_TRANSMIT_HOLD_REG_EMPTY = 0x02,
    UART_INT_EN_RECEIVER_LINE_STATUS    = 0x04,
    UART_INT_EN_MODEM_STATUS            = 0x08,
    UART_INT_EN_PROGRAMMABLE_THRE       = 0x80
} Uart_It_Enable;//Enable the specified UART interrupt


/**
 * @brief Rx trigger level. Above the trigger level, an uart rx interrupt will trigger. Uart Rx Fifo is 8 bytes.
 */
typedef enum
{
    UART_RCVR_TRIGGER_FIFO_HAS_ONE_CHARACTER   = 0x00,
    UART_RCVR_TRIGGER_FIFO_ONE_QUARTER_FULL    = 0x01,
    UART_RCVR_TRIGGER_FIFO_HALF_FULL           = 0x02,
    UART_RCVR_TRIGGER_FIFO_TWO_LESS_THAN_FULL  = 0x03
} Uart_RX_Trigger_Lvl;


/**
 * @brief Tx trigger level.Below the trigger level, an uart tx interrupt will trigger. Uart Tx Fifo is 8 bytes.
 */
typedef enum
{
    UART_TX_EMPTY_TRIGGER_FIFO_EMPTY              = 0x00,
    UART_TX_EMPTY_TRIGGER_TWO_CHARACTERS_IN_FIFO  = 0x01,
    UART_TX_EMPTY_TRIGGER_ONE_QUARTER_FULL        = 0x02,
    UART_TX_EMPTY_TRIGGER_HALF_FULL               = 0x03
} Uart_TX_Trigger_Lvl;


/**
 * @brief Set Uart DMA mode
 */
typedef enum
{
    UART_DMA_MODE0 = 0,
    UART_DMA_MODE1
} Uart_Dma_Mode;


/**
 * @brief The status of the uart interrupt.
 */
typedef enum
{
	UART_INT_ID_MODEM_STATUS             = 0,
	UART_INT_ID_NO_INTERRUPT_PENDING     = 1,
	UART_INT_ID_THR_EMPTY                = 2, //Transmitter holding register empty
	UART_INT_ID_RECV_DATA_AVAILABLE      = 4,
	UART_INT_ID_RECV_LINE_STATUS         = 6,
	UART_INT_ID_BUSY_DETECT              = 7,
	UART_INT_ID_RECV_CHAR_TIMEOUT        = 12
} Uart_Int_Id;


/**
 * @brief Data Length Select. The number of bits that may be selected are 5, 6, 7, 8bits
 */
typedef enum
{
	UART_DATALEN_5BIT = 0x00,
	UART_DATALEN_6BIT = 0x01,
	UART_DATALEN_7BIT = 0x02,
	UART_DATALEN_8BIT = 0x03,
} Uart_DataLength;

/**
 * @brief Select type of parity check.
 */
typedef enum
{
	UART_PARITY_NONE = 0,
	UART_PARITY_ODD  = 1,
	UART_PARITY_EVEN = 2,
} Uart_Parity;

/**
 * @brief Number of stop bits. 
 * If set to stop_bit_1_5_OR_2 and the data bits are set to 5, one and a half stop bits is transmitted. Otherwise, two stop bits are transmitted.
 */
typedef enum
{
	UART_STOP_BIT_1 	   = 0,
	UART_STOP_BIT_1_5_OR_2 = 1, //Note:When the UART_DATALEN is set to 5 bits, the stop bit is 1.5 bit;Otherwise,2 stop bits are transmitted. 
} Uart_StopBits;


/**
 * @brief Select mode of flow control. When select hardware flow control, rts is controlled by hardware, otherwise controlled by software.
 */
typedef enum
{
    UART_FLOW_CONTROL_SOFTWARE = 0x00,
    UART_FLOW_CONTROL_HARDWARE = 0x01
} UART_FlowControl;


/**
 * @brief This is the struct to initialize the uart module.
 */
typedef struct
{
    uint32_t         BaudRate;
    Uart_DataLength  DataLength;
    Uart_StopBits    StopBits;
    Uart_Parity      Parity;
    UART_FlowControl FlowControl;
} UART_ConfigTypeDef;


/**
 * @brief Bitmap of line status.
 */
typedef enum
{
	UART_LSR_DATA_READY       = 0x01,
	UART_LSR_OVERRUN_ERR      = 0x02,
	UART_LSR_PARITY_ERR       = 0x04,
	UART_LSR_FRAMING_ERR      = 0x08,
	UART_LSR_BREAK_INT        = 0x10,
	UART_LSR_TX_HOLDING_EMPTY = 0x20,
	UART_LSR_TX_EMPTY         = 0x40,
	UART_LSR_RECV_FIFO_ERR    = 0x80,
    UART_LSR_ADDR_RECEIVED    = 0x100
} Uart_Line_Status;


/**
 * @brief Bitmap of Modem Status
 */
typedef enum
{
    UART_MODEM_STATUS_DELTA_CLEAR_TO_SEND           = 0x01,
    UART_MODEM_STATUS_DELTA_DATA_SET_READY          = 0x02,
    UART_MODEM_STATUS_TRAILING_EDGE_RING_INDICATOR  = 0x04,
    UART_MODEM_STATUS_DELTA_DATA_CARRIER_DETECT     = 0x08,
    UART_MODEM_STATUS_CLEAR_TO_SEND                 = 0x10,
    UART_MODEM_STATUS_DATA_SET_READY                = 0x20,
    UART_MODEM_STATUS_RING_INDICATOR                = 0x40,
    UART_MODEM_STATUS_DATA_CARRIER_DETECT           = 0x80
} Uart_Modem_Status;


/**
 * @brief Bitmap of Uart normal Status
 */
typedef enum
{
    UART_STATUS_UART_BUSY               = 0x01,
    UART_STATUS_TRANSMIT_FIFO_NOT_FULL  = 0x02,
    UART_STATUS_TRANSMIT_FIFO_EMPTY     = 0x04,
    UART_STATUS_RECEIVE_FIFO_NOT_EMPTY  = 0x08,
    UART_STATUS_RECEIVE_FIFO_FULL       = 0x10
} Uart_Status;

typedef struct
{
	UartInstance       *Instance;
	UART_ConfigTypeDef Config;
} UART_DevTypeDef;

typedef void (*pIsrRecvCharCb)(uint8_t *ch);
typedef void (*pIsrSendCharCb)(uint8_t *ch);
void HAL_UART_SetIsrRecvCharCallback(UartInstance * pUARTx, pIsrRecvCharCb callback_fun);
void HAL_UART_SetIsrSendCharCallback(UartInstance * pUARTx, pIsrSendCharCb callback_fun);

/****************************  Function Declaration  ************************/

/**
 * @brief Initialize an UART, config its baudrate, datalenth, stopbits, and parity check, flow control.
 *  
 * @param[in]  pUartDev the UART interface
 * @return  HAL_OK for OK, HAL_ERROR for error.
 */
HAL_StatusTypeDef HAL_UART_Init(UART_DevTypeDef *pUartDev);

/**
 * @brief Deinitialize an UART.
 * 
 * @param hw Serial layer, which contains an UART instance.
 */
HAL_StatusTypeDef HAL_UART_Deinit(UART_DevTypeDef *pUartDev);


/**
 * @brief Set TX, RX FIFO depth and DMA mode.
 * 
 * @param hw Serial layer, which contains an UART instance.
 * @param tx_lvl TX FIFO empty trigger level.
 * @param rx_lvl  RX FIFO trigger level.
 * @param dma_mode DMA mode.
 */
void HAL_UART_FIFOControl(UART_DevTypeDef *pUartDev, Uart_TX_Trigger_Lvl tx_lvl, Uart_RX_Trigger_Lvl rx_lvl, Uart_Dma_Mode dma_mode);


/**
 * @brief Set flow control. (Only UART0 Support AFCE -- hardware flow control.)
 * @param hw Serial layer, which contains an UART instance.
 * @param on true -- for hardware flow control; false for software flow control.
 */
void HAL_UART_Flow_Control(UART_DevTypeDef *pUartDev, bool on);


/**
 * @brief Wait for TX FIFO empty.
 * 
 * @param hw Serial layer, which contains an UART instance.
 */
void HAL_UART_Finish_Transfer(UART_DevTypeDef *pUartDev);

/**
 * Config band rate of a UART interface
 * @param[in]  pUartDev the UART interface
 * @param[in]  baudRate  band rate (9600:误码率,115200:误码率,230400:误码率)
 * @return     HAL_OK for OK, others for error.
 */
HAL_StatusTypeDef HAL_UART_BaudRateConfig(UART_DevTypeDef *pUartDev, uint32_t baudRate);

/**
 * Init a UART interface
 * @param[in]  pUartDev the UART interface
 * @return  HAL_OK for OK, HAL_ERROR for error.
 */
HAL_StatusTypeDef HAL_UART_Init_Raw(UART_DevTypeDef *pUartDev);

/**
 * @brief Get Modem Status, which contains information like Data Carrier Detect,
 * Ring Indicator, Data Set Ready, Clear to Send, Delta Data Carrier Detect, 
 * Trailing Edge of Ring Indicator, Delta Data Set Ready, Delta Clear to Send.
 * 
 * @param hw Serial layer, which contains an UART instance.
 * @return uint8_t Modem status, ORed bits, @see Uart_Modem_Status.
 */
Uart_Modem_Status HAL_UART_Get_Modem_Status(UART_DevTypeDef *pUartDev);


/**
 * @brief Set Low Power parameter. (Only UART0 supports low power mode.)
 * 
 * @param hw Serial layer, which contains an UART instance.
 * @param divisor 
 */
void HAL_UART_Low_Power_Set(UART_DevTypeDef *pUartDev, uint16_t divisor);


/**
 * @brief Enable/Disable specifical UART Interrupt.
 * 
 * @param hw Serial layer, which contains an UART instance.
 * @param en true -- enabe; false -- disable.
 */
void HAL_UART_INT_Switch_RecvDataAvailable(UART_DevTypeDef *pUartDev, uint8_t en);
void HAL_UART_INT_Switch_TransmitHoldingRegEmpty(UART_DevTypeDef *pUartDev, uint8_t en);
void HAL_UART_INT_Switch_RecvLineStatus(UART_DevTypeDef *pUartDev, uint8_t en);
void HAL_UART_INT_Switch_ModemStatus(UART_DevTypeDef *pUartDev, uint8_t en);
void HAL_UART_INT_Switch_ProgramTransmitHoldingRegEmpty(UART_DevTypeDef *pUartDev, uint8_t en);


/**
 * Read one byte on a UART interface in the polling mode
 * @param[in]  pUartDev the UART interface
 * @param[in]  pData    pointer to the data
 * @return  HAL_OK for OK, HAL_TIMEOUT for timeout.
 */
HAL_StatusTypeDef HAL_UART_ReadOneChar(UartInstance *pUARTx, uint8_t * pData);


/**
 * Write one byte on a UART interface in the polling mode
 * @param[in]  pUartDev the UART interface
 * @param[in]  pData    pointer to the data
 * @return  HAL_OK for OK, HAL_TIMEOUT for timeout.
 */
HAL_StatusTypeDef HAL_UART_WriteOneChar(UartInstance *pUARTx, uint8_t * pData);


/**
 * Receive data on a UART interface in the polling mode
 * @param[in]  pUartDev   the UART interface
 * @param[in]  pData      pointer to the start of data
 * @param[in]  expectSize number of bytes to transmit
 * @param[in]  timeOut    timeOut in milisecond, set this value to HAL_WAIT_FOREVER
 *                        if you want to wait forever
 * @return  >=0 : actual number of recive, -1 : if an error occurred with any step
 */
int32_t HAL_UART_ReceiveDataPolling(UART_DevTypeDef *pUartDev, void * pData, uint32_t expectSize, uint32_t timeOut);



/**
 * Transmit data on a UART interface in the polling mode
 * @param[in]  pUartDev the UART interface
 * @param[in]  pData    pointer to the start of data
 * @param[in]  dataLen     number of bytes to transmit
 * @param[in]  timeOut  timeOut in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 * @return  >=0 : actual number of sent, -1 : if an error occurred with any step
 */
int32_t HAL_UART_TransmitDataPolling(UART_DevTypeDef *pUartDev, const void * pData, uint32_t dataLen, uint32_t timeOut);


/**
 * @brief Enable/Disable all UART interrupts.
 * 
 * @param pUartDev  the UART interface.
 * @param enable 
 */
void HAL_UART_INT_Switch_All(UART_DevTypeDef *pUartDev, bool enable);




#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HAL_UART_H__ */
