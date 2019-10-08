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
* \file DtlsHandshakeProtocol.c
*
* \brief   This file implements the DTLS Handshake protocol.
*
* \addtogroup  grMutualAuth
* @{
*/

#include "optiga/dtls/DtlsHandshakeProtocol.h"
#include "optiga/dtls/AlertProtocol.h"
#include "optiga/optiga_dtls.h"
#include "optiga/dtls/DtlsRecordLayer.h"
#include "optiga/dtls/DtlsFlightHandler.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH


///Flight retransmission timeout default value for the first time
#define DEFAULT_TIMEOUT 2

///Maximum Timeout value
#define MAX_FLIGHT_TIMEOUT      60

/// @cond hidden
///Offset for message type
#define OFFSET_MSG_TYPE                 (0)
///Offset for Total length
#define OFFSET_MSG_TOTAL_LENGTH         (OFFSET_MSG_TYPE  + 1) //1
///Offset for message sequence
#define OFFSET_MSG_SEQUENCE             (OFFSET_MSG_TOTAL_LENGTH + 3) //4
///Offset for fragment offset
#define OFFSET_MSG_FRAGMENT_OFFSET      (OFFSET_MSG_SEQUENCE + 2) //6
///Offset for message fragment length
#define OFFSET_MSG_FRAG_LENGTH          (OFFSET_MSG_FRAGMENT_OFFSET+ 3) //9
///Offset for message data
#define OFFSET_MSG_DATA                 (OFFSET_MSG_FRAG_LENGTH  + 3) //12
///Message header length
#define LENGTH_MSG_HEADER               (OFFSET_MSG_DATA)
///Offset for second byte of message fragment length field
#define OFFSET_MSG_FRAG_LENGTH_2BYTE    (OFFSET_MSG_FRAG_LENGTH + 1)

///Macro for Receive Flight
#ifndef DISABLE_RECEIVE_FLIGHT
#define REC_FLIGHT_INITIALIZE(PbLastProcFlight, PppsFlightHead, PpsMessageLayer) DtlsHS_RFlightInitialise(PbLastProcFlight, PppsFlightHead, PpsMessageLayer)
#define REC_FLIGHT_PROCESS(PpbLastProcFlight, PppsRFlightHead,  PpsMessageLayer, PbFlightTimeout) DtlsHS_RFlightProcess(PpbLastProcFlight, PppsRFlightHead,  PpsMessageLayer, PbFlightTimeout)
#else
extern int32_t StubRFlightInitialise(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer);
extern int32_t StubRFlightProcess(uint8_t* PpbLastProcFlight, sFlightDetails_d** PppsRFlightHead,  sMsgLyr_d* PpsMessageLayer, uint8_t PbFlightTimeout);

#define REC_FLIGHT_INITIALIZE(PbLastProcFlight, PppsFlightHead, PpsMessageLayer) StubRFlightInitialise(PbLastProcFlight, PppsFlightHead, PpsMessageLayer)
#define REC_FLIGHT_PROCESS(PpbLastProcFlight, PppsRFlightHead,  PpsMessageLayer, PbFlightTimeout) StubRFlightProcess(PpbLastProcFlight, PppsRFlightHead,  PpsMessageLayer, PbFlightTimeout)
#endif

///Macro for Send Flight
#ifndef DISABLE_SEND_FLIGHT
#define SEND_FLIGHT_INITIALIZE(PbLastProcFlight, PppsFlightHead, PpsMessageLayer) DtlsHS_SFlightInitialise(PbLastProcFlight, PppsFlightHead, PpsMessageLayer)
#define SEND_FLIGHT_PROCESS(PpbLastProcFlight, PpsSFlightHead, PpsMessageLayer) DtlsHS_SFlightProcess(PpbLastProcFlight, PpsSFlightHead, PpsMessageLayer)
#else
extern int32_t StubSFlightInitialise(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer);
extern int32_t StubSFlightProcess(uint8_t *PpbLastProcFlight, sFlightDetails_d* PpsSFlightHead, sMsgLyr_d* PpsMessageLayer);

#define SEND_FLIGHT_INITIALIZE(PbLastProcFlight, PppsFlightHead, PpsMessageLayer) StubSFlightInitialise(PbLastProcFlight, PppsFlightHead, PpsMessageLayer)
#define SEND_FLIGHT_PROCESS(PpbLastProcFlight, PpsSFlightHead, PpsMessageLayer) StubSFlightProcess(PpbLastProcFlight, PpsSFlightHead, PpsMessageLayer)
#endif

#define FLIGHTLoHi(x,y) ((x) | (y<<8))
#define MSGLoHi(x,y) ((x) | (y<<8))
#define IsEVEN_FLIGHT(X) (((X%2) == 0) ? 1 : 0)

///Flight mapping table for the send flights
const sFlightTable_d rgsSFlightInfo[] = {
    {FLIGHTLoHi((uint8_t)eFlight1, \
        (uint8_t)eMandatory), {MSGLoHi((uint8_t)eClientHello, (uint8_t)eMandatory), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF)},\
            DtlsHS_Flight1Handler},
    {FLIGHTLoHi((uint8_t)eFlight3, \
        (uint8_t)eOptional), {MSGLoHi((uint8_t)eClientHello, (uint8_t)eOptional), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF)}, \
            DtlsHS_Flight3Handler},
    {FLIGHTLoHi((uint8_t)eFlight5, (uint8_t)eMandatory), \
        {MSGLoHi((uint8_t)eClientCertificate, (uint8_t)eOptional), MSGLoHi((uint8_t)eClientKeyExchange, (uint8_t)eMandatory), MSGLoHi((uint8_t)eCertificateVerify, (uint8_t)eOptional),  MSGLoHi((uint8_t)eChangeCipherSpec, (uint8_t)eMandatory), MSGLoHi((uint8_t)eClientFinished, (uint8_t)eMandatory), MSGLoHi(0xFF, 0xFF)}, \
            DtlsHS_Flight5Handler},
};

///Flight mapping table for the receive flights
const sFlightTable_d rgsRFlightInfo[] = {
    {FLIGHTLoHi((uint8_t)eFlight2, (uint8_t)eOptional), \
        {MSGLoHi((uint8_t)eHelloVerifyRequest, (uint8_t)eOptional), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF)}, \
            DtlsHS_Flight2Handler},
    {FLIGHTLoHi((uint8_t)eFlight4, (uint8_t)eMandatory), \
        {MSGLoHi((uint8_t)eServerHello, (uint8_t)eMandatory), MSGLoHi((uint8_t)eServerCertificate, (uint8_t)eMandatory), MSGLoHi((uint8_t)eServerKeyExchange, (uint8_t)eMandatory), MSGLoHi((uint8_t)eCertificateRequest, (uint8_t)eOptional), MSGLoHi((uint8_t)eServerHelloDone, (uint8_t)eMandatory), MSGLoHi(0xFF, 0xFF)}, \
            DtlsHS_Flight4Handler},
    {FLIGHTLoHi((uint8_t)eFlight6, (uint8_t)eMandatory), \
        {MSGLoHi((uint8_t)eChangeCipherSpec, (uint8_t)eMandatory), MSGLoHi((uint8_t)eServerFinished, (uint8_t)eMandatory), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF), MSGLoHi(0xFF, 0xFF)}, \
            DtlsHS_Flight6Handler},
};

/// @endcond

/**
 * \brief Fragments a handshake message into smaller fragments.<br>
 */
_STATIC_H int32_t DtlsHS_FragmentMsg(sFragmentMsg_d* PpsFragmentMsg);

/**
 * \brief Frees the complete message list of a flight.<br>
 */
_STATIC_H void DtlsHS_FreeMessageList(sMsgInfo_d **PppsMsgListPtr);

/**
 * \brief Frees flight list except the flight node of interest.<br>
 */
_STATIC_H void DtlsHS_FreeFlightList(uint8_t PbFlightID, sFlightDetails_d** PppsFlightHead);

/**
 * \brief Frees specified flight node.<br>
 */
_STATIC_H void DtlsHS_FreeFlightNode(uint8_t PbFlightID, sFlightDetails_d** PppsFlightHead);

/**
 * \brief Receives a handshake messages from the server.<br>
 */
_STATIC_H int32_t DtlsHS_ReceiveFlightMessage(uint8_t* PpbLastProcFlight, sFlightDetails_d** PppsRFlightHead,  sMsgLyr_d* PpsMessageLayer, uint8_t PbFlightTimeout,uint32_t PdwBasetime);

/**
 * \brief Frees flight node.<br>
 */
_STATIC_H void DtlsHS_ClearBuffer(sFlightDetails_d** PppsFlightHead);

/**
 * \brief Checks a flight is in the list or not.<br>
 */
_STATIC_H int32_t DtlsHS_CheckFlightList(sFlightDetails_d* PpsFlightHead, uint8_t PbFlightID);

/**
 * \brief Initialises the Flight List for Send.<br>
 */
_STATIC_H int32_t DtlsHS_SFlightInitialise(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Initialises the Flight List for Receive.<br>
 */
_STATIC_H int32_t DtlsHS_RFlightInitialise(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Processes the send Flight.<br>
 */
_STATIC_H int32_t DtlsHS_SFlightProcess(uint8_t *PpbLastProcFlight, sFlightDetails_d* PpsSFlightHead, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Processes the receive Flight.<br>
 */
_STATIC_H int32_t DtlsHS_RFlightProcess(uint8_t* PpbLastProcFlight, sFlightDetails_d** PppsRFlightHead,  sMsgLyr_d* PpsMessageLayer, uint8_t PbFlightTimeout);

/**
 * \brief Appends a Flight Node to the end of the list.<br>
 */
_STATIC_H void DtlsHS_CreateFlightList(sFlightDetails_d** PppsFlightHead, sFlightDetails_d* PpsFlightNode);

/**
 * \brief Creates a flight node based on the last processed flight and inserts the node to the head node.<br>
 */
_STATIC_H int32_t DtlsHS_CreateFlightNode(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer);

/**
 * Fragments a handshake message into smaller fragments.<br>
 * Returns a fragment of the handshake message.
 *
 * \param[in,out]   PpsFragmentMsg   Pointer to a structure containing handshake message, fragment size and other
 *                                   information required for fragmentation
 *
 * \retval    #OCP_HL_OK                        Successful execution
 * \retval    #OCP_HL_ERROR                     Failure in execution
\if ENABLE_NULL_CHECKS
 * \retval    #OCP_HL_NULL_PARAM                Null parameter(s)
\endif
 * \retval    #OCP_HL_LENZERO_ERROR             Length of input parameter is zero
 * \retval    #OCP_HL_INVALID_FRAGMENT_SIZE     Invalid fragment size
 * \retval    #OCP_HL_INVALID_OFFSET_LEN        Invalid offset length
 * \retval    #OCP_HL_INSUFFICIENT_MEMORY       Insufficient buffer size
 */
_STATIC_H int32_t DtlsHS_FragmentMsg(sFragmentMsg_d* PpsFragmentMsg)
{
    int32_t i4Status = (int32_t)OCP_HL_ERROR;
    puint8_t prgbSrcBuf;
    puint8_t prgbDesBuf;
    uint16_t wLength;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        //Null Check for input parameters
        if((NULL == PpsFragmentMsg) || (NULL == PpsFragmentMsg->psCompleteMsg) || (NULL == PpsFragmentMsg->psMsgFrag)
            || (NULL == PpsFragmentMsg->psCompleteMsg->prgbStream) ||(NULL == PpsFragmentMsg->psMsgFrag->prgbStream))
        {
            i4Status = (int32_t)OCP_HL_NULL_PARAM;
            break;
        }
#endif
        //Length validation for input parameters
        if((0 == PpsFragmentMsg->psCompleteMsg->wLen) || (0 == PpsFragmentMsg->psMsgFrag->wLen))
        {
            i4Status = (int32_t)OCP_HL_LENZERO_ERROR;
            break;
        }

        //Invalid Fragment size
        if(0 == PpsFragmentMsg->wFragmentSize)
        {
            i4Status = (int32_t)OCP_HL_INVALID_FRAGMENT_SIZE;
            break;
        }

        //Check for sufficient buffer size
        if(PpsFragmentMsg->psMsgFrag->wLen < PpsFragmentMsg->wFragmentSize)
        {
            i4Status = (int32_t)OCP_HL_INSUFFICIENT_MEMORY;
            break;
        }

        //Current offset check
        if(PpsFragmentMsg->wCurrentOffset >= PpsFragmentMsg->psCompleteMsg->wLen)
        {
            i4Status = (int32_t)OCP_HL_INVALID_OFFSET_LEN;
            break;
        }

        if(0 == PpsFragmentMsg->wRemainingLen)
        {
            i4Status = (int32_t)OCP_HL_LENZERO_ERROR;
            break;
        }

        if( 0 == PpsFragmentMsg->wCurrentOffset)
        {
            //Assign the address to copy the message

            prgbSrcBuf = PpsFragmentMsg->psCompleteMsg->prgbStream;

            prgbDesBuf = PpsFragmentMsg->psMsgFrag->prgbStream;

            //If the fragment size is greater than complete message size
            //Length is set to message size
            //else
            //Length is set to fragment size
            wLength = (PpsFragmentMsg->wFragmentSize > PpsFragmentMsg->psCompleteMsg->wLen)? PpsFragmentMsg->psCompleteMsg->wLen : PpsFragmentMsg->wFragmentSize;

            PpsFragmentMsg->psMsgFrag->wLen = wLength;
        }

        else
        {
            //Copy the message header to psMsgFrag
            Utility_Memmove(PpsFragmentMsg->psMsgFrag->prgbStream, PpsFragmentMsg->psCompleteMsg->prgbStream,
                                LENGTH_MSG_HEADER);

            prgbSrcBuf = PpsFragmentMsg->psCompleteMsg->prgbStream + OFFSET_MSG_DATA + PpsFragmentMsg->wCurrentOffset;

            prgbDesBuf = PpsFragmentMsg->psMsgFrag->prgbStream + OFFSET_MSG_DATA;

            //If the remaining length is less than fragment size(Last fragment)
            if(PpsFragmentMsg->wFragmentSize > (PpsFragmentMsg->wRemainingLen + LENGTH_MSG_HEADER))
            {

                //Assign the address to copy the message of size (wRemainingLen) from current offset to psMsgFrag at index msgHeaderLen
                wLength = (uint16_t)PpsFragmentMsg->wRemainingLen;

                PpsFragmentMsg->psMsgFrag->wLen = (uint16_t)(PpsFragmentMsg->wRemainingLen + LENGTH_MSG_HEADER);
            }
            else
            {
                //Assign the address to copy the message of size (wFragmentSize - msgHeaderLen) from current offset to psMsgFrag at index msgHeaderLen
                wLength = PpsFragmentMsg->wFragmentSize - LENGTH_MSG_HEADER;

                PpsFragmentMsg->psMsgFrag->wLen = PpsFragmentMsg->wFragmentSize;
            }
        }

        //To copy the Fragmented message to  psMsgFrag
        Utility_Memmove(prgbDesBuf, prgbSrcBuf, wLength);

        if(PpsFragmentMsg->wFragmentSize < PpsFragmentMsg->psCompleteMsg->wLen)
        {
            //Update the fragment header with current offset at index fragment offset
            Utility_SetUint24 ((PpsFragmentMsg->psMsgFrag->prgbStream + OFFSET_MSG_FRAGMENT_OFFSET),PpsFragmentMsg->wCurrentOffset);

            if(PpsFragmentMsg->wFragmentSize > (PpsFragmentMsg->wRemainingLen + LENGTH_MSG_HEADER))
            {
                //Update the fragment length with wRemainingLen at index fragment length(Last Fragment)
                Utility_SetUint16((PpsFragmentMsg->psMsgFrag->prgbStream + OFFSET_MSG_FRAG_LENGTH_2BYTE),(uint16_t)PpsFragmentMsg->wRemainingLen);
            }
            else
            {
                //Update the fragment length with (fragment size - header length) at index fragment length
                Utility_SetUint16((PpsFragmentMsg->psMsgFrag->prgbStream + OFFSET_MSG_FRAG_LENGTH_2BYTE),(PpsFragmentMsg->wFragmentSize - LENGTH_MSG_HEADER));
            }
        }

        //Update the current offset and remaining length
        //If Last Fragment
        if(PpsFragmentMsg->wFragmentSize > (PpsFragmentMsg->wRemainingLen + LENGTH_MSG_HEADER))
        {
            PpsFragmentMsg->wCurrentOffset += PpsFragmentMsg->wRemainingLen;
            PpsFragmentMsg->wRemainingLen = 0 ;
        }
        //For other fragment
        else
        {
            PpsFragmentMsg->wCurrentOffset += (uint16_t)(PpsFragmentMsg->wFragmentSize - LENGTH_MSG_HEADER);
            PpsFragmentMsg->wRemainingLen -= (uint16_t)(PpsFragmentMsg->wFragmentSize - LENGTH_MSG_HEADER);
        }

        i4Status = (int32_t)OCP_HL_OK;

    }while(0);

    return i4Status;
}

/**
 * Frees the complete message list of a flight.<br>
 *
 * \param[in,out]   PppsMsgListPtr              Pointer to Message list
 *
 */
_STATIC_H void DtlsHS_FreeMessageList(sMsgInfo_d **PppsMsgListPtr)
{
    sMsgInfo_d *pMsgNodeAPtr;
    sMsgInfo_d *pMsgNodeBPtr = NULL;

    do
    {
        if((NULL == PppsMsgListPtr) || (NULL == *PppsMsgListPtr))
        {
            break;
        }
        pMsgNodeAPtr = *PppsMsgListPtr;
        do
        {
            if(NULL != pMsgNodeAPtr->psMsgMapPtr)
            {
                OCP_FREE(pMsgNodeAPtr->psMsgMapPtr);
                pMsgNodeAPtr->psMsgMapPtr = NULL;
            }
            if(NULL != pMsgNodeAPtr->psMsgHolder)
            {
                OCP_FREE(pMsgNodeAPtr->psMsgHolder);
                pMsgNodeAPtr->psMsgHolder = NULL;
            }
            pMsgNodeBPtr = pMsgNodeAPtr->psNext;
            OCP_FREE(pMsgNodeAPtr);
            pMsgNodeAPtr = pMsgNodeBPtr;

        }while(pMsgNodeBPtr != NULL);
        *PppsMsgListPtr = NULL;
    }while(0);
}

/**
 * Frees all flight node from flight list except the flight node of interest.<br>
 *
 * \param[in]        PbFlightID             Flight ID of the flight to be freed
 * \param[in,out]    PppsFlightHead         Pointer to beginning of flight list
 *
 */
_STATIC_H void DtlsHS_FreeFlightList(uint8_t PbFlightID, sFlightDetails_d** PppsFlightHead)
{
    sFlightDetails_d *pNodeToFreePtr = NULL;
    sFlightDetails_d *pCurrentNode = NULL, *pPreviousNode = NULL;


        do
        {
            if(NULL != PppsFlightHead)
            {
                pCurrentNode = *PppsFlightHead;
                pPreviousNode = pCurrentNode;

                if(NULL != pCurrentNode)
                {
                    do
                    {
                        if(FLIGHTID(pCurrentNode->wFlightDecp) != PbFlightID)
                        {
                            if(NULL != pCurrentNode->sFlightStats.psMessageList)
                            {
                                DtlsHS_FreeMessageList(&(pCurrentNode->sFlightStats.psMessageList));
                            }
                            pNodeToFreePtr = pCurrentNode;

                            if(pNodeToFreePtr == *PppsFlightHead)
                            {
                                *PppsFlightHead = pNodeToFreePtr->psNext;
                            }
                            else
                            {
                                pPreviousNode->psNext = pCurrentNode->psNext;
                            }
                            OCP_FREE(pNodeToFreePtr);
                            break;
                        }
                        pPreviousNode = pCurrentNode;
                        pCurrentNode = pCurrentNode->psNext;

                    }while(NULL != pCurrentNode);
                }
            }
        }while(NULL != pCurrentNode);
}

/**
 * Frees a flight node from the flight list which as two flight node at a time.<br>
 *
 * \param[in]       PbFlightID          Flight ID of the flight to be freed
 * \param[in,out]   PppsFlightHead       Pointer to beginning of flight list
 */
_STATIC_H void DtlsHS_FreeFlightNode(uint8_t PbFlightID, sFlightDetails_d** PppsFlightHead)
{
    sFlightDetails_d* pFlightTrav = NULL;
    sFlightDetails_d *pNodeToFreePtr = NULL;

    if((NULL != PppsFlightHead) && (NULL != *PppsFlightHead))
    {
        pFlightTrav = *PppsFlightHead;

        while(NULL != pFlightTrav)
        {
            if(FLIGHTID(pFlightTrav->wFlightDecp) == PbFlightID)
            {
                if(NULL != pFlightTrav->sFlightStats.psMessageList)
                {
                    DtlsHS_FreeMessageList(&(pFlightTrav->sFlightStats.psMessageList));
                }
                pNodeToFreePtr = pFlightTrav;
                if(pNodeToFreePtr == *PppsFlightHead)
                {
                    *PppsFlightHead = pNodeToFreePtr->psNext;
                }

                OCP_FREE(pNodeToFreePtr);
                break;
            }
            pFlightTrav = pFlightTrav->psNext;
        }
    }
}

/**
 * Receives a handshake message from the server.<br>
 * Under some erroneous conditions, error codes from Record Layer and Handshake Layer can also be returned.<br>
 *
 * \param[in]       PpbLastProcFlight           pointer to the last processed flight number
 * \param[in]       PppsRFlightHead             Flight head node for the receive message
 * \param[in,out]   PpsMessageLayer             Pointer to structure containing information required for Message Layer
 * \param[in]       PbFlightTimeout             Flight timeout value
 * \param[in]       PdwBasetime                 Time at which State changed to receive mode
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
_STATIC_H int32_t DtlsHS_ReceiveFlightMessage(uint8_t* PpbLastProcFlight, sFlightDetails_d** PppsRFlightHead,  sMsgLyr_d* PpsMessageLayer, uint8_t PbFlightTimeout,uint32_t PdwBasetime)
{
    int32_t i4Status = (int32_t)OCP_HL_OK;
    int32_t i4Alert ;
    uint32_t dwFragLen = 0;
    uint16_t wTotalMsgLen;
    sbBlob_d sMessage;
    uint8_t bRecvCCSRecord;
    sbBlob_d sBlobMessage;
    sFlightDetails_d *pRNextFlight;
    sFlightDetails_d *pRFlightTrav;

/// @cond hidden
#define B_MULTIPLERECORD (PpsMessageLayer->psConfigRL->sRL.bMultipleRecord)
/// @endcond

    do
    {
        //Assign buffer to store the received message from record layer interface
        sMessage.prgbStream = PpsMessageLayer->sTLMsg.prgbStream;
        sMessage.wLen = PpsMessageLayer->sTLMsg.wLen;

        do
        {
            PpsMessageLayer->psConfigRL->sRL.bContentType = CONTENTTYPE_HANDSHAKE;

            //Invoke Record Layer interface to get the message over UDP
            i4Status =  PpsMessageLayer->psConfigRL->pfRecv(&PpsMessageLayer->psConfigRL->sRL, sMessage.prgbStream, &sMessage.wLen);

            //Enter Flight Handler only if its a valid record
            if((int32_t)OCP_RL_OK == i4Status)
            {
                wTotalMsgLen = sMessage.wLen;

                sBlobMessage.prgbStream = sMessage.prgbStream;
                sBlobMessage.wLen = wTotalMsgLen;

                i4Status = DtlsHS_MsgCheck(*PpbLastProcFlight, &sBlobMessage, PpsMessageLayer);
                if(OCP_FL_OK != i4Status)
                {
                    i4Status = (int32_t)OCP_HL_IGNORE_RECORD;
                }

                if(OCP_FL_OK == i4Status)
                {
                    bRecvCCSRecord = PpsMessageLayer->psConfigRL->sRL.bRecvCCSRecord;

                    while(0 != wTotalMsgLen)
                    {
                        pRNextFlight = *PppsRFlightHead;

                        if(bRecvCCSRecord != CCS_RECORD_RECV)
                        {
                            dwFragLen = Utility_GetUint24(sBlobMessage.prgbStream + OFFSET_MSG_FRAG_LENGTH);

                            PpsMessageLayer->sMsg.wLen = (uint16_t)dwFragLen + LENGTH_MSG_HEADER;
                        }
                        else
                        {
                            PpsMessageLayer->sMsg.wLen = 0x01;
                        }

                        PpsMessageLayer->sMsg.prgbStream = sBlobMessage.prgbStream;

                        do
                        {
                            pRFlightTrav = pRNextFlight ;

                            //Invoke the Flight Handler
                            i4Status = pRFlightTrav->pFlightHndlr(*PpbLastProcFlight, &(pRFlightTrav->sFlightStats), PpsMessageLayer);
                            if((int32_t)OCP_FL_MSG_NOT_IN_FLIGHT != i4Status)
                            {
                                DtlsHS_FreeFlightList(FLIGHTID(pRFlightTrav->wFlightDecp), PppsRFlightHead);
                            }
                            pRNextFlight = pRFlightTrav->psNext;

                        }while(NULL != pRNextFlight);

                        //Error checks to be added
                        if(((int32_t)OCP_HL_MALLOC_FAILURE == i4Status) || ((int32_t)OCP_FL_MSG_MAXCOUNT == i4Status) || ((int32_t)OCP_HL_BUFFER_OVERFLOW == i4Status) ||
                            ((int32_t)OCP_FL_MALLOC_FAILURE == i4Status) || (((int32_t)OCP_FL_ERROR == i4Status)) || ((int32_t)OCP_FL_INT_ERROR == i4Status) || ((int32_t)OCP_FL_HS_ERROR == i4Status))
                        {
                            break;
                        }

                        if((((int32_t)DEV_ERROR_CODE_MASK & i4Status) == (int32_t)CMD_DEV_ERROR) || ((int32_t)OCP_FL_SEND_MSG_TO_OPTIGA_ERROR == i4Status))
                        {
                            break;
                        }

                        if(((int32_t)OCP_FL_OK == i4Status))
                        {
                            break;
                        }

                        if(bRecvCCSRecord != CCS_RECORD_RECV)
                        {
                            wTotalMsgLen -= (uint16_t)dwFragLen + LENGTH_MSG_HEADER;
                            if(wTotalMsgLen != 0x00)
                            {
                                sBlobMessage.prgbStream += dwFragLen + LENGTH_MSG_HEADER;
                            }
                        }
                        else
                        {
                            wTotalMsgLen-= PpsMessageLayer->sMsg.wLen;
                        }

                        sBlobMessage.wLen = wTotalMsgLen;
                    }
                }

                //Error checks to be added
                if(((int32_t)OCP_HL_MALLOC_FAILURE == i4Status) || ((int32_t)OCP_FL_MSG_MAXCOUNT == i4Status) || ((int32_t)OCP_HL_BUFFER_OVERFLOW == i4Status) ||
                    ((int32_t)OCP_FL_MALLOC_FAILURE == i4Status) || (((int32_t)OCP_FL_ERROR == i4Status)) || ((int32_t)OCP_FL_INT_ERROR == i4Status) || ((int32_t)OCP_FL_HS_ERROR == i4Status))
                {
                    break;
                }

                //Device error from Security Chip
                if((((int32_t)DEV_ERROR_CODE_MASK & i4Status) == (int32_t)CMD_DEV_ERROR) || ((int32_t)OCP_FL_SEND_MSG_TO_OPTIGA_ERROR == i4Status))
                {
                    break;
                }

                //If complete flight is received come out of the loop
                if((int32_t)OCP_FL_OK == i4Status)
                {
                    *PpbLastProcFlight = FLIGHTID((*PppsRFlightHead)->wFlightDecp);
                    B_MULTIPLERECORD = 0;
                    i4Status = (int32_t)OCP_HL_OK;
                    break;
                }
            }

            //Malloc failure
            if(((int32_t)OCP_RL_MALLOC_FAILURE == i4Status))
            {
                //Exit the state machine
                break;
            }

            //Alert record received
            if((int32_t)OCP_RL_ALERT_RECEIVED == i4Status)
            {
                i4Status = Alert_ProcessMsg(&sMessage,&i4Alert);
                if(((int32_t)OCP_AL_FATAL_ERROR == i4Alert))
                {
                    i4Status = i4Alert;
                    break;
                }
            }

            //Sequence overflow
            if((int32_t)OCP_RL_SEQUENCE_OVERFLOW == i4Status)
            {
                //Exit the state machine
                break;
            }

            if(((int32_t)OCP_RL_NO_DATA != i4Status) && ((int32_t)OCP_FL_RXING != i4Status))
            {
                i4Status = (int32_t)OCP_HL_IGNORE_RECORD;
            }

            //If timeout expired return timeout error and exit if flight status is not efreceived
            if(!TIMEELAPSED(PdwBasetime, PbFlightTimeout) && (((*PppsRFlightHead)->sFlightStats.bFlightState < (uint8_t)efReceived) || ((*PppsRFlightHead)->sFlightStats.bFlightState == (uint8_t)efReReceive)
                || ((*PppsRFlightHead)->sFlightStats.bFlightState == (uint8_t)efProcessed)))
            {
                i4Status = (int32_t)OCP_HL_TIMEOUT;
                break;
            }

            //Dynamically setting the UDP timeout
            PpsMessageLayer->psConfigRL->sRL.psConfigTL->sTL.wTimeout = (uint16_t)((PbFlightTimeout*1000) - (uint32_t)(pal_os_timer_get_time_in_milliseconds() - PdwBasetime));

        //If multiple record is received in a single datagram loop back and receive other records
        }while(0 != B_MULTIPLERECORD);
    }while(FALSE);

/// @cond hidden
#undef B_MULTIPLERECORD
/// @endcond
    return i4Status;
}

/**
 * Prepares Handshake message header<br>
 *
 * \param[in,out]   PpbMsgHeader        Pointer to buffer where handshake message header is formed
 * \param[in]       PpsMsgInfo          Pointer to #sMsgInfo_d
 *
 * \retval    #OCP_HL_OK                    Successful execution
\if ENABLE_NULL_CHECKS
 * \retval    #OCP_HL_NULL_PARAM            Null Parameters
\endif
 */
int32_t DtlsHS_PrepareMsgHeader(uint8_t* PpbMsgHeader, const sMsgInfo_d *PpsMsgInfo)
{
    int32_t i4Status = (int32_t)OCP_HL_NULL_PARAM;
    do
    {
#ifdef ENABLE_NULL_CHECKS
        if((NULL == PpbMsgHeader)||(NULL == PpsMsgInfo))
        {
            break;
        }
#endif
        *(PpbMsgHeader+OFFSET_MSG_TYPE) = PpsMsgInfo->bMsgType;
        Utility_SetUint24(PpbMsgHeader+OFFSET_MSG_TOTAL_LENGTH, PpsMsgInfo->dwMsgLength);
        Utility_SetUint16(PpbMsgHeader+OFFSET_MSG_SEQUENCE,PpsMsgInfo->wMsgSequence);
        Utility_SetUint24(PpbMsgHeader+OFFSET_MSG_FRAGMENT_OFFSET,(uint32_t)0x00);
        Utility_SetUint24(PpbMsgHeader+OFFSET_MSG_FRAG_LENGTH, PpsMsgInfo->dwMsgLength);

        i4Status = (int32_t)OCP_HL_OK;
    }while(FALSE);

    return i4Status;
}

/**
 * Frees flight node.<br>
 *
 * \param[in,out]   PppsFlightHead       Pointer to beginning of flight list
 *
 */
_STATIC_H void DtlsHS_ClearBuffer(sFlightDetails_d** PppsFlightHead)
{
    sFlightDetails_d* pFlightTrav = *PppsFlightHead;
    sFlightDetails_d *pNodeToFreePtr = NULL;

    do
    {
        if(NULL != pFlightTrav)
        {
            if(NULL != pFlightTrav->sFlightStats.psMessageList)
            {
                DtlsHS_FreeMessageList(&(pFlightTrav->sFlightStats.psMessageList));
            }
            pNodeToFreePtr = pFlightTrav;
            pFlightTrav = pFlightTrav->psNext;
            OCP_FREE(pNodeToFreePtr);
        }
    }while(NULL != pFlightTrav);
    *PppsFlightHead = NULL ;
}

/**
 * Checks a flight is in the list or not.<br>
 *
 * \param[in]   PpsFlightHead               Pointer to list of Flights
 * \param[in]   PbFlightID                  Flight number
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
_STATIC_H int32_t DtlsHS_CheckFlightList(sFlightDetails_d* PpsFlightHead, uint8_t PbFlightID)
{
    int32_t i4Status = (int32_t)OCP_HL_ERROR;
    sFlightDetails_d* psFlightList = PpsFlightHead;
    do
    {
        if(NULL == PpsFlightHead)
        {
            break;
        }
        do
        {
            if(FLIGHTID(psFlightList->wFlightDecp) == PbFlightID)
            {
                i4Status = (int32_t)OCP_HL_OK;
            }
            psFlightList = psFlightList->psNext;
        }while(NULL != psFlightList);
    }while(0);
    return i4Status;
}

/**
 * Appends a Flight Node to the end of the list.<br>
 *
 * \param[in,out]   PppsFlightHead              Pointer to Flight head node list
 * \param[in]       PpsFlightNode               Pointer to new Flight node
 *
 */
_STATIC_H void DtlsHS_CreateFlightList(sFlightDetails_d** PppsFlightHead, sFlightDetails_d* PpsFlightNode)
{
    sFlightDetails_d* psFlightTrav = *PppsFlightHead;
    do
    {
        if(NULL == psFlightTrav)
        {
            PpsFlightNode->psNext = NULL;
            *PppsFlightHead = PpsFlightNode;
        }
        else
        {
            while(psFlightTrav->psNext != NULL)
            {
                psFlightTrav = psFlightTrav->psNext;
            }
            psFlightTrav->psNext = PpsFlightNode;
            PpsFlightNode->psNext = NULL;
        }
    }while(0);
}

/**
 * Creates a flight node based on the last processed flight and inserts the node to the head node..<br>
 *
 * \param[in,out]   PbLastProcFlight            Last processed flight number
 * \param[in]       PppsFlightHead              Pointer to the flight head node
 * \param[in]       PpsMessageLayer             Pointer to message information
 *
 * \retval      #OCP_HL_OK              Successful Execution
 * \retval      #OCP_HL_MALLOC_FAILURE  Malloc failure
 * \retval      #OCP_HL_ERROR           Failure Execution
 */
_STATIC_H int32_t DtlsHS_CreateFlightNode(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_HL_OK;
    sFlightDetails_d* pFlightNode = NULL;

    do
    {
        pFlightNode = (sFlightDetails_d*)OCP_MALLOC(sizeof(sFlightDetails_d));
        if(NULL == pFlightNode)
        {
            i4Status = (int32_t)OCP_HL_MALLOC_FAILURE;
            break;
        }
        else
        {
            if((int32_t)OCP_FL_ERROR == DtlsHS_FlightNodeInit(pFlightNode, PbLastProcFlight))
            {
                OCP_FREE(pFlightNode);
                i4Status = (int32_t)OCP_HL_ERROR;
                break;
            }
            i4Status = pFlightNode->pFlightHndlr(PbLastProcFlight, &pFlightNode->sFlightStats, PpsMessageLayer);
            if(OCP_FL_OK != i4Status)
            {
                break;
            }
            DtlsHS_CreateFlightList(PppsFlightHead, pFlightNode);
            i4Status = (int32_t)OCP_HL_OK;
        }
    }while(0);

    return i4Status;

}

/**
 * Initialises the Flight List for Send.<br>
 *
 * \param[in]       PbLastProcFlight                Last processed flight ID
 * \param[in,out]   PppsFlightHead                  Pointer to list of Flight nodes
 * \param[in]       PpsMessageLayer                 Pointer to message information
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
_STATIC_H int32_t DtlsHS_SFlightInitialise(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer)
{
    sFlightDetails_d* pFlightNode = NULL;
    sFlightDetails_d* pFlightTrav ;
    int32_t i4Status = (int32_t)OCP_HL_OK;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        if((NULL == PppsFlightHead) || (NULL == PpsMessageLayer) ||(NULL == PpsMessageLayer->psConfigRL->pfSend))
        {
            i4Status = (int32_t)OCP_HL_NULL_PARAM;
            break;
        }
#endif
        pFlightTrav = *PppsFlightHead;

        if((NULL != pFlightTrav))
        {
            do
            {
                pFlightNode = pFlightTrav;
                pFlightTrav = pFlightTrav->psNext;

                if(PbLastProcFlight > FLIGHTID(pFlightNode->wFlightDecp))
                {
                    if((int32_t)OCP_FL_OK == pFlightNode->pFlightHndlr(PbLastProcFlight, &pFlightNode->sFlightStats, PpsMessageLayer))
                    {
                        if(efDone == (eFlightState_d)pFlightNode->sFlightStats.bFlightState)
                        {
                            DtlsHS_FreeFlightNode(FLIGHTID(pFlightNode->wFlightDecp), PppsFlightHead);
                        }
                    }
                }
                else if(PbLastProcFlight == FLIGHTID(pFlightNode->wFlightDecp))
                {
                    if(efTransmitted == (eFlightState_d)pFlightNode->sFlightStats.bFlightState)
                    {
                       pFlightNode->sFlightStats.bFlightState =  (uint8_t)efReTransmit;
                    }
                }
            }while(NULL != pFlightTrav);
        }

        //Create Flight List
        if(IsEVEN_FLIGHT(PbLastProcFlight))
        {
            if((int32_t)OCP_HL_ERROR == DtlsHS_CheckFlightList(*PppsFlightHead, PbLastProcFlight+1))
            {
                i4Status = DtlsHS_CreateFlightNode(PbLastProcFlight, PppsFlightHead, PpsMessageLayer);
                if(OCP_HL_OK != i4Status)
                {
                    break;
                }
            }
        }
    }while(0);
    return i4Status;
}

/**
 * Initialises the Flight List for Receive.<br>
 *
 * \param[in]       PbLastProcFlight                Last processed flight ID
 * \param[in,out]   PppsFlightHead                  Pointer to list of Flight nodes
 * \param[in]       PpsMessageLayer                 Pointer to message information
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
_STATIC_H int32_t DtlsHS_RFlightInitialise(uint8_t PbLastProcFlight, sFlightDetails_d** PppsFlightHead, sMsgLyr_d* PpsMessageLayer)
{
    uint8_t bFlightA = 0;
    uint8_t bFlightB = 0;
    int32_t i4Status = (int32_t)OCP_HL_OK;

    do
    {
        //Create Flight List
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
        else
        {
            i4Status = (int32_t)OCP_HL_ERROR;
            break;
        }
        if((int32_t)OCP_HL_ERROR == DtlsHS_CheckFlightList(*PppsFlightHead, bFlightA))
        {
            //CreateFlightNode expects last processed flight ID, hence flight corrected with offset 1
            i4Status = DtlsHS_CreateFlightNode(bFlightA-1, PppsFlightHead, PpsMessageLayer);
            if(OCP_HL_OK != i4Status)
            {
                break;
            }

            if((int32_t)OCP_HL_ERROR == DtlsHS_CheckFlightList(*PppsFlightHead, bFlightB))
            {
                //CreateFlightNode expects last processed flight ID, hence flight corrected with offset 1
                i4Status = DtlsHS_CreateFlightNode(bFlightB-1, PppsFlightHead, PpsMessageLayer);
                if(OCP_HL_OK != i4Status)
                {
                    break;
                }
            }
        }
        else if((uint8_t)efProcessed ==(*PppsFlightHead)->sFlightStats.bFlightState)
        {
            if((int32_t)OCP_HL_ERROR == DtlsHS_CheckFlightList(*PppsFlightHead, bFlightB))
            {
                //CreateFlightNode expects last processed flight ID, hence flight corrected with offset 1
                i4Status = DtlsHS_CreateFlightNode(bFlightB-1, PppsFlightHead, PpsMessageLayer);
                if(OCP_HL_OK != i4Status)
                {
                    break;
                }
            }
        }
    }while(0);
    return i4Status;
}

/**
 * Processes the send Flight.<br>
 *
 * \param[in]       PpbLastProcFlight               Pointer to last processed flight ID
 * \param[in,out]   PpsSFlightHead                  Pointer to list of Send Flight list
 * \param[in]       PpsMessageLayer                 Message layer information
 *
 * \retval      #OCP_HL_OK          Successful Execution
 * \retval      #OCP_HL_ERROR       Failure Execution
 * \retval      #OCP_FL_NOT_LISTED  Flight not found in the list
 */
_STATIC_H int32_t DtlsHS_SFlightProcess(uint8_t *PpbLastProcFlight, sFlightDetails_d* PpsSFlightHead, sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_HL_ERROR;
    sFlightDetails_d* pSFlightTrav = PpsSFlightHead;

    do
    {
        if(NULL == pSFlightTrav)
        {
            i4Status = (int32_t)OCP_FL_NOT_LISTED;
            break;
        }
        do
        {
            i4Status = pSFlightTrav->pFlightHndlr(*PpbLastProcFlight, &pSFlightTrav->sFlightStats, PpsMessageLayer);
            if((int32_t)OCP_FL_OK != i4Status)
            {
                break;
            }

            if((uint8_t)efTransmitted == pSFlightTrav->sFlightStats.bFlightState)
            {
                *PpbLastProcFlight = FLIGHTID(pSFlightTrav->wFlightDecp);
            }
            pSFlightTrav = pSFlightTrav->psNext;
        }while(NULL != pSFlightTrav);

        if((int32_t)OCP_FL_OK == i4Status)
        {
            i4Status = (int32_t)OCP_HL_OK;
        }
    }while(0);
    return i4Status;
}

/**
 * Processes the receive Flight.<br>
 * Under some erroneous conditions, error codes from respective layer can also be returned.<br>
 *
 * \param[in]    PpbLastProcFlight              pointer to the last processed flight ID
 * \param[in]    PppsRFlightHead                    Pointer to list of receivable Flight list
 * \param[in]    PpsMessageLayer                Message layer information
 * \param[in]    PbFlightTimeout                Flight time out value
 *
 * \retval      #OCP_HL_OK          Successful Execution
 * \retval      #OCP_HL_ERROR       Failure Execution
\if ENABLE_NULL_CHECKS
 * \retval      #OCP_HL_NULL_PARAM  NULL parameters
\endif
 */
_STATIC_H int32_t DtlsHS_RFlightProcess(uint8_t* PpbLastProcFlight, sFlightDetails_d** PppsRFlightHead,  sMsgLyr_d* PpsMessageLayer, uint8_t PbFlightTimeout)
{
    int32_t i4Status = (int32_t)OCP_HL_ERROR;
    uint32_t dwBasetime;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        //NULL check
        if((NULL == PppsRFlightHead) || (NULL == PpsMessageLayer) || (NULL == PpsMessageLayer->psConfigRL) || (NULL == PpsMessageLayer->psConfigRL->pfRecv))
        {
            i4Status = (int32_t)OCP_HL_NULL_PARAM;
            break;
        }
#endif
        //Start value for the Flight timeout
        dwBasetime = (uint32_t)pal_os_timer_get_time_in_milliseconds();

        do
        {
            i4Status = DtlsHS_ReceiveFlightMessage(PpbLastProcFlight, PppsRFlightHead, PpsMessageLayer, PbFlightTimeout, dwBasetime);

            //If timeout expired and complete flight is not received then return timeout error and come out of loop
            if((!TIMEELAPSED(dwBasetime, PbFlightTimeout) || ((int32_t)OCP_HL_TIMEOUT == i4Status)) &&    \
                  ((int32_t)OCP_HL_OK != i4Status) && (((*PppsRFlightHead)->sFlightStats.bFlightState < (uint8_t)efReceived) ||
                  ((*PppsRFlightHead)->sFlightStats.bFlightState == (uint8_t)efReReceive) || ((*PppsRFlightHead)->sFlightStats.bFlightState == (uint8_t)efProcessed)))
            {
                i4Status =  (int32_t)OCP_HL_TIMEOUT;
                break;
            }
        //If complete flight message is not received or if no data is received from record layer loop back to get remaining message
        }while((i4Status == (int32_t)OCP_FL_RXING) || (i4Status == (int32_t)OCP_RL_NO_DATA) || (i4Status == (int32_t)OCP_HL_IGNORE_RECORD));

    }while(FALSE);
    return i4Status;
}

/**
 * Sends Handshake message to the server.Fragments the message if the message is greater than PMTU.<br>
 * Under some erroneous conditions, error codes from respective Layer can also be returned. <br>
 *
 * \param[in]       PpsMsgPtr               pointer to the structure containing message information
 * \param[in]       PpsMessageLayer         Pointer to the structure containing list of message
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
int32_t DtlsHS_FSendMessage(const sMsgInfo_d* PpsMsgPtr, const sMsgLyr_d* PpsMessageLayer)
{
    int32_t i4Status = (int32_t)OCP_HL_ERROR;
    uint8_t* pbTotalFragMem = NULL;
    sFragmentMsg_d sFragmentMsg;
    sbBlob_d sMessage;
    sbBlob_d sbBlobMessage;
/// @cond hidden
#define CHANGE_CIPHERSPEC_MSGSIZE 1
/// @endcond
    do{
        if(PpsMsgPtr->bMsgType == (uint8_t)eChangeCipherSpec)
        {
            pbTotalFragMem = (uint8_t*)OCP_MALLOC(CHANGE_CIPHERSPEC_MSGSIZE + LENGTH_RL_HEADER);
            if(NULL == pbTotalFragMem)
            {
                i4Status = (int32_t)OCP_HL_MALLOC_FAILURE;
                break;
            }
            OCP_MEMCPY((pbTotalFragMem + LENGTH_RL_HEADER), PpsMsgPtr->psMsgHolder, PpsMsgPtr->dwMsgLength);


            PpsMessageLayer->psConfigRL->sRL.bContentType = CONTENTTYPE_CIPHER_SPEC;
            PpsMessageLayer->psConfigRL->sRL.bMemoryAllocated = TRUE;

            //Send Data
            i4Status = PpsMessageLayer->psConfigRL->pfSend(&PpsMessageLayer->psConfigRL->sRL,
                pbTotalFragMem,(uint16_t)(PpsMsgPtr->dwMsgLength + LENGTH_RL_HEADER));
            if(OCP_RL_OK != i4Status)
            {
                break;
            }
            i4Status = (int32_t)OCP_HL_OK;
        }
        else
        {
            //Assign pointer of the buffer where the message is buffered
            sbBlobMessage.prgbStream = PpsMsgPtr->psMsgHolder;
            sbBlobMessage.wLen = (uint16_t)PpsMsgPtr->dwMsgLength + OVERHEAD_LEN;

            //Move the pointer to the Handshake message header location
            sbBlobMessage.prgbStream+= (OVERHEAD_LEN - MSG_HEADER_LEN);
            sbBlobMessage.wLen-= (OVERHEAD_LEN - MSG_HEADER_LEN);

            //If the length of the message is within the given PMTU value
            if(sbBlobMessage.wLen <= PpsMessageLayer->wMaxPmtu - UDP_RECORD_OVERHEAD)
            {
                //Assign Buffer
                pbTotalFragMem = (uint8_t*)OCP_MALLOC(sbBlobMessage.wLen + LENGTH_RL_HEADER);
                if(NULL == pbTotalFragMem)
                {
                    i4Status = (int32_t)OCP_HL_MALLOC_FAILURE;
                    break;
                }

                Utility_Memmove(pbTotalFragMem + LENGTH_RL_HEADER, sbBlobMessage.prgbStream, sbBlobMessage.wLen);

                PpsMessageLayer->psConfigRL->sRL.bContentType = CONTENTTYPE_HANDSHAKE;
                PpsMessageLayer->psConfigRL->sRL.bMemoryAllocated = TRUE;
                //Send Data
                i4Status = PpsMessageLayer->psConfigRL->pfSend(&PpsMessageLayer->psConfigRL->sRL,
                    pbTotalFragMem,sbBlobMessage.wLen + LENGTH_RL_HEADER);
                if(OCP_RL_OK != i4Status)
                {
                    break;
                }
                i4Status = (int32_t)OCP_HL_OK;
                break;
            }
            else
            {
                sFragmentMsg.psCompleteMsg = &sbBlobMessage;
                sFragmentMsg.wFragmentSize = PpsMessageLayer->wMaxPmtu - UDP_RECORD_OVERHEAD;

                //Assign Buffer
                pbTotalFragMem = (uint8_t*)OCP_MALLOC(sFragmentMsg.wFragmentSize + LENGTH_RL_HEADER);
                if(NULL == pbTotalFragMem)
                {
                    i4Status = (int32_t)OCP_HL_MALLOC_FAILURE;
                    break;
                }
                sFragmentMsg.psMsgFrag = &sMessage;
                sMessage.prgbStream = pbTotalFragMem + LENGTH_RL_HEADER;
                sMessage.wLen = sFragmentMsg.wFragmentSize;
                sFragmentMsg.wRemainingLen = sbBlobMessage.wLen - LENGTH_MSG_HEADER;
                sFragmentMsg.wCurrentOffset = 0;

                do
                {
                    i4Status = (int32_t)OCP_HL_ERROR;
                    //Fragment the message
                    i4Status = DtlsHS_FragmentMsg(&sFragmentMsg);
                    if(OCP_HL_OK != i4Status)
                    {
                        break;
                    }

                    PpsMessageLayer->psConfigRL->sRL.bContentType = CONTENTTYPE_HANDSHAKE;
                    PpsMessageLayer->psConfigRL->sRL.bMemoryAllocated = TRUE;
                    //Send Data
                    i4Status = PpsMessageLayer->psConfigRL->pfSend(&PpsMessageLayer->psConfigRL->sRL,
                        pbTotalFragMem,sMessage.wLen + LENGTH_RL_HEADER);
                    if(OCP_RL_OK != i4Status)
                    {
                        break;
                    }
                    i4Status = (int32_t)OCP_HL_OK;
                }while(0 != sFragmentMsg.wRemainingLen);
            }
        }
    }while(FALSE);

    //Clear the message header list
    if(NULL != pbTotalFragMem)
    {
        OCP_FREE(pbTotalFragMem);
    }
/// @cond hidden
#undef CHANGE_CIPHERSPEC_MSGSIZE
/// @endcond
    return i4Status;
}

/**
 *
 * Process and validates Handshake message header.<br>
 *
 * \param[in]       PsBlobMessage      Pointer to a blob with Message including header.
 *
 * \retval    #OCP_HL_OK                    Successful execution
 * \retval    #OCP_HL_ERROR                 Failure in execution
 * \retval    #OCP_HL_LEN_MISMATCH          Fragment length mismatch
 * \retval    #OCP_HL_TOTALLEN_MISMATCH     Total length mismatch
 * \retval    #OCP_HL_BUFFER_OVERFLOW       Message length exceeded the limit
 *
 */
int32_t DtlsHS_ProcHeader(sbBlob_d PsBlobMessage)
{
    int32_t i4Status = (int32_t)OCP_HL_ERROR;
    uint32_t dwFragLen;
    uint32_t dwFragOffset;

    do
    {
        if(!((((uint8_t)eServer_Certificate <=*PsBlobMessage.prgbStream) && ((uint8_t)eServer_Hello_Done >= *PsBlobMessage.prgbStream)) ||
        ((uint8_t)eServer_Hello == *PsBlobMessage.prgbStream) || ((uint8_t)eHello_Verify_Request == *PsBlobMessage.prgbStream)||
        ((uint8_t)eServer_Finished == *PsBlobMessage.prgbStream)))
        {
            i4Status = (int32_t)OCP_HL_INVALID_MSGTYPE;
            break;
        }

        dwFragLen = Utility_GetUint24(PsBlobMessage.prgbStream + OFFSET_MSG_FRAG_LENGTH);

        //check if length fragment len + hmsg header is less than total stream length
        if((dwFragLen  + LENGTH_MSG_HEADER) > PsBlobMessage.wLen)
        {
            //Length indicated by fragment length not available
            i4Status = (int32_t)OCP_HL_LEN_MISMATCH;
            break;
        }

        //Fragment offset,3 bytes
        dwFragOffset = Utility_GetUint24(PsBlobMessage.prgbStream+OFFSET_MSG_FRAGMENT_OFFSET);

        if((dwFragOffset+ dwFragLen) > Utility_GetUint24(PsBlobMessage.prgbStream+OFFSET_MSG_TOTAL_LENGTH))
        {
            i4Status = (int32_t)OCP_HL_TOTALLEN_MISMATCH;
            break;
        }
        i4Status = (int32_t)OCP_HL_OK;
    }while(0);
    return i4Status;
}

/**
 *
 * Validates the Hello request message.<br>
 *
 * \param[in]       PprgbData               Pointer to the Hello Request Message.
 * \param[in]       PwLen                   Length of the Message.
 *
 * \retval    #OCP_HL_OK                    Successful execution
 * \retval    #OCP_HL_INVALID_HRMSG         Invalid Handshake message
 *
 */
int32_t DtlsHS_VerifyHR(const uint8_t* PprgbData, uint16_t PwLen)
{
    int32_t i4Status = (int32_t)OCP_HL_INVALID_HRMSG;
    uint32_t dwFragLen;
    uint32_t dwFragOffset;
    uint32_t dwTotalLen;

    do
    {
        if(0x00 != *PprgbData)
        {
            //Invalid Msg type
            break;
        }

        //Fragment Length,3 bytes
        dwFragLen = Utility_GetUint24(PprgbData + OFFSET_MSG_FRAG_LENGTH);

        //Fragment offset,3 bytes
        dwFragOffset = Utility_GetUint24(PprgbData + OFFSET_MSG_FRAGMENT_OFFSET);

        //Total Length,3 bytes
        dwTotalLen = Utility_GetUint24(PprgbData + OFFSET_MSG_TOTAL_LENGTH);

        if(PwLen != LENGTH_MSG_HEADER)
        {
            //Incorrect length
            break;
        }

        if(0x00 != Utility_GetUint24(PprgbData + OFFSET_MSG_SEQUENCE))
        {
            //Incorrect Msg sequence
            break;
        }

        //check if length fragment len + fragment 0ffset + total len is less than total stream length
        if((dwFragLen  + dwFragOffset + dwTotalLen) > 0x00)
        {
            //Length fields indicated in the message are incorrect
            break;
        }

        i4Status = (int32_t)OCP_HL_OK;
    }while(0);
    return i4Status;
}

/**
 * Performs a DTLS handshake.<br>
 * The state machine is configurable as a client or as a server based on the selected protocol.Currently server configuration is not supported.<br>
 *
 * \param[in,out]   PphHandshake                Pointer to structure containing data to perform handshake
 *
 * \retval      #OCP_HL_OK      Successful Execution
 * \retval      #OCP_HL_ERROR   Failure Execution
 */
int32_t DtlsHS_Handshake(sHandshake_d* PphHandshake)
{
/// @cond hidden
    #define STATE_SEND      0x11
    #define STATE_RECV      0x22
    #define STATE_EXIT      0x33
/// @endcond

    uint8_t bLastProcFlight=0;
    uint8_t bSmMode = STATE_RECV;
    uint8_t bIndex;
    uint8_t bFlightTimeout = DEFAULT_TIMEOUT;
    sFlightDetails_d* pSFlightHead=NULL;
    sFlightDetails_d* pRFlightHead=NULL;
    sMsgLyr_d sMessageLayer;
    int32_t i4Status = (int32_t)OCP_HL_ERROR;

    if(eClient == PphHandshake->eMode)
    {
        bSmMode = STATE_SEND;
    }
    else
    {
        bSmMode = STATE_EXIT;
    }

    //Populate structure to be passed to MessageLayer
    sMessageLayer.psConfigRL = PphHandshake->psConfigRL;
    sMessageLayer.wSessionID = PphHandshake->wSessionOID;
    ((sRecordLayer_d*)PphHandshake->psConfigRL->sRL.phRLHdl)->wSessionKeyOID = PphHandshake->wSessionOID;
    sMessageLayer.wMaxPmtu = PphHandshake->wMaxPmtu;
    sMessageLayer.wOIDDevCertificate = PphHandshake->wOIDDevCertificate;
    sMessageLayer.pfGetUnixTIme = PphHandshake->pfGetUnixTIme;
    sMessageLayer.eFlight = eFlight0;
    sMessageLayer.dwRMsgSeqNum = 0xFFFFFFFF;
    sMessageLayer.sTLMsg.prgbStream = (uint8_t*)OCP_MALLOC(TLBUFFER_SIZE);
    if(NULL == sMessageLayer.sTLMsg.prgbStream)
    {
        i4Status = (int32_t)OCP_LIB_MALLOC_FAILURE;
        bSmMode = STATE_EXIT;
    }
    sMessageLayer.sTLMsg.wLen = (uint16_t)TLBUFFER_SIZE;

    for(bIndex = 0; bIndex < (sizeof(sMessageLayer.rgbOptMsgList)/sizeof(sMessageLayer.rgbOptMsgList[0])); bIndex++)
    {
        sMessageLayer.rgbOptMsgList[bIndex] = 0xFF;
    }

    //Start state machine
    do
    {
        switch(bSmMode)
        {
            case STATE_SEND:
            {
                i4Status = SEND_FLIGHT_INITIALIZE(bLastProcFlight, &pSFlightHead, &sMessageLayer);
                if((int32_t)OCP_HL_OK != i4Status)
                {
                    if(PphHandshake->eAuthState == eAuthStarted)
                    {
                        PphHandshake->fFatalError = TRUE;
                        SEND_ALERT(sMessageLayer.psConfigRL, i4Status);
                    }
                    DtlsHS_ClearBuffer(&pRFlightHead);
                    DtlsHS_ClearBuffer(&pSFlightHead);
                    bSmMode = STATE_EXIT;
                    break;
                }

                i4Status = SEND_FLIGHT_PROCESS(&bLastProcFlight, pSFlightHead, &sMessageLayer);
                if(OCP_HL_OK == i4Status)
                {
                    if(PphHandshake->eAuthState == eAuthInitialised)
                    {
                        PphHandshake->eAuthState = eAuthStarted;
                    }
                    bSmMode = STATE_RECV;
                }
                else
                {
                    if(PphHandshake->eAuthState == eAuthStarted)
                    {
                        PphHandshake->fFatalError = TRUE;
                        SEND_ALERT(sMessageLayer.psConfigRL, i4Status);
                    }
                    bSmMode =  STATE_EXIT;
                    DtlsHS_ClearBuffer(&pRFlightHead);
                    DtlsHS_ClearBuffer(&pSFlightHead);
                }
                break;
            }
            case STATE_RECV:
            {
                i4Status = REC_FLIGHT_INITIALIZE(bLastProcFlight, &pRFlightHead, &sMessageLayer);
                if((int32_t)OCP_HL_OK != i4Status)
                {
                    PphHandshake->fFatalError = TRUE;
                    DtlsHS_ClearBuffer(&pRFlightHead);
                    DtlsHS_ClearBuffer(&pSFlightHead);
                    SEND_ALERT(sMessageLayer.psConfigRL, i4Status);
                    bSmMode =  STATE_EXIT;
                    break;
                }

                i4Status = REC_FLIGHT_PROCESS(&bLastProcFlight, &pRFlightHead, &sMessageLayer, bFlightTimeout);

                if ((int32_t)OCP_HL_TIMEOUT == i4Status)
                {
                    bFlightTimeout = ((bFlightTimeout*2) == 64)?(uint8_t)MAX_FLIGHT_TIMEOUT: (uint8_t)(bFlightTimeout*2);
                    //Check for Maximum Flight timeout value
                    if(bFlightTimeout > MAX_FLIGHT_TIMEOUT)
                    {
                        PphHandshake->fFatalError = FALSE;
                        DtlsHS_ClearBuffer(&pRFlightHead);
                        DtlsHS_ClearBuffer(&pSFlightHead);
                        bSmMode =  STATE_EXIT;
                        break;
                    }
                    sMessageLayer.psConfigRL->sRL.psConfigTL->sTL.wTimeout = (uint16_t)(bFlightTimeout * 1000);
                    bSmMode = STATE_SEND;
                }
                //Fatal Alert received
                else if((int32_t)OCP_AL_FATAL_ERROR == i4Status)
                {
                    PphHandshake->fFatalError = FALSE;
                    DtlsHS_ClearBuffer(&pRFlightHead);
                    DtlsHS_ClearBuffer(&pSFlightHead);
                    bSmMode = STATE_EXIT;
                }
                else if(OCP_HL_OK != i4Status)
                {
                    PphHandshake->fFatalError = TRUE;
                    SEND_ALERT(sMessageLayer.psConfigRL, i4Status);
                    DtlsHS_ClearBuffer(&pRFlightHead);
                    DtlsHS_ClearBuffer(&pSFlightHead);
                    bSmMode =  STATE_EXIT;
                }
                else if(bLastProcFlight != (uint8_t)eFlight6)
                {
                    bFlightTimeout = DEFAULT_TIMEOUT;
                    //Initial UDP Time out
                    sMessageLayer.psConfigRL->sRL.psConfigTL->sTL.wTimeout = 200;
                    Dtls_SlideWindow(&sMessageLayer.psConfigRL->sRL, PphHandshake->eAuthState);
                    bSmMode = STATE_SEND;
                }
                else
                {
                    //state machine is over
                    PphHandshake->eAuthState = eAuthCompleted;
                    Dtls_SlideWindow(&sMessageLayer.psConfigRL->sRL, PphHandshake->eAuthState);
                    PphHandshake->fFatalError = FALSE;
                    bSmMode = STATE_EXIT;
                    DtlsHS_ClearBuffer(&pRFlightHead);
                    DtlsHS_ClearBuffer(&pSFlightHead);
                }
                break;
            }
            default:
            {
                PphHandshake->fFatalError = TRUE;
                bSmMode = STATE_EXIT;
            }
            break;
        }
    }while(STATE_EXIT != bSmMode);

/// @cond hidden
    #undef STATE_SEND
    #undef STATE_RECV
    #undef STATE_EXIT
/// @endcond

    if(sMessageLayer.sTLMsg.prgbStream != NULL)
    {
        OCP_FREE(sMessageLayer.sTLMsg.prgbStream);
    }
    return i4Status;
}

/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
