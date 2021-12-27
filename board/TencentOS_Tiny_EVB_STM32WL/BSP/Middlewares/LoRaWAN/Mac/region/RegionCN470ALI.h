/*!
 * \file      RegionCN470ALI.h
 *
 * \brief     Region definition for CN470ALI
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
 * \defgroup  REGIONCN470ALI Region CN470ALI
 *            Implementation according to LoRaWAN Specification v1.0.2.
 * \{
 */
#ifndef __REGION_CN470ALI_H__
#define __REGION_CN470ALI_H__

#include "region/Region.h"

#define	ALI_FREQ470_S		(0)
#define ALI_FREQ470_D		(1)

#define ALI_FREQ480_S       (3)
#define ALI_FREQ480_D       (4)

/*table[1]  for CN470ALID  and  CN480ALIS*/
#define ALI_RX_WND_2_FREQ_1A1D_1B1S				 485300000
#define ALI_RX_WND_2_FREQ_1A2D_1B2S				 486900000
#define ALI_RX_WND_2_FREQ_2A1D_2B1S				 488500000
#define ALI_RX_WND_2_FREQ_2A2D_2B2S				 490100000
#define ALI_RX_WND_2_FREQ_3B1D_3A1S				 491700000
#define ALI_RX_WND_2_FREQ_3B2D_3A2S				 493300000
#define ALI_RX_WND_2_FREQ_4B1D_4A1S				 494900000
#define ALI_RX_WND_2_FREQ_4B2D_4A2S				 496500000

/*table[2]  for CN470ALIS  and  CN480ALID*/
#define ALI_RX_WND_2_FREQ_1A1S_1B1D				 471700000
#define ALI_RX_WND_2_FREQ_1A2S_1B2D				 473300000
#define ALI_RX_WND_2_FREQ_2A1S_2B1D				 474900000
#define ALI_RX_WND_2_FREQ_2A2S_2B2D				 476500000
#define ALI_RX_WND_2_FREQ_3B1S_3A1D				 504900000
#define ALI_RX_WND_2_FREQ_3B2S_3A2D				 506500000
#define ALI_RX_WND_2_FREQ_4B1S_4A1D				 508100000
#define ALI_RX_WND_2_FREQ_4B2S_4A2D				 509700000



/*TODO: add beacon freq*/



#define ALI_DOWNLINK_FREQ  		 ALI_FREQ470_D
#define ALI_RX2WIN_FREQ          ALI_RX_WND_2_FREQ_1A2D_1B2S
#define ALI_BEACON_FREQ               


/*error msg*/
#if ((ALI_DOWNLINK_FREQ == ALI_FREQ470_D)||(ALI_DOWNLINK_FREQ == ALI_FREQ480_S))
	#if (ALI_RX2WIN_FREQ < ALI_RX_WND_2_FREQ_1A1D_1B1S) ||((ALI_RX2WIN_FREQ > ALI_RX_WND_2_FREQ_4B2D_4A2S))
		#error ======>Incorrect   ALI_RX2WIN_FREQ  , you should use the value in table[1] 
	#endif
#else
	#if	((ALI_RX2WIN_FREQ < ALI_RX_WND_2_FREQ_1A1S_1B1D)||(ALI_RX2WIN_FREQ > ALI_RX_WND_2_FREQ_4B2S_4A2D))
		#error ======>Incorrect   ALI_RX2WIN_FREQ  , you should use the value in table[2] 
	#endif	
	#if	((ALI_RX2WIN_FREQ > ALI_RX_WND_2_FREQ_2A2S_2B2D) && (ALI_RX2WIN_FREQ < ALI_RX_WND_2_FREQ_3B1S_3A1D))
		##error ======>Incorrect   ALI_RX2WIN_FREQ  , you should use the value in table[2] 
	#endif		
#endif

#ifndef ALI_DOWNLINK_FREQ
	#error ALI_DOWNLINK_FREQ not defined
#endif
#ifndef ALI_RX2WIN_FREQ
    #erorr ALI_RX2WIN_FREQ not defined
#endif

/*!
 * LoRaMac maximum number of channels
 */
#define CN470ALI_MAX_NB_CHANNELS                        64

/*!
 * Minimal datarate that can be used by the node
 */
#define CN470ALI_TX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define CN470ALI_TX_MAX_DATARATE                       DR_5

/*!
 * Minimal datarate that can be used by the node
 */
#define CN470ALI_RX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define CN470ALI_RX_MAX_DATARATE                       DR_5

/*!
 * Default datarate used by the node
 */
 
/*===============================ALI_Region==============================================*/
#define CN470ALI_DEFAULT_DATARATE                      DR_2//primitive_cn470 DR_0
/*=======================================================================================*/

/*!
 * Minimal Rx1 receive datarate offset
 */
#define CN470ALI_MIN_RX1_DR_OFFSET                     0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define CN470ALI_MAX_RX1_DR_OFFSET                     3

/*!
 * Default Rx1 receive datarate offset
 */
#define CN470ALI_DEFAULT_RX1_DR_OFFSET                 0

/*!
 * Minimal Tx output power that can be used by the node
 */
#define CN470ALI_MIN_TX_POWER                        TX_POWER_7

/*!
 * Maximal Tx output power that can be used by the node
 */
#define CN470ALI_MAX_TX_POWER                        TX_POWER_0

/*!
 * Default Tx output power used by the node
 */
#define CN470ALI_DEFAULT_TX_POWER                    TX_POWER_0

/*!
 * Default Max EIRP
 */
#define CN470ALI_DEFAULT_MAX_EIRP                      19.15f

/*!
 * Default antenna gain
 */
#define CN470ALI_DEFAULT_ANTENNA_GAIN                  2.15f

/*!
 * ADR Ack limit
 */
#define CN470ALI_ADR_ACK_LIMIT                         64

/*!
 * ADR Ack delay
 */
#define CN470ALI_ADR_ACK_DELAY                         32

/*!
 * Enabled or disabled the duty cycle
 */
#define CN470ALI_DUTY_CYCLE_ENABLED                    0

/*!
 * Maximum RX window duration
 */
#define CN470ALI_MAX_RX_WINDOW                         3000

/*!
 * Receive delay 1
 */
#define CN470ALI_RECEIVE_DELAY1                        1000

/*!
 * Receive delay 2
 */
#define CN470ALI_RECEIVE_DELAY2                        2000

/*!
 * Join accept delay 1
 */
#define CN470ALI_JOIN_ACCEPT_DELAY1                    5000

/*!
 * Join accept delay 2
 */
#define CN470ALI_JOIN_ACCEPT_DELAY2                    6000

/*!
 * Maximum frame counter gap
 */
#define CN470ALI_MAX_FCNT_GAP                          16384

/*!
 * Ack timeout
 */
#define CN470ALI_ACKTIMEOUT                            5500

/*!
 * Random ack timeout limits
 */
#define CN470ALI_ACK_TIMEOUT_RND                       500


/*!
 * Second reception window channel datarate definition.
 */
#define CN470ALI_RX_WND_2_DR                           DR_0

/*
 * CLASS B
 */
/*!
 * Beacon frequency
 */
#define CN470ALI_BEACON_CHANNEL_FREQ                   485500000

/*!
 * Beacon frequency channel stepwidth
 */
#define CN470ALI_BEACON_CHANNEL_STEPWIDTH              200000

/*!
 * Number of possible beacon channels
 */
#define CN470ALI_BEACON_NB_CHANNELS                    8

/*!
 * Payload size of a beacon frame
 */
#define CN470ALI_BEACON_SIZE                           19

/*!
 * Size of RFU 1 field
 */
#define CN470ALI_RFU1_SIZE                             3

/*!
 * Size of RFU 2 field
 */
#define CN470ALI_RFU2_SIZE                             1

/*!
 * Datarate of the beacon channel
 */
#define CN470ALI_BEACON_CHANNEL_DR                     DR_2

/*!
 * Bandwith of the beacon channel
 */
#define CN470ALI_BEACON_CHANNEL_BW                     0

/*!
 * Ping slot channel datarate
 */
#define CN470ALI_PING_SLOT_CHANNEL_DR                  DR_2

/*!
 * LoRaMac maximum number of bands
 */
#define CN470ALI_MAX_NB_BANDS                          1

/*!
 * Band 0 definition
 * { DutyCycle, TxMaxPower, LastJoinTxDoneTime, LastTxDoneTime, TimeOff }
 */
#define CN470ALI_BAND0                                 { 1, CN470ALI_MAX_TX_POWER, 0, 0, 0 } //  100.0 %

#ifdef ALI_DOWNLINK_FREQ

    #if (ALI_DOWNLINK_FREQ == ALI_FREQ470_S || ALI_DOWNLINK_FREQ == ALI_FREQ480_D)  
        /*!
         * Defines the first channel for RX window 1 for CN470ALI band
         */
        #define CN470ALI_FIRST_RX1_CHANNEL                     ( (uint32_t) 470300000 )

        /*!
         * Defines the last channel for RX window 1 for CN470ALI band
         */
        #define CN470ALI_LAST_RX1_CHANNEL                      ( (uint32_t) 509700000 )

        /*!
         * Defines the step width of the channels for RX window 1
         */
        #define CN470ALI_STEPWIDTH_RX1_CHANNEL                 ( (uint32_t) 200000 )

        
        /*!
         * Second reception window channel frequency definition.
         */
        #define CN470ALI_RX_WND_2_FREQ                         ((uint32_t)ALI_RX2WIN_FREQ)

        
    #elif (ALI_DOWNLINK_FREQ == ALI_FREQ470_D || ALI_DOWNLINK_FREQ == ALI_FREQ480_S)

        /*!
         * Defines the first channel for RX window 1 for CN470ALI band
         */
        #define CN470ALI_FIRST_RX1_CHANNEL                     ( (uint32_t) 483900000 )

        /*!
         * Defines the last channel for RX window 1 for CN470ALI band
         */
        #define CN470ALI_LAST_RX1_CHANNEL                      ( (uint32_t) 496500000 )

        /*!
         * Defines the step width of the channels for RX window 1
         */
        #define CN470ALI_STEPWIDTH_RX1_CHANNEL                 ( (uint32_t) 200000 )
        
        /*!
         * Second reception window channel frequency definition.
         */
        #define CN470ALI_RX_WND_2_FREQ                         ((uint32_t)ALI_RX2WIN_FREQ)

    
    #else
        #error ALI_DOWNLINK_FREQ not existed

      
    #endif //===============================================================================>
#else
    #error ALI_DOWNLINK_FREQ not define
#endif

/*!
 * Data rates table definition
 */
static const uint8_t DataratesCN470ALI[]  = { 12, 11, 10,  9,  8,  7 };

/*!
 * Bandwidths table definition in Hz
 */
static const uint32_t BandwidthsCN470ALI[] = { 125000, 125000, 125000, 125000, 125000, 125000 };
/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.

 
 */
static const uint8_t MaxPayloadOfDatarateCN470ALI[] = { 51+1, 51+1, 51+1, 115+1, 242+1, 242+1 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateRepeaterCN470ALI[] = { 51+1, 51+1, 51+1, 115+1, 222+1, 222+1 };

/*!
 * \brief The function gets a value of a specific phy attribute.
 *
 * \param [IN] getPhy Pointer to the function parameters.
 *
 * \retval Returns a structure containing the PHY parameter.
 */
PhyParam_t RegionCN470ALIGetPhyParam( GetPhyParams_t* getPhy );

/*!
 * \brief Updates the last TX done parameters of the current channel.
 *
 * \param [IN] txDone Pointer to the function parameters.
 */
void RegionCN470ALISetBandTxDone( SetBandTxDoneParams_t* txDone );

/*!
 * \brief Initializes the channels masks and the channels.
 *
 * \param [IN] type Sets the initialization type.
 */
void RegionCN470ALIInitDefaults( InitDefaultsParams_t* params );

/*!
 * \brief Returns a pointer to the internal context and its size.
 *
 * \param [OUT] params Pointer to the function parameters.
 *
 * \retval      Points to a structure where the module store its non-volatile context.
 */
void* RegionCN470ALIGetNvmCtx( GetNvmCtxParams_t* params );

/*!
 * \brief Verifies a parameter.
 *
 * \param [IN] verify Pointer to the function parameters.
 *
 * \param [IN] type Sets the initialization type.
 *
 * \retval Returns true, if the parameter is valid.
 */
bool RegionCN470ALIVerify( VerifyParams_t* verify, PhyAttribute_t phyAttribute );

/*!
 * \brief The function parses the input buffer and sets up the channels of the
 *        CF list.
 *
 * \param [IN] applyCFList Pointer to the function parameters.
 */
void RegionCN470ALIApplyCFList( ApplyCFListParams_t* applyCFList );

/*!
 * \brief Sets a channels mask.
 *
 * \param [IN] chanMaskSet Pointer to the function parameters.
 *
 * \retval Returns true, if the channels mask could be set.
 */
bool RegionCN470ALIChanMaskSet( ChanMaskSetParams_t* chanMaskSet );

/*!
 * Computes the Rx window timeout and offset.
 *
 * \param [IN] datarate     Rx window datarate index to be used
 *
 * \param [IN] minRxSymbols Minimum required number of symbols to detect an Rx frame.
 *
 * \param [IN] rxError      System maximum timing error of the receiver. In milliseconds
 *                          The receiver will turn on in a [-rxError : +rxError] ms
 *                          interval around RxOffset
 *
 * \param [OUT]rxConfigParams Returns updated WindowTimeout and WindowOffset fields.
 */
void RegionCN470ALIComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams );

/*!
 * \brief Configuration of the RX windows.
 *
 * \param [IN] rxConfig Pointer to the function parameters.
 *
 * \param [OUT] datarate The datarate index which was set.
 *
 * \retval Returns true, if the configuration was applied successfully.
 */
bool RegionCN470ALIRxConfig( RxConfigParams_t* rxConfig, int8_t* datarate );

/*!
 * \brief TX configuration.
 *
 * \param [IN] txConfig Pointer to the function parameters.
 *
 * \param [OUT] txPower The tx power index which was set.
 *
 * \param [OUT] txTimeOnAir The time-on-air of the frame.
 *
 * \retval Returns true, if the configuration was applied successfully.
 */
bool RegionCN470ALITxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir );

/*!
 * \brief The function processes a Link ADR Request.
 *
 * \param [IN] linkAdrReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionCN470ALILinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed );

/*!
 * \brief The function processes a RX Parameter Setup Request.
 *
 * \param [IN] rxParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionCN470ALIRxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq );

/*!
 * \brief The function processes a Channel Request.
 *
 * \param [IN] newChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionCN470ALINewChannelReq( NewChannelReqParams_t* newChannelReq );

/*!
 * \brief The function processes a TX ParamSetup Request.
 *
 * \param [IN] txParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 *         Returns -1, if the functionality is not implemented. In this case, the end node
 *         shall not process the command.
 */
int8_t RegionCN470ALITxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq );

/*!
 * \brief The function processes a DlChannel Request.
 *
 * \param [IN] dlChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionCN470ALIDlChannelReq( DlChannelReqParams_t* dlChannelReq );

/*!
 * \brief Alternates the datarate of the channel for the join request.
 *
 * \param [IN] currentDr Current datarate.
 *
 * \retval Datarate to apply.
 */
int8_t RegionCN470ALIAlternateDr( int8_t currentDr, AlternateDrType_t type );

/*!
 * \brief Calculates the back-off time.
 *
 * \param [IN] calcBackOff Pointer to the function parameters.
 */
void RegionCN470ALICalcBackOff( CalcBackOffParams_t* calcBackOff );

/*!
 * \brief Searches and set the next random available channel
 *
 * \param [OUT] channel Next channel to use for TX.
 *
 * \param [OUT] time Time to wait for the next transmission according to the duty
 *              cycle.
 *
 * \param [OUT] aggregatedTimeOff Updates the aggregated time off.
 *
 * \retval Function status [1: OK, 0: Unable to find a channel on the current datarate]
 */
LoRaMacStatus_t RegionCN470ALINextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff );

/*!
 * \brief Adds a channel.
 *
 * \param [IN] channelAdd Pointer to the function parameters.
 *
 * \retval Status of the operation.
 */
LoRaMacStatus_t RegionCN470ALIChannelAdd( ChannelAddParams_t* channelAdd );

/*!
 * \brief Removes a channel.
 *
 * \param [IN] channelRemove Pointer to the function parameters.
 *
 * \retval Returns true, if the channel was removed successfully.
 */
bool RegionCN470ALIChannelsRemove( ChannelRemoveParams_t* channelRemove  );

/*!
 * \brief Sets the radio into continuous wave mode.
 *
 * \param [IN] continuousWave Pointer to the function parameters.
 */
void RegionCN470ALISetContinuousWave( ContinuousWaveParams_t* continuousWave );

/*!
 * \brief Computes new datarate according to the given offset
 *
 * \param [IN] downlinkDwellTime Downlink dwell time configuration. 0: No limit, 1: 400ms
 *
 * \param [IN] dr Current datarate
 *
 * \param [IN] drOffset Offset to be applied
 *
 * \retval newDr Computed datarate.
 */
uint8_t RegionCN470ALIApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset );

/*!
 * \brief Sets the radio into beacon reception mode
 *
 * \param [IN] rxBeaconSetup Pointer to the function parameters
 */
 void RegionCN470ALIRxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr );

/*! \} defgroup REGIONCN470ALI */

#endif // __REGION_CN470ALI_H__
