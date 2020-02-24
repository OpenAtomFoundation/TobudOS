/*
 * FreeModbus Libary: HCS08 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Copyright (C) 2008 Trond Melen
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

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

UCHAR           CCR_reg;        // SaveStatusReg stores the Condition Code Register here

/* ----------------------- Start implementation -----------------------------*/

void
main( void )
{
    eMBErrorCode    eStatus;

    /* Use external 32.768 Hz crystal to generate 4.194.304 Hz bus clock */
    ICGC1 = 0x38;               // ??=0,RANGE=0,REFS=1,CLKS=1:1,OSCSTEN=0,??=0:0
    while( ICGS2_DCOS == 0 );

#if 0
    /* Test code for porting
     */
#if 1
    /* Timer test
     * Comment out call to pxMBPortCBTimerExpired() in prvvTIMERExpiredISR when testing the timer
     */

    /* Disable the COP watchdog */
    SOPT = 0x53;                // COPE=0,COPT=1,STOPE=0,??=1:0:0,BKGDPE=1,??=1

    ( void )xMBPortTimersInit( 20000 );

    EnableInterrupts;
    for( ;; )
    {
        vMBPortTimersEnable(  );
        _Wait;                  // wait for an interrupt

        /* toggle LED1 */
        PTFD_PTFD0 = !PTFD_PTFD0;
        PTFDD_PTFDD0 = 1;
    }

#else
    /* Receiver test
     * Comment out call to pxMBFrameCBByteReceived() in prvvUARTRxISR() when testing the receiver
     */

    /* Disable the COP watchdog */
    SOPT = 0x53;                // COPE=0,COPT=1,STOPE=0,??=1:0:0,BKGDPE=1,??=1

    /* Enable the receiver. */
    assert( xMBPortSerialInit( 0, 9600, 8, MB_PAR_NONE ) );

    EnableInterrupts;
    for( ;; )
    {
        UCHAR           ucByte;

        vMBPortSerialEnable( TRUE, FALSE );
        _Wait;                  // wait for an interrupt
        assert( xMBPortSerialGetByte( &ucByte ) );

        /* toggle LED1 */
        PTFD_PTFD0 = !PTFD_PTFD0;
        PTFDD_PTFDD0 = 1;

        /* Transmitter test
         * Comment out call to pxMBFrameCBTransmitterEmpty() in prvvUARTTxReadyISR() when testing the transmitter
         */
#if 0
        vMBPortSerialEnable( FALSE, TRUE );
        assert( xMBPortSerialPutByte( ucByte ) );
        _Wait;                  // wait for an interrupt

        /* toggle LED1 */
        PTFD_PTFD0 = !PTFD_PTFD0;
#endif // Transmitter test
    }
#endif // Receiver test

#else
    /* Demo
     * NOTE: Make sure the callbacks in the three ISPs have been restored after above testing
     */

    /* Initialization */
    eStatus = eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN );
//      eStatus = eMBInit( MB_ASCII, 0x0A, 0, 38400, MB_PAR_EVEN );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    /* Start polling */
    EnableInterrupts;
    for( ;; )
    {
        /* Poll for Modbus events */
        ( void )eMBPoll(  );

        /* Count the number of polls */
        usRegInputBuf[0]++;

        /* Count the number of timer overflows */
        if( TPM1SC_TOF )
        {
            TPM1SC_TOF = 0;
            ENTER_CRITICAL_SECTION(  );
            if( ++usRegInputBuf[1] == 0 )       // Happens every 2 seconds
                usRegInputBuf[2]++;     // Happens every 36.4 hours
            EXIT_CRITICAL_SECTION(  );
        }

        /* Keep the COP watchdog happy */
        __RESET_WATCHDOG(  );
    }
#endif // Test code when porting
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
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
    return MB_ENOREG;
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
