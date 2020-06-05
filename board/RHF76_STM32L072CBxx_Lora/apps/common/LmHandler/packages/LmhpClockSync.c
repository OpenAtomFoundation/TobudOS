/*!
 * \file      LmhpClockSync.c
 *
 * \brief     Implements the LoRa-Alliance clock synchronization package
 *            Specification: https://lora-alliance.org/sites/default/files/2018-09/application_layer_clock_synchronization_v1.0.0.pdf
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
#include "systime.h"
#include "LmHandler.h"
#include "LmhpClockSync.h"

/*!
 * LoRaWAN Application Layer Clock Synchronization Specification
 */
#define CLOCK_SYNC_PORT                             202

#define CLOCK_SYNC_ID                               1
#define CLOCK_SYNC_VERSION                          1

/*!
 * Package current context
 */
typedef struct LmhpClockSyncState_s
{
    bool Initialized;
    bool IsRunning;
    uint8_t DataBufferMaxSize;
    uint8_t *DataBuffer;
    union
    {
        uint8_t Value;
        struct
        {
            uint8_t TokenReq:    4;
            uint8_t AnsRequired: 1;
            uint8_t RFU:         3;
        }Fields;
    }TimeReqParam;
    bool AppTimeReqPending;
    bool AdrEnabledPrev;
    uint8_t NbTransPrev;
    uint8_t DataratePrev;
    uint8_t NbTransmissions;
}LmhpClockSyncState_t;

typedef enum LmhpClockSyncMoteCmd_e
{
    CLOCK_SYNC_PKG_VERSION_ANS       = 0x00,
    CLOCK_SYNC_APP_TIME_REQ          = 0x01,
    CLOCK_SYNC_APP_TIME_PERIOD_ANS   = 0x02,
    CLOCK_SYNC_FORCE_RESYNC_ANS      = 0x03,
}LmhpClockSyncMoteCmd_t;

typedef enum LmhpClockSyncSrvCmd_e
{
    CLOCK_SYNC_PKG_VERSION_REQ       = 0x00,
    CLOCK_SYNC_APP_TIME_ANS          = 0x01,
    CLOCK_SYNC_APP_TIME_PERIOD_REQ   = 0x02,
    CLOCK_SYNC_FORCE_RESYNC_REQ      = 0x03,
}LmhpClockSyncSrvCmd_t;

/*!
 * Initializes the package with provided parameters
 *
 * \param [IN] params            Pointer to the package parameters
 * \param [IN] dataBuffer        Pointer to main application buffer
 * \param [IN] dataBufferMaxSize Main application buffer maximum size
 */
static void LmhpClockSyncInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize );

/*!
 * Returns the current package initialization status.
 *
 * \retval status Package initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpClockSyncIsInitialized( void );

/*!
 * Returns the package operation status.
 *
 * \retval status Package operation status
 *                [true: Running, false: Not running]
 */
static bool LmhpClockSyncIsRunning( void );

/*!
 * Processes the internal package events.
 */
static void LmhpClockSyncProcess( void );

/*!
 * Processes the MCSP Confirm
 *
 * \param [IN] mcpsConfirm MCPS confirmation primitive data
 */
static void LmhpClockSyncOnMcpsConfirm( McpsConfirm_t *mcpsConfirm );

/*!
 * Processes the MCPS Indication
 *
 * \param [IN] mcpsIndication     MCPS indication primitive data
 */
static void LmhpClockSyncOnMcpsIndication( McpsIndication_t *mcpsIndication );

static LmhpClockSyncState_t LmhpClockSyncState =
{
    .Initialized = false,
    .IsRunning = false,
    .TimeReqParam.Value = 0,
    .AppTimeReqPending = false,
    .AdrEnabledPrev = false,
    .NbTransPrev = 0,
    .NbTransmissions = 0,
};

static LmhPackage_t LmhpClockSyncPackage =
{
    .Port = CLOCK_SYNC_PORT,
    .Init = LmhpClockSyncInit,
    .IsInitialized = LmhpClockSyncIsInitialized,
    .IsRunning = LmhpClockSyncIsRunning,
    .Process = LmhpClockSyncProcess,
    .OnMcpsConfirmProcess = LmhpClockSyncOnMcpsConfirm,
    .OnMcpsIndicationProcess = LmhpClockSyncOnMcpsIndication,
    .OnMlmeConfirmProcess = NULL,                              // Not used in this package
    .OnMlmeIndicationProcess = NULL,                           // Not used in this package
    .OnMacMcpsRequest = NULL,                                  // To be initialized by LmHandler
    .OnMacMlmeRequest = NULL,                                  // To be initialized by LmHandler
    .OnJoinRequest = NULL,                                     // To be initialized by LmHandler
    .OnSendRequest = NULL,                                     // To be initialized by LmHandler
    .OnDeviceTimeRequest = NULL,                               // To be initialized by LmHandler
    .OnSysTimeUpdate = NULL,                                   // To be initialized by LmHandler
};

LmhPackage_t *LmphClockSyncPackageFactory( void )
{
    return &LmhpClockSyncPackage;
}

static void LmhpClockSyncInit( void * params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize )
{
    if( dataBuffer != NULL )
    {
        LmhpClockSyncState.DataBuffer = dataBuffer;
        LmhpClockSyncState.DataBufferMaxSize = dataBufferMaxSize;
        LmhpClockSyncState.Initialized = true;
        LmhpClockSyncState.IsRunning = true;
    }
    else
    {
        LmhpClockSyncState.IsRunning = false;
        LmhpClockSyncState.Initialized = false;
    }
}

static bool LmhpClockSyncIsInitialized( void )
{
    return LmhpClockSyncState.Initialized;
}

static bool LmhpClockSyncIsRunning( void )
{
    if( LmhpClockSyncState.Initialized == false )
    {
        return false;
    }

    return LmhpClockSyncState.IsRunning;
}

static void LmhpClockSyncProcess( void )
{
    if( LmhpClockSyncState.NbTransmissions > 0 )
    {
        if( LmhpClockSyncAppTimeReq( ) == LORAMAC_HANDLER_SUCCESS )
        {
            LmhpClockSyncState.NbTransmissions--;
        }
    }
}

static void LmhpClockSyncOnMcpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    MibRequestConfirm_t mibReq;

    if( LmhpClockSyncState.AppTimeReqPending == true )
    {
        // Revert ADR setting
        mibReq.Type = MIB_ADR;
        mibReq.Param.AdrEnable = LmhpClockSyncState.AdrEnabledPrev;
        LoRaMacMibSetRequestConfirm( &mibReq );

        // Revert NbTrans setting
        mibReq.Type = MIB_CHANNELS_NB_TRANS;
        mibReq.Param.ChannelsNbTrans = LmhpClockSyncState.NbTransPrev;
        LoRaMacMibSetRequestConfirm( &mibReq );

        // Revert data rate setting
        mibReq.Type = MIB_CHANNELS_DATARATE;
        mibReq.Param.ChannelsDatarate = LmhpClockSyncState.DataratePrev;
        LoRaMacMibSetRequestConfirm( &mibReq );        
        
        LmhpClockSyncState.AppTimeReqPending = false;
    }
}

static void LmhpClockSyncOnMcpsIndication( McpsIndication_t *mcpsIndication )
{
    uint8_t cmdIndex = 0;
    uint8_t dataBufferIndex = 0;

    while( cmdIndex < mcpsIndication->BufferSize )
    {
        switch( mcpsIndication->Buffer[cmdIndex++] )
        {
            case CLOCK_SYNC_PKG_VERSION_REQ:
            {
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = CLOCK_SYNC_PKG_VERSION_ANS;
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = CLOCK_SYNC_ID;
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = CLOCK_SYNC_VERSION;
                break;
            }
            case CLOCK_SYNC_APP_TIME_ANS:
            {
                LmhpClockSyncState.NbTransmissions = 0;

                // Check if a more precise time correction has been received.
                // If yes then don't process and ignore this answer.
                if( mcpsIndication->DeviceTimeAnsReceived == true )
                {
                    cmdIndex += 5;
                    break;
                }
                int32_t timeCorrection = 0;
                timeCorrection  = ( mcpsIndication->Buffer[cmdIndex++] << 0  ) & 0x000000FF;
                timeCorrection += ( mcpsIndication->Buffer[cmdIndex++] << 8  ) & 0x0000FF00;
                timeCorrection += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                timeCorrection += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;
                if( ( mcpsIndication->Buffer[cmdIndex++] & 0x0F ) == LmhpClockSyncState.TimeReqParam.Fields.TokenReq )
                {
                    SysTime_t curTime = { .Seconds = 0, .SubSeconds = 0 };
                    curTime = SysTimeGet( );
                    curTime.Seconds += timeCorrection;
                    SysTimeSet( curTime );
                    LmhpClockSyncState.TimeReqParam.Fields.TokenReq = ( LmhpClockSyncState.TimeReqParam.Fields.TokenReq + 1 ) & 0x0F;
                    if( LmhpClockSyncPackage.OnSysTimeUpdate != NULL )
                    {
#if( LMH_SYS_TIME_UPDATE_NEW_API == 1 )
                        LmhpClockSyncPackage.OnSysTimeUpdate( 
                                        ( timeCorrection >= -1 ) && ( timeCorrection <= 1 ),
                                        timeCorrection );
#else
                        if( ( timeCorrection >= -1 ) && ( timeCorrection <= 1 ) )
                        {
                            LmhpClockSyncPackage.OnSysTimeUpdate( );
                        }
#endif
                    }
                }
                break;
            }
            case CLOCK_SYNC_APP_TIME_PERIOD_REQ:
            {
                // Increment index
                cmdIndex++;
                // TODO implement command prosessing and handling
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = CLOCK_SYNC_APP_TIME_PERIOD_ANS;
                // Answer status not supported.
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = 0x01;

                SysTime_t curTime = SysTimeGet( );
                // Substract Unix to Gps epcoh offset. The system time is based on Unix time.
                curTime.Seconds -= UNIX_GPS_EPOCH_OFFSET;
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 0  ) & 0xFF;
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 8  ) & 0xFF;
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 16 ) & 0xFF;
                LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 24 ) & 0xFF;
                break;
            }
            case CLOCK_SYNC_FORCE_RESYNC_REQ:
            {
                LmhpClockSyncState.NbTransmissions = mcpsIndication->Buffer[cmdIndex++] & 0X07;
                break;
            }
        }
    }

    if( dataBufferIndex != 0 )
    {
        // Answer commands
        LmHandlerAppData_t appData =
        {
            .Buffer = LmhpClockSyncState.DataBuffer,
            .BufferSize = dataBufferIndex,
            .Port = CLOCK_SYNC_PORT
        };
        LmhpClockSyncPackage.OnSendRequest( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG );
    }
}

LmHandlerErrorStatus_t LmhpClockSyncAppTimeReq( void )
{
    if( LmHandlerIsBusy( ) == true )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if( LmhpClockSyncState.AppTimeReqPending == false )
    {
        MibRequestConfirm_t mibReq;

        // Disable ADR
        mibReq.Type = MIB_ADR;
        LoRaMacMibGetRequestConfirm( &mibReq );
        LmhpClockSyncState.AdrEnabledPrev = mibReq.Param.AdrEnable;
        mibReq.Param.AdrEnable = false;
        LoRaMacMibSetRequestConfirm( &mibReq );

        // Set NbTrans = 1
        mibReq.Type = MIB_CHANNELS_NB_TRANS;
        LoRaMacMibGetRequestConfirm( &mibReq );
        LmhpClockSyncState.NbTransPrev = mibReq.Param.ChannelsNbTrans;
        mibReq.Param.ChannelsNbTrans = 1;
        LoRaMacMibSetRequestConfirm( &mibReq );

        // Store data rate
        mibReq.Type = MIB_CHANNELS_DATARATE;
        LoRaMacMibGetRequestConfirm( &mibReq );  
        LmhpClockSyncState.DataratePrev = mibReq.Param.ChannelsDatarate;

        // Add DeviceTimeReq MAC command.
        // In case the network server supports this more precise command
        // this package will use DeviceTimeAns answer as clock synchronization
        // mechanism.
        LmhpClockSyncPackage.OnDeviceTimeRequest( );
    }

    SysTime_t curTime = SysTimeGet( );
    uint8_t dataBufferIndex = 0;

    // Substract Unix to Gps epcoh offset. The system time is based on Unix time.
    curTime.Seconds -= UNIX_GPS_EPOCH_OFFSET;

    LmhpClockSyncState.DataBuffer[dataBufferIndex++] = CLOCK_SYNC_APP_TIME_REQ;
    LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 0  ) & 0xFF;
    LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 8  ) & 0xFF;
    LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 16 ) & 0xFF;
    LmhpClockSyncState.DataBuffer[dataBufferIndex++] = ( curTime.Seconds >> 24 ) & 0xFF;
    LmhpClockSyncState.TimeReqParam.Fields.AnsRequired = 0;
    LmhpClockSyncState.DataBuffer[dataBufferIndex++] = LmhpClockSyncState.TimeReqParam.Value;

    LmHandlerAppData_t appData =
    {
        .Buffer = LmhpClockSyncState.DataBuffer,
        .BufferSize = dataBufferIndex,
        .Port = CLOCK_SYNC_PORT
    };
    LmhpClockSyncState.AppTimeReqPending = true;
    return LmhpClockSyncPackage.OnSendRequest( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG );
}
