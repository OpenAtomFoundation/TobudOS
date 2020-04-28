/*
 * FreeModbus Libary: MCF5235 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
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
#include "port.h"
#include "mcf5xxx.h"
#include "mcf523x.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegSR;
static char    *heap_ptr;

/* ----------------------- Start implementation -----------------------------*/
void
prvvPortEnterCritical(  )
{
    asm volatile    ( "move.w	%%sr, %%d0\n"
                      "move.w	%%d0, %0\n"
                      "move.w	#0x2700, %%sr":"=m" ( usRegSR )::"%%d0" );
}

void
prvvPortExitCritical(  )
{
    asm volatile    ( "move.w	%0, %%d0\n"
                      "move.w	%%d0, %%sr\n"::"m" ( usRegSR ):"%%d0" );
}


char           *
sbrk( int nbytes )
{
    extern void     _end;
    char           *base;

    if( !heap_ptr )
        heap_ptr = ( char * )&_end;
    base = heap_ptr;
    heap_ptr += nbytes;
    return base;
}
