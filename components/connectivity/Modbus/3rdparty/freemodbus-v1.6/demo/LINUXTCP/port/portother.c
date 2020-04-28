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

 /**********************************************************
 *	Linux TCP support.
 *	Based on Walter's project. 
 *	Modified by Steven Guo <gotop167@163.com>
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"


BOOL
prvMBTCPPortAddressToString( SOCKET xSocket, CHAR * szAddr, USHORT usBufSize )
{
    return TRUE;
}

CHAR           *
prvMBTCPPortFrameToString( UCHAR * pucFrame, USHORT usFrameLen )
{
    return NULL;
}

CHAR           *
WsaError2String( int iError )
{
    return NULL;
}

void
vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule, const CHAR * szFmt, ... )
{
    va_list         args;
    static const CHAR *arszLevel2Str[] = { "DEBUG", "INFO", "WARN", "ERROR" };

    fprintf( stderr, "%s: %s: ", arszLevel2Str[eLevel], szModule );

    va_start( args, szFmt );
    fprintf( stderr, szFmt, args );
    va_end( args );
}
