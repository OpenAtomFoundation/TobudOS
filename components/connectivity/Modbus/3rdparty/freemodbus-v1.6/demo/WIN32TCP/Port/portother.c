/*
 * FreeModbus Libary: Win32 Port
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

#include <stdio.h>
#include <stdlib.h>
#include "winsock2.h"

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"


BOOL
prvMBTCPPortAddressToString( SOCKET xSocket, LPTSTR szAddr, USHORT usBufSize )
{
    BOOL            bOkay;
    SOCKADDR_IN     xClientAddr;
    int             iAddrLen = sizeof( SOCKADDR_IN );
    DWORD           dwBufSize = usBufSize;

    assert( ( szAddr != NULL ) && ( usBufSize > 0 ) );
    if( getsockname( xSocket, ( SOCKADDR * ) & xClientAddr, &iAddrLen ) == SOCKET_ERROR )
    {
        bOkay = FALSE;
    }
    else if( WSAAddressToString( ( SOCKADDR * ) & xClientAddr, iAddrLen, NULL, szAddr,
                                 &dwBufSize ) == SOCKET_ERROR )
    {
        bOkay = FALSE;
    }
    else
    {
        bOkay = TRUE;
    }
    return bOkay;
}

LPTSTR
prvMBTCPPortFrameToString( UCHAR * pucFrame, USHORT usFrameLen )
{
    LPTSTR          szBuf;
    int             i;
    int             res;
    int             szBufPos = 0;
    int             szBufLength = usFrameLen + 128;

    assert( ( pucFrame != NULL ) );

    szBuf = malloc( szBufLength * sizeof( TCHAR ) );
    if( szBuf != NULL )
    {
        for( i = 0; i < usFrameLen; i++ )
        {
            /* Print some additional frame information. */
            switch ( i )
            {
            case 0:            /* TID = Transaction Identifier. */
                res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( "| TID = " ) );
                break;
            case 2:            /* PID = Protocol Identifier. */
                res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( " | PID = " ) );
                break;
            case 4:            /* Length */
                res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( " | LEN = " ) );
                break;
            case 6:            /* UID = Unit Identifier. */
                res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( " | UID = " ) );
                break;
            case 7:            /* MB Function Code. */
                res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( " || FUNC = " ) );
                break;
            case 8:            /* MB PDU rest. */
                res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( " | DATA = " ) );
                break;
            default:
                res = 0;
                break;
            }
            if( res == -1 )
            {
                break;
            }
            else if( res != 0 )
            {
                szBufPos += res;
                szBufLength -= res;
            }

            /* Print the data. */
            res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( "%02X" ),
                                pucFrame[i] );
            if( res == -1 )
            {
                break;
            }
            szBufPos += res;
            szBufLength -= res;
        }
        if( res != -1 )
        {
            /* Append an end of frame string. */
            res = _sntprintf_s( &szBuf[szBufPos], szBufLength, _TRUNCATE, _T( " |" ), pucFrame[i] );
        }
        if( res == -1 )
        {
            free( szBuf );
            szBuf = NULL;
        }
    }
    return szBuf;
}

TCHAR          *
WsaError2String( DWORD dwError )
{
    static TCHAR    szUserBuf[512];
    static LPTSTR   szErrorMsg = _T( "internal error" );
    LPTSTR          lpMsgBuf = NULL;
    DWORD           dwLength;

    dwLength = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                              FORMAT_MESSAGE_FROM_SYSTEM,
                              NULL,
                              dwError,
                              MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                              ( LPTSTR ) & lpMsgBuf, 0, NULL );
    if( dwLength == 0 )
    {
        lpMsgBuf = _T( "internal error" );
    }

    _tcsnccpy_s( szUserBuf, _countof( szUserBuf ), lpMsgBuf, _tcslen( lpMsgBuf ) );
    LocalFree( lpMsgBuf );

    return szUserBuf;
}

void
vMBPortLog( eMBPortLogLevel eLevel, const TCHAR * szModule, const TCHAR * szFmt, ... )
{
    va_list         args;
    static const LPTSTR arszLevel2Str[] =
        { _T( "DEBUG" ), _T( "INFO" ), _T( "WARN" ), _T( "ERROR" ) };

    _ftprintf( stderr, _T( "%s: %s: " ), arszLevel2Str[eLevel], szModule );

    va_start( args, szFmt );
    _vftprintf( stderr, szFmt, args );
    va_end( args );
}
