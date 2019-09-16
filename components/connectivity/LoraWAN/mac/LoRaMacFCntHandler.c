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

Description: LoRa MAC layer frame counter handling implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis ( Semtech ), Daniel Jaeckle ( STACKFORCE ), Johannes Bruder ( STACKFORCE )
*/

#include <string.h>
#include "utilities.h"
#include "LoRaMacFCntHandler.h"

/*
 * Initial value of the frame counters
 */
#define FCNT_DOWN_INITAL_VALUE          0xFFFFFFFF


/*!
 * LoRaWAN Frame counter list.
 */
typedef struct sFCntList
{
    /*
     * Uplink frame counter which is incremented with each uplink.
     */
    uint32_t FCntUp;
    /*
     * Network downlink frame counter which is incremented with each downlink on FPort 0
     * or when the FPort field is missing.
     */
    uint32_t NFCntDown;
    /*
     * Application downlink frame counter which is incremented with each downlink
     * on a port different than 0.
     */
    uint32_t AFCntDown;
    /*
     * In case if the device is connected to a LoRaWAN 1.0 Server,
     * this counter is used for every kind of downlink frame.
     */
    uint32_t FCntDown;
    /*!
     * Multicast downlink counter for index 0
     */
    uint32_t McFCntDown0;
    /*!
     * Multicast downlink counter for index 1
     */
    uint32_t McFCntDown1;
    /*!
     * Multicast downlink counter for index 2
     */
    uint32_t McFCntDown2;
    /*!
     * Multicast downlink counter for index 3
     */
    uint32_t McFCntDown3;
}FCntList_t;


/*
 * LoRaMac Commands Context structure
 */
typedef struct sLoRaMacFCntHandlerNvmCtx
{
    /*
     * Frame counter list
     */
    FCntList_t FCntList;
} LoRaMacFCntHandlerNvmCtx_t;

/*
 * Non-volatile module context.
 */
static LoRaMacFCntHandlerNvmCtx_t FCntHandlerNvmCtx;

/*
 * Callback function to notify the upper layer about context change
 */
static EventNvmCtxChanged FCntHandlerNvmCtxChanged;

/*
 * \brief Wrapper function for the NvmCtx
 */
static void NvmCtxChanged( void )
{
    if( FCntHandlerNvmCtxChanged != NULL )
    {
        FCntHandlerNvmCtxChanged( );
    }
}


LoRaMacFCntHandlerStatus_t LoRaMacFCntHandlerInit( EventNvmCtxChanged fCntHandlerNvmCtxChanged )
{
    // Initialize with default
    LoRaMacResetFCnts( );

    // Assign callback
    FCntHandlerNvmCtxChanged = fCntHandlerNvmCtxChanged;

    return LORAMAC_FCNT_HANDLER_SUCCESS;
}

LoRaMacFCntHandlerStatus_t LoRaMacFCntHandlerRestoreNvmCtx( void* fCntHandlerNvmCtx )
{
    // Restore module context
    if( fCntHandlerNvmCtx != NULL )
    {
        memcpy1( ( uint8_t* ) &FCntHandlerNvmCtx, ( uint8_t* ) fCntHandlerNvmCtx, sizeof( FCntHandlerNvmCtx ) );
        return LORAMAC_FCNT_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_FCNT_HANDLER_ERROR_NPE;
    }
}

void* LoRaMacFCntHandlerGetNvmCtx( size_t* fCntHandlerNvmCtxSize )
{
    *fCntHandlerNvmCtxSize = sizeof( FCntHandlerNvmCtx );
    return &FCntHandlerNvmCtx;
}

LoRaMacFCntHandlerStatus_t LoRaMacGetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                               uint16_t maxFCntGap, FCntIdentifier_t* fCntID, uint32_t* currentDown )
{
    uint32_t previousDown = 0;
    int32_t fCntDiff = 0;

    if( ( macMsg == NULL ) || ( fCntID == NULL ) ||
        ( currentDown == NULL ) )
    {
        return LORAMAC_FCNT_HANDLER_ERROR_NPE;
    }

    // Determine the frame counter identifier and choose counter from FCntList
    switch( addrID )
    {
        case UNICAST_DEV_ADDR:
            if( lrWanVersion.Fields.Minor == 1 )
            {
                if( ( fType == FRAME_TYPE_A ) || ( fType == FRAME_TYPE_D ) )
                {
                    *fCntID = A_FCNT_DOWN;
                    previousDown = FCntHandlerNvmCtx.FCntList.AFCntDown;
                }
                else
                {
                    *fCntID = N_FCNT_DOWN;
                    previousDown = FCntHandlerNvmCtx.FCntList.NFCntDown;
                }
            }
            else
            { // For LoRaWAN 1.0.X
                *fCntID = FCNT_DOWN;
                previousDown = FCntHandlerNvmCtx.FCntList.FCntDown;
            }
            break;
        case MULTICAST_0_ADDR:
            *fCntID = MC_FCNT_DOWN_0;
            previousDown = FCntHandlerNvmCtx.FCntList.McFCntDown0;
            break;
        case MULTICAST_1_ADDR:
            *fCntID = MC_FCNT_DOWN_1;
            previousDown = FCntHandlerNvmCtx.FCntList.McFCntDown1;
            break;
        case MULTICAST_2_ADDR:
            *fCntID = MC_FCNT_DOWN_2;
            previousDown = FCntHandlerNvmCtx.FCntList.McFCntDown3;
            break;
        case MULTICAST_3_ADDR:
            *fCntID = MC_FCNT_DOWN_3;
            previousDown = FCntHandlerNvmCtx.FCntList.McFCntDown3;
            break;
        default:
            return LORAMAC_FCNT_HANDLER_ERROR;
    }

    // For LoRaWAN 1.0.X only, allow downlink frames of 0
    if( previousDown == FCNT_DOWN_INITAL_VALUE )
    {
         *currentDown = macMsg->FHDR.FCnt;
    }
    else
    {
        // Add difference, consider roll-over
        fCntDiff = ( int32_t )macMsg->FHDR.FCnt - ( int32_t )( previousDown & 0x0000FFFF );

        if( fCntDiff > 0 )
        {  // Positive difference
            *currentDown = previousDown + fCntDiff;
        }
        else if( fCntDiff == 0 )
        {  // Duplicate FCnt value, keep the current value.
            *currentDown = previousDown;
            return LORAMAC_FCNT_HANDLER_CHECK_FAIL;
        }
        else
        {  // Negative difference, assume a roll-over of one uint16_t
            *currentDown = ( previousDown & 0xFFFF0000 ) + 0x10000 + macMsg->FHDR.FCnt;
        }
    }


    // For LoRaWAN 1.0.X only, check maxFCntGap
    if( lrWanVersion.Fields.Minor == 0 )
    {
        if( ( ( int32_t )*currentDown - ( int32_t )previousDown ) >= maxFCntGap )
        {
            return LORAMAC_FCNT_HANDLER_MAX_GAP_FAIL;
        }
    }

    return LORAMAC_FCNT_HANDLER_SUCCESS;
}

LoRaMacFCntHandlerStatus_t LoRaMacSetFCntDown( FCntIdentifier_t fCntID, uint32_t currentDown )
{
    switch( fCntID )
    {
        case FCNT_UP:
            return LORAMAC_FCNT_HANDLER_ERROR_INVALID_FCNT_ID;
        case N_FCNT_DOWN:
            FCntHandlerNvmCtx.FCntList.NFCntDown = currentDown;
            break;
        case A_FCNT_DOWN:
            FCntHandlerNvmCtx.FCntList.AFCntDown = currentDown;
            break;
        case FCNT_DOWN:
            FCntHandlerNvmCtx.FCntList.FCntDown = currentDown;
            break;
        case MC_FCNT_DOWN_0:
            FCntHandlerNvmCtx.FCntList.McFCntDown0 = currentDown;
            break;
        case MC_FCNT_DOWN_1:
            FCntHandlerNvmCtx.FCntList.McFCntDown1 = currentDown;
            break;
        case MC_FCNT_DOWN_2:
            FCntHandlerNvmCtx.FCntList.McFCntDown2 = currentDown;
            break;
        case MC_FCNT_DOWN_3:
            FCntHandlerNvmCtx.FCntList.McFCntDown3 = currentDown;
            break;
        default:
            return LORAMAC_FCNT_HANDLER_ERROR;
    }

    NvmCtxChanged( );

    return LORAMAC_FCNT_HANDLER_SUCCESS;
}

LoRaMacFCntHandlerStatus_t LoRaMacGetFCntUp( uint32_t* currentUp )
{
    if( currentUp == NULL )
    {
        return LORAMAC_FCNT_HANDLER_ERROR_NPE;
    }

    *currentUp = FCntHandlerNvmCtx.FCntList.FCntUp + 1;

    return LORAMAC_FCNT_HANDLER_SUCCESS;
}

LoRaMacFCntHandlerStatus_t LoRaMacSetFCntUp( uint32_t currentUp )
{
    FCntHandlerNvmCtx.FCntList.FCntUp = currentUp;

    NvmCtxChanged( );

    return LORAMAC_FCNT_HANDLER_SUCCESS;
}

LoRaMacFCntHandlerStatus_t LoRaMacResetFCnts( void )
{

    FCntHandlerNvmCtx.FCntList.FCntUp = 0;
    FCntHandlerNvmCtx.FCntList.NFCntDown = FCNT_DOWN_INITAL_VALUE;
    FCntHandlerNvmCtx.FCntList.AFCntDown = FCNT_DOWN_INITAL_VALUE;
    FCntHandlerNvmCtx.FCntList.FCntDown = FCNT_DOWN_INITAL_VALUE;

    FCntHandlerNvmCtx.FCntList.McFCntDown0 = FCNT_DOWN_INITAL_VALUE;
    FCntHandlerNvmCtx.FCntList.McFCntDown1 = FCNT_DOWN_INITAL_VALUE;
    FCntHandlerNvmCtx.FCntList.McFCntDown2 = FCNT_DOWN_INITAL_VALUE;
    FCntHandlerNvmCtx.FCntList.McFCntDown3 = FCNT_DOWN_INITAL_VALUE;

    NvmCtxChanged( );

    return LORAMAC_FCNT_HANDLER_SUCCESS;
}

LoRaMacFCntHandlerStatus_t LoRaMacFCntHandlerSetMulticastReference( MulticastCtx_t* multicastList )
{
    if( multicastList == NULL )
    {
        return LORAMAC_FCNT_HANDLER_ERROR_NPE;
    }

    multicastList[0].DownLinkCounter = &FCntHandlerNvmCtx.FCntList.McFCntDown0;
    multicastList[1].DownLinkCounter = &FCntHandlerNvmCtx.FCntList.McFCntDown1;
    multicastList[2].DownLinkCounter = &FCntHandlerNvmCtx.FCntList.McFCntDown2;
    multicastList[3].DownLinkCounter = &FCntHandlerNvmCtx.FCntList.McFCntDown3;

    return LORAMAC_FCNT_HANDLER_SUCCESS;
}
