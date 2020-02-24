/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
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
 * Implements UART console.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <pio/pio.h>
#include <pmc/pmc.h>

#include "usart/uart_console.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** Console baudrate always using 115200. */
#define CONSOLE_BAUDRATE    115200
/** Usart Hw interface used by the console (UART0). */
#define CONSOLE_USART       UART0
/** Usart Hw ID used by the console (UART0). */
#define CONSOLE_ID          ID_UART0
/** Pins description corresponding to Rxd,Txd, (UART pins) */
#define CONSOLE_PINS        {PINS_UART}

/*----------------------------------------------------------------------------
 *        Variables
 *----------------------------------------------------------------------------*/

/** Is Console Initialized. */
static uint8_t isConsolInitialized = 0;

/**
 * \brief Configures an USART peripheral with the specified parameters.
 *
 * \param baudrate  Baudrate at which the USART should operate (in Hz).
 * \param masterClock  Frequency of the system master clock (in Hz).
 */
static void UART_Configure( uint32_t baudrate,
                            uint32_t masterClock)
{
    const Pin pPins[] = CONSOLE_PINS;
    Uart *pUart = CONSOLE_USART;

    /* Configure PIO */
    PIO_Configure(pPins, PIO_LISTSIZE(pPins));

    /* Configure PMC */
    PMC->PMC_PCER0 = 1 << CONSOLE_ID;

    /* Reset and disable receiver & transmitter */
    pUart->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX
                   | UART_CR_RXDIS | UART_CR_TXDIS;

    /* Configure mode */
    pUart->UART_MR =  (0x4 <<  9); /* TODO: Change with new symbols: AT91C_UART_CHRL_8_BITS | AT91C_UART_PAR_NONE */

    /* Configure baudrate */
    /* Asynchronous, no oversampling */
    pUart->UART_BRGR = (masterClock / baudrate) / 16;

    /* Disable PDC channel */
    pUart->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

    /* Enable receiver and transmitter */
    pUart->UART_CR = UART_CR_RXEN | UART_CR_TXEN;

    isConsolInitialized = 1;
}

/**
 * \brief Outputs a character on the UART line.
 *
 * \note This function is synchronous (i.e. uses polling).
 * \param c  Character to send.
 */
void UART_PutChar(uint8_t c)
{
    Uart *pUart = CONSOLE_USART;

    if (!isConsolInitialized)
        UART_Configure(CONSOLE_BAUDRATE, BOARD_MCK);

    /* Wait for the transmitter to be ready */
    while ((pUart->UART_SR & UART_SR_TXEMPTY) == 0);

    /* Send character */
    pUart->UART_THR = c;

}

/**
 * \brief Input a character from the UART line.
 *
 * \note This function is synchronous
 * \return character received.
 */
uint8_t UART_GetChar(void)
{
    Uart *pUart = CONSOLE_USART;

    if (!isConsolInitialized)
        UART_Configure(CONSOLE_BAUDRATE, BOARD_MCK);

    while((pUart->UART_SR & UART_SR_RXRDY) == 0);
    return pUart->UART_RHR;
}

/**
 * \brief Check if there is Input from UART line.
 *
 * \return true if there is Input.
 */
bool UART_IsRxReady(void)
{
    Uart *pUart = CONSOLE_USART;

    if (!isConsolInitialized)
        UART_Configure(CONSOLE_BAUDRATE, BOARD_MCK);

    return (pUart->UART_SR & UART_SR_RXRDY) > 0;
}

/* \cond NOFPUT */
#ifndef NOFPUT
#include <stdio.h>
#include <stdarg.h>
/**
 * \brief Implementation of fputc using the UART as the standard output. Required
 * for printf().
 *
 * \param c  Character to write.
 * \param pStream  Output stream.
 *
 * \return The character written if successful, or -1 if the output stream is
 * not stdout or stderr.
 */
int fputc ( int c, FILE * pStream )
{
    if ((pStream == stdout) || (pStream == stderr)) {
        UART_PutChar(c);
        return c;
    }
    else {

        return EOF;
    }
}

/**
 * \brief Implementation of fputs using the UART as the standard output. Required
 * for printf(). Does NOT currently use the PDC.
 *
 * \param pStr  String to write.
 * \param pStream  Output stream.
 *
 * \return Number of characters written if successful, or -1 if the output
 * stream is not stdout or stderr.
 */
signed int fputs(const char *pStr, FILE *pStream)
{
    signed int num = 0;

    while (*pStr != 0) {

        if (fputc(*pStr, pStream) == -1) {

            return -1;
        }
        num++;
        pStr++;
    }

    return num;
}

#undef putchar

/**
 * \brief Outputs a character on the UART.
 *
 * \param c  Character to output.
 *
 * \return The character that was output.
 */
signed int putchar(signed int c)
{
    return fputc(c, stdout);
}

/* The char which has been ungotten, or EOF. */
static int _UngetChar = EOF;

/**
 * \brief Implementation of fputc using the UART as the standard output. Required
 * for printf().
 *
 * \param c  Character to write.
 * \param pStream  Output stream.
 *
 * \return The character written if successful, or -1 if the output stream is
 * not stdout or stderr.
 */
int fgetc (FILE * pStream )
{
    int c;

    if (pStream == stdin) {
        c = UART_GetChar();
        return c;
    }
    else {

        return EOF;
    }
}

#undef getchar
/**
 * \brief Get a character from UART.
 *
 * \return The character received.
 */
signed int getchar(void)
{
    if (_UngetChar != EOF)
    {
        int res = _UngetChar;
        _UngetChar = EOF;
        return res;
    }
    else
    {
        return fgetc(stdin);
    }
}

#undef __ungetchar
/**
 * \brief Unget a character from stdin.
 *
 * \return The unget character.
 */
signed int __ungetchar(int c)
{
    _UngetChar = c;
    return c;
}

#endif /* #ifndef NOFPUT */
/* \endcond */
