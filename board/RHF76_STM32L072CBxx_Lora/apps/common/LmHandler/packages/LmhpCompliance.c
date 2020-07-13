/*!
 * \file      LmhpCompliance.c
 *
 * \brief     Implements the LoRa-Alliance certification protocol handling
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "utilities.h"
#include "timer.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
#include "Region.h"
#include "LmhPackage.h"
#include "LmhpCompliance.h"

/*!
 * LoRaWAN compliance certification protocol port number.
 *
 * LoRaWAN Specification V1.0.2, chapter 4.3.2
 */
#define COMPLIANCE_PORT                             224

/*!
 * Defines the compliance mode data transmission duty cycle.
 * An uplink will be transmitted ever \ref COMPLIANCE_TX_DUTYCYCLE [ms].
 */
#define COMPLIANCE_TX_DUTYCYCLE                     5000

/*!
 * LoRaWAN compliance tests support data
 */
typedef struct ComplianceTestState_s
{
    bool Initialized;
    bool IsRunning;
    uint8_t State;
    bool TxPending;
    bool IsTxConfirmed;
    uint8_t Port;
    uint8_t DataBufferMaxSize;
    uint8_t DataBufferSize;
    uint8_t *DataBuffer;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
}ComplianceTestState_t;

/*!
 * Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t ComplianceTxNextPacketTimer;

/*!
 * Holds the compliance test current context
 */
static ComplianceTestState_t ComplianceTestState =
{ 
    .Initialized = false,
    .IsRunning = false,
    .State = 0,
    .TxPending = false,
    .IsTxConfirmed = false,
    .Port = 0,
    .DataBufferMaxSize = 0,
    .DataBufferSize = 0,
    .DataBuffer = NULL,
    .DownLinkCounter = 0,
    .LinkCheck = false,
    .DemodMargin = 0,
    .NbGateways = 0
};

/*!
 * LoRaWAN compliance tests protocol handler parameters
 */
static LmhpComplianceParams_t* LmhpComplianceParams;

/*!
 * Initializes the compliance tests with provided parameters
 *
 * \param [IN] params Structure containing the initial compliance
 *                    tests parameters.
 * \param [IN] dataBuffer        Pointer to main application buffer
 * \param [IN] dataBufferMaxSize Application buffer maximum size
 */
static void LmhpComplianceInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize );

/*!
 * Returns the current compliance certification protocol initialization status.
 *
 * \retval status Compliance certification protocol initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpComplianceIsInitialized( void );

/*!
 * Returns the current compliance certification protocol handling status.
 *
 * \retval status Compliance certification protocol handling status
 *                [true: Running, false: Not running]
 */
static bool LmhpComplianceIsRunning( void );

/*!
 * Processes the LoRaMac Compliance events.
 */
static void LmhpComplianceProcess( void );

/*!
 * Processes the MCPS Indication
 *
 * \param [IN] mcpsIndication     MCPS indication primitive data
 */
static void LmhpComplianceOnMcpsIndication( McpsIndication_t *mcpsIndication );

/*!
 * Processes the MLME Confirm
 *
 * \param [IN] mlmeConfirm MLME confirmation primitive data
 */
static void LmhpComplianceOnMlmeConfirm( MlmeConfirm_t *mlmeConfirm );

/*!
 * Function executed on TxNextPacket Timeout event
 */
static void OnComplianceTxNextPacketTimerEvent( void *context );

/*!
 * Processes the data to transmit on port \ref COMPLIANCE_PORT
 * Handles the compliance certification protocol data transmission
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
static LmHandlerErrorStatus_t LmhpComplianceTxProcess( void );

LmhPackage_t LmhpCompliancePackage =
{
    .Port = COMPLIANCE_PORT,
    .Init = LmhpComplianceInit,
    .IsInitialized = LmhpComplianceIsInitialized,
    .IsRunning = LmhpComplianceIsRunning,
    .Process = LmhpComplianceProcess,
    .OnMcpsConfirmProcess = NULL,                              // Not used in this package
    .OnMcpsIndicationProcess = LmhpComplianceOnMcpsIndication,
    .OnMlmeConfirmProcess = LmhpComplianceOnMlmeConfirm,
    .OnMlmeIndicationProcess = NULL,                           // Not used in this package
    .OnMacMcpsRequest = NULL,                                  // To be initialized by LmHandler
    .OnMacMlmeRequest = NULL,                                  // To be initialized by LmHandler
    .OnJoinRequest = NULL,                                     // To be initialized by LmHandler
    .OnSendRequest = NULL,                                     // To be initialized by LmHandler
    .OnDeviceTimeRequest = NULL,                               // To be initialized by LmHandler
    .OnSysTimeUpdate = NULL,                                   // To be initialized by LmHandler
};

LmhPackage_t *LmphCompliancePackageFactory( void )
{
    return &LmhpCompliancePackage;
}

static void LmhpComplianceInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize )
{
    if( ( params != NULL ) && ( dataBuffer != NULL ) )
    {
        LmhpComplianceParams = ( LmhpComplianceParams_t* )params;
        ComplianceTestState.DataBuffer = dataBuffer;
        ComplianceTestState.DataBufferMaxSize = dataBufferMaxSize;
        ComplianceTestState.Initialized = true;
    }
    else
    {
        LmhpComplianceParams = NULL;
        ComplianceTestState.Initialized = false;
    }
}

static bool LmhpComplianceIsInitialized( void )
{
    return ComplianceTestState.Initialized;
}

static bool LmhpComplianceIsRunning( void )
{
    if( ComplianceTestState.Initialized == false )
    {
        return false;
    }

    return ComplianceTestState.IsRunning;
}

static void LmhpComplianceOnMlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    if( ComplianceTestState.Initialized == false )
    {
        return;
    }

    if( mlmeConfirm->MlmeRequest == MLME_LINK_CHECK )
    {
        ComplianceTestState.LinkCheck = true;
        ComplianceTestState.DemodMargin = mlmeConfirm->DemodMargin;
        ComplianceTestState.NbGateways = mlmeConfirm->NbGateways;
    }
}

static LmHandlerErrorStatus_t LmhpComplianceTxProcess( void )
{
    if( ComplianceTestState.Initialized == false )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if( ComplianceTestState.LinkCheck == true )
    {
        ComplianceTestState.LinkCheck = false;
        ComplianceTestState.DataBufferSize = 3;
        ComplianceTestState.DataBuffer[0] = 5;
        ComplianceTestState.DataBuffer[1] = ComplianceTestState.DemodMargin;
        ComplianceTestState.DataBuffer[2] = ComplianceTestState.NbGateways;
        ComplianceTestState.State = 1;
    }
    else
    {
        switch( ComplianceTestState.State )
        {
        case 4:
            ComplianceTestState.State = 1;
            break;
        case 1:
            ComplianceTestState.DataBufferSize = 2;
            ComplianceTestState.DataBuffer[0] = ComplianceTestState.DownLinkCounter >> 8;
            ComplianceTestState.DataBuffer[1] = ComplianceTestState.DownLinkCounter;
            break;
        }
    }

    LmHandlerAppData_t appData =
    {
        .Buffer = ComplianceTestState.DataBuffer,
        .BufferSize = ComplianceTestState.DataBufferSize,
        .Port = COMPLIANCE_PORT
    };

    // Schedule next transmission
    TimerStart( &ComplianceTxNextPacketTimer );

    return LmhpCompliancePackage.OnSendRequest( &appData, ( LmHandlerMsgTypes_t )ComplianceTestState.IsTxConfirmed );
}

static void LmhpComplianceOnMcpsIndication( McpsIndication_t* mcpsIndication )
{
    if( ComplianceTestState.Initialized == false )
    {
        return;
    }

    if( mcpsIndication->RxData == false )
    {
        return;
    }

    if( mcpsIndication->Port != COMPLIANCE_PORT )
    {
        return;
    }

    if( ComplianceTestState.IsRunning == false )
    {
        // Check compliance test enable command (i)
        if( ( mcpsIndication->BufferSize == 4 ) &&
            ( mcpsIndication->Buffer[0] == 0x01 ) &&
            ( mcpsIndication->Buffer[1] == 0x01 ) &&
            ( mcpsIndication->Buffer[2] == 0x01 ) &&
            ( mcpsIndication->Buffer[3] == 0x01 ) )
        {
            MibRequestConfirm_t mibReq;

            // Initialize compliance test mode context
            ComplianceTestState.IsTxConfirmed = false;
            ComplianceTestState.TxPending = false;
            ComplianceTestState.Port = 224;
            ComplianceTestState.DataBufferSize = 2;
            ComplianceTestState.DownLinkCounter = 0;
            ComplianceTestState.LinkCheck = false;
            ComplianceTestState.DemodMargin = 0;
            ComplianceTestState.NbGateways = 0;
            ComplianceTestState.IsRunning = true;
            ComplianceTestState.State = 1;

            // Enable ADR while in compliance test mode
            mibReq.Type = MIB_ADR;
            mibReq.Param.AdrEnable = true;
            LoRaMacMibSetRequestConfirm( &mibReq );

            // Disable duty cycle enforcement while in compliance test mode
            LoRaMacTestSetDutyCycleOn( false );

            // Stop peripherals
            if( LmhpComplianceParams->StopPeripherals != NULL )
            {
                LmhpComplianceParams->StopPeripherals( );
            }
            // Initialize compliance protocol transmission timer
            TimerInit( &ComplianceTxNextPacketTimer, OnComplianceTxNextPacketTimerEvent );
            TimerSetValue( &ComplianceTxNextPacketTimer, COMPLIANCE_TX_DUTYCYCLE );

            // Confirm compliance test protocol activation
            CRITICAL_SECTION_BEGIN( );
            ComplianceTestState.TxPending = true; //LmhpComplianceTxProcess( );
            CRITICAL_SECTION_END( );
        }
    }
    else
    {
        // Increment the compliance certification protocol downlink counter
        ComplianceTestState.DownLinkCounter++;

        // Parse compliance test protocol
        ComplianceTestState.State = mcpsIndication->Buffer[0];
        switch( ComplianceTestState.State )
        {
        case 0: // Check compliance test disable command (ii)
            {
                MibRequestConfirm_t mibReq;

                TimerStop( &ComplianceTxNextPacketTimer );

                // Disable compliance test mode and reset the downlink counter.
                ComplianceTestState.DownLinkCounter = 0;
                ComplianceTestState.IsRunning = false;

                // Restore previous ADR seeting
                mibReq.Type = MIB_ADR;
                mibReq.Param.AdrEnable = LmhpComplianceParams->AdrEnabled;
                LoRaMacMibSetRequestConfirm( &mibReq );

                // Enable duty cycle enforcement
                LoRaMacTestSetDutyCycleOn( LmhpComplianceParams->DutyCycleEnabled );

                // Restart peripherals
                if( LmhpComplianceParams->StartPeripherals != NULL )
                {
                    LmhpComplianceParams->StartPeripherals( );
                }
            }
            break;
        case 1: // (iii, iv)
            ComplianceTestState.DataBufferSize = 2;
            break;
        case 2: // Enable confirmed messages (v)
            ComplianceTestState.IsTxConfirmed = true;
            ComplianceTestState.State = 1;
            break;
        case 3:  // Disable confirmed messages (vi)
            ComplianceTestState.IsTxConfirmed = false;
            ComplianceTestState.State = 1;
            break;
        case 4: // (vii)
            ComplianceTestState.DataBufferSize = mcpsIndication->BufferSize;

            ComplianceTestState.DataBuffer[0] = 4;
            for( uint8_t i = 1; i < MIN( ComplianceTestState.DataBufferSize, ComplianceTestState.DataBufferMaxSize ); i++ )
            {
                ComplianceTestState.DataBuffer[i] = mcpsIndication->Buffer[i] + 1;
            }
            break;
        case 5: // (viii)
            {
                MlmeReq_t mlmeReq;

                mlmeReq.Type = MLME_LINK_CHECK;

                LmhpCompliancePackage.OnMacMlmeRequest( LoRaMacMlmeRequest( &mlmeReq ), &mlmeReq, 0 );
            }
            break;
        case 6: // (ix)
            {
                MibRequestConfirm_t mibReq;

                // Disable TestMode and revert back to normal operation
                // Disable compliance test mode and reset the downlink counter.
                ComplianceTestState.DownLinkCounter = 0;
                ComplianceTestState.IsRunning = false;

                // Restore previous ADR seeting
                mibReq.Type = MIB_ADR;
                mibReq.Param.AdrEnable = LmhpComplianceParams->AdrEnabled;
                LoRaMacMibSetRequestConfirm( &mibReq );

                // Enable duty cycle enforcement
                LoRaMacTestSetDutyCycleOn( LmhpComplianceParams->DutyCycleEnabled );

                // Restart peripherals
                if( LmhpComplianceParams->StartPeripherals != NULL )
                {
                    LmhpComplianceParams->StartPeripherals( );
                }

                LmhpCompliancePackage.OnJoinRequest( true );
            }
            break;
        case 7: // (x)
            {
                MlmeReq_t mlmeReq;
                if( mcpsIndication->BufferSize == 3 )
                {
                    mlmeReq.Type = MLME_TXCW;
                    mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                }
                else if( mcpsIndication->BufferSize == 7 )
                {
                    mlmeReq.Type = MLME_TXCW_1;
                    mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                    mlmeReq.Req.TxCw.Frequency = ( uint32_t )( ( mcpsIndication->Buffer[3] << 16 ) | ( mcpsIndication->Buffer[4] << 8 ) | mcpsIndication->Buffer[5] ) * 100;
                    mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[6];
                }
                LmhpCompliancePackage.OnMacMlmeRequest( LoRaMacMlmeRequest( &mlmeReq ), &mlmeReq, 0 );
                ComplianceTestState.State = 1;
            }
            break;
        case 8: // Send DeviceTimeReq
            {
                MlmeReq_t mlmeReq;

                mlmeReq.Type = MLME_DEVICE_TIME;

                LmhpCompliancePackage.OnMacMlmeRequest( LoRaMacMlmeRequest( &mlmeReq ), &mlmeReq, 0 );
            }
            break;
        case 9: // Switch end device Class
            {
                MibRequestConfirm_t mibReq;

                mibReq.Type = MIB_DEVICE_CLASS;
                // CLASS_A = 0, CLASS_B = 1, CLASS_C = 2
                mibReq.Param.Class = ( DeviceClass_t )mcpsIndication->Buffer[1];;
                LoRaMacMibSetRequestConfirm( &mibReq );
            }
            break;
        case 10: // Send PingSlotInfoReq
            {
                MlmeReq_t mlmeReq;

                mlmeReq.Type = MLME_PING_SLOT_INFO;
                mlmeReq.Req.PingSlotInfo.PingSlot.Value = mcpsIndication->Buffer[1];

                LmhpCompliancePackage.OnMacMlmeRequest( LoRaMacMlmeRequest( &mlmeReq ), &mlmeReq, 0 );
            }
            break;
        default:
            break;
        }
    }
}

static void LmhpComplianceProcess( void )
{
    bool isPending;

    CRITICAL_SECTION_BEGIN( );
    isPending = ComplianceTestState.TxPending;
    ComplianceTestState.TxPending = false;
    CRITICAL_SECTION_END( );
    if( isPending == true )
    {
        LmhpComplianceTxProcess( );
    }
}

static void OnComplianceTxNextPacketTimerEvent( void* context )
{
    ComplianceTestState.TxPending = true;
}
