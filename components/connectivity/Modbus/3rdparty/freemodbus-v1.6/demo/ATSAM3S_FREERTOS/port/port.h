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

#ifndef _PORT_H
#define _PORT_H

/* ----------------------- Platform includes --------------------------------*/
#include <board.h>
#include <tc/tc.h>
#include <pmc/pmc.h>
#include <wdt/wdt.h>
#include <pio/pio.h>
#include <cmsis/core_cm3.h>
#include <usart/usart.h>

/* ----------------------- Defines ------------------------------------------*/
#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION( )   vMBPortEnterCritical()
#define EXIT_CRITICAL_SECTION( )    vMBPortExitCritical()

#define assert( x )

typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef TRUE
#define TRUE                                    1
#endif

#ifndef FALSE
#define FALSE                                   0
#endif

#define MB_PORT_HAS_CLOSE	                    1
#define MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS    2

/* ----------------------- Prototypes ---------------------------------------*/
void            vMBPortSetWithinException( BOOL bInException );
BOOL            bMBPortIsWithinException( void );

void            vMBPortEnterCritical( void );
void            vMBPortExitCritical( void );

#endif
