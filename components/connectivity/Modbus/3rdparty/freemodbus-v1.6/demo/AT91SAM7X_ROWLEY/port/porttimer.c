/* 
 * MODBUS Library: AT91SAM7X/FreeRTOS port
 * Copyright (c) 2007 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define TIMER_TIMEOUT_INVALID	( 65535U )

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    volatile USHORT usNTimeOutMS;
    volatile USHORT usNTimeLeft;
} xTimerInternalHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC xTimerInternalHandle arxTimerHdls[1];
STATIC BOOL     bIsInitalized = FALSE;

/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    USHORT          usTimeoutMS;

    ENTER_CRITICAL_SECTION(  );
    usTimeoutMS = ( usTim1Timerout50us + 10 ) / 20;
    if( 0 == usTimeoutMS )
    {
        usTimeoutMS = 1;
    }
    arxTimerHdls[0].usNTimeOutMS = usTimeoutMS;
    arxTimerHdls[0].usNTimeLeft = TIMER_TIMEOUT_INVALID;
    bIsInitalized = TRUE;
    EXIT_CRITICAL_SECTION(  );

    return TRUE;
}

void
vMBPortTimerClose( void )
{
    ENTER_CRITICAL_SECTION(  );
    if( bIsInitalized )
    {
        arxTimerHdls[0].usNTimeLeft = TIMER_TIMEOUT_INVALID;
        bIsInitalized = FALSE;
    }
    EXIT_CRITICAL_SECTION(  );
}

void
vMBPortTimersEnable(  )
{
    assert( bIsInitalized );
    ENTER_CRITICAL_SECTION(  );
    arxTimerHdls[0].usNTimeLeft = arxTimerHdls[0].usNTimeOutMS;
    EXIT_CRITICAL_SECTION(  );
}

void
vMBPortTimersDisable(  )
{
    assert( bIsInitalized );
    ENTER_CRITICAL_SECTION(  );
    arxTimerHdls[0].usNTimeLeft = TIMER_TIMEOUT_INVALID;
    EXIT_CRITICAL_SECTION(  );
}

void
vApplicationTickHook( void )
{
    if( bIsInitalized )
    {
        if( TIMER_TIMEOUT_INVALID != arxTimerHdls[0].usNTimeLeft )
        {
            arxTimerHdls[0].usNTimeLeft--;
            if( 0 == arxTimerHdls[0].usNTimeLeft )
            {
                arxTimerHdls[0].usNTimeLeft = TIMER_TIMEOUT_INVALID;
                pxMBPortCBTimerExpired(  );
            }
        }
    }
}
