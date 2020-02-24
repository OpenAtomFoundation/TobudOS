/*
 * FreeModbus Libary: STR71/lwIP Port serial driver.
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */


/* ----------------------- Platform includes --------------------------------*/
#include "uart.h"
#include "eic.h"
#include "gpio.h"

/* ----------------------- lwIP includes ------------------------------------*/
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sio.h"
#include "lwip/err.h"

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "task.h"
#include "semphr.h"

/* ----------------------- Project includes ---------------------------------*/
#include "serial.h"

/* ----------------------- Defines ------------------------------------------*/
#define UART0_DEV               ( UART0 )
#define UART0_RX_PORT           ( GPIO0 )
#define UART0_RX_PIN            ( 8 )
#define UART0_TX_PORT           ( GPIO0 )
#define UART0_TX_PIN            ( 9 )
#define UART0_IRQ_CH            ( UART0_IRQChannel )
#define UART0_IRQ_PRIORITY      ( 1 )

#define UART1_DEV               ( UART1 )
#define UART1_RX_PORT           ( GPIO0 )
#define UART1_RX_PIN            ( 10 )
#define UART1_TX_PORT           ( GPIO0 )
#define UART1_TX_PIN            ( 11 )
#define UART1_IRQ_CH            ( UART1_IRQChannel )
#define UART1_IRQ_PRIORITY      ( 1 )

#define UART_DEVICES_MAX        ( 2 )

#define DEFAULT_BAUDRATE        ( 38400 )
#define DEFAULT_DATABITS        ( 8 )
#define DEFAULT_STOPBITS        ( 1 )
#define DEFAULT_PARITY          ( SIO_PAR_NONE )

#define DEFAULT_TX_BUFSIZE      ( 64 )
#define DEFAULT_RX_BUFSIZE      ( 64 )

#define DEFAULT_READTIMEOUT_MS  ( 10 )

#define SIO_RESET_STATE( dev )    do { \
        ( dev )->ready = 0; \
        ( dev )->abort = 0; \
        ( dev )->UARTx = NULL; \
        ( dev )->rx_buf_rdpos = ( dev )->rx_buf_wrpos = 0; \
        ( dev )->rx_sem = SYS_SEM_NULL; \
        ( dev )->tx_buf_rdpos = ( dev )->tx_buf_wrpos = 0; \
        ( dev )->tx_sem = SYS_SEM_NULL; \
    } while( 0 )

#define MS_TO_TICKS( ms )           \
    ( portTickType )( ( portTickType ) ( ms ) / portTICK_RATE_MS )

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    u8_t            ready;
    u8_t            abort;

    u8_t            rx_buf[DEFAULT_RX_BUFSIZE];
    u8_t            rx_buf_rdpos;
    u8_t            rx_buf_wrpos;
    u8_t            rx_buf_cnt;
    xSemaphoreHandle rx_sem;

    u8_t            tx_buf[DEFAULT_TX_BUFSIZE];
    u8_t            tx_buf_rdpos;
    u8_t            tx_buf_wrpos;
    u8_t            tx_buf_cnt;
    xSemaphoreHandle tx_sem;
    UART_TypeDef   *UARTx;
} serdev_t;

/* ----------------------- Prototypes ---------------------------------------*/
void            sio_uart0_irq( void ) __attribute__ ( ( naked ) );
void            sio_uart1_irq( void ) __attribute__ ( ( naked ) );

/* ----------------------- Static functions ---------------------------------*/

static err_t    sio_open_low_level( u8_t devnr, serdev_t * dev );
static err_t    sio_close_low_level( u8_t devnr, serdev_t * dev );

/* ----------------------- Static variables ---------------------------------*/
static u8_t     initialized = FALSE;
static volatile serdev_t devices[UART_DEVICES_MAX];

/* ----------------------- Start implementation -----------------------------*/

err_t
sio_open_low_level( u8_t devnr, serdev_t * dev )
{
    err_t           error = ERR_OK;

    if( devnr == 0 )
    {
        /* Return value. */
        dev->UARTx = UART0;

        /* Reset the UART. */
        UART_Init( dev->UARTx );

        /* Configure the GPIO pints for the UART device. */
        GPIO_Config( UART0_TX_PORT, 1 << UART0_TX_PIN, GPIO_AF_PP );
        GPIO_Config( UART0_RX_PORT, 1 << UART0_RX_PIN, GPIO_IN_TRI_CMOS );

        /* Configure the IEC for the UART interrupts. */
        EIC_IRQChannelPriorityConfig( UART0_IRQ_CH, UART0_IRQ_PRIORITY );
        EIC_IRQChannelConfig( UART0_IRQ_CH, ENABLE );
    }
    else if( devnr == 1 )
    {
        /* Return value. */
        dev->UARTx = UART1;

        /* Reset the UART. */
        UART_Init( dev->UARTx );

        /* Configure the GPIO pints for the UART device. */
        GPIO_Config( UART1_TX_PORT, 1 << UART1_TX_PIN, GPIO_AF_PP );
        GPIO_Config( UART1_RX_PORT, 1 << UART1_RX_PIN, GPIO_IN_TRI_TTL );

        /* Configure the EIC for the UART interrupts. */
        EIC_IRQChannelPriorityConfig( UART1_IRQ_CH, UART1_IRQ_PRIORITY );
        EIC_IRQChannelConfig( UART1_IRQ_CH, ENABLE );
    }
    else
    {
        error = ERR_IF;
    }
    return error;
}

err_t
sio_close_low_level( u8_t devnr, serdev_t * dev )
{
    err_t           error = ERR_OK;

    if( devnr == 0 )
    {
        UART_Init( dev->UARTx );

        /* Disable the GPIO pints for the UART device. */
        GPIO_Config( UART0_TX_PORT, 1 << UART1_TX_PIN, GPIO_IN_TRI_TTL );
        GPIO_Config( UART0_RX_PORT, 1 << UART1_RX_PIN, GPIO_IN_TRI_TTL );

        /* Disable the UART interrupts in the EIC. */
        EIC_IRQChannelConfig( UART0_IRQ_CH, DISABLE );
    }
    else if( devnr == 1 )
    {
        UART_Init( dev->UARTx );

        /* Disable the GPIO pints for the UART device. */
        GPIO_Config( UART1_TX_PORT, 1 << UART1_TX_PIN, GPIO_IN_TRI_TTL );
        GPIO_Config( UART1_RX_PORT, 1 << UART1_RX_PIN, GPIO_IN_TRI_TTL );

        /* Disable the UART interrupts in the EIC. */
        EIC_IRQChannelConfig( UART1_IRQ_CH, DISABLE );
    }
    else
    {
        error = ERR_IF;
    }
    return error;
}

err_t
sio_close( serdev_t * dev )
{
    int             i;
    err_t           error = ERR_VAL;

    for( i = 0; i < UART_DEVICES_MAX; i++ )
    {
        if( &devices[i] == dev )
        {
            break;
        }
    }
    if( i < UART_DEVICES_MAX )
    {
        vPortEnterCritical(  );
        error = sio_close_low_level( i, dev );
        vPortExitCritical(  );

        if( dev->tx_sem != ( xSemaphoreHandle ) 0 )
        {
            vQueueDelete( dev->tx_sem );
        }
        if( dev->rx_sem != ( xSemaphoreHandle ) 0 )
        {
            vQueueDelete( dev->tx_sem );
        }
        SIO_RESET_STATE( dev );
    }

    return error;
}

sio_fd_t
sio_open_new( u8_t devnr, u32_t baudrate, u8_t databits, sio_stop_t stopbits, sio_parity_t parity )
{
    int             i;
    err_t           error = ERR_OK;
    serdev_t       *dev;
    UARTParity_TypeDef eUARTParity = UART_NO_PARITY;
    UARTMode_TypeDef eUARTMode = UARTM_8D;
    UARTStopBits_TypeDef eUARTStopBits;

    if( !initialized )
    {
        for( i = 0; i < UART_DEVICES_MAX; i++ )
        {
            SIO_RESET_STATE( &devices[i] );
        }
        initialized = 1;
    }

    /* Check if devicename is valid and not in use. */
    if( ( devnr < UART_DEVICES_MAX ) && ( devices[devnr].ready == 0 ) )
    {
        dev = ( serdev_t * ) & devices[devnr];

        switch ( parity )
        {
        case SIO_PAR_EVEN:
            eUARTParity = UART_EVEN_PARITY;
            break;
        case SIO_PAR_ODD:
            eUARTParity = UART_ODD_PARITY;
            break;
        case SIO_PAR_NONE:
            eUARTParity = UART_NO_PARITY;
            break;
        default:
            error = ERR_VAL;
        }

        switch ( databits )
        {
        case 7:
            if( parity != SIO_PAR_NONE )
            {
                eUARTMode = UARTM_7D_P;
            }
            break;
        case 8:
            eUARTMode = parity == SIO_PAR_NONE ? UARTM_8D : UARTM_8D_P;
            break;
        default:
            error = ERR_VAL;
        }

        switch ( stopbits )
        {
        case SIO_STOP_0_5:
            eUARTStopBits = UART_0_5_StopBits;
            break;
        case SIO_STOP_1:
            eUARTStopBits = UART_1_StopBits;
            break;
        case SIO_STOP_1_5:
            eUARTStopBits = UART_1_5_StopBits;
            break;
        case SIO_STOP_2:
            eUARTStopBits = UART_2_StopBits;
            break;
        default:
            error = ERR_VAL;
        }

        if( error == ERR_OK )
        {
            SIO_RESET_STATE( dev );

            vSemaphoreCreateBinary( dev->rx_sem );
            vSemaphoreCreateBinary( dev->tx_sem );

            vPortEnterCritical(  );
            if( ( error = sio_open_low_level( devnr, dev ) ) != ERR_OK )
            {
                /* Hardware interface does not exist. */
            }
            else if( dev->tx_sem == ( xSemaphoreHandle ) 0 )
            {
                error = ERR_MEM;
            }
            else if( dev->rx_sem == ( xSemaphoreHandle ) 0 )
            {
                error = ERR_MEM;
            }
            else
            {
                /* UART parameter correct and hardware device available. */
                UART_OnOffConfig( dev->UARTx, ENABLE );
                UART_FifoConfig( dev->UARTx, ENABLE );
                UART_FifoReset( dev->UARTx, UART_RxFIFO );
                UART_FifoReset( dev->UARTx, UART_TxFIFO );
                UART_LoopBackConfig( dev->UARTx, DISABLE );
                UART_Config( dev->UARTx, baudrate, eUARTParity, eUARTStopBits, eUARTMode );
                UART_TimeOutPeriodConfig( dev->UARTx, 0xFF );
                UART_ItConfig( dev->UARTx, UART_RxBufFull, ENABLE );
                UART_RxConfig( dev->UARTx, ENABLE );

                /* Device is now ready for use. */
                dev->ready = 1;
            }

            if( error != ERR_OK )
            {
                sio_close( dev );
            }
            vPortExitCritical(  );
        }
    }
    else
    {
        error = ERR_VAL;
    }
    return error == ERR_OK ? ( void * )dev : SIO_FD_NULL;
}

sio_fd_t
sio_open( u8_t devnr )
{
    return sio_open_new( devnr, DEFAULT_BAUDRATE, DEFAULT_DATABITS,
                         DEFAULT_STOPBITS, DEFAULT_PARITY );
}

void
sio_send_noisr( u8_t data, sio_fd_t fd )
{
    serdev_t       *dev = fd;

    if( dev->ready )
    {
        UART_ByteSend( dev->UARTx, &data );
    }
}

u32_t
sio_write_noisr( sio_fd_t fd, u8_t * buf, u32_t size )
{
    u32_t           left = size;
    u8_t            send;
    serdev_t       *dev = fd;

    if( dev->ready )
    {
        left = size;
        while( left > 0 )
        {
            send = size % 256;
            UART_DataSend( dev->UARTx, ( u8 * ) buf, ( u8 ) send );
            left -= send;
        }
    }
    return size - left;
}

void
sio_send( u8_t data, sio_fd_t fd )
{
    while( sio_write( fd, &data, 1 ) != 1 );
}

u8_t
sio_recv( sio_fd_t fd )
{
    u8_t            data;
    serdev_t       *dev = fd;

    if( dev->ready )
    {
        while( sio_read( fd, &data, 1 ) != 1 );
    }
    else
    {
        LWIP_ASSERT( "sio_recv: dev->ready != 0 ", dev->ready != 0 );
        data = '\0';
    }
    return data;
}

u32_t
sio_read( sio_fd_t fd, u8_t * buf, u32_t size )
{
    u32_t           ch_left = size;
    u32_t           ch_received = 0;
    volatile serdev_t *dev = fd;

    if( dev->ready )
    {
        dev->abort = 0;
        while( ch_left && !dev->abort )
        {
            vPortEnterCritical(  );
            while( ( dev->rx_buf_cnt > 0 ) && ( ch_left > 0 ) )
            {
                /* Fetch character from the ring buffer. */
                *buf++ = dev->rx_buf[dev->rx_buf_rdpos];
                dev->rx_buf_rdpos = ( dev->rx_buf_rdpos + 1 ) % DEFAULT_RX_BUFSIZE;
                dev->rx_buf_cnt--;
                /* Count character received and left for read. */
                ch_left--;
                ch_received++;
            }
            vPortExitCritical(  );
            /* If we want more data block on the semaphore and wait until
             * something happens.
             */
            if( ch_left )
            {
                if( xSemaphoreTake( dev->rx_sem, MS_TO_TICKS( DEFAULT_READTIMEOUT_MS ) ) ==
                    pdFALSE )
                {
                    /* A timeout. Abort the read and return the characters 
                     * received so far. 
                     */
                    dev->abort = 1;
                }
            }
        }
    }
    return ch_received;
}

u32_t
sio_write( sio_fd_t fd, u8_t * buf, u32_t size )
{
    u32_t           ch_left;

    volatile serdev_t *dev = fd;

    if( dev->ready )
    {
        ch_left = size;
        while( ch_left > 0 )
        {
            vPortEnterCritical(  );
            while( ( dev->tx_buf_cnt < DEFAULT_TX_BUFSIZE ) && ( ch_left > 0 ) )
            {
                dev->tx_buf[dev->tx_buf_wrpos] = *buf++;
                dev->tx_buf_wrpos = ( dev->tx_buf_wrpos + 1 ) % DEFAULT_TX_BUFSIZE;
                dev->tx_buf_cnt++;
                ch_left--;
            }
            /* Enable transmit FIFO empty interrupts and block. */
            UART_ItConfig( dev->UARTx, UART_TxHalfEmpty, ENABLE );
            vPortExitCritical(  );

            /* Not all characters sent within one write. Block on a semaphore
             * which is triggered when the buffer is empty again.
             */
            if( ch_left != 0 )
            {
                while( xSemaphoreTake( dev->tx_sem, portMAX_DELAY ) != pdTRUE );
            }
        }
    }
    return size;
}

void
sio_read_abort( sio_fd_t fd )
{
    volatile serdev_t *dev = fd;

    dev->abort = 1;
}

void
sio_serial_isr( UART_TypeDef * UARTx, u8_t * need_ctx_switch )
{
    int             i;
    u16             status;
    volatile serdev_t *dev = SIO_FD_NULL;

    portBASE_TYPE   rx_woken = pdFALSE;
    portBASE_TYPE   tx_woken = pdFALSE;

    for( i = 0; i < UART_DEVICES_MAX; i++ )
    {
        if( devices[i].ready && ( devices[i].UARTx == UARTx ) )
        {
            dev = &devices[i];
            break;
        }
    }
    if( dev != SIO_FD_NULL )
    {
        status = UART_FlagStatus( dev->UARTx );

        /* If there are characters in the UART fifo place them into the 
         * ring buffer. In case the buffer is filled half or the requested
         * number of bytes has been read wakeup the receiver.
         */
        if( status & UART_RxBufFull )
        {
            do
            {
                /* Store the character in the ring buffer and advance write
                 * position. */
                dev->rx_buf[dev->rx_buf_wrpos] = dev->UARTx->RxBUFR;
                dev->rx_buf_wrpos = ( dev->rx_buf_wrpos + 1 ) % DEFAULT_RX_BUFSIZE;

                /* Increment the receiver buffer counter. Check for a buffer
                 * overrun. In that case we have overwritten a old character.
                 * Therefore we have to advance the read position. Note that
                 * in this error case we must not increment the read counter
                 * because an old character was lost.
                 */
                if( dev->rx_buf_cnt >= DEFAULT_RX_BUFSIZE )
                {
                    /* LWIP_ASSERT( "sio_serial_isr: receiver buffer overflow", 0 ); */
                    dev->rx_buf_rdpos = ( dev->rx_buf_rdpos + 1 ) % DEFAULT_RX_BUFSIZE;
                }
                else
                {
                    dev->rx_buf_cnt++;
                }

                /* Get the new status from the UART. */
                status = UART_FlagStatus( dev->UARTx );
            }
            while( status & UART_RxBufFull );

            /* Wakeup receiver if buffer is starting to fill. */
            if( dev->rx_buf_cnt > ( DEFAULT_RX_BUFSIZE / 2 ) )
            {
                rx_woken = xSemaphoreGiveFromISR( dev->rx_sem, rx_woken );
            }
        }

        /* Check if we must send characters. */
        if( ( dev->tx_buf_cnt > 0 ) && ( status & UART_TxHalfEmpty ) )
        {
            do
            {
                /* Fetch character from the ring buffer and place them into
                 * the FIFO. */
                dev->UARTx->TxBUFR = dev->tx_buf[dev->tx_buf_rdpos];
                dev->tx_buf_rdpos = ( dev->tx_buf_rdpos + 1 ) % DEFAULT_TX_BUFSIZE;
                dev->tx_buf_cnt--;

                /* Get the new status from the UART. */
                status = UART_FlagStatus( dev->UARTx );
            }
            while( ( dev->tx_buf_cnt > 0 ) && ( status & UART_TxHalfEmpty ) );

            if( dev->tx_buf_cnt == 0 )
            {
                tx_woken = xSemaphoreGiveFromISR( dev->tx_sem, tx_woken );
                UART_ItConfig( dev->UARTx, UART_TxHalfEmpty, DISABLE );
            }
        }

        if( tx_woken || rx_woken )
        {
            *need_ctx_switch = 1;
        }
    }
}


void
sio_uart0_irq( void )
{
    /* Save context to stack. */
    portENTER_SWITCHING_ISR(  );

    static u8_t     need_ctx_switch;

    sio_serial_isr( UART0, &need_ctx_switch );

    /* End the interrupt in the EIC. */
    EIC->IPR |= 1 << EIC_CurrentIRQChannelValue(  );

    /* End the ISR. */
    portEXIT_SWITCHING_ISR( need_ctx_switch ? pdTRUE : pdFALSE );
}

void
sio_uart1_irq( void )
{
    /* Save context to stack. */
    portENTER_SWITCHING_ISR(  );

    static u8_t     need_ctx_switch;

    sio_serial_isr( UART1, &need_ctx_switch );

    /* End the interrupt in the EIC. */
    EIC->IPR |= 1 << EIC_CurrentIRQChannelValue(  );

    /* End the ISR. */
    portEXIT_SWITCHING_ISR( need_ctx_switch ? pdTRUE : pdFALSE );
}
