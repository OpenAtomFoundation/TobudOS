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

Description: LoRa MAC layer message serializer functionality implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis ( Semtech ), Gregory Cristian ( Semtech ),
            Daniel Jaeckle ( STACKFORCE ),  Johannes Bruder ( STACKFORCE )
*/
#include "LoRaMacSerializer.h"
#include "utilities.h"

LoRaMacSerializerStatus_t LoRaMacSerializerJoinRequest( LoRaMacMessageJoinRequest_t* macMsg )
{
    if( ( macMsg == 0 ) || ( macMsg->Buffer == 0 ) )
    {
        return LORAMAC_SERIALIZER_ERROR_NPE;
    }

    uint16_t bufItr = 0;

    // Check macMsg->BufSize
    if( macMsg->BufSize < LORAMAC_JOIN_REQ_MSG_SIZE )
    {
        return LORAMAC_SERIALIZER_ERROR_BUF_SIZE;
    }

    macMsg->Buffer[bufItr++] = macMsg->MHDR.Value;

    memcpyr( &macMsg->Buffer[bufItr], macMsg->JoinEUI, LORAMAC_JOIN_EUI_FIELD_SIZE );
    bufItr += LORAMAC_JOIN_EUI_FIELD_SIZE;

    memcpyr( &macMsg->Buffer[bufItr], macMsg->DevEUI, LORAMAC_DEV_EUI_FIELD_SIZE );
    bufItr += LORAMAC_DEV_EUI_FIELD_SIZE;

    macMsg->Buffer[bufItr++] = macMsg->DevNonce & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->DevNonce >> 8 ) & 0xFF;

    macMsg->Buffer[bufItr++] = macMsg->MIC & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->MIC >> 8 ) & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->MIC >> 16 ) & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->MIC >> 24 ) & 0xFF;

    macMsg->BufSize = bufItr;

    return LORAMAC_SERIALIZER_SUCCESS;
}

LoRaMacSerializerStatus_t LoRaMacSerializerReJoinType1( LoRaMacMessageReJoinType1_t* macMsg )
{
    if( ( macMsg == 0 ) || ( macMsg->Buffer == 0 ) )
    {
        return LORAMAC_SERIALIZER_ERROR_NPE;
    }

    uint16_t bufItr = 0;

    // Check macMsg->BufSize
    if( macMsg->BufSize < LORAMAC_RE_JOIN_1_MSG_SIZE )
    {
        return LORAMAC_SERIALIZER_ERROR_BUF_SIZE;
    }

    macMsg->Buffer[bufItr++] = macMsg->MHDR.Value;

    macMsg->Buffer[bufItr++] = macMsg->ReJoinType;

    memcpyr( &macMsg->Buffer[bufItr], macMsg->JoinEUI, LORAMAC_JOIN_EUI_FIELD_SIZE );
    bufItr += LORAMAC_JOIN_EUI_FIELD_SIZE;

    memcpyr( &macMsg->Buffer[bufItr], macMsg->DevEUI, LORAMAC_DEV_EUI_FIELD_SIZE );
    bufItr += LORAMAC_DEV_EUI_FIELD_SIZE;

    macMsg->Buffer[bufItr++] = macMsg->RJcount1 & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->RJcount1 >> 8 ) & 0xFF;

    return LORAMAC_SERIALIZER_SUCCESS;
}

LoRaMacSerializerStatus_t LoRaMacSerializerReJoinType0or2( LoRaMacMessageReJoinType0or2_t* macMsg )
{
    if( ( macMsg == 0 ) || ( macMsg->Buffer == 0 ) )
    {
        return LORAMAC_SERIALIZER_ERROR_NPE;
    }

    uint16_t bufItr = 0;

    // Check macMsg->BufSize
    if( macMsg->BufSize < LORAMAC_RE_JOIN_0_2_MSG_SIZE )
    {
        return LORAMAC_SERIALIZER_ERROR_BUF_SIZE;
    }

    macMsg->Buffer[bufItr++] = macMsg->MHDR.Value;

    macMsg->Buffer[bufItr++] = macMsg->ReJoinType;

    memcpyr( &macMsg->Buffer[bufItr], macMsg->NetID, LORAMAC_NET_ID_FIELD_SIZE );
    bufItr += LORAMAC_NET_ID_FIELD_SIZE;

    memcpyr( &macMsg->Buffer[bufItr], macMsg->DevEUI, LORAMAC_DEV_EUI_FIELD_SIZE );
    bufItr += LORAMAC_DEV_EUI_FIELD_SIZE;

    macMsg->Buffer[bufItr++] = macMsg->RJcount0 & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->RJcount0 >> 8 ) & 0xFF;

    return LORAMAC_SERIALIZER_SUCCESS;
}

LoRaMacSerializerStatus_t LoRaMacSerializerData( LoRaMacMessageData_t* macMsg )
{
    if( ( macMsg == 0 ) || ( macMsg->Buffer == 0 ) )
    {
        return LORAMAC_SERIALIZER_ERROR_NPE;
    }

    uint16_t bufItr = 0;

    // Check macMsg->BufSize
    uint16_t computedBufSize =   LORAMAC_MHDR_FIELD_SIZE
                               + LORAMAC_FHDR_DEV_ADD_FIELD_SIZE
                               + LORAMAC_FHDR_F_CTRL_FIELD_SIZE
                               + LORAMAC_FHDR_F_CNT_FIELD_SIZE;

    if( macMsg->FRMPayloadSize == 0 )
    {
        if( macMsg->BufSize < computedBufSize )
        {
            return LORAMAC_SERIALIZER_ERROR_BUF_SIZE;
        }
    }
    else
    {   //If FRMPayload >0, FPort field is present.
        if( macMsg->BufSize < computedBufSize + macMsg->FHDR.FCtrl.Bits.FOptsLen + macMsg->FRMPayloadSize + LORAMAC_F_PORT_FIELD_SIZE )
        {
            return LORAMAC_SERIALIZER_ERROR_BUF_SIZE;
        }
    }

    macMsg->Buffer[bufItr++] = macMsg->MHDR.Value;

    macMsg->Buffer[bufItr++] = ( macMsg->FHDR.DevAddr ) & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->FHDR.DevAddr >> 8 ) & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->FHDR.DevAddr >> 16 ) & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->FHDR.DevAddr >> 24 ) & 0xFF;

    macMsg->Buffer[bufItr++] = macMsg->FHDR.FCtrl.Value;

    macMsg->Buffer[bufItr++] = macMsg->FHDR.FCnt & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->FHDR.FCnt >> 8 ) & 0xFF;

    memcpy1( &macMsg->Buffer[bufItr], macMsg->FHDR.FOpts, macMsg->FHDR.FCtrl.Bits.FOptsLen );
    bufItr = bufItr + macMsg->FHDR.FCtrl.Bits.FOptsLen;

    if( macMsg->FRMPayloadSize > 0 )
    {
        macMsg->Buffer[bufItr++] = macMsg->FPort;
    }

    memcpy1( &macMsg->Buffer[bufItr], macMsg->FRMPayload, macMsg->FRMPayloadSize );
    bufItr = bufItr + macMsg->FRMPayloadSize;

    macMsg->Buffer[bufItr++] = macMsg->MIC & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->MIC >> 8 ) & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->MIC >> 16 ) & 0xFF;
    macMsg->Buffer[bufItr++] = ( macMsg->MIC >> 24 ) & 0xFF;

    macMsg->BufSize = bufItr;

    return LORAMAC_SERIALIZER_SUCCESS;
}
