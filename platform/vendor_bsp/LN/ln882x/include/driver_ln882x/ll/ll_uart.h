#ifndef __LL_UART_H__
#define __LL_UART_H__

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "ln88xx.h"

typedef void UartInstance;
#define UART0   ((UartInstance *) REG_UART0_BASE) 
#define UART1   ((UartInstance *) REG_UART1_BASE)

/////////// Do not modify this section/////////////
#define UART_DLF_SIZE  (4)

static inline uint16_t uart_divisor_integer(uint32_t bus_clk, uint32_t baudrate)
{
    return (uint16_t)((bus_clk/16u) / baudrate);
}
static inline uint8_t uart_dlf(uint32_t bus_clk, uint32_t baudrate)
{
    uint32_t integer1 = ((bus_clk/16u) / baudrate) * (1 << UART_DLF_SIZE);
    float    integer2 = (float)(((float)bus_clk/16.0f)/((float)baudrate) * (1 << UART_DLF_SIZE));
    float fractional =  integer2 - integer1;
    return (uint8_t)(fractional + 0.5f);
}
////////////////////////////////////////////////////

/**
 * @brief Uart interrupt bit map
 */
#define LL_UART_INT_EN_RECEIVE_DATA_AVAILABLE       0x01
#define LL_UART_INT_EN_TRANSMIT_HOLD_REG_EMPTY      0x02
#define LL_UART_INT_EN_RECEIVER_LINE_STATUS         0x04
#define LL_UART_INT_EN_MODEM_STATUS                 0x08
#define LL_UART_INT_EN_PROGRAMMABLE_THRE            0x80

/**
 * @brief Rx trigger level. Above the trigger level, an uart rx interrupt will trigger. Uart Rx Fifo is 8 bytes.
 * 
 */
#define LL_UART_RCVR_TRIGGER_LVL_HAS_ONE_CHARACTER     0x00
#define LL_UART_RCVR_TRIGGER_LVL_ONE_QUARTER_FULL      0x01
#define LL_UART_RCVR_TRIGGER_LVL_HALF_FULL             0x02
#define LL_UART_RCVR_TRIGGER_LVL_TWO_LESS_THAN_FULL    0x03

/**
 * @brief Tx trigger level.Below the trigger level, an uart tx interrupt will trigger. Uart Tx Fifo is 8 bytes.
 */
#define LL_UART_TX_EMPTY_TRIGGER_LVL_EMPTY                     0x00
#define LL_UART_TX_EMPTY_TRIGGER_LVL_TWO_CHAR_IN_FIFO          0x01
#define LL_UART_TX_EMPTY_TRIGGER_LVL_ONE_QUARTER_FULL          0x02
#define LL_UART_TX_EMPTY_TRIGGER_LVL_HALF_FULL                 0x03

/**
 * @brief Set Uart DMA mode
 */
#define LL_UART_DMA_MODE0   0x00
#define LL_UART_DMA_MODE1   0x01

/**
 * @brief The status of the uart interrupt.
 */
#define LL_UART_INT_ID_MODEM_STATUS             0x00
#define LL_UART_INT_ID_NO_INTERRUPT_PENDING     0x01
#define LL_UART_INT_ID_THR_EMPTY                0x02
#define LL_UART_INT_ID_RECV_DATA_AVAILABLE      0x04
#define LL_UART_INT_ID_RECV_LINE_STATUS         0x06
#define LL_UART_INT_ID_BUSY_DETECT              0x07
#define LL_UART_INT_ID_CHAR_TIMEOUT             0x0C

/**s
 * @brief Data Length Select. The number of bits that may be selected are 5, 6, 7, 8bits
 */
#define LL_UART_DATALEN_5BIT                    0x00
#define LL_UART_DATALEN_6BIT                    0x01
#define LL_UART_DATALEN_7BIT                    0x02
#define LL_UART_DATALEN_8BIT                    0x03

/**
 * @brief Select type of parity check.
 */
#define LL_UART_PARITY_NONE                     0x00
#define LL_UART_PARITY_ODD                      0x01
#define LL_UART_PARITY_EVEN                     0x02

/**
 * @brief Number of stop bits. 
 * If set to stop_bit_1_5_OR_2 and the data bits are set to 5, one and a half stop bits is transmitted. Otherwise, two stop bits are transmitted.
 */
#define LL_UART_STOP_BIT_1                      0x00
//Note:When the UART_DATALEN is set to 5 bits, the stop bit is 1.5 bit;Otherwise,2 stop bits are transmitted. 
#define LL_UART_STOP_BIT_1_5_OR_2               0x01


/**
 * @brief Select mode of flow control. When select hardware flow control, rts is controlled by hardware, otherwise controlled by software.
 */
#define LL_UART_FLOW_CONTROL_SOFTWARE           0x00
#define LL_UART_FLOW_CONTROL_HARDWARE           0x01

/**
 * @brief Bitmap of line status.
 */
#define LL_UART_LSR_DATA_READY                  0x01
#define LL_UART_LSR_OVERRUN_ERR                 0x02
#define LL_UART_LSR_PARITY_ERR                  0x04
#define LL_UART_LSR_FRAMING_ERR                 0x08
#define LL_UART_LSR_BREAK_INT                   0x10
#define LL_UART_LSR_TX_HOLDING_EMPTY            0x20
#define LL_UART_LSR_TX_EMPTY                    0x40
#define LL_UART_LSR_RECV_FIFO_ERR               0x80
#define LL_UART_LSR_ADDR_RECEIVED               0x0100

/**
 * @brief Bitmap of Modem Status
 */
#define LL_UART_MODEM_STATUS_DELTA_CLEAR_TO_SEND            0x01
#define LL_UART_MODEM_STATUS_DELTA_DATA_SET_READY           0x02
#define LL_UART_MODEM_STATUS_TRAILING_EDGE_RING_INDICATOR   0x04
#define LL_UART_MODEM_STATUS_DELTA_DATA_CARRIER_DETECT      0x08
#define LL_UART_MODEM_STATUS_CLEAR_TO_SEND                  0x10
#define LL_UART_MODEM_STATUS_DATA_SET_READY                 0x20
#define LL_UART_MODEM_STATUS_RING_INDICATOR                 0x40
#define LL_UART_MODEM_STATUS_DATA_CARRIER_DETECT            0x80

/**
 * @brief Bitmap of Uart normal Status
 */
#define LL_UART_STATUS_UART_BUSY                   0x01
#define LL_UART_STATUS_TRANSMIT_FIFO_NOT_FULL      0x02
#define LL_UART_STATUS_TRANSMIT_FIFO_EMPTY         0x04
#define LL_UART_STATUS_RECEIVE_FIFO_NOT_EMPTY      0x08
#define LL_UART_STATUS_RECEIVE_FIFO_FULL           0x10

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 * @brief Read one character from register. (not polling.)
 * 
 * @param UARTx 
 * @return uint8_t the received character.
 */
uint8_t LL_UART_ReadChar(UartInstance *UARTx);

/**
 * @brief Send one character to register.
 * 
 * @param UARTx 
 * @param ch The character to be transmitted.
 */
void LL_UART_WriteChar(UartInstance *UARTx, uint8_t ch);

/**
 * @brief Enbale/disable UART module
 * 
 * @param UARTx 
 * @param 1:Enbale   0:disable.
 */

void LL_UART_En(UartInstance *UARTx, uint8_t value);

/**
 * @brief Set Divisor Latch Low
 * 
 * @param UARTx 
 * @param value 
 */
void LL_UART_DLL_Set(UartInstance *UARTx, uint8_t value);

/**
 * @brief Set Divisor Latch High
 * 
 * @param UARTx 
 * @param value 
 */
void LL_UART_DLH_Set(UartInstance *UARTx, uint8_t value);

/**
 * @brief Set Divisor Latch Fraction
 * 
 * @param UARTx 
 * @param value 
 */
void LL_UART_DLF_Set(UartInstance *UARTx, uint8_t value);

/**
 * @brief Enalbe Received Data Available Interrupt and Character Timeout Interrupt.
 * 
 * @param UARTx 
 * @param enable Enable/Disable
 */
void LL_UART_IT_Enable_ReceivedDataAvailable(UartInstance *UARTx, bool enable);

/**
 * @brief Enable Transmit Holding Register Empty Interrupt.
 * 
 * @param UARTx 
 * @param enable 
 */
void LL_UART_IT_Enable_TransmitHoldingRegisterEmpty(UartInstance *UARTx, bool enable);

/**
 * @brief Enable Receiver Line Status Interrupt.
 * 
 * @param UARTx 
 * @param enable 
 */
void LL_UART_IT_Enable_ReceiverLineStatus(UartInstance *UARTx, bool enable);

/**
 * @brief Enable Modem Status Interrupt.
 * 
 * @param UARTx 
 * @param enable 
 */
void LL_UART_IT_Enable_ModemStatus(UartInstance *UARTx, bool enable);

/**
 * @brief Enable THRE Interrupt.
 * 
 * @param UARTx 
 * @param enable 
 */
void LL_UART_IT_Enable_ProgramTransmitHoldingRegisterEmpty(UartInstance *UARTx, bool enable);

/**
 * @brief Enable multiple Interrupts at once.
 * 
 * @param UARTx 
 * @param irq_map ORed bits, @see Uart_It_Enable
 */
void LL_UART_INT_Config(UartInstance *UARTx, uint8_t irq_map);

/**
 * @brief Get Interrupt ID.
 * 
 * @param UARTx 
 * @return Uart_Int_Id 
 */
uint8_t LL_UART_GetInterruptId(UartInstance *UARTx);

/**
 * @brief Is FIFO enabled?
 * 
 * @param UARTx 
 * @return true FIFO enabled.
 * @return false FIFO disabled.
 */
bool LL_UART_IsFIFOEnable(UartInstance *UARTx);

/**
 * @brief FIFO control
 * @param UARTx     [description]
 * @param rxLvl      RCVR Trigger Level
 * @param txLvl      TX Empty Trigger Level
 * @param dma_mode   DMA mode
 */
void LL_UART_FIFO_Control(UartInstance *UARTx, uint8_t rxLvl, uint8_t txLvl, uint8_t dma_mode);

/**
 * @brief Set RX FIFO Triggler Level.
 * 
 * @param UARTx 
 * @param rxLvl 
 */
void LL_UART_FIFO_Set_RX_TrigLvl(UartInstance *UARTx, uint8_t rxLvl);

/**
 * @brief Set TX FIFO Empty Trigger Level.
 * 
 * @param UARTx 
 * @param txLvl
 */
void LL_UART_FIFO_Set_TX_Empty_TrigLvl(UartInstance *UARTx, uint8_t txLvl);

/**
 * Enable/Disable FIFO
 * @param UARTx
 * @param enable
 */
void LL_UART_FIFO_Enable(UartInstance *UARTx, bool enable);

/**
 * @brief Reset RX FIFO.
 * 
 * @param UARTx 
 */
void LL_UART_RX_FIFO_Reset(UartInstance *UARTx);

/**
 * @brief Reset TX FIFO.
 * 
 * @param UARTx 
 */
void LL_UART_TX_FIFO_Reset(UartInstance *UARTx);

/**
 * @brief Divisor Latch Bit set method.
 * 
 * @param UARTx 
 * @param value 
 * @return true set ok.
 * @return false set faild.
 */
bool LL_UART_DLAB_Set(UartInstance *UARTx, uint8_t value);

/**
 * @brief Parity enable/disable
 * 
 * @param UARTx 
 * @param enable 
 */
void LL_UART_Parity_Enable(UartInstance *UARTx, bool enable);

/**
 * @brief UART set Parity check even or odd.
 * 
 * @param UARTx 
 * @param parity 
 */
void LL_UART_Parity_Set_EvenOdd(UartInstance *UARTx, uint8_t parity);

/**
 * @brief UART set stopbits.
 * 
 * @param UARTx 
 * @param stopbit 
 */
void LL_UART_Stopbits_Set(UartInstance *UARTx, uint8_t stopbit);

/**
 * @brief UART set data length bits.
 * 
 * @param UARTx 
 * @param length 
 */
void LL_UART_DataLenth_Set(UartInstance *UARTx, uint8_t length);

/**
 * @brief UART set auto flow control. (only UART0 supports Auto Flow Control)
 * 
 * @param UARTx 
 * @param control 
 */
void LL_UART_FlowControl_Set(UartInstance *UARTx, uint8_t control);

void LL_UART_Finish_Transfer(UartInstance *UARTx);

/**
 * @brief Get Uart Line Status. @see Uart_Line_Status.
 * 
 * @param UARTx 
 * @return @see Uart_Line_Status 
 */
uint32_t LL_UART_GetLineStatus(UartInstance *UARTx);

/**
 * @brief Data ready in Receiver Buffer or RX FIFO.
 * 
 * @param UARTx 
 * @return true  data ready.
 * @return false data not ready.
 */
bool LL_UART_isDataReady(UartInstance *UARTx);

/**
 * @brief Get UART Modem Status
 * 
 * @param UARTx 
 * @return uint8_t @see Uart_Modem_Status, ORed bits.
 */
uint8_t LL_UART_Get_Modem_Status(UartInstance *UARTx);

/**
 * @brief Set UART Low Power Divisor. (on UART0 supports Low Power)
 * 
 * @param UARTx 
 * @param divisor 
 */
void LL_UART_Low_Power_Divisor_Set(UartInstance *UARTx, uint16_t divisor);

/**
 * @brief Get UART Low Power Divisor value. (only UART0 supports Low Power)
 * 
 * @param UARTx 
 * @return uint16_t 
 */
uint16_t LL_UART_Low_Power_Divisor_Get(UartInstance *UARTx);

/**
 * @brief Get UART Status, value is ORed from enum Uart_Status.
 * 
 * @param UARTx 
 * @return uint8_t @see Uart_Status.
 */
uint8_t LL_UART_Normal_Status(UartInstance *UARTx);

/**
 * @brief RX FIFO is full or not.
 * 
 * @param UARTx 
 * @return true  RX FIFO is full.
 * @return false RX FIFO is not full.
 */
bool LL_UART_IsRxFIFOFull(UartInstance *UARTx);

/**
 * @brief RX FIFO is not empty or not.
 * 
 * @param UARTx 
 * @return true RX FIFO is not empty, has at least one character.
 * @return false RX FIFO is empty.
 */
bool LL_UART_IsRxFIFONotEmpty(UartInstance *UARTx);

/**
 * @brief Transmit FIFO is empty or not.
 * 
 * @param UARTx 
 * @return true TX FIFO is empty.
 * @return false TX FIFO is not empty, has at least one character to send.
 */
bool LL_UART_IsTxFIFOEmpty(UartInstance *UARTx);

/**
 * @brief TX FIFO is not full.
 * 
 * @param UARTx 
 * @return true TX FIFO is not full.
 * @return false TX FIFO is full.
 */
bool LL_UART_IsTxFIFONotFull(UartInstance *UARTx);

/**
 * @brief UART is busy or not.
 * 
 * @param UARTx 
 * @return true UART is busy.
 * @return false UART is not busy.
 */
bool LL_UART_IsUARTBusy(UartInstance *UARTx);
/**
 * @brief Get UART RX FIFO Level.
 * 
 * @param UARTx 
 * @return UART RX FIFO Level.
 */

uint8_t LL_UART_GetRxFIFOLevel(UartInstance *UARTx);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __LL_UART_H__ */
