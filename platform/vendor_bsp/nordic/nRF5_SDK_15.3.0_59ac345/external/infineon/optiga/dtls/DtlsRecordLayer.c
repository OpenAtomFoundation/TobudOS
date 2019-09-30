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
 * \file DtlsRecordLayer.c
 *
 * \brief This file provides APIs for the record layer functionalities.
 *
 * \addtogroup grOCP
 * @{
 *
 */

#include "optiga/common/Util.h"
#include "optiga/dtls/DtlsRecordLayer.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

///Default size of the window
#define DEFAULT_WINDOW_SIZE         32

/// @cond hidden
//Protocol version for DTLS 1.2
#define PROTOCOL_VERSION_1_2        0xFEFD

#define UDP_OVERHEAD                28              //20(IP Header) + 8(UDP Header)

/**
 * \brief  Structure to provide input to DtlsRL_CallBack_ValidateRec.
 */
typedef struct sCBValidateRec_d
{
    ///Record Layer
    sRecordLayer_d* psRecordLayer;

    ///Input record
    sbBlob_d* psbBlob;

    ///Record data
    sRecordData_d* psRecordData;
}sCBValidateRec_d;

/// @endcond

/**
 * \brief Validates the record header and decrypts the fragments if PpsRecData.bEncDecFlag is set
 */
_STATIC_H int32_t DtlsRL_Record_ProcessRecord(const sRecordLayer_d* PpsRecordLayer,const sbBlob_d* PpsBlobRecord,sRecordData_d* PpsRecData);

/**
 * \brief Prepares a record  and encrypts the fragment if PpsRecData.bEncDecFlag is set.<br>
 */
_STATIC_H int32_t DtlsRL_Record_PrepareRecord(sRecordLayer_d* PpsRecordLayer,const sRecordData_d* PpsRecData,sbBlob_d* PpsBlobRecord);

/**
 * \brief Callback to validate the record.
 */
_STATIC_H int32_t DtlsRL_CallBack_ValidateRec(const Void* PpvParams);

/**
 * \brief Gets the count of the total number of record in the buffer
 */
_STATIC_H int32_t DtlsRL_GetRecordCount(uint8_t* PpbBuffer,uint16_t PwLen,uint8_t* PpbRecCount);

/**
 *
 * Validates the record header and decrypts the fragments if PpsRecData.bEncDecFlag is set<br>
 * Updates sRecordData_d.dwRecordInfo with information, whether the record is from<br>
 * next epoch or the current epoch.<br>
 * \param[in] PpsRecordLayer    Pointer to #sRecordLayer_d structure
 * \param[in] PpsBlobRecord     Pointer to a blob containing record
 * \param[out] PpsRecData       Pointer to a blob containing processed record
 *
 * \retval    #OCP_RL_OK  Successful execution
 * \retval    #OCP_RL_ERROR    Failure in execution
 *
 */
_STATIC_H int32_t DtlsRL_Record_ProcessRecord(const sRecordLayer_d* PpsRecordLayer,const sbBlob_d* PpsBlobRecord,sRecordData_d* PpsRecData)
{
    int32_t i4Status = OCP_RL_ERROR;
    uint16_t wRecvFragLen;
    uint16_t wServerEpoch;
    sbBlob_d sBlobCipherMsg;
    sbBlob_d sBlobPlainMsg;
    uint8_t bContentType;
    uint8_t* pbDecData = NULL;
    uint16_t wInputProtVersion;
    do
    {

/// @cond hidden
#define MIN_DEC_PAYLOAD  17         //(1(Min 1 byte cipher text)+ 8(MAC) + 8 (Explicit nonce)))
/// @endcond

        //Check for zero len blobs
        if((0 == PpsBlobRecord->wLen)||(0 == PpsRecData->psBlobInOutMsg->wLen))
        {
            //Any error code
            i4Status = (int32_t)OCP_RL_INVALID_RECORD_LENGTH;
            break;
        }

        bContentType = PpsBlobRecord->prgbStream[OFFSET_RL_CONTENTTYPE];
        wServerEpoch = Utility_GetUint16(PpsBlobRecord->prgbStream + OFFSET_RL_EPOCH);

        wInputProtVersion = Utility_GetUint16(PpsBlobRecord->prgbStream + OFFSET_RL_PROT_VERSION);

//Validate protocol version
#ifdef SUPPORT_OPENSSL
        //Protocol version for DTLS 1.0
        #define PROTOCOL_VERSION_1_0        0xFEFF

        //for Hello verify request, allow DTLS 1.2 ,1.0 as protocol version
        if(*(PpsBlobRecord->prgbStream+LENGTH_RL_HEADER) == 0x03)
        {
            if((PROTOCOL_VERSION_1_2 != wInputProtVersion)&&(PROTOCOL_VERSION_1_0 != wInputProtVersion))
            {
                i4Status = (int32_t)OCP_RL_INVALID_PROTOCOL_VERSION;
                break;
            }
        }
        else
        {
            if(PpsRecordLayer->wTlsVersionInfo != wInputProtVersion)
            {
                i4Status = (int32_t)OCP_RL_INVALID_PROTOCOL_VERSION;
                break;
            }
        }
        #undef PROTOCOL_VERSION_1_0
        #else
        if(PpsRecordLayer->wTlsVersionInfo != wInputProtVersion)
        {
            i4Status = (int32_t)OCP_RL_INVALID_PROTOCOL_VERSION;
            break;
        }
#endif /*SUPPORT_OPENSSL*/

        wRecvFragLen = Utility_GetUint16(PpsBlobRecord->prgbStream + OFFSET_RL_FRAG_LENGTH);

        //Reset the flag for each record being being processed which indicates
        *PpsRecordLayer->pbDec = 0x00;

        if(CONTENTTYPE_CIPHER_SPEC == bContentType)
        {
            if((wRecvFragLen != 0x01) || ((PpsRecordLayer->wClientNextEpoch != (PpsRecordLayer->wServerEpoch+1))) || (wServerEpoch != 0x00)
                || (CONTENTTYPE_HANDSHAKE != PpsRecData->bContentType))
            {
                //Error since state has not changed on client side
                i4Status = (int32_t)OCP_RL_BAD_RECORD;
            }
            else
            {
                PpsRecData->bContentType = bContentType;
                *PpsRecordLayer->pbRecvCCSRecord = CCS_RECORD_RECV;
                Utility_Memmove(PpsRecData->psBlobInOutMsg->prgbStream, PpsBlobRecord->prgbStream + OFFSET_RL_FRAGMENT, \
                    wRecvFragLen);
                PpsRecData->psBlobInOutMsg->wLen = wRecvFragLen;
                i4Status = OCP_RL_OK;
            }
            break;
        }
        else if((CONTENTTYPE_HANDSHAKE == PpsRecData->bContentType) && (CONTENTTYPE_APP_DATA == bContentType))
        {
            //Error, since content type is Application but no decryption
            i4Status = (int32_t)OCP_RL_BAD_RECORD;
            break;
        }
        //Is Decryption required?
        else if(((ENC_DEC_ENABLED & PpsRecordLayer->bEncDecFlag)== ENC_DEC_ENABLED) &&
                (wServerEpoch  == PpsRecordLayer->wClientNextEpoch))
        {
            if(wRecvFragLen < MIN_DEC_PAYLOAD)
            {
                i4Status = (int32_t)OCP_RL_INVALID_DEC_PAYLOAD_LEN;
                break;
            }

            pbDecData = (uint8_t*)OCP_MALLOC(PpsBlobRecord->wLen + OVERHEAD_UPDOWNLINK);
            if(NULL == pbDecData)
            {
                i4Status = (int32_t)OCP_RL_MALLOC_FAILURE;
                break;
            }

            //Copy the data to be decrypted to a offset by OVERHEAD_UPDOWNLINK bytes
            Utility_Memmove((pbDecData + OVERHEAD_UPDOWNLINK), PpsBlobRecord->prgbStream, PpsBlobRecord->wLen);
            //Decrypt data
            sBlobCipherMsg.prgbStream = pbDecData;
            sBlobCipherMsg.wLen = PpsBlobRecord->wLen + OVERHEAD_UPDOWNLINK;

            sBlobPlainMsg.prgbStream = pbDecData;
            sBlobPlainMsg.wLen = sBlobCipherMsg.wLen;

            //Decrypt call back function
            i4Status = PpsRecordLayer->fEncDecRecord(PpsRecordLayer->pEncDecArgs, &sBlobCipherMsg, &sBlobPlainMsg, PpsBlobRecord->wLen);

            if(OCP_CL_OK == i4Status)
            {
                i4Status = OCP_RL_OK;
                PpsRecData->bContentType = bContentType;
                *PpsRecordLayer->pbDec = 0x01;
                //Remove the record header
                PpsRecData->psBlobInOutMsg->wLen = sBlobPlainMsg.wLen - LENGTH_RL_HEADER;
                Utility_Memmove(PpsRecData->psBlobInOutMsg->prgbStream, sBlobPlainMsg.prgbStream + LENGTH_RL_HEADER, \
                    PpsRecData->psBlobInOutMsg->wLen);
            }
            OCP_FREE(pbDecData);
            break;
        }
        else
        {
            PpsRecData->bContentType = bContentType;
            //No Decryption, just copy the data
            Utility_Memmove(PpsRecData->psBlobInOutMsg->prgbStream, \
                    PpsBlobRecord->prgbStream + OFFSET_RL_FRAGMENT, \
                    wRecvFragLen);
            PpsRecData->psBlobInOutMsg->wLen = wRecvFragLen;
            i4Status = OCP_RL_OK;
        }
    }while(0);

    return i4Status;
}


/**
 *
 * Prepares a record  and encrypts the fragment if PpsRecData.bEncDecFlag is set.<br>
 *
 * \param[in] PpsRecordLayer        Pointer to #sRecordLayer_d structure
 * \param[in] PpsRecData            Pointer to a blob containing processed record
 * \param[in,out] PpsBlobRecord     Pointer to a blob containing record
 *
 * \retval    #OCP_RL_OK        Successful execution
 * \retval    #OCP_RL_ERROR     Failure in execution
 *
 */
_STATIC_H int32_t DtlsRL_Record_PrepareRecord(sRecordLayer_d* PpsRecordLayer,const sRecordData_d* PpsRecData,sbBlob_d* PpsBlobRecord)
{
    int32_t i4Status = OCP_RL_ERROR;

    sbBlob_d sBlobPlainMsg;
    sbBlob_d sBlobCipherMsg;
    sUint64 *psClientSeqNumber;
    uint16_t wClientEpoch = 0;

    do
    {
        //Check for zero len blobs
        if((0 == PpsBlobRecord->wLen)||(0 == PpsRecData->psBlobInOutMsg->wLen))
        {
            //Any error code
            i4Status = (int32_t)OCP_RL_INVALID_RECORD_LENGTH;
            break;
        }

        //Client as moved to new state(Change cipher spec is sent) and next state sequence number will be used
        if(PpsRecordLayer->wClientNextEpoch > PpsRecordLayer->wClientEpoch)
        {
            psClientSeqNumber = &PpsRecordLayer->sClientNextSeqNumber;
            wClientEpoch = PpsRecordLayer->wClientNextEpoch;
        }
        else if(PpsRecordLayer->wClientNextEpoch == PpsRecordLayer->wClientEpoch)
        {
            psClientSeqNumber = &PpsRecordLayer->sClientSeqNumber;
            wClientEpoch = PpsRecordLayer->wClientEpoch;
        }
        //Client Next epoch must not be less than current client epoch
        else
        {
            break;
        }

        //Add Content
        PpsBlobRecord->prgbStream[0] = PpsRecData->bContentType;
        //Add Protocol Version
        PpsBlobRecord->prgbStream[OFFSET_RL_PROT_VERSION] = (uint8_t)(PpsRecordLayer->wTlsVersionInfo >> 8);
        PpsBlobRecord->prgbStream[OFFSET_RL_PROT_VERSION+1] = (uint8_t)PpsRecordLayer->wTlsVersionInfo;
        //Add Epoch
        PpsBlobRecord->prgbStream[OFFSET_RL_EPOCH] = (uint8_t)(wClientEpoch >> 8);
        PpsBlobRecord->prgbStream[OFFSET_RL_EPOCH + 1] = (uint8_t)wClientEpoch;
        //Add Sequence Number.
        if((OFFSET_RL_SEQUENCE + LENGTH_RL_SEQUENCE)>PpsBlobRecord->wLen)
        {
            break;
        }

/// @cond hidden
#define MAX_HIGH_SEQ_NUM     0x00010000
/// @endcond

        //Check for window overflow if equal max value
        if((MAX_HIGH_SEQ_NUM == psClientSeqNumber->dwHigherByte) && (0x00 == psClientSeqNumber->dwLowerByte))
        {
            i4Status = (int32_t) OCP_RL_SEQUENCE_OVERFLOW;
            break;
        }

        //GetUint64(&PpsRecordLayer->sClientSeqNumber,PpsBlobRecord->prgbStream+OFFSET_RL_SEQUENCE,LENGTH_RL_SEQUENCE);
        Utility_SetUint16(PpsBlobRecord->prgbStream+OFFSET_RL_SEQUENCE,(uint16_t)psClientSeqNumber->dwHigherByte);
        Utility_SetUint32(PpsBlobRecord->prgbStream+OFFSET_RL_SEQUENCE+2,psClientSeqNumber->dwLowerByte);

        //Add Final Length
        PpsBlobRecord->prgbStream[OFFSET_RL_FRAG_LENGTH] = (uint8_t)(PpsRecData->psBlobInOutMsg->wLen >> 8);
        PpsBlobRecord->prgbStream[OFFSET_RL_FRAG_LENGTH+1] = (uint8_t)PpsRecData->psBlobInOutMsg->wLen;

        //Is Encryption required?
        if((ENC_DEC_ENABLED&PpsRecordLayer->bEncDecFlag)== ENC_DEC_ENABLED)
        {
            //Move the formed Record header by command lib over head(20) number of bytes
            Utility_Memmove(PpsBlobRecord->prgbStream + OVERHEAD_UPDOWNLINK, PpsBlobRecord->prgbStream, LENGTH_RL_HEADER);
            //Copy the data to be encrypted
            Utility_Memmove(PpsBlobRecord->prgbStream + LENGTH_RL_HEADER + OVERHEAD_UPDOWNLINK,
                    PpsRecData->psBlobInOutMsg->prgbStream, PpsRecData->psBlobInOutMsg->wLen);


            sBlobPlainMsg.prgbStream = PpsBlobRecord->prgbStream;
            sBlobPlainMsg.wLen = PpsRecData->psBlobInOutMsg->wLen + LENGTH_RL_HEADER + OVERHEAD_UPDOWNLINK;
            sBlobCipherMsg.prgbStream = PpsBlobRecord->prgbStream;
            sBlobCipherMsg.wLen = PpsBlobRecord->wLen;

            //Encrypt data
            i4Status = PpsRecordLayer->fEncDecRecord(PpsRecordLayer->pEncDecArgs, &sBlobPlainMsg, &sBlobCipherMsg,
                (PpsRecData->psBlobInOutMsg->wLen + LENGTH_RL_HEADER));

            if(OCP_CL_OK == i4Status)
            {
                i4Status = OCP_RL_OK;
                PpsBlobRecord->wLen = sBlobCipherMsg.wLen;
            }
        }
        else
        {
            if(CONTENTTYPE_CIPHER_SPEC == PpsRecData->bContentType)
            {
                PpsRecordLayer->wClientNextEpoch++ ;
                PpsRecordLayer->bEncDecFlag = 0xB5;
            }

            if(FALSE == PpsRecData->bMemoryAllocated)
            {
                //No encryption, just copy the data
                Utility_Memmove(PpsBlobRecord->prgbStream + OFFSET_RL_FRAGMENT,
                PpsRecData->psBlobInOutMsg->prgbStream,
                PpsRecData->psBlobInOutMsg->wLen);
            }
            PpsBlobRecord->wLen = PpsRecData->psBlobInOutMsg->wLen + LENGTH_RL_HEADER;
            i4Status = OCP_RL_OK;
        }
        //Update final length on success
        if(OCP_RL_OK != i4Status)
        {
            PpsBlobRecord->wLen = 0;
            break;
        }

        i4Status = IncrementUint64(psClientSeqNumber);
        if(UTIL_SUCCESS == i4Status)
        {
            i4Status = OCP_RL_OK;
        }

    }while(0);
/// @cond hidden
#undef RECORDSEQ_HIGH
/// @endcond
    return i4Status;
}





/**
 * \brief  Callback to validate the record.
 */
_STATIC_H int32_t DtlsRL_CallBack_ValidateRec(const Void* PpvParams)
{
/// @cond hidden
#define CB_PARAMS ((sCBValidateRec_d*)PpvParams)
/// @endcond
    return DtlsRL_Record_ProcessRecord(CB_PARAMS->psRecordLayer,CB_PARAMS->psbBlob,CB_PARAMS->psRecordData);
/// @cond hidden
#undef CB_PARAMS
/// @endcond
}

/**
 * \brief  Callback to Change the server epoch state and reset the change cipher spec receive flag.
 */
_STATIC_H Void DtlsRL_CB_ChangeServerState(const Void* PpCBParams)
{
/// @cond hidden
#define S_RECORDLAYER ((sRecordLayer_d*)(((sCBStateTrn_d*)PpCBParams)->phRLHdl))
/// @endcond

    S_RECORDLAYER->wServerEpoch++;
    *S_RECORDLAYER->pbRecvCCSRecord = CCS_RECORD_NOTRECV;

/// @cond hidden
#undef S_RECORDLAYER
/// @endcond
}

/**
 * Gets the count of the total number of record in the buffer.<br>
 *
 * \param[in,out]   PpbBuffer       Pointer to buffer containing the records.
 * \param[in]       PwLen           Length of the buffer.
 * \param[in,out]   PpbRecCount     Pointer to record count value.
 *
 * \retval    #OCP_RL_OK        Successful execution
 * \retval    #OCP_RL_ERROR     Failure in execution
 *
 */
_STATIC_H int32_t DtlsRL_GetRecordCount(uint8_t* PpbBuffer,uint16_t PwLen,uint8_t* PpbRecCount)
{
    int32_t i4Status = OCP_RL_ERROR;
    uint8_t* pbRecPtr = PpbBuffer;
    uint16_t wRemainingLen = PwLen;
    uint16_t wRecLen = 0;
    *PpbRecCount = 0;

    do
    {
        //Check for remaining length
        if(wRemainingLen > LENGTH_RL_HEADER)
        {
            //Content type check
            if((*pbRecPtr != CONTENTTYPE_CIPHER_SPEC) &&
            (*pbRecPtr != CONTENTTYPE_ALERT) &&
            (*pbRecPtr != CONTENTTYPE_HANDSHAKE) &&
            (*pbRecPtr != CONTENTTYPE_APP_DATA))
            {
                break;
            }

            //Get the record length
            wRecLen = (uint32_t)Utility_GetUint16(pbRecPtr+OFFSET_RL_FRAG_LENGTH);

            if((wRecLen+LENGTH_RL_HEADER) > wRemainingLen)
            {
                break;
            }
            (*PpbRecCount)++;
            wRemainingLen -= (wRecLen + LENGTH_RL_HEADER);
            pbRecPtr += (wRecLen + LENGTH_RL_HEADER);
            i4Status = OCP_RL_OK;
        }
        else
        {
            i4Status = (int32_t)OCP_RL_OK;
            break;
        }
    }while(TRUE);

    if(0 == (*PpbRecCount))
    {
        i4Status = OCP_RL_ERROR;
    }
    return i4Status;
}

/**
 * Adds record header and sends the record over the transport layer.<br>
 * Based on the input provided in PpsRecordLayer->bMemoryAllocated,the function decides whether to allocate
 * memory for the record or not.
 * For internal handshake implementation, memory is already allocated by Handshake layer.
 * In case of Application layer, memory should be allocated here.
 *
 * \param[in] PpsRecordLayer    Pointer to #sRecordLayer_d structure.
 * \param[in] PpbData           Pointer to a Data to be sent.
 * \param[in] PwDataLen         Pointer to length of data to be sent.
 *
 * \retval    #OCP_RL_OK  Successful execution
 * \retval    #OCP_RL_ERROR    Failure in execution
 *
 */
int32_t DtlsRL_Send(sRL_d* PpsRecordLayer,uint8_t* PpbData,uint16_t PwDataLen)
{
    int32_t i4Status = OCP_RL_ERROR;
    sRecordData_d sRecordData;
    uint8_t* pbTotalFragMem = NULL;
    uint8_t* pbEncData = NULL;
    sbBlob_d sBlobData;
    sbBlob_d sRecordBlobData;
/// @cond hidden
#define S_RECORDLAYER ((sRecordLayer_d*)(PpsRecordLayer->phRLHdl))

/// @endcond
    do
    {
        //If Flight 5 is retransmitted.Reset the value to previous state
        if(PpsRecordLayer->fRetransmit == TRUE)
        {
            S_RECORDLAYER->wClientNextEpoch--;
            S_RECORDLAYER->bEncDecFlag = 0x00;
            PpsRecordLayer->fRetransmit = FALSE;
        }

        if(TRUE == PpsRecordLayer->bMemoryAllocated)
        {
            //In case of Handshake
            //Form struture, point to message Data
            sRecordData.bContentType = PpsRecordLayer->bContentType;
            sRecordData.psBlobInOutMsg = &sRecordBlobData;
            sRecordData.psBlobInOutMsg->prgbStream = PpbData+LENGTH_RL_HEADER;
            sRecordData.psBlobInOutMsg->wLen = PwDataLen-LENGTH_RL_HEADER;

            //Client and server are in the same state.Encryption is disabled
            if(S_RECORDLAYER->bEncDecFlag != ENC_DEC_ENABLED)
            {
                sBlobData.prgbStream = PpbData;
                sBlobData.wLen  = PwDataLen;
            }
        }
        else
        {
            sRecordData.bContentType = PpsRecordLayer->bContentType;
            sRecordData.psBlobInOutMsg = &sRecordBlobData;
            sRecordData.psBlobInOutMsg->prgbStream = PpbData;
            sRecordData.psBlobInOutMsg->wLen = PwDataLen;

            if(S_RECORDLAYER->bEncDecFlag != ENC_DEC_ENABLED)
            {
                //Assign Buffer
                pbTotalFragMem = (uint8_t*)OCP_MALLOC(PwDataLen + LENGTH_RL_HEADER);
                if(NULL == pbTotalFragMem)
                {
                    i4Status = (int32_t)OCP_RL_MALLOC_FAILURE;
                    break;
                }
                sBlobData.prgbStream = pbTotalFragMem;
                sBlobData.wLen  = PwDataLen + LENGTH_RL_HEADER;
            }
        }

        //Client as moved to new state and encryption is enabled.Sufficient memory is allocated to store the encrypted data
        if(S_RECORDLAYER->bEncDecFlag == ENC_DEC_ENABLED)
        {
            pbEncData = (uint8_t*)OCP_MALLOC(PwDataLen + LENGTH_RL_HEADER + MAC_LENGTH +  EXPLICIT_NOUNCE_LENGTH + OVERHEAD_UPDOWNLINK);
            if(NULL == pbEncData)
            {
                i4Status = (int32_t)OCP_RL_MALLOC_FAILURE;
                break;
            }
            sBlobData.prgbStream = pbEncData;
            sBlobData.wLen  = PwDataLen + LENGTH_RL_HEADER + MAC_LENGTH +  EXPLICIT_NOUNCE_LENGTH + OVERHEAD_UPDOWNLINK;
        }

        //Assign function pointer for encryption
        S_RECORDLAYER->fEncDecRecord = PpsRecordLayer->psConfigCL->pfEncrypt;
        S_RECORDLAYER->pEncDecArgs = &(PpsRecordLayer->psConfigCL->sCL);

        //Add Record
        sRecordData.bMemoryAllocated = PpsRecordLayer->bMemoryAllocated;
        i4Status = DtlsRL_Record_PrepareRecord(S_RECORDLAYER,&sRecordData,&sBlobData);
        if(OCP_RL_OK != i4Status)
        {
            break;
        }

        //Send the data over transport layer
        i4Status = PpsRecordLayer->psConfigTL->pfSend(&(PpsRecordLayer->psConfigTL->sTL),
        sBlobData.prgbStream,sBlobData.wLen);
        if(OCP_TL_OK != i4Status)
        {
            break;
        }
        i4Status = (int32_t)OCP_RL_OK;

    }while(FALSE);
    if(FALSE == PpsRecordLayer->bMemoryAllocated)
    {
        OCP_FREE(pbTotalFragMem);
    }
    if(S_RECORDLAYER->bEncDecFlag == ENC_DEC_ENABLED)
    {
        OCP_FREE(pbEncData);
    }
/// @cond hidden
#undef S_RECORDLAYER
/// @endcond
    return i4Status;
}




/**
 * To Slide the window to highest set sequence number.
 * If Higher bound reaches a value greater than maximum possible sequence number all the bits greater than
 * maximum sequence number is set to 1 in the window.
 *
 * \param[in,out] PpsRecordLayer        Pointer to #sRL_d structure.
 * \param[in]     PeAuthState            Indicates the state of Mutual Authentication Public Key Scheme (DTLS)
 *
 * \retval    #OCP_RL_OK  Successful execution
 * \retval    #OCP_RL_ERROR    Failure in execution
 *
 */
void Dtls_SlideWindow(const sRL_d* PpsRecordLayer, eAuthState_d PeAuthState)
{
    int32_t i4Status = (int32_t)OCP_RL_OK;
    uint8_t bSlideCount = 0;
    uint8_t count = 0;
    uint8_t bLoopCount = 1;
    uint16_t wWordValue;
    uint32_t dwValue;
    sUint64 sBoundValue;
    sWindow_d *psWindow;

    /// @cond hidden
    #define PS_WINDOW ((sRecordLayer_d*)(PpsRecordLayer->phRLHdl))->psWindow
    #define PS_NEXTWINDOW ((sRecordLayer_d*)(PpsRecordLayer->phRLHdl))->psNextWindow
    /// @endcond
    do
    {
/// @cond hidden
#define MAX_LOW_SEQ_NUM     0xFFFFFFFF
#define RECORDSEQ_HIGH  0xFFFF
/// @endcond
        if(eAuthCompleted == PeAuthState)
        {
            bLoopCount = 2;
        }
        while(bLoopCount > 0)
        {
            if(bLoopCount > 1)
            {
                psWindow = PS_NEXTWINDOW;
            }
            else
            {
                psWindow = PS_WINDOW;
            }
            //If window size is 32 slide the higher word of the window
            if(32 == psWindow->bWindowSize)
            {
                dwValue = psWindow->sWindowFrame.dwHigherByte;
                //Check for highest bit which is set
                while(count < 32)
                {
                    if(LEAST_SIGNIFICANT_BIT_HIGH == (LEAST_SIGNIFICANT_BIT_HIGH & dwValue))
                    {
                        break;
                    }
                    dwValue = dwValue >> 1 ;
                    count++;
                }
                bSlideCount = psWindow->bWindowSize - count;
            }

            //If window size is greater then 32
            if(32 < psWindow->bWindowSize)
            {
                //If the value of lower word is greater than zero,than highest bit is set in lower double word
                if(psWindow->sWindowFrame.dwLowerByte > 0)
                {
                    //If the value of lower double word is greater then 0xFFFF,then the highest bit is set in MSB 16 bits
                    if(psWindow->sWindowFrame.dwLowerByte > 0xFFFF)
                    {
                       wWordValue = psWindow->sWindowFrame.dwLowerByte >> 16;
                    }
                    //If the value of lower double word is lesser then 0xFFFF,then the highest bit is set in LSB 16 bits
                    else
                    {
                        bSlideCount+= 16;
                        wWordValue = (uint16_t)psWindow->sWindowFrame.dwLowerByte;

                    }
                    while(count < 16)
                    {
                        if(0x0001 == (wWordValue & 0x0001))
                        {
                            break;
                        }
                        wWordValue = wWordValue >> 1;
                        count++;
                    }
                    count = 16 - count;
                    bSlideCount += count ;
                    bSlideCount += (psWindow->bWindowSize - 32);
                }
                //If the value of lower word is lesser than or equal to zero,than highest bit is set in higher double word
                else
                {
                    dwValue = psWindow->sWindowFrame.dwHigherByte;
                    //Check for highest bit which is set
                    while(count < 32)
                    {
                        if(LEAST_SIGNIFICANT_BIT_HIGH == (LEAST_SIGNIFICANT_BIT_HIGH & dwValue))
                        {
                            break;
                        }
                        dwValue = dwValue >> 1 ;
                        count++;
                    }
                    bSlideCount = ((psWindow->bWindowSize - 32) - count);
                }
            }

            sBoundValue.dwHigherByte = 0x00;
            sBoundValue.dwLowerByte = bSlideCount;

            //Set the window lower bound to the new value
            i4Status =  AddUint64(&psWindow->sLowerBound, &sBoundValue,&psWindow->sLowerBound);
            if(UTIL_SUCCESS != i4Status)
            {
                break;
            }

            //Set the window higher bound to the new value
            i4Status =  AddUint64(&psWindow->sHigherBound, &sBoundValue,&psWindow->sHigherBound);
            if(UTIL_SUCCESS != i4Status)
            {
                break;
            }

            //Update the window
            psWindow->sWindowFrame.dwLowerByte = 0x00;
            psWindow->sWindowFrame.dwHigherByte = 0x00;

            //If the Higher bound is greater than maximum value of the sequence number
            if(psWindow->sHigherBound.dwHigherByte > (uint32_t)RECORDSEQ_HIGH)
            {
                sBoundValue.dwHigherByte = RECORDSEQ_HIGH;
                sBoundValue.dwLowerByte = MAX_LOW_SEQ_NUM;

                //Difference of Higher bound and max sequene number
                i4Status = SubtractUint64(&psWindow->sHigherBound, &sBoundValue, &sBoundValue);
                if(UTIL_SUCCESS != i4Status)
                {
                    break;
                }

                //If 32 bit window
                if(32 == psWindow->bWindowSize)
                {
                    psWindow->sWindowFrame.dwHigherByte = MAX_LOW_SEQ_NUM;
                    dwValue = 32 - sBoundValue.dwLowerByte;
                    while(dwValue--)
                    {
                        psWindow->sWindowFrame.dwHigherByte = psWindow->sWindowFrame.dwHigherByte >> 1;
                    }
                }
                //If window is greater than 32
                else
                {
                    psWindow->sWindowFrame.dwLowerByte = MAX_LOW_SEQ_NUM;
                    if(sBoundValue.dwLowerByte > 32)
                    {
                        psWindow->sWindowFrame.dwHigherByte = MAX_LOW_SEQ_NUM;

                        dwValue = psWindow->bWindowSize - sBoundValue.dwLowerByte;
                        while(dwValue--)
                        {
                            psWindow->sWindowFrame.dwHigherByte = psWindow->sWindowFrame.dwHigherByte >> 1;
                        }
                    }
                    else
                    {
                        dwValue = 32 - sBoundValue.dwLowerByte;
                        while(dwValue--)
                        {
                            psWindow->sWindowFrame.dwLowerByte = psWindow->sWindowFrame.dwLowerByte >> 1;
                        }
                    }
                }
            }
            //Reset the count values
            count = 0;
            bSlideCount = 0;
            dwValue = 0;
            wWordValue = 0;
            bLoopCount--;
        }
    }while(FALSE);
/// @cond hidden
#undef PS_WINDOW
#undef PS_NEXTWINDOW
#undef MAX_SEQ_NUM
#undef RECORDSEQ_HIGH
/// @endcond

}

/**
 * Receives a record over transport layer, performs window check and remove the record header
 * before passing back the data.
 * The function also handles multiple record received in a single datagram by maintaining a count of
 * number of records received.
 * If the count is non-zero then a record is chosen from the given array, otherwise new record is received
 * from the transport layer.
 *
 * \param[in] PpsRecordLayer    Pointer to #sRecordLayer_d structure.
 * \param[in,out] PpbBuffer     Pointer to buffer to receive data.
 * \param[in,out] PpwLen        Pointer to variable containing buffer size and later length of received data.
 *
 * \retval    #OCP_RL_OK  Successful execution
 * \retval    #OCP_RL_ERROR    Failure in execution
 *
 */

int32_t DtlsRL_Recv(sRL_d* PpsRecordLayer,uint8_t* PpbBuffer,uint16_t* PpwLen)
{
    int32_t i4Status = OCP_RL_ERROR;
    sbBlob_d sbBlobCBData;
    sCBValidateRec_d sCBValidateRec;
    sRecordData_d sRecData;
    sbBlob_d sInBlobData;
    sWindow_d *psWindow;
    uint16_t wServerEpoch;
/// @cond hidden
#define S_RECORDLAYER ((sRecordLayer_d*)(PpsRecordLayer->phRLHdl))
/// @endcond
    do
    {
        //If all record not processed, do not call receive
        if(0 == PpsRecordLayer->bMultipleRecord)
        {
            //Receive Data over Transport
            i4Status = PpsRecordLayer->psConfigTL->pfRecv(&(PpsRecordLayer->psConfigTL->sTL),
            PpbBuffer,PpwLen);
            if((int32_t)OCP_TL_NO_DATA == i4Status)
            {
                i4Status = (int32_t)OCP_RL_NO_DATA;
                break;
            }
            if(OCP_TL_OK != i4Status)
            {
                break;
            }

            if(*PpwLen > (MAX_PMTU - UDP_OVERHEAD))
            {
                i4Status = (int32_t)OCP_RL_INVALID_RECORD_LENGTH;
                break;
            }

            //Check how many record are available
            i4Status = DtlsRL_GetRecordCount(PpbBuffer,*PpwLen,&(PpsRecordLayer->bMultipleRecord));
            if(OCP_RL_OK != i4Status)
            {
                break;
            }

            //Copy the received first record to call back input sBlob
            sbBlobCBData.prgbStream = PpbBuffer;
            sbBlobCBData.wLen = LENGTH_RL_HEADER;
            sbBlobCBData.wLen += Utility_GetUint16(sbBlobCBData.prgbStream + OFFSET_RL_FRAG_LENGTH);

            //Copy the location of the next record
            PpsRecordLayer->pNextRecord = (sbBlobCBData.prgbStream + sbBlobCBData.wLen);

            //Decrement the record count after a record is copied
            PpsRecordLayer->bMultipleRecord--;
        }
        else
        {
            //For multiple record
            ////Copy the received record to call back input sBlob
            sbBlobCBData.prgbStream = PpsRecordLayer->pNextRecord;
            sbBlobCBData.wLen = LENGTH_RL_HEADER;
            sbBlobCBData.wLen += Utility_GetUint16(sbBlobCBData.prgbStream + OFFSET_RL_FRAG_LENGTH);

            //Copy the location of the next successive record
            PpsRecordLayer->pNextRecord = (sbBlobCBData.prgbStream + sbBlobCBData.wLen);

            //Decrement the record count after a record is copied
            PpsRecordLayer->bMultipleRecord--;
        }

        //Assign function pointer for Decryption
        S_RECORDLAYER->fEncDecRecord = PpsRecordLayer->psConfigCL->pfDecrypt;
        S_RECORDLAYER->pEncDecArgs = &(PpsRecordLayer->psConfigCL->sCL);

        sCBValidateRec.psRecordLayer = S_RECORDLAYER;
        sCBValidateRec.psRecordData = &sRecData;

        //Get message from received record
        sCBValidateRec.psRecordData->bContentType = PpsRecordLayer->bContentType;
        sCBValidateRec.psRecordData->psBlobInOutMsg = &sInBlobData;
        sCBValidateRec.psRecordData->psBlobInOutMsg->prgbStream = PpbBuffer;
        sCBValidateRec.psRecordData->psBlobInOutMsg->wLen = *PpwLen;


        S_RECORDLAYER->sServerSeqNumber.dwHigherByte = (uint32_t)Utility_GetUint16 (sbBlobCBData.prgbStream + OFFSET_RL_SEQUENCE);
        S_RECORDLAYER->sServerSeqNumber.dwLowerByte = Utility_GetUint32 (sbBlobCBData.prgbStream + OFFSET_RL_SEQUENCE + 2);

        //Pass received Record
        sCBValidateRec.psbBlob = &sbBlobCBData;

        //Validate epoch
        //Ignore record,If received epoch less than current epoch or
        //received epoch is one greater than current epoch.
        wServerEpoch = Utility_GetUint16 (sbBlobCBData.prgbStream + OFFSET_RL_EPOCH);
        if((S_RECORDLAYER->wServerEpoch > wServerEpoch)||
        (1 + S_RECORDLAYER->wServerEpoch) < wServerEpoch)
        {
            i4Status = (int32_t)OCP_RL_INCORRECT_EPOCH;
            break;
        }

        //If epoch is one greater than current epoch and Server has not moved to new epoch
        if((1 == (wServerEpoch-S_RECORDLAYER->wServerEpoch)) && (S_RECORDLAYER->wClientNextEpoch != wServerEpoch))
        {
            i4Status = (int32_t)OCP_RL_INCORRECT_EPOCH;
            break;
        }

        //If the Client and server are in the same state use the current window for record sequence filtering
        if(wServerEpoch == S_RECORDLAYER->wClientEpoch)
        {
            psWindow = S_RECORDLAYER->psWindow;
        }
        //If Server as moved to the new state (Change cipher spec is already sent from server) use the new window for record sequence filtering
        else if(wServerEpoch == (S_RECORDLAYER->wClientEpoch + 1))
        {
            psWindow = S_RECORDLAYER->psNextWindow;
        }
        //Error if server epoch is less then client epoch
        else
        {
            i4Status = (int32_t)OCP_RL_INCORRECT_EPOCH;
            break;
        }

        psWindow->fValidateRecord = DtlsRL_CallBack_ValidateRec;
        psWindow->pValidateArgs = (Void*)&sCBValidateRec;

        psWindow->sRecvSeqNumber.dwHigherByte = S_RECORDLAYER->sServerSeqNumber.dwHigherByte;
        psWindow->sRecvSeqNumber.dwLowerByte = S_RECORDLAYER->sServerSeqNumber.dwLowerByte;

        i4Status = DtlsCheckReplay(psWindow);

        if(((int32_t)CMD_LIB_DECRYPT_FAILURE == i4Status) || ((int32_t)OCP_RL_MALLOC_FAILURE == i4Status))
        {
            break;
        }
        else if((int32_t)OCP_RL_WINDOW_IGNORE != i4Status)
        {

            *PpwLen = sCBValidateRec.psRecordData->psBlobInOutMsg->wLen;
            if( CONTENTTYPE_ALERT == sCBValidateRec.psRecordData->bContentType)
            {
                i4Status = (int32_t)OCP_RL_ALERT_RECEIVED;
            }
            else if(CONTENTTYPE_APP_DATA == sCBValidateRec.psRecordData->bContentType)
            {
                i4Status = (int32_t)OCP_RL_APPDATA_RECEIVED;
            }
            else
            {
                i4Status =  (int32_t)OCP_RL_OK;
            }
        }
        else
        {
            i4Status = (int32_t)OCP_RL_ERROR;
        }
        //if window slide refresh buffer to removed old sequence number
    }while(FALSE);
/// @cond hidden
#undef S_RECORDLAYER
/// @endcond
    return i4Status;
}


/**
 * Initialise the DTLS Record Layer
 * \param[in,out] PpsRL    Pointer to #sRecordLayer_d structure.
 *
 * \retval    #OCP_RL_OK  Successful execution
 * \retval    #OCP_RL_ERROR    Failure in execution
 *
 */
int32_t DtlsRL_Init(sRL_d* PpsRL)
{
    int32_t i4Status = OCP_RL_ERROR;

    do
    {
        PpsRL->phRLHdl = (sRecordLayer_d*)OCP_MALLOC(sizeof(sRecordLayer_d));
        if(NULL == PpsRL->phRLHdl)
        {
            i4Status = (int32_t)OCP_RL_MALLOC_FAILURE;
            break;
        }
        memset(PpsRL->phRLHdl, 0x00, sizeof(sRecordLayer_d));

        /// @cond hidden
        #define S_RECORDLAYER ((sRecordLayer_d*)(PpsRL->phRLHdl))
        #define PS_WINDOW (S_RECORDLAYER->psWindow)
        #define PS_NEXTWINDOW (S_RECORDLAYER->psNextWindow)
        /// @endcond

        S_RECORDLAYER->psWindow = NULL;
        S_RECORDLAYER->psNextWindow = NULL;
        S_RECORDLAYER->bEncDecFlag = 0;
        S_RECORDLAYER->wClientEpoch = 0;
        S_RECORDLAYER->wClientNextEpoch = 0;
        PpsRL->bDecRecord = 0;
        PpsRL->bRecvCCSRecord = CCS_RECORD_NOTRECV;
        S_RECORDLAYER->pbDec = &PpsRL->bDecRecord;
        S_RECORDLAYER->pbRecvCCSRecord = &PpsRL->bRecvCCSRecord;
        PpsRL->fServerStateTrn = DtlsRL_CB_ChangeServerState;


        S_RECORDLAYER->wTlsVersionInfo = PROTOCOL_VERSION_1_2;//0xFE,0xFD

        PpsRL->fRetransmit = FALSE;
        PpsRL->bMultipleRecord = 0x00;
        S_RECORDLAYER->psWindow = (sWindow_d*)OCP_MALLOC(sizeof(sWindow_d));
        if(NULL == S_RECORDLAYER->psWindow)
        {
            i4Status = (int32_t)OCP_RL_MALLOC_FAILURE;
            break;
        }
        memset(S_RECORDLAYER->psWindow, 0x00, sizeof(sWindow_d));

        PS_WINDOW->bWindowSize = DEFAULT_WINDOW_SIZE;
        PS_WINDOW->sHigherBound.dwLowerByte = PS_WINDOW->bWindowSize - 1;
        PS_WINDOW->sHigherBound.dwHigherByte = 0x00;

        S_RECORDLAYER->psNextWindow = (sWindow_d*)OCP_MALLOC(sizeof(sWindow_d));
        if(NULL == S_RECORDLAYER->psNextWindow)
        {
            i4Status = (int32_t)OCP_RL_MALLOC_FAILURE;
            break;
        }
        memset(S_RECORDLAYER->psNextWindow, 0x00, sizeof(sWindow_d));

        PS_NEXTWINDOW->bWindowSize = DEFAULT_WINDOW_SIZE;
        PS_NEXTWINDOW->sHigherBound.dwLowerByte = PS_WINDOW->bWindowSize - 1;
        PS_NEXTWINDOW->sHigherBound.dwHigherByte = 0x00;

        PS_WINDOW->fValidateRecord = NULL;
        PS_WINDOW->pValidateArgs = NULL;

        i4Status = OCP_RL_OK;
    }while(FALSE);
/// @cond hidden
#undef S_RECORDLAYER
#undef PS_WINDOW
#undef PS_NEXTWINDOW
/// @endcond
    return i4Status;
}

/**
 * This function frees memory held by dtls record layer
 *
 * \param[in]  PpsRL     Pointer to the record layer structure
 *
 * \return  None
 */
void DtlsRL_Close(sRL_d* PpsRL)
{
/// @cond hidden
#define PS_WINDOW (((sRecordLayer_d*)PpsRL->phRLHdl)->psWindow)
#define PS_NEXTWINDOW (((sRecordLayer_d*)PpsRL->phRLHdl)->psNextWindow)
/// @endcond
    //NULL check
    if(NULL != PpsRL)
    {
        if(NULL != PpsRL->phRLHdl)
        {
            if(NULL != PS_WINDOW)
            {
                //Free the allocated memory for sWindow_d structure
                OCP_FREE(PS_WINDOW);

                if(NULL != PS_NEXTWINDOW)
                {
                    OCP_FREE(PS_NEXTWINDOW);
                    PS_NEXTWINDOW = NULL;
                }
                PS_WINDOW = NULL;
            }
            //Free the allocated memory record handle
            OCP_FREE(PpsRL->phRLHdl);

            PpsRL->phRLHdl = NULL;
        }
    }
/// @cond hidden
#undef PS_WINDOW
#undef PS_NEXTWINDOW
/// @endcond
}

/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH */
