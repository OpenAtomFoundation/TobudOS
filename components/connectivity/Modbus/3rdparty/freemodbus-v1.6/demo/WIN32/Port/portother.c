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

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"

LPTSTR
Error2String( DWORD dwError )
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
    TCHAR    szBuf[512];
    int      i;
    va_list         args;
    static const LPTSTR arszLevel2Str[] =
        { _T( "DEBUG" ), _T( "INFO" ), _T( "WARN" ), _T( "ERROR" ) };

    i = _sntprintf_s( szBuf, _countof(szBuf) , _TRUNCATE, _T( "%s: %s: " ), 
                     arszLevel2Str[eLevel], szModule );

    if( i != 0 )
    {
        va_start( args, szFmt );
        i += _vsntprintf_s( &szBuf[i], _countof(szBuf) - i, _TRUNCATE, szFmt, args );
        va_end( args );
    }

    if( i != 0 )
    {
        if( eLevel == MB_LOG_DEBUG )
        {
            OutputDebugString( szBuf );
        }
        else
        {
            _fputts( szBuf, stderr );
        }
    }
    
}
