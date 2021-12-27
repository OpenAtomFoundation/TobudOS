/*!
 * \file      LoRaMacFCntHandler.h
 *
 * \brief     LoRa MAC layer frame counter handling
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
 *              (C)2013 Semtech
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
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * addtogroup LORAMAC
 * \{
 *
 */
#ifndef __LORAMAC_FCNT_HANDLER_H__
#define __LORAMAC_FCNT_HANDLER_H__

#include <stdint.h>
#include "LoRaMacTypes.h"
#include "LoRaMacMessageTypes.h"

/*!
 * LoRaMac FCnt Handler Status
 */
typedef enum eLoRaMacFCntHandlerStatus
{
    /*!
     * No error occurred
     */
    LORAMAC_FCNT_HANDLER_SUCCESS = 0,
    /*!
     * FCntUp/Down check failed
     */
    LORAMAC_FCNT_HANDLER_CHECK_FAIL,
    /*!
     * The status idicates that the node has lost MAX_FCNT_GAP or more frames
     */
    LORAMAC_FCNT_HANDLER_MAX_GAP_FAIL,
    /*!
     * Invalid frame counter identifier exception
     */
    LORAMAC_FCNT_HANDLER_ERROR_INVALID_FCNT_ID,
    /*!
     * Null pointer exception
     */
    LORAMAC_FCNT_HANDLER_ERROR_NPE,
    /*!
     * Undefined Error occurred
     */
    LORAMAC_FCNT_HANDLER_ERROR,
}LoRaMacFCntHandlerStatus_t;

/*!
 * Signature of callback function to be called by this module when the
 * non-volatile needs to be saved.
 */
typedef void ( *EventNvmCtxChanged )( void );

/*!
 * \brief Initialization of LoRaMac FCnt Handler module
 *
 * \param[IN]    fCntHandlerNvmCtxChanged - Callback function which will be called when the
 *                                          non-volatile context needs to be saved.
 *
 * \retval                      - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacFCntHandlerInit( EventNvmCtxChanged fCntHandlerNvmCtxChanged );

/*!
 * Restores the internal non-volatile context from passed pointer.
 *
 * \param[IN]     fCntHandlerNvmCtx      - Pointer to non-volatile FCnt handler module context to be restored.
 *
 * \retval                     - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacFCntHandlerRestoreNvmCtx( void* fCntHandlerNvmCtx );

/*!
 * Returns a pointer to the internal non-volatile context.
 *
 * \param[IN]     fCntHandlerNvmCtxSize  - Size of the module non-volatile context
 *
 * \retval                    - Points to a structure where the module store its non-volatile context
 */
void* LoRaMacFCntHandlerGetNvmCtx( size_t* fCntHandlerNvmCtxSize );

/*!
 * Computes next 32 bit downlink counter value and determines the frame counter ID.
 *
 * \param[IN]     addrID                - Address identifier
 * \param[IN]     fType                 - Frame type
 * \param[IN]     macMsg                - Data message object, holding the current 16 bit transmitted frame counter
 * \param[IN]     lrWanVersion          - LoRaWAN version
 * \param[IN]     maxFCntGap            - Maximum allowed frame counter difference (only for 1.0.X necessary)
 * \param[OUT]    fCntID                - Frame counter identifier
 * \param[OUT]    currentDown           - Current downlink counter value
 *
 * \retval                     - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacGetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                               uint16_t maxFCntGap, FCntIdentifier_t* fCntID, uint32_t* currentDown );

/*!
 * Sets the downlink counter value according to the counter ID.
 *
 * \param[IN]     fCntID          - Frame counter identifier
 * \param[IN]     currentDown     - Current downlink counter value
 *
 * \retval                     - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacSetFCntDown( FCntIdentifier_t fCntID, uint32_t currentDown );

/*!
 * Computes next uplink counter value.
 *
 * \param[OUT]    currentUp       - Current uplink counter value
 *
 * \retval                     - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacGetFCntUp( uint32_t* currentUp );

/*!
 * Sets the uplink counter value according to the counter ID.
 *
 * \param[IN]     currentUp       - Current uplink counter value
 *
 * \retval                     - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacSetFCntUp( uint32_t currentUp );

/*!
 * Resets the value of FCntUp, NFCntDown, AFCntDown and FCntDown to zero
 *
 * \retval                             - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacResetFCnts( void );

/*!
 * Sets the reference to the multicast downlink counter
 *
 * \param[IN]     multicastList Pointer to the multicast list
 *
 * \retval                     - Status of the operation
 */
LoRaMacFCntHandlerStatus_t LoRaMacFCntHandlerSetMulticastReference( MulticastCtx_t* multicastList );

/*! \} addtogroup LORAMAC */

#endif // __LORAMAC_FCNT_HANDLER_H__

