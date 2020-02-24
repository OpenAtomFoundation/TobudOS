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
#include <FreeRTOS.h>
#include <task.h>

/* ----------------------- AT91SAM3S includes -------------------------------*/
#include <board.h>
#include <usart/usart.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START                 ( 1000 )
#define REG_INPUT_NREGS                 ( 64 )

#define REG_HOLDING_START               ( 1 )
#define REG_HOLDING_NREGS               ( 32 )

#define TASK_MODBUS_STACK_SIZE          ( 256 )
#define TASK_MODBUS_PRIORITY            ( tskIDLE_PRIORITY + 1 )

#define TASK_APPL_STACK_SIZE            ( 256 )
#define TASK_APPL_PRIORITY              ( tskIDLE_PRIORITY + 1 )

/* ----------------------- Static functions ---------------------------------*/
static void     _SetupHardware( void );
static void     vTaskApplication( void *pvArg );
static void     vTaskMODBUS( void *pvArg );

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    _SetupHardware(  );

    if( pdPASS != xTaskCreate( vTaskMODBUS, "MODBUS", TASK_MODBUS_STACK_SIZE, NULL, TASK_MODBUS_PRIORITY, NULL ) )
    {
    }
    else if( pdPASS != xTaskCreate( vTaskApplication, "APPL", TASK_APPL_STACK_SIZE, NULL, TASK_APPL_PRIORITY, NULL ) )
    {
    }
    else
    {
        vTaskStartScheduler(  );
    }
    return 1;
}

static void
vTaskApplication( void *pvArg )
{
    for( ;; )
    {
        vTaskDelay( 1000 );
    }
}

static void
vTaskMODBUS( void *pvArg )
{
    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
    eMBErrorCode    eStatus;

    for( ;; )
    {
        if( MB_ENOERR != ( eStatus = eMBInit( MB_ASCII, 0x0A, 1, 38400, MB_PAR_EVEN ) ) )
        {
            /* Can not initialize. Add error handling code here. */
        }
        else
        {
            if( MB_ENOERR != ( eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 ) ) )
            {
                /* Can not set slave id. Check arguments */
            }
            else if( MB_ENOERR != ( eStatus = eMBEnable(  ) ) )
            {
                /* Enable failed. */
            }
            else
            {
                usRegHoldingBuf[0] = 1;
                do
                {
                    ( void )eMBPoll(  );

                    /* Here we simply count the number of poll cycles. */
                    usRegInputBuf[0]++;
                }
                while( usRegHoldingBuf[0] );
            }
            ( void )eMBDisable(  );
            ( void )eMBClose(  );
        }
        vTaskDelay( 50 );
    }
}

static void
_SetupHardware( void )
{
    WDT_Disable(  );

    uint32_t        i = 0;
    for( i = 0; i < 35; i++ )
    {
        NVIC_SetPriority( ( IRQn_Type ) i, 0xF << 4 );
    }
}

void
vApplicationStackOverflowHook( xTaskHandle * pxTask, signed char *pcTaskName )
{
    ( void )pxTask;
    ( void )pcTaskName;
    for( ;; );
}

void
vApplicationIdleHook( void )
{
}

void
vApplicationTickHook( void )
{
}


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
