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
* \file
*
* \brief   This file defines APIs, types and data structures used in the
*          DTLS Handshake protocol.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef __DTLSHS_H__
#define __DTLSHS_H__

#include "optiga/common/Util.h"
#include "optiga/dtls/OcpCommon.h"
#include "optiga/dtls/MessageLayer.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH
/****************************************************************************
 *
 * API Error Codes
 *
 ****************************************************************************/
///Invalid Fragment size
#define OCP_HL_INVALID_FRAGMENT_SIZE    (BASE_ERROR_HANDSHAKELAYER + 2)

///Invalid Current offset
#define OCP_HL_INVALID_OFFSET_LEN       (BASE_ERROR_HANDSHAKELAYER + 3)

///Insufficient memory
#define OCP_HL_INSUFFICIENT_MEMORY      (BASE_ERROR_HANDSHAKELAYER + 4)

///Invalid Length
#define OCP_HL_INVALID_LENGTH           (BASE_ERROR_HANDSHAKELAYER + 5)

///Fragment Length mismatch
#define OCP_HL_LEN_MISMATCH             (BASE_ERROR_HANDSHAKELAYER + 6)

///Total Length mismatch
#define OCP_HL_TOTALLEN_MISMATCH        (BASE_ERROR_HANDSHAKELAYER + 7)

///Invalid message type
#define OCP_HL_INVALID_MSGTYPE          (BASE_ERROR_HANDSHAKELAYER + 8)

///Previous flight message received
#define OCP_HL_RETRANSMISSION           (BASE_ERROR_HANDSHAKELAYER + 9)

///Future flight message received
#define OCP_HL_CONTINUE                 (BASE_ERROR_HANDSHAKELAYER + 10)

///Future flight message received
#define OCP_HL_TIMEOUT                  (BASE_ERROR_HANDSHAKELAYER + 11)

///Malloc failure
#define OCP_HL_MALLOC_FAILURE           (BASE_ERROR_HANDSHAKELAYER + 12)

///Ignore the Record
#define OCP_HL_IGNORE_RECORD            (BASE_ERROR_HANDSHAKELAYER + 13)

///Buffer overflow
#define OCP_HL_BUFFER_OVERFLOW          (BASE_ERROR_HANDSHAKELAYER + 14)

///Invalid sequence
#define OCP_HL_INVALID_SEQUENCE         (BASE_ERROR_HANDSHAKELAYER + 15)

///Invalid Hello request message
#define OCP_HL_INVALID_HRMSG            (BASE_ERROR_HANDSHAKELAYER + 16)


/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/
///Maximum value of message type
#define MAX_MSG_TYPE_VALUE              20

/**
 * \brief Structure to hold fragmentation data
 */
typedef struct sFragmentMsg_d
{
    ///Complete Message
    sbBlob_d* psCompleteMsg;
    ///Fragment Size
    uint16_t wFragmentSize;
    ///Message fragment
    sbBlob_d* psMsgFrag;
    ///Current offset in complete message
    uint16_t wCurrentOffset;
    ///Remaining length to be fragmented
    uint16_t wRemainingLen;
}sFragmentMsg_d;

/**
 * \brief Structure to hold header information
 */
typedef struct sMsgHeader_d
{
    ///Specifies message type
    uint8_t bMsgType;
    ///Total length of the handshake message
    uint32_t dwTotalLength;
    ///Message sequence
    uint16_t wMsgSequence;
    ///Specifies the offset of fragment data in actual message
    uint32_t dwFragmentOffset;
    ///Message fragment
    uint32_t dwFragmentLength;
    ///Flight to which the message belongs
    eFlight_d eFlight;
    ///Indicate memory is already Allocated
    bool_t fMemoryAllocated;
}sMsgHeader_d;

/**
 * \brief Status of Message in the buffer
 */
typedef enum eMsgState_d
{
    ///Message is partial
    ePartial = 0x01,
    ///Message is complete
    eComplete = 0x02,
    //Message is processed/returned
    eProcessed = 0x03,
    //Message receiving again
    eReReceiving = 0x04
}eMsgState_d;

/**
 * \brief Enum values for server message type
 */
typedef enum eServerMessage_d
{
    //Server Side Message type
    ///Server Hello message
    eServer_Hello = 2,
    ///Hello Verify Request
    eHello_Verify_Request = 3,
    ///Server request message
    eServer_Certificate = 11,
    ///Server key Exchange
    eServer_Key_Exchange = 12,
    ///Certificate request message
    eServer_Certificate_Request = 13,
    ///Server hello done message
    eServer_Hello_Done = 14,
    ///Server finished message
    eServer_Finished = 20
}eServerMessage_d;

/**
 * \brief This structure maps message type to the flight number
 */
typedef struct sFlightMapTable_d
{
    ///Message type
    uint8_t bMsgType;
    ///Flight number
    eFlight_d FlightNum;
}sFlightMapTable_d;

/// MAcro to update the flight or message state
#define UPDATE_FSTATE(X,Y) (X=Y)

/**
 * \brief Structure to hold message fragment in buffer
 */
typedef struct sMsg_d
{
    ///Message type
    uint8_t bMsgType;
    ///Message sequence
    uint16_t wMsgSequence;
    ///Complete Message Holder
    uint8_t* psMsgHolder;
    //Message Length
    uint32_t dwMsgLength;
    //Fragments info
    uint8_t* psMsgMapPtr;
    ///State of the Message
    eMsgState_d eMsgState;
    ///Max Msg reception count
    uint8_t bMsgCount;
    ///Pointer to next message
    struct sMsg_d* psNext;
}sMsgInfo_d;

/**
 * \brief Flight States
 */
typedef enum eFlightState_d
{
    ///Initialize state where flight node is created
    efInit = 0,
    ///State where message will be receiving or sent to/from Host
    efReady,
    ///State where flight is completely received and buffered
    efReceived,
    ///State where flight is received again
    efReReceive,
    ///Message is processed by security chip
    efProcessed,
    ///Message is formed by security chip and sent out
    efTransmitted,
    ///Message is retransmitted
    efReTransmit,
    ///Completion of flight handling
    efDone
}eFlightState_d;

/**
 * \brief Enum values for message type in the flight
 */
typedef enum eOptionalFlag_d
{
    ///Mandatory message in the flight
    eMandatory = 0,

    ///Optional message in the flight
    eOptional = 1,
}eOptionalFlag_d;

/**
 * \brief Structure to hold the Flight status
 */
typedef struct sFlightStats_d
{
    ///Current Flight state
    uint8_t bFlightState;
    ///Pointer to structure #sMsgInfo_d
    sMsgInfo_d *psMessageList;
}sFlightStats_d;

/**
 * \brief  Structure to hold the information required for Message Layer.
 */
typedef struct sMsgLyr_d
{
    ///Optional Message List
    uint8_t rgbOptMsgList[5];
    ///(D)TLS Session ID
    uint16_t wSessionID;
    ///Maximum PMTU
    uint16_t wMaxPmtu;
    ///Receive Message Sequence Number
    uint32_t dwRMsgSeqNum;
    ///Record Layer
    sConfigRL_d* psConfigRL;
    ///Certificate Type used for Authentication
    uint16_t  wOIDDevCertificate;
    ///Callback function pointer to get unixtime
    fGetUnixTime_d pfGetUnixTIme;
    ///Message/Fragment
    sbBlob_d sMsg;
    ///Message/Fragment from the transport layer
    sbBlob_d sTLMsg;
    ///Flight received
    eFlight_d eFlight;
} sMsgLyr_d;


///Macro to check for mandatory message
#define IsMandatory(x) ((x & 0xFF00) ? 0: 1)
///Macro to check for optional message
#define IsOptional(x)  ((x & 0xFF00) ? 1: 0)
///Macro to get the Flight number
#define FLIGHTID(x)     (x & 0xFF)


/**
 * \brief  Flight handler function pointer
 */
typedef int32_t (*fFlightHandler)(uint8_t PbLastProcFlight, sFlightStats_d* PpThisFlight, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief  DTLS Spec reference table
 */
typedef struct sFlightTable_d
{
    ///Lower byte - Flight ID, Upper byte - eOptional/ eMandatory
    uint16_t wFlightDesc;
    ///Lower byte - Message ID, Upper byte - eOptional/ eMandatory
    uint16_t wMsgTypes[6];
    /// Flight Handler
    fFlightHandler pFlightHndlr;
}sFlightTable_d;

/**
 * \brief  Structure to hold information of a Flight
 */
typedef struct sFlightDetails_d
{
    ///Flight Descriptor
    uint16_t wFlightDecp;
    ///Flight handler
    fFlightHandler pFlightHndlr;
    ///Structure containing flight status
    sFlightStats_d sFlightStats;
    ///Pointer to the next flight node
    struct sFlightDetails_d* psNext;
}sFlightDetails_d;

///Table to map number of msg in a send flight and its flight handler
extern const sFlightTable_d rgsSFlightInfo[];

///Table to map number of msg in a send flight and its flight handler
extern const sFlightTable_d rgsRFlightInfo[];

/****************************************************************************
 *
 * Definitions related to (D)TLS commands.
 *
 ****************************************************************************/
/**
 * \brief Prepares Handshake message header
 */
int32_t DtlsHS_PrepareMsgHeader(uint8_t* PpbMsgHeader, const sMsgInfo_d *sMsgInfo);

/**
 * \brief Performs (D)TLS handshake
 */
int32_t DtlsHS_Handshake(sHandshake_d* PphHandshake);

/**
 * \brief Sends a message to the server.
 */
int32_t DtlsHS_FSendMessage(const sMsgInfo_d* PpsMsgPtr, const sMsgLyr_d* PpsMessageLayer);

/**
 * \brief Process and validates Handshake message header.<br>
 */
int32_t DtlsHS_ProcHeader(sbBlob_d PsBlobMessage);

#endif /*  MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
#endif //__DTLSHS_H__
/**
* @}
*/

