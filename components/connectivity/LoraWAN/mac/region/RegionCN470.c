/*!
 * \file      RegionCN470.c
 *
 * \brief     Region implementation for CN470
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
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
*/
#include "utilities.h"

#include "RegionCommon.h"
#include "RegionCN470.h"

// Definitions
#define CHANNELS_MASK_SIZE              6

/*!
 * Region specific context
 */
typedef struct sRegionCN470NvmCtx
{
    /*!
     * LoRaMAC channels
     */
    ChannelParams_t Channels[ CN470_MAX_NB_CHANNELS ];
    /*!
     * LoRaMac bands
     */
    Band_t Bands[ CN470_MAX_NB_BANDS ];
    /*!
     * LoRaMac channels mask
     */
    uint16_t ChannelsMask[ CHANNELS_MASK_SIZE ];
    /*!
     * LoRaMac channels default mask
     */
    uint16_t ChannelsDefaultMask[ CHANNELS_MASK_SIZE ];
}RegionCN470NvmCtx_t;

/*
 * Non-volatile module context.
 */
static RegionCN470NvmCtx_t NvmCtx;

// Static functions
static int8_t GetNextLowerTxDr( int8_t dr, int8_t minDr )
{
    uint8_t nextLowerDr = 0;

    if( dr == minDr )
    {
        nextLowerDr = minDr;
    }
    else
    {
        nextLowerDr = dr - 1;
    }
    return nextLowerDr;
}

static uint32_t GetBandwidth( uint32_t drIndex )
{
    switch( BandwidthsCN470[drIndex] )
    {
        default:
        case 125000:
            return 0;
        case 250000:
            return 1;
        case 500000:
            return 2;
    }
}

static int8_t LimitTxPower( int8_t txPower, int8_t maxBandTxPower, int8_t datarate, uint16_t* channelsMask )
{
    int8_t txPowerResult = txPower;

    // Limit tx power to the band max
    txPowerResult =  MAX( txPower, maxBandTxPower );

    return txPowerResult;
}

static bool VerifyRfFreq( uint32_t freq )
{
    // Check radio driver support
    if( Radio.CheckRfFrequency( freq ) == false )
    {
        return false;
    }

    // Rx frequencies
    if( ( freq < CN470_FIRST_RX1_CHANNEL ) ||
        ( freq > CN470_LAST_RX1_CHANNEL ) ||
        ( ( ( freq - ( uint32_t ) CN470_FIRST_RX1_CHANNEL ) % ( uint32_t ) CN470_STEPWIDTH_RX1_CHANNEL ) != 0 ) )
    {
        return false;
    }

    // Test for frequency range - take RX and TX freqencies into account
    if( ( freq < 470300000 ) ||  ( freq > 509700000 ) )
    {
        return false;
    }
    return true;
}

static TimerTime_t GetTimeOnAir( int8_t datarate, uint16_t pktLen )
{
    int8_t phyDr = DataratesCN470[datarate];
    uint32_t bandwidth = GetBandwidth( datarate );

    return Radio.TimeOnAir( MODEM_LORA, bandwidth, phyDr, 1, 8, false, pktLen, true );
}

PhyParam_t RegionCN470GetPhyParam( GetPhyParams_t* getPhy )
{
    PhyParam_t phyParam = { 0 };

    switch( getPhy->Attribute )
    {
        case PHY_MIN_RX_DR:
        {
            phyParam.Value = CN470_RX_MIN_DATARATE;
            break;
        }
        case PHY_MIN_TX_DR:
        {
            phyParam.Value = CN470_TX_MIN_DATARATE;
            break;
        }
        case PHY_DEF_TX_DR:
        {
            phyParam.Value = CN470_DEFAULT_DATARATE;
            break;
        }
        case PHY_NEXT_LOWER_TX_DR:
        {
            phyParam.Value = GetNextLowerTxDr( getPhy->Datarate, CN470_TX_MIN_DATARATE );
            break;
        }
        case PHY_MAX_TX_POWER:
        {
            phyParam.Value = CN470_MAX_TX_POWER;
            break;
        }
        case PHY_DEF_TX_POWER:
        {
            phyParam.Value = CN470_DEFAULT_TX_POWER;
            break;
        }
        case PHY_DEF_ADR_ACK_LIMIT:
        {
            phyParam.Value = CN470_ADR_ACK_LIMIT;
            break;
        }
        case PHY_DEF_ADR_ACK_DELAY:
        {
            phyParam.Value = CN470_ADR_ACK_DELAY;
            break;
        }
        case PHY_MAX_PAYLOAD:
        {
            phyParam.Value = MaxPayloadOfDatarateCN470[getPhy->Datarate];
            break;
        }
        case PHY_DUTY_CYCLE:
        {
            phyParam.Value = CN470_DUTY_CYCLE_ENABLED;
            break;
        }
        case PHY_MAX_RX_WINDOW:
        {
            phyParam.Value = CN470_MAX_RX_WINDOW;
            break;
        }
        case PHY_RECEIVE_DELAY1:
        {
            phyParam.Value = CN470_RECEIVE_DELAY1;
            break;
        }
        case PHY_RECEIVE_DELAY2:
        {
            phyParam.Value = CN470_RECEIVE_DELAY2;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY1:
        {
            phyParam.Value = CN470_JOIN_ACCEPT_DELAY1;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY2:
        {
            phyParam.Value = CN470_JOIN_ACCEPT_DELAY2;
            break;
        }
        case PHY_MAX_FCNT_GAP:
        {
            phyParam.Value = CN470_MAX_FCNT_GAP;
            break;
        }
        case PHY_ACK_TIMEOUT:
        {
            phyParam.Value = ( CN470_ACKTIMEOUT + randr( -CN470_ACK_TIMEOUT_RND, CN470_ACK_TIMEOUT_RND ) );
            break;
        }
        case PHY_DEF_DR1_OFFSET:
        {
            phyParam.Value = CN470_DEFAULT_RX1_DR_OFFSET;
            break;
        }
        case PHY_DEF_RX2_FREQUENCY:
        {
            phyParam.Value = CN470_RX_WND_2_FREQ;
            break;
        }
        case PHY_DEF_RX2_DR:
        {
            phyParam.Value = CN470_RX_WND_2_DR;
            break;
        }
        case PHY_CHANNELS_MASK:
        {
            phyParam.ChannelsMask = NvmCtx.ChannelsMask;
            break;
        }
        case PHY_CHANNELS_DEFAULT_MASK:
        {
            phyParam.ChannelsMask = NvmCtx.ChannelsDefaultMask;
            break;
        }
        case PHY_MAX_NB_CHANNELS:
        {
            phyParam.Value = CN470_MAX_NB_CHANNELS;
            break;
        }
        case PHY_CHANNELS:
        {
            phyParam.Channels = NvmCtx.Channels;
            break;
        }
        case PHY_DEF_UPLINK_DWELL_TIME:
        case PHY_DEF_DOWNLINK_DWELL_TIME:
        {
            phyParam.Value = 0;
            break;
        }
        case PHY_DEF_MAX_EIRP:
        {
            phyParam.fValue = CN470_DEFAULT_MAX_EIRP;
            break;
        }
        case PHY_DEF_ANTENNA_GAIN:
        {
            phyParam.fValue = CN470_DEFAULT_ANTENNA_GAIN;
            break;
        }
        case PHY_BEACON_CHANNEL_FREQ:
        {
            phyParam.Value = CN470_BEACON_CHANNEL_FREQ;
            break;
        }
        case PHY_BEACON_FORMAT:
        {
            phyParam.BeaconFormat.BeaconSize = CN470_BEACON_SIZE;
            phyParam.BeaconFormat.Rfu1Size = CN470_RFU1_SIZE;
            phyParam.BeaconFormat.Rfu2Size = CN470_RFU2_SIZE;
            break;
        }
        case PHY_BEACON_CHANNEL_DR:
        {
            phyParam.Value = CN470_BEACON_CHANNEL_DR;
            break;
        }
        case PHY_BEACON_CHANNEL_STEPWIDTH:
        {
            phyParam.Value = CN470_BEACON_CHANNEL_STEPWIDTH;
            break;
        }
        case PHY_BEACON_NB_CHANNELS:
        {
            phyParam.Value = CN470_BEACON_NB_CHANNELS;
            break;
        }
        case PHY_PING_SLOT_CHANNEL_FREQ:
        {
            phyParam.Value = CN470_PING_SLOT_CHANNEL_FREQ;
            break;
        }
        case PHY_PING_SLOT_CHANNEL_DR:
        {
            phyParam.Value = CN470_PING_SLOT_CHANNEL_DR;
            break;
        }
        case PHY_SF_FROM_DR:
        {
            phyParam.Value = DataratesCN470[getPhy->Datarate];
            break;
        }
        case PHY_BW_FROM_DR:
        {
            phyParam.Value = GetBandwidth( getPhy->Datarate );
            break;
        }
        default:
        {
            break;
        }
    }

    return phyParam;
}

void RegionCN470SetBandTxDone( SetBandTxDoneParams_t* txDone )
{
    RegionCommonSetBandTxDone( &NvmCtx.Bands[NvmCtx.Channels[txDone->Channel].Band],
                               txDone->LastTxAirTime, txDone->Joined, txDone->ElapsedTimeSinceStartUp );
}

void RegionCN470InitDefaults( InitDefaultsParams_t* params )
{
    Band_t bands[CN470_MAX_NB_BANDS] =
    {
        CN470_BAND0
    };

    switch( params->Type )
    {
        case INIT_TYPE_BANDS:
        {
            // Initialize bands
            memcpy1( ( uint8_t* )NvmCtx.Bands, ( uint8_t* )bands, sizeof( Band_t ) * CN470_MAX_NB_BANDS );
            break;
        }
        case INIT_TYPE_INIT:
        {
            // Channels
            // 125 kHz channels
            for( uint8_t i = 0; i < CN470_MAX_NB_CHANNELS; i++ )
            {
                NvmCtx.Channels[i].Frequency = 470300000 + i * 200000;
                NvmCtx.Channels[i].DrRange.Value = ( DR_5 << 4 ) | DR_0;
                NvmCtx.Channels[i].Band = 0;
            }

            // Initialize the channels default mask
            #if 1 // set your channels
            NvmCtx.ChannelsDefaultMask[0] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[1] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[2] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[3] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[4] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[5] = 0xFFFF;
            #else
            NvmCtx.ChannelsDefaultMask[0] = 0x0000;
            NvmCtx.ChannelsDefaultMask[1] = 0x0000;
            NvmCtx.ChannelsDefaultMask[2] = 0x0000;
            NvmCtx.ChannelsDefaultMask[3] = 0x0000;
            NvmCtx.ChannelsDefaultMask[4] = 0x0000;
            NvmCtx.ChannelsDefaultMask[5] = 0x00FF;
            #endif
            // Update the channels mask
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, NvmCtx.ChannelsDefaultMask, 6 );
            break;
        }
        case INIT_TYPE_RESTORE_CTX:
        {
            if( params->NvmCtx != 0 )
            {
                memcpy1( (uint8_t*) &NvmCtx, (uint8_t*) params->NvmCtx, sizeof( NvmCtx ) );
            }
            break;
        }
        case INIT_TYPE_RESTORE_DEFAULT_CHANNELS:
        {
            // Restore channels default mask
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, NvmCtx.ChannelsDefaultMask, 6 );
            break;
        }
        default:
        {
            break;
        }
    }
}

void* RegionCN470GetNvmCtx( GetNvmCtxParams_t* params )
{
    params->nvmCtxSize = sizeof( RegionCN470NvmCtx_t );
    return &NvmCtx;
}

bool RegionCN470Verify( VerifyParams_t* verify, PhyAttribute_t phyAttribute )
{
    switch( phyAttribute )
    {
        case PHY_FREQUENCY:
        {
            return VerifyRfFreq( verify->Frequency );
        }
        case PHY_TX_DR:
        case PHY_DEF_TX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, CN470_TX_MIN_DATARATE, CN470_TX_MAX_DATARATE );
        }
        case PHY_RX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, CN470_RX_MIN_DATARATE, CN470_RX_MAX_DATARATE );
        }
        case PHY_DEF_TX_POWER:
        case PHY_TX_POWER:
        {
            // Remark: switched min and max!
            return RegionCommonValueInRange( verify->TxPower, CN470_MAX_TX_POWER, CN470_MIN_TX_POWER );
        }
        case PHY_DUTY_CYCLE:
        {
            return CN470_DUTY_CYCLE_ENABLED;
        }
        default:
            return false;
    }
}

void RegionCN470ApplyCFList( ApplyCFListParams_t* applyCFList )
{
    // Size of the optional CF list must be 16 byte
    if( applyCFList->Size != 16 )
    {
        return;
    }

    // Last byte CFListType must be 0x01 to indicate the CFList contains a series of ChMask fields
    if( applyCFList->Payload[15] != 0x01 )
    {
        return;
    }

    // ChMask0 - ChMask5 must be set (every ChMask has 16 bit)
    for( uint8_t chMaskItr = 0, cntPayload = 0; chMaskItr <= 5; chMaskItr++, cntPayload+=2 )
    {
        NvmCtx.ChannelsMask[chMaskItr] = (uint16_t) (0x00FF & applyCFList->Payload[cntPayload]);
        NvmCtx.ChannelsMask[chMaskItr] |= (uint16_t) (applyCFList->Payload[cntPayload+1] << 8);
    }
}

bool RegionCN470ChanMaskSet( ChanMaskSetParams_t* chanMaskSet )
{
    switch( chanMaskSet->ChannelsMaskType )
    {
        case CHANNELS_MASK:
        {
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, chanMaskSet->ChannelsMaskIn, 6 );
            break;
        }
        case CHANNELS_DEFAULT_MASK:
        {
            RegionCommonChanMaskCopy( NvmCtx.ChannelsDefaultMask, chanMaskSet->ChannelsMaskIn, 6 );
            break;
        }
        default:
            return false;
    }
    return true;
}

void RegionCN470ComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams )
{
    double tSymbol = 0.0;

    // Get the datarate, perform a boundary check
    rxConfigParams->Datarate = MIN( datarate, CN470_RX_MAX_DATARATE );
    rxConfigParams->Bandwidth = GetBandwidth( rxConfigParams->Datarate );

    tSymbol = RegionCommonComputeSymbolTimeLoRa( DataratesCN470[rxConfigParams->Datarate], BandwidthsCN470[rxConfigParams->Datarate] );

    RegionCommonComputeRxWindowParameters( tSymbol, minRxSymbols, rxError, Radio.GetWakeupTime( ), &rxConfigParams->WindowTimeout, &rxConfigParams->WindowOffset );
}

bool RegionCN470RxConfig( RxConfigParams_t* rxConfig, int8_t* datarate )
{
    int8_t dr = rxConfig->Datarate;
    int8_t phyDr = 0;
    uint32_t frequency = rxConfig->Frequency;

    if( Radio.GetStatus( ) != RF_IDLE )
    {
        return false;
    }

    if( rxConfig->RxSlot == RX_SLOT_WIN_1 )
    {
        // Apply window 1 frequency
        frequency = CN470_FIRST_RX1_CHANNEL + ( rxConfig->Channel % 48 ) * CN470_STEPWIDTH_RX1_CHANNEL;
    }

    // Read the physical datarate from the datarates table
    phyDr = DataratesCN470[dr];

    Radio.SetChannel( frequency );

    // Radio configuration
    Radio.SetRxConfig( MODEM_LORA, rxConfig->Bandwidth, phyDr, 1, 0, 8, rxConfig->WindowTimeout, false, 0, false, 0, 0, true, rxConfig->RxContinuous );

    Radio.SetMaxPayloadLength( MODEM_LORA, MaxPayloadOfDatarateCN470[dr] + LORAMAC_FRAME_PAYLOAD_OVERHEAD_SIZE );

    *datarate = (uint8_t) dr;
    return true;
}

bool RegionCN470TxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir )
{
    int8_t phyDr = DataratesCN470[txConfig->Datarate];
    int8_t txPowerLimited = LimitTxPower( txConfig->TxPower, NvmCtx.Bands[NvmCtx.Channels[txConfig->Channel].Band].TxMaxPower, txConfig->Datarate, NvmCtx.ChannelsMask );
    int8_t phyTxPower = 0;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, txConfig->MaxEirp, txConfig->AntennaGain );

    // Setup the radio frequency
    Radio.SetChannel( NvmCtx.Channels[txConfig->Channel].Frequency );

    Radio.SetTxConfig( MODEM_LORA, phyTxPower, 0, 0, phyDr, 1, 8, false, true, 0, 0, false, 4000 );
    // Setup maximum payload lenght of the radio driver
    Radio.SetMaxPayloadLength( MODEM_LORA, txConfig->PktLen );
    // Update time-on-air
    *txTimeOnAir = GetTimeOnAir( txConfig->Datarate, txConfig->PktLen );
    *txPower = txPowerLimited;

    return true;
}

uint8_t RegionCN470LinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed )
{
    uint8_t status = 0x07;
    RegionCommonLinkAdrParams_t linkAdrParams = {0};
    uint8_t nextIndex = 0;
    uint8_t bytesProcessed = 0;
    uint16_t channelsMask[6] = { 0, 0, 0, 0, 0, 0 };
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    RegionCommonLinkAdrReqVerifyParams_t linkAdrVerifyParams;

    // Initialize local copy of channels mask
    RegionCommonChanMaskCopy( channelsMask, NvmCtx.ChannelsMask, 6 );

    while( bytesProcessed < linkAdrReq->PayloadSize )
    {
        // Get ADR request parameters
        nextIndex = RegionCommonParseLinkAdrReq( &( linkAdrReq->Payload[bytesProcessed] ), &linkAdrParams );

        if( nextIndex == 0 )
            break; // break loop, since no more request has been found

        // Update bytes processed
        bytesProcessed += nextIndex;

        // Revert status, as we only check the last ADR request for the channel mask KO
        status = 0x07;

        if( linkAdrParams.ChMaskCtrl == 6 )
        {
            // Enable all 125 kHz channels
            channelsMask[0] = 0xFFFF;
            channelsMask[1] = 0xFFFF;
            channelsMask[2] = 0xFFFF;
            channelsMask[3] = 0xFFFF;
            channelsMask[4] = 0xFFFF;
            channelsMask[5] = 0xFFFF;
        }
        else if( linkAdrParams.ChMaskCtrl == 7 )
        {
            status &= 0xFE; // Channel mask KO
        }
        else
        {
            for( uint8_t i = 0; i < 16; i++ )
            {
                if( ( ( linkAdrParams.ChMask & ( 1 << i ) ) != 0 ) &&
                    ( NvmCtx.Channels[linkAdrParams.ChMaskCtrl * 16 + i].Frequency == 0 ) )
                {// Trying to enable an undefined channel
                    status &= 0xFE; // Channel mask KO
                }
            }
            channelsMask[linkAdrParams.ChMaskCtrl] = linkAdrParams.ChMask;
        }
    }

    // Get the minimum possible datarate
    getPhy.Attribute = PHY_MIN_TX_DR;
    getPhy.UplinkDwellTime = linkAdrReq->UplinkDwellTime;
    phyParam = RegionCN470GetPhyParam( &getPhy );

    linkAdrVerifyParams.Status = status;
    linkAdrVerifyParams.AdrEnabled = linkAdrReq->AdrEnabled;
    linkAdrVerifyParams.Datarate = linkAdrParams.Datarate;
    linkAdrVerifyParams.TxPower = linkAdrParams.TxPower;
    linkAdrVerifyParams.NbRep = linkAdrParams.NbRep;
    linkAdrVerifyParams.CurrentDatarate = linkAdrReq->CurrentDatarate;
    linkAdrVerifyParams.CurrentTxPower = linkAdrReq->CurrentTxPower;
    linkAdrVerifyParams.CurrentNbRep = linkAdrReq->CurrentNbRep;
    linkAdrVerifyParams.NbChannels = CN470_MAX_NB_CHANNELS;
    linkAdrVerifyParams.ChannelsMask = channelsMask;
    linkAdrVerifyParams.MinDatarate = ( int8_t )phyParam.Value;
    linkAdrVerifyParams.MaxDatarate = CN470_TX_MAX_DATARATE;
    linkAdrVerifyParams.Channels = NvmCtx.Channels;
    linkAdrVerifyParams.MinTxPower = CN470_MIN_TX_POWER;
    linkAdrVerifyParams.MaxTxPower = CN470_MAX_TX_POWER;
    linkAdrVerifyParams.Version = linkAdrReq->Version;

    // Verify the parameters and update, if necessary
    status = RegionCommonLinkAdrReqVerifyParams( &linkAdrVerifyParams, &linkAdrParams.Datarate, &linkAdrParams.TxPower, &linkAdrParams.NbRep );

    // Update channelsMask if everything is correct
    if( status == 0x07 )
    {
        // Copy Mask
        RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, channelsMask, 6 );
    }

    // Update status variables
    *drOut = linkAdrParams.Datarate;
    *txPowOut = linkAdrParams.TxPower;
    *nbRepOut = linkAdrParams.NbRep;
    *nbBytesParsed = bytesProcessed;

    return status;
}

uint8_t RegionCN470RxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq )
{
    uint8_t status = 0x07;

    // Verify radio frequency
    if( VerifyRfFreq( rxParamSetupReq->Frequency ) == false )
    {
        status &= 0xFE; // Channel frequency KO
    }

    // Verify datarate
    if( RegionCommonValueInRange( rxParamSetupReq->Datarate, CN470_RX_MIN_DATARATE, CN470_RX_MAX_DATARATE ) == false )
    {
        status &= 0xFD; // Datarate KO
    }

    // Verify datarate offset
    if( RegionCommonValueInRange( rxParamSetupReq->DrOffset, CN470_MIN_RX1_DR_OFFSET, CN470_MAX_RX1_DR_OFFSET ) == false )
    {
        status &= 0xFB; // Rx1DrOffset range KO
    }

    return status;
}

uint8_t RegionCN470NewChannelReq( NewChannelReqParams_t* newChannelReq )
{
    // Datarate and frequency KO
    return 0;
}

int8_t RegionCN470TxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq )
{
    return -1;
}

uint8_t RegionCN470DlChannelReq( DlChannelReqParams_t* dlChannelReq )
{
    return 0;
}

int8_t RegionCN470AlternateDr( int8_t currentDr, AlternateDrType_t type )
{
    return currentDr;
}

LoRaMacStatus_t RegionCN470NextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff )
{
    uint8_t nbEnabledChannels = 0;
    uint8_t nbRestrictedChannels = 0;
    uint8_t enabledChannels[CN470_MAX_NB_CHANNELS] = { 0 };
    RegionCommonIdentifyChannelsParam_t identifyChannelsParam;
    RegionCommonCountNbOfEnabledChannelsParams_t countChannelsParams;
    LoRaMacStatus_t status = LORAMAC_STATUS_NO_CHANNEL_FOUND;

    // Count 125kHz channels
    if( RegionCommonCountChannels( NvmCtx.ChannelsMask, 0, 6 ) == 0 )
    { // Reactivate default channels
        NvmCtx.ChannelsMask[0] = 0xFFFF;
        NvmCtx.ChannelsMask[1] = 0xFFFF;
        NvmCtx.ChannelsMask[2] = 0xFFFF;
        NvmCtx.ChannelsMask[3] = 0xFFFF;
        NvmCtx.ChannelsMask[4] = 0xFFFF;
        NvmCtx.ChannelsMask[5] = 0xFFFF;
    }

    // Search how many channels are enabled
    countChannelsParams.Joined = nextChanParams->Joined;
    countChannelsParams.Datarate = nextChanParams->Datarate;
    countChannelsParams.ChannelsMask = NvmCtx.ChannelsMask;
    countChannelsParams.Channels = NvmCtx.Channels;
    countChannelsParams.Bands = NvmCtx.Bands;
    countChannelsParams.MaxNbChannels = CN470_MAX_NB_CHANNELS;
    countChannelsParams.JoinChannels = 0;

    identifyChannelsParam.AggrTimeOff = nextChanParams->AggrTimeOff;
    identifyChannelsParam.LastAggrTx = nextChanParams->LastAggrTx;
    identifyChannelsParam.DutyCycleEnabled = nextChanParams->DutyCycleEnabled;
    identifyChannelsParam.MaxBands = CN470_MAX_NB_BANDS;

    identifyChannelsParam.ElapsedTimeSinceStartUp = nextChanParams->ElapsedTimeSinceStartUp;
    identifyChannelsParam.LastTxIsJoinRequest = nextChanParams->LastTxIsJoinRequest;
    identifyChannelsParam.ExpectedTimeOnAir = GetTimeOnAir( nextChanParams->Datarate, nextChanParams->PktLen );

    identifyChannelsParam.CountNbOfEnabledChannelsParam = &countChannelsParams;

    status = RegionCommonIdentifyChannels( &identifyChannelsParam, aggregatedTimeOff, enabledChannels,
                                           &nbEnabledChannels, &nbRestrictedChannels, time );

    if( status == LORAMAC_STATUS_OK )
    {
        // We found a valid channel
        *channel = enabledChannels[randr( 0, nbEnabledChannels - 1 )];
    }
    return status;
}

LoRaMacStatus_t RegionCN470ChannelAdd( ChannelAddParams_t* channelAdd )
{
    return LORAMAC_STATUS_PARAMETER_INVALID;
}

bool RegionCN470ChannelsRemove( ChannelRemoveParams_t* channelRemove  )
{
    return LORAMAC_STATUS_PARAMETER_INVALID;
}

void RegionCN470SetContinuousWave( ContinuousWaveParams_t* continuousWave )
{
    int8_t txPowerLimited = LimitTxPower( continuousWave->TxPower, NvmCtx.Bands[NvmCtx.Channels[continuousWave->Channel].Band].TxMaxPower, continuousWave->Datarate, NvmCtx.ChannelsMask );
    int8_t phyTxPower = 0;
    uint32_t frequency = NvmCtx.Channels[continuousWave->Channel].Frequency;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, continuousWave->MaxEirp, continuousWave->AntennaGain );

    Radio.SetTxContinuousWave( frequency, phyTxPower, continuousWave->Timeout );
}

uint8_t RegionCN470ApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset )
{
    int8_t datarate = dr - drOffset;

    if( datarate < 0 )
    {
        datarate = DR_0;
    }
    return datarate;
}

void RegionCN470RxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr )
{
    RegionCommonRxBeaconSetupParams_t regionCommonRxBeaconSetup;

    regionCommonRxBeaconSetup.Datarates = DataratesCN470;
    regionCommonRxBeaconSetup.Frequency = rxBeaconSetup->Frequency;
    regionCommonRxBeaconSetup.BeaconSize = CN470_BEACON_SIZE;
    regionCommonRxBeaconSetup.BeaconDatarate = CN470_BEACON_CHANNEL_DR;
    regionCommonRxBeaconSetup.BeaconChannelBW = CN470_BEACON_CHANNEL_BW;
    regionCommonRxBeaconSetup.RxTime = rxBeaconSetup->RxTime;
    regionCommonRxBeaconSetup.SymbolTimeout = rxBeaconSetup->SymbolTimeout;

    RegionCommonRxBeaconSetup( &regionCommonRxBeaconSetup );

    // Store downlink datarate
    *outDr = CN470_BEACON_CHANNEL_DR;
}
