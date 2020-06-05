/*!
 * \file      Region.c
 *
 * \brief     Region implementation.
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
#include "LoRaMac.h"

// Setup regions
#ifdef REGION_AS923
#include "RegionAS923.h"
#define AS923_CASE                                 case LORAMAC_REGION_AS923:
#define AS923_IS_ACTIVE( )                         AS923_CASE { return true; }
#define AS923_GET_PHY_PARAM( )                     AS923_CASE { return RegionAS923GetPhyParam( getPhy ); }
#define AS923_SET_BAND_TX_DONE( )                  AS923_CASE { RegionAS923SetBandTxDone( txDone ); break; }
#define AS923_INIT_DEFAULTS( )                     AS923_CASE { RegionAS923InitDefaults( params ); break; }
#define AS923_GET_NVM_CTX( )                       AS923_CASE { return RegionAS923GetNvmCtx( params ); }
#define AS923_VERIFY( )                            AS923_CASE { return RegionAS923Verify( verify, phyAttribute ); }
#define AS923_APPLY_CF_LIST( )                     AS923_CASE { RegionAS923ApplyCFList( applyCFList ); break; }
#define AS923_CHAN_MASK_SET( )                     AS923_CASE { return RegionAS923ChanMaskSet( chanMaskSet ); }
#define AS923_COMPUTE_RX_WINDOW_PARAMETERS( )      AS923_CASE { RegionAS923ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define AS923_RX_CONFIG( )                         AS923_CASE { return RegionAS923RxConfig( rxConfig, datarate ); }
#define AS923_TX_CONFIG( )                         AS923_CASE { return RegionAS923TxConfig( txConfig, txPower, txTimeOnAir ); }
#define AS923_LINK_ADR_REQ( )                      AS923_CASE { return RegionAS923LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define AS923_RX_PARAM_SETUP_REQ( )                AS923_CASE { return RegionAS923RxParamSetupReq( rxParamSetupReq ); }
#define AS923_NEW_CHANNEL_REQ( )                   AS923_CASE { return RegionAS923NewChannelReq( newChannelReq ); }
#define AS923_TX_PARAM_SETUP_REQ( )                AS923_CASE { return RegionAS923TxParamSetupReq( txParamSetupReq ); }
#define AS923_DL_CHANNEL_REQ( )                    AS923_CASE { return RegionAS923DlChannelReq( dlChannelReq ); }
#define AS923_ALTERNATE_DR( )                      AS923_CASE { return RegionAS923AlternateDr( currentDr, type ); }
#define AS923_NEXT_CHANNEL( )                      AS923_CASE { return RegionAS923NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define AS923_CHANNEL_ADD( )                       AS923_CASE { return RegionAS923ChannelAdd( channelAdd ); }
#define AS923_CHANNEL_REMOVE( )                    AS923_CASE { return RegionAS923ChannelsRemove( channelRemove ); }
#define AS923_SET_CONTINUOUS_WAVE( )               AS923_CASE { RegionAS923SetContinuousWave( continuousWave ); break; }
#define AS923_APPLY_DR_OFFSET( )                   AS923_CASE { return RegionAS923ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define AS923_RX_BEACON_SETUP( )                   AS923_CASE { RegionAS923RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define AS923_IS_ACTIVE( )
#define AS923_GET_PHY_PARAM( )
#define AS923_SET_BAND_TX_DONE( )
#define AS923_INIT_DEFAULTS( )
#define AS923_GET_NVM_CTX( )
#define AS923_VERIFY( )
#define AS923_APPLY_CF_LIST( )
#define AS923_CHAN_MASK_SET( )
#define AS923_COMPUTE_RX_WINDOW_PARAMETERS( )
#define AS923_RX_CONFIG( )
#define AS923_TX_CONFIG( )
#define AS923_LINK_ADR_REQ( )
#define AS923_RX_PARAM_SETUP_REQ( )
#define AS923_NEW_CHANNEL_REQ( )
#define AS923_TX_PARAM_SETUP_REQ( )
#define AS923_DL_CHANNEL_REQ( )
#define AS923_ALTERNATE_DR( )
#define AS923_NEXT_CHANNEL( )
#define AS923_CHANNEL_ADD( )
#define AS923_CHANNEL_REMOVE( )
#define AS923_SET_CONTINUOUS_WAVE( )
#define AS923_APPLY_DR_OFFSET( )
#define AS923_RX_BEACON_SETUP( )
#endif

#ifdef REGION_AU915
#include "RegionAU915.h"
#define AU915_CASE                                 case LORAMAC_REGION_AU915:
#define AU915_IS_ACTIVE( )                         AU915_CASE { return true; }
#define AU915_GET_PHY_PARAM( )                     AU915_CASE { return RegionAU915GetPhyParam( getPhy ); }
#define AU915_SET_BAND_TX_DONE( )                  AU915_CASE { RegionAU915SetBandTxDone( txDone ); break; }
#define AU915_INIT_DEFAULTS( )                     AU915_CASE { RegionAU915InitDefaults( params ); break; }
#define AU915_GET_NVM_CTX( )                       AU915_CASE { return RegionAU915GetNvmCtx( params ); }
#define AU915_VERIFY( )                            AU915_CASE { return RegionAU915Verify( verify, phyAttribute ); }
#define AU915_APPLY_CF_LIST( )                     AU915_CASE { RegionAU915ApplyCFList( applyCFList ); break; }
#define AU915_CHAN_MASK_SET( )                     AU915_CASE { return RegionAU915ChanMaskSet( chanMaskSet ); }
#define AU915_COMPUTE_RX_WINDOW_PARAMETERS( )      AU915_CASE { RegionAU915ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define AU915_RX_CONFIG( )                         AU915_CASE { return RegionAU915RxConfig( rxConfig, datarate ); }
#define AU915_TX_CONFIG( )                         AU915_CASE { return RegionAU915TxConfig( txConfig, txPower, txTimeOnAir ); }
#define AU915_LINK_ADR_REQ( )                      AU915_CASE { return RegionAU915LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define AU915_RX_PARAM_SETUP_REQ( )                AU915_CASE { return RegionAU915RxParamSetupReq( rxParamSetupReq ); }
#define AU915_NEW_CHANNEL_REQ( )                   AU915_CASE { return RegionAU915NewChannelReq( newChannelReq ); }
#define AU915_TX_PARAM_SETUP_REQ( )                AU915_CASE { return RegionAU915TxParamSetupReq( txParamSetupReq ); }
#define AU915_DL_CHANNEL_REQ( )                    AU915_CASE { return RegionAU915DlChannelReq( dlChannelReq ); }
#define AU915_ALTERNATE_DR( )                      AU915_CASE { return RegionAU915AlternateDr( currentDr, type ); }
#define AU915_NEXT_CHANNEL( )                      AU915_CASE { return RegionAU915NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define AU915_CHANNEL_ADD( )                       AU915_CASE { return RegionAU915ChannelAdd( channelAdd ); }
#define AU915_CHANNEL_REMOVE( )                    AU915_CASE { return RegionAU915ChannelsRemove( channelRemove ); }
#define AU915_SET_CONTINUOUS_WAVE( )               AU915_CASE { RegionAU915SetContinuousWave( continuousWave ); break; }
#define AU915_APPLY_DR_OFFSET( )                   AU915_CASE { return RegionAU915ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define AU915_RX_BEACON_SETUP( )                   AU915_CASE { RegionAU915RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define AU915_IS_ACTIVE( )
#define AU915_GET_PHY_PARAM( )
#define AU915_SET_BAND_TX_DONE( )
#define AU915_INIT_DEFAULTS( )
#define AU915_GET_NVM_CTX( )
#define AU915_VERIFY( )
#define AU915_APPLY_CF_LIST( )
#define AU915_CHAN_MASK_SET( )
#define AU915_COMPUTE_RX_WINDOW_PARAMETERS( )
#define AU915_RX_CONFIG( )
#define AU915_TX_CONFIG( )
#define AU915_LINK_ADR_REQ( )
#define AU915_RX_PARAM_SETUP_REQ( )
#define AU915_NEW_CHANNEL_REQ( )
#define AU915_TX_PARAM_SETUP_REQ( )
#define AU915_DL_CHANNEL_REQ( )
#define AU915_ALTERNATE_DR( )
#define AU915_NEXT_CHANNEL( )
#define AU915_CHANNEL_ADD( )
#define AU915_CHANNEL_REMOVE( )
#define AU915_SET_CONTINUOUS_WAVE( )
#define AU915_APPLY_DR_OFFSET( )
#define AU915_RX_BEACON_SETUP( )
#endif

#ifdef REGION_CN470
#include "RegionCN470.h"
#define CN470_CASE                                 case LORAMAC_REGION_CN470:
#define CN470_IS_ACTIVE( )                         CN470_CASE { return true; }
#define CN470_GET_PHY_PARAM( )                     CN470_CASE { return RegionCN470GetPhyParam( getPhy ); }
#define CN470_SET_BAND_TX_DONE( )                  CN470_CASE { RegionCN470SetBandTxDone( txDone ); break; }
#define CN470_INIT_DEFAULTS( )                     CN470_CASE { RegionCN470InitDefaults( params ); break; }
#define CN470_GET_NVM_CTX( )                       CN470_CASE { return RegionCN470GetNvmCtx( params ); }
#define CN470_VERIFY( )                            CN470_CASE { return RegionCN470Verify( verify, phyAttribute ); }
#define CN470_APPLY_CF_LIST( )                     CN470_CASE { RegionCN470ApplyCFList( applyCFList ); break; }
#define CN470_CHAN_MASK_SET( )                     CN470_CASE { return RegionCN470ChanMaskSet( chanMaskSet ); }
#define CN470_COMPUTE_RX_WINDOW_PARAMETERS( )      CN470_CASE { RegionCN470ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define CN470_RX_CONFIG( )                         CN470_CASE { return RegionCN470RxConfig( rxConfig, datarate ); }
#define CN470_TX_CONFIG( )                         CN470_CASE { return RegionCN470TxConfig( txConfig, txPower, txTimeOnAir ); }
#define CN470_LINK_ADR_REQ( )                      CN470_CASE { return RegionCN470LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define CN470_RX_PARAM_SETUP_REQ( )                CN470_CASE { return RegionCN470RxParamSetupReq( rxParamSetupReq ); }
#define CN470_NEW_CHANNEL_REQ( )                   CN470_CASE { return RegionCN470NewChannelReq( newChannelReq ); }
#define CN470_TX_PARAM_SETUP_REQ( )                CN470_CASE { return RegionCN470TxParamSetupReq( txParamSetupReq ); }
#define CN470_DL_CHANNEL_REQ( )                    CN470_CASE { return RegionCN470DlChannelReq( dlChannelReq ); }
#define CN470_ALTERNATE_DR( )                      CN470_CASE { return RegionCN470AlternateDr( currentDr, type ); }
#define CN470_NEXT_CHANNEL( )                      CN470_CASE { return RegionCN470NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define CN470_CHANNEL_ADD( )                       CN470_CASE { return RegionCN470ChannelAdd( channelAdd ); }
#define CN470_CHANNEL_REMOVE( )                    CN470_CASE { return RegionCN470ChannelsRemove( channelRemove ); }
#define CN470_SET_CONTINUOUS_WAVE( )               CN470_CASE { RegionCN470SetContinuousWave( continuousWave ); break; }
#define CN470_APPLY_DR_OFFSET( )                   CN470_CASE { return RegionCN470ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define CN470_RX_BEACON_SETUP( )                   CN470_CASE { RegionCN470RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define CN470_IS_ACTIVE( )
#define CN470_GET_PHY_PARAM( )
#define CN470_SET_BAND_TX_DONE( )
#define CN470_INIT_DEFAULTS( )
#define CN470_GET_NVM_CTX( )
#define CN470_VERIFY( )
#define CN470_APPLY_CF_LIST( )
#define CN470_CHAN_MASK_SET( )
#define CN470_COMPUTE_RX_WINDOW_PARAMETERS( )
#define CN470_RX_CONFIG( )
#define CN470_TX_CONFIG( )
#define CN470_LINK_ADR_REQ( )
#define CN470_RX_PARAM_SETUP_REQ( )
#define CN470_NEW_CHANNEL_REQ( )
#define CN470_TX_PARAM_SETUP_REQ( )
#define CN470_DL_CHANNEL_REQ( )
#define CN470_ALTERNATE_DR( )
#define CN470_NEXT_CHANNEL( )
#define CN470_CHANNEL_ADD( )
#define CN470_CHANNEL_REMOVE( )
#define CN470_SET_CONTINUOUS_WAVE( )
#define CN470_APPLY_DR_OFFSET( )
#define CN470_RX_BEACON_SETUP( )
#endif

#ifdef REGION_CN779
#include "RegionCN779.h"
#define CN779_CASE                                 case LORAMAC_REGION_CN779:
#define CN779_IS_ACTIVE( )                         CN779_CASE { return true; }
#define CN779_GET_PHY_PARAM( )                     CN779_CASE { return RegionCN779GetPhyParam( getPhy ); }
#define CN779_SET_BAND_TX_DONE( )                  CN779_CASE { RegionCN779SetBandTxDone( txDone ); break; }
#define CN779_INIT_DEFAULTS( )                     CN779_CASE { RegionCN779InitDefaults( params ); break; }
#define CN779_GET_NVM_CTX( )                       CN779_CASE { return RegionCN779GetNvmCtx( params ); }
#define CN779_VERIFY( )                            CN779_CASE { return RegionCN779Verify( verify, phyAttribute ); }
#define CN779_APPLY_CF_LIST( )                     CN779_CASE { RegionCN779ApplyCFList( applyCFList ); break; }
#define CN779_CHAN_MASK_SET( )                     CN779_CASE { return RegionCN779ChanMaskSet( chanMaskSet ); }
#define CN779_COMPUTE_RX_WINDOW_PARAMETERS( )      CN779_CASE { RegionCN779ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define CN779_RX_CONFIG( )                         CN779_CASE { return RegionCN779RxConfig( rxConfig, datarate ); }
#define CN779_TX_CONFIG( )                         CN779_CASE { return RegionCN779TxConfig( txConfig, txPower, txTimeOnAir ); }
#define CN779_LINK_ADR_REQ( )                      CN779_CASE { return RegionCN779LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define CN779_RX_PARAM_SETUP_REQ( )                CN779_CASE { return RegionCN779RxParamSetupReq( rxParamSetupReq ); }
#define CN779_NEW_CHANNEL_REQ( )                   CN779_CASE { return RegionCN779NewChannelReq( newChannelReq ); }
#define CN779_TX_PARAM_SETUP_REQ( )                CN779_CASE { return RegionCN779TxParamSetupReq( txParamSetupReq ); }
#define CN779_DL_CHANNEL_REQ( )                    CN779_CASE { return RegionCN779DlChannelReq( dlChannelReq ); }
#define CN779_ALTERNATE_DR( )                      CN779_CASE { return RegionCN779AlternateDr( currentDr, type ); }
#define CN779_NEXT_CHANNEL( )                      CN779_CASE { return RegionCN779NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define CN779_CHANNEL_ADD( )                       CN779_CASE { return RegionCN779ChannelAdd( channelAdd ); }
#define CN779_CHANNEL_REMOVE( )                    CN779_CASE { return RegionCN779ChannelsRemove( channelRemove ); }
#define CN779_SET_CONTINUOUS_WAVE( )               CN779_CASE { RegionCN779SetContinuousWave( continuousWave ); break; }
#define CN779_APPLY_DR_OFFSET( )                   CN779_CASE { return RegionCN779ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define CN779_RX_BEACON_SETUP( )                   CN779_CASE { RegionCN779RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define CN779_IS_ACTIVE( )
#define CN779_GET_PHY_PARAM( )
#define CN779_SET_BAND_TX_DONE( )
#define CN779_INIT_DEFAULTS( )
#define CN779_GET_NVM_CTX( )
#define CN779_VERIFY( )
#define CN779_APPLY_CF_LIST( )
#define CN779_CHAN_MASK_SET( )
#define CN779_COMPUTE_RX_WINDOW_PARAMETERS( )
#define CN779_RX_CONFIG( )
#define CN779_TX_CONFIG( )
#define CN779_LINK_ADR_REQ( )
#define CN779_RX_PARAM_SETUP_REQ( )
#define CN779_NEW_CHANNEL_REQ( )
#define CN779_TX_PARAM_SETUP_REQ( )
#define CN779_DL_CHANNEL_REQ( )
#define CN779_ALTERNATE_DR( )
#define CN779_NEXT_CHANNEL( )
#define CN779_CHANNEL_ADD( )
#define CN779_CHANNEL_REMOVE( )
#define CN779_SET_CONTINUOUS_WAVE( )
#define CN779_APPLY_DR_OFFSET( )
#define CN779_RX_BEACON_SETUP( )
#endif

#ifdef REGION_EU433
#include "RegionEU433.h"
#define EU433_CASE                                 case LORAMAC_REGION_EU433:
#define EU433_IS_ACTIVE( )                         EU433_CASE { return true; }
#define EU433_GET_PHY_PARAM( )                     EU433_CASE { return RegionEU433GetPhyParam( getPhy ); }
#define EU433_SET_BAND_TX_DONE( )                  EU433_CASE { RegionEU433SetBandTxDone( txDone ); break; }
#define EU433_INIT_DEFAULTS( )                     EU433_CASE { RegionEU433InitDefaults( params ); break; }
#define EU433_GET_NVM_CTX( )                       EU433_CASE { return RegionEU433GetNvmCtx( params ); }
#define EU433_VERIFY( )                            EU433_CASE { return RegionEU433Verify( verify, phyAttribute ); }
#define EU433_APPLY_CF_LIST( )                     EU433_CASE { RegionEU433ApplyCFList( applyCFList ); break; }
#define EU433_CHAN_MASK_SET( )                     EU433_CASE { return RegionEU433ChanMaskSet( chanMaskSet ); }
#define EU433_COMPUTE_RX_WINDOW_PARAMETERS( )      EU433_CASE { RegionEU433ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define EU433_RX_CONFIG( )                         EU433_CASE { return RegionEU433RxConfig( rxConfig, datarate ); }
#define EU433_TX_CONFIG( )                         EU433_CASE { return RegionEU433TxConfig( txConfig, txPower, txTimeOnAir ); }
#define EU433_LINK_ADR_REQ( )                      EU433_CASE { return RegionEU433LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define EU433_RX_PARAM_SETUP_REQ( )                EU433_CASE { return RegionEU433RxParamSetupReq( rxParamSetupReq ); }
#define EU433_NEW_CHANNEL_REQ( )                   EU433_CASE { return RegionEU433NewChannelReq( newChannelReq ); }
#define EU433_TX_PARAM_SETUP_REQ( )                EU433_CASE { return RegionEU433TxParamSetupReq( txParamSetupReq ); }
#define EU433_DL_CHANNEL_REQ( )                    EU433_CASE { return RegionEU433DlChannelReq( dlChannelReq ); }
#define EU433_ALTERNATE_DR( )                      EU433_CASE { return RegionEU433AlternateDr( currentDr, type ); }
#define EU433_NEXT_CHANNEL( )                      EU433_CASE { return RegionEU433NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define EU433_CHANNEL_ADD( )                       EU433_CASE { return RegionEU433ChannelAdd( channelAdd ); }
#define EU433_CHANNEL_REMOVE( )                    EU433_CASE { return RegionEU433ChannelsRemove( channelRemove ); }
#define EU433_SET_CONTINUOUS_WAVE( )               EU433_CASE { RegionEU433SetContinuousWave( continuousWave ); break; }
#define EU433_APPLY_DR_OFFSET( )                   EU433_CASE { return RegionEU433ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define EU433_RX_BEACON_SETUP( )                   EU433_CASE { RegionEU433RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define EU433_IS_ACTIVE( )
#define EU433_GET_PHY_PARAM( )
#define EU433_SET_BAND_TX_DONE( )
#define EU433_INIT_DEFAULTS( )
#define EU433_GET_NVM_CTX( )
#define EU433_VERIFY( )
#define EU433_APPLY_CF_LIST( )
#define EU433_CHAN_MASK_SET( )
#define EU433_COMPUTE_RX_WINDOW_PARAMETERS( )
#define EU433_RX_CONFIG( )
#define EU433_TX_CONFIG( )
#define EU433_LINK_ADR_REQ( )
#define EU433_RX_PARAM_SETUP_REQ( )
#define EU433_NEW_CHANNEL_REQ( )
#define EU433_TX_PARAM_SETUP_REQ( )
#define EU433_DL_CHANNEL_REQ( )
#define EU433_ALTERNATE_DR( )
#define EU433_NEXT_CHANNEL( )
#define EU433_CHANNEL_ADD( )
#define EU433_CHANNEL_REMOVE( )
#define EU433_SET_CONTINUOUS_WAVE( )
#define EU433_APPLY_DR_OFFSET( )
#define EU433_RX_BEACON_SETUP( )
#endif

#ifdef REGION_EU868
#include "RegionEU868.h"
#define EU868_CASE                                 case LORAMAC_REGION_EU868:
#define EU868_IS_ACTIVE( )                         EU868_CASE { return true; }
#define EU868_GET_PHY_PARAM( )                     EU868_CASE { return RegionEU868GetPhyParam( getPhy ); }
#define EU868_SET_BAND_TX_DONE( )                  EU868_CASE { RegionEU868SetBandTxDone( txDone ); break; }
#define EU868_INIT_DEFAULTS( )                     EU868_CASE { RegionEU868InitDefaults( params ); break; }
#define EU868_GET_NVM_CTX( )                       EU868_CASE { return RegionEU868GetNvmCtx( params ); }
#define EU868_VERIFY( )                            EU868_CASE { return RegionEU868Verify( verify, phyAttribute ); }
#define EU868_APPLY_CF_LIST( )                     EU868_CASE { RegionEU868ApplyCFList( applyCFList ); break; }
#define EU868_CHAN_MASK_SET( )                     EU868_CASE { return RegionEU868ChanMaskSet( chanMaskSet ); }
#define EU868_COMPUTE_RX_WINDOW_PARAMETERS( )      EU868_CASE { RegionEU868ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define EU868_RX_CONFIG( )                         EU868_CASE { return RegionEU868RxConfig( rxConfig, datarate ); }
#define EU868_TX_CONFIG( )                         EU868_CASE { return RegionEU868TxConfig( txConfig, txPower, txTimeOnAir ); }
#define EU868_LINK_ADR_REQ( )                      EU868_CASE { return RegionEU868LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define EU868_RX_PARAM_SETUP_REQ( )                EU868_CASE { return RegionEU868RxParamSetupReq( rxParamSetupReq ); }
#define EU868_NEW_CHANNEL_REQ( )                   EU868_CASE { return RegionEU868NewChannelReq( newChannelReq ); }
#define EU868_TX_PARAM_SETUP_REQ( )                EU868_CASE { return RegionEU868TxParamSetupReq( txParamSetupReq ); }
#define EU868_DL_CHANNEL_REQ( )                    EU868_CASE { return RegionEU868DlChannelReq( dlChannelReq ); }
#define EU868_ALTERNATE_DR( )                      EU868_CASE { return RegionEU868AlternateDr( currentDr, type ); }
#define EU868_NEXT_CHANNEL( )                      EU868_CASE { return RegionEU868NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define EU868_CHANNEL_ADD( )                       EU868_CASE { return RegionEU868ChannelAdd( channelAdd ); }
#define EU868_CHANNEL_REMOVE( )                    EU868_CASE { return RegionEU868ChannelsRemove( channelRemove ); }
#define EU868_SET_CONTINUOUS_WAVE( )               EU868_CASE { RegionEU868SetContinuousWave( continuousWave ); break; }
#define EU868_APPLY_DR_OFFSET( )                   EU868_CASE { return RegionEU868ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define EU868_RX_BEACON_SETUP( )                   EU868_CASE { RegionEU868RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define EU868_IS_ACTIVE( )
#define EU868_GET_PHY_PARAM( )
#define EU868_SET_BAND_TX_DONE( )
#define EU868_INIT_DEFAULTS( )
#define EU868_GET_NVM_CTX( )
#define EU868_VERIFY( )
#define EU868_APPLY_CF_LIST( )
#define EU868_CHAN_MASK_SET( )
#define EU868_COMPUTE_RX_WINDOW_PARAMETERS( )
#define EU868_RX_CONFIG( )
#define EU868_TX_CONFIG( )
#define EU868_LINK_ADR_REQ( )
#define EU868_RX_PARAM_SETUP_REQ( )
#define EU868_NEW_CHANNEL_REQ( )
#define EU868_TX_PARAM_SETUP_REQ( )
#define EU868_DL_CHANNEL_REQ( )
#define EU868_ALTERNATE_DR( )
#define EU868_NEXT_CHANNEL( )
#define EU868_CHANNEL_ADD( )
#define EU868_CHANNEL_REMOVE( )
#define EU868_SET_CONTINUOUS_WAVE( )
#define EU868_APPLY_DR_OFFSET( )
#define EU868_RX_BEACON_SETUP( )
#endif

#ifdef REGION_KR920
#include "RegionKR920.h"
#define KR920_CASE                                 case LORAMAC_REGION_KR920:
#define KR920_IS_ACTIVE( )                         KR920_CASE { return true; }
#define KR920_GET_PHY_PARAM( )                     KR920_CASE { return RegionKR920GetPhyParam( getPhy ); }
#define KR920_SET_BAND_TX_DONE( )                  KR920_CASE { RegionKR920SetBandTxDone( txDone ); break; }
#define KR920_INIT_DEFAULTS( )                     KR920_CASE { RegionKR920InitDefaults( params ); break; }
#define KR920_GET_NVM_CTX( )                       KR920_CASE { return RegionKR920GetNvmCtx( params ); }
#define KR920_VERIFY( )                            KR920_CASE { return RegionKR920Verify( verify, phyAttribute ); }
#define KR920_APPLY_CF_LIST( )                     KR920_CASE { RegionKR920ApplyCFList( applyCFList ); break; }
#define KR920_CHAN_MASK_SET( )                     KR920_CASE { return RegionKR920ChanMaskSet( chanMaskSet ); }
#define KR920_COMPUTE_RX_WINDOW_PARAMETERS( )      KR920_CASE { RegionKR920ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define KR920_RX_CONFIG( )                         KR920_CASE { return RegionKR920RxConfig( rxConfig, datarate ); }
#define KR920_TX_CONFIG( )                         KR920_CASE { return RegionKR920TxConfig( txConfig, txPower, txTimeOnAir ); }
#define KR920_LINK_ADR_REQ( )                      KR920_CASE { return RegionKR920LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define KR920_RX_PARAM_SETUP_REQ( )                KR920_CASE { return RegionKR920RxParamSetupReq( rxParamSetupReq ); }
#define KR920_NEW_CHANNEL_REQ( )                   KR920_CASE { return RegionKR920NewChannelReq( newChannelReq ); }
#define KR920_TX_PARAM_SETUP_REQ( )                KR920_CASE { return RegionKR920TxParamSetupReq( txParamSetupReq ); }
#define KR920_DL_CHANNEL_REQ( )                    KR920_CASE { return RegionKR920DlChannelReq( dlChannelReq ); }
#define KR920_ALTERNATE_DR( )                      KR920_CASE { return RegionKR920AlternateDr( currentDr, type ); }
#define KR920_NEXT_CHANNEL( )                      KR920_CASE { return RegionKR920NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define KR920_CHANNEL_ADD( )                       KR920_CASE { return RegionKR920ChannelAdd( channelAdd ); }
#define KR920_CHANNEL_REMOVE( )                    KR920_CASE { return RegionKR920ChannelsRemove( channelRemove ); }
#define KR920_SET_CONTINUOUS_WAVE( )               KR920_CASE { RegionKR920SetContinuousWave( continuousWave ); break; }
#define KR920_APPLY_DR_OFFSET( )                   KR920_CASE { return RegionKR920ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define KR920_RX_BEACON_SETUP( )                   KR920_CASE { RegionKR920RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define KR920_IS_ACTIVE( )
#define KR920_GET_PHY_PARAM( )
#define KR920_SET_BAND_TX_DONE( )
#define KR920_INIT_DEFAULTS( )
#define KR920_GET_NVM_CTX( )
#define KR920_VERIFY( )
#define KR920_APPLY_CF_LIST( )
#define KR920_CHAN_MASK_SET( )
#define KR920_COMPUTE_RX_WINDOW_PARAMETERS( )
#define KR920_RX_CONFIG( )
#define KR920_TX_CONFIG( )
#define KR920_LINK_ADR_REQ( )
#define KR920_RX_PARAM_SETUP_REQ( )
#define KR920_NEW_CHANNEL_REQ( )
#define KR920_TX_PARAM_SETUP_REQ( )
#define KR920_DL_CHANNEL_REQ( )
#define KR920_ALTERNATE_DR( )
#define KR920_NEXT_CHANNEL( )
#define KR920_CHANNEL_ADD( )
#define KR920_CHANNEL_REMOVE( )
#define KR920_SET_CONTINUOUS_WAVE( )
#define KR920_APPLY_DR_OFFSET( )
#define KR920_RX_BEACON_SETUP( )
#endif

#ifdef REGION_IN865
#include "RegionIN865.h"
#define IN865_CASE                                 case LORAMAC_REGION_IN865:
#define IN865_IS_ACTIVE( )                         IN865_CASE { return true; }
#define IN865_GET_PHY_PARAM( )                     IN865_CASE { return RegionIN865GetPhyParam( getPhy ); }
#define IN865_SET_BAND_TX_DONE( )                  IN865_CASE { RegionIN865SetBandTxDone( txDone ); break; }
#define IN865_INIT_DEFAULTS( )                     IN865_CASE { RegionIN865InitDefaults( params ); break; }
#define IN865_GET_NVM_CTX( )                       IN865_CASE { return RegionIN865GetNvmCtx( params ); }
#define IN865_VERIFY( )                            IN865_CASE { return RegionIN865Verify( verify, phyAttribute ); }
#define IN865_APPLY_CF_LIST( )                     IN865_CASE { RegionIN865ApplyCFList( applyCFList ); break; }
#define IN865_CHAN_MASK_SET( )                     IN865_CASE { return RegionIN865ChanMaskSet( chanMaskSet ); }
#define IN865_COMPUTE_RX_WINDOW_PARAMETERS( )      IN865_CASE { RegionIN865ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define IN865_RX_CONFIG( )                         IN865_CASE { return RegionIN865RxConfig( rxConfig, datarate ); }
#define IN865_TX_CONFIG( )                         IN865_CASE { return RegionIN865TxConfig( txConfig, txPower, txTimeOnAir ); }
#define IN865_LINK_ADR_REQ( )                      IN865_CASE { return RegionIN865LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define IN865_RX_PARAM_SETUP_REQ( )                IN865_CASE { return RegionIN865RxParamSetupReq( rxParamSetupReq ); }
#define IN865_NEW_CHANNEL_REQ( )                   IN865_CASE { return RegionIN865NewChannelReq( newChannelReq ); }
#define IN865_TX_PARAM_SETUP_REQ( )                IN865_CASE { return RegionIN865TxParamSetupReq( txParamSetupReq ); }
#define IN865_DL_CHANNEL_REQ( )                    IN865_CASE { return RegionIN865DlChannelReq( dlChannelReq ); }
#define IN865_ALTERNATE_DR( )                      IN865_CASE { return RegionIN865AlternateDr( currentDr, type ); }
#define IN865_NEXT_CHANNEL( )                      IN865_CASE { return RegionIN865NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define IN865_CHANNEL_ADD( )                       IN865_CASE { return RegionIN865ChannelAdd( channelAdd ); }
#define IN865_CHANNEL_REMOVE( )                    IN865_CASE { return RegionIN865ChannelsRemove( channelRemove ); }
#define IN865_SET_CONTINUOUS_WAVE( )               IN865_CASE { RegionIN865SetContinuousWave( continuousWave ); break; }
#define IN865_APPLY_DR_OFFSET( )                   IN865_CASE { return RegionIN865ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define IN865_RX_BEACON_SETUP( )                   IN865_CASE { RegionIN865RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define IN865_IS_ACTIVE( )
#define IN865_GET_PHY_PARAM( )
#define IN865_SET_BAND_TX_DONE( )
#define IN865_INIT_DEFAULTS( )
#define IN865_GET_NVM_CTX( )
#define IN865_VERIFY( )
#define IN865_APPLY_CF_LIST( )
#define IN865_CHAN_MASK_SET( )
#define IN865_COMPUTE_RX_WINDOW_PARAMETERS( )
#define IN865_RX_CONFIG( )
#define IN865_TX_CONFIG( )
#define IN865_LINK_ADR_REQ( )
#define IN865_RX_PARAM_SETUP_REQ( )
#define IN865_NEW_CHANNEL_REQ( )
#define IN865_TX_PARAM_SETUP_REQ( )
#define IN865_DL_CHANNEL_REQ( )
#define IN865_ALTERNATE_DR( )
#define IN865_NEXT_CHANNEL( )
#define IN865_CHANNEL_ADD( )
#define IN865_CHANNEL_REMOVE( )
#define IN865_SET_CONTINUOUS_WAVE( )
#define IN865_APPLY_DR_OFFSET( )
#define IN865_RX_BEACON_SETUP( )
#endif

#ifdef REGION_US915
#include "RegionUS915.h"
#define US915_CASE                                 case LORAMAC_REGION_US915:
#define US915_IS_ACTIVE( )                         US915_CASE { return true; }
#define US915_GET_PHY_PARAM( )                     US915_CASE { return RegionUS915GetPhyParam( getPhy ); }
#define US915_SET_BAND_TX_DONE( )                  US915_CASE { RegionUS915SetBandTxDone( txDone ); break; }
#define US915_INIT_DEFAULTS( )                     US915_CASE { RegionUS915InitDefaults( params ); break; }
#define US915_GET_NVM_CTX( )                       US915_CASE { return RegionUS915GetNvmCtx( params ); }
#define US915_VERIFY( )                            US915_CASE { return RegionUS915Verify( verify, phyAttribute ); }
#define US915_APPLY_CF_LIST( )                     US915_CASE { RegionUS915ApplyCFList( applyCFList ); break; }
#define US915_CHAN_MASK_SET( )                     US915_CASE { return RegionUS915ChanMaskSet( chanMaskSet ); }
#define US915_COMPUTE_RX_WINDOW_PARAMETERS( )      US915_CASE { RegionUS915ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define US915_RX_CONFIG( )                         US915_CASE { return RegionUS915RxConfig( rxConfig, datarate ); }
#define US915_TX_CONFIG( )                         US915_CASE { return RegionUS915TxConfig( txConfig, txPower, txTimeOnAir ); }
#define US915_LINK_ADR_REQ( )                      US915_CASE { return RegionUS915LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define US915_RX_PARAM_SETUP_REQ( )                US915_CASE { return RegionUS915RxParamSetupReq( rxParamSetupReq ); }
#define US915_NEW_CHANNEL_REQ( )                   US915_CASE { return RegionUS915NewChannelReq( newChannelReq ); }
#define US915_TX_PARAM_SETUP_REQ( )                US915_CASE { return RegionUS915TxParamSetupReq( txParamSetupReq ); }
#define US915_DL_CHANNEL_REQ( )                    US915_CASE { return RegionUS915DlChannelReq( dlChannelReq ); }
#define US915_ALTERNATE_DR( )                      US915_CASE { return RegionUS915AlternateDr( currentDr, type ); }
#define US915_NEXT_CHANNEL( )                      US915_CASE { return RegionUS915NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define US915_CHANNEL_ADD( )                       US915_CASE { return RegionUS915ChannelAdd( channelAdd ); }
#define US915_CHANNEL_REMOVE( )                    US915_CASE { return RegionUS915ChannelsRemove( channelRemove ); }
#define US915_SET_CONTINUOUS_WAVE( )               US915_CASE { RegionUS915SetContinuousWave( continuousWave ); break; }
#define US915_APPLY_DR_OFFSET( )                   US915_CASE { return RegionUS915ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define US915_RX_BEACON_SETUP( )                   US915_CASE { RegionUS915RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define US915_IS_ACTIVE( )
#define US915_GET_PHY_PARAM( )
#define US915_SET_BAND_TX_DONE( )
#define US915_INIT_DEFAULTS( )
#define US915_GET_NVM_CTX( )
#define US915_VERIFY( )
#define US915_APPLY_CF_LIST( )
#define US915_CHAN_MASK_SET( )
#define US915_COMPUTE_RX_WINDOW_PARAMETERS( )
#define US915_RX_CONFIG( )
#define US915_TX_CONFIG( )
#define US915_LINK_ADR_REQ( )
#define US915_RX_PARAM_SETUP_REQ( )
#define US915_NEW_CHANNEL_REQ( )
#define US915_TX_PARAM_SETUP_REQ( )
#define US915_DL_CHANNEL_REQ( )
#define US915_ALTERNATE_DR( )
#define US915_NEXT_CHANNEL( )
#define US915_CHANNEL_ADD( )
#define US915_CHANNEL_REMOVE( )
#define US915_SET_CONTINUOUS_WAVE( )
#define US915_APPLY_DR_OFFSET( )
#define US915_RX_BEACON_SETUP( )
#endif

#ifdef REGION_RU864
#include "RegionRU864.h"
#define RU864_CASE                                 case LORAMAC_REGION_RU864:
#define RU864_IS_ACTIVE( )                         RU864_CASE { return true; }
#define RU864_GET_PHY_PARAM( )                     RU864_CASE { return RegionRU864GetPhyParam( getPhy ); }
#define RU864_SET_BAND_TX_DONE( )                  RU864_CASE { RegionRU864SetBandTxDone( txDone ); break; }
#define RU864_INIT_DEFAULTS( )                     RU864_CASE { RegionRU864InitDefaults( params ); break; }
#define RU864_GET_NVM_CTX( )                       RU864_CASE { return RegionRU864GetNvmCtx( params ); }
#define RU864_VERIFY( )                            RU864_CASE { return RegionRU864Verify( verify, phyAttribute ); }
#define RU864_APPLY_CF_LIST( )                     RU864_CASE { RegionRU864ApplyCFList( applyCFList ); break; }
#define RU864_CHAN_MASK_SET( )                     RU864_CASE { return RegionRU864ChanMaskSet( chanMaskSet ); }
#define RU864_COMPUTE_RX_WINDOW_PARAMETERS( )      RU864_CASE { RegionRU864ComputeRxWindowParameters( datarate, minRxSymbols, rxError, rxConfigParams ); break; }
#define RU864_RX_CONFIG( )                         RU864_CASE { return RegionRU864RxConfig( rxConfig, datarate ); }
#define RU864_TX_CONFIG( )                         RU864_CASE { return RegionRU864TxConfig( txConfig, txPower, txTimeOnAir ); }
#define RU864_LINK_ADR_REQ( )                      RU864_CASE { return RegionRU864LinkAdrReq( linkAdrReq, drOut, txPowOut, nbRepOut, nbBytesParsed ); }
#define RU864_RX_PARAM_SETUP_REQ( )                RU864_CASE { return RegionRU864RxParamSetupReq( rxParamSetupReq ); }
#define RU864_NEW_CHANNEL_REQ( )                   RU864_CASE { return RegionRU864NewChannelReq( newChannelReq ); }
#define RU864_TX_PARAM_SETUP_REQ( )                RU864_CASE { return RegionRU864TxParamSetupReq( txParamSetupReq ); }
#define RU864_DL_CHANNEL_REQ( )                    RU864_CASE { return RegionRU864DlChannelReq( dlChannelReq ); }
#define RU864_ALTERNATE_DR( )                      RU864_CASE { return RegionRU864AlternateDr( currentDr, type ); }
#define RU864_NEXT_CHANNEL( )                      RU864_CASE { return RegionRU864NextChannel( nextChanParams, channel, time, aggregatedTimeOff ); }
#define RU864_CHANNEL_ADD( )                       RU864_CASE { return RegionRU864ChannelAdd( channelAdd ); }
#define RU864_CHANNEL_REMOVE( )                    RU864_CASE { return RegionRU864ChannelsRemove( channelRemove ); }
#define RU864_SET_CONTINUOUS_WAVE( )               RU864_CASE { RegionRU864SetContinuousWave( continuousWave ); break; }
#define RU864_APPLY_DR_OFFSET( )                   RU864_CASE { return RegionRU864ApplyDrOffset( downlinkDwellTime, dr, drOffset ); }
#define RU864_RX_BEACON_SETUP( )                   RU864_CASE { RegionRU864RxBeaconSetup( rxBeaconSetup, outDr ); break; }
#else
#define RU864_IS_ACTIVE( )
#define RU864_GET_PHY_PARAM( )
#define RU864_SET_BAND_TX_DONE( )
#define RU864_INIT_DEFAULTS( )
#define RU864_GET_NVM_CTX( )
#define RU864_VERIFY( )
#define RU864_APPLY_CF_LIST( )
#define RU864_CHAN_MASK_SET( )
#define RU864_COMPUTE_RX_WINDOW_PARAMETERS( )
#define RU864_RX_CONFIG( )
#define RU864_TX_CONFIG( )
#define RU864_LINK_ADR_REQ( )
#define RU864_RX_PARAM_SETUP_REQ( )
#define RU864_NEW_CHANNEL_REQ( )
#define RU864_TX_PARAM_SETUP_REQ( )
#define RU864_DL_CHANNEL_REQ( )
#define RU864_ALTERNATE_DR( )
#define RU864_NEXT_CHANNEL( )
#define RU864_CHANNEL_ADD( )
#define RU864_CHANNEL_REMOVE( )
#define RU864_SET_CONTINUOUS_WAVE( )
#define RU864_APPLY_DR_OFFSET( )
#define RU864_RX_BEACON_SETUP( )
#endif

bool RegionIsActive( LoRaMacRegion_t region )
{
    switch( region )
    {
        AS923_IS_ACTIVE( );
        AU915_IS_ACTIVE( );
        CN470_IS_ACTIVE( );
        CN779_IS_ACTIVE( );
        EU433_IS_ACTIVE( );
        EU868_IS_ACTIVE( );
        KR920_IS_ACTIVE( );
        IN865_IS_ACTIVE( );
        US915_IS_ACTIVE( );
        RU864_IS_ACTIVE( );
        default:
        {
            return false;
        }
    }
}

PhyParam_t RegionGetPhyParam( LoRaMacRegion_t region, GetPhyParams_t* getPhy )
{
    PhyParam_t phyParam = { 0 };
    switch( region )
    {
        AS923_GET_PHY_PARAM( );
        AU915_GET_PHY_PARAM( );
        CN470_GET_PHY_PARAM( );
        CN779_GET_PHY_PARAM( );
        EU433_GET_PHY_PARAM( );
        EU868_GET_PHY_PARAM( );
        KR920_GET_PHY_PARAM( );
        IN865_GET_PHY_PARAM( );
        US915_GET_PHY_PARAM( );
        RU864_GET_PHY_PARAM( );
        default:
        {
            return phyParam;
        }
    }
}

void RegionSetBandTxDone( LoRaMacRegion_t region, SetBandTxDoneParams_t* txDone )
{
    switch( region )
    {
        AS923_SET_BAND_TX_DONE( );
        AU915_SET_BAND_TX_DONE( );
        CN470_SET_BAND_TX_DONE( );
        CN779_SET_BAND_TX_DONE( );
        EU433_SET_BAND_TX_DONE( );
        EU868_SET_BAND_TX_DONE( );
        KR920_SET_BAND_TX_DONE( );
        IN865_SET_BAND_TX_DONE( );
        US915_SET_BAND_TX_DONE( );
        RU864_SET_BAND_TX_DONE( );
        default:
        {
            return;
        }
    }
}

void RegionInitDefaults( LoRaMacRegion_t region, InitDefaultsParams_t* params )
{
    switch( region )
    {
        AS923_INIT_DEFAULTS( );
        AU915_INIT_DEFAULTS( );
        CN470_INIT_DEFAULTS( );
        CN779_INIT_DEFAULTS( );
        EU433_INIT_DEFAULTS( );
        EU868_INIT_DEFAULTS( );
        KR920_INIT_DEFAULTS( );
        IN865_INIT_DEFAULTS( );
        US915_INIT_DEFAULTS( );
        RU864_INIT_DEFAULTS( );
        default:
        {
            break;
        }
    }
}

void* RegionGetNvmCtx( LoRaMacRegion_t region, GetNvmCtxParams_t* params )
{
    switch( region )
    {
        AS923_GET_NVM_CTX( );
        AU915_GET_NVM_CTX( );
        CN470_GET_NVM_CTX( );
        CN779_GET_NVM_CTX( );
        EU433_GET_NVM_CTX( );
        EU868_GET_NVM_CTX( );
        KR920_GET_NVM_CTX( );
        IN865_GET_NVM_CTX( );
        US915_GET_NVM_CTX( );
        RU864_GET_NVM_CTX( );
        default:
        {
           return 0;
        }
    }
}

bool RegionVerify( LoRaMacRegion_t region, VerifyParams_t* verify, PhyAttribute_t phyAttribute )
{
    switch( region )
    {
        AS923_VERIFY( );
        AU915_VERIFY( );
        CN470_VERIFY( );
        CN779_VERIFY( );
        EU433_VERIFY( );
        EU868_VERIFY( );
        KR920_VERIFY( );
        IN865_VERIFY( );
        US915_VERIFY( );
        RU864_VERIFY( );
        default:
        {
            return false;
        }
    }
}

void RegionApplyCFList( LoRaMacRegion_t region, ApplyCFListParams_t* applyCFList )
{
    switch( region )
    {
        AS923_APPLY_CF_LIST( );
        AU915_APPLY_CF_LIST( );
        CN470_APPLY_CF_LIST( );
        CN779_APPLY_CF_LIST( );
        EU433_APPLY_CF_LIST( );
        EU868_APPLY_CF_LIST( );
        KR920_APPLY_CF_LIST( );
        IN865_APPLY_CF_LIST( );
        US915_APPLY_CF_LIST( );
        RU864_APPLY_CF_LIST( );
        default:
        {
            break;
        }
    }
}

bool RegionChanMaskSet( LoRaMacRegion_t region, ChanMaskSetParams_t* chanMaskSet )
{
    switch( region )
    {
        AS923_CHAN_MASK_SET( );
        AU915_CHAN_MASK_SET( );
        CN470_CHAN_MASK_SET( );
        CN779_CHAN_MASK_SET( );
        EU433_CHAN_MASK_SET( );
        EU868_CHAN_MASK_SET( );
        KR920_CHAN_MASK_SET( );
        IN865_CHAN_MASK_SET( );
        US915_CHAN_MASK_SET( );
        RU864_CHAN_MASK_SET( );
        default:
        {
            return false;
        }
    }
}

void RegionComputeRxWindowParameters( LoRaMacRegion_t region, int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams )
{
    switch( region )
    {
        AS923_COMPUTE_RX_WINDOW_PARAMETERS( );
        AU915_COMPUTE_RX_WINDOW_PARAMETERS( );
        CN470_COMPUTE_RX_WINDOW_PARAMETERS( );
        CN779_COMPUTE_RX_WINDOW_PARAMETERS( );
        EU433_COMPUTE_RX_WINDOW_PARAMETERS( );
        EU868_COMPUTE_RX_WINDOW_PARAMETERS( );
        KR920_COMPUTE_RX_WINDOW_PARAMETERS( );
        IN865_COMPUTE_RX_WINDOW_PARAMETERS( );
        US915_COMPUTE_RX_WINDOW_PARAMETERS( );
        RU864_COMPUTE_RX_WINDOW_PARAMETERS( );
        default:
        {
            break;
        }
    }
}

bool RegionRxConfig( LoRaMacRegion_t region, RxConfigParams_t* rxConfig, int8_t* datarate )
{
    switch( region )
    {
        AS923_RX_CONFIG( );
        AU915_RX_CONFIG( );
        CN470_RX_CONFIG( );
        CN779_RX_CONFIG( );
        EU433_RX_CONFIG( );
        EU868_RX_CONFIG( );
        KR920_RX_CONFIG( );
        IN865_RX_CONFIG( );
        US915_RX_CONFIG( );
        RU864_RX_CONFIG( );
        default:
        {
            return false;
        }
    }
}

bool RegionTxConfig( LoRaMacRegion_t region, TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir )
{
    switch( region )
    {
        AS923_TX_CONFIG( );
        AU915_TX_CONFIG( );
        CN470_TX_CONFIG( );
        CN779_TX_CONFIG( );
        EU433_TX_CONFIG( );
        EU868_TX_CONFIG( );
        KR920_TX_CONFIG( );
        IN865_TX_CONFIG( );
        US915_TX_CONFIG( );
        RU864_TX_CONFIG( );
        default:
        {
            return false;
        }
    }
}

uint8_t RegionLinkAdrReq( LoRaMacRegion_t region, LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed )
{
    switch( region )
    {
        AS923_LINK_ADR_REQ( );
        AU915_LINK_ADR_REQ( );
        CN470_LINK_ADR_REQ( );
        CN779_LINK_ADR_REQ( );
        EU433_LINK_ADR_REQ( );
        EU868_LINK_ADR_REQ( );
        KR920_LINK_ADR_REQ( );
        IN865_LINK_ADR_REQ( );
        US915_LINK_ADR_REQ( );
        RU864_LINK_ADR_REQ( );
        default:
        {
            return 0;
        }
    }
}

uint8_t RegionRxParamSetupReq( LoRaMacRegion_t region, RxParamSetupReqParams_t* rxParamSetupReq )
{
    switch( region )
    {
        AS923_RX_PARAM_SETUP_REQ( );
        AU915_RX_PARAM_SETUP_REQ( );
        CN470_RX_PARAM_SETUP_REQ( );
        CN779_RX_PARAM_SETUP_REQ( );
        EU433_RX_PARAM_SETUP_REQ( );
        EU868_RX_PARAM_SETUP_REQ( );
        KR920_RX_PARAM_SETUP_REQ( );
        IN865_RX_PARAM_SETUP_REQ( );
        US915_RX_PARAM_SETUP_REQ( );
        RU864_RX_PARAM_SETUP_REQ( );
        default:
        {
            return 0;
        }
    }
}

uint8_t RegionNewChannelReq( LoRaMacRegion_t region, NewChannelReqParams_t* newChannelReq )
{
    switch( region )
    {
        AS923_NEW_CHANNEL_REQ( );
        AU915_NEW_CHANNEL_REQ( );
        CN470_NEW_CHANNEL_REQ( );
        CN779_NEW_CHANNEL_REQ( );
        EU433_NEW_CHANNEL_REQ( );
        EU868_NEW_CHANNEL_REQ( );
        KR920_NEW_CHANNEL_REQ( );
        IN865_NEW_CHANNEL_REQ( );
        US915_NEW_CHANNEL_REQ( );
        RU864_NEW_CHANNEL_REQ( );
        default:
        {
            return 0;
        }
    }
}

int8_t RegionTxParamSetupReq( LoRaMacRegion_t region, TxParamSetupReqParams_t* txParamSetupReq )
{
    switch( region )
    {
        AS923_TX_PARAM_SETUP_REQ( );
        AU915_TX_PARAM_SETUP_REQ( );
        CN470_TX_PARAM_SETUP_REQ( );
        CN779_TX_PARAM_SETUP_REQ( );
        EU433_TX_PARAM_SETUP_REQ( );
        EU868_TX_PARAM_SETUP_REQ( );
        KR920_TX_PARAM_SETUP_REQ( );
        IN865_TX_PARAM_SETUP_REQ( );
        US915_TX_PARAM_SETUP_REQ( );
        RU864_TX_PARAM_SETUP_REQ( );
        default:
        {
            return 0;
        }
    }
}

uint8_t RegionDlChannelReq( LoRaMacRegion_t region, DlChannelReqParams_t* dlChannelReq )
{
    switch( region )
    {
        AS923_DL_CHANNEL_REQ( );
        AU915_DL_CHANNEL_REQ( );
        CN470_DL_CHANNEL_REQ( );
        CN779_DL_CHANNEL_REQ( );
        EU433_DL_CHANNEL_REQ( );
        EU868_DL_CHANNEL_REQ( );
        KR920_DL_CHANNEL_REQ( );
        IN865_DL_CHANNEL_REQ( );
        US915_DL_CHANNEL_REQ( );
        RU864_DL_CHANNEL_REQ( );
        default:
        {
            return 0;
        }
    }
}

int8_t RegionAlternateDr( LoRaMacRegion_t region, int8_t currentDr, AlternateDrType_t type )
{
    switch( region )
    {
        AS923_ALTERNATE_DR( );
        AU915_ALTERNATE_DR( );
        CN470_ALTERNATE_DR( );
        CN779_ALTERNATE_DR( );
        EU433_ALTERNATE_DR( );
        EU868_ALTERNATE_DR( );
        KR920_ALTERNATE_DR( );
        IN865_ALTERNATE_DR( );
        US915_ALTERNATE_DR( );
        RU864_ALTERNATE_DR( );
        default:
        {
            return 0;
        }
    }
}

LoRaMacStatus_t RegionNextChannel( LoRaMacRegion_t region, NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff )
{
    switch( region )
    {
        AS923_NEXT_CHANNEL( );
        AU915_NEXT_CHANNEL( );
        CN470_NEXT_CHANNEL( );
        CN779_NEXT_CHANNEL( );
        EU433_NEXT_CHANNEL( );
        EU868_NEXT_CHANNEL( );
        KR920_NEXT_CHANNEL( );
        IN865_NEXT_CHANNEL( );
        US915_NEXT_CHANNEL( );
        RU864_NEXT_CHANNEL( );
        default:
        {
            return LORAMAC_STATUS_REGION_NOT_SUPPORTED;
        }
    }
}

LoRaMacStatus_t RegionChannelAdd( LoRaMacRegion_t region, ChannelAddParams_t* channelAdd )
{
    switch( region )
    {
        AS923_CHANNEL_ADD( );
        AU915_CHANNEL_ADD( );
        CN470_CHANNEL_ADD( );
        CN779_CHANNEL_ADD( );
        EU433_CHANNEL_ADD( );
        EU868_CHANNEL_ADD( );
        KR920_CHANNEL_ADD( );
        IN865_CHANNEL_ADD( );
        US915_CHANNEL_ADD( );
        RU864_CHANNEL_ADD( );
        default:
        {
            return LORAMAC_STATUS_PARAMETER_INVALID;
        }
    }
}

bool RegionChannelsRemove( LoRaMacRegion_t region, ChannelRemoveParams_t* channelRemove )
{
    switch( region )
    {
        AS923_CHANNEL_REMOVE( );
        AU915_CHANNEL_REMOVE( );
        CN470_CHANNEL_REMOVE( );
        CN779_CHANNEL_REMOVE( );
        EU433_CHANNEL_REMOVE( );
        EU868_CHANNEL_REMOVE( );
        KR920_CHANNEL_REMOVE( );
        IN865_CHANNEL_REMOVE( );
        US915_CHANNEL_REMOVE( );
        RU864_CHANNEL_REMOVE( );
        default:
        {
            return false;
        }
    }
}

void RegionSetContinuousWave( LoRaMacRegion_t region, ContinuousWaveParams_t* continuousWave )
{
    switch( region )
    {
        AS923_SET_CONTINUOUS_WAVE( );
        AU915_SET_CONTINUOUS_WAVE( );
        CN470_SET_CONTINUOUS_WAVE( );
        CN779_SET_CONTINUOUS_WAVE( );
        EU433_SET_CONTINUOUS_WAVE( );
        EU868_SET_CONTINUOUS_WAVE( );
        KR920_SET_CONTINUOUS_WAVE( );
        IN865_SET_CONTINUOUS_WAVE( );
        US915_SET_CONTINUOUS_WAVE( );
        RU864_SET_CONTINUOUS_WAVE( );
        default:
        {
            break;
        }
    }
}

uint8_t RegionApplyDrOffset( LoRaMacRegion_t region, uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset )
{
    switch( region )
    {
        AS923_APPLY_DR_OFFSET( );
        AU915_APPLY_DR_OFFSET( );
        CN470_APPLY_DR_OFFSET( );
        CN779_APPLY_DR_OFFSET( );
        EU433_APPLY_DR_OFFSET( );
        EU868_APPLY_DR_OFFSET( );
        KR920_APPLY_DR_OFFSET( );
        IN865_APPLY_DR_OFFSET( );
        US915_APPLY_DR_OFFSET( );
        RU864_APPLY_DR_OFFSET( );
        default:
        {
            return dr;
        }
    }
}

void RegionRxBeaconSetup( LoRaMacRegion_t region, RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr )
{
    switch( region )
    {
        AS923_RX_BEACON_SETUP( );
        AU915_RX_BEACON_SETUP( );
        CN470_RX_BEACON_SETUP( );
        CN779_RX_BEACON_SETUP( );
        EU433_RX_BEACON_SETUP( );
        EU868_RX_BEACON_SETUP( );
        KR920_RX_BEACON_SETUP( );
        IN865_RX_BEACON_SETUP( );
        US915_RX_BEACON_SETUP( );
        RU864_RX_BEACON_SETUP( );
        default:
        {
            break;
        }
    }
}

Version_t RegionGetVersion( void )
{
    Version_t version;

    version.Value = REGION_VERSION;

    return version;
}

