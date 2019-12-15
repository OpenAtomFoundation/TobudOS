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
* \file DtlsFlighthandler.h
*
* \brief   This file defines APIs, types and data structures used in the
*          DTLS Flight handlers implementation.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef __DTLSFH_H__
#define __DTLSFH_H__

#include "optiga/dtls/MessageLayer.h"
#include "optiga/dtls/DtlsHandshakeProtocol.h"
#include "optiga/dtls/AlertProtocol.h"
#include "optiga/optiga_dtls.h"
#include "optiga/dtls/DtlsRecordLayer.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH
/// @cond hidden
#define DEV_ERROR_CODE_MASK 0xFFFFFF00
#define DEV_ERROR_LSBCODE_MASK 0xFF
/// @endcond

/****************************************************************************
 *
 * API Error Codes
 *
 ****************************************************************************/

/// Successful execution
#define OCP_FL_OK                           0x7569412

/// Failure in execution
#define OCP_FL_ERROR                        0xF4358417

///Null Parameter(s)
#define OCP_FL_NULL_PARAM                   BASE_ERROR_FLIGHT

///Flight not listed
#define OCP_FL_NOT_LISTED                   (OCP_FL_NULL_PARAM + 1)

///State of Flight is old
#define OCP_FL_STATE_OLD                    (OCP_FL_NULL_PARAM + 2)

///Message with invalid sequence number
#define OCP_FL_INVALID_MSG_SEQNUM           (OCP_FL_NULL_PARAM + 3)

///Message doesn't belong to a flight
#define OCP_FL_MSG_NOT_IN_FLIGHT            (OCP_FL_NULL_PARAM + 4)

///Message sequence number is not in expected sequence
#define OCP_FL_MSG_OPT_MSG_ERROR            (OCP_FL_NULL_PARAM + 5)

///Message received again
#define OCP_MSG_RXAGAIN                     (OCP_FL_NULL_PARAM + 6)

///Flight messages being received
#define OCP_FL_RXING                        (OCP_FL_NULL_PARAM + 7)

///Flight processing is past the current flight
#define OCP_FL_DONE                         (OCP_FL_NULL_PARAM + 8)

///Node for a message is not available in the list
#define OCP_FL_MSG_NODE_NOT_AVAIL           (OCP_FL_NULL_PARAM + 9)

///Node for a message is available in the list
#define OCP_FL_MSG_NODE_AVAIL               (OCP_FL_NULL_PARAM + 10)

///Message is completely received
#define OCP_FL_MSG_RECEIVED                 (OCP_FL_NULL_PARAM + 11)

///Message completion check error
#define OCP_FL_MSG_ERROR                    (OCP_FL_NULL_PARAM + 12)

///Malloc failure
#define OCP_FL_MALLOC_FAILURE               (OCP_FL_NULL_PARAM + 13)

///Message sending to Security chip failed
#define OCP_FL_SEND_MSG_TO_OPTIGA_ERROR     (OCP_FL_NULL_PARAM + 14)

///Sending message to server failed
#define OCP_FL_FLIGHTSEND_ERROR             (OCP_FL_NULL_PARAM + 15)

///Getting a message from Security chip failed
#define OCP_FL_GET_MSG_FROM_OPTIGA_ERROR        (OCP_FL_NULL_PARAM + 16)

///Message Incomplete error
#define OCP_FL_MSG_INCOMPLETE               (OCP_FL_NULL_PARAM + 17)

///Message with invalid length
#define OCP_FL_INVALID_MSG_LENGTH           (OCP_FL_NULL_PARAM + 18)

///Message received is more than max count
#define OCP_FL_MSG_MAXCOUNT                 (OCP_FL_NULL_PARAM + 19)

///Message with invalid sequence
#define OCP_FL_INVALID_MSG_SEQ              (OCP_FL_NULL_PARAM + 20)

///Message with invalid LENGTH
#define OCP_FL_INVALID_PROCFLIGHT           (OCP_FL_NULL_PARAM + 21)

///Message with invalid message type
#define OCP_FL_MSG_NOT_LISTED               (OCP_FL_NULL_PARAM + 22)

///Message to be ignored
#define OCP_FL_MSG_IGNORE                   (OCP_FL_NULL_PARAM + 23)

///Internal Error
#define OCP_FL_INT_ERROR                    (OCP_FL_NULL_PARAM + 24)

///Invalid Handshake Error
#define OCP_FL_HS_ERROR                     (OCP_FL_NULL_PARAM + 25)

/**
 * \brief Checks whether the received message belongs to the expected flight.<br>
 */
int32_t DtlsHS_MsgCheck(uint8_t PbLastProcFlight, const sbBlob_d* PpsBlobMessage, const sMsgLyr_d* PpsMessageLayer);

/**
 * \brief  Initializes flight node.
 */
int32_t DtlsHS_FlightNodeInit(sFlightDetails_d* PpsFlightNode, uint8_t PbLastProcFlight);

/**
 * \brief  Gets the Flight type for the corresponding message type.<br>
 */
int32_t DtlsHS_GetFlightID(uint8_t PbMsgType, uint8_t* PpFlightID);

/**
 * \brief  Searches the look-up table and returns the message descriptors of a flight.<br>
 */
void DtlsHS_GetFlightMsgInfo(uint8_t PeFlightID, uint16_t** PpwMessageList);

/**
 * \brief  Validates the sequence number of message/ fragment received of flight 2.<br>
 */
int32_t DtlsHS_Flight2CheckMsgSeqNum(uint8_t PbRxMsgID, uint16_t PwRxMsgSeqNum, const sMsgInfo_d *PpsMessageList);

/**
 * \brief  Flight one handler to process flight 1 messages.
 */
int32_t DtlsHS_Flight1Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpThisFlight, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief  Flight two handler to process flight 2 messages.
 */
int32_t DtlsHS_Flight2Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpThisFlight, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief  Flight three handler to process flight 3 messages.
 */
int32_t DtlsHS_Flight3Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpThisFlight, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief  Flight four handler to process flight 4 messages.
 */
int32_t DtlsHS_Flight4Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpThisFlight, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief  Flight five handler to process flight 5 messages.
 */
int32_t DtlsHS_Flight5Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpThisFlight, sMsgLyr_d* PpsMessageLayer);

/**
 * \brief  Flight six handler to process flight 6 messages.
 */
int32_t DtlsHS_Flight6Handler(uint8_t PbLastProcFlight, sFlightStats_d* PpThisFlight, sMsgLyr_d* PpsMessageLayer);

#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH*/

#endif //__DTLSFH_H__
/**
* @}
*/
