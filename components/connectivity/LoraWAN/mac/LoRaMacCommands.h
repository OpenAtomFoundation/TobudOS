/*!
 * \file      LoRaMacCommands.h
 *
 * \brief     LoRa MAC commands
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
#ifndef __LORAMAC_COMMANDS_H__
#define __LORAMAC_COMMANDS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include "LoRaMacTypes.h"


/*
 * Number of MAC Command slots
 */
#define LORAMAC_COMMADS_MAX_NUM_OF_PARAMS   2

/*!
 * LoRaWAN MAC Command element
 */
typedef struct sMacCommand MacCommand_t;

struct sMacCommand
{
    /*!
     *  The pointer to the next MAC Command element in the list
     */
    MacCommand_t* Next;
    /*!
     * MAC command identifier
     */
    uint8_t CID;
    /*!
     * MAC command payload
     */
    uint8_t Payload[LORAMAC_COMMADS_MAX_NUM_OF_PARAMS];
    /*!
     * Size of MAC command payload
     */
    size_t PayloadSize;
    /*!
     * Indicates if it's a sticky MAC command
     */
    bool IsSticky;
};

/*!
 * LoRaMac Commands Status
 */
typedef enum eLoRaMacCommandsStatus
{
    /*!
     * No error occurred
     */
    LORAMAC_COMMANDS_SUCCESS = 0,
    /*!
     * Null pointer exception
     */
    LORAMAC_COMMANDS_ERROR_NPE,
    /*!
     * There is no memory left to add a further MAC command
     */
    LORAMAC_COMMANDS_ERROR_MEMORY,
    /*!
     * MAC command not found.
     */
    LORAMAC_COMMANDS_ERROR_CMD_NOT_FOUND,
    /*!
     * Unknown or corrupted command error occurred.
     */
    LORAMAC_COMMANDS_ERROR_UNKNOWN_CMD,
    /*!
     * Undefined Error occurred
     */
    LORAMAC_COMMANDS_ERROR,
}LoRaMacCommandStatus_t;

/*!
 * Signature of callback function to be called by this module when the
 * non-volatile needs to be saved.
 */
typedef void ( *LoRaMacCommandsNvmEvent )( void );

/*!
 * \brief Initialization of LoRaMac MAC commands module
 *
 * \param[IN]    commandsNvmCtxChanged - Callback function which will be called when the
 *                                      non-volatile context needs to be saved.
 *
 * \retval                            - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsInit( LoRaMacCommandsNvmEvent commandsNvmCtxChanged );

/*!
 * Restores the internal non-volatile context from passed pointer.
 *
 * \param[IN]     commandsNvmCtx     - Pointer to non-volatile MAC commands module context to be restored.
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsRestoreNvmCtx( void* commandsNvmCtx );

/*!
 * Returns a pointer to the internal non-volatile context.
 *
 * \param[IN]     commandsNvmCtxSize - Size of the module non-volatile context
 *
 * \retval                    - Points to a structure where the module store its non-volatile context
 */
void* LoRaMacCommandsGetNvmCtx( size_t* commandsNvmCtxSize );

/*!
 * \brief Adds a new MAC command to be sent.
 *
 * \param[IN]   cid                - MAC command identifier
 * \param[IN]   payload            - MAC command payload containing parameters
 * \param[IN]   payloadSize        - Size of MAC command payload
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsAddCmd( uint8_t cid, uint8_t* payload, size_t payloadSize );

/*!
 * \brief Remove a MAC command.
 *
 * \param[OUT]  cmd                - MAC command
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsRemoveCmd( MacCommand_t* macCmd );

/*!
 * \brief Get the MAC command with corresponding CID.
 *
 * \param[IN]   cid                - MAC command identifier
 * \param[OUT]  cmd                - MAC command
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsGetCmd( uint8_t cid, MacCommand_t** macCmd );

/*!
 * \brief Remove all none sticky MAC commands.
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsRemoveNoneStickyCmds( void );

/*!
 * \brief Remove all sticky answer MAC commands.
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsRemoveStickyAnsCmds( void );

/*!
 * \brief Get size of all MAC commands serialized as buffer
 *
 * \param[out]   size               - Available size of memory for MAC commands
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsGetSizeSerializedCmds( size_t* size );

/*!
 * \brief Get as many as possible MAC commands serialized
 *
 * \param[IN]   availableSize      - Available size of memory for MAC commands
 * \param[out]  effectiveSize      - Size of memory which was effectively used for serializing.
 * \param[out]  buffer             - Destination data buffer
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsSerializeCmds( size_t availableSize, size_t* effectiveSize,  uint8_t* buffer );

/*!
 * \brief Determines if there are sticky MAC commands pending.
 *
 * \param[IN]   cmdsPending        - Indicates if there are sticky MAC commands in the queue.
 *
 * \retval                     - Status of the operation
 */
LoRaMacCommandStatus_t LoRaMacCommandsStickyCmdsPending( bool* cmdsPending );

/*!
 * \brief Get the MAC command size with corresponding CID.
 *
 * \param[IN]   cid                - MAC command identifier
 *
 * \retval Size of the command.
 */
uint8_t LoRaMacCommandsGetCmdSize( uint8_t cid );

/*! \} addtogroup LORAMAC */

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_COMMANDS_H__

