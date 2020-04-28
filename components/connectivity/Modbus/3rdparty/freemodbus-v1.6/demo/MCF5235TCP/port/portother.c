/*
 * FreeModbus Libary: lwIP Port
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

/* ----------------------- System includes ----------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "port.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_FRAME_LOG_BUFSIZE    512

/* ----------------------- Start implementation -----------------------------*/

#ifdef MB_TCP_DEBUG
void
prvvMBTCPLogFrame( UCHAR * pucMsg, UCHAR * pucFrame, USHORT usFrameLen )
{
    int             i;
    int             res;
    int             iBufPos = 0;
    size_t          iBufLeft = MB_FRAME_LOG_BUFSIZE;
    static CHAR     arcBuffer[MB_FRAME_LOG_BUFSIZE];

    assert( pucFrame != NULL );

    for( i = 0; i < usFrameLen; i++ )
    {
        /* Print some additional frame information. */
        switch ( i )
        {
        case 0:
            /* TID = Transaction Identifier. */
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, "| TID = " );
            break;
        case 2:
            /* PID = Protocol Identifier. */
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | PID = " );
            break;
        case 4:
            /* Length */
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | LEN = " );
            break;
        case 6:
            /* UID = Unit Identifier. */
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | UID = " );
            break;
        case 7:
            /* MB Function Code. */
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, "|| FUNC = " );
            break;
        case 8:
            /* MB PDU rest. */
            res = snprintf( &arcBuffer[iBufPos], iBufLeft, " | DATA = " );
            break;
        default:
            res = 0;
            break;
        }
        if( res == -1 )
        {
            break;
        }
        else
        {
            iBufPos += res;
            iBufLeft -= res;
        }

        /* Print the data. */
        res = snprintf( &arcBuffer[iBufPos], iBufLeft, "%02X", pucFrame[i] );
        if( res == -1 )
        {
            break;
        }
        else
        {
            iBufPos += res;
            iBufLeft -= res;
        }
    }

    if( res != -1 )
    {
        /* Append an end of frame string. */
        res = snprintf( &arcBuffer[iBufPos], iBufLeft, " |\r\n" );
        if( res != -1 )
        {
            vMBPortLog( MB_LOG_DEBUG, pucMsg, "%s", arcBuffer );
        }
    }
}
#endif

#ifdef MB_TCP_DEBUG
void
vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule, const CHAR * szFmt, ... )
{
    va_list         args;
    static const char *arszLevel2Str[] = { "DEBUG", "INFO", "WARN", "ERROR" };

    ( void )printf( "%s: %s: ", arszLevel2Str[eLevel], szModule );
    va_start( args, szFmt );
    vprintf( szFmt, args );
    va_end( args );
}
#endif
