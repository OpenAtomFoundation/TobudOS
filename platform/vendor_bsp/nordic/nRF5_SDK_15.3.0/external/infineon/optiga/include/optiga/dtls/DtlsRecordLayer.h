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
 * \file DtlsRecordLayer.h
 *
 * \brief This file contains structures and prototypes of record layer functionalities for DTLS.
 *
 * \addtogroup grOCP
 * @{
 *
 */


#ifndef __DTLSRECORDLAYER_H__
#define __DTLSRECORDLAYER_H__

#include "optiga/dtls/DtlsWindowing.h"
#include "optiga/dtls/OcpRecordLayer.h"
#include "optiga/dtls/MessageLayer.h"
#include "optiga/dtls/OcpCommonIncludes.h"


#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

///Failure in execution and record already received
#define OCP_RL_WINDOW_IGNORE            BASE_ERROR_WINDOW

///Valid record is received and window is updated and moved
#define OCP_RL_WINDOW_UPDATED           (BASE_ERROR_WINDOW + 1)

///Valid record is received and window is updated
#define OCP_RL_WINDOW_MOVED             (BASE_ERROR_WINDOW + 2)

///Window overflow
#define OCP_RL_SEQUENCE_OVERFLOW        (BASE_ERROR_WINDOW + 3)

///Encryption/Decryption enabled flag
#define ENC_DEC_ENABLED                 0xB5

///Encryption/Decryption disabled flag
#define ENC_DEC_DISABLED                0x6A

/// @cond hidden

///Record Header offset for Content Type
#define OFFSET_RL_CONTENTTYPE      (0)

///Record Header offset for Protocol Version
#define OFFSET_RL_PROT_VERSION     (OFFSET_RL_CONTENTTYPE  + 1) //1

///Record Header offset for Epoch
#define OFFSET_RL_EPOCH            (OFFSET_RL_PROT_VERSION + 2) //3

///Record Header offset Sequence Number
#define OFFSET_RL_SEQUENCE         (OFFSET_RL_EPOCH        + 2) //5

///Record Header offset for Fragment Length
#define OFFSET_RL_FRAG_LENGTH      (OFFSET_RL_SEQUENCE     + 6) //11

///Record Header offset for Start of Fragment
#define OFFSET_RL_FRAGMENT         (OFFSET_RL_FRAG_LENGTH  + 2) //13

///Record Header length
#define LENGTH_RL_HEADER            (OFFSET_RL_FRAGMENT)

///Record Header length of sequence number bytes
#define LENGTH_RL_SEQUENCE          6

///Flag to indicate change cipher spec is received
#define CCS_RECORD_RECV             0x01

///Flag to indicate change cipher spec is not received
#define CCS_RECORD_NOTRECV          0x00

/// @endcond
/**
 * \brief  Structure for Record Layer (D)TLS.
 */
typedef struct sRecordLayer_d
{
    ///Server epoch Number
    uint16_t wServerEpoch;
    ///Server Sequence Number
    sUint64 sServerSeqNumber;
    ///(D)TLS Version Information
    uint16_t wTlsVersionInfo;
    ///Client epoch Number
    uint16_t wClientEpoch;
    ///Client Next epoch Number
    uint16_t wClientNextEpoch;
    ///Client Sequence Number
    sUint64 sClientSeqNumber;
    ///Client Sequence Number for the next Epoch
    sUint64 sClientNextSeqNumber;
    ///Flag whether record to be encrypted/decrypted while send/recv.
    uint8_t bEncDecFlag;
    ///Pointer to callback to validate record
    int32_t (*fEncDecRecord)(const sCL_d*, const sbBlob_d*, sbBlob_d*, uint16_t);
    ///Argument to be passed to callback, if any
    sCL_d* pEncDecArgs;
    ///Window
    sWindow_d* psWindow;
    ///Window for next epoch
    sWindow_d* psNextWindow;
    ///Session Key OID
    uint16_t wSessionKeyOID;
    ///Indicates if the record received is encrypted or not
    uint8_t *pbDec;
    ///Indicates if the record received is Change cipher spec
    uint8_t *pbRecvCCSRecord;
} sRecordLayer_d;

/**
 * \brief  Structure for Record contents.
 */
typedef struct sRecordData_d
{
    ///Content Type
    uint8_t bContentType;

    ///Actual message in the record
    sbBlob_d *psBlobInOutMsg;

    ///Information of record
    //uint32_t dwRecordInfo;

    ///Indicates if memory needs to be allocated or not
    uint8_t bMemoryAllocated;
}sRecordData_d;

/**
 * \brief  Structure to hold the information required for Encrypt and Decrypt message call back to encrypt the data.
 */
//typedef struct sCBEncDecMsg_d
//{
//    //Length of data to be encrypted/decrypted
//    uint16_t wInDataLength;
//
//    ///Session Key OID
//    uint16_t wSessionKeyOID;
//
//}sCBEncDecMsg_d;

/**
 * \brief  Structure to hold the information required change the server epoch state.
 */
typedef struct sCBStateTrn_d
{
     ///Handle to Record layer structure
    hdl_t phRLHdl;

}sCBStateTrn_d;

/**
 * \brief  Initialise DTLS Record Layer.
 */
int32_t DtlsRL_Init(sRL_d* psRL);

/**
 * \brief  Adds record header and send data over transport layer.
 */
int32_t DtlsRL_Send(sRL_d* psRecordLayer,uint8_t* pbData,uint16_t wDataLen);

/**
 * \brief  Receives a record over transport layer, performs window check and remove the record header.
 */
int32_t DtlsRL_Recv(sRL_d* psRecordLayer,uint8_t* pbBuffer,uint16_t* pwLen);

/**
 * \brief  Frees memory held by dtls record layer.
 */
void DtlsRL_Close(sRL_d* psRL);

/**
 * \brief Slides the window to highest set sequence number.
 */
void Dtls_SlideWindow(const sRL_d* PpsRecordLayer, eAuthState_d PeAuthState);

#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH */

#endif //__DTLSRECORDLAYER_H__
/**
* @}
*/
