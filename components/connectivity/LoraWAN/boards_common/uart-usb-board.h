/*!
 * \file      uart-usb-board.h
 *
 * \brief     Target board UART over USB driver implementation
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
#ifndef __UART_USB_BOARD_H__
#define __UART_USB_BOARD_H__

#include <stdint.h>
#include "uart.h"

/*!
 * \brief Initializes the UART object and MCU peripheral
 *
 * \param [IN] obj  UART object
 * \param [IN] tx   UART Tx pin name to be used
 * \param [IN] rx   UART Rx pin name to be used
 */
void UartUsbInit( Uart_t *obj, UartId_t uartId, PinNames tx, PinNames rx );

/*!
 * \brief Initializes the UART object and USB peripheral
 *
 * \param [IN] obj          UART object
 * \param [IN] mode         Mode of operation for the UART
 * \param [IN] baudrate     UART baudrate
 * \param [IN] wordLength   packet length
 * \param [IN] stopBits     stop bits setup
 * \param [IN] parity       packet parity
 * \param [IN] flowCtrl     UART flow control
 */
void UartUsbConfig( Uart_t *obj, UartMode_t mode, uint32_t baudrate, WordLength_t wordLength, StopBits_t stopBits, Parity_t parity, FlowCtrl_t flowCtrl );

/*!
 * \brief DeInitializes the UART object and USB peripheral
 *
 * \param [IN] obj  UART object
 */
void UartUsbDeInit( Uart_t *obj );

/*!
 * \brief Checks if the cable is connected or not
 *
 * \retval connected [0: Not connected, 1: Connected]
 */
uint8_t UartUsbIsUsbCableConnected( void );

/*!
 * \brief Sends a buffer to the UART
 *
 * \param [IN] obj    UART object
 * \param [IN] buffer Buffer to be sent
 * \param [IN] size   Buffer size
 * \retval status     [0: OK, 1: Busy, 2: Fail]
 */
uint8_t UartUsbPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );

/*!
 * \brief Sends a character to the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Character to be sent
 * \retval status    [0: OK, 1: Busy, 2: Fail]
 */
uint8_t UartUsbPutChar( Uart_t *obj, uint8_t data );

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Received character
 * \retval status    [0: OK, 1: Busy, 2: Fail]
 */
uint8_t UartUsbGetChar( Uart_t *obj, uint8_t *data );

#endif // __UART_USB_BOARD_H__
