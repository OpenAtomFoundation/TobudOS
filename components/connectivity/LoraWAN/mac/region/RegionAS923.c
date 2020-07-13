/*!
 * \file      RegionAS923.c
 *
 * \brief     Region implementation for AS923
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
#include "RegionAS923.h"

// Definitions
#define CHANNELS_MASK_SIZE              1

/*!
 * Region specific context
 */
typedef struct sRegionAS923NvmCtx
{
    /*!
     * LoRaMAC channels
     */
    ChannelParams_t Channels[ AS923_MAX_NB_CHANNELS ];
    /*!
     * LoRaMac bands
     */
    Band_t Bands[ AS923_MAX_NB_BANDS ];
    /*!
     * LoRaMac channels mask
     */
    uint16_t ChannelsMask[ CHANNELS_MASK_SIZE ];
    /*!
     * LoRaMac channels default mask
     */
    uint16_t ChannelsDefaultMask[ CHANNELS_MASK_SIZE ];
}RegionAS923NvmCtx_t;

/*
 * Non-volatile module context.
 */
static RegionAS923NvmCtx_t NvmCtx;

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
    switch( BandwidthsAS923[drIndex] )
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

    if( ( freq < 915000000 ) || ( freq > 928000000 ) )
    {
        return false;
    }
    return true;
}

static TimerTime_t GetTimeOnAir( int8_t datarate, uint16_t pktLen )
{
    int8_t phyDr = DataratesAS923[datarate];
    uint32_t bandwidth = GetBandwidth( datarate );
    TimerTime_t timeOnAir = 0;

    if( datarate == DR_7 )
    { // High Speed FSK channel
        timeOnAir = Radio.TimeOnAir( MODEM_FSK, bandwidth, phyDr * 1000, 0, 5, false, pktLen, true );
    }
    else
    {
        timeOnAir = Radio.TimeOnAir( MODEM_LORA, bandwidth, phyDr, 1, 8, false, pktLen, true );
    }
    return timeOnAir;
}

PhyParam_t RegionAS923GetPhyParam( GetPhyParams_t* getPhy )
{
    PhyParam_t phyParam = { 0 };

    switch( getPhy->Attribute )
    {
        case PHY_MIN_RX_DR:
        {
            if( getPhy->DownlinkDwellTime == 0 )
            {
                phyParam.Value = AS923_RX_MIN_DATARATE;
            }
            else
            {
                phyParam.Value = AS923_DWELL_LIMIT_DATARATE;
            }
            break;
        }
        case PHY_MIN_TX_DR:
        {
            if( getPhy->UplinkDwellTime == 0 )
            {
                phyParam.Value = AS923_TX_MIN_DATARATE;
            }
            else
            {
                phyParam.Value = AS923_DWELL_LIMIT_DATARATE;
            }
            break;
        }
        case PHY_DEF_TX_DR:
        {
            phyParam.Value = AS923_DEFAULT_DATARATE;
            break;
        }
        case PHY_NEXT_LOWER_TX_DR:
        {
            if( getPhy->UplinkDwellTime == 0 )
            {
                phyParam.Value = GetNextLowerTxDr( getPhy->Datarate, AS923_TX_MIN_DATARATE );
            }
            else
            {
                phyParam.Value = GetNextLowerTxDr( getPhy->Datarate, AS923_DWELL_LIMIT_DATARATE );
            }
            break;
        }
        case PHY_MAX_TX_POWER:
        {
            phyParam.Value = AS923_MAX_TX_POWER;
            break;
        }
        case PHY_DEF_TX_POWER:
        {
            phyParam.Value = AS923_DEFAULT_TX_POWER;
            break;
        }
        case PHY_DEF_ADR_ACK_LIMIT:
        {
            phyParam.Value = AS923_ADR_ACK_LIMIT;
            break;
        }
        case PHY_DEF_ADR_ACK_DELAY:
        {
            phyParam.Value = AS923_ADR_ACK_DELAY;
            break;
        }
        case PHY_MAX_PAYLOAD:
        {
            if( getPhy->UplinkDwellTime == 0 )
            {
                phyParam.Value = MaxPayloadOfDatarateDwell0AS923[getPhy->Datarate];
            }
            else
            {
                phyParam.Value = MaxPayloadOfDatarateDwell1UpAS923[getPhy->Datarate];
            }
            break;
        }
        case PHY_DUTY_CYCLE:
        {
            phyParam.Value = AS923_DUTY_CYCLE_ENABLED;
            break;
        }
        case PHY_MAX_RX_WINDOW:
        {
            phyParam.Value = AS923_MAX_RX_WINDOW;
            break;
        }
        case PHY_RECEIVE_DELAY1:
        {
            phyParam.Value = AS923_RECEIVE_DELAY1;
            break;
        }
        case PHY_RECEIVE_DELAY2:
        {
            phyParam.Value = AS923_RECEIVE_DELAY2;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY1:
        {
            phyParam.Value = AS923_JOIN_ACCEPT_DELAY1;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY2:
        {
            phyParam.Value = AS923_JOIN_ACCEPT_DELAY2;
            break;
        }
        case PHY_MAX_FCNT_GAP:
        {
            phyParam.Value = AS923_MAX_FCNT_GAP;
            break;
        }
        case PHY_ACK_TIMEOUT:
        {
            phyParam.Value = ( AS923_ACKTIMEOUT + randr( -AS923_ACK_TIMEOUT_RND, AS923_ACK_TIMEOUT_RND ) );
            break;
        }
        case PHY_DEF_DR1_OFFSET:
        {
            phyParam.Value = AS923_DEFAULT_RX1_DR_OFFSET;
            break;
        }
        case PHY_DEF_RX2_FREQUENCY:
        {
            phyParam.Value = AS923_RX_WND_2_FREQ;
            break;
        }
        case PHY_DEF_RX2_DR:
        {
            phyParam.Value = AS923_RX_WND_2_DR;
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
            phyParam.Value = AS923_MAX_NB_CHANNELS;
            break;
        }
        case PHY_CHANNELS:
        {
            phyParam.Channels = NvmCtx.Channels;
            break;
        }
        case PHY_DEF_UPLINK_DWELL_TIME:
        {
            phyParam.Value = AS923_DEFAULT_UPLINK_DWELL_TIME;
            break;
        }
        case PHY_DEF_DOWNLINK_DWELL_TIME:
        {
            phyParam.Value = AS923_DEFAULT_DOWNLINK_DWELL_TIME;
            break;
        }
        case PHY_DEF_MAX_EIRP:
        {
            phyParam.fValue = AS923_DEFAULT_MAX_EIRP;
            break;
        }
        case PHY_DEF_ANTENNA_GAIN:
        {
            phyParam.fValue = AS923_DEFAULT_ANTENNA_GAIN;
            break;
        }
        case PHY_BEACON_CHANNEL_FREQ:
        {
            phyParam.Value = AS923_BEACON_CHANNEL_FREQ;
            break;
        }
        case PHY_BEACON_FORMAT:
        {
            phyParam.BeaconFormat.BeaconSize = AS923_BEACON_SIZE;
            phyParam.BeaconFormat.Rfu1Size = AS923_RFU1_SIZE;
            phyParam.BeaconFormat.Rfu2Size = AS923_RFU2_SIZE;
            break;
        }
        case PHY_BEACON_CHANNEL_DR:
        {
            phyParam.Value = AS923_BEACON_CHANNEL_DR;
            break;
        }
        case PHY_PING_SLOT_CHANNEL_FREQ:
        {
            phyParam.Value = AS923_PING_SLOT_CHANNEL_FREQ;
            break;
        }
        case PHY_PING_SLOT_CHANNEL_DR:
        {
            phyParam.Value = AS923_PING_SLOT_CHANNEL_DR;
            break;
        }
        case PHY_SF_FROM_DR:
        {
            phyParam.Value = DataratesAS923[getPhy->Datarate];
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

void RegionAS923SetBandTxDone( SetBandTxDoneParams_t* txDone )
{
    RegionCommonSetBandTxDone( &NvmCtx.Bands[NvmCtx.Channels[txDone->Channel].Band],
                               txDone->LastTxAirTime, txDone->Joined, txDone->ElapsedTimeSinceStartUp );
}

void RegionAS923InitDefaults( InitDefaultsParams_t* params )
{
    Band_t bands[AS923_MAX_NB_BANDS] =
    {
        AS923_BAND0
    };

    switch( params->Type )
    {
        case INIT_TYPE_BANDS:
        {
            // Initialize bands
            memcpy1( ( uint8_t* )NvmCtx.Bands, ( uint8_t* )bands, sizeof( Band_t ) * AS923_MAX_NB_BANDS );
            break;
        }
        case INIT_TYPE_INIT:
        {

            // Channels
            NvmCtx.Channels[0] = ( ChannelParams_t ) AS923_LC1;
            NvmCtx.Channels[1] = ( ChannelParams_t ) AS923_LC2;

            // Initialize the channels default mask
            NvmCtx.ChannelsDefaultMask[0] = LC( 1 ) + LC( 2 );

            // Update the channels mask
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, NvmCtx.ChannelsDefaultMask, 1 );
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
            NvmCtx.ChannelsMask[0] |= NvmCtx.ChannelsDefaultMask[0];

            // Channels
            NvmCtx.Channels[0] = ( ChannelParams_t ) AS923_LC1;
            NvmCtx.Channels[1] = ( ChannelParams_t ) AS923_LC2;
            break;
        }
        default:
        {
            break;
        }
    }
}

void* RegionAS923GetNvmCtx( GetNvmCtxParams_t* params )
{
    params->nvmCtxSize = sizeof( RegionAS923NvmCtx_t );
    return &NvmCtx;
}

bool RegionAS923Verify( VerifyParams_t* verify, PhyAttribute_t phyAttribute )
{
    switch( phyAttribute )
    {
        case PHY_FREQUENCY:
        {
            return VerifyRfFreq( verify->Frequency );
        }
        case PHY_TX_DR:
        {
            if( verify->DatarateParams.UplinkDwellTime == 0 )
            {
                return RegionCommonValueInRange( verify->DatarateParams.Datarate, AS923_TX_MIN_DATARATE, AS923_TX_MAX_DATARATE );
            }
            else
            {
                return RegionCommonValueInRange( verify->DatarateParams.Datarate, AS923_DWELL_LIMIT_DATARATE, AS923_TX_MAX_DATARATE );
            }
        }
        case PHY_DEF_TX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, DR_0, DR_5 );
        }
        case PHY_RX_DR:
        {
            if( verify->DatarateParams.DownlinkDwellTime == 0 )
            {
                return RegionCommonValueInRange( verify->DatarateParams.Datarate, AS923_RX_MIN_DATARATE, AS923_RX_MAX_DATARATE );
            }
            else
            {
                return RegionCommonValueInRange( verify->DatarateParams.Datarate, AS923_DWELL_LIMIT_DATARATE, AS923_RX_MAX_DATARATE );
            }
        }
        case PHY_DEF_TX_POWER:
        case PHY_TX_POWER:
        {
            // Remark: switched min and max!
            return RegionCommonValueInRange( verify->TxPower, AS923_MAX_TX_POWER, AS923_MIN_TX_POWER );
        }
        case PHY_DUTY_CYCLE:
        {
            return AS923_DUTY_CYCLE_ENABLED;
        }
        default:
            return false;
    }
}

void RegionAS923ApplyCFList( ApplyCFListParams_t* applyCFList )
{
    ChannelParams_t newChannel;
    ChannelAddParams_t channelAdd;
    ChannelRemoveParams_t channelRemove;

    // Setup default datarate range
    newChannel.DrRange.Value = ( DR_5 << 4 ) | DR_0;

    // Size of the optional CF list
    if( applyCFList->Size != 16 )
    {
        return;
    }

    // Last byte CFListType must be 0 to indicate the CFList contains a list of frequencies
    if( applyCFList->Payload[15] != 0 )
    {
        return;
    }

    // Last byte is RFU, don't take it into account
    for( uint8_t i = 0, chanIdx = AS923_NUMB_DEFAULT_CHANNELS; chanIdx < AS923_MAX_NB_CHANNELS; i+=3, chanIdx++ )
    {
        if( chanIdx < ( AS923_NUMB_CHANNELS_CF_LIST + AS923_NUMB_DEFAULT_CHANNELS ) )
        {
            // Channel frequency
            newChannel.Frequency = (uint32_t) applyCFList->Payload[i];
            newChannel.Frequency |= ( (uint32_t) applyCFList->Payload[i + 1] << 8 );
            newChannel.Frequency |= ( (uint32_t) applyCFList->Payload[i + 2] << 16 );
            newChannel.Frequency *= 100;

            // Initialize alternative frequency to 0
            newChannel.Rx1Frequency = 0;
        }
        else
        {
            newChannel.Frequency = 0;
            newChannel.DrRange.Value = 0;
            newChannel.Rx1Frequency = 0;
        }

        if( newChannel.Frequency != 0 )
        {
            channelAdd.NewChannel = &newChannel;
            channelAdd.ChannelId = chanIdx;

            // Try to add all channels
            RegionAS923ChannelAdd( &channelAdd );
        }
        else
        {
            channelRemove.ChannelId = chanIdx;

            RegionAS923ChannelsRemove( &channelRemove );
        }
    }
}

bool RegionAS923ChanMaskSet( ChanMaskSetParams_t* chanMaskSet )
{
    switch( chanMaskSet->ChannelsMaskType )
    {
        case CHANNELS_MASK:
        {
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, chanMaskSet->ChannelsMaskIn, 1 );
            break;
        }
        case CHANNELS_DEFAULT_MASK:
        {
            RegionCommonChanMaskCopy( NvmCtx.ChannelsDefaultMask, chanMaskSet->ChannelsMaskIn, 1 );
            break;
        }
        default:
            return false;
    }
    return true;
}

void RegionAS923ComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams )
{
    double tSymbol = 0.0;

    // Get the datarate, perform a boundary check
    rxConfigParams->Datarate = MIN( datarate, AS923_RX_MAX_DATARATE );
    rxConfigParams->Bandwidth = GetBandwidth( rxConfigParams->Datarate );

    if( rxConfigParams->Datarate == DR_7 )
    { // FSK
        tSymbol = RegionCommonComputeSymbolTimeFsk( DataratesAS923[rxConfigParams->Datarate] );
    }
    else
    { // LoRa
        tSymbol = RegionCommonComputeSymbolTimeLoRa( DataratesAS923[rxConfigParams->Datarate], BandwidthsAS923[rxConfigParams->Datarate] );
    }

    RegionCommonComputeRxWindowParameters( tSymbol, minRxSymbols, rxError, Radio.GetWakeupTime( ), &rxConfigParams->WindowTimeout, &rxConfigParams->WindowOffset );
}

bool RegionAS923RxConfig( RxConfigParams_t* rxConfig, int8_t* datarate )
{
    RadioModems_t modem;
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
        frequency = NvmCtx.Channels[rxConfig->Channel].Frequency;
        // Apply the alternative RX 1 window frequency, if it is available
        if( NvmCtx.Channels[rxConfig->Channel].Rx1Frequency != 0 )
        {
            frequency = NvmCtx.Channels[rxConfig->Channel].Rx1Frequency;
        }
    }

    // Read the physical datarate from the datarates table
    phyDr = DataratesAS923[dr];

    Radio.SetChannel( frequency );

    // Radio configuration
    if( dr == DR_7 )
    {
        modem = MODEM_FSK;
        Radio.SetRxConfig( modem, 50000, phyDr * 1000, 0, 83333, 5, rxConfig->WindowTimeout, false, 0, true, 0, 0, false, rxConfig->RxContinuous );
    }
    else
    {
        modem = MODEM_LORA;
        Radio.SetRxConfig( modem, rxConfig->Bandwidth, phyDr, 1, 0, 8, rxConfig->WindowTimeout, false, 0, false, 0, 0, true, rxConfig->RxContinuous );
    }

    Radio.SetMaxPayloadLength( modem, MaxPayloadOfDatarateDwell0AS923[dr] + LORAMAC_FRAME_PAYLOAD_OVERHEAD_SIZE );

    *datarate = (uint8_t) dr;
    return true;
}

bool RegionAS923TxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir )
{
    RadioModems_t modem;
    int8_t phyDr = DataratesAS923[txConfig->Datarate];
    int8_t txPowerLimited = LimitTxPower( txConfig->TxPower, NvmCtx.Bands[NvmCtx.Channels[txConfig->Channel].Band].TxMaxPower, txConfig->Datarate, NvmCtx.ChannelsMask );
    uint32_t bandwidth = GetBandwidth( txConfig->Datarate );
    int8_t phyTxPower = 0;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, txConfig->MaxEirp, txConfig->AntennaGain );

    // Setup the radio frequency
    Radio.SetChannel( NvmCtx.Channels[txConfig->Channel].Frequency );

    if( txConfig->Datarate == DR_7 )
    { // High Speed FSK channel
        modem = MODEM_FSK;
        Radio.SetTxConfig( modem, phyTxPower, 25000, bandwidth, phyDr * 1000, 0, 5, false, true, 0, 0, false, 4000 );
    }
    else
    {
        modem = MODEM_LORA;
        Radio.SetTxConfig( modem, phyTxPower, 0, bandwidth, phyDr, 1, 8, false, true, 0, 0, false, 4000 );
    }

    // Update time-on-air
    *txTimeOnAir = GetTimeOnAir( txConfig->Datarate, txConfig->PktLen );

    // Setup maximum payload lenght of the radio driver
    Radio.SetMaxPayloadLength( modem, txConfig->PktLen );

    *txPower = txPowerLimited;
    return true;
}

uint8_t RegionAS923LinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed )
{
    uint8_t status = 0x07;
    RegionCommonLinkAdrParams_t linkAdrParams = {0};
    uint8_t nextIndex = 0;
    uint8_t bytesProcessed = 0;
    uint16_t chMask = 0;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    RegionCommonLinkAdrReqVerifyParams_t linkAdrVerifyParams;

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

        // Setup temporary channels mask
        chMask = linkAdrParams.ChMask;

        // Verify channels mask
        if( ( linkAdrParams.ChMaskCtrl == 0 ) && ( chMask == 0 ) )
        {
            status &= 0xFE; // Channel mask KO
        }
        else if( ( ( linkAdrParams.ChMaskCtrl >= 1 ) && ( linkAdrParams.ChMaskCtrl <= 5 )) ||
                ( linkAdrParams.ChMaskCtrl >= 7 ) )
        {
            // RFU
            status &= 0xFE; // Channel mask KO
        }
        else
        {
            for( uint8_t i = 0; i < AS923_MAX_NB_CHANNELS; i++ )
            {
                if( linkAdrParams.ChMaskCtrl == 6 )
                {
                    if( NvmCtx.Channels[i].Frequency != 0 )
                    {
                        chMask |= 1 << i;
                    }
                }
                else
                {
                    if( ( ( chMask & ( 1 << i ) ) != 0 ) &&
                        ( NvmCtx.Channels[i].Frequency == 0 ) )
                    {// Trying to enable an undefined channel
                        status &= 0xFE; // Channel mask KO
                    }
                }
            }
        }
    }

    // Get the minimum possible datarate
    getPhy.Attribute = PHY_MIN_TX_DR;
    getPhy.UplinkDwellTime = linkAdrReq->UplinkDwellTime;
    phyParam = RegionAS923GetPhyParam( &getPhy );

    linkAdrVerifyParams.Status = status;
    linkAdrVerifyParams.AdrEnabled = linkAdrReq->AdrEnabled;
    linkAdrVerifyParams.Datarate = linkAdrParams.Datarate;
    linkAdrVerifyParams.TxPower = linkAdrParams.TxPower;
    linkAdrVerifyParams.NbRep = linkAdrParams.NbRep;
    linkAdrVerifyParams.CurrentDatarate = linkAdrReq->CurrentDatarate;
    linkAdrVerifyParams.CurrentTxPower = linkAdrReq->CurrentTxPower;
    linkAdrVerifyParams.CurrentNbRep = linkAdrReq->CurrentNbRep;
    linkAdrVerifyParams.NbChannels = AS923_MAX_NB_CHANNELS;
    linkAdrVerifyParams.ChannelsMask = &chMask;
    linkAdrVerifyParams.MinDatarate = ( int8_t )phyParam.Value;
    linkAdrVerifyParams.MaxDatarate = AS923_TX_MAX_DATARATE;
    linkAdrVerifyParams.Channels = NvmCtx.Channels;
    linkAdrVerifyParams.MinTxPower = AS923_MIN_TX_POWER;
    linkAdrVerifyParams.MaxTxPower = AS923_MAX_TX_POWER;
    linkAdrVerifyParams.Version = linkAdrReq->Version;

    // Verify the parameters and update, if necessary
    status = RegionCommonLinkAdrReqVerifyParams( &linkAdrVerifyParams, &linkAdrParams.Datarate, &linkAdrParams.TxPower, &linkAdrParams.NbRep );

    // Update channelsMask if everything is correct
    if( status == 0x07 )
    {
        // Set the channels mask to a default value
        memset1( ( uint8_t* ) NvmCtx.ChannelsMask, 0, sizeof( NvmCtx.ChannelsMask ) );
        // Update the channels mask
        NvmCtx.ChannelsMask[0] = chMask;
    }

    // Update status variables
    *drOut = linkAdrParams.Datarate;
    *txPowOut = linkAdrParams.TxPower;
    *nbRepOut = linkAdrParams.NbRep;
    *nbBytesParsed = bytesProcessed;

    return status;
}

uint8_t RegionAS923RxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq )
{
    uint8_t status = 0x07;

    // Verify radio frequency
    if( VerifyRfFreq( rxParamSetupReq->Frequency ) == false )
    {
        status &= 0xFE; // Channel frequency KO
    }

    // Verify datarate
    if( RegionCommonValueInRange( rxParamSetupReq->Datarate, AS923_RX_MIN_DATARATE, AS923_RX_MAX_DATARATE ) == false )
    {
        status &= 0xFD; // Datarate KO
    }

    // Verify datarate offset
    if( RegionCommonValueInRange( rxParamSetupReq->DrOffset, AS923_MIN_RX1_DR_OFFSET, AS923_MAX_RX1_DR_OFFSET ) == false )
    {
        status &= 0xFB; // Rx1DrOffset range KO
    }

    return status;
}

uint8_t RegionAS923NewChannelReq( NewChannelReqParams_t* newChannelReq )
{
    uint8_t status = 0x03;
    ChannelAddParams_t channelAdd;
    ChannelRemoveParams_t channelRemove;

    if( newChannelReq->NewChannel->Frequency == 0 )
    {
        channelRemove.ChannelId = newChannelReq->ChannelId;

        // Remove
        if( RegionAS923ChannelsRemove( &channelRemove ) == false )
        {
            status &= 0xFC;
        }
    }
    else
    {
        channelAdd.NewChannel = newChannelReq->NewChannel;
        channelAdd.ChannelId = newChannelReq->ChannelId;

        switch( RegionAS923ChannelAdd( &channelAdd ) )
        {
            case LORAMAC_STATUS_OK:
            {
                break;
            }
            case LORAMAC_STATUS_FREQUENCY_INVALID:
            {
                status &= 0xFE;
                break;
            }
            case LORAMAC_STATUS_DATARATE_INVALID:
            {
                status &= 0xFD;
                break;
            }
            case LORAMAC_STATUS_FREQ_AND_DR_INVALID:
            {
                status &= 0xFC;
                break;
            }
            default:
            {
                status &= 0xFC;
                break;
            }
        }
    }

    return status;
}

int8_t RegionAS923TxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq )
{
    // Accept the request
    return 0;
}

uint8_t RegionAS923DlChannelReq( DlChannelReqParams_t* dlChannelReq )
{
    uint8_t status = 0x03;

    // Verify if the frequency is supported
    if( VerifyRfFreq( dlChannelReq->Rx1Frequency ) == false )
    {
        status &= 0xFE;
    }

    // Verify if an uplink frequency exists
    if( NvmCtx.Channels[dlChannelReq->ChannelId].Frequency == 0 )
    {
        status &= 0xFD;
    }

    // Apply Rx1 frequency, if the status is OK
    if( status == 0x03 )
    {
        NvmCtx.Channels[dlChannelReq->ChannelId].Rx1Frequency = dlChannelReq->Rx1Frequency;
    }

    return status;
}

int8_t RegionAS923AlternateDr( int8_t currentDr, AlternateDrType_t type )
{
    // Only AS923_DWELL_LIMIT_DATARATE is supported
    return AS923_DWELL_LIMIT_DATARATE;
}

LoRaMacStatus_t RegionAS923NextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff )
{
    uint8_t channelNext = 0;
    uint8_t nbEnabledChannels = 0;
    uint8_t nbRestrictedChannels = 0;
    uint8_t enabledChannels[AS923_MAX_NB_CHANNELS] = { 0 };
    RegionCommonIdentifyChannelsParam_t identifyChannelsParam;
    RegionCommonCountNbOfEnabledChannelsParams_t countChannelsParams;
    LoRaMacStatus_t status = LORAMAC_STATUS_NO_CHANNEL_FOUND;

    if( RegionCommonCountChannels( NvmCtx.ChannelsMask, 0, 1 ) == 0 )
    { // Reactivate default channels
        NvmCtx.ChannelsMask[0] |= LC( 1 ) + LC( 2 );
    }

    // Search how many channels are enabled
    countChannelsParams.Joined = nextChanParams->Joined;
    countChannelsParams.Datarate = nextChanParams->Datarate;
    countChannelsParams.ChannelsMask = NvmCtx.ChannelsMask;
    countChannelsParams.Channels = NvmCtx.Channels;
    countChannelsParams.Bands = NvmCtx.Bands;
    countChannelsParams.MaxNbChannels = AS923_MAX_NB_CHANNELS;
    countChannelsParams.JoinChannels = AS923_JOIN_CHANNELS;

    identifyChannelsParam.AggrTimeOff = nextChanParams->AggrTimeOff;
    identifyChannelsParam.LastAggrTx = nextChanParams->LastAggrTx;
    identifyChannelsParam.DutyCycleEnabled = nextChanParams->DutyCycleEnabled;
    identifyChannelsParam.MaxBands = AS923_MAX_NB_BANDS;

    identifyChannelsParam.ElapsedTimeSinceStartUp = nextChanParams->ElapsedTimeSinceStartUp;
    identifyChannelsParam.LastTxIsJoinRequest = nextChanParams->LastTxIsJoinRequest;
    identifyChannelsParam.ExpectedTimeOnAir = GetTimeOnAir( nextChanParams->Datarate, nextChanParams->PktLen );

    identifyChannelsParam.CountNbOfEnabledChannelsParam = &countChannelsParams;

    status = RegionCommonIdentifyChannels( &identifyChannelsParam, aggregatedTimeOff, enabledChannels,
                                           &nbEnabledChannels, &nbRestrictedChannels, time );

    if( status == LORAMAC_STATUS_OK )
    {
        for( uint8_t  i = 0, j = randr( 0, nbEnabledChannels - 1 ); i < AS923_MAX_NB_CHANNELS; i++ )
        {
            channelNext = enabledChannels[j];
            j = ( j + 1 ) % nbEnabledChannels;

            // Perform carrier sense for AS923_CARRIER_SENSE_TIME
            // If the channel is free, we can stop the LBT mechanism
            if( Radio.IsChannelFree( MODEM_LORA, NvmCtx.Channels[channelNext].Frequency, AS923_RSSI_FREE_TH, AS923_CARRIER_SENSE_TIME ) == true )
            {
                // Free channel found
                *channel = channelNext;
                return LORAMAC_STATUS_OK;
            }
        }
        // Even if one or more channels are available according to the channel plan, no free channel
        // was found during the LBT procedure.
        status = LORAMAC_STATUS_NO_FREE_CHANNEL_FOUND;
    }
    else if( status == LORAMAC_STATUS_NO_CHANNEL_FOUND )
    {
        // Datarate not supported by any channel, restore defaults
        NvmCtx.ChannelsMask[0] |= LC( 1 ) + LC( 2 );
    }
    return status;
}

LoRaMacStatus_t RegionAS923ChannelAdd( ChannelAddParams_t* channelAdd )
{
    bool drInvalid = false;
    bool freqInvalid = false;
    uint8_t id = channelAdd->ChannelId;

    if( id < AS923_NUMB_DEFAULT_CHANNELS )
    {
        return LORAMAC_STATUS_FREQ_AND_DR_INVALID;
    }

    if( id >= AS923_MAX_NB_CHANNELS )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    // Validate the datarate range
    if( RegionCommonValueInRange( channelAdd->NewChannel->DrRange.Fields.Min, AS923_TX_MIN_DATARATE, AS923_TX_MAX_DATARATE ) == false )
    {
        drInvalid = true;
    }
    if( RegionCommonValueInRange( channelAdd->NewChannel->DrRange.Fields.Max, AS923_TX_MIN_DATARATE, AS923_TX_MAX_DATARATE ) == false )
    {
        drInvalid = true;
    }
    if( channelAdd->NewChannel->DrRange.Fields.Min > channelAdd->NewChannel->DrRange.Fields.Max )
    {
        drInvalid = true;
    }

    // Check frequency
    if( freqInvalid == false )
    {
        if( VerifyRfFreq( channelAdd->NewChannel->Frequency ) == false )
        {
            freqInvalid = true;
        }
    }

    // Check status
    if( ( drInvalid == true ) && ( freqInvalid == true ) )
    {
        return LORAMAC_STATUS_FREQ_AND_DR_INVALID;
    }
    if( drInvalid == true )
    {
        return LORAMAC_STATUS_DATARATE_INVALID;
    }
    if( freqInvalid == true )
    {
        return LORAMAC_STATUS_FREQUENCY_INVALID;
    }

    memcpy1( ( uint8_t* ) &(NvmCtx.Channels[id]), ( uint8_t* ) channelAdd->NewChannel, sizeof( NvmCtx.Channels[id] ) );
    NvmCtx.Channels[id].Band = 0;
    NvmCtx.ChannelsMask[0] |= ( 1 << id );
    return LORAMAC_STATUS_OK;
}

bool RegionAS923ChannelsRemove( ChannelRemoveParams_t* channelRemove  )
{
    uint8_t id = channelRemove->ChannelId;

    if( id < AS923_NUMB_DEFAULT_CHANNELS )
    {
        return false;
    }

    // Remove the channel from the list of channels
    NvmCtx.Channels[id] = ( ChannelParams_t ){ 0, 0, { 0 }, 0 };

    return RegionCommonChanDisable( NvmCtx.ChannelsMask, id, AS923_MAX_NB_CHANNELS );
}

void RegionAS923SetContinuousWave( ContinuousWaveParams_t* continuousWave )
{
    int8_t txPowerLimited = LimitTxPower( continuousWave->TxPower, NvmCtx.Bands[NvmCtx.Channels[continuousWave->Channel].Band].TxMaxPower, continuousWave->Datarate, NvmCtx.ChannelsMask );
    int8_t phyTxPower = 0;
    uint32_t frequency = NvmCtx.Channels[continuousWave->Channel].Frequency;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, continuousWave->MaxEirp, continuousWave->AntennaGain );

    Radio.SetTxContinuousWave( frequency, phyTxPower, continuousWave->Timeout );
}

uint8_t RegionAS923ApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset )
{
    // Initialize minDr for a downlink dwell time configuration of 0
    int8_t minDr = DR_0;

    // Update the minDR for a downlink dwell time configuration of 1
    if( downlinkDwellTime == 1 )
    {
        minDr = AS923_DWELL_LIMIT_DATARATE;
    }

    // Apply offset formula
    return MIN( DR_5, MAX( minDr, dr - EffectiveRx1DrOffsetAS923[drOffset] ) );
}

void RegionAS923RxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr )
{
    RegionCommonRxBeaconSetupParams_t regionCommonRxBeaconSetup;

    regionCommonRxBeaconSetup.Datarates = DataratesAS923;
    regionCommonRxBeaconSetup.Frequency = rxBeaconSetup->Frequency;
    regionCommonRxBeaconSetup.BeaconSize = AS923_BEACON_SIZE;
    regionCommonRxBeaconSetup.BeaconDatarate = AS923_BEACON_CHANNEL_DR;
    regionCommonRxBeaconSetup.BeaconChannelBW = AS923_BEACON_CHANNEL_BW;
    regionCommonRxBeaconSetup.RxTime = rxBeaconSetup->RxTime;
    regionCommonRxBeaconSetup.SymbolTimeout = rxBeaconSetup->SymbolTimeout;

    RegionCommonRxBeaconSetup( &regionCommonRxBeaconSetup );

    // Store downlink datarate
    *outDr = AS923_BEACON_CHANNEL_DR;
}
