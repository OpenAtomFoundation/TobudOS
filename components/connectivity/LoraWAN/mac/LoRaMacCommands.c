/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech
 ___ _____ _   ___ _  _____ ___  ___  ___ ___
/ __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
\__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
|___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
embedded.connectivity.solutions===============

Description: LoRa MAC commands

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis ( Semtech ), Daniel Jaeckle ( STACKFORCE ), Johannes Bruder ( STACKFORCE )
*/

#include "utilities.h"
#include "LoRaMacCommands.h"
#include "LoRaMacConfirmQueue.h"

/*
 * Number of MAC Command slots
 */
#define NUM_OF_MAC_COMMANDS 15

/*
 * Size of the CID field of MAC commands
 */
#define CID_FIELD_SIZE  1

/*
 * List of all stick MAC command answers which will be deleted after a receiving downlink
 */
const uint8_t CIDsStickyAnsCmds[] = { MOTE_MAC_DL_CHANNEL_ANS, MOTE_MAC_RX_PARAM_SETUP_ANS, MOTE_MAC_RX_TIMING_SETUP_ANS };

/*
 *  Mac Commands list structure
 */
typedef struct sMacCommandsList
{
    /*
     * First element of MAC command list.
     */
    MacCommand_t* First;
    /*
     * Last element of MAC command list.
     */
    MacCommand_t* Last;
} MacCommandsList_t;

/*
 * LoRaMac Commands Context structure
 */
typedef struct sLoRaMacCommandsCtx
{
    /*
     * List of MAC command elements
     */
    MacCommandsList_t MacCommandList;
    /*
     * Buffer to store MAC command elements
     */
    MacCommand_t MacCommandSlots[NUM_OF_MAC_COMMANDS];
    /*
     * Size of all MAC commands serialized as buffer
     */
    size_t SerializedCmdsSize;
} LoRaMacCommandsCtx_t;

/*
 * Callback function to notify the upper layer about context change
 */
static EventNvmCtxChanged CommandsNvmCtxChanged;

/*
 * Non-volatile module context.
 */
static LoRaMacCommandsCtx_t NvmCtx;

/* Memory management functions */

/*
 * \brief Determines if a MAC command slot is free
 *
 * \param[IN]     slot           - Slot to check
 * \retval                       - Status of the operation
 */
bool isSlotFree( const MacCommand_t* slot )
{
    uint8_t* mem = (uint8_t*) slot;

    for( uint16_t size = 0; size < sizeof( MacCommand_t ); size++ )
    {
        if( mem[size] != 0x00 )
        {
            return false;
        }
    }
    return true;
}

/*
 * \brief Allocates a new MAC command memory slot
 *
 * \retval                       - Pointer to slot
 */
MacCommand_t* mallocNewMacCommandSlot( )
{
    uint8_t itr = 0;

    while( isSlotFree( ( const MacCommand_t* ) &NvmCtx.MacCommandSlots[itr]) == false )
    {
        itr++;
        if( itr == NUM_OF_MAC_COMMANDS )
        {
            return 0;
        }
    }

    return &NvmCtx.MacCommandSlots[itr];
}


/*
 * \brief Free memory slot
 *
 * \param[IN]     slot           - Slot to free
 *
 * \retval                       - Status of the operation
 */
bool freeMacCommandSlot( MacCommand_t* slot )
{
    if( slot == 0 )
    {
        return false;
    }

    memset1( (uint8_t*) slot, 0x00, sizeof( MacCommand_t ));

    return true;
}


/* Linked list functions */

/*
 * \brief Initialize list
 *
 * \param[IN]     list           - List that shall be initialized
 * \retval                       - Status of the operation
 */
static bool linkedListInit( MacCommandsList_t* list )
{
    if( list == 0 )
    {
        return false;
    }

    list->First = 0;
    list->Last = 0;

    return true;
}

/*
 * \brief Add an element to the list
 *
 * \param[IN]     list           - List where the element shall be added.
 * \param[IN]     element        - Element to add
 * \retval                       - Status of the operation
 */
static bool linkedListAdd( MacCommandsList_t* list, MacCommand_t* element )
{
    if( ( list == 0 ) && ( element == 0 ) )
    {
        return false;
    }

    /* Check if this is the first entry to enter the list. */
    if( list->First == 0 )
    {
        list->First = element;
    }

    /* Check if the last entry exists and update its next point. */
    if( list->Last )
    {
        list->Last->Next = element;
    }

    /* Update the next point of this entry. */
    element->Next = 0;

    /* Update the last entry of the list. */
    list->Last = element;

    return true;
}

/*
 * \brief Return the previous element in the list.
 *
 * \param[IN]     list           - List
 * \param[IN]     element        - Element where the previous element shall be searched
 * \retval                       - Status of the operation
 */
static MacCommand_t* linkedListGetPrevious( MacCommandsList_t* list, MacCommand_t* element )
{
    if( ( list == 0 ) && ( element == 0 ) )
    {
        return NULL;
    }

    MacCommand_t* curElement;

    // Start at the head of the list
    curElement = list->First;

    /*
     * When current element is the first of the list, there's no previous element so we can return NULL immediately.
     */
    if( element != curElement)
    {

        // Loop through all elements until the end is reached or the next of current is the current element.
        while(curElement && (curElement->Next != element))
        {
            curElement = curElement->Next;
        }
    }
    else
    {
        curElement = NULL;
    }

    return curElement;
}

/*
 * \brief Remove an element from the list
 *
 * \param[IN]     list           - List where the element shall be removed from.
 * \param[IN]     element        - Element to remove
 * \retval                       - Status of the operation
 */
static bool linkedListRemove( MacCommandsList_t* list, MacCommand_t* element )
{
    if( ( list == 0 ) && ( element == 0 ) )
    {
        return false;
    }

    MacCommand_t* PrevElement = linkedListGetPrevious( list, element );

    if( list->First == element )
    {
        list->First = element->Next;
    }

    if( list->Last == element )
    {
        list->Last = PrevElement;
    }

    if( PrevElement )
    {
        PrevElement->Next = element->Next;
    }

    element->Next = 0;

    return true;
}

/*
 * \brief Determines if a MAC command is sticky or not
 *
 * \param[IN]   cid                - MAC command identifier
 *
 * \retval                     - Status of the operation
 */
static bool IsSticky( uint8_t cid )
{
    switch( cid )
    {
        case MOTE_MAC_DL_CHANNEL_ANS:
        case MOTE_MAC_RX_PARAM_SETUP_ANS:
        case MOTE_MAC_RX_TIMING_SETUP_ANS:
            return true;
        default:
            return false;
    }
}

/*
 * \brief Wrapper function for the NvmCtx
 */
static void NvmCtxCallback( void )
{
    if( CommandsNvmCtxChanged != NULL )
    {
        CommandsNvmCtxChanged( );
    }
}

LoRaMacCommandStatus_t LoRaMacCommandsInit( EventNvmCtxChanged commandsNvmCtxChanged )
{

    // Initialize with default
    memset1( (uint8_t*)&NvmCtx, 0, sizeof( NvmCtx ) );

    linkedListInit( &NvmCtx.MacCommandList );

    // Assign callback
    CommandsNvmCtxChanged = commandsNvmCtxChanged;

    return LORAMAC_COMMANDS_SUCCESS;
}

LoRaMacCommandStatus_t LoRaMacCommandsRestoreNvmCtx( void* commandsNvmCtx )
{
    // Restore module context
    if( commandsNvmCtx != NULL )
    {
        memcpy1( ( uint8_t* ) &NvmCtx, ( uint8_t* ) commandsNvmCtx, sizeof( NvmCtx ) );
        return LORAMAC_COMMANDS_SUCCESS;
    }
    else
    {
        return LORAMAC_COMMANDS_ERROR_NPE;
    }
}

void* LoRaMacCommandsGetNvmCtx( size_t* commandsNvmCtxSize )
{
    *commandsNvmCtxSize = sizeof( NvmCtx );
    return &NvmCtx;
}

LoRaMacCommandStatus_t LoRaMacCommandsAddCmd( uint8_t cid,  uint8_t* payload, size_t payloadSize )
{
    if( payload == 0 )
    {
        return LORAMAC_COMMANDS_ERROR_NPE;
    }
    MacCommand_t* newCmd;

    // Allocate a memory slot
    newCmd = mallocNewMacCommandSlot( );

    if( newCmd == 0 )
    {
        return LORAMAC_COMMANDS_ERROR_MEMORY;
    }

    // Add it to the list of Mac commands
    if( linkedListAdd( &NvmCtx.MacCommandList, newCmd ) == false )
    {
        return LORAMAC_COMMANDS_ERROR;
    }

    // Set Values
    newCmd->CID = cid;
    newCmd->PayloadSize = payloadSize;
    memcpy1( ( uint8_t* ) newCmd->Payload, payload, payloadSize );
    newCmd->IsSticky = IsSticky( cid );

    NvmCtx.SerializedCmdsSize += ( CID_FIELD_SIZE + payloadSize );

    NvmCtxCallback( );

    return LORAMAC_COMMANDS_SUCCESS;
}

LoRaMacCommandStatus_t LoRaMacCommandsRemoveCmd( MacCommand_t* macCmd )
{
    if( macCmd == NULL )
    {
        return LORAMAC_COMMANDS_ERROR_NPE;
    }

    // Remove the Mac command element from MacCommandList
    if( linkedListRemove( &NvmCtx.MacCommandList, macCmd ) == false )
    {
        return LORAMAC_COMMANDS_ERROR_CMD_NOT_FOUND;
    }

    NvmCtx.SerializedCmdsSize -= ( CID_FIELD_SIZE + macCmd->PayloadSize );

    // Free the MacCommand Slot
    if( freeMacCommandSlot( macCmd ) == false )
    {
        return LORAMAC_COMMANDS_ERROR;
    }

    NvmCtxCallback( );

    return LORAMAC_COMMANDS_SUCCESS;
}

LoRaMacCommandStatus_t LoRaMacCommandsGetCmd( uint8_t cid, MacCommand_t** macCmd )
{
    MacCommand_t* curElement;

    // Start at the head of the list
    curElement = NvmCtx.MacCommandList.First;

    // Loop through all elements until we find the element with the given CID
    while(curElement && ( curElement->CID != cid ) )
    {
        curElement = curElement->Next;
    }

    // Handle error in case if we reached the end without finding it.
    if( curElement == NULL )
    {
        return LORAMAC_COMMANDS_ERROR_CMD_NOT_FOUND;
    }

    *macCmd = curElement;

    return LORAMAC_COMMANDS_SUCCESS;
}

LoRaMacCommandStatus_t LoRaMacCommandsRemoveNoneStickyCmds( void )
{
    MacCommand_t* curElement;
    MacCommand_t* nexElement;

    // Start at the head of the list
    curElement = NvmCtx.MacCommandList.First;

    // Loop through all elements
    while( curElement )
    {
        if( curElement->IsSticky == false )
        {
            nexElement = curElement->Next;
            LoRaMacCommandsRemoveCmd( curElement );
            curElement = nexElement;
        }
        else
        {
            curElement = curElement->Next;
        }
    }

    NvmCtxCallback( );

    return LORAMAC_COMMANDS_SUCCESS;
}

LoRaMacCommandStatus_t LoRaMacCommandsRemoveStickyAnsCmds( void )
{
    MacCommand_t* curElement;
    MacCommand_t* nexElement;

    // Start at the head of the list
    curElement = NvmCtx.MacCommandList.First;

    // Loop through all elements
    while( curElement != NULL )
    {
        nexElement = curElement->Next;
        if( curElement->IsSticky == true )
        {
            for( uint8_t i = 0; i < sizeof( CIDsStickyAnsCmds ); i++)
            {
                if( curElement->CID == CIDsStickyAnsCmds[i] )
                {
                    LoRaMacCommandsRemoveCmd( curElement );
                    break;
                }
            }
        }
        curElement = nexElement;
    }

    NvmCtxCallback( );

    return LORAMAC_COMMANDS_SUCCESS;
}

LoRaMacCommandStatus_t LoRaMacCommandsGetSizeSerializedCmds( size_t* size )
{
    if( size == NULL )
    {
        return LORAMAC_COMMANDS_ERROR_NPE;
    }
    *size = NvmCtx.SerializedCmdsSize;
    return LORAMAC_COMMANDS_SUCCESS;

}

LoRaMacCommandStatus_t LoRaMacCommandsSerializeCmds( size_t availableSize, size_t* effectiveSize,  uint8_t* buffer )
{
    if( ( buffer == NULL ) || ( effectiveSize == NULL ) )
    {
        return LORAMAC_COMMANDS_ERROR_NPE;
    }
    MacCommand_t* curElement;
    curElement = NvmCtx.MacCommandList.First;
    uint8_t itr = 0;

    // Loop through all elements
    while( curElement )
    {
        // If the next MAC command still fits into the buffer, add it.
        if( ( availableSize - itr ) >= ( CID_FIELD_SIZE + curElement->PayloadSize ) )
        {
            buffer[itr++] = curElement->CID;
            memcpy1( &buffer[itr], curElement->Payload, curElement->PayloadSize );
            itr = itr + curElement->PayloadSize;
        }
        else
        {
            break;
        }
        curElement = curElement->Next;
    }

    return LORAMAC_COMMANDS_SUCCESS;
}

LoRaMacCommandStatus_t LoRaMacCommandsStickyCmdsPending( bool* cmdsPending )
{
    if( cmdsPending == NULL )
    {
        return LORAMAC_COMMANDS_ERROR_NPE;
    }
    MacCommand_t* curElement;
    curElement = NvmCtx.MacCommandList.First;

    *cmdsPending = false;

    // Loop through all elements
    while( curElement )
    {
        if( curElement->IsSticky == true )
        {
            // Found one sticky MAC command
            *cmdsPending = true;
            return LORAMAC_COMMANDS_SUCCESS;
        }
        curElement = curElement->Next;
    }

    return LORAMAC_COMMANDS_SUCCESS;
}
