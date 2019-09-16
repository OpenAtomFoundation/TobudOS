/*!
 * \file      uart-board.h
 *
 * \brief     Target board UART driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __UART_BOARD_H__
#define __UART_BOARD_H__

#include <stdint.h>
#include "uart.h"

/*!
 * \brief Initializes the UART object and MCU peripheral
 *
 * \param [IN] obj  UART object
 * \param [IN] tx   UART Tx pin name to be used
 * \param [IN] rx   UART Rx pin name to be used
 */
void UartMcuInit( Uart_t *obj, UartId_t uartId, PinNames tx, PinNames rx );

/*!
 * \brief Configures the UART object and MCU peripheral
 *
 * \remark UartInit function must be called first.
 *
 * \param [IN] obj          UART object
 * \param [IN] mode         Mode of operation for the UART
 * \param [IN] baudrate     UART baudrate
 * \param [IN] wordLength   packet length
 * \param [IN] stopBits     stop bits setup
 * \param [IN] parity       packet parity
 * \param [IN] flowCtrl     UART flow control
 */
void UartMcuConfig( Uart_t *obj, UartMode_t mode, uint32_t baudrate, WordLength_t wordLength, StopBits_t stopBits, Parity_t parity, FlowCtrl_t flowCtrl );

/*!
 * \brief DeInitializes the UART object and MCU pins
 *
 * \param [IN] obj  UART object
 */
void UartMcuDeInit( Uart_t *obj );

/*!
 * \brief Sends a character to the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Character to be sent
 * \retval status    [0: OK, 1: Busy]
 */
uint8_t UartMcuPutChar( Uart_t *obj, uint8_t data );

/*!
 * \brief Sends a buffer to the UART
 *
 * \param [IN] obj    UART object
 * \param [IN] buffer Buffer to be sent
 * \param [IN] size   Buffer size
 * \retval status     [0: OK, 1: Busy]
 */
uint8_t UartMcuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Received character
 * \retval status    [0: OK, 1: Busy]
 */
uint8_t UartMcuGetChar( Uart_t *obj, uint8_t *data );

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj          UART object
 * \param [IN] buffer       Received buffer
 * \param [IN] size         Number of bytes to be received
 * \param [OUT] nbReadBytes Number of bytes really read
 * \retval status           [0: OK, 1: Busy]
 */
uint8_t UartMcuGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes );

#endif // __UART_BOARD_H__
