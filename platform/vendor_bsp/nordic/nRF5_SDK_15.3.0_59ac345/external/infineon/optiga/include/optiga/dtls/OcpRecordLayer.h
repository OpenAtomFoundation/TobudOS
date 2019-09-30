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
*          OCP Record Layer.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef __OCPRECORDLAYER_H__
#define __OCPRECORDLAYER_H__

#include "optiga/dtls/OcpTransportLayer.h"
#include "optiga/dtls/OcpCryptoLayer.h"
#include "optiga/common/Logger.h"


/// Failure in execution
#define OCP_RL_ERROR                     0x55667F82

/// Internal status for success.
#define OCP_RL_OK                        0x297D9015

///Invalid record length
#define OCP_RL_INVALID_RECORD_LENGTH     BASE_ERROR_RECORDLAYER

///Invalid record content type
#define OCP_RL_INVALID_CONTENTTYPE       (BASE_ERROR_RECORDLAYER + 1)

///Invalid decrypt payload lenght
#define OCP_RL_INVALID_DEC_PAYLOAD_LEN   (BASE_ERROR_RECORDLAYER + 2)

///Invalid protocol version
#define OCP_RL_INVALID_PROTOCOL_VERSION  (BASE_ERROR_RECORDLAYER + 3)

///Incorrect epoch
#define OCP_RL_INCORRECT_EPOCH           (BASE_ERROR_RECORDLAYER + 4)

///Record length mismatch
#define OCP_RL_RECORD_LEN_MISMATCH       (BASE_ERROR_RECORDLAYER + 5)

///Bad record
#define OCP_RL_BAD_RECORD                (BASE_ERROR_RECORDLAYER + 6)

/// No data
#define OCP_RL_NO_DATA                   (BASE_ERROR_RECORDLAYER + 7)

/// Data length is greater than PMTU
#define OCP_RL_LEN_GREATER_PMTU          (BASE_ERROR_RECORDLAYER + 8)

///Invalid record instance
#define OCP_RL_INVALID_INSTANCE          (BASE_ERROR_RECORDLAYER + 9)

///Alert record received
#define OCP_RL_ALERT_RECEIVED            (BASE_ERROR_RECORDLAYER + 10)

///Alert record received
#define OCP_RL_APPDATA_RECEIVED          (BASE_ERROR_RECORDLAYER + 11)

///Malloc Failure
#define OCP_RL_MALLOC_FAILURE            (BASE_ERROR_RECORDLAYER + 12)

///Cipher Spec Content Spec
#define CONTENTTYPE_CIPHER_SPEC         0x14
///Alert Content Spec
#define CONTENTTYPE_ALERT               0x15
///Handshake Content Spec
#define CONTENTTYPE_HANDSHAKE           0x16
///Application Data Content Spec
#define CONTENTTYPE_APP_DATA            0x17

///UDP REcord overhead length
#define UDP_RECORD_OVERHEAD             41         //20(IP Header) + 8(UDP Header) + 13(RL Header)

///Length of the MAC generated for encrypted message
#define MAC_LENGTH     8

///Length of Explicit Nounce
#define EXPLICIT_NOUNCE_LENGTH  8

/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/

/**
 * \brief Structure containing Record Layer information.
 */
typedef struct sRL_d
{
    ///Handle to Record layer structure sRecordLayer_d
    hdl_t phRLHdl;

    ///Pointer to Configured TL
    sConfigTL_d* psConfigTL;

    ///Pointer to Configured Crypto
    sConfigCL_d* psConfigCL;

    ///Structure that holds logger parameters
    sLogger_d sLogger;

    ///Indicates if memory needs to be allocated or not
    uint8_t bMemoryAllocated;

    ///Content Type
    uint8_t bContentType;

    ///Indicate Multiple record received
    uint8_t bMultipleRecord;

    ///Indicates if the send flight is retransmitted
    bool_t fRetransmit;

    ///Indicates if the record received is encrypted or not
    uint8_t bDecRecord;

    ///Indicates if the record received is Change cipher spec
    uint8_t bRecvCCSRecord;

    ///pointer to a Next record
    uint8_t* pNextRecord;

    ///Pointer to callback to change the server epoch state
    Void (*fServerStateTrn)(const void*);
}sRL_d;


///Function pointer to Initialize Record Layer.
typedef int32_t (*fRLInit)(sRL_d* psRL);

///Function pointer for Record Layer Send
typedef int32_t (*fRLSend)(sRL_d* psRL, uint8_t* pbBuffer,uint16_t wLen);

///Function pointer for Record Layer Receive
typedef int32_t (*fRLRecv)(sRL_d* psRL,uint8_t* pbBuffer,uint16_t* pwLen);

///Function pointer to close Record Layer
typedef void (*fRLClose)(sRL_d* psRL);

/**
 * \brief Structure to configure Record Layer.
 */
typedef struct sConfigRL_d
{
    ///Function pointer to Init RL
    fRLInit pfInit;

    ///Function pointer to CloseRL
    fRLClose pfClose;

    ///Function pointer to Send via RL
    fRLSend pfSend;

    ///Function pointer to Receive via RL
    fRLRecv pfRecv;

    ///Record Layer
    sRL_d sRL;
}sConfigRL_d;

#endif //__OCPRECORDLAYER_H__
/**
* @}
*/
