/*
 * FreeModbus Libary: STR71x Port
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
#include "assert.h"

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- STR71X includes ----------------------------------*/
#include "gpio.h"
#include "eic.h"
#include "uart.h"
#include "tim.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_TIMER_DEV            ( TIM0 )
#define MB_TIMER_PRESCALER      ( 255UL )
#define MB_TIMER_IRQ_CH         ( T0TIMI_IRQChannel )
#define MB_IRQ_PRIORITY         ( 1 )

/* Timer ticks are counted in multiples of 50us. Therefore 20000 ticks are
 * one second.
 */
#define MB_TIMER_TICKS          ( 20000UL )

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usTimerDeltaOCRA;

/* ----------------------- Static functions ---------------------------------*/
void            prvvMBTimerIRQHandler( void ) __attribute__ ( ( naked ) );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    /* Calculate output compare value for timer1. */
    usTimerDeltaOCRA =
        ( ( RCCU_FrequencyValue( RCCU_PCLK ) / ( MB_TIMER_PRESCALER + 1 ) ) *
          usTim1Timerout50us ) / ( MB_TIMER_TICKS );

    TIM_Init( MB_TIMER_DEV );
    TIM_PrescalerConfig( MB_TIMER_DEV, MB_TIMER_PRESCALER );
    if( usTimerDeltaOCRA > 0 )
    {
        TIM_OCMPModeConfig( MB_TIMER_DEV, TIM_CHANNEL_A, usTimerDeltaOCRA, TIM_TIMING, TIM_LOW );
    }

    vMBPortTimersDisable(  );
    EIC_IRQChannelConfig( MB_TIMER_IRQ_CH, ENABLE );
    EIC_IRQChannelPriorityConfig( MB_TIMER_IRQ_CH, MB_IRQ_PRIORITY );

    return TRUE;
}

void
prvvMBTimerIRQHandler( void )
{
    portENTER_SWITCHING_ISR(  );

    static portBASE_TYPE xTaskSwitch = pdFALSE;

    if( ( usTimerDeltaOCRA > 0 ) && ( TIM_FlagStatus( MB_TIMER_DEV, TIM_OCFA ) ) )
    {
        xTaskSwitch |= pxMBPortCBTimerExpired(  );
        TIM_FlagClear( MB_TIMER_DEV, TIM_OCFA );
    }

    /* End the interrupt in the EIC. */
    EIC->IPR |= 1 << EIC_CurrentIRQChannelValue(  );
    portEXIT_SWITCHING_ISR( xTaskSwitch );
}

void
vMBPortTimersEnable(  )
{
    unsigned portSHORT maskTimFlags = 0;
    unsigned portSHORT maskTimIT = 0;

    MB_TIMER_DEV->CNTR = 0;
    if( usTimerDeltaOCRA > 0 )
    {
        MB_TIMER_DEV->OCAR = usTimerDeltaOCRA;
        maskTimFlags |= TIM_OCFA;
        maskTimIT |= TIM_OCA_IT;
    }

    TIM_FlagClear( MB_TIMER_DEV, maskTimFlags );
    TIM_ITConfig( MB_TIMER_DEV, maskTimIT, ENABLE );
    TIM_CounterConfig( MB_TIMER_DEV, TIM_START );
}

void
vMBPortTimersDisable(  )
{
    /* We can always clear both flags. This improves performance. */
    TIM_FlagClear( MB_TIMER_DEV, TIM_OCFA | TIM_OCFB );
    TIM_ITConfig( MB_TIMER_DEV, TIM_OCA_IT | TIM_OCB_IT, DISABLE );
    TIM_CounterConfig( MB_TIMER_DEV, TIM_STOP );
}
