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
#ifndef __LMHANDLER_MSG_DISPLAY_H__
#define __LMHANDLER_MSG_DISPLAY_H__

#include "utilities.h"
#include "LmHandler.h"

/*!
 * \brief Displays NVM context operation state
 *
 * \param [IN] state Indicates if we are storing (true) or
 *                    restoring (false) the NVM context
 */
void DisplayNvmContextChange( LmHandlerNvmContextStates_t state );

/*!
 * \brief Displays updated network parameters
 *
 * \param [IN] commissioningParams Commissioning provided parameters
 */
void DisplayNetworkParametersUpdate( CommissioningParams_t* commissioningParams );

/*!
 * \brief Displays updated McpsRequest
 *
 * \param [IN] status McpsRequest execution status
 * \param [IN] mcpsReq McpsRequest command executed
 * \param [IN] nextTxIn Time to wait for the next uplink transmission
 */
void DisplayMacMcpsRequestUpdate( LoRaMacStatus_t status, McpsReq_t *mcpsReq, TimerTime_t nextTxIn );

/*!
 * \brief Displays updated MlmeRequest
 *
 * \param [IN] status MlmeRequest execution status
 * \param [IN] mlmeReq MlmeRequest command executed
 * \param [IN] nextTxIn Time to wait for the next uplink transmission
 */
void DisplayMacMlmeRequestUpdate( LoRaMacStatus_t status, MlmeReq_t *mlmeReq, TimerTime_t nextTxIn );

/*!
 * \brief Displays updated JoinRequest
 *
 * \param [IN] params Executed JoinRequest parameters
 */
void DisplayJoinRequestUpdate( LmHandlerJoinParams_t* params );

/*!
 * \brief Displays Tx params
 *
 * \param [IN] params Tx parameters
 */
void DisplayTxUpdate( LmHandlerTxParams_t* params );

/*!
 * \brief Displays Rx params
 *
 * \param [IN] appData Receive data payload and port number
 * \param [IN] params Rx parameters
 */
void DisplayRxUpdate( LmHandlerAppData_t* appData, LmHandlerRxParams_t* params );

/*!
 * \brief Displays beacon status update
 *
 * \param [IN] params Beacon parameters
 */
void DisplayBeaconUpdate( LoRaMAcHandlerBeaconParams_t* params );

/*!
 * \brief Displays end-device class update
 *
 * \param [IN] deviceClass Current end-device class
 */
void DisplayClassUpdate( DeviceClass_t deviceClass );

/*!
 * \brief Displays application information
 */
void DisplayAppInfo( const char* appName, const Version_t* appVersion, const Version_t* gitHubVersion );

#endif // __LMHANDLER_MSG_DISPLAY_H__
