/*!
 * \file      LmHandlerMsgDisplay.h
 *
 * \brief     Common set of functions to display default messages from
 *            LoRaMacHandler.
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
 *              (C)2013-2019 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "utilities.h"
#include "timer.h"

#include "LmHandlerMsgDisplay.h"

/*!
 * MAC status strings
 */
const char* MacStatusStrings[] =
{
    "OK",                            // LORAMAC_STATUS_OK
    "Busy",                          // LORAMAC_STATUS_BUSY
    "Service unknown",               // LORAMAC_STATUS_SERVICE_UNKNOWN
    "Parameter invalid",             // LORAMAC_STATUS_PARAMETER_INVALID
    "Frequency invalid",             // LORAMAC_STATUS_FREQUENCY_INVALID
    "Datarate invalid",              // LORAMAC_STATUS_DATARATE_INVALID
    "Frequency or datarate invalid", // LORAMAC_STATUS_FREQ_AND_DR_INVALID
    "No network joined",             // LORAMAC_STATUS_NO_NETWORK_JOINED
    "Length error",                  // LORAMAC_STATUS_LENGTH_ERROR
    "Region not supported",          // LORAMAC_STATUS_REGION_NOT_SUPPORTED
    "Skipped APP data",              // LORAMAC_STATUS_SKIPPED_APP_DATA
    "Duty-cycle restricted",         // LORAMAC_STATUS_DUTYCYCLE_RESTRICTED
    "No channel found",              // LORAMAC_STATUS_NO_CHANNEL_FOUND
    "No free channel found",         // LORAMAC_STATUS_NO_FREE_CHANNEL_FOUND
    "Busy beacon reserved time",     // LORAMAC_STATUS_BUSY_BEACON_RESERVED_TIME
    "Busy ping-slot window time",    // LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME
    "Busy uplink collision",         // LORAMAC_STATUS_BUSY_UPLINK_COLLISION
    "Crypto error",                  // LORAMAC_STATUS_CRYPTO_ERROR
    "FCnt handler error",            // LORAMAC_STATUS_FCNT_HANDLER_ERROR
    "MAC command error",             // LORAMAC_STATUS_MAC_COMMAD_ERROR
    "ClassB error",                  // LORAMAC_STATUS_CLASS_B_ERROR
    "Confirm queue error",           // LORAMAC_STATUS_CONFIRM_QUEUE_ERROR
    "Multicast group undefined",     // LORAMAC_STATUS_MC_GROUP_UNDEFINED
    "Unknown error",                 // LORAMAC_STATUS_ERROR
};

/*!
 * MAC event info status strings.
 */
const char* EventInfoStatusStrings[] =
{ 
    "OK",                            // LORAMAC_EVENT_INFO_STATUS_OK
    "Error",                         // LORAMAC_EVENT_INFO_STATUS_ERROR
    "Tx timeout",                    // LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT
    "Rx 1 timeout",                  // LORAMAC_EVENT_INFO_STATUS_RX1_TIMEOUT
    "Rx 2 timeout",                  // LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT
    "Rx1 error",                     // LORAMAC_EVENT_INFO_STATUS_RX1_ERROR
    "Rx2 error",                     // LORAMAC_EVENT_INFO_STATUS_RX2_ERROR
    "Join failed",                   // LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL
    "Downlink repeated",             // LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED
    "Tx DR payload size error",      // LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR
    "Downlink too many frames loss", // LORAMAC_EVENT_INFO_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS
    "Address fail",                  // LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL
    "MIC fail",                      // LORAMAC_EVENT_INFO_STATUS_MIC_FAIL
    "Multicast fail",                // LORAMAC_EVENT_INFO_STATUS_MULTICAST_FAIL
    "Beacon locked",                 // LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED
    "Beacon lost",                   // LORAMAC_EVENT_INFO_STATUS_BEACON_LOST
    "Beacon not found"               // LORAMAC_EVENT_INFO_STATUS_BEACON_NOT_FOUND
};

/*!
 * Prints the provided buffer in HEX
 * 
 * \param buffer Buffer to be printed
 * \param size   Buffer size to be printed
 */
void PrintHexBuffer( uint8_t *buffer, uint8_t size )
{
    uint8_t newline = 0;

    for( uint8_t i = 0; i < size; i++ )
    {
        if( newline != 0 )
        {
            printf( "\n" );
            newline = 0;
        }

        printf( "%02X ", buffer[i] );

        if( ( ( i + 1 ) % 16 ) == 0 )
        {
            newline = 1;
        }
    }
    printf( "\n" );
}

void DisplayNvmContextChange( LmHandlerNvmContextStates_t state )
{
    if( state == LORAMAC_HANDLER_NVM_STORE )
    {
        printf( "\n###### ============ CTXS STORED ============ ######\n\n" );
    }
    else
    {
        printf( "\n###### =========== CTXS RESTORED =========== ######\n\n" );
    }
}

void DisplayNetworkParametersUpdate( CommissioningParams_t *commissioningParams )
{
    printf( "DevEui      : %02X", commissioningParams->DevEui[0] );
    for( int i = 1; i < 8; i++ )
    {
        printf( "-%02X", commissioningParams->DevEui[i] );
    }
    printf( "\n" );
    printf( "JoinEui     : %02X", commissioningParams->JoinEui[0] );
    for( int i = 1; i < 8; i++ )
    {
        printf( "-%02X", commissioningParams->JoinEui[i] );
    }
    printf( "\n" );
    printf( "Pin         : %02X", commissioningParams->SePin[0] );
    for( int i = 1; i < 4; i++ )
    {
        printf( "-%02X", commissioningParams->SePin[i] );
    }
    printf( "\n\n" );
}

void DisplayMacMcpsRequestUpdate( LoRaMacStatus_t status, McpsReq_t *mcpsReq, TimerTime_t nextTxIn )
{
    switch( mcpsReq->Type )
    {
        case MCPS_CONFIRMED:
        {
            printf( "\n###### =========== MCPS-Request ============ ######\n" );
            printf( "######            MCPS_CONFIRMED             ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        case MCPS_UNCONFIRMED:
        {
            printf( "\n###### =========== MCPS-Request ============ ######\n" );
            printf( "######           MCPS_UNCONFIRMED            ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        case MCPS_PROPRIETARY:
        {
            printf( "\n###### =========== MCPS-Request ============ ######\n" );
            printf( "######           MCPS_PROPRIETARY            ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        default:
        {
            printf( "\n###### =========== MCPS-Request ============ ######\n" );
            printf( "######                MCPS_ERROR             ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
    }
    printf( "STATUS      : %s\n", MacStatusStrings[status] );
    if( status == LORAMAC_STATUS_DUTYCYCLE_RESTRICTED )
    {
        printf( "Next Tx in  : %lu [ms]\n", nextTxIn );
    }
}

void DisplayMacMlmeRequestUpdate( LoRaMacStatus_t status, MlmeReq_t *mlmeReq, TimerTime_t nextTxIn )
{
    switch( mlmeReq->Type )
    {
        case MLME_JOIN:
        {
            printf( "\n###### =========== MLME-Request ============ ######\n" );
            printf( "######               MLME_JOIN               ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        case MLME_LINK_CHECK:
        {
            printf( "\n###### =========== MLME-Request ============ ######\n" );
            printf( "######            MLME_LINK_CHECK            ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        case MLME_DEVICE_TIME:
        {
            printf( "\n###### =========== MLME-Request ============ ######\n" );
            printf( "######            MLME_DEVICE_TIME           ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        case MLME_TXCW:
        {
            printf( "\n###### =========== MLME-Request ============ ######\n" );
            printf( "######               MLME_TXCW               ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        case MLME_TXCW_1:
        {
            printf( "\n###### =========== MLME-Request ============ ######\n" );
            printf( "######               MLME_TXCW_1             ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
        default:
        {
            printf( "\n###### =========== MLME-Request ============ ######\n" );
            printf( "######              MLME_UNKNOWN             ######\n");
            printf( "###### ===================================== ######\n");
            break;
        }
    }
    printf( "STATUS      : %s\n", MacStatusStrings[status] );
    if( status == LORAMAC_STATUS_DUTYCYCLE_RESTRICTED )
    {
        printf( "Next Tx in  : %lu [ms]\n", nextTxIn );
    }
}

void DisplayJoinRequestUpdate( LmHandlerJoinParams_t *params )
{
    if( params->CommissioningParams->IsOtaaActivation == true )
    {
        if( params->Status == LORAMAC_HANDLER_SUCCESS )
        {
            printf( "###### ===========   JOINED     ============ ######\n" );
            printf( "\nOTAA\n\n" );
            printf( "DevAddr     :  %08lX\n", params->CommissioningParams->DevAddr );
            printf( "\n\n" );
            printf( "DATA RATE   : DR_%d\n\n", params->Datarate );
        }
    }
#if ( OVER_THE_AIR_ACTIVATION == 0 )
    else
    {
        printf( "###### ===========   JOINED     ============ ######\n" );
        printf( "\nABP\n\n" );
        printf( "DevAddr     : %08lX\n", params->CommissioningParams->DevAddr );
        printf( "\n\n" );
    }
#endif
}

void DisplayTxUpdate( LmHandlerTxParams_t *params )
{
    MibRequestConfirm_t mibGet;

    if( params->IsMcpsConfirm == 0 )
    {
        printf( "\n###### =========== MLME-Confirm ============ ######\n" );
        printf( "STATUS      : %s\n", EventInfoStatusStrings[params->Status] );
        return;
    }

    printf( "\n###### =========== MCPS-Confirm ============ ######\n" );
    printf( "STATUS      : %s\n", EventInfoStatusStrings[params->Status] );

    printf( "\n###### =====   UPLINK FRAME %8lu   ===== ######\n", params->UplinkCounter );
    printf( "\n" );

    printf( "CLASS       : %c\n", "ABC"[LmHandlerGetCurrentClass( )] );
    printf( "\n" );
    printf( "TX PORT     : %d\n", params->AppData.Port );

    if( params->AppData.BufferSize != 0 )
    {
        printf( "TX DATA     : " );
        if( params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG )
        {
            printf( "CONFIRMED - %s\n", ( params->AckReceived != 0 ) ? "ACK" : "NACK" );
        }
        else
        {
            printf( "UNCONFIRMED\n" );
        }
        PrintHexBuffer( params->AppData.Buffer, params->AppData.BufferSize );
    }

    printf( "\n" );
    printf( "DATA RATE   : DR_%d\n", params->Datarate );

    mibGet.Type  = MIB_CHANNELS;
    if( LoRaMacMibGetRequestConfirm( &mibGet ) == LORAMAC_STATUS_OK )
    {
        printf( "U/L FREQ    : %lu\n", mibGet.Param.ChannelList[params->Channel].Frequency );
    }

    printf( "TX POWER    : %d\n", params->TxPower );

    mibGet.Type  = MIB_CHANNELS_MASK;
    if( LoRaMacMibGetRequestConfirm( &mibGet ) == LORAMAC_STATUS_OK )
    {
        printf("CHANNEL MASK: ");
        switch( LmHandlerGetActiveRegion( ) )
        {
            case LORAMAC_REGION_AS923:
            case LORAMAC_REGION_CN779:
            case LORAMAC_REGION_EU868:
            case LORAMAC_REGION_IN865:
            case LORAMAC_REGION_KR920:
            case LORAMAC_REGION_EU433:
            case LORAMAC_REGION_RU864:
            {
                printf( "%04X ", mibGet.Param.ChannelsMask[0] );
                break;
            }
            case LORAMAC_REGION_AU915:
            case LORAMAC_REGION_CN470:
            case LORAMAC_REGION_US915:
            {
                for( uint8_t i = 0; i < 5; i++)
                {
                    printf( "%04X ", mibGet.Param.ChannelsMask[i] );
                }
                break;
            }
            default:
            {
                printf( "\n###### ========= Unknown Region ============ ######" );
                break;
            }
        }
        printf("\n");
    }

    printf( "\n" );
}

void DisplayRxUpdate( LmHandlerAppData_t *appData, LmHandlerRxParams_t *params )
{
    const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };

    if( params->IsMcpsIndication == 0 )
    {
        printf( "\n###### ========== MLME-Indication ========== ######\n" );
        printf( "STATUS      : %s\n", EventInfoStatusStrings[params->Status] );
        return;
    }

    printf( "\n###### ========== MCPS-Indication ========== ######\n" );
    printf( "STATUS      : %s\n", EventInfoStatusStrings[params->Status] );

    printf( "\n###### =====  DOWNLINK FRAME %8lu  ===== ######\n", params->DownlinkCounter );

    printf( "RX WINDOW   : %s\n", slotStrings[params->RxSlot] );
    
    printf( "RX PORT     : %d\n", appData->Port );

    if( appData->BufferSize != 0 )
    {
        printf( "RX DATA     : \n" );
        PrintHexBuffer( appData->Buffer, appData->BufferSize );
    }

    printf( "\n" );
    printf( "DATA RATE   : DR_%d\n", params->Datarate );
    printf( "RX RSSI     : %d\n", params->Rssi );
    printf( "RX SNR      : %d\n", params->Snr );

    printf( "\n" );
}

void DisplayBeaconUpdate( LoRaMAcHandlerBeaconParams_t *params )
{
    switch( params->State )
    {
        default:
        case LORAMAC_HANDLER_BEACON_ACQUIRING:
        {
            printf( "\n###### ========= BEACON ACQUIRING ========== ######\n" );
            break;
        }
        case LORAMAC_HANDLER_BEACON_LOST:
        {
            printf( "\n###### ============ BEACON LOST ============ ######\n" );
            break;
        }
        case LORAMAC_HANDLER_BEACON_RX:
        {
            printf( "\n###### ===== BEACON %8lu ==== ######\n", params->Info.Time.Seconds );
            printf( "GW DESC     : %d\n", params->Info.GwSpecific.InfoDesc );
            printf( "GW INFO     : " );
            PrintHexBuffer( params->Info.GwSpecific.Info, 6 );
            printf( "\n" );
            printf( "FREQ        : %lu\n", params->Info.Frequency );
            printf( "DATA RATE   : DR_%d\n", params->Info.Datarate );
            printf( "RX RSSI     : %d\n", params->Info.Rssi );
            printf( "RX SNR      : %d\n", params->Info.Snr );
            printf( "\n" );
            break;
        }
        case LORAMAC_HANDLER_BEACON_NRX:
        {
            printf( "\n###### ======== BEACON NOT RECEIVED ======== ######\n" );
            break;
        }
    }
}

void DisplayClassUpdate( DeviceClass_t deviceClass )
{
    printf( "\n\n###### ===== Switch to Class %c done.  ===== ######\n\n", "ABC"[deviceClass] );
}

void DisplayAppInfo( const char* appName, const Version_t* appVersion, const Version_t* gitHubVersion )
{
    printf( "\n###### ===================================== ######\n\n" );
    printf( "Application name   : %s\n", appName );
    printf( "Application version: %d.%d.%d\n", appVersion->Fields.Major, appVersion->Fields.Minor, appVersion->Fields.Patch );
    printf( "GitHub base version: %d.%d.%d\n", gitHubVersion->Fields.Major, gitHubVersion->Fields.Minor, gitHubVersion->Fields.Patch );
    printf( "\n###### ===================================== ######\n\n" );
}
