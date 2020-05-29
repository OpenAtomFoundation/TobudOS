/*!
 * \file      RegionCommon.h
 *
 * \brief     Region independent implementations which are common to all regions.
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
 *
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * \defgroup  REGIONCOMMON Common region implementation
 *            Region independent implementations which are common to all regions.
 * \{
 */
#ifndef __REGIONCOMMON_H__
#define __REGIONCOMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LoRaMacTypes.h"
#include "LoRaMacHeaderTypes.h"
#include "region/Region.h"

/*!
 * Default ping slots periodicity
 *
 * Periodicity is equal to 2^REGION_COMMON_DEFAULT_PING_SLOT_PERIODICITY seconds.
 * Example: 2^7 = 128 seconds. The end-device will open an Rx slot every 128 seconds.
 */
#define REGION_COMMON_DEFAULT_PING_SLOT_PERIODICITY     7

typedef struct sRegionCommonLinkAdrParams
{
    /*!
     * Number of repetitions.
     */
    uint8_t NbRep;
    /*!
     * Datarate.
     */
    int8_t Datarate;
    /*!
     * Tx power.
     */
    int8_t TxPower;
    /*!
     * Channels mask control field.
     */
    uint8_t ChMaskCtrl;
    /*!
     * Channels mask field.
     */
    uint16_t ChMask;
}RegionCommonLinkAdrParams_t;

typedef struct sRegionCommonLinkAdrReqVerifyParams
{
    /*!
     * LoRaWAN specification Version
     */
    Version_t Version;
    /*!
     * The current status of the AdrLinkRequest.
     */
    uint8_t Status;
    /*!
     * Set to true, if ADR is enabled.
     */
    bool AdrEnabled;
    /*!
     * The datarate the AdrLinkRequest wants to set.
     */
    int8_t Datarate;
    /*!
     * The TX power the AdrLinkRequest wants to set.
     */
    int8_t TxPower;
    /*!
     * The number of repetitions the AdrLinkRequest wants to set.
     */
    uint8_t NbRep;
    /*!
     * The current datarate the node is using.
     */
    int8_t CurrentDatarate;
    /*!
     * The current TX power the node is using.
     */
    int8_t CurrentTxPower;
    /*!
     * The current number of repetitions the node is using.
     */
    int8_t CurrentNbRep;
    /*!
     * The number of channels.
     */
    uint8_t NbChannels;
    /*!
     * Pointer to the first element of the channels mask.
     */
    uint16_t* ChannelsMask;
    /*!
     * The minimum possible datarate.
     */
    int8_t MinDatarate;
    /*!
     * The maximum possible datarate.
     */
    int8_t MaxDatarate;
    /*!
     * Pointer to the channels.
     */
    ChannelParams_t* Channels;
    /*!
     * The minimum possible TX power.
     */
    int8_t MinTxPower;
    /*!
     * The maximum possible TX power.
     */
    int8_t MaxTxPower;
}RegionCommonLinkAdrReqVerifyParams_t;

typedef struct sRegionCommonRxBeaconSetupParams
{
    /*!
     * A pointer to the available datarates.
     */
    const uint8_t* Datarates;
    /*!
     * Frequency
     */
    uint32_t Frequency;
    /*!
     * The size of the beacon frame.
     */
    uint8_t BeaconSize;
    /*!
     * The datarate of the beacon.
     */
    uint8_t BeaconDatarate;
    /*!
     * The channel bandwidth of the beacon.
     */
    uint8_t BeaconChannelBW;
    /*!
     * The RX time.
     */
    uint32_t RxTime;
    /*!
     * The symbol timeout of the RX procedure.
     */
    uint16_t SymbolTimeout;
}RegionCommonRxBeaconSetupParams_t;

typedef struct sRegionCommonCountNbOfEnabledChannelsParams
{
    /*!
     * Set to true, if the device is joined.
     */
    bool Joined;
    /*!
     * The datarate to count the available channels.
     */
    uint8_t Datarate;
    /*!
     * A pointer to the channels mask to verify.
     */
    uint16_t* ChannelsMask;
    /*!
     * A pointer to the channels.
     */
    ChannelParams_t* Channels;
    /*!
     * A pointer to the bands.
     */
    Band_t* Bands;
    /*!
     * The number of available channels.
     */
    uint16_t MaxNbChannels;
    /*!
     * A bitmask containing the join channels.
     */
    uint16_t JoinChannels;
}RegionCommonCountNbOfEnabledChannelsParams_t;

typedef struct sRegionCommonIdentifyChannelsParam
{
    /*!
     * Aggregated time-off time.
     */
    TimerTime_t AggrTimeOff;
    /*!
     * Time of the last aggregated TX.
     */
    TimerTime_t LastAggrTx;
    /*!
     * Set to true, if the duty cycle is enabled, otherwise false.
     */
    bool DutyCycleEnabled;
    /*!
     * Maximum number of bands.
     */
    uint8_t MaxBands;
    /*!
     * Elapsed time since the start of the node.
     */
    SysTime_t ElapsedTimeSinceStartUp;
    /*!
     * Joined Set to true, if the last uplink was a join request
     */
    bool LastTxIsJoinRequest;
    /*!
     * Expected time-on-air
     */
    TimerTime_t ExpectedTimeOnAir;
    /*!
     * Pointer to a structure of RegionCommonCountNbOfEnabledChannelsParams_t.
     */
    RegionCommonCountNbOfEnabledChannelsParams_t* CountNbOfEnabledChannelsParam;
}RegionCommonIdentifyChannelsParam_t;

typedef struct sRegionCommonSetDutyCycleParams
{
    /*!
     * Duty cycle period.
     */
    TimerTime_t DutyCycleTimePeriod;
    /*!
     * Number of bands available.
     */
    uint8_t MaxBands;
    /*!
     * A pointer to the bands.
     */
    Band_t* Bands;
}RegionCommonSetDutyCycleParams_t;

/*!
 * \brief Calculates the join duty cycle.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] elapsedTime Elapsed time since the start of the device.
 *
 * \retval Duty cycle restriction.
 */
uint16_t RegionCommonGetJoinDc( SysTime_t elapsedTime );

/*!
 * \brief Verifies, if a value is in a given range.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] value Value to verify, if it is in range.
 *
 * \param [IN] min Minimum possible value.
 *
 * \param [IN] max Maximum possible value.
 *
 * \retval Returns 1 if the value is in range, otherwise 0.
 */
uint8_t RegionCommonValueInRange( int8_t value, int8_t min, int8_t max );

/*!
 * \brief Verifies, if a datarate is available on an active channel.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] nbChannels Number of channels.
 *
 * \param [IN] channelsMask The channels mask of the region.
 *
 * \param [IN] dr The datarate to verify.
 *
 * \param [IN] minDr Minimum datarate.
 *
 * \param [IN] maxDr Maximum datarate.
 *
 * \param [IN] channels The channels of the region.
 *
 * \retval Returns true if the datarate is supported, false if not.
 */
bool RegionCommonChanVerifyDr( uint8_t nbChannels, uint16_t* channelsMask, int8_t dr,
                            int8_t minDr, int8_t maxDr, ChannelParams_t* channels );

/*!
 * \brief Disables a channel in a given channels mask.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] channelsMask The channels mask of the region.
 *
 * \param [IN] id The id of the channels mask to disable.
 *
 * \param [IN] maxChannels Maximum number of channels.
 *
 * \retval Returns true if the channel could be disabled, false if not.
 */
bool RegionCommonChanDisable( uint16_t* channelsMask, uint8_t id, uint8_t maxChannels );

/*!
 * \brief Counts the number of active channels in a given channels mask.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] channelsMask The channels mask of the region.
 *
 * \param [IN] startIdx Start index.
 *
 * \param [IN] stopIdx Stop index ( the channels of this index will not be counted ).
 *
 * \retval Returns the number of active channels.
 */
uint8_t RegionCommonCountChannels( uint16_t* channelsMask, uint8_t startIdx, uint8_t stopIdx );

/*!
 * \brief Copy a channels mask.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] channelsMaskDest The destination channels mask.
 *
 * \param [IN] channelsMaskSrc The source channels mask.
 *
 * \param [IN] len The index length to copy.
 */
void RegionCommonChanMaskCopy( uint16_t* channelsMaskDest, uint16_t* channelsMaskSrc, uint8_t len );

/*!
 * \brief Sets the last tx done property.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] band The band to be updated.
 *
 * \param [IN] lastTxAirTime The time on air of the last TX frame.
 *
 * \param [IN] joined Set to true if the device has joined.
 *
 * \param [IN] elapsedTimeSinceStartup Elapsed time since initialization.
 */
void RegionCommonSetBandTxDone( Band_t* band, TimerTime_t lastTxAirTime, bool joined, SysTime_t elapsedTimeSinceStartup );

/*!
 * \brief Updates the time-offs of the bands.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] joined Set to true, if the node has joined the network
 *
 * \param [IN] bands A pointer to the bands.
 *
 * \param [IN] nbBands The number of bands available.
 *
 * \param [IN] dutyCycleEnabled Set to true, if the duty cycle is enabled.
 *
 * \param [IN] lastTxIsJoinRequest Set to true, if the last TX is a join request.
 *
 * \param [IN] elapsedTimeSinceStartup Elapsed time since start up.
 *
 * \param [IN] expectedTimeOnAir Expected time on air for the next transmission.
 *
 * \retval Returns the time which must be waited to perform the next uplink.
 */
TimerTime_t RegionCommonUpdateBandTimeOff( bool joined, Band_t* bands,
                                           uint8_t nbBands, bool dutyCycleEnabled,
                                           bool lastTxIsJoinRequest, SysTime_t elapsedTimeSinceStartup,
                                           TimerTime_t expectedTimeOnAir );

/*!
 * \brief Parses the parameter of an LinkAdrRequest.
 *        This is a generic function and valid for all regions.
 *
 * \param [IN] payload Pointer to the payload containing the MAC commands. The payload
 *                     must contain the CMD identifier, following by the parameters.
 *
 * \param [OUT] parseLinkAdr The function fills the structure with the ADR parameters.
 *
 * \retval Returns the length of the ADR request, if a request was found. Otherwise, the
 *         function returns 0.
 */
uint8_t RegionCommonParseLinkAdrReq( uint8_t* payload, RegionCommonLinkAdrParams_t* parseLinkAdr );

/*!
 * \brief Verifies and updates the datarate, the TX power and the number of repetitions
 *        of a LinkAdrRequest. This depends on the configuration of ADR also.
 *
 * \param [IN] verifyParams Pointer to a structure containing input parameters.
 *
 * \param [OUT] dr The updated datarate.
 *
 * \param [OUT] txPow The updated TX power.
 *
 * \param [OUT] nbRep The updated number of repetitions.
 *
 * \retval Returns the status according to the LinkAdrRequest definition.
 */
uint8_t RegionCommonLinkAdrReqVerifyParams( RegionCommonLinkAdrReqVerifyParams_t* verifyParams, int8_t* dr, int8_t* txPow, uint8_t* nbRep );

/*!
 * \brief Computes the symbol time for LoRa modulation.
 *
 * \param [IN] phyDr Physical datarate to use.
 *
 * \param [IN] bandwidth Bandwidth to use.
 *
 * \retval Returns the symbol time.
 */
double RegionCommonComputeSymbolTimeLoRa( uint8_t phyDr, uint32_t bandwidth );

/*!
 * \brief Computes the symbol time for FSK modulation.
 *
 * \param [IN] phyDr Physical datarate to use.
 *
 * \param [IN] bandwidth Bandwidth to use.
 *
 * \retval Returns the symbol time.
 */
double RegionCommonComputeSymbolTimeFsk( uint8_t phyDr );

/*!
 * \brief Computes the RX window timeout and the RX window offset.
 *
 * \param [IN] tSymbol Symbol timeout.
 *
 * \param [IN] minRxSymbols Minimum required number of symbols to detect an Rx frame.
 *
 * \param [IN] rxError System maximum timing error of the receiver. In milliseconds
 *                     The receiver will turn on in a [-rxError : +rxError] ms interval around RxOffset.
 *
 * \param [IN] wakeUpTime Wakeup time of the system.
 *
 * \param [OUT] windowTimeout RX window timeout.
 *
 * \param [OUT] windowOffset RX window time offset to be applied to the RX delay.
 */
void RegionCommonComputeRxWindowParameters( double tSymbol, uint8_t minRxSymbols, uint32_t rxError, uint32_t wakeUpTime, uint32_t* windowTimeout, int32_t* windowOffset );

/*!
 * \brief Computes the txPower, based on the max EIRP and the antenna gain.
 *
 * \remark US915 region uses a conducted power as input value for maxEirp.
 *         Thus, the antennaGain parameter must be set to 0.
 *
 * \param [IN] txPower TX power index.
 *
 * \param [IN] maxEirp Maximum EIRP.
 *
 * \param [IN] antennaGain Antenna gain. Referenced to the isotropic antenna.
 *                         Value is in dBi. ( antennaGain[dBi] = measuredAntennaGain[dBd] + 2.15 )
 *
 * \retval Returns the physical TX power.
 */
int8_t RegionCommonComputeTxPower( int8_t txPowerIndex, float maxEirp, float antennaGain );

/*!
 * \brief Sets up the radio into RX beacon mode.
 *
 * \param [IN] rxBeaconSetupParams A pointer to the input parameters.
 */
void RegionCommonRxBeaconSetup( RegionCommonRxBeaconSetupParams_t* rxBeaconSetupParams );

/*!
 * \brief Counts the number of enabled channels.
 *
 * \param [IN] countNbOfEnabledChannelsParams A pointer to the input parameters.
 *
 * \param [OUT] enabledChannels A pointer to an array of size XX_MAX_NB_CHANNELS. The function
 *              stores the available channels into this array.
 *
 * \param [OUT] nbEnabledChannels The number of available channels found.
 *
 * \param [OUT] nbRestrictedChannels It contains the number of channel
 *                      which are available, but restricted due to duty cycle.
 */
void RegionCommonCountNbOfEnabledChannels( RegionCommonCountNbOfEnabledChannelsParams_t* countNbOfEnabledChannelsParams,
                                           uint8_t* enabledChannels, uint8_t* nbEnabledChannels, uint8_t* nbRestrictedChannels );

/*!
 * \brief Identifies all channels which are available currently.
 *
 * \param [IN] identifyChannelsParam A pointer to the input parameters.
 *
 * \param [OUT] aggregatedTimeOff The new value of the aggregatedTimeOff. The function
 *                                may resets it to 0.
 *
 * \param [OUT] enabledChannels A pointer to an array of size XX_MAX_NB_CHANNELS. The function
 *              stores the available channels into this array.
 *
 * \param [OUT] nbEnabledChannels The number of available channels found.
 *
 * \param [OUT] nbRestrictedChannels It contains the number of channel
 *                      which are available, but restricted due to duty cycle.
 *
 * \param [OUT] nextTxDelay Holds the time which has to be waited for the next possible
 *                          uplink transmission.
 *
 *\retval Status of the operation.
 */
LoRaMacStatus_t RegionCommonIdentifyChannels( RegionCommonIdentifyChannelsParam_t* identifyChannelsParam,
                                              TimerTime_t* aggregatedTimeOff, uint8_t* enabledChannels,
                                              uint8_t* nbEnabledChannels, uint8_t* nbRestrictedChannels,
                                              TimerTime_t* nextTxDelay );

/*! \} defgroup REGIONCOMMON */

#ifdef __cplusplus
}
#endif

#endif // __REGIONCOMMON_H__
