/*
 * FreeModbus Libary: STR71XTCP Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>

/* ----------------------- lwIP includes ------------------------------------*/
#include "lwip/opt.h"
#include "lwip/sio.h"
#include "lwip/sys.h"
#include "lwip/inet.h"
#include "ppp/ppp.h"
#include "arch/cc.h"

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ----------------------- Platform includes --------------------------------*/
#include "eic.h"
#include "netif/serial.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"

/* ----------------------- Defines ------------------------------------------*/
#define mainMB_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define REG_INPUT_START         1000
#define REG_INPUT_NREGS         4
#define REG_HOLDING_START       2000
#define REG_HOLDING_NREGS       130

#define PPP_AUTH_ENABLED        1
#define PPP_USER                "freemodbus"
#define PPP_PASS                "insecure"

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    CONNECTING, CONNECTED, DISCONNECT
} ePPPThreadControl;

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];
static ePPPThreadControl ePPPThrCtl;


/* ----------------------- Static functions ---------------------------------*/
static void     vlwIPInit( void );
static void     vMBServerTask( void *arg );
static void     vPPPStatusCB( void *ctx, int errCode, void *arg );

sio_fd_t        stdio_fd;
sio_fd_t        ppp_fd;

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    EIC_Init(  );
    EIC_IRQConfig( ENABLE );

    /* Use UART0 as stdin/stdout for debug purposes. */
    if( ( stdio_fd = sio_open_new( 0, 115200, 8, SIO_STOP_1, SIO_PAR_NONE ) ) == SIO_FD_NULL )
    {
        /* nothing we can do here - no stdout means no logging. */
    }
    else
    {
        /* Initialize lwIP and its interface layer. */
        vlwIPInit(  );

        /* Use UART1 as PPP device. */
        if( ( ppp_fd = sio_open_new( 1, 115200, 8, SIO_STOP_1, SIO_PAR_NONE ) ) == SIO_FD_NULL )
        {
            vMBPortLog( MB_LOG_ERROR, "PPP", "can't open PPP device!\r\n" );
        }
        else if( sys_thread_new( vMBServerTask, NULL, mainMB_TASK_PRIORITY ) == SYS_THREAD_NULL )
        {
            vMBPortLog( MB_LOG_ERROR, "MB-INIT", "can't start modbus task!\r\n" );
        }
        else
        {
            vMBPortLog( MB_LOG_INFO, "MB-INIT", "FreeModbus demo application starting...\r\n" );
            /* Everything ready. Start the scheduler. */
            vTaskStartScheduler(  );
        }
    }

    for( ;; );
}

void
vlwIPInit( void )
{
    sys_init(  );
    mem_init(  );
    memp_init(  );
    pbuf_init(  );
    netif_init(  );
    ip_init(  );
    tcpip_init( NULL, NULL );
}

void
vMBServerTask( void *arg )
{
    eMBErrorCode    xStatus;
    ePPPThreadControl ePPPThrCtlCur;
    int             ppp_con_fd;
    portTickType    xTicks;

    pppInit(  );
    if( PPP_AUTH_ENABLED )
    {
      pppSetAuth( PPPAUTHTYPE_PAP, PPP_USER, PPP_PASS );
    }
    else
    {
      pppSetAuth( PPPAUTHTYPE_NONE, NULL, NULL );
    }
    do
    {
        vPortEnterCritical(  );
        ePPPThrCtl = CONNECTING;
        vPortExitCritical(  );
        if( ( ppp_con_fd = pppOpen( ppp_fd, vPPPStatusCB, NULL ) ) == PPPERR_NONE )
        {
            /* Check every 50ms if the state of the connecton has changed.
             * This could either mean it was aborted or successful.
             */
            do
            {
                vTaskDelay( ( portTickType ) ( 50UL / portTICK_RATE_MS ) );
                vPortEnterCritical(  );
                ePPPThrCtlCur = ePPPThrCtl;
                vPortExitCritical(  );
            }
            while( ePPPThrCtlCur == CONNECTING );

            if( ePPPThrCtlCur == CONNECTED )
            {
                if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
                {
                    vMBPortLog( MB_LOG_ERROR, "PPP", "can't initalize modbus stack!\r\n" );
                }
                else if( eMBEnable(  ) != MB_ENOERR )
                {
                    vMBPortLog( MB_LOG_ERROR, "PPP", "can't enable modbus stack!\r\n" );
                }
                else
                {
                    do
                    {
                        vPortEnterCritical(  );
                        ePPPThrCtlCur = ePPPThrCtl;
                        vPortExitCritical(  );

                        /* Application code here. */
                        xStatus = eMBPoll(  );

                        /* Update input registers with the current system  tick. */
                        xTicks = xTaskGetTickCount(  );

                        /* Note: little endian stuff */
                        usRegInputBuf[0] = ( USHORT ) ( xTicks );
                        usRegInputBuf[1] = ( USHORT ) ( xTicks >> 16UL );

                    }
                    while( ( xStatus == MB_ENOERR ) && ( ePPPThrCtlCur == CONNECTED ) );

                    ( void )eMBDisable(  );
                    ( void )eMBClose(  );
                }
            }
            /* FIXME: pppClose bugs because thread is not stopped. */
            /* Connection has been closed. */
            pppClose( ppp_con_fd );
        }

        /* Wait 1s until reopening the connection. */
        vTaskDelay( ( portTickType ) ( 1000UL / portTICK_RATE_MS ) );
    }
    while( pdTRUE );
}

void
vPPPStatusCB( void *ctx, int err, void *arg )
{
    /* Imported from ipcp.c */
    extern char    *_inet_ntoa( u32_t n );

    ePPPThreadControl ePPPThrCtlNew;
    struct ppp_addrs *ppp_addrs;

    switch ( err )
    {
        /* No error. */
    case PPPERR_NONE:
        ePPPThrCtlNew = CONNECTED;
        ppp_addrs = arg;
        vMBPortLog( MB_LOG_INFO, "PPP", "new PPP connection established\r\n" );
        vMBPortLog( MB_LOG_INFO, "PPP", "  our IP address = %s\r\n",
                    _inet_ntoa( ppp_addrs->our_ipaddr.addr ) );
        vMBPortLog( MB_LOG_INFO, "PPP", "  his IP address = %s\r\n",
                    _inet_ntoa( ppp_addrs->his_ipaddr.addr ) );
        vMBPortLog( MB_LOG_INFO, "PPP", "  netmask = %s\r\n",
                    _inet_ntoa( ppp_addrs->netmask.addr ) );
        break;
    default:
        ePPPThrCtlNew = DISCONNECT;
        vMBPortLog( MB_LOG_ERROR, "PPP", "PPP connection died ( err = %d )\r\n", err );
        break;
    }
    vPortEnterCritical(  );
    ePPPThrCtl = ePPPThrCtlNew;
    vPortExitCritical(  );
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
