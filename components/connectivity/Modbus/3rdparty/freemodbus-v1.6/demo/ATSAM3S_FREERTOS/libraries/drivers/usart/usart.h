/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 *
 * \par Purpose
 *
 * This module provides several definitions and methods for using an USART
 * peripheral.
 *
 * \par Usage
 *
 * -# Enable the USART peripheral clock in the PMC.
 * -# Enable the required USART PIOs (see pio.h).
 * -# Configure the UART by calling USART_Configure.
 * -# Enable the transmitter and/or the receiver of the USART using
 *    USART_SetTransmitterEnabled and USART_SetReceiverEnabled.
 * -# Send data through the USART using the USART_Write and
 *    USART_WriteBuffer methods.
 * -# Receive data from the USART using the USART_Read and
 *    USART_ReadBuffer functions; the availability of data can be polled
 *    with USART_IsDataAvailable.
 * -# Disable the transmitter and/or the receiver of the USART with
 *    USART_SetTransmitterEnabled and USART_SetReceiverEnabled.
 */

#ifndef USART_H
#define USART_H

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include <board.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *------------------------------------------------------------------------------*/

/** \section USART_mode USART modes
 * This section lists several common operating modes for an USART peripheral.
 *
 * \b Modes
 * - USART_MODE_ASYNCHRONOUS
 * - USART_MODE_IRDA
 */

/** Basic asynchronous mode, i.e. 8 bits no parity.*/
#define USART_MODE_ASYNCHRONOUS        (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE)

/** IRDA mode*/
#define USART_MODE_IRDA                (AT91C_US_USMODE_IRDA | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_FILTER)


/*------------------------------------------------------------------------------*/
/*         Exported functions*/
/*------------------------------------------------------------------------------*/

extern void USART_Configure(
    Usart *usart,
    uint32_t mode,
    uint32_t baudrate,
    uint32_t masterClock);
extern uint32_t USART_GetStatus(Usart *usart);
extern void USART_EnableIt(Usart *usart,uint32_t mode);
extern void USART_DisableIt(Usart *usart,uint32_t mode);
extern void USART_SetTransmitterEnabled(Usart *usart, uint8_t enabled);

extern void USART_SetReceiverEnabled(Usart *usart, uint8_t enabled);

extern void USART_Write(
    Usart *usart,
    uint16_t data,
    volatile uint32_t timeOut);

extern uint8_t USART_WriteBuffer(
    Usart *usart,
    void *buffer,
    uint32_t size);

extern uint16_t USART_Read(
    Usart *usart,
    volatile uint32_t timeOut);

extern uint8_t USART_ReadBuffer(
    Usart *usart,
    void *buffer,
    uint32_t size);

extern uint8_t USART_IsDataAvailable(Usart *usart);

extern void USART_SetIrdaFilter(Usart *pUsart, uint8_t filter);

extern void USART_PutChar(Usart *usart, uint8_t c);

extern uint32_t USART_IsRxReady(Usart *usart);

extern uint8_t USART_GetChar(Usart *usart);


#endif /*#ifndef USART_H*/

