/*!
 * \file      RegionUS915.c
 *
 * \brief     Region implementation for US915
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
#include "RegionUS915.h"

// Definitions
#define CHANNELS_MASK_SIZE              6

// A mask to select only valid 500KHz channels
#define CHANNELS_MASK_500KHZ_MASK       0x00FF

/*!
 * Region specific context
 */
typedef struct sRegionUS915NvmCtx
{
    /*!
     * LoRaMAC channels
     */
    ChannelParams_t Channels[ US915_MAX_NB_CHANNELS ];
    /*!
     * LoRaMac bands
     */
    Band_t Bands[ US915_MAX_NB_BANDS ];
    /*!
     * LoRaMac channels mask
     */
    uint16_t ChannelsMask[ CHANNELS_MASK_SIZE ];
    /*!
     * LoRaMac channels remaining
     */
    uint16_t ChannelsMaskRemaining[CHANNELS_MASK_SIZE];
    /*!
     * LoRaMac channels default mask
     */
    uint16_t ChannelsDefaultMask[ CHANNELS_MASK_SIZE ];
    /*!
     * Index of current in use 8 bit group (0: bit 0 - 7, 1: bit 8 - 15, ..., 7: bit 56 - 63)
     */
    uint8_t JoinChannelGroupsCurrentIndex;
    /*!
     * Counter of join trials needed to alternate between DR0 and DR4, see \ref RegionUS915AlternateDr
     */
    uint8_t JoinTrialsCounter;
}RegionUS915NvmCtx_t;

/*
 * Non-volatile module context.
 */
static RegionUS915NvmCtx_t NvmCtx;

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

/*!
 * \brief Searches for available 125 kHz channels in the given channel mask.
 *
 * \param [IN] channelMaskRemaining The remaining channel mask.
 *
 * \param [OUT] findAvailableChannelsIndex List containing the indexes of all available 125 kHz channels.
 *
 * \param [OUT] availableChannels Number of available 125 kHz channels.
 *
 * \retval Status
 */
static LoRaMacStatus_t FindAvailable125kHzChannels( uint8_t* findAvailableChannelsIndex, uint16_t channelMaskRemaining, uint8_t* availableChannels )
{
    // Nullpointer check
    if( findAvailableChannelsIndex == NULL || availableChannels == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    // Initialize counter
    *availableChannels = 0;
    for( uint8_t i = 0; i < 8; i++ )
    {
        // Find available channels
        if( ( channelMaskRemaining & ( 1 << i ) ) != 0 )
        {
            // Save available channel index
            findAvailableChannelsIndex[*availableChannels] = i;
            // Increment counter of available channels if the current channel is available
            ( *availableChannels )++;
        }
    }

    return LORAMAC_STATUS_OK;
}

/*!
 * \brief Computes the next 125kHz channel used for join requests.
 *
 * \param [OUT] newChannelIndex Index of available channel.
 *
 * \retval Status
 */
static LoRaMacStatus_t ComputeNext125kHzJoinChannel( uint8_t* newChannelIndex )
{
    uint8_t currentChannelsMaskRemainingIndex;
    uint16_t channelMaskRemaining;
    uint8_t findAvailableChannelsIndex[8] = { 0 };
    uint8_t availableChannels = 0;
    uint8_t startIndex = NvmCtx.JoinChannelGroupsCurrentIndex;

    // Null pointer check
    if( newChannelIndex == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    do {
        // Current ChannelMaskRemaining, two groups per channel mask. For example Group 0 and 1 (8 bit) are ChannelMaskRemaining 0 (16 bit), etc.
        currentChannelsMaskRemainingIndex = (uint8_t) startIndex / 2;

        // For even numbers we need the 8 LSBs and for uneven the 8 MSBs
        if( ( startIndex % 2 ) == 0 )
        {
            channelMaskRemaining = ( NvmCtx.ChannelsMaskRemaining[currentChannelsMaskRemainingIndex] & 0x00FF );
        }
        else
        {
            channelMaskRemaining = ( ( NvmCtx.ChannelsMaskRemaining[currentChannelsMaskRemainingIndex] >> 8 ) & 0x00FF );
        }


        if( FindAvailable125kHzChannels( findAvailableChannelsIndex, channelMaskRemaining, &availableChannels ) == LORAMAC_STATUS_PARAMETER_INVALID )
        {
            return LORAMAC_STATUS_PARAMETER_INVALID;
        }

        if ( availableChannels > 0 )
        {
            // Choose randomly a free channel 125kHz
            *newChannelIndex = ( startIndex * 8 ) + findAvailableChannelsIndex[randr( 0, ( availableChannels - 1 ) )];
        }

        // Increment start index
        startIndex++;
        if ( startIndex > 7 )
        {
            startIndex = 0;
        }
    } while( ( availableChannels == 0 ) && ( startIndex != NvmCtx.JoinChannelGroupsCurrentIndex ) );

    if ( availableChannels > 0 )
    {
        NvmCtx.JoinChannelGroupsCurrentIndex = startIndex;
        return LORAMAC_STATUS_OK;
    }

    return LORAMAC_STATUS_PARAMETER_INVALID;
}

static uint32_t GetBandwidth( uint32_t drIndex )
{
    switch( BandwidthsUS915[drIndex] )
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

    if( datarate == DR_4 )
    {// Limit tx power to max 26dBm
        txPowerResult = MAX( txPower, TX_POWER_2 );
    }
    else
    {
        if( RegionCommonCountChannels( channelsMask, 0, 4 ) < 50 )
        {// Limit tx power to max 21dBm
            txPowerResult = MAX( txPower, TX_POWER_5 );
        }
    }
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
    if( ( freq < US915_FIRST_RX1_CHANNEL ) ||
        ( freq > US915_LAST_RX1_CHANNEL ) ||
        ( ( ( freq - ( uint32_t ) US915_FIRST_RX1_CHANNEL ) % ( uint32_t ) US915_STEPWIDTH_RX1_CHANNEL ) != 0 ) )
    {
        return false;
    }

    // Test for frequency range - take RX and TX freqencies into account
    if( ( freq < 902300000 ) ||  ( freq > 927500000 ) )
    {
        return false;
    }
    return true;
}

static TimerTime_t GetTimeOnAir( int8_t datarate, uint16_t pktLen )
{
    int8_t phyDr = DataratesUS915[datarate];
    uint32_t bandwidth = GetBandwidth( datarate );

    return Radio.TimeOnAir( MODEM_LORA, bandwidth, phyDr, 1, 8, false, pktLen, true );
}

PhyParam_t RegionUS915GetPhyParam( GetPhyParams_t* getPhy )
{
    PhyParam_t phyParam = { 0 };

    switch( getPhy->Attribute )
    {
        case PHY_MIN_RX_DR:
        {
            phyParam.Value = US915_RX_MIN_DATARATE;
            break;
        }
        case PHY_MIN_TX_DR:
        {
            phyParam.Value = US915_TX_MIN_DATARATE;
            break;
        }
        case PHY_DEF_TX_DR:
        {
            phyParam.Value = US915_DEFAULT_DATARATE;
            break;
        }
        case PHY_NEXT_LOWER_TX_DR:
        {
            phyParam.Value = GetNextLowerTxDr( getPhy->Datarate, US915_TX_MIN_DATARATE );
            break;
        }
        case PHY_MAX_TX_POWER:
        {
            phyParam.Value = US915_MAX_TX_POWER;
            break;
        }
        case PHY_DEF_TX_POWER:
        {
            phyParam.Value = US915_DEFAULT_TX_POWER;
            break;
        }
        case PHY_DEF_ADR_ACK_LIMIT:
        {
            phyParam.Value = US915_ADR_ACK_LIMIT;
            break;
        }
        case PHY_DEF_ADR_ACK_DELAY:
        {
            phyParam.Value = US915_ADR_ACK_DELAY;
            break;
        }
        case PHY_MAX_PAYLOAD:
        {
            phyParam.Value = MaxPayloadOfDatarateUS915[getPhy->Datarate];
            break;
        }
        case PHY_DUTY_CYCLE:
        {
            phyParam.Value = US915_DUTY_CYCLE_ENABLED;
            break;
        }
        case PHY_MAX_RX_WINDOW:
        {
            phyParam.Value = US915_MAX_RX_WINDOW;
            break;
        }
        case PHY_RECEIVE_DELAY1:
        {
            phyParam.Value = US915_RECEIVE_DELAY1;
            break;
        }
        case PHY_RECEIVE_DELAY2:
        {
            phyParam.Value = US915_RECEIVE_DELAY2;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY1:
        {
            phyParam.Value = US915_JOIN_ACCEPT_DELAY1;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY2:
        {
            phyParam.Value = US915_JOIN_ACCEPT_DELAY2;
            break;
        }
        case PHY_MAX_FCNT_GAP:
        {
            phyParam.Value = US915_MAX_FCNT_GAP;
            break;
        }
        case PHY_ACK_TIMEOUT:
        {
            phyParam.Value = ( US915_ACKTIMEOUT + randr( -US915_ACK_TIMEOUT_RND, US915_ACK_TIMEOUT_RND ) );
            break;
        }
        case PHY_DEF_DR1_OFFSET:
        {
            phyParam.Value = US915_DEFAULT_RX1_DR_OFFSET;
            break;
        }
        case PHY_DEF_RX2_FREQUENCY:
        {
            phyParam.Value = US915_RX_WND_2_FREQ;
            break;
        }
        case PHY_DEF_RX2_DR:
        {
            phyParam.Value = US915_RX_WND_2_DR;
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
            phyParam.Value = US915_MAX_NB_CHANNELS;
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
            phyParam.fValue = US915_DEFAULT_MAX_ERP + 2.15f;
            break;
        }
        case PHY_DEF_ANTENNA_GAIN:
        {
            phyParam.fValue = 0;
            break;
        }
        case PHY_BEACON_CHANNEL_FREQ:
        {
            phyParam.Value = US915_BEACON_CHANNEL_FREQ + ( getPhy->Channel * US915_BEACON_CHANNEL_STEPWIDTH );
            break;
        }
        case PHY_BEACON_FORMAT:
        {
            phyParam.BeaconFormat.BeaconSize = US915_BEACON_SIZE;
            phyParam.BeaconFormat.Rfu1Size = US915_RFU1_SIZE;
            phyParam.BeaconFormat.Rfu2Size = US915_RFU2_SIZE;
            break;
        }
        case PHY_BEACON_CHANNEL_DR:
        {
            phyParam.Value = US915_BEACON_CHANNEL_DR;
            break;
        }
        case PHY_BEACON_CHANNEL_STEPWIDTH:
        {
            phyParam.Value = US915_BEACON_CHANNEL_STEPWIDTH;
            break;
        }
        case PHY_BEACON_NB_CHANNELS:
        {
            phyParam.Value = US915_BEACON_NB_CHANNELS;
            break;
        }
        case PHY_PING_SLOT_CHANNEL_FREQ:
        {
            phyParam.Value = US915_PING_SLOT_CHANNEL_FREQ + ( getPhy->Channel * US915_BEACON_CHANNEL_STEPWIDTH );
            break;
        }
        case PHY_PING_SLOT_CHANNEL_DR:
        {
            phyParam.Value = US915_PING_SLOT_CHANNEL_DR;
            break;
        }
        case PHY_SF_FROM_DR:
        {
            phyParam.Value = DataratesUS915[getPhy->Datarate];
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

void RegionUS915SetBandTxDone( SetBandTxDoneParams_t* txDone )
{
    RegionCommonSetBandTxDone( &NvmCtx.Bands[NvmCtx.Channels[txDone->Channel].Band],
                               txDone->LastTxAirTime, txDone->Joined, txDone->ElapsedTimeSinceStartUp );
}

void RegionUS915InitDefaults( InitDefaultsParams_t* params )
{
    Band_t bands[US915_MAX_NB_BANDS] =
    {
       US915_BAND0
    };

    switch( params->Type )
    {
        case INIT_TYPE_BANDS:
        {
            // Initialize bands
            memcpy1( ( uint8_t* )NvmCtx.Bands, ( uint8_t* )bands, sizeof( Band_t ) * US915_MAX_NB_BANDS );

            // Initialize 8 bit channel groups index
            NvmCtx.JoinChannelGroupsCurrentIndex = 0;

            // Initialize the join trials counter
            NvmCtx.JoinTrialsCounter = 0;

            // ChannelsMask
            NvmCtx.ChannelsDefaultMask[0] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[1] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[2] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[3] = 0xFFFF;
            NvmCtx.ChannelsDefaultMask[4] = 0x00FF;
            NvmCtx.ChannelsDefaultMask[5] = 0x0000;

            // Copy channels default mask
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, NvmCtx.ChannelsDefaultMask, 6 );

            // Copy into channels mask remaining
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMaskRemaining, NvmCtx.ChannelsMask, 6 );
            break;
        }
        case INIT_TYPE_INIT:
        {
            // Channels
            // 125 kHz channels
            for( uint8_t i = 0; i < US915_MAX_NB_CHANNELS - 8; i++ )
            {
                NvmCtx.Channels[i].Frequency = 902300000 + i * 200000;
                NvmCtx.Channels[i].DrRange.Value = ( DR_3 << 4 ) | DR_0;
                NvmCtx.Channels[i].Band = 0;
            }
            // 500 kHz channels
            for( uint8_t i = US915_MAX_NB_CHANNELS - 8; i < US915_MAX_NB_CHANNELS; i++ )
            {
                NvmCtx.Channels[i].Frequency = 903000000 + ( i - ( US915_MAX_NB_CHANNELS - 8 ) ) * 1600000;
                NvmCtx.Channels[i].DrRange.Value = ( DR_4 << 4 ) | DR_4;
                NvmCtx.Channels[i].Band = 0;
            }
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
            // Copy channels default mask
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, NvmCtx.ChannelsDefaultMask, 6 );

            for( uint8_t i = 0; i < 6; i++ )
            { // Copy-And the channels mask
                NvmCtx.ChannelsMaskRemaining[i] &= NvmCtx.ChannelsMask[i];
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void* RegionUS915GetNvmCtx( GetNvmCtxParams_t* params )
{
    params->nvmCtxSize = sizeof( RegionUS915NvmCtx_t );
    return &NvmCtx;
}

bool RegionUS915Verify( VerifyParams_t* verify, PhyAttribute_t phyAttribute )
{
    switch( phyAttribute )
    {
        case PHY_FREQUENCY:
        {
            return VerifyRfFreq( verify->Frequency );
        }
        case PHY_TX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, US915_TX_MIN_DATARATE, US915_TX_MAX_DATARATE );
        }
        case PHY_DEF_TX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, DR_0, DR_5 );
        }
        case PHY_RX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, US915_RX_MIN_DATARATE, US915_RX_MAX_DATARATE );
        }
        case PHY_DEF_TX_POWER:
        case PHY_TX_POWER:
        {
            // Remark: switched min and max!
            return RegionCommonValueInRange( verify->TxPower, US915_MAX_TX_POWER, US915_MIN_TX_POWER );
        }
        case PHY_DUTY_CYCLE:
        {
            return US915_DUTY_CYCLE_ENABLED;
        }
        default:
            return false;
    }
}

void RegionUS915ApplyCFList( ApplyCFListParams_t* applyCFList )
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

    // ChMask0 - ChMask4 must be set (every ChMask has 16 bit)
    for( uint8_t chMaskItr = 0, cntPayload = 0; chMaskItr <= 4; chMaskItr++, cntPayload+=2 )
    {
        NvmCtx.ChannelsMask[chMaskItr] = (uint16_t) (0x00FF & applyCFList->Payload[cntPayload]);
        NvmCtx.ChannelsMask[chMaskItr] |= (uint16_t) (applyCFList->Payload[cntPayload+1] << 8);
        if( chMaskItr == 4 )
        {
            NvmCtx.ChannelsMask[chMaskItr] = NvmCtx.ChannelsMask[chMaskItr] & CHANNELS_MASK_500KHZ_MASK;
        }
        // Set the channel mask to the remaining
        NvmCtx.ChannelsMaskRemaining[chMaskItr] &= NvmCtx.ChannelsMask[chMaskItr];
    }
}

bool RegionUS915ChanMaskSet( ChanMaskSetParams_t* chanMaskSet )
{
    uint8_t nbChannels = RegionCommonCountChannels( chanMaskSet->ChannelsMaskIn, 0, 4 );

    // Check the number of active channels
    if( ( nbChannels < 2 ) &&
        ( nbChannels > 0 ) )
    {
        return false;
    }

    switch( chanMaskSet->ChannelsMaskType )
    {
        case CHANNELS_MASK:
        {
            RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, chanMaskSet->ChannelsMaskIn, CHANNELS_MASK_SIZE );

            NvmCtx.ChannelsDefaultMask[4] = NvmCtx.ChannelsDefaultMask[4] & CHANNELS_MASK_500KHZ_MASK;
            NvmCtx.ChannelsDefaultMask[5] = 0x0000;

            for( uint8_t i = 0; i < CHANNELS_MASK_SIZE; i++ )
            { // Copy-And the channels mask
                NvmCtx.ChannelsMaskRemaining[i] &= NvmCtx.ChannelsMask[i];
            }
            break;
        }
        case CHANNELS_DEFAULT_MASK:
        {
            RegionCommonChanMaskCopy( NvmCtx.ChannelsDefaultMask, chanMaskSet->ChannelsMaskIn, CHANNELS_MASK_SIZE );
            break;
        }
        default:
            return false;
    }
    return true;
}

void RegionUS915ComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams )
{
    double tSymbol = 0.0;

    // Get the datarate, perform a boundary check
    rxConfigParams->Datarate = MIN( datarate, US915_RX_MAX_DATARATE );
    rxConfigParams->Bandwidth = GetBandwidth( rxConfigParams->Datarate );

    tSymbol = RegionCommonComputeSymbolTimeLoRa( DataratesUS915[rxConfigParams->Datarate], BandwidthsUS915[rxConfigParams->Datarate] );

    RegionCommonComputeRxWindowParameters( tSymbol, minRxSymbols, rxError, Radio.GetWakeupTime( ), &rxConfigParams->WindowTimeout, &rxConfigParams->WindowOffset );
}

bool RegionUS915RxConfig( RxConfigParams_t* rxConfig, int8_t* datarate )
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
        frequency = US915_FIRST_RX1_CHANNEL + ( rxConfig->Channel % 8 ) * US915_STEPWIDTH_RX1_CHANNEL;
    }

    // Read the physical datarate from the datarates table
    phyDr = DataratesUS915[dr];

    Radio.SetChannel( frequency );

    // Radio configuration
    Radio.SetRxConfig( MODEM_LORA, rxConfig->Bandwidth, phyDr, 1, 0, 8, rxConfig->WindowTimeout, false, 0, false, 0, 0, true, rxConfig->RxContinuous );

    Radio.SetMaxPayloadLength( MODEM_LORA, MaxPayloadOfDatarateUS915[dr] + LORAMAC_FRAME_PAYLOAD_OVERHEAD_SIZE );

    *datarate = (uint8_t) dr;
    return true;
}

bool RegionUS915TxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir )
{
    int8_t phyDr = DataratesUS915[txConfig->Datarate];
    int8_t txPowerLimited = LimitTxPower( txConfig->TxPower, NvmCtx.Bands[NvmCtx.Channels[txConfig->Channel].Band].TxMaxPower, txConfig->Datarate, NvmCtx.ChannelsMask );
    uint32_t bandwidth = GetBandwidth( txConfig->Datarate );
    int8_t phyTxPower = 0;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, US915_DEFAULT_MAX_ERP, 0 );

    // Setup the radio frequency
    Radio.SetChannel( NvmCtx.Channels[txConfig->Channel].Frequency );

    Radio.SetTxConfig( MODEM_LORA, phyTxPower, 0, bandwidth, phyDr, 1, 8, false, true, 0, 0, false, 4000 );

    // Setup maximum payload lenght of the radio driver
    Radio.SetMaxPayloadLength( MODEM_LORA, txConfig->PktLen );

    // Update time-on-air
    *txTimeOnAir = GetTimeOnAir( txConfig->Datarate, txConfig->PktLen );

    *txPower = txPowerLimited;
    return true;
}

uint8_t RegionUS915LinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed )
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
            // Apply chMask to channels 64 to 71
            channelsMask[4] = linkAdrParams.ChMask & CHANNELS_MASK_500KHZ_MASK;
        }
        else if( linkAdrParams.ChMaskCtrl == 7 )
        {
            // Disable all 125 kHz channels
            channelsMask[0] = 0x0000;
            channelsMask[1] = 0x0000;
            channelsMask[2] = 0x0000;
            channelsMask[3] = 0x0000;
            // Apply chMask to channels 64 to 71
            channelsMask[4] = linkAdrParams.ChMask & CHANNELS_MASK_500KHZ_MASK;
        }
        else if( linkAdrParams.ChMaskCtrl == 5 )
        {
            // Start value for comparision
            uint8_t bitMask = 1;

            // cntChannelMask for channelsMask[0] until channelsMask[3]
            uint8_t cntChannelMask = 0;

            // i will be 1, 2, 3, ..., 7
            for( uint8_t i = 0; i <= 7; i++ )
            {
                // 8 MSBs of ChMask are RFU
                // Checking if the ChMask is set, then true
                if( ( ( linkAdrParams.ChMask & 0x00FF ) & ( bitMask << i ) ) != 0 )
                {
                    if( ( i % 2 ) == 0 )
                    {
                        // Enable a bank of 8 125kHz channels, 8 LSBs
                        channelsMask[cntChannelMask] |= 0x00FF;
                        // Enable the corresponding 500kHz channel
                        channelsMask[4] |= ( bitMask << i );
                    }
                    else
                    {
                        // Enable a bank of 8 125kHz channels, 8 MSBs
                        channelsMask[cntChannelMask] |= 0xFF00;
                        // Enable the corresponding 500kHz channel
                        channelsMask[4] |= ( bitMask << i );
                        // cntChannelMask increment for uneven i
                        cntChannelMask++;
                    }
                }
                // ChMask is not set
                else
                {
                    if( ( i % 2 ) == 0 )
                    {
                        // Disable a bank of 8 125kHz channels, 8 LSBs
                        channelsMask[cntChannelMask] &= 0xFF00;
                        // Disable the corresponding 500kHz channel
                        channelsMask[4] &= ~( bitMask << i );
                    }
                    else
                    {
                        // Enable a bank of 8 125kHz channels, 8 MSBs
                        channelsMask[cntChannelMask] &= 0x00FF;
                        // Disable the corresponding 500kHz channel
                        channelsMask[4] &= ~( bitMask << i );
                        // cntChannelMask increment for uneven i
                        cntChannelMask++;
                    }
                }
            }
        }
        else
        {
            channelsMask[linkAdrParams.ChMaskCtrl] = linkAdrParams.ChMask;
        }
    }

    // FCC 15.247 paragraph F mandates to hop on at least 2 125 kHz channels
    if( ( linkAdrParams.Datarate < DR_4 ) && ( RegionCommonCountChannels( channelsMask, 0, 4 ) < 2 ) )
    {
        status &= 0xFE; // Channel mask KO
    }

    // Get the minimum possible datarate
    getPhy.Attribute = PHY_MIN_TX_DR;
    getPhy.UplinkDwellTime = linkAdrReq->UplinkDwellTime;
    phyParam = RegionUS915GetPhyParam( &getPhy );

    linkAdrVerifyParams.Status = status;
    linkAdrVerifyParams.AdrEnabled = linkAdrReq->AdrEnabled;
    linkAdrVerifyParams.Datarate = linkAdrParams.Datarate;
    linkAdrVerifyParams.TxPower = linkAdrParams.TxPower;
    linkAdrVerifyParams.NbRep = linkAdrParams.NbRep;
    linkAdrVerifyParams.CurrentDatarate = linkAdrReq->CurrentDatarate;
    linkAdrVerifyParams.CurrentTxPower = linkAdrReq->CurrentTxPower;
    linkAdrVerifyParams.CurrentNbRep = linkAdrReq->CurrentNbRep;
    linkAdrVerifyParams.NbChannels = US915_MAX_NB_CHANNELS;
    linkAdrVerifyParams.ChannelsMask = channelsMask;
    linkAdrVerifyParams.MinDatarate = ( int8_t )phyParam.Value;
    linkAdrVerifyParams.MaxDatarate = US915_TX_MAX_DATARATE;
    linkAdrVerifyParams.Channels = NvmCtx.Channels;
    linkAdrVerifyParams.MinTxPower = US915_MIN_TX_POWER;
    linkAdrVerifyParams.MaxTxPower = US915_MAX_TX_POWER;
    linkAdrVerifyParams.Version = linkAdrReq->Version;

    // Verify the parameters and update, if necessary
    status = RegionCommonLinkAdrReqVerifyParams( &linkAdrVerifyParams, &linkAdrParams.Datarate, &linkAdrParams.TxPower, &linkAdrParams.NbRep );

    // Update channelsMask if everything is correct
    if( status == 0x07 )
    {
        // Copy Mask
        RegionCommonChanMaskCopy( NvmCtx.ChannelsMask, channelsMask, 6 );

        NvmCtx.ChannelsMaskRemaining[0] &= NvmCtx.ChannelsMask[0];
        NvmCtx.ChannelsMaskRemaining[1] &= NvmCtx.ChannelsMask[1];
        NvmCtx.ChannelsMaskRemaining[2] &= NvmCtx.ChannelsMask[2];
        NvmCtx.ChannelsMaskRemaining[3] &= NvmCtx.ChannelsMask[3];
        NvmCtx.ChannelsMaskRemaining[4] = NvmCtx.ChannelsMask[4];
        NvmCtx.ChannelsMaskRemaining[5] = NvmCtx.ChannelsMask[5];
    }

    // Update status variables
    *drOut = linkAdrParams.Datarate;
    *txPowOut = linkAdrParams.TxPower;
    *nbRepOut = linkAdrParams.NbRep;
    *nbBytesParsed = bytesProcessed;

    return status;
}

uint8_t RegionUS915RxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq )
{
    uint8_t status = 0x07;

    // Verify radio frequency
    if( VerifyRfFreq( rxParamSetupReq->Frequency ) == false )
    {
        status &= 0xFE; // Channel frequency KO
    }

    // Verify datarate
    if( RegionCommonValueInRange( rxParamSetupReq->Datarate, US915_RX_MIN_DATARATE, US915_RX_MAX_DATARATE ) == false )
    {
        status &= 0xFD; // Datarate KO
    }
    if( ( RegionCommonValueInRange( rxParamSetupReq->Datarate, DR_5, DR_7 ) == true ) ||
        ( rxParamSetupReq->Datarate > DR_13 ) )
    {
        status &= 0xFD; // Datarate KO
    }

    // Verify datarate offset
    if( RegionCommonValueInRange( rxParamSetupReq->DrOffset, US915_MIN_RX1_DR_OFFSET, US915_MAX_RX1_DR_OFFSET ) == false )
    {
        status &= 0xFB; // Rx1DrOffset range KO
    }

    return status;
}

uint8_t RegionUS915NewChannelReq( NewChannelReqParams_t* newChannelReq )
{
    // Datarate and frequency KO
    return 0;
}

int8_t RegionUS915TxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq )
{
    return -1;
}

uint8_t RegionUS915DlChannelReq( DlChannelReqParams_t* dlChannelReq )
{
    return 0;
}

int8_t RegionUS915AlternateDr( int8_t currentDr, AlternateDrType_t type )
{
    // Alternates the data rate according to the channel sequence:
    // Eight times a 125kHz DR_0 and then one 500kHz DR_4 channel
    if( type == ALTERNATE_DR )
    {
        NvmCtx.JoinTrialsCounter++;
    }
    else
    {
        NvmCtx.JoinTrialsCounter--;
    }

    if( NvmCtx.JoinTrialsCounter % 9 == 0 )
    {
        // Use DR_4 every 9th times.
        currentDr = DR_4;
    }
    else
    {
        currentDr = DR_0;
    }
    return currentDr;
}

LoRaMacStatus_t RegionUS915NextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff )
{
    uint8_t nbEnabledChannels = 0;
    uint8_t nbRestrictedChannels = 0;
    uint8_t enabledChannels[US915_MAX_NB_CHANNELS] = { 0 };
    uint8_t newChannelIndex = 0;
    RegionCommonIdentifyChannelsParam_t identifyChannelsParam;
    RegionCommonCountNbOfEnabledChannelsParams_t countChannelsParams;
    LoRaMacStatus_t status = LORAMAC_STATUS_NO_CHANNEL_FOUND;

    // Count 125kHz channels
    if( RegionCommonCountChannels( NvmCtx.ChannelsMaskRemaining, 0, 4 ) == 0 )
    { // Reactivate default channels
        RegionCommonChanMaskCopy( NvmCtx.ChannelsMaskRemaining, NvmCtx.ChannelsMask, 4  );

        NvmCtx.JoinChannelGroupsCurrentIndex = 0;
    }
    // Check other channels
    if( nextChanParams->Datarate >= DR_4 )
    {
        if( ( NvmCtx.ChannelsMaskRemaining[4] & CHANNELS_MASK_500KHZ_MASK ) == 0 )
        {
            NvmCtx.ChannelsMaskRemaining[4] = NvmCtx.ChannelsMask[4];
        }
    }

    // Search how many channels are enabled
    countChannelsParams.Joined = nextChanParams->Joined;
    countChannelsParams.Datarate = nextChanParams->Datarate;
    countChannelsParams.ChannelsMask = NvmCtx.ChannelsMaskRemaining;
    countChannelsParams.Channels = NvmCtx.Channels;
    countChannelsParams.Bands = NvmCtx.Bands;
    countChannelsParams.MaxNbChannels = US915_MAX_NB_CHANNELS;
    countChannelsParams.JoinChannels = 0;

    identifyChannelsParam.AggrTimeOff = nextChanParams->AggrTimeOff;
    identifyChannelsParam.LastAggrTx = nextChanParams->LastAggrTx;
    identifyChannelsParam.DutyCycleEnabled = nextChanParams->DutyCycleEnabled;
    identifyChannelsParam.MaxBands = US915_MAX_NB_BANDS;

    identifyChannelsParam.CountNbOfEnabledChannelsParam = &countChannelsParams;

    identifyChannelsParam.ElapsedTimeSinceStartUp = nextChanParams->ElapsedTimeSinceStartUp;
    identifyChannelsParam.LastTxIsJoinRequest = nextChanParams->LastTxIsJoinRequest;
    identifyChannelsParam.ExpectedTimeOnAir = GetTimeOnAir( nextChanParams->Datarate, nextChanParams->PktLen );

    status = RegionCommonIdentifyChannels( &identifyChannelsParam, aggregatedTimeOff, enabledChannels,
                                           &nbEnabledChannels, &nbRestrictedChannels, time );

    if( status == LORAMAC_STATUS_OK )
    {
        if( nextChanParams->Joined == true )
        {
            // Choose randomly on of the remaining channels
            *channel = enabledChannels[randr( 0, nbEnabledChannels - 1 )];
        }
        else
        {
            // For rapid network acquisition in mixed gateway channel plan environments, the device
            // follow a random channel selection sequence. It probes alternating one out of a
            // group of eight 125 kHz channels followed by probing one 500 kHz channel each pass.
            // Each time a 125 kHz channel will be selected from another group.

            // 125kHz Channels (0 - 63) DR0
            if( nextChanParams->Datarate == DR_0 )
            {
                if( ComputeNext125kHzJoinChannel( &newChannelIndex ) == LORAMAC_STATUS_PARAMETER_INVALID )
                {
                    return LORAMAC_STATUS_PARAMETER_INVALID;
                }
                *channel = newChannelIndex;
            }
            // 500kHz Channels (64 - 71) DR4
            else
            {
                // Choose the next available channel
                uint8_t i = 0;
                while( ( ( NvmCtx.ChannelsMaskRemaining[4] & CHANNELS_MASK_500KHZ_MASK ) & ( 1 << i ) ) == 0 )
                {
                    i++;
                }
                *channel = 64 + i;
            }
        }

        // Disable the channel in the mask
        RegionCommonChanDisable( NvmCtx.ChannelsMaskRemaining, *channel, US915_MAX_NB_CHANNELS );
    }
    return status;
}

LoRaMacStatus_t RegionUS915ChannelAdd( ChannelAddParams_t* channelAdd )
{
    return LORAMAC_STATUS_PARAMETER_INVALID;
}

bool RegionUS915ChannelsRemove( ChannelRemoveParams_t* channelRemove  )
{
    return LORAMAC_STATUS_PARAMETER_INVALID;
}

void RegionUS915SetContinuousWave( ContinuousWaveParams_t* continuousWave )
{
    int8_t txPowerLimited = LimitTxPower( continuousWave->TxPower, NvmCtx.Bands[NvmCtx.Channels[continuousWave->Channel].Band].TxMaxPower, continuousWave->Datarate, NvmCtx.ChannelsMask );
    int8_t phyTxPower = 0;
    uint32_t frequency = NvmCtx.Channels[continuousWave->Channel].Frequency;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, US915_DEFAULT_MAX_ERP, 0 );

    Radio.SetTxContinuousWave( frequency, phyTxPower, continuousWave->Timeout );
}

uint8_t RegionUS915ApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset )
{
    int8_t datarate = DatarateOffsetsUS915[dr][drOffset];

    if( datarate < 0 )
    {
        datarate = DR_0;
    }
    return datarate;
}

void RegionUS915RxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr )
{
    RegionCommonRxBeaconSetupParams_t regionCommonRxBeaconSetup;

    regionCommonRxBeaconSetup.Datarates = DataratesUS915;
    regionCommonRxBeaconSetup.Frequency = rxBeaconSetup->Frequency;
    regionCommonRxBeaconSetup.BeaconSize = US915_BEACON_SIZE;
    regionCommonRxBeaconSetup.BeaconDatarate = US915_BEACON_CHANNEL_DR;
    regionCommonRxBeaconSetup.BeaconChannelBW = US915_BEACON_CHANNEL_BW;
    regionCommonRxBeaconSetup.RxTime = rxBeaconSetup->RxTime;
    regionCommonRxBeaconSetup.SymbolTimeout = rxBeaconSetup->SymbolTimeout;

    RegionCommonRxBeaconSetup( &regionCommonRxBeaconSetup );

    // Store downlink datarate
    *outDr = US915_BEACON_CHANNEL_DR;
}
