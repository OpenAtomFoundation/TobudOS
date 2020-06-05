/*!
 * \file      LoRaMacConfirmQueue.h
 *
 * \brief     LoRa MAC confirm queue implementation
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
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \defgroup  LORAMACCONFIRMQUEUE LoRa MAC confirm queue implementation
 *            This module specifies the API implementation of the LoRaMAC confirm queue.
 *            The confirm queue is implemented with as a ring buffer. The number of
 *            elements can be defined with \ref LORA_MAC_MLME_CONFIRM_QUEUE_LEN. The
 *            current implementation does not support multiple elements of the same
 *            Mlme_t type.
 * \{
 */
#ifndef __LORAMAC_CONFIRMQUEUE_H__
#define __LORAMAC_CONFIRMQUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "LoRaMac.h"

/*!
 * LoRaMac MLME-Confirm queue length
 */
#define LORA_MAC_MLME_CONFIRM_QUEUE_LEN             5

/*!
 * Structure to hold multiple MLME request confirm data
 */
typedef struct sMlmeConfirmQueue
{
    /*!
     * Holds the previously performed MLME-Request
     */
    Mlme_t Request;
    /*!
     * Status of the operation
     */
    LoRaMacEventInfoStatus_t Status;
    /*!
     * Set to true, if the request is ready to be handled
     */
    bool ReadyToHandle;
    /*!
     * Set to true, if it is not permitted to set the ReadyToHandle variable
     * with a function call to LoRaMacConfirmQueueSetStatusCmn.
     */
    bool RestrictCommonReadyToHandle;
}MlmeConfirmQueue_t;

/*!
 * Signature of callback function to be called by this module when the
 * non-volatile needs to be saved.
 */
typedef void ( *LoRaMacConfirmQueueNvmEvent )( void );

/*!
 * \brief   Initializes the confirm queue
 *
 * \param   [IN] primitives - Pointer to the LoRaMac primitives.
 *
 * \param   [IN] confirmQueueNvmCtxChanged - Callback function which will be called when the
 *                                           non-volatile context needs to be saved.
 */
void LoRaMacConfirmQueueInit( LoRaMacPrimitives_t* primitives, LoRaMacConfirmQueueNvmEvent confirmQueueNvmCtxChanged  );

/*!
 * Restores the internal non-volatile context from passed pointer.
 *
 * \param   [IN] confirmQueueNvmCtx - Pointer to non-volatile class B module context to be restored.
 *
 * \retval  [true - operation was successful, false - operation failed]
 */
bool LoRaMacConfirmQueueRestoreNvmCtx( void* confirmQueueNvmCtx );

/*!
 * Returns a pointer to the internal non-volatile context.
 *
 * \param   [IN] confirmQueueNvmCtxSize - Size of the module non-volatile context
 *
 * \retval  - Points to a structure where the module store its non-volatile context
 */
void* LoRaMacConfirmQueueGetNvmCtx( size_t* confirmQueueNvmCtxSize );

/*!
 * \brief   Adds an element to the confirm queue.
 *
 * \param   [IN] mlmeConfirm - Pointer to the element to add.
 *
 * \retval  [true - operation was successful, false - operation failed]
 */
bool LoRaMacConfirmQueueAdd( MlmeConfirmQueue_t* mlmeConfirm );

/*!
 * \brief   Removes the last element which was added into the queue.
 *
 * \retval  [true - operation was successful, false - operation failed]
 */
bool LoRaMacConfirmQueueRemoveLast( void );

/*!
 * \brief   Removes the first element which was added to the confirm queue.
 *
 * \retval  [true - operation was successful, false - operation failed]
 */
bool LoRaMacConfirmQueueRemoveFirst( void );

/*!
 * \brief   Sets the status of an element.
 *
 * \param   [IN] status - The status to set.
 *
 * \param   [IN] request - The related request to set the status.
 */
void LoRaMacConfirmQueueSetStatus( LoRaMacEventInfoStatus_t status, Mlme_t request );

/*!
 * \brief   Gets the status of an element.
 *
 * \param   [IN] request - The request to query the status.
 *
 * \retval  The status of the related MlmeRequest.
 */
LoRaMacEventInfoStatus_t LoRaMacConfirmQueueGetStatus( Mlme_t request );

/*!
 * \brief   Sets a common status for all elements in the queue.
 *
 * \param   [IN] status - The status to set.
 */
void LoRaMacConfirmQueueSetStatusCmn( LoRaMacEventInfoStatus_t status );

/*!
 * \brief   Gets the common status of all elements.
 *
 * \retval  The common status of all elements.
 */
LoRaMacEventInfoStatus_t LoRaMacConfirmQueueGetStatusCmn( void );

/*!
 * \brief   Verifies if a request is in the queue and active.
 *
 * \param   [IN] request - The request to verify.
 *
 * \retval  [true - element is in the queue, false - element is not in the queue].
 */
bool LoRaMacConfirmQueueIsCmdActive( Mlme_t request );

/*!
 * \brief   Handles all callbacks of active requests
 *
 * \param   [IN] mlmeConfirm - Pointer to the generic mlmeConfirm structure.
 */
void LoRaMacConfirmQueueHandleCb( MlmeConfirm_t* mlmeConfirm );

/*!
 * \brief   Query number of elements in the queue.
 *
 * \retval  Number of elements.
 */
uint8_t LoRaMacConfirmQueueGetCnt( void );

/*!
 * \brief   Verify if the confirm queue is full.
 *
 * \retval  [true - queue is full, false - queue is not full].
 */
bool LoRaMacConfirmQueueIsFull( void );

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_CONFIRMQUEUE_H__
