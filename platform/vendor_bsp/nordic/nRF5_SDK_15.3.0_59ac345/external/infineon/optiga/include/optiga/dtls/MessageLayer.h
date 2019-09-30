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
*          (D)TLS Message Layer implementation.
*
* \addtogroup  grMutualAuth
* @{
*/
#ifndef __MSGLAYER_H__
#define __MSGLAYER_H__

#include <stdio.h>
#include "optiga/dtls/DtlsHandshakeProtocol.h"
#include "optiga/common/Datatypes.h"
#include "optiga/dtls/OcpCommon.h"
#include "optiga/cmd/CommandLib.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/****************************************************************************
 *
 * API Error Codes
 *
 ****************************************************************************/

/// Successful execution
#define OCP_ML_OK                           0x75268512

/// Failure in execution
#define OCP_ML_ERROR                        0xF4350158

///Null Parameter(s)
#define OCP_ML_NULL_PARAM                   BASE_ERROR_MESSAGELAYER

///Zero Length input parameter(s)
#define OCP_ML_ZERO_LEN                     (BASE_ERROR_MESSAGELAYER + 1)

///Insufficient memory
#define OCP_ML_INSUFFICIENT_MEMORY          (BASE_ERROR_MESSAGELAYER + 2)

///Invalid unix time
#define OCP_ML_INVALID_UNIXTIME             (BASE_ERROR_MESSAGELAYER + 3)

///Invalid certificate type
#define OCP_ML_INVALID_CERTTYPE             (BASE_ERROR_MESSAGELAYER + 4)

///Invalid certificate type
#define OCP_ML_MALLOC_FAILURE               (BASE_ERROR_MESSAGELAYER + 5)

/****************************************************************************
 *
 * Definitions related to (D)TLS commands.
 *
 ****************************************************************************/
/// @cond hidden

///Timeout expired
#define OCP_ML_TIMEOUT                      (OCP_ML_NULL_PARAM + 3)

///Length of Handshake message header
#define MSG_HEADER_LEN                      12

///Over head length for command library
#define OVERHEAD_LEN                        21                     //APDU (4) + Message header len(12) + Tag enconding len(5)

//Macro to validate the time out
#define TIMEELAPSED(bStartTime,bTimeout)    (((uint32_t)(pal_os_timer_get_time_in_milliseconds() - bStartTime) < (uint32_t)(bTimeout*1000))?TRUE:FALSE)
/// @endcond

/****************************************************************************
 *
 * Common data structures used across all functions.
 *
 ****************************************************************************/

/**
 * \brief  Structure to hold the information required for Get message call back to allocate memory.
 */
typedef struct sCBGetMsg_d
{
    ///Indicate to allocate memory only for first time
    uint8_t bRepeatCall;
    ///Pointer to the allocated memory where the message is stored
    uint8_t* pbActualMsg;
    ///Size of the allocated memory
    uint16_t dwMsgLen;

}sCBGetMsg_d;

/**
 * \brief  Structure to hold the information required for Message Layer.
 */
typedef struct sMessageLayer_d
{
    ///(D)TLS Session ID
    uint16_t wSessionID;
    ///Maximum PMTU
    uint16_t wMaxPmtu;
    ///Record Layer
    sConfigRL_d* psConfigRL;
    ///Certificate Type used for Authentication
    uint16_t  wOIDDevCertificate;
    ///Callback function pointer to get unixtime
    fGetUnixTime_d pfGetUnixTIme;
} sMessageLayer_d;

/**
 * \brief Provide Handshake message using Command Library.<br>
 */
int32_t MsgLayer_FormMessage(eMsgType_d eMsgType,const sMessageLayer_d* PpsMessageLayer, sbBlob_d* PpsMessage);

/**
 * \brief Process Handshake message using Command Library.<br>
 */
int32_t MsgLayer_ProcessMessage(eMsgType_d eMsgType,const sMessageLayer_d* PpsMessageLayer, sbBlob_d* PpsMessage);

/**
 * \brief Encrypt the Data using Command Library.<br>
 */
int32_t MsgLayer_EncryptMessage(const sbBlob_d* PpsBlobPlainMsg, sbBlob_d* PpsBlobCipherMsg, uint16_t PwInDataLen, uint16_t PwSessionKeyOID);

/**
 * \brief Decrypt the Data using Command Library.<br>
 */
int32_t MsgLayer_DecryptMessage(const sbBlob_d* PpsBlobCipherMsg, sbBlob_d* PpsBlobPlainMsg, uint16_t PwInDataLen, uint16_t PwSessionKeyOID);

#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH*/

#endif //__MSGLAYER_H__
/**
* @}
*/
