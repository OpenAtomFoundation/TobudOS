/*
 * FreeModbus Libary: Atmel AT91SAM3S Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id$
 */


/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include <intrinsics.h>

/* ----------------------- Variables ----------------------------------------*/
static ULONG    ulNesting;
static __istate_t xOldState;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortEnterCritical( void )
{
    __istate_t      xCurState;
    xCurState = __get_interrupt_state(  );
    __disable_interrupt(  );
    if( ulNesting == 0 )
    {
        xOldState = xCurState;
    }
    ulNesting++;
}

void
vMBPortExitCritical( void )
{
    ulNesting--;
    if( 0 == ulNesting )
    {
        __set_interrupt_state( xOldState );
    }
}

void
vMBPortClose( void )
{
    extern void vMBPortSerialClose( void );
    extern void vMBPortTimerClose( void );
    vMBPortSerialClose(  );
    vMBPortTimerClose(  );
}