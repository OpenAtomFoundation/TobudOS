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

/* ----------------------- System includes ----------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/


#define MB_TIMER_DEBUG                      ( 0 )
#define MB_TIMER_PRESCALER                  ( 128UL )
#define MB_TIMER_TICKS                      ( BOARD_MCK / MB_TIMER_PRESCALER )
#define MB_50US_TICKS                       ( 20000UL )

#define TCX                                 ( TC0 )
#define TCXIRQ                              ( TC0_IRQn )
#define TCCHANNEL                           ( 0 )
#define TCX_IRQHANDLER                      TC0_IrqHandler

#define TC_CMRX_WAVE                        ( 0x1 << 15 )
#define TC_CMRX_TCCLKS_TIMER_DIV4_CLOCK     ( 0x3 << 0 )
#define TC_CMRX_CPCSTOP                     ( 0x1 << 6 )
#define TC_CMRX_WAVESEL_UP_RC               ( 0x2 << 13 )

#define TC_IERX_CPCS                        ( 0x1 << 4 )
#define TC_IERX_CPAS                        ( 0x1 << 2 )
#define TC_SRX_CPAS                         ( 0x1 << 2 )
#if MB_TIMER_DEBUG == 1
#define TIMER_PIN { 1 << 6, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT }
#endif

#ifndef SYSTICK_COUNTFLAG
/* missing in CMSIS */
#define SYSTICK_COUNTFLAG                   ( 16 )             
#endif
/* ----------------------- Static variables ---------------------------------*/
#if MB_TIMER_DEBUG == 1
const static Pin xTimerDebugPins[] = { TIMER_PIN };
#endif

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
#if MB_TIMER_DEBUG == 1
    PIO_Configure( xTimerDebugPins, PIO_LISTSIZE( xTimerDebugPins ) );
#endif
    NVIC_DisableIRQ( TCXIRQ );

    PMC_EnablePeripheral( ID_TC0 );
    TC_Configure( TCX, 0, TC_CMRX_WAVE | TC_CMRX_TCCLKS_TIMER_DIV4_CLOCK | TC_CMRX_WAVESEL_UP_RC | TC_CMRX_CPCSTOP );
    TCX->TC_CHANNEL[TCCHANNEL].TC_RA = ( MB_TIMER_TICKS * usTim1Timerout50us ) / ( MB_50US_TICKS );
    TCX->TC_CHANNEL[TCCHANNEL].TC_RC = ( MB_TIMER_TICKS * usTim1Timerout50us ) / ( MB_50US_TICKS );

    NVIC_ClearPendingIRQ( TCXIRQ );
    NVIC_SetPriority( TCXIRQ, 0xF << 4 );
    NVIC_EnableIRQ( TCXIRQ );

    return TRUE;
}

void 
vMBPortTimerClose( void )
{
    NVIC_DisableIRQ( TCXIRQ );
    PMC_DisablePeripheral( ID_TC0 );
}

void
vMBPortTimersEnable(  )
{
#if MB_TIMER_DEBUG == 1
    PIO_Set( &xTimerDebugPins[0] );  
#endif  
    TCX->TC_CHANNEL[TCCHANNEL].TC_IER = TC_IERX_CPAS;
    TC_Start( TCX, 0 );
}

void
vMBPortTimersDisable(  )
{
    TC_Stop( TCX, 0 );
#if MB_TIMER_DEBUG == 1
    PIO_Clear( &xTimerDebugPins[0] );
#endif   
}

void
vMBPortTimersDelay( USHORT usTimeOutMS )
{

    SysTick->CTRL = 0;
    SysTick->LOAD = BOARD_MCK / 1000;  
    SysTick->VAL = 0; /* Clear COUNTFLAG */
    SysTick->CTRL = ( 1 << SYSTICK_CLKSOURCE) | ( 1<<SYSTICK_ENABLE);   
    while( usTimeOutMS )
    {
        while( 0 == ( SysTick->CTRL & ( 1 << SYSTICK_COUNTFLAG ) ) );
        SysTick->VAL = 0;
        usTimeOutMS--;
    }
}

void
TCX_IRQHANDLER( void )
{
    uint32_t        xTCX_IMRX = TCX->TC_CHANNEL[TCCHANNEL].TC_IMR;
    uint32_t        xTCX_SRX = TCX->TC_CHANNEL[TCCHANNEL].TC_SR;
    uint32_t        uiSRMasked = xTCX_SRX & xTCX_IMRX;

    if( ( uiSRMasked & TC_SRX_CPAS ) > 0 )
    {
#if MB_TIMER_DEBUG == 1
        PIO_Clear( &xTimerDebugPins[0] );
#endif
        ( void )pxMBPortCBTimerExpired(  );
    }
}
