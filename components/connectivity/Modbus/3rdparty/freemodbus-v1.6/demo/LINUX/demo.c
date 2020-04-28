/*
 * FreeModbus Libary: Linux Demo Application
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

/* ----------------------- Standard includes --------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define PROG            "freemodbus"

#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define REG_HOLDING_START 2000
#define REG_HOLDING_NREGS 130

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

static enum ThreadState
{
    STOPPED,
    RUNNING,
    SHUTDOWN
} ePollThreadState;

static pthread_mutex_t xLock = PTHREAD_MUTEX_INITIALIZER;
static BOOL     bDoExit;

/* ----------------------- Static functions ---------------------------------*/
static BOOL     bCreatePollingThread( void );
static enum ThreadState eGetPollingThreadState( void );
static void     vSetPollingThreadState( enum ThreadState eNewState );
static void    *pvPollingThread( void *pvParameter );

/* ----------------------- Start implementation -----------------------------*/
BOOL
bSetSignal( int iSignalNr, void ( *pSigHandler ) ( int ) )
{
    BOOL            bResult;
    struct sigaction xNewSig, xOldSig;

    xNewSig.sa_handler = pSigHandler;
    sigemptyset( &xNewSig.sa_mask );
    xNewSig.sa_flags = 0;
    if( sigaction( iSignalNr, &xNewSig, &xOldSig ) != 0 )
    {
        bResult = FALSE;
    }
    else
    {
        bResult = TRUE;
    }
    return bResult;
}

void
vSigShutdown( int xSigNr )
{
    switch ( xSigNr )
    {
    case SIGQUIT:
    case SIGINT:
    case SIGTERM:
        vSetPollingThreadState( SHUTDOWN );
        bDoExit = TRUE;
    }
}

int
main( int argc, char *argv[] )
{
    int             iExitCode;
    CHAR            cCh;

    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
    if( !bSetSignal( SIGQUIT, vSigShutdown ) ||
        !bSetSignal( SIGINT, vSigShutdown ) || !bSetSignal( SIGTERM, vSigShutdown ) )
    {
        fprintf( stderr, "%s: can't install signal handlers: %s!\n", PROG, strerror( errno ) );
        iExitCode = EXIT_FAILURE;
    }
    else if( eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN ) != MB_ENOERR )
    {
        fprintf( stderr, "%s: can't initialize modbus stack!\n", PROG );
        iExitCode = EXIT_FAILURE;
    }
    else if( eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 ) != MB_ENOERR )
    {
        fprintf( stderr, "%s: can't set slave id!\n", PROG );
        iExitCode = EXIT_FAILURE;
    }
    else
    {
        vSetPollingThreadState( STOPPED );

        /* CLI interface. */
        printf( "Type 'q' for quit or 'h' for help!\n" );
        bDoExit = FALSE;
        do
        {
            printf( "> " );
            cCh = getchar(  );

            switch ( cCh )
            {
            case 'q':
                bDoExit = TRUE;
                break;
            case 'd':
                vSetPollingThreadState( SHUTDOWN );
                break;
            case 'e':
                if( bCreatePollingThread(  ) != TRUE )
                {
                    printf( "Can't start protocol stack! Already running?\n" );
                }
                break;
            case 's':
                switch ( eGetPollingThreadState(  ) )
                {
                case RUNNING:
                    printf( "Protocol stack is running.\n" );
                    break;
                case STOPPED:
                    printf( "Protocol stack is stopped.\n" );
                    break;
                case SHUTDOWN:
                    printf( "Protocol stack is shuting down.\n" );
                    break;
                }
                break;
            case 'h':
                printf( "FreeModbus demo application help:\n" );
                printf( "  'd' ... disable protocol stack.\n" );
                printf( "  'e' ... enabled the protocol stack.\n" );
                printf( "  's' ... show current status.\n" );
                printf( "  'q' ... quit application.\n" );
                printf( "  'h' ... this information.\n" );
                printf( "\n" );
                printf( "Copyright 2006 Christian Walter <wolti@sil.at>\n" );
                break;
            default:
                if( !bDoExit && ( cCh != '\n' ) )
                {
                    printf( "illegal command '%c'!\n", cCh );
                }
                break;
            }

            /* eat up everything untill return character. */
            while( !bDoExit && ( cCh != '\n' ) )
            {
                cCh = getchar(  );
            }
        }
        while( !bDoExit );

        /* Release hardware resources. */
        ( void )eMBClose(  );
        iExitCode = EXIT_SUCCESS;
    }
    return iExitCode;
}

BOOL
bCreatePollingThread( void )
{
    BOOL            bResult;
    pthread_t       xThread;

    if( eGetPollingThreadState(  ) == STOPPED )
    {
        if( pthread_create( &xThread, NULL, pvPollingThread, NULL ) != 0 )
        {
            bResult = FALSE;
        }
        else
        {
            bResult = TRUE;
        }
    }
    else
    {
        bResult = FALSE;
    }

    return bResult;
}

void           *
pvPollingThread( void *pvParameter )
{
    vSetPollingThreadState( RUNNING );

    if( eMBEnable(  ) == MB_ENOERR )
    {
        do
        {
            if( eMBPoll(  ) != MB_ENOERR )
                break;
            usRegInputBuf[0] = ( USHORT ) rand(  );
        }
        while( eGetPollingThreadState(  ) != SHUTDOWN );
    }

    ( void )eMBDisable(  );

    vSetPollingThreadState( STOPPED );

    return 0;
}

enum ThreadState
eGetPollingThreadState(  )
{
    enum ThreadState eCurState;

    ( void )pthread_mutex_lock( &xLock );
    eCurState = ePollThreadState;
    ( void )pthread_mutex_unlock( &xLock );

    return eCurState;
}

void
vSetPollingThreadState( enum ThreadState eNewState )
{
    ( void )pthread_mutex_lock( &xLock );
    ePollThreadState = eNewState;
    ( void )pthread_mutex_unlock( &xLock );
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
