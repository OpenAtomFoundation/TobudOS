/*!
 * \file      RegionCommon.c
 *
 * \brief     LoRa MAC common region implementation
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
#include <math.h>
#include "radio.h"
#include "utilities.h"
#include "RegionCommon.h"

#define BACKOFF_DC_1_HOUR                   100
#define BACKOFF_DC_10_HOURS                 1000
#define BACKOFF_DC_24_HOURS                 10000
#define BACKOFF_DC_TIMER_PERIOD_FACTOR      100

#ifndef DUTY_CYCLE_TIME_PERIOD
/*!
 * Default duty cycle time period is 1 hour = 3600000 ms
 */
#define DUTY_CYCLE_TIME_PERIOD              3600000
#endif

static uint16_t GetDutyCycle( Band_t* band, bool joined, SysTime_t elapsedTimeSinceStartup )
{
    uint16_t joinDutyCycle = RegionCommonGetJoinDc( elapsedTimeSinceStartup );
    uint16_t dutyCycle = band->DCycle;

    if( joined == false )
    {
        // Get the join duty cycle which depends on the runtime
        joinDutyCycle = RegionCommonGetJoinDc( elapsedTimeSinceStartup );
        // Take the most restrictive duty cycle
        dutyCycle = MAX( dutyCycle, joinDutyCycle );
    }

    // Prevent value of 0
    if( dutyCycle == 0 )
    {
        dutyCycle = 1;
    }

    return dutyCycle;
}

static uint16_t SetMaxTimeCredits( Band_t* band, bool joined, SysTime_t elapsedTimeSinceStartup )
{
    uint16_t dutyCycle = band->DCycle;
    uint8_t timePeriodFactor = 1;

    // Get the band duty cycle. If not joined, the function either returns the join duty cycle
    // or the band duty cycle, whichever is more restrictive.
    dutyCycle = GetDutyCycle( band, joined, elapsedTimeSinceStartup );

    if( joined == false )
    {
        // Apply a factor to increase the maximum time period of observation
        timePeriodFactor = dutyCycle / BACKOFF_DC_TIMER_PERIOD_FACTOR;
    }

    // Setup the maximum allowed credits
    band->MaxTimeCredits = DUTY_CYCLE_TIME_PERIOD * timePeriodFactor;

    // In case if it is the first time, update also the current
    // time credits
    if( band->LastBandUpdateTime == 0 )
    {
        band->TimeCredits = band->MaxTimeCredits;
    }

    return dutyCycle;
}

static uint16_t UpdateTimeCredits( Band_t* band, bool joined, bool dutyCycleEnabled,
                                   bool lastTxIsJoinRequest, SysTime_t elapsedTimeSinceStartup,
                                   TimerTime_t currentTime )
{
    uint16_t dutyCycle = SetMaxTimeCredits( band, joined, elapsedTimeSinceStartup );

    if( joined == false )
    {
        if( ( dutyCycleEnabled == false ) &&
            ( lastTxIsJoinRequest == false ) )
        {
            // This is the case when the duty cycle is off and the last uplink frame was not a join.
            // This could happen in case of a rejoin, e.g. in compliance test mode.
            // In this special case we have to set the time off to 0, since the join duty cycle shall only
            // be applied after the first join request.
            band->TimeCredits = band->MaxTimeCredits;
        }
    }
    else
    {
        if( dutyCycleEnabled == false )
        {
            band->TimeCredits = band->MaxTimeCredits;
        }
    }

    // Get the difference between now and the last update
    band->TimeCredits += TimerGetElapsedTime( band->LastBandUpdateTime );

    // Limit band credits to maximum
    if( band->TimeCredits > band->MaxTimeCredits )
    {
        band->TimeCredits = band->MaxTimeCredits;
    }

    // Synchronize update time
    band->LastBandUpdateTime = currentTime;

    return dutyCycle;
}

static uint8_t CountChannels( uint16_t mask, uint8_t nbBits )
{
    uint8_t nbActiveBits = 0;

    for( uint8_t j = 0; j < nbBits; j++ )
    {
        if( ( mask & ( 1 << j ) ) == ( 1 << j ) )
        {
            nbActiveBits++;
        }
    }
    return nbActiveBits;
}

uint16_t RegionCommonGetJoinDc( SysTime_t elapsedTime )
{
    uint16_t dutyCycle = 0;

    if( elapsedTime.Seconds < 3600 )
    {
        dutyCycle = BACKOFF_DC_1_HOUR;
    }
    else if( elapsedTime.Seconds < ( 3600 + 36000 ) )
    {
        dutyCycle = BACKOFF_DC_10_HOURS;
    }
    else
    {
        dutyCycle = BACKOFF_DC_24_HOURS;
    }
    return dutyCycle;
}

bool RegionCommonChanVerifyDr( uint8_t nbChannels, uint16_t* channelsMask, int8_t dr, int8_t minDr, int8_t maxDr, ChannelParams_t* channels )
{
    if( RegionCommonValueInRange( dr, minDr, maxDr ) == 0 )
    {
        return false;
    }

    for( uint8_t i = 0, k = 0; i < nbChannels; i += 16, k++ )
    {
        for( uint8_t j = 0; j < 16; j++ )
        {
            if( ( ( channelsMask[k] & ( 1 << j ) ) != 0 ) )
            {// Check datarate validity for enabled channels
                if( RegionCommonValueInRange( dr, ( channels[i + j].DrRange.Fields.Min & 0x0F ),
                                                  ( channels[i + j].DrRange.Fields.Max & 0x0F ) ) == 1 )
                {
                    // At least 1 channel has been found we can return OK.
                    return true;
                }
            }
        }
    }
    return false;
}

uint8_t RegionCommonValueInRange( int8_t value, int8_t min, int8_t max )
{
    if( ( value >= min ) && ( value <= max ) )
    {
        return 1;
    }
    return 0;
}

bool RegionCommonChanDisable( uint16_t* channelsMask, uint8_t id, uint8_t maxChannels )
{
    uint8_t index = id / 16;

    if( ( index > ( maxChannels / 16 ) ) || ( id >= maxChannels ) )
    {
        return false;
    }

    // Deactivate channel
    channelsMask[index] &= ~( 1 << ( id % 16 ) );

    return true;
}

uint8_t RegionCommonCountChannels( uint16_t* channelsMask, uint8_t startIdx, uint8_t stopIdx )
{
    uint8_t nbChannels = 0;

    if( channelsMask == NULL )
    {
        return 0;
    }

    for( uint8_t i = startIdx; i < stopIdx; i++ )
    {
        nbChannels += CountChannels( channelsMask[i], 16 );
    }

    return nbChannels;
}

void RegionCommonChanMaskCopy( uint16_t* channelsMaskDest, uint16_t* channelsMaskSrc, uint8_t len )
{
    if( ( channelsMaskDest != NULL ) && ( channelsMaskSrc != NULL ) )
    {
        for( uint8_t i = 0; i < len; i++ )
        {
            channelsMaskDest[i] = channelsMaskSrc[i];
        }
    }
}

void RegionCommonSetBandTxDone( Band_t* band, TimerTime_t lastTxAirTime, bool joined, SysTime_t elapsedTimeSinceStartup )
{
    // Get the band duty cycle. If not joined, the function either returns the join duty cycle
    // or the band duty cycle, whichever is more restrictive.
    uint16_t dutyCycle = GetDutyCycle( band, joined, elapsedTimeSinceStartup );

    // Reduce with transmission time
    if( band->TimeCredits > ( lastTxAirTime * dutyCycle ) )
    {
        // Reduce time credits by the time of air
        band->TimeCredits -= ( lastTxAirTime * dutyCycle );
    }
    else
    {
        band->TimeCredits = 0;
    }
}

TimerTime_t RegionCommonUpdateBandTimeOff( bool joined, Band_t* bands,
                                           uint8_t nbBands, bool dutyCycleEnabled,
                                           bool lastTxIsJoinRequest, SysTime_t elapsedTimeSinceStartup,
                                           TimerTime_t expectedTimeOnAir )
{
    TimerTime_t minTimeToWait = TIMERTIME_T_MAX;
    TimerTime_t currentTime = TimerGetCurrentTime( );
    TimerTime_t creditCosts = 0;
    uint16_t dutyCycle = 1;
    uint8_t validBands = 0;

    for( uint8_t i = 0; i < nbBands; i++ )
    {
        // Synchronization of bands and credits
        dutyCycle = UpdateTimeCredits( &bands[i], joined, dutyCycleEnabled,
                                       lastTxIsJoinRequest, elapsedTimeSinceStartup,
                                       currentTime );

        // Calculate the credit costs for the next transmission
        // with the duty cycle and the expected time on air
        creditCosts = expectedTimeOnAir * dutyCycle;

        // Check if the band is ready for transmission. Its ready,
        // when the duty cycle is off, or the TimeCredits of the band
        // is higher than the credit costs for the transmission.
        if( ( bands[i].TimeCredits > creditCosts ) ||
            ( dutyCycleEnabled == false ) )
        {
            bands[i].ReadyForTransmission = true;
            // This band is a potential candidate for an
            // upcoming transmission, so increase the counter.
            validBands++;
        }
        else
        {
            // In this case, the band has not enough credits
            // for the next transmission.
            bands[i].ReadyForTransmission = false;

            if( bands[i].MaxTimeCredits > creditCosts )
            {
                // The band can only be taken into account, if the maximum credits
                // of the band are higher than the credit costs.
                // We calculate the minTimeToWait among the bands which are not
                // ready for transmission and which are potentially available
                // for a transmission in the future.
                minTimeToWait = MIN( minTimeToWait, ( creditCosts - bands[i].TimeCredits ) );
                // This band is a potential candidate for an
                // upcoming transmission (even if its time credits are not enough
                // at the moment), so increase the counter.
                validBands++;
            }
        }
    }


    if( validBands == 0 )
    {
        // There is no valid band available to handle a transmission
        // in the given DUTY_CYCLE_TIME_PERIOD.
        return TIMERTIME_T_MAX;
    }
    return minTimeToWait;
}

uint8_t RegionCommonParseLinkAdrReq( uint8_t* payload, RegionCommonLinkAdrParams_t* linkAdrParams )
{
    uint8_t retIndex = 0;

    if( payload[0] == SRV_MAC_LINK_ADR_REQ )
    {
        // Parse datarate and tx power
        linkAdrParams->Datarate = payload[1];
        linkAdrParams->TxPower = linkAdrParams->Datarate & 0x0F;
        linkAdrParams->Datarate = ( linkAdrParams->Datarate >> 4 ) & 0x0F;
        // Parse ChMask
        linkAdrParams->ChMask = ( uint16_t )payload[2];
        linkAdrParams->ChMask |= ( uint16_t )payload[3] << 8;
        // Parse ChMaskCtrl and nbRep
        linkAdrParams->NbRep = payload[4];
        linkAdrParams->ChMaskCtrl = ( linkAdrParams->NbRep >> 4 ) & 0x07;
        linkAdrParams->NbRep &= 0x0F;

        // LinkAdrReq has 4 bytes length + 1 byte CMD
        retIndex = 5;
    }
    return retIndex;
}

uint8_t RegionCommonLinkAdrReqVerifyParams( RegionCommonLinkAdrReqVerifyParams_t* verifyParams, int8_t* dr, int8_t* txPow, uint8_t* nbRep )
{
    uint8_t status = verifyParams->Status;
    int8_t datarate = verifyParams->Datarate;
    int8_t txPower = verifyParams->TxPower;
    int8_t nbRepetitions = verifyParams->NbRep;

    // Handle the case when ADR is off.
    if( verifyParams->AdrEnabled == false )
    {
        // When ADR is off, we are allowed to change the channels mask
        nbRepetitions = verifyParams->CurrentNbRep;
        datarate =  verifyParams->CurrentDatarate;
        txPower =  verifyParams->CurrentTxPower;
    }

    if( status != 0 )
    {
        // Verify datarate. The variable phyParam. Value contains the minimum allowed datarate.
        if( RegionCommonChanVerifyDr( verifyParams->NbChannels, verifyParams->ChannelsMask, datarate,
                                      verifyParams->MinDatarate, verifyParams->MaxDatarate, verifyParams->Channels  ) == false )
        {
            status &= 0xFD; // Datarate KO
        }

        // Verify tx power
        if( RegionCommonValueInRange( txPower, verifyParams->MaxTxPower, verifyParams->MinTxPower ) == 0 )
        {
            // Verify if the maximum TX power is exceeded
            if( verifyParams->MaxTxPower > txPower )
            { // Apply maximum TX power. Accept TX power.
                txPower = verifyParams->MaxTxPower;
            }
            else
            {
                status &= 0xFB; // TxPower KO
            }
        }
    }

    // If the status is ok, verify the NbRep
    if( status == 0x07 )
    {
        if( nbRepetitions == 0 )
        { // Restore the default value according to the LoRaWAN specification
            nbRepetitions = 1;
        }
    }

    // Apply changes
    *dr = datarate;
    *txPow = txPower;
    *nbRep = nbRepetitions;

    return status;
}

double RegionCommonComputeSymbolTimeLoRa( uint8_t phyDr, uint32_t bandwidth )
{
    return ( ( double )( 1 << phyDr ) / ( double )bandwidth ) * 1000;
}

double RegionCommonComputeSymbolTimeFsk( uint8_t phyDr )
{
    return ( 8.0 / ( double )phyDr ); // 1 symbol equals 1 byte
}

void RegionCommonComputeRxWindowParameters( double tSymbol, uint8_t minRxSymbols, uint32_t rxError, uint32_t wakeUpTime, uint32_t* windowTimeout, int32_t* windowOffset )
{
    *windowTimeout = MAX( ( uint32_t )ceil( ( ( 2 * minRxSymbols - 8 ) * tSymbol + 2 * rxError ) / tSymbol ), minRxSymbols ); // Computed number of symbols
    *windowOffset = ( int32_t )ceil( ( 4.0 * tSymbol ) - ( ( *windowTimeout * tSymbol ) / 2.0 ) - wakeUpTime );
}

int8_t RegionCommonComputeTxPower( int8_t txPowerIndex, float maxEirp, float antennaGain )
{
    int8_t phyTxPower = 0;

    phyTxPower = ( int8_t )floor( ( maxEirp - ( txPowerIndex * 2U ) ) - antennaGain );

    return phyTxPower;
}

void RegionCommonRxBeaconSetup( RegionCommonRxBeaconSetupParams_t* rxBeaconSetupParams )
{
    bool rxContinuous = true;
    uint8_t datarate;

    // Set the radio into sleep mode
    Radio.Sleep( );

    // Setup frequency and payload length
    Radio.SetChannel( rxBeaconSetupParams->Frequency );
    Radio.SetMaxPayloadLength( MODEM_LORA, rxBeaconSetupParams->BeaconSize );

    // Check the RX continuous mode
    if( rxBeaconSetupParams->RxTime != 0 )
    {
        rxContinuous = false;
    }

    // Get region specific datarate
    datarate = rxBeaconSetupParams->Datarates[rxBeaconSetupParams->BeaconDatarate];

    // Setup radio
    Radio.SetRxConfig( MODEM_LORA, rxBeaconSetupParams->BeaconChannelBW, datarate,
                       1, 0, 10, rxBeaconSetupParams->SymbolTimeout, true, rxBeaconSetupParams->BeaconSize, false, 0, 0, false, rxContinuous );

    Radio.Rx( rxBeaconSetupParams->RxTime );
}

void RegionCommonCountNbOfEnabledChannels( RegionCommonCountNbOfEnabledChannelsParams_t* countNbOfEnabledChannelsParams,
                                           uint8_t* enabledChannels, uint8_t* nbEnabledChannels, uint8_t* nbRestrictedChannels )
{
    uint8_t nbChannelCount = 0;
    uint8_t nbRestrictedChannelsCount = 0;

    for( uint8_t i = 0, k = 0; i < countNbOfEnabledChannelsParams->MaxNbChannels; i += 16, k++ )
    {
        for( uint8_t j = 0; j < 16; j++ )
        {
            if( ( countNbOfEnabledChannelsParams->ChannelsMask[k] & ( 1 << j ) ) != 0 )
            {
                if( countNbOfEnabledChannelsParams->Channels[i + j].Frequency == 0 )
                { // Check if the channel is enabled
                    continue;
                }
                if( ( countNbOfEnabledChannelsParams->Joined == false ) &&
                    ( countNbOfEnabledChannelsParams->JoinChannels > 0 ) )
                {
                    if( ( countNbOfEnabledChannelsParams->JoinChannels & ( 1 << j ) ) == 0 )
                    {
                        continue;
                    }
                }
                if( RegionCommonValueInRange( countNbOfEnabledChannelsParams->Datarate,
                                              countNbOfEnabledChannelsParams->Channels[i + j].DrRange.Fields.Min,
                                              countNbOfEnabledChannelsParams->Channels[i + j].DrRange.Fields.Max ) == false )
                { // Check if the current channel selection supports the given datarate
                    continue;
                }
                if( countNbOfEnabledChannelsParams->Bands[countNbOfEnabledChannelsParams->Channels[i + j].Band].ReadyForTransmission == false )
                { // Check if the band is available for transmission
                    nbRestrictedChannelsCount++;
                    continue;
                }
                enabledChannels[nbChannelCount++] = i + j;
            }
        }
    }
    *nbEnabledChannels = nbChannelCount;
    *nbRestrictedChannels = nbRestrictedChannelsCount;
}

LoRaMacStatus_t RegionCommonIdentifyChannels( RegionCommonIdentifyChannelsParam_t* identifyChannelsParam,
                                              TimerTime_t* aggregatedTimeOff, uint8_t* enabledChannels,
                                              uint8_t* nbEnabledChannels, uint8_t* nbRestrictedChannels,
                                              TimerTime_t* nextTxDelay )
{
    TimerTime_t elapsed = TimerGetElapsedTime( identifyChannelsParam->LastAggrTx );
    *nextTxDelay = identifyChannelsParam->AggrTimeOff - elapsed;
    *nbRestrictedChannels = 1;
    *nbEnabledChannels = 0;

    if( ( identifyChannelsParam->LastAggrTx == 0 ) ||
        ( identifyChannelsParam->AggrTimeOff <= elapsed ) )
    {
        // Reset Aggregated time off
        *aggregatedTimeOff = 0;

        // Update bands Time OFF
        *nextTxDelay = RegionCommonUpdateBandTimeOff( identifyChannelsParam->CountNbOfEnabledChannelsParam->Joined,
                                                      identifyChannelsParam->CountNbOfEnabledChannelsParam->Bands,
                                                      identifyChannelsParam->MaxBands,
                                                      identifyChannelsParam->DutyCycleEnabled,
                                                      identifyChannelsParam->LastTxIsJoinRequest,
                                                      identifyChannelsParam->ElapsedTimeSinceStartUp,
                                                      identifyChannelsParam->ExpectedTimeOnAir );

        RegionCommonCountNbOfEnabledChannels( identifyChannelsParam->CountNbOfEnabledChannelsParam, enabledChannels,
                                              nbEnabledChannels, nbRestrictedChannels );
    }

    if( *nbEnabledChannels > 0 )
    {
        *nextTxDelay = 0;
        return LORAMAC_STATUS_OK;
    }
    else if( *nbRestrictedChannels > 0 )
    {
        return LORAMAC_STATUS_DUTYCYCLE_RESTRICTED;
    }
    else
    {
        return LORAMAC_STATUS_NO_CHANNEL_FOUND;
    }
}
