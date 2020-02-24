/* ------------------------ System includes ------------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ------------------------ FreeRTOS includes ----------------------------- */
#include "FreeRTOS.h"
#include "task.h"

/* ------------------------ LWIP includes --------------------------------- */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"

/* ------------------------ FreeModbus includes --------------------------- */
#include "mb.h"

/* ------------------------ Project includes ------------------------------ */
#include "mcf5xxx.h"
#include "mcf523x.h"
#include "netif/fec.h"

#include "serial.h"

/* ------------------------ Defines --------------------------------------- */
#define mainCOM_TEST_BAUD_RATE  ( ( unsigned portLONG ) 38400 )

#define mainMB_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define PROG                    "FreeModbus"
#define REG_INPUT_START         1000
#define REG_INPUT_NREGS         4
#define REG_HOLDING_START       2000
#define REG_HOLDING_NREGS       130

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

xComPortHandle  xSTDComPort = NULL;

/* ------------------------ Static functions ------------------------------ */
static void     vlwIPInit( void );
static void     vMBServerTask( void *arg );

/* ------------------------ Implementation -------------------------------- */
int
main( int argc, char *argv[] )
{
    asm volatile    ( "move.w  #0x2000, %sr\n\t" );

    /* Initialize serial communication device. */
    xSTDComPort = xSerialPortInitMinimal( 38400, 8 );

    /* Initialize lwIP protocol stack. */
    vlwIPInit(  );

    if( sys_thread_new( vMBServerTask, NULL, mainMB_TASK_PRIORITY ) == NULL )
    {
        fprintf( stderr, "%s: can't create modbus task!\r\n", PROG );
    }
    else
    {
        /* Now all the tasks have been started - start the scheduler. */
        vTaskStartScheduler(  );
    }

    /* Should never get here! */
    return 0;
}

void
vlwIPInit( void )
{
    /* Initialize lwIP and its interface layer. */
    sys_init(  );
    mem_init(  );
    memp_init(  );
    pbuf_init(  );
    netif_init(  );
    ip_init(  );
    tcpip_init( NULL, NULL );
}

void
vMBServerTask( void *arg )
{
    eMBErrorCode    xStatus;
    struct ip_addr  xIpAddr, xNetMast, xGateway;
    struct netif    xFEC523x;

    IP4_ADDR( &xIpAddr, 10, 0, 10, 2 );
    IP4_ADDR( &xNetMast, 255, 255, 255, 0 );
    IP4_ADDR( &xGateway, 10, 0, 10, 1 );
    netif_add( &xFEC523x, &xIpAddr, &xNetMast, &xGateway, NULL, mcf523xfec_init, tcpip_input );
    /* Make it the default interface */
    netif_set_default( &xFEC523x );
    /* Bring it up */
    netif_set_up( &xFEC523x );


    for( ;; )
    {
        if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
        {
            fprintf( stderr, "%s: can't initialize modbus stack!\r\n", PROG );
        }
        else if( eMBEnable(  ) != MB_ENOERR )
        {
            fprintf( stderr, "%s: can't enable modbus stack!\r\n", PROG );
        }
        else
        {
            do
            {
                xStatus = eMBPoll(  );
            }
            while( xStatus == MB_ENOERR );
        }
        /* An error occured. Maybe we can restart. */
        ( void )eMBDisable(  );
        ( void )eMBClose(  );
    }
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
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
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
