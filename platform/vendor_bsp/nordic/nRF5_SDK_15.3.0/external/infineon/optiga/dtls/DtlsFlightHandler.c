/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file DtlsFlightHandler.c
*
* \brief   This file implements the DTLS Flight handling details.
*
* \addtogroup  grMutualAuth
* @{
*/

#include "optiga/dtls/DtlsFlighthandler.h"
#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/// @cond hidden
#define LSBREM8(x) (x & 0x07)
#define LSTBYTE(x) ( ((LSBREM8(x))) ? 1 : 0)
#define DIVBY8(x) (x >> 3)
#define DIVBY4(x) (x >> 2)
#define UPDATEGRANLRTY 32

#define MSG_BITMAP8SET(dwNumBits, pbByte2Set, dwStartBit) (*pbByte2Set |= (uint8_t)((uint8_t)((1 << dwNumBits)-1) << dwStartBit))

#define MSG_BITMAP32SET(dwNumBits, pbWord2Set, dwStartBit) (*pbWord2Set |= ((uint32_t)(0-1)) << dwStartBit)

#define MSG_ID(X)                       (X & 0xFF)
#define FLIGHT_IDLIMITCHK(LL, X, UL)    (((X)>=(LL) && (X)<=(UL)) ? OCP_FL_OK : OCP_FL_ERROR)

///Maximum message payload
#define MAX_MSG_PAYLOAD                 1536            //1558-5(APDU)-12(Msg header)-5(Session ID)

///Offset for handshake message
#define HANDSHAKE_HEADER_OFFSET         0x00
///Offset for Message ID in handshake header
#define OFFSET_HS_MSG_TYPE              (HANDSHAKE_HEADER_OFFSET + 0)
///Offset for Message length in handshake header
#define OFFSET_HS_MSG_LENGTH            (OFFSET_HS_MSG_TYPE + 1)
///Offset for Message Sequence number in handshake header
#define OFFSET_HS_MSG_SEQNUM            (OFFSET_HS_MSG_LENGTH + 3)
///Offset for Fragment offset in handshake header
#define OFFSET_HS_MSG_FRAGOFFSET        (OFFSET_HS_MSG_SEQNUM + 2)
///Offset for Fragment length in handshake header
#define OFFSET_HS_MSG_FRAGLEN           (OFFSET_HS_MSG_FRAGOFFSET + 3)
///Offset for message data
#define OFFSET_HS_MSG_DATA              (OFFSET_HS_MSG_FRAGLEN  + 3) //12
///Message header length
#define LENGTH_HS_MSG_HEADER            (OFFSET_HS_MSG_DATA)

///Value of Message type in handshake header
#define HS_MESSAGE_TYPE(X)              (*((X)+(OFFSET_HS_MSG_TYPE)))
///Value of Message length in handshake header
#define HS_MESSAGE_LENGTH(X)            (Utility_GetUint24((X)+(OFFSET_HS_MSG_LENGTH)))
///Value of Message sequence number in handshake header
#define HS_MESSAGE_SEQNUM(X)            (Utility_GetUint16((X)+(OFFSET_HS_MSG_SEQNUM)))
///Value of Message fragment offset in handshake header
#define HS_MESSAGE_FRAGOFFSET(X)        (Utility_GetUint24((X)+(OFFSET_HS_MSG_FRAGOFFSET)))
///Value of Message fragment length in handshake header
#define HS_MESSAGE_FRAGLEN(X)           (Utility_GetUint24((X)+(OFFSET_HS_MSG_FRAGLEN)))

#define UPDATE_RX_MSGSEQNUM(X,Y)        (X=Y)
#define OCP_FLIGHT_TABLE_MAX_SIZE       3
#define UPDATE_MSGSTATE(X,Y)            (X=Y)
/// @endcond

///Maximum number of retransmission of a flight in a session during the handshake protocol
#define OCP_MSGRX_MAX_COUNT             6

/**
 * \brief Initializes the pointer to bit map representing message status.<br>
 */
_STATIC_H int32_t DtlsHS_MsgCompleteInit(uint32_t PdwMsgLen, uint8_t** PppbMapPtr);

/**
 * \brief Sets the number of bits in bit map equal to the number of bytes received in message/ fragment.<br>
 */
_STATIC_H int32_t DtlsHS_MsgUptBitMsk(uint32_t PdwOffset, uint32_t PdwFragLen, uint8_t* PprgbMapPtr, uint32_t PdwMsgLen);

/**
 * \brief Checks if all the bits in the bitmap are set for the message completion.<br>
 */
_STATIC_H int32_t DtlsHS_MsgCompleteCheck(uint8_t* PprgbMapPtr, uint32_t PdwMsgLen);

/**
 * \brief Clears all the bits in the bitmap.<br>
 */
_STATIC_H int32_t DtlsHS_MsgClearBitMap(uint8_t* PprgbMapPtr, uint32_t PdwMsgLen);

/**
 * \brief Searches the look-up table and returns the flight descriptor.<br>
 */
_STATIC_H int32_t DtlsHS_GetFlightDescriptor(uint8_t PbFlightID, uint16_t* PpwFlightID);

/**
 * \brief Searches the look-up table and returns the flight handler.<br>
 */
_STATIC_H int32_t DtlsHS_GetFlightHandler(uint8_t PeFlightID, fFlightHandler * PfHndler);

/**
 * \brief Inserts the message node to flight head node.<br>
 */
_STATIC_H void DtlsHS_InsertMsgNode(sMsgInfo_d** PppsNodeHead, sMsgInfo_d* PpsNewNode);

/**
 * \brief Adds a message node to the end of the message list.<br>
 */
_STATIC_H void DtlsHS_AddMsgNode(sMsgInfo_d** PppsNodeHead, sMsgInfo_d* PpsNewNode);

/**
 * \brief Initialises message node to respective parameters.<br>
 */
_STATIC_H int32_t DtlsHS_SInit_MessageNode(sMsgInfo_d* PpsMsgNode, const sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Checks if given optional message is in the list.<br>
 */
_STATIC_H int32_t DtlsHS_CheckOptMsg(uint8_t PbMsgID, const uint8_t* PpbOptMsgList);

/**
 * \brief Checks if the given message can be sent to the server or not.<br>
 */
_STATIC_H int32_t DtlsHS_Flight5_CheckOptMsg(uint8_t PbMsgID, const uint8_t* PpbOptMsgList, const sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Forms the change cipher spec message.<br>
 */
_STATIC_H int32_t DtlsHS_SInit_ChangeCipherSpec(sMsgInfo_d* PpsMsgNode);

/**
 * \brief Returns the sequence number of the last message in a flight.<br>
 */
_STATIC_H void DtlsHS_FlightGetLastMsgSeqNum(const sMsgInfo_d *PpsMessageList, uint16_t *PwLastMsgSeqNum);

/**
 * \brief Initializes the message node with respective parameters and updates the bit map.<br>
 */
_STATIC_H int32_t DtlsHS_RInit_MessageNode(sMsgInfo_d* PpsMsgNode, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Returns the total number of messages in a flight.<br>
 */
_STATIC_H void DtlsHS_GetTotalMsgCount(const uint16_t *PwMsgDescList, uint8_t *PbMsgCount);

/**
 * \brief Returns the number of optional messages in a flight.<br>
 */
_STATIC_H void DtlsHS_GetOptMsgCount(const uint16_t *PwMsgDescList, uint8_t *PbOptMsgCount);

/**
 * \brief Returns the message descriptor from the list of messages.<br>
 */
_STATIC_H void DtlsHS_GetMsgDesc(const uint16_t *PwMsgDescList, uint8_t PbMsgType, uint16_t *PwMsgDesc);

/**
 * \brief Adds a optional message to the list of optional messages.<br>
 */
_STATIC_H void DtlsHS_Update_OptList(sMsgLyr_d* PpsMessageLayer, uint8_t PbMsgType, uint8_t PeFlightID);

/**
 * \brief Checks if all the messages of a flight are completely received.<br>
 */
_STATIC_H int32_t DtlsHS_CheckFlight_Compl(sMsgInfo_d *PpsMessageList, uint8_t PeFlightID, const sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Checks if a message is received more than or equal to six times.<br>
 */
_STATIC_H int32_t DtlsHS_MessageLimitCheck(sMsgInfo_d *PpsMsgNode);

/**
 * \brief Buffers the received message/ fragment.<br>
 */
_STATIC_H int32_t DtlsHS_FlightMsgChkAndBuffer(sMsgInfo_d *PpsMessageList, uint8_t bMsgID, const sbBlob_d* PpsMsgIn, sMsgLyr_d* PpsMessageLayer, uint8_t PeFlightID);

/**
 * \brief Checks if the message type of the received message/ fragment belongs to the flight.<br>
 */
_STATIC_H int32_t DtlsHS_Flight_LimitChk(uint8_t PbFlightID, uint8_t PbMsgID);

/**
 * \brief Sends the message to be processed to Security Chip.<br>
 */
_STATIC_H int32_t DtlsHS_SendFlightToOptiga(sMsgInfo_d *PpsMessageList, const sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Resets the flight 2 node.<br>
 */
_STATIC_H void DtlsHS_ResetFlight2MsgNode(const sFlightStats_d* PpsThisFlight);

/**
 * \brief Checks if message sequence number of received message/ fragment of flight4 is correct.<br>
 */
_STATIC_H int32_t DtlsHS_Flight4CheckMsgSeqNum(const sMsgLyr_d* PpsMessageLayer, uint8_t PbRxMsgID, uint16_t PwRxMsgSeqNum);

/**
 * \brief Checks if message sequence number and length of received message/ fragment of flight4 is the same as the buffered one.<br>
 */
_STATIC_H int32_t DtlsHS_Flight4CheckBuffMsg(uint8_t PbMsgType, uint16_t PwMsgSeqNum, uint32_t PdwMsgLen, sMsgInfo_d *PpsMsgList);

/**
 * \brief Checks for flight4 completion.<br>
 */
_STATIC_H int32_t DtlsHS_Flight4ReRxCompl(sMsgInfo_d *PpsMsgList);

/**
 * \brief Clears the messages of a flight and resets the bit map.<br>
 */
_STATIC_H void DtlsHS_Flight4ClearMsgsInList(sMsgInfo_d *PpsMsgList);

/**
 * \brief Updates bit map and sets the message state.<br>
 */
_STATIC_H int32_t DtlsHS_Flight4UpdateMsgStat(uint8_t PbMsgID, sMsgInfo_d *PpsMsgList, const sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Checks whether flight six is received.<br>
 */
_STATIC_H int32_t DtlsHS_CheckFlight6_Compl(sMsgInfo_d *PpsMessageList);

/**
 * \brief Checks Message Sequence number of flight six messages.<br>
 */
_STATIC_H int32_t DtlsHS_Flight6CheckMsgSeqNum(const sMsgLyr_d* PpsMessageLayer, uint8_t PbMsgID, uint16_t PwMsgSeqNum);

/**
 * \brief Frees a node and all the pointers in it .<br>
 */
_STATIC_H void DtlsHS_FreeMsgNode(sMsgInfo_d *PpsMsgNode);

/**
 * Initializes the pointer to bit map representing message status.<br>
 *
 * \param[in]   PdwMsgLen       Total length of the message received.
 * \param[out]  PppbMapPtr      Pointer container to map pointer.
 *
 * \retval      #OCP_FL_OK              Successful execution
 * \retval      #OCP_FL_MSG_ERROR       Failure in execution
\if ENABLE_NULL_CHECKS
 * \retval      #OCP_FL_NULL_PARAM      Null parameter
\endif
 */
_STATIC_H int32_t DtlsHS_MsgCompleteInit(uint32_t PdwMsgLen, uint8_t** PppbMapPtr)
{
    int32_t i4Status = (int32_t)OCP_FL_MSG_ERROR;
    uint32_t dwMapSize;
    uint8_t* pbMapPtr = NULL;
    uint8_t  bLbyteMsk;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if(NULL == PppbMapPtr)
        {
            i4Status = (int32_t)OCP_FL_NULL_PARAM;
            break;
        }
#endif
        dwMapSize = DIVBY8(PdwMsgLen) + LSTBYTE(PdwMsgLen);
        if(*PppbMapPtr == NULL)
        {
            pbMapPtr = (uint8_t*)OCP_CALLOC(dwMapSize, sizeof(uint8_t));
            if(pbMapPtr == NULL)
            {
                i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                break;
            }
            *PppbMapPtr = pbMapPtr;

            // Set remaining bits in last byte
            bLbyteMsk = (uint8_t)((uint8_t)( (1 << (8 - (LSBREM8(PdwMsgLen) ))) - (LSTBYTE(PdwMsgLen)) ) << (LSBREM8(PdwMsgLen)));

            *(pbMapPtr+(dwMapSize-1)) |= bLbyteMsk;
        }

        i4Status = (int32_t)OCP_FL_OK;

    }while(0);

    return i4Status;
}

/**
 * Sets the number of bits in bit map equal to the number of bytes received in message/ fragment.<br>
 *
 * \param[in]       PdwOffset       Offset of the message/ fragment received.
 * \param[in]       PdwFragLen      Length of the message/ fragment received.
 * \param[in,out]   PprgbMapPtr     Pointer to bit map.
 * \param[in]       PdwMsgLen       Length of the message/ fragment received.
 *
 * \retval      #OCP_FL_OK              Successful execution
 * \retval      #OCP_FL_MSG_ERROR       Failure in execution
\if ENABLE_NULL_CHECKS
 * \retval      #OCP_FL_NULL_PARAM      Null parameter
\endif
 */
_STATIC_H int32_t DtlsHS_MsgUptBitMsk(uint32_t PdwOffset, uint32_t PdwFragLen, uint8_t* PprgbMapPtr, uint32_t PdwMsgLen)
{
    int32_t i4Status = (int32_t)OCP_FL_MSG_ERROR;
    uint32_t dwStartByte, dwStartBit, dwNumofBit2Set, dwLoopCount, dwCount;
    uint8_t* prgbByte2Set = NULL;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if(NULL == PprgbMapPtr)
        {
            i4Status = (int32_t)OCP_FL_NULL_PARAM;
            break;
        }
#endif
        dwStartByte = DIVBY8(PdwOffset);
        dwStartBit  = LSBREM8(PdwOffset);
        dwNumofBit2Set = PdwFragLen;
        prgbByte2Set = PprgbMapPtr + dwStartByte;

        if(dwStartBit != 0)
        {
            if(dwNumofBit2Set < (8-dwStartBit))
            {
                MSG_BITMAP8SET(dwNumofBit2Set, prgbByte2Set, dwStartBit);
                dwNumofBit2Set -= dwNumofBit2Set;
            }
            else
            {
                MSG_BITMAP8SET((8-dwStartBit), prgbByte2Set, dwStartBit);
                dwNumofBit2Set -= (8-dwStartBit);
            }
            prgbByte2Set++;
        }

        if(prgbByte2Set < ( PprgbMapPtr+(DIVBY8(PdwMsgLen)+LSTBYTE(PdwMsgLen))) )
        {
            dwLoopCount = dwNumofBit2Set/UPDATEGRANLRTY;

            for(dwCount=0; dwCount < dwLoopCount; dwCount++)
            {
                //lint --e{826} suppress "Implicit type casting to 32 bit pointer for 32 bit granularity check"
                MSG_BITMAP32SET(UPDATEGRANLRTY, (uint32_t*)prgbByte2Set, 0);
                prgbByte2Set += (UPDATEGRANLRTY >> 3);
                dwNumofBit2Set -= UPDATEGRANLRTY;
            }
            if(dwNumofBit2Set >= 0x08)
            {
                dwLoopCount = DIVBY8(dwNumofBit2Set);
                for(dwCount=0; dwCount < dwLoopCount; dwCount++)
                {
                    MSG_BITMAP8SET(8, prgbByte2Set, 0);
                    prgbByte2Set++;
                    dwNumofBit2Set -= 8;
                }
            }
            if(dwNumofBit2Set)
            {
                MSG_BITMAP8SET(dwNumofBit2Set, prgbByte2Set, 0);
                dwNumofBit2Set -= dwNumofBit2Set;
            }
        }
        i4Status = (int32_t)OCP_FL_OK;
    }while(0);

    return i4Status;

}

/**
 * Checks if all the bits in the bitmap is set for the message completion.<br>
 *
 * \param[in]       PprgbMapPtr     Pointer to bit map.
 * \param[in]       PdwMsgLen       Total Length of the message received.
 *
 * \retval      #OCP_FL_OK              Successful execution
 * \retval      #OCP_FL_MSG_ERROR       Failure in execution
\if ENABLE_NULL_CHECKS
 * \retval      #OCP_FL_NULL_PARAM      Null parameter
\endif
 */

_STATIC_H int32_t DtlsHS_MsgCompleteCheck(uint8_t* PprgbMapPtr, uint32_t PdwMsgLen)
{
    uint32_t dwLoopCount;
    int32_t i4Status = (int32_t)OCP_FL_OK;
    uint32_t* pdwMapptr = NULL;
    uint8_t* pbMapptr = NULL;

    dwLoopCount = DIVBY4(DIVBY8(PdwMsgLen));

/// @cond hidden
#define DWVAL 0xFFFFFFFF
/// @endcond
    do
    {
#ifdef ENABLE_NULL_CHECKS
        if(NULL == PprgbMapPtr)
        {
            i4Status = (int32_t)OCP_FL_NULL_PARAM;
            break;
        }
#endif
        //lint --e{826} suppress "Implicit type casting to 32 bit pointer for 32 bit granularity check"
        pdwMapptr = (uint32_t*)PprgbMapPtr;

        while(dwLoopCount--)
        {
            if(*pdwMapptr++ < DWVAL )
            {
                i4Status = (int32_t)OCP_FL_MSG_ERROR;
                break;
            }
            PdwMsgLen -= UPDATEGRANLRTY;
        }

        if(i4Status != (int32_t)OCP_FL_MSG_ERROR)
        {
            dwLoopCount = DIVBY8(PdwMsgLen)+LSTBYTE(PdwMsgLen);
            pbMapptr = (uint8_t*)pdwMapptr;
            while(dwLoopCount--)
            {
                if(*pbMapptr++ < ((1<<8)-1))
                {
                    i4Status = (int32_t)OCP_FL_MSG_ERROR;
                    break;
                }
            }
        }
    }while(0);
/// @cond hidden
#undef DWVAL
/// @endcond
    return i4Status;
}

/**
 * Clears all the bits in the bitmap.<br>
 *
 * \param[in,out]   PprgbMapPtr     Pointer to bit map.
 * \param[in]       PdwMsgLen       Total length of the message received.
 *
 * \retval      #OCP_FL_OK              Successful execution
 * \retval      #OCP_FL_MSG_ERROR       Failure in execution
\if ENABLE_NULL_CHECKS
 * \retval      #OCP_FL_NULL_PARAM      Null parameter
\endif
 */
_STATIC_H int32_t DtlsHS_MsgClearBitMap(uint8_t* PprgbMapPtr, uint32_t PdwMsgLen)
{
    uint32_t dwLoopCount, dwMapSize;
    int32_t i4Status = (int32_t)OCP_FL_MSG_ERROR;
    uint32_t* pdwMapptr = NULL;
    uint8_t* pbMapptr = NULL;
    uint8_t  bLbyteMsk;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if(NULL == PprgbMapPtr)
        {
            i4Status = (int32_t)OCP_FL_NULL_PARAM;
            break;
        }
#endif
        //lint --e{826} suppress "Implicit type casting to 32 bit pointer for 32 bit granularity check"
        pdwMapptr = (uint32_t*)PprgbMapPtr;
        dwMapSize = DIVBY8(PdwMsgLen) + LSTBYTE(PdwMsgLen);
        dwLoopCount = DIVBY4(DIVBY8(PdwMsgLen));

        while(dwLoopCount--)
        {
            *pdwMapptr++ = 0x00;
            PdwMsgLen -= UPDATEGRANLRTY;
        }

        dwLoopCount = DIVBY8(PdwMsgLen)+LSTBYTE(PdwMsgLen);
        pbMapptr = (uint8_t*)pdwMapptr;

        while(dwLoopCount--)
        {
            *pbMapptr++ = 0x00;
        }

        bLbyteMsk = (uint8_t)((uint8_t)((1 << (8 - (LSBREM8(PdwMsgLen) ))) - (LSTBYTE(PdwMsgLen)) ) << (LSBREM8(PdwMsgLen)));
        *(PprgbMapPtr+(dwMapSize-1)) |= bLbyteMsk;

        i4Status = (int32_t)OCP_FL_OK;

    }while(0);

    return i4Status;
}

/**
 * Searches the look-up table and returns the flight descriptor.<br>
 *
 * \param[in]       PbFlightID          Flight number
 * \param[in,out]   PpwFlightID         Container to flight descriptor
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
_STATIC_H int32_t DtlsHS_GetFlightDescriptor(uint8_t PbFlightID, uint16_t* PpwFlightID)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    uint8_t bIndex = 0;

    do
    {
        for(bIndex = 0; bIndex < OCP_FLIGHT_TABLE_MAX_SIZE; bIndex++)
        {
            if(((rgsSFlightInfo[bIndex].wFlightDesc) & 0xFF) == (PbFlightID))
            {
                *PpwFlightID = rgsSFlightInfo[bIndex].wFlightDesc;
                i4Status = (int32_t)OCP_FL_OK;
                break;
            }
        }
        if((int32_t)OCP_FL_ERROR == i4Status)
        {
            for(bIndex = 0; bIndex < OCP_FLIGHT_TABLE_MAX_SIZE; bIndex++)
            {
                if((rgsRFlightInfo[bIndex].wFlightDesc & 0xFF) == (PbFlightID))
                {
                    *PpwFlightID = rgsRFlightInfo[bIndex].wFlightDesc;
                    i4Status = (int32_t)OCP_FL_OK;
                    break;
                }
            }
        }
    }while(0);
    return i4Status;
}

/**
 * Searches the look-up table and returns the flight handler.<br>
 *
 * \param[in]      PeFlightID           Flight number
 * \param[in,out]  PfHndler             Container to flight handler
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
_STATIC_H int32_t DtlsHS_GetFlightHandler(uint8_t PeFlightID, fFlightHandler * PfHndler)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    uint8_t bIndex;

    do
    {
        for(bIndex = 0; bIndex < OCP_FLIGHT_TABLE_MAX_SIZE; bIndex++)
        {
            if(((rgsSFlightInfo[bIndex].wFlightDesc) & 0xFF) == (uint8_t)PeFlightID)
            {
                *PfHndler = (rgsSFlightInfo[bIndex].pFlightHndlr);
                i4Status = (int32_t)OCP_FL_OK;
                break;
            }
        }
        if((int32_t)OCP_FL_ERROR == i4Status)
        {
            for(bIndex = 0; bIndex < OCP_FLIGHT_TABLE_MAX_SIZE; bIndex++)
            {
                if(((rgsRFlightInfo[bIndex].wFlightDesc) & 0xFF) == (uint8_t)PeFlightID)
                {
                    *PfHndler = (rgsRFlightInfo[bIndex].pFlightHndlr);
                    i4Status = (int32_t)OCP_FL_OK;
                    break;
                }
            }
        }
    }while(0);

    return i4Status;
}

/**
 * Inserts the message node to flight head node .<br>
 *
 * \param[in,out]   PppsNodeHead        Pointer container holding head node pointer.
 * \param[in]       PpsNewNode          Pointer to the message node to be inserted.
 *
 */
_STATIC_H void DtlsHS_InsertMsgNode(sMsgInfo_d** PppsNodeHead, sMsgInfo_d* PpsNewNode)
{
    sMsgInfo_d* pTravA = *PppsNodeHead;
    sMsgInfo_d* pTravB = NULL;

    do
    {
        if(NULL == pTravA)
        {
            *PppsNodeHead = PpsNewNode;
        }
        else
        {
            do
            {
                pTravB = pTravA->psNext;
                if (PpsNewNode->bMsgType < pTravA->bMsgType)
                {
                    PpsNewNode->psNext = pTravA;
                    *PppsNodeHead = PpsNewNode;
                    break;
                }
                else if (NULL != pTravB)
                {
                    if((PpsNewNode->bMsgType > pTravA->bMsgType) && (PpsNewNode->bMsgType < pTravB->bMsgType))
                    {
                        pTravA->psNext = PpsNewNode;
                        PpsNewNode->psNext = pTravB;
                        break;
                    }
                }
                else
                {
                    if(PpsNewNode->bMsgType > pTravA->bMsgType)
                    {
                        pTravA->psNext = PpsNewNode;
                        PpsNewNode->psNext = NULL;
                    }
                    break;
                }
                pTravA = pTravA->psNext;
            }while(NULL != pTravA);
        }
    }while(0);
}

/**
 * Adds a message node to the end of the message list.<br>
 *
 * \param[in,out]   PppsNodeHead        Pointer container holding head node pointer.
 * \param[in]       PpsNewNode          Pointer to the message node to be added.
 *
 */
_STATIC_H void DtlsHS_AddMsgNode(sMsgInfo_d** PppsNodeHead, sMsgInfo_d* PpsNewNode)
{
    sMsgInfo_d* psTrav = *PppsNodeHead;

    do
    {
        if(NULL == psTrav)
        {
            *PppsNodeHead = PpsNewNode;
        }
        else
        {
            while(NULL != psTrav->psNext)
            {
                psTrav = psTrav->psNext;
            }
            psTrav->psNext = PpsNewNode;
        }

    }while(0);
}
/**
 * Initialises message node to respective parameters.<br>
 *  -   Calls message layer to get the message.<br>
 *  -   Checks the header of the message received from message layer.<br>
 *  -   Updates the bit map.<br>
 *
 * \param[in,out]   PpsMsgNode              Pointer to the message node.
 * \param[in]       PpsMessageLayer         Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK      Successful Execution
 * \retval      #OCP_FL_ERROR   Failure Execution
 */
_STATIC_H int32_t DtlsHS_SInit_MessageNode(sMsgInfo_d* PpsMsgNode, const sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sbBlob_d sbBlobMessage;
    sbBlob_d sMessage;
    sMessageLayer_d sMessageLayer;
    uint32_t dwTotalLength;

    sMessageLayer.wOIDDevCertificate = PpsMessageLayer->wOIDDevCertificate;
    sMessageLayer.pfGetUnixTIme = PpsMessageLayer->pfGetUnixTIme;
    sMessageLayer.psConfigRL = PpsMessageLayer->psConfigRL;
    sMessageLayer.wMaxPmtu = PpsMessageLayer->wMaxPmtu;
    sMessageLayer.wSessionID = PpsMessageLayer->wSessionID;

    do
    {
        i4Status = MsgLayer_FormMessage((eMsgType_d)PpsMsgNode->bMsgType, &sMessageLayer, &sbBlobMessage);
        if(OCP_ML_OK != i4Status)
        {
            break;
        }
        PpsMsgNode->psMsgHolder = sbBlobMessage.prgbStream;

        //Assign the pointer from Handshake message header to local blob
        sMessage.prgbStream = sbBlobMessage.prgbStream + (OVERHEAD_LEN - MSG_HEADER_LEN);
        sMessage.wLen = sbBlobMessage.wLen - (OVERHEAD_LEN - MSG_HEADER_LEN);

        dwTotalLength = HS_MESSAGE_LENGTH(sMessage.prgbStream);

        if(sMessage.wLen != (dwTotalLength + MSG_HEADER_LEN))
        {
            break;
        }
        PpsMsgNode->psMsgMapPtr = NULL;
        PpsMsgNode->dwMsgLength = dwTotalLength;
        PpsMsgNode->wMsgSequence = HS_MESSAGE_SEQNUM(sMessage.prgbStream);
        PpsMsgNode->eMsgState = eComplete;

        i4Status = (int32_t)OCP_FL_OK;
    }while(0);

    return i4Status;
}

/**
 * Checks if given optional message is in the list.<br>
 *
 * \param[in]       PbMsgID             Message type.
 * \param[in]       PpbOptMsgList       List of received optional messages.
 *
 * \retval      #OCP_FL_OK              Requested message found in list
 * \retval      #OCP_FL_ERROR           Requested message not found in list
 */
_STATIC_H int32_t DtlsHS_CheckOptMsg(uint8_t PbMsgID, const uint8_t* PpbOptMsgList)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;

    do
    {
        if(NULL == PpbOptMsgList)
        {
            break;
        }
        while(0xFF != *(PpbOptMsgList))
        {
            if(PbMsgID == *PpbOptMsgList)
            {
                i4Status = (int32_t)OCP_FL_OK;
                break;
            }
            PpbOptMsgList++;
        }

    }while(0);

    return i4Status;
}

/**
 * Checks if the given message can be sent to the server or not.<br>
 *
 * \param[in]       PbMsgID             Message type.
 * \param[in]       PpbOptMsgList       List of received optional messages.
 * \param[in]       PpsMessageLayer     Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK              Requested message can be sent
 * \retval      #OCP_FL_ERROR           Requested message cannot be sent
 */
_STATIC_H int32_t DtlsHS_Flight5_CheckOptMsg(uint8_t PbMsgID, const uint8_t* PpbOptMsgList, const sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;

    do
    {
        if(((uint8_t)eClientCertificate == PbMsgID) || ((uint8_t)eCertificateVerify == PbMsgID))
        {
            if(OCP_FL_OK != DtlsHS_CheckOptMsg((uint8_t)eCertificateRequest, PpbOptMsgList))
            {
                i4Status = (int32_t)OCP_FL_ERROR;
                break;
            }
        }

        if(((uint8_t)eCertificateVerify == PbMsgID) && (0x0000 == PpsMessageLayer->wOIDDevCertificate))
        {
            i4Status = (int32_t)OCP_FL_ERROR;
            break;
        }

    }while(0);

    return i4Status;
}

/**
 * Initialises message node to respective parameters.<br>
 *  - Forms the change cipher spec message.<br>
 *
 * \param[in,out]   PpsMsgNode              Pointer to the message node.
 *
 * \retval      #OCP_FL_OK              Requested message can be sent
 * \retval      #OCP_FL_ERROR           Requested message cannot be sent
 */
_STATIC_H int32_t DtlsHS_SInit_ChangeCipherSpec(sMsgInfo_d* PpsMsgNode)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
/// @cond hidden
#define CHANGE_CIPHERSPEC_MSGSIZE 1
/// @endcond
    do
    {
        PpsMsgNode->psMsgHolder = (uint8_t*)OCP_MALLOC(CHANGE_CIPHERSPEC_MSGSIZE);
        if(NULL == PpsMsgNode->psMsgHolder)
        {
            i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
            break;
        }
        *PpsMsgNode->psMsgHolder = (int32_t)0x01;
        PpsMsgNode->dwMsgLength = CHANGE_CIPHERSPEC_MSGSIZE;
        PpsMsgNode->psMsgMapPtr = NULL;

    }while(0);
/// @cond hidden
#undef CHANGE_CIPHERSPEC_MSGSIZE
/// @endcond
    return i4Status;
}


/**
 * Returns the sequence number of the last message in a flight.<br>
 *
 * \param[in]       PpsMessageList          Pointer to list of messages.
 * \param[out]      PwLastMsgSeqNum         Pointer to last sequence number.
 *
 * \retval      #OCP_FL_OK      Successful Execution
 * \retval      #OCP_FL_ERROR   Failure Execution
 */
_STATIC_H void DtlsHS_FlightGetLastMsgSeqNum(const sMsgInfo_d *PpsMessageList, uint16_t *PwLastMsgSeqNum)
{
    do
    {
        if(NULL == PpsMessageList)
        {
            break;
        }
        while(NULL != PpsMessageList->psNext)
        {
            PpsMessageList = PpsMessageList->psNext;
        }
        *PwLastMsgSeqNum = PpsMessageList->wMsgSequence;

    }while(0);
}

/**
 * Initializes the message node with respective parameters and updates the bit map.<br>
 *
 * \param[in,out]       PpsMsgNode          Pointer to new message node.
 * \param[in]           PpsMessageLayer     Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK      Successful Execution
 * \retval      #OCP_FL_ERROR   Failure Execution
 * \retval      #OCP_FL_MSG_INCOMPLETE  Message is not completely received
 */
_STATIC_H int32_t DtlsHS_RInit_MessageNode(sMsgInfo_d* PpsMsgNode, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = OCP_FL_OK;
    uint32_t dwOffset;
    uint32_t dwFragLen;
    uint32_t dwTotalLen;

    do
    {
        PpsMsgNode->bMsgType = HS_MESSAGE_TYPE(PpsMessageLayer->sMsg.prgbStream);

        dwOffset = HS_MESSAGE_FRAGOFFSET(PpsMessageLayer->sMsg.prgbStream);
        dwFragLen = HS_MESSAGE_FRAGLEN(PpsMessageLayer->sMsg.prgbStream);
        dwTotalLen = HS_MESSAGE_LENGTH(PpsMessageLayer->sMsg.prgbStream);

        //Length of the message payload should not exceed 1536 bytes
        if(MAX_MSG_PAYLOAD < dwTotalLen)
        {
            i4Status = (int32_t)OCP_HL_BUFFER_OVERFLOW;
            break;
        }

        memcpy((PpsMsgNode->psMsgHolder + OVERHEAD_LEN + dwOffset), PpsMessageLayer->sMsg.prgbStream + LENGTH_HS_MSG_HEADER, dwFragLen);
        PpsMsgNode->wMsgSequence = HS_MESSAGE_SEQNUM(PpsMessageLayer->sMsg.prgbStream);
        PpsMsgNode->dwMsgLength = dwTotalLen;
        PpsMsgNode->psMsgMapPtr = NULL;
        PpsMsgNode->bMsgCount = 0;

        //lint --e{534} suppress "Return value is not required to be checked"
        DtlsHS_PrepareMsgHeader((PpsMsgNode->psMsgHolder + (OVERHEAD_LEN - MSG_HEADER_LEN)), PpsMsgNode);

        //Initialise Bit Map for message status
        if(OCP_FL_OK != DtlsHS_MsgCompleteInit(PpsMsgNode->dwMsgLength, &PpsMsgNode->psMsgMapPtr))
        {
            i4Status = (int32_t)OCP_FL_ERROR;
            break;
        }

        //lint --e{534} suppress "Return value is not required to be checked"
        DtlsHS_MsgUptBitMsk(dwOffset, dwFragLen, PpsMsgNode->psMsgMapPtr, PpsMsgNode->dwMsgLength);

        // Check Message Completeness
        if(OCP_FL_OK != DtlsHS_MsgCompleteCheck(PpsMsgNode->psMsgMapPtr, PpsMsgNode->dwMsgLength))
        {
            i4Status = (int32_t)OCP_FL_MSG_INCOMPLETE;
            break;
        }
        DtlsHS_Update_OptList(PpsMessageLayer, PpsMsgNode->bMsgType, (uint8_t)PpsMessageLayer->eFlight);
        // If message is completely received, change state to Received
        UPDATE_MSGSTATE(PpsMsgNode->eMsgState, eComplete);
        PpsMsgNode->bMsgCount++;

    }while(0);
    return i4Status;
}

/**
 * Returns the total number of messages in a flight.<br>
 *
 * \param[in]       PwMsgDescList           Pointer to list of messages.
 * \param[out]      PbMsgCount              Pointer to message counter.
 *
 */
_STATIC_H void DtlsHS_GetTotalMsgCount(const uint16_t *PwMsgDescList, uint8_t *PbMsgCount)
{
    *PbMsgCount = 0;

    do
    {
        (*PbMsgCount)++;

    }while(0xFFFF != *(++PwMsgDescList));

}

/**
 * Returns the number of optional messages in a flight.<br>
 *
 * \param[in]       PwMsgDescList           Pointer to list of messages.
 * \param[out]      PbOptMsgCount               Pointer to message counter.
 *
 */
_STATIC_H void DtlsHS_GetOptMsgCount(const uint16_t *PwMsgDescList, uint8_t *PbOptMsgCount)
{

    do
    {
        if(IsOptional(*PwMsgDescList))
        {
            (*PbOptMsgCount)++;
        }

    }while(0xFFFF != *(++PwMsgDescList));

}

/**
 * Returns the message descriptor from the list of messages.<br>
 *
 * \param[in]       PwMsgDescList           Pointer to list of messages.
 * \param[in]       PbMsgType               Message ID of which descriptor is required.
 * \param[out]      PwMsgDesc               Pointer to message descriptor.
 *
 */
_STATIC_H void DtlsHS_GetMsgDesc(const uint16_t *PwMsgDescList, uint8_t PbMsgType, uint16_t *PwMsgDesc)
{
    do
    {
        if(MSG_ID(*PwMsgDescList) == PbMsgType)
        {
            *PwMsgDesc = *PwMsgDescList;
        }
        PwMsgDescList++;

    }while(0xFFFF != *PwMsgDescList);
}

/**
 * Adds a optional message to the list of optional messages.<br>
 *
 * \param[in,out]       PpsMessageLayer         Pointer to the structure containing message configuration information.
 * \param[in]           PbMsgType               Optional Message ID.
 * \param[in]           PeFlightID              Flight ID.
 *
 */
_STATIC_H void DtlsHS_Update_OptList(sMsgLyr_d* PpsMessageLayer, uint8_t PbMsgType, uint8_t PeFlightID)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    uint8_t bIndex = 0;
    uint16_t *pwFlightMsgs;

    do
    {

        DtlsHS_GetFlightMsgInfo(PeFlightID, &pwFlightMsgs);

        //Check whether given message is optional
        while(0xFFFF != *pwFlightMsgs)
        {
            if(MSG_ID(*pwFlightMsgs) == PbMsgType)
            {
                if(IsOptional(*pwFlightMsgs))
                {
                    i4Status = (int32_t)OCP_FL_OK;
                    break;
                }
            }
            pwFlightMsgs++;
        }
        //If the Message is optional, then store the message ID in Optional Message list
        if((int32_t)OCP_FL_OK == i4Status)
        {
            do
            {
                if((PpsMessageLayer->rgbOptMsgList[bIndex] == PbMsgType) || (PpsMessageLayer->rgbOptMsgList[bIndex] == 0xFF))
                {
                    PpsMessageLayer->rgbOptMsgList[bIndex] = PbMsgType;
                    break;
                }
            }while(++bIndex < ((sizeof(PpsMessageLayer->rgbOptMsgList)/sizeof(PpsMessageLayer->rgbOptMsgList[0]))));
        }
    }while(0);
}

/**
 * Checks if all the messages of a flight are completely received.<br>
 *
 * \param[in]       PpsMessageList          Pointer to list of messages of a flight.
 * \param[in]       PeFlightID              Flight ID.
 * \param[in]       PpsMessageLayer         Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK      All messages are completely received
 * \retval      #OCP_FL_ERROR   Atleast one message is incomplete
 */
_STATIC_H int32_t DtlsHS_CheckFlight_Compl(sMsgInfo_d *PpsMessageList, uint8_t PeFlightID, const sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    sMsgInfo_d* psMsgListTrav = PpsMessageList;
    uint32_t dwLastMsgSeq, dwPrevFlightMsgSeq;
    uint16_t *pwFlightMsgs;
    uint8_t bOptMsgCount = 0;
    uint8_t bTotalMsgInFlight = 0;
    uint8_t bTotalOptMsgInFlight = 0;
    uint8_t bTotalMsgRxDInFlight = 0;
    uint16_t wMsgDesc;
    uint8_t bSeqNumFlag = TRUE, bLoopCount, bIndex;

    dwLastMsgSeq = PpsMessageLayer->dwRMsgSeqNum;
    dwPrevFlightMsgSeq = PpsMessageLayer->dwRMsgSeqNum;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if(NULL == psMsgListTrav)
        {
            break;
        }
#endif
        DtlsHS_GetFlightMsgInfo(PeFlightID, &pwFlightMsgs);

        DtlsHS_GetTotalMsgCount(pwFlightMsgs, &bTotalMsgInFlight);

        DtlsHS_GetOptMsgCount(pwFlightMsgs, &bTotalOptMsgInFlight);

        if(dwPrevFlightMsgSeq == 0xFFFFFFFF)
        {
            bLoopCount = 2;
        }
        else
        {
            bLoopCount = 1;
        }

        for(bIndex = 0; bIndex < bLoopCount; bIndex++)
        {
            psMsgListTrav = PpsMessageList;
            do
            {
                if(eComplete == psMsgListTrav->eMsgState)
                {
                    DtlsHS_GetMsgDesc(pwFlightMsgs, psMsgListTrav->bMsgType, &wMsgDesc);

                    if(IsOptional(wMsgDesc))
                    {
                        bOptMsgCount++;
                    }
                    if(psMsgListTrav->wMsgSequence != dwLastMsgSeq+1)
                    {
                        bSeqNumFlag = FALSE;
                    }
                    dwLastMsgSeq = psMsgListTrav->wMsgSequence;
                    bTotalMsgRxDInFlight++;
                }
                psMsgListTrav = psMsgListTrav->psNext;
            }while(NULL != psMsgListTrav);

            if((bOptMsgCount != 0) && ((bTotalMsgInFlight == bTotalMsgRxDInFlight) && (TRUE == bSeqNumFlag)))
            {
                i4Status = (int32_t)OCP_FL_OK;
                break;
            }
            else if((bOptMsgCount == 0) && ((bTotalMsgRxDInFlight == bTotalMsgInFlight-bTotalOptMsgInFlight) && (TRUE == bSeqNumFlag)))
            {
                i4Status = (int32_t)OCP_FL_OK;
                break;
            }
            else
            {
                i4Status = (int32_t)OCP_FL_MSG_INCOMPLETE;
            }
            dwLastMsgSeq = dwPrevFlightMsgSeq+1;
            bOptMsgCount = 0;
            bTotalMsgRxDInFlight = 0;
            bSeqNumFlag = TRUE;
        }

    }while(0);

    return i4Status;
}

/**
 * Checks if a message is received more than or equal to six times.<br>
 *
 * \param[in]       PpsMsgNode          Pointer to message node.
 *
 * \retval      #OCP_FL_OK      Message is received less than six times
 * \retval      #OCP_FL_ERROR   Message is received six times
 */
_STATIC_H int32_t DtlsHS_MessageLimitCheck(sMsgInfo_d *PpsMsgNode)
{
    int32_t i4Status = OCP_FL_OK;
    do
    {
        if(++(PpsMsgNode->bMsgCount) > OCP_MSGRX_MAX_COUNT)
        {
            PpsMsgNode->bMsgCount = OCP_MSGRX_MAX_COUNT;
            i4Status = (int32_t)OCP_FL_ERROR;
        }
    }while(0);
    return i4Status;
}

/**
 * Buffers the received message/ fragment.<br>
 *
 * \param[in,out]   PpsMessageList            Pointer to list of messages in a flight.
 * \param[in]       PbMsgID                   Received message type.
 * \param[in]       PpsMsgIn                  Pointer to the received message buffer.
 * \param[in]       PpsMessageLayer           Pointer to the structure containing message configuration information.
 * \param[in]       PeFlightID                Flight number.
 *
 * \retval      #OCP_FL_OK                    Successful execution
 * \retval      #OCP_FL_ERROR                 Failure in execution
 * \retval      #OCP_FL_MSG_NODE_NOT_AVAIL    Node corresponding to message is not in the list of messages
 * \retval      #OCP_FL_MALLOC_FAILURE        Memory allocation failure
 * \retval      #OCP_FL_INVALID_MSG_LENGTH    Message Length of new fragment does not match the buffered message length
 * \retval      #OCP_FL_INVALID_MSG_SEQ       Sequence number of new fragment does not match the buffered message/ fragment sequence length
 * \retval      #OCP_FL_MSG_INCOMPLETE        Message is not completely received
 * \retval      #OCP_FL_MSG_MAXCOUNT          Message is received six times
 */
_STATIC_H int32_t DtlsHS_FlightMsgChkAndBuffer(sMsgInfo_d *PpsMessageList, uint8_t PbMsgID, const sbBlob_d* PpsMsgIn, sMsgLyr_d* PpsMessageLayer, uint8_t PeFlightID)
{
    int32_t i4Status = (int32_t)OCP_FL_MSG_NODE_NOT_AVAIL;
    uint32_t dwOffset = 0;
    uint32_t dwFragLength = 0;
    sMsgInfo_d *psMsgListTrav = PpsMessageList;

    do
    {
        if(NULL == psMsgListTrav)
        {
            break;
        }

        do
        {
            if(psMsgListTrav->bMsgType == PbMsgID)
            {
                if((ePartial == psMsgListTrav->eMsgState))
                {
                    dwOffset = HS_MESSAGE_FRAGOFFSET(PpsMsgIn->prgbStream);
                    dwFragLength = HS_MESSAGE_FRAGLEN(PpsMsgIn->prgbStream);
                    if(NULL == psMsgListTrav->psMsgHolder)
                    {
                        psMsgListTrav->dwMsgLength = HS_MESSAGE_LENGTH(PpsMsgIn->prgbStream);
                        psMsgListTrav->bMsgType = *PpsMsgIn->prgbStream;

                        psMsgListTrav->psMsgHolder = (uint8_t*)OCP_MALLOC( psMsgListTrav->dwMsgLength + OVERHEAD_LEN);
                        if(NULL == psMsgListTrav->psMsgHolder)
                        {
                            i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                            break;
                        }
                        //lint --e{534} suppress "Return value is not required to be checked"
                        DtlsHS_PrepareMsgHeader((psMsgListTrav->psMsgHolder + (OVERHEAD_LEN - MSG_HEADER_LEN)), psMsgListTrav);
                    }

                    if(NULL == psMsgListTrav->psMsgMapPtr)
                    {
                        //Initialise Bit Map for message status
                        if(OCP_FL_OK != DtlsHS_MsgCompleteInit(psMsgListTrav->dwMsgLength, &psMsgListTrav->psMsgMapPtr))
                        {
                            i4Status = (int32_t)OCP_FL_ERROR;
                            break;
                        }
                    }

                    if(psMsgListTrav->dwMsgLength != HS_MESSAGE_LENGTH(PpsMsgIn->prgbStream))
                    {
                        i4Status = (int32_t)OCP_FL_INVALID_MSG_LENGTH;
                        break;
                    }

                    if(psMsgListTrav->wMsgSequence != HS_MESSAGE_SEQNUM(PpsMsgIn->prgbStream))
                    {
                        i4Status = (int32_t)OCP_FL_INVALID_MSG_SEQ;
                        break;
                    }
                    //Buffer the message
                    memcpy(psMsgListTrav->psMsgHolder+OVERHEAD_LEN+dwOffset, PpsMsgIn->prgbStream+LENGTH_HS_MSG_HEADER, dwFragLength);
                    //Update message status
                    //lint --e{534} suppress "Return value is not required to be checked"
                    DtlsHS_MsgUptBitMsk(dwOffset, dwFragLength, psMsgListTrav->psMsgMapPtr, psMsgListTrav->dwMsgLength);
                }

                // Check Message Completeness
                if(OCP_FL_OK != DtlsHS_MsgCompleteCheck(psMsgListTrav->psMsgMapPtr, psMsgListTrav->dwMsgLength))
                {
                    i4Status = (int32_t)OCP_FL_MSG_INCOMPLETE;
                    break;
                }

                // If message is completely received, change state to Complete
                UPDATE_MSGSTATE(psMsgListTrav->eMsgState, eComplete);

                DtlsHS_Update_OptList(PpsMessageLayer, psMsgListTrav->bMsgType, PeFlightID);

                if(OCP_FL_OK != DtlsHS_MessageLimitCheck(psMsgListTrav))
                {
                    i4Status = (int32_t)OCP_FL_MSG_MAXCOUNT;
                    break;
                }

                i4Status = OCP_FL_OK;
            }
            psMsgListTrav = psMsgListTrav->psNext;

        }while(NULL != psMsgListTrav);

    }while(0);

    return i4Status;
}

/**
 * Checks if the message type of the received message/ fragment belongs to the flight.<br>
 *
 * \param[in]       PbFlightID              Flight ID.
 * \param[in]       PbMsgID                 Received Message type
 *
 * \retval      #OCP_FL_OK                    Successful execution
 * \retval      #OCP_FL_MSG_NOT_IN_FLIGHT     Received message is not in flight
 */
_STATIC_H int32_t DtlsHS_Flight_LimitChk(uint8_t PbFlightID, uint8_t PbMsgID)
{
    int32_t i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
    uint16_t* pwMsgList = NULL;

    do
    {
        DtlsHS_GetFlightMsgInfo(PbFlightID, &pwMsgList);

        while(0xFFFF != *pwMsgList)
        {
            if(PbMsgID == FLIGHTID(*pwMsgList))
            {
                i4Status = OCP_FL_OK;
                break;
            }
            pwMsgList++;
        }

    }while(0);
    return i4Status;
}

/**
 * Sends the message to be processed to Security Chip.<br>
 *
 * \param[in]   PpsMessageList            Pointer to the structure containing message linked list.
 * \param[in]   PpsMessageLayer           Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_ML_OK                          Successful execution
 * \retval      #OCP_ML_ERROR                       Failure in execution
 * \retval      #CMD_DEV_ERROR                      Command lib error
 * \retval      #OCP_FL_SEND_MSG_TO_OPTIGA_ERROR    Message processing failed on Security Chip
 *
 */
_STATIC_H int32_t DtlsHS_SendFlightToOptiga(sMsgInfo_d *PpsMessageList, const sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_ML_ERROR;
    sMessageLayer_d sMessageLayer;
    sbBlob_d sMessage;

    do
    {
        sMessageLayer.wOIDDevCertificate = PpsMessageLayer->wOIDDevCertificate;
        sMessageLayer.pfGetUnixTIme = PpsMessageLayer->pfGetUnixTIme;
        sMessageLayer.psConfigRL = PpsMessageLayer->psConfigRL;
        sMessageLayer.wMaxPmtu = PpsMessageLayer->wMaxPmtu;
        sMessageLayer.wSessionID = PpsMessageLayer->wSessionID;

        while(NULL != PpsMessageList)
        {
            if(NULL != PpsMessageList->psMsgHolder)
            {
                sMessage.prgbStream = PpsMessageList->psMsgHolder;
                sMessage.wLen = (uint16_t)PpsMessageList->dwMsgLength + OVERHEAD_LEN;

                //Process the obtained message by sending the message to Security Chip
                i4Status = MsgLayer_ProcessMessage((eMsgType_d)PpsMessageList->bMsgType, &sMessageLayer, &sMessage);
                if((i4Status & (int32_t)DEV_ERROR_CODE_MASK) == (int32_t)CMD_DEV_ERROR)
                {
                    break;
                }
                else if((int32_t)OCP_ML_OK != i4Status)
                {
                    i4Status = (int32_t)OCP_FL_SEND_MSG_TO_OPTIGA_ERROR;
                    break;
                }
            }
            UPDATE_MSGSTATE(PpsMessageList->eMsgState, eProcessed);
            PpsMessageList = PpsMessageList->psNext;
        }
    }while(0);

    return i4Status;

}

/**
 * Resets the flight 2 node .<br>
 *
 * \param[in,out]   PpsThisFlight             Pointer to the structure containing flight status.
 *
 */
_STATIC_H void DtlsHS_ResetFlight2MsgNode(const sFlightStats_d* PpsThisFlight)
{
    do
    {
        if(NULL != PpsThisFlight->psMessageList->psMsgHolder)
        {
            OCP_FREE(PpsThisFlight->psMessageList->psMsgHolder);
            PpsThisFlight->psMessageList->psMsgHolder = NULL;
        }
        if(NULL != PpsThisFlight->psMessageList->psMsgMapPtr)
        {
            OCP_FREE(PpsThisFlight->psMessageList->psMsgMapPtr);
            PpsThisFlight->psMessageList->psMsgMapPtr = NULL;
        }
        PpsThisFlight->psMessageList->eMsgState = ePartial;
    }while(0);
}

/**
 * Frees a node and all the pointers in it .<br>
 *
 * \param[in]   PpsMsgNode            Pointer to the message node.
 *
 */
_STATIC_H void DtlsHS_FreeMsgNode(sMsgInfo_d *PpsMsgNode)
{
    if(NULL != PpsMsgNode->psMsgHolder)
    {
        OCP_FREE(PpsMsgNode->psMsgHolder);
        PpsMsgNode->psMsgHolder = NULL;
    }
    if(NULL != PpsMsgNode->psMsgMapPtr)
    {
        OCP_FREE(PpsMsgNode->psMsgMapPtr);
        PpsMsgNode->psMsgMapPtr = NULL;
    }
    OCP_FREE(PpsMsgNode);
}
/**
 * Checks if message sequence number and length of received message/ fragment of flight4 is the same as the buffered one.<br>
 *
 * \param[in]       PbRxMsgType             Message type of received message/ fragment.
 * \param[in]       PwRxMsgSeqNum           Message sequence number of received message/ fragment.
 * \param[in]       PdwRxMsgLen             Message sequence number of received message/ fragment.
 * \param[in]       PpsMsgList              Pointer to list of messages in the flight.
 *
 * \retval      #OCP_FL_OK                  Sequence number and message length of buffered message is same as re-received message/ fragment
 * \retval      #OCP_FL_ERROR               Received message/ fragment can be ignored
 */
_STATIC_H int32_t DtlsHS_Flight4CheckBuffMsg(uint8_t PbRxMsgType, uint16_t PwRxMsgSeqNum, uint32_t PdwRxMsgLen, sMsgInfo_d *PpsMsgList)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    sMsgInfo_d *psMsgListTrav = PpsMsgList;

    do
    {
        if(psMsgListTrav->bMsgType == PbRxMsgType)
        {
            if((psMsgListTrav->wMsgSequence == PwRxMsgSeqNum) && (psMsgListTrav->dwMsgLength == PdwRxMsgLen))
            {
                i4Status = (int32_t)OCP_FL_OK;
                break;
            }
        }
        psMsgListTrav = psMsgListTrav->psNext;

    }while(NULL != psMsgListTrav);
    return i4Status;
}

/**
 * Checks for flight4 completion.<br>
 *
 * \param[in]       PpsMsgList              Pointer to message list of the flight.
 *
 * \retval      #OCP_FL_OK                  Flight 4 is received completely
 * \retval      #OCP_FL_ERROR               Flight 4 is not received completely
 */
_STATIC_H int32_t DtlsHS_Flight4ReRxCompl(sMsgInfo_d *PpsMsgList)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sMsgInfo_d *psMsgListTrav = PpsMsgList;

    do
    {
        if(eProcessed != psMsgListTrav->eMsgState)
        {
            i4Status = (int32_t)OCP_FL_ERROR;
            break;
        }
        psMsgListTrav = psMsgListTrav->psNext;

    }while(NULL != psMsgListTrav);

    return i4Status;
}

/**
 * Updates bit map and sets the message state.<br>
 *
 * \param[in]       PbMsgID                 Message type of received message/ fragment.
 * \param[in]       PpsMsgList              Pointer to message list of the flight.
 * \param[in]       PpsMessageLayer         Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK                  Successful execution
 * \retval      #OCP_FL_ERROR               Received message/ fragment can be ignored
 */
_STATIC_H int32_t DtlsHS_Flight4UpdateMsgStat(uint8_t PbMsgID, sMsgInfo_d *PpsMsgList, const sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    sMsgInfo_d *psMsgListTrav = PpsMsgList;
    uint32_t dwOffset, dwFragLen;

    dwOffset = HS_MESSAGE_FRAGOFFSET(PpsMessageLayer->sMsg.prgbStream);
    dwFragLen = HS_MESSAGE_FRAGLEN(PpsMessageLayer->sMsg.prgbStream);

    do
    {
        do
        {
            if(NULL != psMsgListTrav)
            {
                if(psMsgListTrav->bMsgType == PbMsgID)
                {
                    i4Status = (int32_t)OCP_FL_OK;
                    break;
                }
                psMsgListTrav = psMsgListTrav->psNext;
            }

        }while(NULL != psMsgListTrav);

        //lint --e{613} suppress "If 'psMsgListTrav' parameter is null then based on return code it doesnt enter the below path"
        if(OCP_FL_OK == i4Status)
        {
            if(OCP_FL_OK != DtlsHS_MsgUptBitMsk(dwOffset, dwFragLen, psMsgListTrav->psMsgMapPtr, psMsgListTrav->dwMsgLength))
            {
                i4Status = (int32_t)OCP_FL_ERROR;
                break;
            }
            // Check Message Completeness
            if(OCP_FL_OK != DtlsHS_MsgCompleteCheck(psMsgListTrav->psMsgMapPtr, psMsgListTrav->dwMsgLength))
            {
                i4Status = (int32_t)OCP_FL_MSG_INCOMPLETE;
                break;
            }

            UPDATE_MSGSTATE(psMsgListTrav->eMsgState, eProcessed);

            if(OCP_FL_OK != DtlsHS_MessageLimitCheck(psMsgListTrav))
            {
                i4Status = (int32_t)OCP_FL_MSG_MAXCOUNT;
                break;
            }
        }
    }while(0);
    return i4Status;
}

/**
 * Clears the messages of a flight and resets the bit map.<br>
 *
 * \param[in, out]       PpsMsgList          Pointer to list of messages.
 *
 */
_STATIC_H void DtlsHS_Flight4ClearMsgsInList(sMsgInfo_d *PpsMsgList)
{
    sMsgInfo_d *psMsgListTrav = PpsMsgList;

    do
    {
        if(NULL!= psMsgListTrav->psMsgHolder)
        {
            OCP_FREE(psMsgListTrav->psMsgHolder);
            psMsgListTrav->psMsgHolder = NULL;
        }
        if(NULL != psMsgListTrav->psMsgMapPtr)
        {
            if(OCP_FL_OK == DtlsHS_MsgClearBitMap(psMsgListTrav->psMsgMapPtr, psMsgListTrav->dwMsgLength))
            {
                UPDATE_MSGSTATE(psMsgListTrav->eMsgState, ePartial);
            }
            else
            {
                break;
            }
        }
        psMsgListTrav = psMsgListTrav->psNext;
    }while(NULL != psMsgListTrav);
}

/**
 * Checks Message Sequence number of flight six messages.<br>
 *
 * \param[in]       PpsMessageLayer         Pointer to the structure containing message configuration information.
 * \param[in]       PbMsgID                 Message Type of the received message/ fragment.
 * \param[in]       PwMsgSeqNum             Message sequence number of received message.
 *
 * \retval      #OCP_FL_OK                  Sequence number is correct
 * \retval      #OCP_FL_ERROR               Sequence number is incorrect
 */
_STATIC_H int32_t DtlsHS_Flight6CheckMsgSeqNum(const sMsgLyr_d* PpsMessageLayer, uint8_t PbMsgID, uint16_t PwMsgSeqNum)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;

    do
    {
        if((PbMsgID == (uint8_t)eServerFinished) && (PwMsgSeqNum != (uint16_t)PpsMessageLayer->dwRMsgSeqNum+1))
        {
            i4Status = (int32_t)OCP_FL_ERROR;
        }
        else if(PbMsgID == (uint8_t)eChangeCipherSpec)
        {
            i4Status = (int32_t)OCP_FL_OK;
        }

    }while(0);

    return i4Status;
}


/**
 * Searches the look-up table and returns the message descriptors of a flight.<br>
 *
 * \param[in]       PeFlightID      Flight number.
 * \param[in,out]       PpwMessageList       Pointer container to message descriptor.
 *
 */
void DtlsHS_GetFlightMsgInfo(uint8_t PeFlightID, uint16_t** PpwMessageList)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    uint8_t bIndex;

    do
    {
        for(bIndex = 0; bIndex < OCP_FLIGHT_TABLE_MAX_SIZE; bIndex++)
        {
            if(((rgsSFlightInfo[bIndex].wFlightDesc) & 0xFF) == (uint8_t)PeFlightID)
            {
                *PpwMessageList = (uint16_t*)&(rgsSFlightInfo[bIndex].wMsgTypes[0]);
                i4Status = (int32_t)OCP_FL_OK;
                break;
            }
        }
        if((int32_t)OCP_FL_ERROR == i4Status)
        {
            for(bIndex = 0; bIndex < OCP_FLIGHT_TABLE_MAX_SIZE; bIndex++)
            {
                if(((rgsRFlightInfo[bIndex].wFlightDesc) & 0xFF) == (uint8_t)PeFlightID)
                {
                    *PpwMessageList = (uint16_t*)&(rgsRFlightInfo[bIndex].wMsgTypes[0]);
                    break;
                }
            }
        }
    }while(0);
}

/**
 * Initialises flight node to default parameters.<br>
 *
 * \param[in,out]   PpsFlightNode               Pointer to flight node
 * \param[in]       PbLastProcFlight            Last processed flight ID
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
int32_t DtlsHS_FlightNodeInit(sFlightDetails_d* PpsFlightNode, uint8_t PbLastProcFlight)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;

    do
    {
        PpsFlightNode->psNext = NULL;
        if(OCP_FL_OK != DtlsHS_GetFlightDescriptor(PbLastProcFlight+1, &PpsFlightNode->wFlightDecp))
        {
            i4Status = (int32_t)OCP_FL_ERROR;
            break;
        }
        if(OCP_FL_OK != DtlsHS_GetFlightHandler(FLIGHTID(PpsFlightNode->wFlightDecp), &PpsFlightNode->pFlightHndlr))
        {
            i4Status = (int32_t)OCP_FL_ERROR;
            break;
        }
        PpsFlightNode->sFlightStats.psMessageList = NULL;
        UPDATE_FSTATE(PpsFlightNode->sFlightStats.bFlightState, (uint8_t)efInit);
    }while(0);
    return i4Status;
}

/**
 * Gets the Flight type for the corresponding message type.<br>
 *
 * \param[in]       PbMsgType               Message type
 * \param[in,out]   PpFlightID              Pointer to the flight number
 *
 * \retval      #OCP_FL_OK      Successful Execution
 * \retval      #OCP_FL_ERROR   Failure Execution
 */
int32_t DtlsHS_GetFlightID(uint8_t PbMsgType, uint8_t* PpFlightID)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    uint8_t bFlightIndex,bMsgIndex ;

    do
    {
        for(bFlightIndex = 0; bFlightIndex < OCP_FLIGHT_TABLE_MAX_SIZE; bFlightIndex++)
        {
            bMsgIndex = 0;
            while(MSG_ID(rgsRFlightInfo[bFlightIndex].wMsgTypes[bMsgIndex]) != 0xFF)
            {
                if(MSG_ID(rgsRFlightInfo[bFlightIndex].wMsgTypes[bMsgIndex]) == PbMsgType)
                {
                    *PpFlightID = FLIGHTID(rgsRFlightInfo[bFlightIndex].wFlightDesc);
                    i4Status = (int32_t)OCP_FL_OK;
                    break;
                }
                bMsgIndex++;
            }
            if((int32_t)OCP_FL_OK == i4Status)
            {
                break;
            }
        }
    }while(0);

    return i4Status;
}

/**
 * Checks whether the received message belongs to the expected flight.<br>
 *
 * \param[in]       PbLastProcFlight            Last processed flight number
 * \param[in,out]   PpsBlobMessage              Pointer to the Blob containing record
 * \param[in]       PpsMessageLayer             Pointer to the structure containing message layer information
 *
 * \retval      #OCP_FL_OK      Successful Execution
 * \retval      #OCP_FL_ERROR   Failure Execution
 */
int32_t DtlsHS_MsgCheck(uint8_t PbLastProcFlight, const sbBlob_d* PpsBlobMessage, const sMsgLyr_d* PpsMessageLayer)
{
    uint8_t bFlightA = 0;
    uint8_t bFlightB = 0;
    int32_t i4Status = (int32_t)OCP_FL_OK;
    uint8_t bMsgType;
    uint32_t dwTotalMsgLen = 0;
    uint8_t bFirstMsgFlightID = 0;
    uint8_t bMsgFlightID;
    sbBlob_d sMsgIn;


    do
    {
        //Change cipher spec record is received
        if(PpsMessageLayer->psConfigRL->sRL.bRecvCCSRecord == CCS_RECORD_RECV)
        {
            if((PpsMessageLayer->eFlight != eFlight5) && (PpsMessageLayer->eFlight != eFlight6))
            {
                //CCS message doesn't belongs to flight 6
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                PpsMessageLayer->psConfigRL->sRL.bRecvCCSRecord = CCS_RECORD_NOTRECV;
            }
            break;
        }

        //Minimum length check
        if(LENGTH_HS_MSG_HEADER > PpsBlobMessage->wLen)
        {
            i4Status = (int32_t)OCP_HL_INVALID_LENGTH;
            break;
        }

        sMsgIn.prgbStream = PpsBlobMessage->prgbStream;
        sMsgIn.wLen = PpsBlobMessage->wLen;

        //Check for default flight state
        if((uint8_t)PpsMessageLayer->eFlight == PbLastProcFlight)
        {
            if((PbLastProcFlight == 1) || (PbLastProcFlight == 3))
            {
                bFlightA = (uint8_t)eFlight2;
                bFlightB = (uint8_t)eFlight4;
            }
            else if(PbLastProcFlight == 5)
            {
                bFlightA = (uint8_t)eFlight4;
                bFlightB = (uint8_t)eFlight6;
            }

            bMsgType = *(sMsgIn.prgbStream);

            //Get the flight number to which the message belongs
            if(OCP_FL_OK != DtlsHS_GetFlightID(bMsgType, &bFirstMsgFlightID))
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_LISTED;
                break;
            }

            //Check if the flight number belongs to the expected the flight
            if((bFlightA != bFirstMsgFlightID) && (bFlightB != bFirstMsgFlightID))
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }

            //Header validation
            i4Status = DtlsHS_ProcHeader(sMsgIn);
            if(OCP_HL_OK == i4Status)
            {
                i4Status = (int32_t)OCP_FL_OK;
            }
            else
            {
                break;
            }

            dwTotalMsgLen = Utility_GetUint24(sMsgIn.prgbStream + OFFSET_HS_MSG_FRAGLEN);

            sMsgIn.prgbStream += dwTotalMsgLen + LENGTH_HS_MSG_HEADER;
            sMsgIn.wLen -= (uint16_t)dwTotalMsgLen + LENGTH_HS_MSG_HEADER;
        }

        //For multiple messages in a record
        while(sMsgIn.wLen != 0)
        {
            bMsgType = *(sMsgIn.prgbStream);

            //Minimum length check
            if(LENGTH_HS_MSG_HEADER > sMsgIn.wLen)
            {
                i4Status = (int32_t)OCP_HL_INVALID_LENGTH;
                break;
            }

            //Get the flight number to which the message belongs
            if(OCP_FL_OK != DtlsHS_GetFlightID(bMsgType, &bMsgFlightID))
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_LISTED;
                break;
            }

            if((uint8_t)PpsMessageLayer->eFlight == PbLastProcFlight)
            {
                if(bFirstMsgFlightID != bMsgFlightID)
                {
                    i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                    break;
                }
            }
            else
            {
                if(PpsMessageLayer->eFlight != (eFlight_d)bMsgFlightID)
                {
                    i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                    break;
                }
            }

            i4Status = DtlsHS_ProcHeader(sMsgIn);
            if(OCP_HL_OK == i4Status)
            {
                i4Status = (int32_t)OCP_FL_OK;
            }
            else
            {
                break;
            }

            dwTotalMsgLen = Utility_GetUint24(sMsgIn.prgbStream + OFFSET_HS_MSG_FRAGLEN);

            sMsgIn.wLen -= (uint16_t)dwTotalMsgLen + LENGTH_HS_MSG_HEADER;

            if(sMsgIn.wLen != 0)
            {
                sMsgIn.prgbStream += dwTotalMsgLen + LENGTH_HS_MSG_HEADER;
            }
        }
    }while(0);

    return i4Status;
}

/**
 * Flight one handler to process flight 1 messages .<br>
 *
 * \param[in]       PbLastProcFlight          Last processed flight.
 * \param[in,out]   PpsThisFlight             Pointer to structure containing flight1 status.
 * \param[in]       PpsMessageLayer           Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK                          Successful execution
 * \retval      #OCP_FL_MSG_ERROR                   Failure in execution
 * \retval      #OCP_FL_MALLOC_FAILURE              Malloc failure
 * \retval      #OCP_FL_GET_MSG_FROM_OPTIGA_ERROR    Error from Security Chip
 * \retval      #OCP_FL_FLIGHTSEND_ERROR            Error while sending flight to Security Chip
 */
//lint --e{818} suppress "This is ignored as same flight handler pointer is used for both send and receive"
int32_t DtlsHS_Flight1Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpsThisFlight, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sMsgInfo_d* psMsgListTrav = NULL;
    uint16_t *pwMsgIDList = NULL;

    do
    {
        if(((uint8_t)efInit == PpsThisFlight->bFlightState) && ((uint8_t)eFlight0 == PbLastProcFlight))
        {
            //Generate List of Messages of this flight
            //create list of messages of this flight and call Command lib to get messages from Security Chip
            DtlsHS_GetFlightMsgInfo((uint8_t)eFlight1, &pwMsgIDList);

            while(0xFF != MSG_ID(*pwMsgIDList))
            {
                psMsgListTrav = (sMsgInfo_d*)OCP_MALLOC(sizeof(sMsgInfo_d));
                if(NULL == psMsgListTrav)
                {
                    i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                    break;
                }
                psMsgListTrav->bMsgType = MSG_ID(*pwMsgIDList);
                psMsgListTrav->eMsgState = ePartial;
                psMsgListTrav->psNext = NULL;
                psMsgListTrav->psMsgMapPtr = NULL;
                psMsgListTrav->psMsgHolder = NULL;

                DtlsHS_InsertMsgNode(&PpsThisFlight->psMessageList, psMsgListTrav);
                pwMsgIDList++;
            }

            if((int32_t)OCP_FL_OK == i4Status)
            {
                UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efReady);
            }
        }
        else if(((uint8_t)eFlight1 == PbLastProcFlight+1) || ((uint8_t)eFlight1 == PbLastProcFlight))
        {

            if((uint8_t)efReady == PpsThisFlight->bFlightState)
            {
                //Get Message from Security Chip and Send to Server
                psMsgListTrav = PpsThisFlight->psMessageList;

                while(NULL != psMsgListTrav)
                {
                    if(ePartial == psMsgListTrav->eMsgState)
                    {
                        i4Status = DtlsHS_SInit_MessageNode(psMsgListTrav, PpsMessageLayer);
                        if((i4Status & (int32_t)DEV_ERROR_CODE_MASK) == (int32_t)CMD_DEV_ERROR)
                        {
                            break;
                        }
                        else if((int32_t)OCP_FL_OK != i4Status)
                        {
                            i4Status = (int32_t)OCP_FL_GET_MSG_FROM_OPTIGA_ERROR;
                            break;
                        }
                        psMsgListTrav->eMsgState = eComplete;
                    }
                    if(OCP_HL_OK != DtlsHS_FSendMessage(psMsgListTrav, PpsMessageLayer))
                    {
                        i4Status = (int32_t)OCP_FL_FLIGHTSEND_ERROR;
                        break;
                    }
                    psMsgListTrav->eMsgState = eProcessed;
                    psMsgListTrav = psMsgListTrav->psNext;
                }
                if((int32_t)OCP_FL_OK == i4Status)
                {
                    PpsMessageLayer->eFlight = eFlight1;
                    UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efTransmitted);
                }
            }
            else if((uint8_t)efReTransmit == PpsThisFlight->bFlightState)
            {
                psMsgListTrav = PpsThisFlight->psMessageList;
                //If already transmitted, now retransmit
                while(NULL != psMsgListTrav)
                {
                    if(OCP_HL_OK != DtlsHS_FSendMessage(psMsgListTrav, PpsMessageLayer))
                    {
                        i4Status = (int32_t)OCP_FL_FLIGHTSEND_ERROR;
                        break;
                    }
                    psMsgListTrav = psMsgListTrav->psNext;
                }
                if((int32_t)OCP_FL_OK == i4Status)
                {
                    UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efTransmitted);
                }
            }
        }
        else
        {
            PpsThisFlight->bFlightState = (uint8_t)efDone;
            break;
        }
    }while(0);

    return i4Status;
}

/**
 * Flight three handler to process flight 3 message.<br>
 *
 * \param[in]       PbLastProcFlight          Last processed flight.
 * \param[in,out]   PpsThisFlight             Pointer to structure containing flight3 status.
 * \param[in]       PpsMessageLayer           Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK                          Successful execution
 * \retval      #OCP_FL_MSG_ERROR                   Failure in execution
 * \retval      #OCP_FL_MALLOC_FAILURE              Malloc failure
 * \retval      #OCP_FL_GET_MSG_FROM_OPTIGA_ERROR    Error from Security Chip
 * \retval      #OCP_FL_FLIGHTSEND_ERROR            Error while sending flight to Security Chip
 */
int32_t DtlsHS_Flight3Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpsThisFlight, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sMsgInfo_d* psMsgListTrav = NULL;
    uint16_t *pwMsgIDList = NULL;
    uint8_t bMsgType = 0;

    do
    {
        if(((uint8_t)efInit == PpsThisFlight->bFlightState) && ((uint8_t)eFlight2 == PbLastProcFlight))
        {
            //create list of messages of this flight and call Command lib to get messages from Security Chip
            DtlsHS_GetFlightMsgInfo((uint8_t)eFlight3, &pwMsgIDList);

            while(0xFF != MSG_ID(*pwMsgIDList))
            {
                psMsgListTrav = (sMsgInfo_d*)OCP_MALLOC(sizeof(sMsgInfo_d));
                if(NULL == psMsgListTrav)
                {
                    i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                    break;
                }
                psMsgListTrav->bMsgType = MSG_ID(*pwMsgIDList);
                psMsgListTrav->eMsgState = ePartial;
                psMsgListTrav->psNext = NULL;
                psMsgListTrav->psMsgMapPtr = NULL;
                psMsgListTrav->psMsgHolder = NULL;

                DtlsHS_InsertMsgNode(&PpsThisFlight->psMessageList, psMsgListTrav);
                pwMsgIDList++;
            }

            if((int32_t)OCP_FL_OK == i4Status)
            {
                UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efReady);
            }
        }
        else if((uint8_t)eFlight3 == PbLastProcFlight+1)
        {
            if(((uint8_t)efReady == PpsThisFlight->bFlightState) || ((uint8_t)efTransmitted == PpsThisFlight->bFlightState))
            {
                //Get Message from Security Chip and Send to Server
                psMsgListTrav = PpsThisFlight->psMessageList;

                if((uint8_t)efTransmitted == PpsThisFlight->bFlightState)
                {
                    OCP_FREE(psMsgListTrav->psMsgHolder);
                    psMsgListTrav->psMsgHolder = NULL;
                    psMsgListTrav->eMsgState = ePartial;
                }
                while(NULL != psMsgListTrav)
                {
                    if(ePartial == psMsgListTrav->eMsgState)
                    {
                        bMsgType = psMsgListTrav->bMsgType;
                        psMsgListTrav->bMsgType = (uint8_t)eClientHelloWithCookie;

                        i4Status = DtlsHS_SInit_MessageNode(psMsgListTrav, PpsMessageLayer);
                        psMsgListTrav->bMsgType = bMsgType;

                        if((i4Status & (int32_t)DEV_ERROR_CODE_MASK) == (int32_t)CMD_DEV_ERROR)
                        {
                            break;
                        }
                        else if((int32_t)OCP_FL_OK != i4Status)
                        {
                            i4Status = (int32_t)OCP_FL_GET_MSG_FROM_OPTIGA_ERROR;
                            break;
                        }

                        psMsgListTrav->eMsgState = eComplete;
                    }
                    if(OCP_HL_OK != DtlsHS_FSendMessage(psMsgListTrav, PpsMessageLayer))
                    {
                        i4Status = (int32_t)OCP_FL_FLIGHTSEND_ERROR;
                        break;
                    }
                    psMsgListTrav->eMsgState = eProcessed;
                    psMsgListTrav = psMsgListTrav->psNext;
                }
                if(OCP_FL_OK == i4Status)
                {
                    PpsMessageLayer->eFlight = eFlight3;
                    UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efTransmitted);
                }
            }
        }
        else if((uint8_t)eFlight3 == PbLastProcFlight)
        {
            if((uint8_t)efReTransmit == PpsThisFlight->bFlightState)
            {
                psMsgListTrav = PpsThisFlight->psMessageList;
                //If already transmitted, now retransmit
                while(NULL != psMsgListTrav)
                {
                    if(OCP_HL_OK != DtlsHS_FSendMessage(psMsgListTrav, PpsMessageLayer))
                    {
                        i4Status = (int32_t)OCP_FL_FLIGHTSEND_ERROR;
                        break;
                    }
                    psMsgListTrav = psMsgListTrav->psNext;
                }
                if(OCP_FL_OK == i4Status)
                {
                    UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efTransmitted);
                }
            }
        }
        else
        {
            PpsThisFlight->bFlightState = (uint8_t)efDone;
            break;
        }
    }while(0);

    return i4Status;
}

/**
 * Flight five handler to process flight 5 messages .<br>
 *
 * \param[in]       PbLastProcFlight          Last processed flight.
 * \param[in,out]   PpsThisFlight             Pointer to structure containing flight5 status.
 * \param[in]       PpsMessageLayer           Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK                          Successful execution
 * \retval      #OCP_FL_MSG_ERROR                   Failure in execution
 * \retval      #OCP_FL_MALLOC_FAILURE              Malloc failure
 * \retval      #OCP_FL_GET_MSG_FROM_OPTIGA_ERROR    Error from Security Chip
 * \retval      #OCP_FL_FLIGHTSEND_ERROR            Error while sending flight to Security Chip
 */
//lint --e{818} suppress "This is ignored as same flight handler pointer is used for both send and receive"
int32_t DtlsHS_Flight5Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpsThisFlight, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sMsgInfo_d* psMsgListTrav = NULL;
    uint16_t *pwMsgIDList = NULL;

    do
    {
        if(((uint8_t)efInit == PpsThisFlight->bFlightState) && ((uint8_t)eFlight4 == PbLastProcFlight))
        {
            //create list of messages of this flight and call Command lib to get messages from Security Chip
            DtlsHS_GetFlightMsgInfo((uint8_t)eFlight5, &pwMsgIDList);

            while(0xFF != MSG_ID(*pwMsgIDList))
            {
                if(OCP_FL_OK == DtlsHS_Flight5_CheckOptMsg(MSG_ID(*pwMsgIDList), &(PpsMessageLayer->rgbOptMsgList[0]), PpsMessageLayer))
                {
                    psMsgListTrav = (sMsgInfo_d*)OCP_MALLOC(sizeof(sMsgInfo_d));
                    if(NULL == psMsgListTrav)
                    {
                        i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                        break;
                    }
                    psMsgListTrav->bMsgType = MSG_ID(*pwMsgIDList);
                    psMsgListTrav->eMsgState = ePartial;
                    psMsgListTrav->psNext = NULL;
                    psMsgListTrav->psMsgMapPtr = NULL;
                    psMsgListTrav->psMsgHolder = NULL;

                    DtlsHS_AddMsgNode(&PpsThisFlight->psMessageList, psMsgListTrav);
                }
                pwMsgIDList++;
            }

            if((int32_t)OCP_FL_OK == i4Status)
            {
                UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efReady);
            }
        }
        else if(((uint8_t)eFlight5 == PbLastProcFlight+1) || ((uint8_t)eFlight5 == PbLastProcFlight))
        {

            if((uint8_t)efReady == PpsThisFlight->bFlightState)
            {
                //Get Message from Security Chip and Send to Server
                psMsgListTrav = PpsThisFlight->psMessageList;

                while(NULL != psMsgListTrav)
                {
                    if(ePartial == psMsgListTrav->eMsgState)
                    {
                        if((uint8_t)eChangeCipherSpec == psMsgListTrav->bMsgType)
                        {
                            i4Status = DtlsHS_SInit_ChangeCipherSpec(psMsgListTrav);
                        }
                        else
                        {
                            i4Status = DtlsHS_SInit_MessageNode(psMsgListTrav, PpsMessageLayer);
                        }
                        if((i4Status & (int32_t)DEV_ERROR_CODE_MASK) == (int32_t)CMD_DEV_ERROR)
                        {
                            break;
                        }
                        else if((int32_t)OCP_FL_OK != i4Status)
                        {
                            i4Status = (int32_t)OCP_FL_GET_MSG_FROM_OPTIGA_ERROR;
                            break;
                        }
                        psMsgListTrav->eMsgState = eComplete;
                    }
                    if(OCP_HL_OK != DtlsHS_FSendMessage(psMsgListTrav, PpsMessageLayer))
                    {
                        i4Status = (int32_t)OCP_FL_FLIGHTSEND_ERROR;
                        break;
                    }
                    psMsgListTrav->eMsgState = eProcessed;
                    psMsgListTrav = psMsgListTrav->psNext;
                }
                if((int32_t)OCP_FL_OK == i4Status)
                {
                    PpsMessageLayer->eFlight = eFlight5;
                    UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efTransmitted);
                }
            }
            else if(((uint8_t)efReTransmit == PpsThisFlight->bFlightState) || ((uint8_t)efTransmitted == PpsThisFlight->bFlightState))
            {
                PpsMessageLayer->psConfigRL->sRL.fRetransmit = TRUE;

                psMsgListTrav = PpsThisFlight->psMessageList;
                //If already transmitted, now retransmit
                while(NULL != psMsgListTrav)
                {
                    if(OCP_HL_OK != DtlsHS_FSendMessage(psMsgListTrav, PpsMessageLayer))
                    {
                        i4Status = (int32_t)OCP_FL_FLIGHTSEND_ERROR;
                        break;
                    }
                    psMsgListTrav = psMsgListTrav->psNext;
                }
                if((int32_t)OCP_FL_OK == i4Status)
                {
                    UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efTransmitted);
                }
            }
        }
    }while(0);

    return i4Status;
}

/**
 * Validates the sequence number of message/ fragment received of flight 2.<br>
 *
 * \param[in]       PbRxMsgID           Message type of the received message/ fragment.
 * \param[in]   PwRxMsgSeqNum       Message sequence number of the received message/ fragment.
 * \param[in]       PpsMessageList      Pointer to list of messages.
 *
 * \retval      #OCP_FL_OK                  Successful Execution
 * \retval      #OCP_FL_INVALID_MSG_SEQNUM  Sequence number does not match to buffered fragment sequence number
 */
int32_t DtlsHS_Flight2CheckMsgSeqNum(uint8_t PbRxMsgID, uint16_t PwRxMsgSeqNum, const sMsgInfo_d *PpsMessageList)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;

    do
    {
        if(PwRxMsgSeqNum > 1)
        {
            i4Status = (int32_t)OCP_FL_INVALID_MSG_SEQNUM;
            break;
        }

        while(NULL != PpsMessageList)
        {
            if(PpsMessageList->bMsgType == PbRxMsgID)
            {
                if(PpsMessageList->wMsgSequence != PwRxMsgSeqNum)
                {
                    i4Status = (int32_t)OCP_FL_INVALID_MSG_SEQNUM;
                    break;
                }
            }
            PpsMessageList = PpsMessageList->psNext;
        }
    }while(0);
    return i4Status;
}

/**
 * Flight two handler to process flight 2 messages.<br>
 *
 * \param[in]       PbLastProcFlight          Last processed flight.
 * \param[in,out]   PpsThisFlight             Pointer to the structure containing flight2 status.
 * \param[in]       PpsMessageLayer           Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK                          Successful execution
 * \retval      #OCP_FL_MSG_ERROR                   Failure in execution
 * \retval      #OCP_FL_MALLOC_FAILURE              Memory allocation failure
 * \retval      #OCP_FL_INVALID_MSG_SEQNUM          Invalid Message sequence number
 * \retval      #OCP_FL_MSG_NOT_IN_FLIGHT           Message doesn't belong to current flight
 * \retval      #OCP_FL_RXING                       Message is not yet completely received
 */
//lint --e{818} suppress "This is ignored as same flight handler pointer is used for both send and receive"
int32_t DtlsHS_Flight2Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpsThisFlight, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    uint16_t bMsgSeqNum;
    uint8_t bMsgID;
    sMsgInfo_d* psMsgListTrav = NULL;
    uint16_t wFlightLastMsgSeqNum = 0xFFFF;

    do
    {
        //Check Message/ Fragment
        //Optional Message Check
        //Check Message Sequence Number
        //Buffer the message/ De-Fragmentation()
        //Message Completeness()
        //Send to Security Chip for Processing()
        if((uint8_t)efInit == PpsThisFlight->bFlightState)
        {
            PpsThisFlight->bFlightState = (uint8_t)efReady;
            break;
        }
        else if((PpsThisFlight->bFlightState == (uint8_t)efReady) || (PpsThisFlight->bFlightState == (uint8_t)efProcessed) || (PpsThisFlight->bFlightState == (uint8_t)efReReceive))
        {
            if(PpsThisFlight->bFlightState == (uint8_t)efProcessed)
            {
                PpsThisFlight->bFlightState = (uint8_t)efReReceive;
            }

            bMsgSeqNum = HS_MESSAGE_SEQNUM(PpsMessageLayer->sMsg.prgbStream);
            bMsgID = HS_MESSAGE_TYPE(PpsMessageLayer->sMsg.prgbStream);

            if(OCP_FL_OK != DtlsHS_Flight_LimitChk((uint8_t)eFlight2,bMsgID))
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }
            if(OCP_FL_OK != FLIGHT_IDLIMITCHK(1, PbLastProcFlight, 3))
            {
                i4Status = (int32_t)OCP_FL_INVALID_PROCFLIGHT;
                break;
            }
            if(OCP_FL_OK != DtlsHS_Flight2CheckMsgSeqNum(bMsgID, bMsgSeqNum, PpsThisFlight->psMessageList))
            {
                i4Status = (int32_t)OCP_FL_INVALID_MSG_SEQNUM;
                break;
            }

            i4Status = DtlsHS_FlightMsgChkAndBuffer(PpsThisFlight->psMessageList, bMsgID, &(PpsMessageLayer->sMsg), PpsMessageLayer, (uint8_t)eFlight2);
            if(((int32_t)OCP_FL_ERROR == i4Status) || ((int32_t)OCP_FL_MSG_MAXCOUNT == i4Status) || ((int32_t)OCP_FL_MALLOC_FAILURE == i4Status))
            {
                break;
            }

            PpsMessageLayer->eFlight = eFlight2;

            if((int32_t)OCP_FL_MSG_NODE_NOT_AVAIL == i4Status)
            {
                // Buffer the message
                psMsgListTrav = (sMsgInfo_d*)OCP_MALLOC(sizeof(sMsgInfo_d));
                if(NULL == psMsgListTrav)
                {
                    i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                    break;
                }
                psMsgListTrav->eMsgState = ePartial;
                psMsgListTrav->psNext = NULL;
                psMsgListTrav->psMsgMapPtr = NULL;
                psMsgListTrav->psMsgHolder = (uint8_t*)OCP_MALLOC(HS_MESSAGE_LENGTH(PpsMessageLayer->sMsg.prgbStream) + OVERHEAD_LEN);
                if(NULL == psMsgListTrav->psMsgHolder)
                {
                    DtlsHS_FreeMsgNode(psMsgListTrav);
                    i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                    break;
                }

                i4Status = DtlsHS_RInit_MessageNode(psMsgListTrav, PpsMessageLayer);

                if(((int32_t)OCP_FL_OK != i4Status) && ((int32_t)OCP_FL_MSG_INCOMPLETE != i4Status))
                {
                    DtlsHS_FreeMsgNode(psMsgListTrav);
                    break;
                }
                DtlsHS_InsertMsgNode(&PpsThisFlight->psMessageList, psMsgListTrav);
            }

            //Check Flight Completeness
            if(NULL != PpsThisFlight->psMessageList)
            {
                if((eComplete != PpsThisFlight->psMessageList->eMsgState) && (eProcessed != PpsThisFlight->psMessageList->eMsgState))
                {
                    i4Status = (int32_t)OCP_FL_RXING;
                    break;
                }
            }
            else
            {
                i4Status = (int32_t)OCP_FL_ERROR;
                break;
            }

            PpsThisFlight->bFlightState = (uint8_t)efReceived;

            //Send to Security Chip For Processing
            i4Status = DtlsHS_SendFlightToOptiga(PpsThisFlight->psMessageList, PpsMessageLayer);
            if(OCP_ML_OK != i4Status)
            {
                break;
            }
            //Update Flight Status
            PpsThisFlight->bFlightState = (uint8_t)efProcessed;
            DtlsHS_ResetFlight2MsgNode(PpsThisFlight);
            DtlsHS_FlightGetLastMsgSeqNum(PpsThisFlight->psMessageList, &wFlightLastMsgSeqNum);
            UPDATE_RX_MSGSEQNUM(PpsMessageLayer->dwRMsgSeqNum, wFlightLastMsgSeqNum);
        }
        i4Status = OCP_FL_OK;
    }while(0);

    return i4Status;
}

/**
 * Checks if message sequence number of received message/ fragment of flight4 is correct.<br>
 *
 * \param[in]       PpsMessageLayer             Pointer to the structure containing message configuration information.
 * \param[in]       PbRxMsgID                   Message type of received message/ fragment.
 * \param[in]       PwRxMsgSeqNum               Message sequence number of received message/ fragment.
 *
 * \retval      #OCP_FL_OK                      Successful execution
 * \retval      #OCP_FL_ERROR                   Message Sequence number incorrect
 */
int32_t DtlsHS_Flight4CheckMsgSeqNum(const sMsgLyr_d* PpsMessageLayer, uint8_t PbRxMsgID, uint16_t PwRxMsgSeqNum)
{
    int32_t i4Status = (int32_t)OCP_FL_ERROR;
    uint16_t *pwFlightMsgs = NULL;
    uint32_t dwPrevFlightMsgSeq, dwLastFlightSeqNum ;
    uint8_t bOptMsgCount = 0;
    uint8_t bLoopCount, bIndex, bMandtryMsgCount = 0;

    do
    {
        dwLastFlightSeqNum = PpsMessageLayer->dwRMsgSeqNum;
        dwPrevFlightMsgSeq = PpsMessageLayer->dwRMsgSeqNum;

        if(dwPrevFlightMsgSeq == 0xFFFFFFFF)
        {
            bLoopCount = 2;
        }
        else
        {
            bLoopCount = 1;
        }
        for(bIndex = 0; bIndex < bLoopCount; bIndex++)
        {
            DtlsHS_GetFlightMsgInfo((uint8_t)eFlight4, &pwFlightMsgs);

            do
            {

                if(IsMandatory(*pwFlightMsgs))
                {
                    bMandtryMsgCount++;
                }
                else
                {
                    bOptMsgCount++;
                }
                if(MSG_ID(*pwFlightMsgs) == PbRxMsgID)
                {

                    if(((dwLastFlightSeqNum+bMandtryMsgCount+bOptMsgCount) == PwRxMsgSeqNum) || ((dwLastFlightSeqNum+bMandtryMsgCount) == PwRxMsgSeqNum))
                    {
                        i4Status = (int32_t)OCP_FL_OK;
                        break;
                    }
                }
            }while(0xFFFF != *(++pwFlightMsgs));

            if((int32_t)OCP_FL_OK == i4Status)
            {
                break;
            }
            dwLastFlightSeqNum = dwPrevFlightMsgSeq + 1;
            bMandtryMsgCount = 0;
            bOptMsgCount = 0;

        }
    }while(FALSE);
    return i4Status;
}

/**
 * Flight four handler to process flight 4 messages.<br>
 *
 * \param[in]       PbLastProcFlight          Last processed flight.
 * \param[in,out]   PpsThisFlight             Pointer to the structure containing flight4 status.
 * \param[in]       PpsMessageLayer           Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK                          Successful execution
 * \retval      #OCP_FL_MSG_ERROR                   Failure in execution
 * \retval      #OCP_FL_MALLOC_FAILURE              Memory allocation failure
 * \retval      #OCP_FL_INVALID_MSG_SEQNUM          Invalid Message sequence number
 * \retval      #OCP_FL_MSG_NOT_IN_FLIGHT           Message doesn't belong to current flight
 * \retval      #OCP_FL_RXING                       Message is not yet completely received
 * \retval      #OCP_FL_MSG_MAXCOUNT                Message is received at least 6 times
 */
int32_t DtlsHS_Flight4Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpsThisFlight, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sMsgInfo_d* psMsgListTrav = NULL;
    uint8_t bMsgID;
    uint16_t wMsgSeqNum;
    uint32_t dwMsgLen;
    uint16_t wFlightLastMsgSeqNum = 0xFFFF;

    do
    {
        //Check Message/ Fragment
        //Optional Message Check
        //Check Message Sequence Number
        //Buffer the message/ De-Fragmentation()
        //Message Completeness()
        //Send to Security Chip for Processing()
        if((uint8_t)efInit == PpsThisFlight->bFlightState)
        {
            PpsThisFlight->bFlightState = (uint8_t)efReady;
            break;
        }
        else if((uint8_t)efReady == PpsThisFlight->bFlightState)
        {
            bMsgID = HS_MESSAGE_TYPE(PpsMessageLayer->sMsg.prgbStream);
            wMsgSeqNum = HS_MESSAGE_SEQNUM(PpsMessageLayer->sMsg.prgbStream);
            dwMsgLen = HS_MESSAGE_LENGTH(PpsMessageLayer->sMsg.prgbStream);

            if(OCP_FL_OK != DtlsHS_Flight_LimitChk((uint8_t)eFlight4,bMsgID))
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }

            if(0x00 != PpsMessageLayer->psConfigRL->sRL.bDecRecord)
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }

            if(OCP_FL_OK != FLIGHT_IDLIMITCHK(1, PbLastProcFlight, 5))
            {
                i4Status = (int32_t)OCP_FL_INVALID_PROCFLIGHT;
                break;
            }

            if(OCP_FL_OK != DtlsHS_Flight4CheckMsgSeqNum(PpsMessageLayer, bMsgID, wMsgSeqNum))
            {
                i4Status = (int32_t)OCP_FL_INVALID_MSG_SEQNUM;
                break;
            }

            i4Status = DtlsHS_FlightMsgChkAndBuffer(PpsThisFlight->psMessageList, bMsgID, &(PpsMessageLayer->sMsg), PpsMessageLayer, (uint8_t)eFlight4);
            if(((int32_t)OCP_FL_ERROR == i4Status) || ((int32_t)OCP_FL_MSG_MAXCOUNT == i4Status) || ((int32_t)OCP_FL_MALLOC_FAILURE == i4Status))
            {
                break;
            }

            PpsMessageLayer->eFlight = eFlight4;

            if((int32_t)OCP_FL_MSG_NODE_NOT_AVAIL == i4Status)
            {
                // Buffer the message
                psMsgListTrav = (sMsgInfo_d*)OCP_MALLOC(sizeof(sMsgInfo_d));
                if(NULL == psMsgListTrav)
                {
                    i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                    break;
                }

                psMsgListTrav->eMsgState = ePartial;
                psMsgListTrav->psNext = NULL;
                psMsgListTrav->psMsgMapPtr = NULL;
                psMsgListTrav->psMsgHolder = (uint8_t*)OCP_MALLOC(HS_MESSAGE_LENGTH(PpsMessageLayer->sMsg.prgbStream) + OVERHEAD_LEN);
                if(NULL == psMsgListTrav->psMsgHolder)
                {
                    DtlsHS_FreeMsgNode(psMsgListTrav);
                    i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                    break;
                }

                i4Status = DtlsHS_RInit_MessageNode(psMsgListTrav, PpsMessageLayer);

                if(((int32_t)OCP_FL_OK != i4Status) && ((int32_t)OCP_FL_MSG_INCOMPLETE != i4Status))
                {
                    DtlsHS_FreeMsgNode(psMsgListTrav);
                    break;
                }
                DtlsHS_InsertMsgNode(&PpsThisFlight->psMessageList, psMsgListTrav);
            }

            //Check Flight Completeness
            i4Status = DtlsHS_CheckFlight_Compl(PpsThisFlight->psMessageList, (uint8_t)eFlight4, PpsMessageLayer);
            if((int32_t)OCP_FL_MSG_INCOMPLETE == i4Status)
            {
                i4Status = (int32_t)OCP_FL_RXING;
                break;
            }

            if(OCP_FL_OK != i4Status)
            {
                break;
            }

            PpsThisFlight->bFlightState = (uint8_t)efReceived;

            //Send to Security Chip For Processing
            i4Status = DtlsHS_SendFlightToOptiga(PpsThisFlight->psMessageList, PpsMessageLayer);
            if(OCP_ML_OK != i4Status)
            {
                break;
            }
            //Update Flight Status
            PpsThisFlight->bFlightState = (uint8_t)efProcessed;
            DtlsHS_FlightGetLastMsgSeqNum(PpsThisFlight->psMessageList, &wFlightLastMsgSeqNum);
            UPDATE_RX_MSGSEQNUM(PpsMessageLayer->dwRMsgSeqNum, wFlightLastMsgSeqNum);
            PpsMessageLayer->eFlight = eFlight0;
            DtlsHS_Flight4ClearMsgsInList(PpsThisFlight->psMessageList);
        }
        else if (((uint8_t)efProcessed == PpsThisFlight->bFlightState) || ((uint8_t)efReReceive == PpsThisFlight->bFlightState))
        {
            if((uint8_t)efProcessed == PpsThisFlight->bFlightState)
            {
                UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efReReceive);
            }

            if(PpsMessageLayer->psConfigRL->sRL.bRecvCCSRecord == CCS_RECORD_RECV)
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }

            bMsgID = HS_MESSAGE_TYPE(PpsMessageLayer->sMsg.prgbStream);
            wMsgSeqNum = HS_MESSAGE_SEQNUM(PpsMessageLayer->sMsg.prgbStream);
            dwMsgLen = HS_MESSAGE_LENGTH(PpsMessageLayer->sMsg.prgbStream);
            psMsgListTrav = PpsThisFlight->psMessageList;

            if(OCP_FL_OK != DtlsHS_Flight_LimitChk((uint8_t)eFlight4,bMsgID))
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }

            if(0x00 != PpsMessageLayer->psConfigRL->sRL.bDecRecord)
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }

            if(OCP_FL_OK != DtlsHS_Flight4CheckBuffMsg(bMsgID, wMsgSeqNum, dwMsgLen, PpsThisFlight->psMessageList))
            {
                i4Status = (int32_t)OCP_FL_MSG_IGNORE;
                break;
            }

            PpsMessageLayer->eFlight = eFlight4;

            i4Status = DtlsHS_Flight4UpdateMsgStat(bMsgID, PpsThisFlight->psMessageList, PpsMessageLayer);

            if(((int32_t)OCP_FL_OK != i4Status) && ((int32_t)OCP_FL_MSG_INCOMPLETE != i4Status))
            {
                break;
            }

            if(OCP_FL_OK != DtlsHS_Flight4ReRxCompl(PpsThisFlight->psMessageList))
            {
                i4Status = (int32_t)OCP_FL_RXING;
                break;
            }
            //Update Flight Status
            UPDATE_FSTATE(PpsThisFlight->bFlightState, (uint8_t)efProcessed);
            DtlsHS_Flight4ClearMsgsInList(PpsThisFlight->psMessageList);
        }
        i4Status = (int32_t)OCP_FL_OK;
    }while(0);

    return i4Status;
}

/**
 * Checks whether flight six is received.<br>
 *
 * \param[in]       PpsMessageList          Pointer to the list of messages of flight6.
 *
 * \retval      #OCP_FL_OK                  Flight six is received
 * \retval      #OCP_FL_ERROR               Flight six is not received
 */
_STATIC_H int32_t DtlsHS_CheckFlight6_Compl(sMsgInfo_d *PpsMessageList)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sMsgInfo_d * psMsgListTrav = PpsMessageList;
    uint16_t * pwFlightMsgs;
    uint8_t bTotalMsgInFlight;
    uint8_t bNumMsgRxdInFlight = 0;

    DtlsHS_GetFlightMsgInfo((uint8_t)eFlight6, &pwFlightMsgs);
    DtlsHS_GetTotalMsgCount(pwFlightMsgs, &bTotalMsgInFlight);

    do
    {
        while(NULL != psMsgListTrav)
        {
            bNumMsgRxdInFlight++;
            if(eComplete != psMsgListTrav->eMsgState)
            {
                i4Status = (int32_t)OCP_FL_MSG_INCOMPLETE;
                break;
            }
            psMsgListTrav = psMsgListTrav->psNext;
        }
        if(((int32_t)OCP_FL_OK == i4Status) && (bNumMsgRxdInFlight != bTotalMsgInFlight))
        {
            i4Status = (int32_t)OCP_FL_MSG_INCOMPLETE;
            break;
        }
    }while(0);
    return i4Status;
}

/**
 * Flight six handler to process flight 6 messages.<br>
 *
 * \param[in]       PbLastProcFlight          Last processed flight.
 * \param[in,out]   PpsThisFlight             Pointer to the structure containing flight6 status.
 * \param[in]       PpsMessageLayer           Pointer to the structure containing message configuration information.
 *
 * \retval      #OCP_FL_OK                          Successful execution
 * \retval      #OCP_FL_MSG_ERROR                   Failure in execution
 * \retval      #OCP_FL_MALLOC_FAILURE              Memory allocation failure
 * \retval      #OCP_FL_INVALID_MSG_SEQNUM          Invalid Message sequence number
 * \retval      #OCP_FL_MSG_NOT_IN_FLIGHT           Message doesn't belong to current flight
 * \retval      #OCP_FL_RXING                       Message is not yet completely received
 * \retval      #OCP_FL_MSG_MAXCOUNT                Message is received at least 6 times
 */
int32_t DtlsHS_Flight6Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpsThisFlight, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_FL_OK;
    sMsgInfo_d* psMsgListTrav = NULL;
    uint8_t bMsgID;
    uint16_t wMsgSeqNum = 0;
    sCBStateTrn_d sCBStateTrn;
    uint16_t wFlightLastMsgSeqNum = 0xFFFF;

/// @cond hidden
#define SETFULL_BITMAP 0xFF
#define SIZE_OF_CCSMSG 0x01
#define CHANGE_CIPHER_SPEC_PROTOCOL 0x01
#define INTERNAL_PROC_ERROR 0x06
#define INVALID_HS_MESSAGE 0x21
/// @endcond
    do
    {
        //Check Message/ Fragment
        //Optional Message Check
        //Check Message Sequence Number
        //Buffer the message/ De-Fragmentation()
        //Message Completeness()
        //Send to Security Chip for Processing()
        if((uint8_t)efInit == PpsThisFlight->bFlightState)
        {
            PpsThisFlight->bFlightState = (uint8_t)efReady;
            break;
        }
        else if((uint8_t)efReady == PpsThisFlight->bFlightState)
        {

            if(PpsMessageLayer->psConfigRL->sRL.bRecvCCSRecord == CCS_RECORD_RECV)
            {
                if((*(PpsMessageLayer->sMsg.prgbStream) != (uint8_t)CHANGE_CIPHER_SPEC_PROTOCOL) || (PpsMessageLayer->sMsg.wLen != (uint16_t)SIZE_OF_CCSMSG))
                {
                    i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                    PpsMessageLayer->psConfigRL->sRL.bRecvCCSRecord = CCS_RECORD_NOTRECV;
                    break;
                }
                if(NULL != PpsMessageLayer->psConfigRL->sRL.fServerStateTrn)
                {
                    sCBStateTrn.phRLHdl = PpsMessageLayer->psConfigRL->sRL.phRLHdl;
                    PpsMessageLayer->psConfigRL->sRL.fServerStateTrn((Void*)&sCBStateTrn);

                }
                else
                {
                    i4Status = (int32_t)OCP_FL_NULL_PARAM;
                    break;
                }

                bMsgID = (uint8_t)eChangeCipherSpec;
            }
            else
            {
                bMsgID = HS_MESSAGE_TYPE(PpsMessageLayer->sMsg.prgbStream);
                wMsgSeqNum = HS_MESSAGE_SEQNUM(PpsMessageLayer->sMsg.prgbStream);
            }

            if(OCP_FL_OK != DtlsHS_Flight_LimitChk((uint8_t)eFlight6, bMsgID))
            {
                i4Status = (int32_t)OCP_FL_MSG_NOT_IN_FLIGHT;
                break;
            }
            if(OCP_FL_OK != FLIGHT_IDLIMITCHK(4, PbLastProcFlight, 6))
            {
                i4Status = (int32_t)OCP_FL_INVALID_PROCFLIGHT;
                break;
            }
            if(OCP_FL_OK != DtlsHS_Flight6CheckMsgSeqNum(PpsMessageLayer, bMsgID, wMsgSeqNum))
            {
                i4Status = (int32_t)OCP_FL_INVALID_MSG_SEQNUM;
                break;
            }

            i4Status = DtlsHS_FlightMsgChkAndBuffer(PpsThisFlight->psMessageList, bMsgID, &(PpsMessageLayer->sMsg), PpsMessageLayer, (uint8_t)eFlight6);
            if(((int32_t)OCP_FL_ERROR == i4Status) || ((int32_t)OCP_FL_MSG_MAXCOUNT == i4Status) || ((int32_t)OCP_FL_MALLOC_FAILURE == i4Status))
            {
                break;
            }

            PpsMessageLayer->eFlight = eFlight6;

            if((int32_t)OCP_FL_MSG_NODE_NOT_AVAIL == i4Status)
            {
                // Buffer the message
                psMsgListTrav = (sMsgInfo_d*)OCP_MALLOC(sizeof(sMsgInfo_d));
                if(NULL == psMsgListTrav)
                {
                    i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                    break;
                }

                if((uint8_t)eChangeCipherSpec == bMsgID)
                {
                    psMsgListTrav->bMsgType = (uint8_t)eChangeCipherSpec;
                    psMsgListTrav->psNext = NULL;
                    psMsgListTrav->psMsgHolder = NULL;
                    psMsgListTrav->bMsgCount = 1;
                    psMsgListTrav->psMsgMapPtr = (uint8_t*)OCP_MALLOC(SIZE_OF_CCSMSG);
                    if(NULL == psMsgListTrav->psMsgMapPtr)
                    {
                        DtlsHS_FreeMsgNode(psMsgListTrav);
                        i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                        break;
                    }
                    *psMsgListTrav->psMsgMapPtr = SETFULL_BITMAP;
                    psMsgListTrav->dwMsgLength = SIZE_OF_CCSMSG;
                    psMsgListTrav->eMsgState = eComplete;
                    i4Status = OCP_FL_OK;
                }
                else
                {
                    psMsgListTrav->eMsgState = ePartial;
                    psMsgListTrav->psNext = NULL;
                    psMsgListTrav->psMsgMapPtr = NULL;

                    psMsgListTrav->psMsgHolder = (uint8_t*)OCP_MALLOC(HS_MESSAGE_LENGTH(PpsMessageLayer->sMsg.prgbStream) + OVERHEAD_LEN);
                    if(NULL == psMsgListTrav->psMsgHolder)
                    {
                        DtlsHS_FreeMsgNode(psMsgListTrav);
                        i4Status = (int32_t)OCP_FL_MALLOC_FAILURE;
                        break;
                    }

                    i4Status = DtlsHS_RInit_MessageNode(psMsgListTrav, PpsMessageLayer);

                    if(((int32_t)OCP_FL_OK != i4Status) && ((int32_t)OCP_FL_MSG_INCOMPLETE != i4Status))
                    {
                        DtlsHS_FreeMsgNode(psMsgListTrav);
                        break;
                    }
                }
                DtlsHS_InsertMsgNode(&PpsThisFlight->psMessageList, psMsgListTrav);
            }

            //Check Flight Completeness
            if((int32_t)OCP_FL_MSG_INCOMPLETE == DtlsHS_CheckFlight6_Compl(PpsThisFlight->psMessageList))
            {
                i4Status = (int32_t)OCP_FL_RXING;
                break;
            }

            if(OCP_FL_OK != i4Status)
            {
                break;
            }

            PpsThisFlight->bFlightState = (uint8_t)efReceived;

            //Send to Security Chip For Processing
            i4Status = DtlsHS_SendFlightToOptiga(PpsThisFlight->psMessageList, PpsMessageLayer);
            if(OCP_ML_OK != i4Status)
            {
                if((i4Status & (int32_t)DEV_ERROR_CODE_MASK) == (int32_t)CMD_DEV_ERROR)
                {
                    if((i4Status & (int32_t)DEV_ERROR_LSBCODE_MASK) == INTERNAL_PROC_ERROR)
                    {
                        i4Status = (int32_t)OCP_FL_INT_ERROR;
                    }
                    else if((i4Status & (int32_t)DEV_ERROR_LSBCODE_MASK) == INVALID_HS_MESSAGE)
                    {
                        i4Status = (int32_t)OCP_FL_HS_ERROR;
                    }
                }
                break;
            }
            //Update Flight Status
            PpsThisFlight->bFlightState = (uint8_t)efProcessed;
            DtlsHS_FlightGetLastMsgSeqNum(PpsThisFlight->psMessageList, &wFlightLastMsgSeqNum);
            UPDATE_RX_MSGSEQNUM(PpsMessageLayer->dwRMsgSeqNum, wFlightLastMsgSeqNum);
        }

        i4Status = (int32_t)OCP_FL_OK;
    }while(0);

/// @cond hidden
#undef SETFULL_BITMAP
#undef SIZE_OF_CCSMSG
#undef CHANGE_CIPHER_SPEC_PROTOCOL
#undef INTERNAL_PROC_ERROR
#undef INVALID_HS_MESSAGE
/// @endcond

    return i4Status;
}

/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
