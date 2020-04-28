/*
 * FreeModbus Libary: STR71x Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"

/* ----------------------- Platform includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ----------------------- STR71X includes ----------------------------------*/
#include "eic.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbutils.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_COILS_START     1000
#define REG_COILS_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
static unsigned char ucRegCoilsBuf[REG_COILS_SIZE / 8];

/* ----------------------- Static functions ---------------------------------*/
static void     vModbusTask( void *pvParameters );

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    EIC_Init(  );
    EIC_IRQConfig( ENABLE );

    ( void )xTaskCreate( vModbusTask, NULL, configMINIMAL_STACK_SIZE, NULL,
                         tskIDLE_PRIORITY, NULL );

    vTaskStartScheduler(  );
    return 0;
}

static void
vModbusTask( void *pvParameters )
{
    /* Select either ASCII or RTU Mode. */
    ( void )eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN );

    /* Enable the Modbus Protocol Stack. */
    ( void )eMBEnable(  );

    /* Enter main loop. */
    for( ;; )
    {
        /* Call the main polling loop of the Modbus protocol stack. */
        ( void )eMBPoll(  );
    }
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iNCoils = ( int )usNCoils;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
                /* Read current values and pass to protocol stack. */
            case MB_REG_READ:
                while( iNCoils > 0 )
                {
                    *pucRegBuffer++ =
                        xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                        ( unsigned char )( iNCoils >
                                                           8 ? 8 :
                                                           iNCoils ) );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;

                /* Update current register values. */
            case MB_REG_WRITE:
                while( iNCoils > 0 )
                {
                    xMBUtilSetBits( ucRegCoilsBuf, usBitOffset, 
                                    ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                                    *pucRegBuffer++ );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;
        }

    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

void
__assert( const char *pcFile, const char *pcLine, int iLineNumber )
{
    portENTER_CRITICAL(  );
    for( ;; );
}
