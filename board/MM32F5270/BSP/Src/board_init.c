/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "board_init.h"
#include "hal_gpio.h"
#include "tos_shell.h"

/*
* Definitions.
*/

/*
* Declerations.
*/
void BOARD_InitDebugConsole(void);

/*
* Functions.
*/
void BOARD_Init(void)
{
    BOARD_InitBootClocks();
    BOARD_InitPins();

    BOARD_InitDebugConsole();
}

void BOARD_InitDebugConsole(void)
{
    UART_Init_Type uart_init;

    uart_init.ClockFreqHz   = BOARD_DEBUG_UART_FREQ;
    uart_init.BaudRate      = BOARD_DEBUG_UART_BAUDRATE;
    uart_init.WordLength    = UART_WordLength_8b;
    uart_init.StopBits      = UART_StopBits_1;
    uart_init.Parity        = UART_Parity_None;
    uart_init.XferMode      = UART_XferMode_RxTx;
    uart_init.HwFlowControl = UART_HwFlowControl_None;
    UART_Init(BOARD_DEBUG_UART_PORT, &uart_init);
    UART_EnableInterrupts(BOARD_DEBUG_UART_PORT, UART_ISR_RXINTF_MASK, true);
    UART_Enable(BOARD_DEBUG_UART_PORT, true);

    NVIC_SetPriority(BOARD_DEBUG_UART_IRQ, (1UL << __NVIC_PRIO_BITS) - 2UL);
}

#if defined(__ARMCC_VERSION)
int fputc(int c, FILE *f)
{
    (void)(f);
    while ( 0u == (UART_STATUS_TX_EMPTY & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    UART_PutData(BOARD_DEBUG_UART_PORT, (uint8_t)(c));
    return c;
}

int fgetc(FILE *f)
{
    (void)(f);
    while ( 0u == (UART_STATUS_RX_DONE & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    return UART_GetData(BOARD_DEBUG_UART_PORT);
}

#elif defined(__GNUC__)

/*
 * Called by libc stdio fwrite functions
 */
int _write(int fd, char *ptr, int len)
{
    int i = 0;

    /*
     * write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
    * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2)
    {
        return -1;
    }

    while (*ptr && (i < len))
    {
        while ( 0u == (UART_STATUS_TX_EMPTY & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
        {}
        UART_PutData(BOARD_DEBUG_UART_PORT, (uint8_t)(*ptr));
        i++;
        ptr++;
    }

    return i;
}

/*
 * Called by the libc stdio fread fucntions
 *
 * Implements a buffered read with line editing.
 */
int _read(int fd, char *ptr, int len)
{
    int my_len;

    if (fd > 2)
    {
        return -1;
    }

    my_len = 0;
    while (len > 0)
    {
        while ( 0u == (UART_STATUS_RX_DONE & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
        {}
        *ptr = UART_GetData(BOARD_DEBUG_UART_PORT);
        len--;
        my_len++;

        if ( (*ptr == '\r') || (*ptr == '\n') || (*ptr == '\0') )
        {
            break;
        }

        ptr++;
    }

    return my_len; /* return the length we got */
}


int putchar(int c)
{
    while ( 0u == (UART_STATUS_TX_EMPTY & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    UART_PutData(BOARD_DEBUG_UART_PORT, (uint8_t)(c));
    return c;
}

int getchar(void)
{
    while ( 0u == (UART_STATUS_RX_DONE & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    return UART_GetData(BOARD_DEBUG_UART_PORT);
}

#elif (defined(__ICCARM__))
/* These function __write and __read is used to support IAR toolchain to printf and scanf. */

int fputc(int ch, FILE *f)
{
    while ( 0u == (UART_STATUS_TX_EMPTY & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    UART_PutData(BOARD_DEBUG_UART_PORT, (uint8_t)(ch));
    return ch;
}

int fgetc(FILE *f)
{
    while ( 0u == (UART_STATUS_RX_DONE & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    return UART_GetData(BOARD_DEBUG_UART_PORT);
}

#endif
void UART1_IRQHandler(void)
{
    uint8_t data = 0;
    if (UART_ISR_RXINTF_MASK & UART_GetInterruptStatus(BOARD_DEBUG_UART_PORT))
    {
        data = UART_GetData(BOARD_DEBUG_UART_PORT);
        tos_shell_input_byte(data);
    }
}
/* EOF. */
