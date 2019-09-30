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
* \brief   This file implements the command library.
*
* \addtogroup  grCmdLib
* @{
*/

#include <stdint.h>
#include "optiga/common/Util.h"
#include "optiga/cmd/CommandLib.h"
#include "optiga/common/MemoryMgmt.h"

#ifdef USE_CMDLIB_WITH_RTOS
#include "optiga/pal/pal_os_timer.h"
#endif

/// @cond hidden

static optiga_comms_t* p_optiga_comms;

///Maximum size of buffer, considering Maximum size of arbitrary data (1500) and header bytes
#define MAX_APDU_BUFF_LEN               1558

///Get Data Parameter
#define PARAM_GET_DATA                  0x00

///Get Meta Data Parameter
#define PARAM_GET_METADATA              0x01

///Set Data Parameter
#define PARAM_SET_DATA                  0x00

///Set Meta Data Parameter
#define PARAM_SET_METADATA              0x01

///Erase and Write Parameter
#define PARAM_SET_DATA_ERASE            0x40

///ECDSA Set Auth message
#define PARAM_SET_AUTH_MSG              0x01

///ECDSA Get Auth message
#define PARAM_GET_AUTH_MSG              0x02

///Encrypt Data
#define PARAM_ENC_DATA                  0x61

///Decrypt Data
#define PARAM_DEC_DATA                  0x51

///Number of bytes in CMD
#define BYTES_CMD                       0x01

///Number of bytes in PARAM
#define BYTES_PARAM                     0x01

///Number of bytes in LEN
#define BYTES_LENGTH                    0x02

///Number of bytes in Sequence
#define BYTES_SEQ                       0x01

///Number of bytes in OID
#define BYTES_OID                       0x02

///Number of bytes in OFFSET
#define BYTES_OFFSET                    0x02

///Offset of status in the response APDU
#define OFFSET_RESP_STATUS              0x00

///Offset of CMD byte
#define OFFSET_CMD                      (0x00)

///Offset of PARAM byte
#define OFFSET_PARAM                    (OFFSET_CMD + BYTES_CMD)

///Offset of LEN byte
#define OFFSET_LENGTH                   (OFFSET_PARAM + BYTES_PARAM)

///Offset of PAYLOAD
#define OFFSET_PAYLOAD                  (OFFSET_LENGTH + BYTES_LENGTH)

///Offset of TAG
#define TAG_LENGTH_OFFSET               0x01

///Offset of TAG + LEN
#define TAG_VALUE_OFFSET                0x03

///Cmd of Get Data Object
#define CMD_GETDATA                     0x01

///Cmd of Set Data Object
#define CMD_SETDATA                     0x02

///Cmd of Set Auth Scheme
#define CMD_SETAUTH_SCM                 0x10

///Cmd of Set Auth Data
#define CMD_SETAUTH_MSG                 0x19

///Cmd of Get Auth Data
#define CMD_GETAUTH_MSG                 0x18

///Cmd of Get Random
#define CMD_GET_RND                     0x0C

///Cmd of Open Application
#define CMD_OPEN_APP                    0x70

///Cmd for ProcUplinkMsg
#define CMD_GETMSG                      0x1A

///Cmd for ProcDownlinkMsg
#define CMD_PUTMSG                      0x1B

///Cmd for Encrypt Data
#define CMD_ENCDATA                     CMD_GETMSG

///Cmd for Decrypt Data
#define CMD_DECDATA                     CMD_PUTMSG

///Cmd for CalcHash
#define CMD_CALCHASH                    0x30

///Cmd for VerifySign
#define CMD_VERIFYSIGN                  0x32

///Cmd for GenKeyPair
#define CMD_GENERATE_KEY_PAIR           0x38

///Cmd for CalcSign
#define CMD_CALC_SIGN                   0x31

///Cmd for CalcSSec
#define CMD_CALC_SHARED_SEC             0x33

///Cmd for DeriveKey
#define CMD_DERIVE_KEY                  0x34

///Payload length of Get Data
#define LEN_PL_OIDDATA                  0x06

///Payload length of OID
#define LEN_PL_OID                      0x02

///Oid of error object
#define OID_ERROR                       0xF1C2

///Cmd with MSB set
#define CMD_CODE_MSB_SET                0x80

/// Unique application identifier
#define UNIQUE_APP_IDENTIFIER           0xD2, 0x76, 0x00, 0x00, 0x04, 0x47, 0x65, 0x6E, 0x41, 0x75, 0x74, 0x68, 0x41, 0x70, 0x70, 0x6C

///Session ID Lower value
#define SESSION_ID_LOWER_VALUE          0xE100

///Session ID Higher value
#define SESSION_ID_HIGHER_VALUE         0xE103

///Tag for unprotected Message in the response data
#define TAG_UNPROTECTED                 0x60

///Tag for protected Message in the response data
#define TAG_PROTECTED                   0x50

///Mask the higher nibble
#define MASK_HIGHER_NIBBLE              0xF0

///Mask the higher nibble
#define MASK_LOWER_NIBBLE               0x0F

///Offset of tag byte
#define OFFSET_TAG                      (OFFSET_PAYLOAD + BYTES_SESSIONID)

///Offset of Tag Length byte
#define OFFSET_TAG_LEN                  (OFFSET_TAG + 1)

///Offset of Tag data
#define OFFSET_TAG_DATA                 (OFFSET_TAG_LEN + BYTES_LENGTH)

///GMT Unix time tag
#define TAG_GMTUNIX_TIME                0x31

///Number of bytes of GMT time
#define BYTES_GMT_TIME                  0x04

///Certificate OID tag
#define TAG_CERTIFICATE_OID             0x32

///Invalid value for Max size of comms buffer
#define INVALID_MAX_COMMS_BUFF_SIZE     0xFFFF

///Tag for digest
#define TAG_DIGEST                      0x01

///Tag for signature
#define TAG_SIGNATURE                   0x02

///Tag for public key oid
#define TAG_PUB_KEY_OID                 0x04

///Tag for algorithm identifier
#define TAG_ALGO_IDENTIFIER             0x05

///Tag for private key OID
#define TAG_OID                         0x01

///Tag for key usage identifier
#define TAG_KEY_USAGE_IDENTIFIER        0x02

///Tag for OID of signature key
#define TAG_OID_SIG_KEY                 0x03

///Tag for seed
#define TAG_SEED                        0x02

/// Length for OID of signature key
#define LEN_OID_SIG_KEY                 0x0002

///Len for algorithm identifier
#define LEN_KEY_USAGE_IDENTIFIER        0x0001

///Tag for export key pair
#define TAG_EXPORT_KEY_PAIR             0x07

///Len for private key oid
#define LEN_PRI_KEY                     0x0002

///Len for Shared Secret oid
#define LEN_SHARED_SECRET_OID           0x0002

///Len for exported key pair
#define LEN_EXPORT_KEY_PAIR             0x0000

///Tag for public key
#define TAG_PUB_KEY                     0x06

///Len for public key oid
#define LEN_PUB_KEY                     0x0002

///Len for algorithm identifier
#define LEN_ALGO_IDENTIFIER             0x0001

///Error in security chip indicating data out of boundary
#define ERR_DATA_OUT_OF_BOUND           0x00000008

//Maximum size of the communication buffer
/*lint --e{552} suppress "Temporary" */
uint16_t wMaxCommsBuffer = INVALID_MAX_COMMS_BUFF_SIZE;

//Finds minimum amongst the given 2 value
#ifndef MIN
#define MIN(a,b) ((a<b)?a:b)
#endif

///Hash Output length for SHA256 Hash Algorithm
#define SHA256_HASH_LEN                 32

///ECC NIST P 256 Private key length
#define ECC_NIST_P_256_KEY_LEN          32

///ECC NIST P 384 Private key length
#define ECC_NIST_P_384_KEY_LEN          48

/**
 * Initializes the APDU buffer in stack.<br>
 **/
#define INIT_STACK_APDUBUFFER(pbBuffer,wLen)\
{                                           \
    /*lint --e{733,830} suppress "Used only within Command Lib Block" */\
    uint8_t rgbAPDUBuffer[wLen];            \
    pbBuffer = rgbAPDUBuffer;               \
}

/**
 * Initializes the APDU buffer in heap.<br>
 **/
#define INIT_HEAP_APDUBUFFER(pbBuffer,wLen)                 \
{                                                           \
    if(INVALID_MAX_COMMS_BUFF_SIZE == wMaxCommsBuffer)      \
    {                                                       \
        i4Status = (int32_t)CMD_DEV_EXEC_ERROR;             \
        break;                                              \
    }                                                       \
    pbBuffer = (uint8_t*)OCP_MALLOC(wLen);                  \
    if(NULL == pbBuffer)                                    \
    {                                                       \
        i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;    \
        break;                                              \
    }                                                       \
}                                                           \

/**
 * Frees the heap memory.<br>
 **/
#define FREE_HEAP_APDUBUFFER(pbBuffer)      \
{                                           \
    if(NULL != pbBuffer)                    \
    {                                       \
        OCP_FREE(pbBuffer);                 \
        pbBuffer = NULL;                    \
    }                                       \
}


//Hash info details for Hash algorithm
const sHashinfo_d sHashInfo[1]=
{
    {eSHA256, SHA256_HASH_LEN, CALC_HASH_SHA256_CONTEXT_SIZE}
};

/**
 * \brief Structure to specify algorithm identifier and key length.
 */
typedef struct sAlgoinfo
{
    ///Algorithm type
    eAlgId_d eAlgKeyId;

    ///Key Length
    uint8_t bKeyLen;

}sAlgoInfo_d;

//Algorithm identifier
const sAlgoInfo_d sAlgoInfo[2]=
{
    {eECC_NIST_P256, ECC_NIST_P_256_KEY_LEN},
    {eECC_NIST_P384, ECC_NIST_P_384_KEY_LEN}
};

/**
 * \brief Structure to specify GetDataObject command parameters.
 */
typedef struct sApduData_d
{
    ///Cmd of the APDU
    uint8_t bCmd;

    ///Param of APDU
    uint8_t bParam;

    ///Payload length
    uint16_t wPayloadLength;

    //Payload Data
    uint8_t* prgbAPDUBuffer;

    //Response Data
    uint8_t* prgbRespBuffer;

    //Response Length
    uint16_t wResponseLength;
}sApduData_d;

/**
 * \brief Enumeration to specify the sequence of message/record.
 */
//lint --e{749} suppress "Added for future use"
typedef enum eFragSeq_d
{
    ///Start
    eStart =    0x00,

    ///Final
    eFinal =    0x01,

    ///Intermediate
    eContinue = 0x02
}eFragSeq_d;

volatile static host_lib_status_t optiga_comms_status;

//lint --e{715, 818} suppress "This is ignored as app_event_handler_t handler function prototype requires this argument.This will be used for object based implementation"
static void optiga_comms_event_handler(void* upper_layer_ctx, host_lib_status_t event)
{
    optiga_comms_status = event;
}

/**
 *
 * Gets the device error code by reading the Error code object id.<br>
 * If there is an internal error in function, then #CMD_DEV_ERROR is returned.<br>
 * Otherwise,the device error code is ORed with #CMD_DEV_ERROR and returned.<br>
 *
 * \retval    #CMD_DEV_ERROR
 * \retval    #CMD_LIB_ERROR
 * \retval    #CMD_DEV_EXEC_ERROR
 *
 */
_STATIC_H int32_t CmdLib_GetDeviceError(void)
{
    int32_t i4Status  = (int32_t)CMD_DEV_ERROR;
    uint8_t rgbErrorCmd[] = {CMD_GETDATA,0x00,0x00,0x02,(uint8_t)(OID_ERROR>>8),(uint8_t)OID_ERROR};
    uint16_t wBufferLength = sizeof(rgbErrorCmd);

    do
    {
        p_optiga_comms->upper_layer_handler = optiga_comms_event_handler;
        optiga_comms_status  = OPTIGA_COMMS_BUSY;
        i4Status  =  optiga_comms_transceive(p_optiga_comms,rgbErrorCmd,&wBufferLength,
                                                 rgbErrorCmd,&wBufferLength);
        if(OPTIGA_COMMS_SUCCESS != i4Status)
        {
            i4Status = (int32_t)CMD_DEV_EXEC_ERROR;
            break;
        }

        //wait for completion
        while(optiga_comms_status == OPTIGA_COMMS_BUSY){
#ifdef USE_CMDLIB_WITH_RTOS
            pal_os_timer_delay_in_milliseconds(1);
#endif
        };
        if(optiga_comms_status != OPTIGA_COMMS_SUCCESS)
        {
            i4Status = (int32_t)CMD_DEV_EXEC_ERROR;
            break;
        }

        if(0 == rgbErrorCmd[OFFSET_RESP_STATUS])
        {   //If response Header
            i4Status = (int32_t)(CMD_DEV_ERROR | rgbErrorCmd[OFFSET_PAYLOAD]);
        }
        else
        {
            //In this case, execution error is returned.
            i4Status  = (int32_t)CMD_DEV_EXEC_ERROR;
        }
    }while(FALSE);
    return i4Status;
}


/**
 * \brief Formats data as per Security Chip application and send using the communication functions.
 */
_STATIC_H int32_t TransceiveAPDU(sApduData_d *PpsApduData,uint8_t bGetError)
{
    //lint --e{818} suppress "PpsResponse is out parameter"
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wTotalLength;
    do
    {
        if(NULL == PpsApduData || NULL == p_optiga_comms)
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }
        PpsApduData->prgbAPDUBuffer[OFFSET_CMD] = PpsApduData->bCmd;
        PpsApduData->prgbAPDUBuffer[OFFSET_PARAM] = PpsApduData->bParam;

        //copy length
        PpsApduData->prgbAPDUBuffer[OFFSET_LENGTH] = (uint8_t)(PpsApduData->wPayloadLength >> BITS_PER_BYTE);
        PpsApduData->prgbAPDUBuffer[OFFSET_LENGTH+1] = (uint8_t)PpsApduData->wPayloadLength;

        //update total length to consider total header length
        wTotalLength = PpsApduData->wPayloadLength + LEN_APDUHEADER;

        p_optiga_comms->upper_layer_handler = optiga_comms_event_handler;
        optiga_comms_status  = OPTIGA_COMMS_BUSY;
        i4Status  =  optiga_comms_transceive(p_optiga_comms,PpsApduData->prgbAPDUBuffer,&wTotalLength,
                                                PpsApduData->prgbRespBuffer,&PpsApduData->wResponseLength);
        if(OPTIGA_COMMS_SUCCESS != i4Status)
        {
            i4Status = (int32_t)CMD_DEV_EXEC_ERROR;
            break;
        }
        //wait for completion
        do
        {
#ifdef USE_CMDLIB_WITH_RTOS
            pal_os_timer_delay_in_milliseconds(1);
#endif
        }while(optiga_comms_status == OPTIGA_COMMS_BUSY);

        if(optiga_comms_status != OPTIGA_COMMS_SUCCESS)
        {
            i4Status = (int32_t)CMD_DEV_EXEC_ERROR;
            break;
        }
        //return device error if not success
        if(0 != PpsApduData->prgbRespBuffer[OFFSET_RESP_STATUS])
        {
            if(TRUE == bGetError)
            {
                i4Status = CmdLib_GetDeviceError();
            }
            else
            {
                i4Status = (int32_t)CMD_LIB_ERROR;
            }
            break;
        }
        i4Status = CMD_LIB_OK;

    }while(FALSE);

    return i4Status;
}

/**
 * \brief Read the maximum size of communication buffer supported by the security chip by reading "Max comms buffer size" OID.
 */
_STATIC_H int32_t GetMaxCommsBuffer()
{
#define GETDATA_MAX_COMMS_SIZE  10
#define OID_MAX_COMMS_SIZE      0xE0C6

    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;

    do
    {
        INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer,GETDATA_MAX_COMMS_SIZE);

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        //Set the CMD
        sApduData.bCmd = CMD_GETDATA;
        sApduData.bParam = PARAM_GET_DATA;
        sApduData.wPayloadLength = LEN_PL_OID;
        sApduData.wResponseLength = GETDATA_MAX_COMMS_SIZE;
        //copy OID
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(OID_MAX_COMMS_SIZE >> BITS_PER_BYTE);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD +1] = (uint8_t)OID_MAX_COMMS_SIZE;

        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //Assign value to MaxCommsBuffer
        wMaxCommsBuffer = (uint16_t )((sApduData.prgbRespBuffer[LEN_APDUHEADER] << 8) | (sApduData.prgbRespBuffer[LEN_APDUHEADER+1]));
    }while(FALSE);

#undef GETDATA_MAX_COMMS_SIZE
#undef OID_MAX_COMMS_SIZE
    return i4Status;
}


#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH
/**
 * \brief A common function for CmdLib_Encrypt and CmdLib_Decrypt.
 *
 */
_STATIC_H int32_t CmdLib_EncDecHelper(sProcCryptoData_d *PpsCryptoVector, uint8_t PbCmd, uint8_t PbParam);

/**
* A common function for CmdLib_Encrypt and CmdLib_Decrypt.<br>
* Forms the APDU required for encryption/decryption and sends to the security chip for processing.<br>
*
* \param[in,out] PpsCryptoVector Pointer to structure containing Ciphertext and Plaintext
* \param[in] bCmd ProcUplink or ProcDownlink
* \param[in] bParam  Parameter to Encrypt/Decrypt data
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_LIB_INVALID_SESSIONID
* \retval  #CMD_LIB_INVALID_LEN
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_NULL_PARAM
*/
_STATIC_H int32_t CmdLib_EncDecHelper(sProcCryptoData_d *PpsCryptoVector, uint8_t PbCmd, uint8_t PbParam)
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wRespLen;
    uint16_t wDataRemaining;
    uint16_t wMaxDataLen;
    uint16_t wTotalEncDecLen =0;
    uint16_t wOffset = ADDITIONALBYTES_ENCDEC;
    uint16_t wMaxPlaintText;
    uint8_t bFragSeq ;
    uint8_t bSendTag,bRecvTag;
    uint8_t *pbResponse;
    uint8_t bGetError = TRUE;
    sApduData_d sApduData;

    do
    {
        //NULL checks
        if((NULL == PpsCryptoVector) || (NULL == PpsCryptoVector->sInData.prgbStream) ||
        (NULL == PpsCryptoVector->sOutData.prgbBuffer))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        //Verify the Session OID reference
        if((SESSION_ID_LOWER_VALUE > PpsCryptoVector->wSessionKeyOID) ||
        (SESSION_ID_HIGHER_VALUE < PpsCryptoVector->wSessionKeyOID))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_SESSIONID;
            break;
        }

        if((0 == PpsCryptoVector->wInDataLength) ||
        (PpsCryptoVector->sInData.wLen < (PpsCryptoVector->wInDataLength + OVERHEAD_UPDOWNLINK)))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_LEN;
            break;
        }

        //Check if plaintext length is more than the size of the out buffer
        if(PpsCryptoVector->wInDataLength > PpsCryptoVector->sOutData.wBufferLength)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }

        PpsCryptoVector->sOutData.wRespLength = 0;

        //Assign cmd,param
        sApduData.bCmd = PbCmd;
        sApduData.bParam = PbParam;

        wMaxPlaintText = wMaxCommsBuffer - OVERHEAD_UPDOWNLINK;

        //Data that is yet to be encrypted/decrypted
        wDataRemaining = PpsCryptoVector->wInDataLength;

        //Set the fragment sequence to start
        bFragSeq = (wDataRemaining>wMaxPlaintText)?(uint8_t)eStart:(uint8_t)eFinal;

        //Assign send or receive Tag
        if(PARAM_ENC_DATA == PbParam)
        {
            bSendTag = TAG_UNPROTECTED;
            bRecvTag = TAG_PROTECTED;
        }
        else
        {
            bSendTag= TAG_PROTECTED;
            bRecvTag = TAG_UNPROTECTED;
            bGetError = FALSE;
        }

        pbResponse = PpsCryptoVector->sOutData.prgbBuffer;

        while(wDataRemaining !=0)
        {
            //Maximum data that can be sent to chip in one APDU
            wMaxDataLen = (wDataRemaining>wMaxPlaintText)?wMaxPlaintText:wDataRemaining;

            //Assign InData memory pointer to the APDU Buffer in the Apdu structure
            sApduData.prgbAPDUBuffer = PpsCryptoVector->sInData.prgbStream + wOffset;

            //Form data and assign to apdu structure
            //Total payload length is Session ID Length + bytes for tag encoding + data
            sApduData.wPayloadLength = BYTES_SESSIONID + LEN_TAG_ENCODING + wMaxDataLen;

            //Add the session ID to the buffer
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(PpsCryptoVector->wSessionKeyOID >> BITS_PER_BYTE);
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + 1] = (uint8_t)PpsCryptoVector->wSessionKeyOID;

            //Add the encoding tag to the buffer
            sApduData.prgbAPDUBuffer[OFFSET_TAG] = (bSendTag | bFragSeq);
            sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN] = (uint8_t)(wMaxDataLen >> 8);
            sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN + 1] = (uint8_t)wMaxDataLen;

            //Payload data should already be present in input buffer as per documentation
            sApduData.prgbRespBuffer = pbResponse;
            sApduData.wResponseLength = PpsCryptoVector->sOutData.wBufferLength - wTotalEncDecLen;

            //Now Transmit data
            i4Status = TransceiveAPDU(&sApduData,bGetError);
            if(CMD_LIB_OK != i4Status)
            {
                if(PARAM_DEC_DATA == PbParam)
                {
                    i4Status = (int32_t)CMD_LIB_DECRYPT_FAILURE;
                }
                break;
            }

            //Remove 4 byte apdu header + tag encoding
            sApduData.wResponseLength -= OVERHEAD_ENCDEC_RESPONSE;

            //Sequence of flag for start,continue or final should be same that was sent
            if((bRecvTag|bFragSeq) != (*(sApduData.prgbRespBuffer + LEN_APDUHEADER)))
            {
                i4Status = (int32_t)CMD_LIB_INVALID_TAG;
                break;
            }

            //Extract the tag length field to get enc data length
            wRespLen = Utility_GetUint16(sApduData.prgbRespBuffer + LEN_APDUHEADER + 1);

            //Length validation for response length with the tag length
            if(sApduData.wResponseLength != wRespLen)
            {
                i4Status = (int32_t)CMD_LIB_INVALID_TAGLEN;
                break;
            }

            //Copy the data to output data buffer
            Utility_Memmove(pbResponse,sApduData.prgbRespBuffer+(LEN_APDUHEADER + LEN_TAG_ENCODING),wRespLen);

            wTotalEncDecLen += wRespLen;
            pbResponse += wRespLen;
            //Data remaining to encrypt
            wDataRemaining -= wMaxDataLen;
            //Since using the buffer provided by user, using offset to form next APDU command
            wOffset += wMaxDataLen;
            //If last fragment then make flag final else let it continue
            bFragSeq = (wDataRemaining>wMaxPlaintText)?(uint8_t)eContinue:(uint8_t)eFinal;
        }

        //Update on success only
        if(CMD_LIB_OK == i4Status)
        {
            PpsCryptoVector->sOutData.wRespLength = wTotalEncDecLen;
        }

    }while(FALSE);

    return i4Status;
}
#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH*/

#ifdef MODULE_ENABLE_TOOLBOX
/**
 * This function returns Hash Algorithm information such as Hash output length, Hash context data.
 *
 * \param[in]        PeHashAlgo     Hash Algorithm type
 * \param[in,out]    PpsHashinfo    Pointer to a structure containing Hash algorithm information
 *
 * \retval  #CMD_LIB_OK
 * \retval  #CMD_LIB_ERROR
 */
_STATIC_H int32_t Get_HashInfo(eHashAlg_d PeHashAlgo, const sHashinfo_d** PppsHashinfo )
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint8_t bCount;

    //Search the table for the given session key id
    for(bCount= 0;bCount<(sizeof(sHashInfo)/sizeof(sHashinfo_d));bCount++)
    {
        if(PeHashAlgo == sHashInfo[bCount].eHashAlgo)
        {
            *PppsHashinfo = &sHashInfo[bCount];
            i4Status = (int32_t) CMD_LIB_OK;
            break;
        }
    }
    return i4Status;
}

/**
 * This function returns key size based on the algorithm.
 *
 * \param[in]        PeAlgoIden     Algorithm identifier type
 * \param[in,out]    PpsAlgoinfo    Pointer to a structure containing key size of algorithm identifier
 *
 * \retval  #CMD_LIB_OK
 * \retval  #CMD_LIB_ERROR
 */
_STATIC_H int32_t Get_KeySize(eAlgId_d PeAlgoIden,uint8_t* pbKeySize )
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint8_t bCount;

    //Search the table for the given session key id
    for(bCount= 0;bCount<(sizeof(sAlgoInfo)/sizeof(sAlgoInfo_d));bCount++)
    {
        if(PeAlgoIden == sAlgoInfo[bCount].eAlgKeyId)
        {
            *pbKeySize = sAlgoInfo[bCount].bKeyLen;
            i4Status = (int32_t) CMD_LIB_OK;
            break;
        }
    }
    return i4Status;
}
#endif/*MODULE_ENABLE_TOOLBOX*/

/// @endcond

/**
* Sets the OPTIGA Comms context provided by user application in the command libary.
*
* <br>
* \param[in] p_input_optiga_comms Pointer to OPTIGA comms context
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
*/
void CmdLib_SetOptigaCommsContext(const optiga_comms_t *p_input_optiga_comms)
{
    p_optiga_comms = (optiga_comms_t*)p_input_optiga_comms;
}

/**
* Opens the Security Chip Application. The Unique Application Identifier is used internally by
* the function while forming a command APDU.
*
*\param[in] PpsOpenApp Pointer to a structure #sOpenApp_d containing inputs for opening application on security chip
*
* Notes:
* - This function must be mandatorily invoked before starting any interactions with security Chip after the reset.
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INVALID_PARAM
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_OpenApplication(const sOpenApp_d* PpsOpenApp)
{
/// @cond hidden
#define OPEN_APDU_BUF_LEN    25
/// @endcond

    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint8_t rgbUID[] = {UNIQUE_APP_IDENTIFIER};
    sApduData_d sApduData;

    do
    {
        INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer,OPEN_APDU_BUF_LEN);

        if(NULL == PpsOpenApp)
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }
        //Validate option for opening application
        if(eInit != PpsOpenApp->eOpenType)
        {
            i4Status = (int32_t)CMD_LIB_INVALID_PARAM;
            break;
        }

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        //Open application command with MSB set to flush the last error code
        sApduData.bCmd = CMD_OPEN_APP | CMD_CODE_MSB_SET ;
        sApduData.bParam = (uint8_t)PpsOpenApp->eOpenType;
        sApduData.wPayloadLength = sizeof(rgbUID);
        sApduData.wResponseLength = OPEN_APDU_BUF_LEN;
        OCP_MEMCPY(sApduData.prgbAPDUBuffer+OFFSET_PAYLOAD, rgbUID, sizeof(rgbUID));
        i4Status = TransceiveAPDU(&sApduData,FALSE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //Read Max comms buffer size if not already read
        if(INVALID_MAX_COMMS_BUFF_SIZE == wMaxCommsBuffer)
        {
            //Get Maximum Comms buffer size
            i4Status = GetMaxCommsBuffer();
        }
    }while(FALSE);

/// @cond hidden
#undef OPEN_APDU_BUF_LEN
/// @endcond

    return i4Status;
}

#ifdef MODULE_ENABLE_READ_WRITE
/**
* Reads data or metadata of the specified data object by issuing GetDataObject command based on input parameters.
*
* <br>
* Notes:
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.<br>
* - The function does not verify if the read access is permitted for the data object.<br>
*
*\param[in] PpsGDVector Pointer to Get Data Object inputs
*\param[in,out] PpsResponse Pointer to Response structure
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_LENZERO_ERROR
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_GetDataObject(const sGetData_d *PpsGDVector, sCmdResponse_d *PpsResponse)
{
/// @cond hidden
#define ALLOCATE_ADDITIONAL_BYTES   6       // hdr(4) + oid(2)
/// @endcond

    //lint --e{818} suppress "PpsResponse is out parameter"
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wTotalRecvLen = 0;
    uint16_t wReadLen = 0;
    uint16_t wOffset = 0;
    sApduData_d sApduData;
    sApduData.prgbAPDUBuffer = NULL;
    do
    {
//To use stack memory for APDU buffer, define STACK_ALLOC macro locally and else heap memory is used
#ifdef STACK_ALLOC
        #error "Implement the inilization of stack memory for the required buffer"
        //INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer, wLen);    //wLen to be replaced with the required const length
#else
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,wMaxCommsBuffer + ALLOCATE_ADDITIONAL_BYTES);
#endif

        if((NULL == PpsGDVector)||(NULL == PpsResponse)||(NULL == PpsResponse->prgbBuffer))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }
        if(0x00 == PpsResponse->wBufferLength)
        {
            i4Status = (int32_t)CMD_LIB_LENZERO_ERROR;
            break;
        }
        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer+ALLOCATE_ADDITIONAL_BYTES;
        sApduData.bCmd = CMD_GETDATA;
        //copy OID
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(PpsGDVector->wOID >> BITS_PER_BYTE);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD +1] = (uint8_t)PpsGDVector->wOID;
        //set param, payload length and offset ,if reading data
        if(eDATA == PpsGDVector->eDataOrMdata)
        {
            sApduData.bParam = PARAM_GET_DATA;
            sApduData.wPayloadLength = LEN_PL_OIDDATA;
            wOffset = PpsGDVector->wOffset;
        }
        //set param and payload length ,if reading metadata
        else if(eMETA_DATA == PpsGDVector->eDataOrMdata)
        {
            sApduData.bParam = PARAM_GET_METADATA;
            sApduData.wPayloadLength = LEN_PL_OID;
        }
        else
        {
            i4Status = (int32_t)CMD_LIB_INVALID_PARAM;
            break;
        }

        do
        {
            if(eDATA == PpsGDVector->eDataOrMdata)
            {
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_OID] = (uint8_t)(wOffset >> BITS_PER_BYTE);
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_OID +1] = (uint8_t)wOffset;

                //copy read length
                wReadLen = MIN((wMaxCommsBuffer-LEN_APDUHEADER),(PpsGDVector->wLength-wTotalRecvLen));
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_OID + BYTES_OFFSET] = (uint8_t)(wReadLen >> BITS_PER_BYTE);
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_OID + BYTES_OFFSET +1] = (uint8_t)wReadLen;
            }

            sApduData.wResponseLength = wMaxCommsBuffer;

            i4Status = TransceiveAPDU(&sApduData,TRUE);
            if(CMD_LIB_OK != i4Status)
            {
                break;
            }
            //strip 4 byte apdu header
            sApduData.wResponseLength -= LEN_APDUHEADER;

            //Copy read data
            if(((PpsResponse->wBufferLength-wTotalRecvLen) < sApduData.wResponseLength))
            {
                i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
                break;
            }
            OCP_MEMCPY(PpsResponse->prgbBuffer+wTotalRecvLen,sApduData.prgbRespBuffer+LEN_APDUHEADER,sApduData.wResponseLength);
            //Update total received data
            wTotalRecvLen += sApduData.wResponseLength;
            //increment the offset to get data from
            wOffset += sApduData.wResponseLength;

            //If requested data is read
            if(wTotalRecvLen == PpsGDVector->wLength)
            {
                break;
            }
        //continue, if total requested data not yet received and more data available for reading
        }while(wReadLen == sApduData.wResponseLength);

        if((CMD_LIB_OK != i4Status)&&((wTotalRecvLen == 0)||
        (ERR_DATA_OUT_OF_BOUND != (i4Status^(int32_t)CMD_DEV_ERROR))))
        {
            //Clear existing data
            OCP_MEMSET(PpsResponse->prgbBuffer,0,wTotalRecvLen);
            PpsResponse->wRespLength = 0;
            break;
        }

        PpsResponse->wRespLength = wTotalRecvLen;
        i4Status = (int32_t)CMD_LIB_OK;
    }while(FALSE);

#ifndef STACK_ALLOC
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);
#else
    #undef STACK_ALLOC
#endif

///@cond hidden
#undef ALLOCATE_ADDITIONAL_BYTES
///@endcond

    return i4Status;
}

/**
* Writes data or metadata to the specified data object by issuing SetDataObject command based on input parameters.
*
* <br>
* Notes: <br>
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.<br>
*
* - The function does not verify if the write access permitted for the data object.
*
* - While writing metadata, the metadata must be specified in an already TLV encoded
*   byte array format. For example, to set LcsO to operational the value passed by
*   the user must be 0x20 0x03 0xC0, 0x01, 0x07. <br>
*
* - The function does not validate if the provided input data bytes are correctly
*   formatted. For example, while setting LcsO to operational, function does not
*   verify if the value is indeed 0x07. <br>
*
* - In case of failure,it is possible that partial data is written into the data object.<br>
*   In such a case, the user should decide if the data has to be re-written.
*
*\param[in] PpsSDVector Pointer to Set Data Object inputs
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INVALID_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_SetDataObject(const sSetData_d *PpsSDVector)
{
/// @cond hidden
#define BUFFER_SIZE (wMaxCommsBuffer)
/// @endcond

    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wTotalWriteLen = 0;
    uint16_t wWriteLen = 0;
    uint16_t wOffset;
    sApduData_d sApduData;
    sApduData.prgbAPDUBuffer = NULL;

    do
    {
//To use stack memory for APDU buffer, define STACK_ALLOC macro locally and else heap memory is used
#ifdef STACK_ALLOC
        #error "Implement the inilization of stack memory for the required buffer"
        //INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer, wLen);    //wLen to be replaced with the required const length
#else
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,BUFFER_SIZE);
#endif

        if((NULL == PpsSDVector)||(NULL == PpsSDVector->prgbData))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }
        if(0x00 == PpsSDVector->wLength)
        {
            i4Status = (int32_t)CMD_LIB_LENZERO_ERROR;
            break;
        }

        sApduData.bCmd = CMD_SETDATA;

        if((eDATA == PpsSDVector->eDataOrMdata)&&
        (eWRITE == PpsSDVector->eWriteOption))
        {
            sApduData.bParam = PARAM_SET_DATA;
        }
        else if((eDATA == PpsSDVector->eDataOrMdata)&&
        (eERASE_AND_WRITE == PpsSDVector->eWriteOption))
        {
            sApduData.bParam = PARAM_SET_DATA_ERASE;
        }
        else if((eMETA_DATA == PpsSDVector->eDataOrMdata)&&
        (eWRITE == PpsSDVector->eWriteOption))
        {
            sApduData.bParam = PARAM_SET_METADATA;
        }
        else
        {
            i4Status = (int32_t)CMD_LIB_INVALID_PARAM;
            break;
        }

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer+7;
        //copy OID
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(PpsSDVector->wOID >> BITS_PER_BYTE);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + 1] = (uint8_t)PpsSDVector->wOID;
        wOffset = PpsSDVector->wOffset;
        do
        {
            //While chaining for erase & write option, all subsequent write must be only write operation
            if((sApduData.bParam == PARAM_SET_DATA_ERASE)&&
            (wTotalWriteLen != 0))
            {
                sApduData.bParam = PARAM_SET_DATA;
            }
/// @cond hidden
#define OVERHEAD (OFFSET_PAYLOAD+BYTES_OID+BYTES_OFFSET)
/// @endcond

            wWriteLen = MIN((wMaxCommsBuffer-OVERHEAD),(PpsSDVector->wLength-wTotalWriteLen));

            //set data payload length is 4(OID length + offset length) plus length of data to write
            sApduData.wPayloadLength = BYTES_OID + BYTES_OFFSET + wWriteLen;

            //copy offset
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_OID] = (uint8_t)(wOffset >> BITS_PER_BYTE);
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_OID + 1] = (uint8_t)wOffset;
            //copy the data

            OCP_MEMCPY(sApduData.prgbAPDUBuffer+OVERHEAD,PpsSDVector->prgbData+wTotalWriteLen,wWriteLen);

            //Set Response buffer length
            sApduData.wResponseLength = BUFFER_SIZE;

            i4Status = TransceiveAPDU(&sApduData,TRUE);
            if(CMD_LIB_OK != i4Status)
            {
                break;
            }

            wTotalWriteLen += wWriteLen;
            wOffset += wWriteLen;
        }while(wTotalWriteLen != PpsSDVector->wLength);
    }while(FALSE);

#ifndef STACK_ALLOC
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);
#else
    #undef STACK_ALLOC
#endif

/// @cond hidden
#undef OVERHEAD
#undef BUFFER_SIZE
/// @endcond

    return i4Status;
}

/**
* Reads maximum communication buffer size supported by the security chip.<br>
*
* Notes:
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.<br>
* - The function does not verify if the read access is permitted for the data object.<br>
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
*/
uint16_t CmdLib_GetMaxCommsBufferSize(Void)
{
    return wMaxCommsBuffer;
}
#endif /* MODULE_ENABLE_READ_WRITE */

/**
* Gets the signature generated by Security Chip. The message to be signed is provided by the user.<br>
* The following commands are issued in the sequence.<br>
* - SetAuthScheme : To set authentication scheme and the private key to be used <br>
* - SetAuthMsg : To write the message to Security Chip that must be digitally signed.<br>
* - GetAuthMsg : To read the digitally signed message from Security Chip.<br>
*
* <br>
* Notes: <br>
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.<br>
* - The function just returns the signature without verifying it.
* - The private key to be used in set auth scheme is passed in \ref #sAuthMsg_d.wOIDDevPrivKey.
* - The \ref #sAuthMsg_d.prgbRnd and \ref #sAuthMsg_d.wRndLength carry the challenge to be signed.
* - The length of challenge should be between 8 and 256 bytes. If the length of challenge is out of this range, #CMD_LIB_INVALID_LEN error is returned.<br>
*
*\param[in] PpsAuthMsg Pointer to Get Signature Object inputs
*\param[in,out] PpsResponse Pointer to Response structure
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_LIB_NULL_PARAM
* \retval  #CMD_LIB_INVALID_LEN
*/
int32_t CmdLib_GetSignature(const sAuthMsg_d *PpsAuthMsg, sCmdResponse_d *PpsResponse)
{
/// @cond hidden
#define STACK_ALLOC
#define GETSIGN_APDU_BUF_LEN    270             //4 (cmd header) + 256 (Max Payload) + additional 10
/// @endcond

    //lint --e{818} suppress "PpsResponse is out parameter"
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;
    sAuthScheme_d sAuthScheme;

    do
    {
//To use stack memory for APDU buffer, define STACK_ALLOC locally else heap memory is used
#ifdef STACK_ALLOC
        INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer,GETSIGN_APDU_BUF_LEN);
#else
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,GETSIGN_APDU_BUF_LEN);
#endif

        if((NULL == PpsAuthMsg)||(NULL == PpsAuthMsg->prgbRnd)||
        (NULL == PpsResponse)||(NULL == PpsResponse->prgbBuffer))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        //Check if Challenge length is in between 8 to 256
        if((CHALLENGE_MIN_LEN > PpsAuthMsg->wRndLength) || (CHALLENGE_MAX_LEN < PpsAuthMsg->wRndLength))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_LEN;
            break;
        }

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        sApduData.wResponseLength = GETSIGN_APDU_BUF_LEN;

        //Set Auth scheme
        sAuthScheme.eAuthScheme = eECDSA;
        sAuthScheme.wDevicePrivKey = PpsAuthMsg->wOIDDevPrivKey;
        i4Status = CmdLib_SetAuthScheme(&sAuthScheme);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //Set Auth msg
        sApduData.bCmd = CMD_SETAUTH_MSG;
        sApduData.bParam = PARAM_SET_AUTH_MSG;
        sApduData.wPayloadLength = PpsAuthMsg->wRndLength;
        sApduData.wResponseLength = GETSIGN_APDU_BUF_LEN;
        //copy the random number
        OCP_MEMCPY(sApduData.prgbAPDUBuffer+OFFSET_PAYLOAD,PpsAuthMsg->prgbRnd,PpsAuthMsg->wRndLength);
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //Get Auth msg
        sApduData.bCmd = CMD_GETAUTH_MSG;
        sApduData.bParam = PARAM_GET_AUTH_MSG;
        sApduData.wPayloadLength = 0;
        sApduData.wResponseLength = GETSIGN_APDU_BUF_LEN;
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //strip 4 byte apdu header
        sApduData.wResponseLength -= LEN_APDUHEADER;
        PpsResponse->wRespLength = sApduData.wResponseLength;
        if(PpsResponse->wBufferLength < sApduData.wResponseLength)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }
        OCP_MEMCPY(PpsResponse->prgbBuffer,sApduData.prgbAPDUBuffer+LEN_APDUHEADER,sApduData.wResponseLength);

    }while(FALSE);
#ifndef STACK_ALLOC
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);
#else
    #undef STACK_ALLOC
#endif

///@cond hidden
#undef GETSIGN_APDU_BUF_LEN
///@endcond

    return i4Status;
}

/**
 * Gets random bytes generated by the Security Chip.<br>
 *
 * <br>
 * Notes: <br>
 * - Command chaining is not supported in this API.<br>
 * - If the requested length of random bytes is either more than communication buffer size or more than the buffer size in PpsResponse,#CMD_LIB_INSUFFICIENT_MEMORY error is returned.<br>
 *
 *\param[in]        PpsRng      Pointer to sRngOptions_d to specify random number generation
 *\param[in,out]    PpsResponse Pointer to sCmdResponse_d to store random number
 *
 * \retval  #CMD_LIB_OK
 * \retval  #CMD_LIB_ERROR
 * \retval  #CMD_LIB_INSUFFICIENT_MEMORY
 * \retval  #CMD_LIB_LENZERO_ERROR
 * \retval  #CMD_DEV_ERROR
 * \retval  #CMD_LIB_NULL_PARAM
 */
int32_t CmdLib_GetRandom(const sRngOptions_d *PpsRng, sCmdResponse_d *PpsResponse)
{
    //lint --e{818} suppress "PpsResponse is out parameter"
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;

    do
    {
        //Initial value set it to NULL
        sApduData.prgbAPDUBuffer = NULL;

        if((NULL == PpsRng)||(NULL == PpsResponse)||(NULL == PpsResponse->prgbBuffer))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }
        if(0x00 == PpsResponse->wBufferLength)
        {
            i4Status = (int32_t)CMD_LIB_LENZERO_ERROR;
            break;
        }

        //If the length of requested random bytes is more than the maximum comms buffer size
        if((wMaxCommsBuffer) < (LEN_APDUHEADER + PpsRng->wRandomDataLen))
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }
        PpsResponse->wRespLength = 0;
        sApduData.bCmd = CMD_GET_RND;
        sApduData.bParam = (uint8_t)PpsRng->eRngType;
        sApduData.wPayloadLength = LEN_PL_OID;

        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer, (LEN_APDUHEADER + PpsRng->wRandomDataLen));

        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(PpsRng->wRandomDataLen >> BITS_PER_BYTE);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + 1] = (uint8_t)PpsRng->wRandomDataLen;
        sApduData.wResponseLength = (LEN_APDUHEADER + PpsRng->wRandomDataLen);

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;

        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }
        //strip 4 byte apdu header
        sApduData.wResponseLength -= LEN_APDUHEADER;
        if(PpsResponse->wBufferLength < sApduData.wResponseLength)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }
        OCP_MEMCPY(PpsResponse->prgbBuffer,sApduData.prgbAPDUBuffer+LEN_APDUHEADER,sApduData.wResponseLength);
        PpsResponse->wRespLength = sApduData.wResponseLength;

    }while(FALSE);

    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);

    return i4Status;
}

/**
* Sets the Authentication Scheme by issuing SetAuthScheme command to Security Chip.
*
* <br>
* Notes: <br>
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.<br>
* - Currently only 1 session OID (0xE100) is supported by the security chip.
*
*\param[in] PpsAuthVector Pointer to Authentication Scheme data
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INVALID_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_SetAuthScheme(const sAuthScheme_d *PpsAuthVector)
{
/// @cond hidden
#define SET_AUTH_SCHEME_APDU_BUF_LEN    10
/// @endcond

    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;
    uint16_t wPayloadSize;

    do
    {
        INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer, SET_AUTH_SCHEME_APDU_BUF_LEN);

        if(NULL == PpsAuthVector)
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        //verify the param
        if(eDTLSClient == PpsAuthVector->eAuthScheme)
        {
            //Verify the range of session ID
            if((SESSION_ID_LOWER_VALUE > PpsAuthVector->wSessionKeyId)
                || (SESSION_ID_HIGHER_VALUE < PpsAuthVector->wSessionKeyId))
            {
                i4Status = (int32_t)CMD_LIB_INVALID_SESSIONID;
                break;
            }
            wPayloadSize = 4;
        }
        else if(eECDSA == PpsAuthVector->eAuthScheme)
        {
            wPayloadSize = 2;
        }
        else
        {
            i4Status = (int32_t)CMD_LIB_INVALID_PARAM;
            break;
        }

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;

        //Form the command
        //Assign Cmd,param,payload length
        sApduData.bCmd = CMD_SETAUTH_SCM;
        sApduData.bParam = (uint8_t)PpsAuthVector->eAuthScheme;
        sApduData.wPayloadLength = wPayloadSize;

        //Assign the device private key OID at payload offset
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(PpsAuthVector->wDevicePrivKey >> BITS_PER_BYTE);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD+1] = (uint8_t)PpsAuthVector->wDevicePrivKey;

        sApduData.wResponseLength = SET_AUTH_SCHEME_APDU_BUF_LEN;

        if(eDTLSClient == PpsAuthVector->eAuthScheme)
        {
            //Assign the session OID
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD+2] = (uint8_t)(PpsAuthVector->wSessionKeyId >> BITS_PER_BYTE);
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD+3] = (uint8_t)PpsAuthVector->wSessionKeyId;
        }

        //Transmit the Data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

    }while(FALSE);

/// @cond hidden
#undef SET_AUTH_SCHEME_APDU_BUF_LEN
/// @endcond

    return i4Status;
}

#ifdef MODULE_ENABLE_TOOLBOX
/**
* Calculates the hash of input data by using the Security Chip.<br>
*
* Input:<br>
* - Provide the required type of input data for hashing. Use \ref sCalcHash_d.eHashDataType with the following options,
*   - eDataStream : Indicates, sDataStream is considered as hash input.
*   - eOIDData : Indicates, sOIDData is considered for hash input.
*
* - Provide the input to import/export the hash context. Use \ref sContextInfo_d.eContextAction with the following options,
*   - #eImport : Import hash context to perform the hash.
*   - #eExport : Export current active hash context.
*   - #eImportExport : Import hash context and Export back the context after hashing.
*   - #eUnused : Context data import/export feature is not used. This option is also recommended for #eHashSequence_d as #eStartFinalizeHash or #eTerminateHash.
*
* Output:<br>
* - Successful API execution,
*   - Hash is returned in sOutHash only if #eHashSequence_d is #eStartFinalizeHash,#eIntermediateHash or #eFinalizeHash.<br>
*   - Hash context data is returned only if \ref sContextInfo_d.eContextAction is #eExport or #eImportExport.<br>
*
* Notes: <br>
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.<br>
* - #eTerminateHash in #eHashSequence_d is used to terminate any existing hash session. Any input data or hash context options supplied with this sequence is ignored.
* - Sequences for generating a hash successfully can be as follows:<br>
*     - #eStartHash,#eFinalizeHash<br>
*     - #eStartHash,#eContinueHash (single or multiple),#eFinalizeHash<br>
*     - #eStartFinalizeHash<br>
*     - #eStartHash,#eIntermediateHash,#eContinueHash,#eFinalizeHash<br>
*
* - If the memory buffer is not sufficient to store output hash/hash context or the data to be sent to security chip is more than communication buffer,#CMD_LIB_INSUFFICIENT_MEMORY error is retured.
* - This API does not maintain any state of hashing operations.<br>
* - There is no support for chaining while sending data therefore in order to avoid communication buffer overflow, the user must take care of fragmenting the data for hashing.<br>
*   Use the API #CmdLib_GetMaxCommsBufferSize to check the maximum communication buffer size supported by the security chip. In addition, the overhead for command APDU header and
*   TLV encoding must be considered as explained below.<br>
*
*   Read the maximum communication buffer size using the API #CmdLib_GetMaxCommsBufferSize() and store in a variable <b>"wMaxCommsBuffer"</b><br>
*   Substract the header overheads and hash context size(depends on applicable Hash algorithm) respectively from wMaxCommsBuffer. The result gives the Available_Size to frame the hash data input.<br>
*
*   - Only hash calculation : <br>
*   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Available_Size = (wMaxCommsBuffer - #CALC_HASH_FIXED_OVERHEAD_SIZE)<br>
*   - Import context to security chip and calculate hash  : <br>
*   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Available_Size = (wMaxCommsBuffer - #CALC_HASH_FIXED_OVERHEAD_SIZE - #CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE - #CALC_HASH_SHA256_CONTEXT_SIZE)<br>
*   - Calulate hash and export context out of security chip : <br>
*   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Available_Size = (wMaxCommsBuffer - #CALC_HASH_FIXED_OVERHEAD_SIZE - #CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE)<br>
*   - Import context to security chip, calculate hash and export context out of security chip :<br>
*   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Available_Size = (wMaxCommsBuffer - #CALC_HASH_FIXED_OVERHEAD_SIZE - #CALC_HASH_IMPORT_AND_EXPORT_OVERHEAD_SIZE - #CALC_HASH_SHA256_CONTEXT_SIZE)<br>
*
*
* \param[in,out] PpsCalcHash Pointer to #sCalcHash_d that contains information to calculate hash
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_NULL_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_EXEC_ERROR
* \retval  #CMD_DEV_ERROR
*/
int32_t CmdLib_CalcHash(sCalcHash_d* PpsCalcHash)
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;
    eDataType_d eHashDataType;
    uint16_t wMemoryAllocLen;
    uint16_t wInDataLen;
    const sHashinfo_d* psHashinfo;
    uint16_t wOptTagLen = 0;
    uint16_t wOptTagOffset = 0;
    uint16_t wOffset;
    uint16_t wBufferLen;

    do
    {
        //Initial value set it to NULL
        sApduData.prgbAPDUBuffer = NULL;

        //Check for NULL inputs
        if(NULL == PpsCalcHash)
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        i4Status = Get_HashInfo(PpsCalcHash->eHashAlg, &psHashinfo);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        //For eHashSequence_d as eFinalizeHash,validate sOutData
        if(((eFinalizeHash == PpsCalcHash->eHashSequence) || (eStartFinalizeHash == PpsCalcHash->eHashSequence) ||
            (eIntermediateHash == PpsCalcHash->eHashSequence)) && (NULL == PpsCalcHash->sOutHash.prgbBuffer))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        //Validate the Context data buffer if optional tag is provided
        if((eUnused != PpsCalcHash->sContextInfo.eContextAction) && (NULL == PpsCalcHash->sContextInfo.pbContextData))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

/// @cond hidden
#define INDATA_LEN_OID     (BYTES_OID + BYTES_LENGTH + BYTES_OFFSET)
#define NIBBLE_LEN      4
/// @endcond

        eHashDataType = PpsCalcHash->eHashDataType;

        //For eHashDataType_d as eDataStream, validate psDataStream
        if(eTerminateHash == PpsCalcHash->eHashSequence)
        {
            wInDataLen = 0;
            eHashDataType = eDataStream;
        }
        else if(eDataStream == eHashDataType)
        {
            if(NULL == PpsCalcHash->sDataStream.prgbStream)
            {
                i4Status = (int32_t)CMD_LIB_NULL_PARAM;
                break;
            }
            //Set the total data length as input length
            wInDataLen = PpsCalcHash->sDataStream.wLen;
        }
        else
        {
            //Set the total data length as 6 bytes (2 byte OID + 2 bytes Offset + 2 bytes Length)
            wInDataLen = INDATA_LEN_OID;
        }

        //Calculate the offset and length for optional tags in the command APDU
        if(eUnused != PpsCalcHash->sContextInfo.eContextAction)
        {
            wOptTagLen = (eExport != PpsCalcHash->sContextInfo.eContextAction)? (CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE + PpsCalcHash->sContextInfo.dwContextLen) : (CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE);
            //Total length of optional tag which includes tag, length and data
            if(eImportExport == PpsCalcHash->sContextInfo.eContextAction)
            {
                wOptTagLen += CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE;
            }
            wOptTagOffset = (eDataStream == PpsCalcHash->eHashDataType)? (OFFSET_PAYLOAD + BYTES_SEQ + BYTES_LENGTH + wInDataLen) :
                                (OFFSET_PAYLOAD + BYTES_SEQ + BYTES_LENGTH + INDATA_LEN_OID);
        }

        //Validate the size of input data with the Communication buffer
        if((wInDataLen + wOptTagLen + CALC_HASH_FIXED_OVERHEAD_SIZE) > wMaxCommsBuffer)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }

        //Form data
        sApduData.bCmd = CMD_CALCHASH;
        sApduData.bParam = (uint8_t)PpsCalcHash->eHashAlg;

        wMemoryAllocLen = CALC_HASH_FIXED_OVERHEAD_SIZE + wInDataLen + wOptTagLen;

        wBufferLen = CALC_HASH_FIXED_OVERHEAD_SIZE;

        //Check to validate sufficient memory to store the output
        //If Hash is the output
        if((eFinalizeHash == PpsCalcHash->eHashSequence) || (eStartFinalizeHash == PpsCalcHash->eHashSequence) ||
            (eIntermediateHash == PpsCalcHash->eHashSequence))
        {
             wBufferLen +=  psHashinfo->bHashLen;
        }
        //If context data is output
        if((eExport == PpsCalcHash->sContextInfo.eContextAction) || (eImportExport == PpsCalcHash->sContextInfo.eContextAction))
        {
            wBufferLen += psHashinfo->wHashCntx;
        }

        if(wBufferLen > wMemoryAllocLen)
        {
            wMemoryAllocLen = wBufferLen;
        }

        // Allocate the memory
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer, wMemoryAllocLen);

        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(((uint8_t)eHashDataType << NIBBLE_LEN) | (uint8_t) PpsCalcHash->eHashSequence);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ] = (uint8_t)(wInDataLen >> 8);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + 1] = (uint8_t)wInDataLen ;

        if(eTerminateHash != PpsCalcHash->eHashSequence)
        {
            //If the DataType is Data stream, copy the input data to the buffer
            if(eDataStream == eHashDataType)
            {
                OCP_MEMCPY(&sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + BYTES_LENGTH], PpsCalcHash->sDataStream.prgbStream,
                wInDataLen);
            }
            else
            {
                //If the Data type is OID, copy the OID information to the buffer
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + BYTES_LENGTH] = (uint8_t)(PpsCalcHash->sOIDData.wOID >> 8);
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + BYTES_LENGTH + 1] = (uint8_t)PpsCalcHash->sOIDData.wOID;
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + BYTES_LENGTH + BYTES_OID] = (uint8_t)(PpsCalcHash->sOIDData.wOffset >> 8);
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + BYTES_LENGTH + BYTES_OID + 1] = (uint8_t)PpsCalcHash->sOIDData.wOffset;
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + INDATA_LEN_OID] = (uint8_t)(PpsCalcHash->sOIDData.wLength >> 8);
                sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + BYTES_SEQ + INDATA_LEN_OID + 1] = (uint8_t)(PpsCalcHash->sOIDData.wLength);
            }

/// @cond hidden
#define OFFSET_EXPORT_CNTXTAG   (wOptTagOffset + BYTES_SEQ+ BYTES_LENGTH + PpsCalcHash->sContextInfo.dwContextLen)
/// @endcond

            //If the optional tag is either eImport or eImportAndExport, 0x06 tag is sent as part of command APDU
            if((eImportExport == PpsCalcHash->sContextInfo.eContextAction) ||
            (eImport == PpsCalcHash->sContextInfo.eContextAction))
            {
                sApduData.prgbAPDUBuffer[wOptTagOffset] = (uint8_t)eImport;
                sApduData.prgbAPDUBuffer[wOptTagOffset + BYTES_SEQ] = (uint8_t)(PpsCalcHash->sContextInfo.dwContextLen >> 8);
                sApduData.prgbAPDUBuffer[wOptTagOffset + BYTES_SEQ + 1] = (uint8_t)(PpsCalcHash->sContextInfo.dwContextLen);
                OCP_MEMCPY(&sApduData.prgbAPDUBuffer[wOptTagOffset + BYTES_SEQ + BYTES_LENGTH], PpsCalcHash->sContextInfo.pbContextData,
                            PpsCalcHash->sContextInfo.dwContextLen);
            }

            //If the optional tag is either eExport or eImportAndeExport, 0x07 tag is sent as part of command APDU
            if((eImportExport == PpsCalcHash->sContextInfo.eContextAction) ||
            (eExport == PpsCalcHash->sContextInfo.eContextAction))
            {
                wOffset = (eExport == PpsCalcHash->sContextInfo.eContextAction)?wOptTagOffset : OFFSET_EXPORT_CNTXTAG;
                sApduData.prgbAPDUBuffer[wOffset] = (uint8_t)eExport;
                sApduData.prgbAPDUBuffer[wOffset + BYTES_SEQ] = 0x00;
                sApduData.prgbAPDUBuffer[wOffset + BYTES_SEQ + 1] =0x00;
            }
        }

        //set data payload length is 3(BYTES_SEQ + BYTES_LENGTH) plus length of data to write
        sApduData.wPayloadLength = BYTES_SEQ + BYTES_LENGTH + wInDataLen + wOptTagLen;

        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;

        sApduData.wResponseLength = wMemoryAllocLen;

        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

/// @cond hidden
#define TAG_HASH_OUTPUT         0x01
#define TAG_CONTEXT_OUTPUT      0x06
/// @endcond

        sApduData.wResponseLength -= LEN_APDUHEADER;

        //Validate the output buffer size if tag received on reponse is 0x01 and
        //copy the hash data to sOutput buffer
        if((TAG_HASH_OUTPUT == (*(sApduData.prgbRespBuffer + LEN_APDUHEADER))) && (sApduData.wResponseLength != 0))
        {
            //Length check for sOutData
            if((psHashinfo->bHashLen) > PpsCalcHash->sOutHash.wBufferLength)
            {
                i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
                break;
            }

            PpsCalcHash->sOutHash.wRespLength = Utility_GetUint16(sApduData.prgbRespBuffer + LEN_APDUHEADER + BYTES_SEQ);
            OCP_MEMCPY(PpsCalcHash->sOutHash.prgbBuffer, (sApduData.prgbRespBuffer + CALC_HASH_FIXED_OVERHEAD_SIZE), PpsCalcHash->sOutHash.wRespLength);
        }

        //Validate the Context buffer size if the 0x06 context data tag is there in response and
        //copy the context data to pbContextData buffer
        if((TAG_CONTEXT_OUTPUT == (*(sApduData.prgbRespBuffer + LEN_APDUHEADER))) && (sApduData.wResponseLength != 0))
        {
            //Length check for Context Data
            if((psHashinfo->wHashCntx) > PpsCalcHash->sContextInfo.dwContextLen)
            {
                i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
                break;
            }

            PpsCalcHash->sContextInfo.dwContextLen = Utility_GetUint16(sApduData.prgbRespBuffer + LEN_APDUHEADER + BYTES_SEQ);
            OCP_MEMCPY(PpsCalcHash->sContextInfo.pbContextData, (sApduData.prgbRespBuffer + CALC_HASH_FIXED_OVERHEAD_SIZE), PpsCalcHash->sContextInfo.dwContextLen);
        }

    }while(FALSE);

    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);

/// @cond hidden
#undef INDATA_LEN_OID
#undef NIBBLE_LEN
#undef OFFSET_EXPORT_CNTXTAG
#undef TAG_HASH_OUTPUT
#undef TAG_CONTEXT_OUTPUT
/// @endcond

    return i4Status;
}

/**
* Verifies the signature over the input digest by using the Security Chip.<br>
*
* Input:<br>
* - For eVerifyDataType
*   - #eDataStream indicates that sPubKeyInput is considered for signature verification.<br>
*   - #eOIDData indicates that wOIDPubKey is considered for signature verification.<br>
*
* Output:<br>
* - Successful signature verification returns #CMD_LIB_OK.<br>
*
* Notes: <br>
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.<br>
* - If the the data to be sent to security chip is more than communication buffer,#CMD_LIB_INSUFFICIENT_MEMORY is returned. Refer OPTIGA_Trust_X_SolutionReferenceManual_v1.x.pdf for more details.
*
* \param[in]     PpsVerifySign  Pointer to information for verifying signature
* \param[in,out] PpsDigest      pointer to a blob which holds the Digest
* \param[in,out] PpsSignature   pointer to a blob which holds the Signature to be verified
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_NULL_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_EXEC_ERROR
* \retval  #CMD_DEV_ERROR
*/
int32_t CmdLib_VerifySign(const sVerifyOption_d* PpsVerifySign,const sbBlob_d * PpsDigest,const sbBlob_d * PpsSignature)
{

    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData = {0};
    uint16_t wWritePosition = OFFSET_PAYLOAD;
    uint16_t wCalApduLen = 0;

/// @cond hidden
    ///Minimum length of APDU InData in case of Public Key from Host. [TLV Header(3) for Digest + TLV Header (3) for Signature + TLV Header(3) for Public Key + TLV for Algo (4)]
    #define DATA_STREAM_APDU_INDATA_LEN     13
    ///Minimum length of APDU InData in case of Public Key OID. [TLV Header(3) for Digest + TLV Header (3) for Signature + TLV for Public Key OID (5)]
    #define OID_APDU_INDATA_LEN             11
/// @endcond

    do
    {
        //NULL checks
        if((NULL == PpsVerifySign) || (NULL == PpsDigest->prgbStream) ||
        (NULL == PpsSignature->prgbStream))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }
        if(eDataStream == PpsVerifySign->eVerifyDataType)
        {
            //NULL checks
            if(NULL == PpsVerifySign->sPubKeyInput.sDataStream.prgbStream)
            {
                i4Status = (int32_t)CMD_LIB_NULL_PARAM;
                break;
            }
            wCalApduLen = OFFSET_PAYLOAD + DATA_STREAM_APDU_INDATA_LEN + PpsDigest->wLen + PpsSignature->wLen + PpsVerifySign->sPubKeyInput.sDataStream.wLen;
        }
        if(eOIDData == PpsVerifySign->eVerifyDataType)
        {
            wCalApduLen = OFFSET_PAYLOAD + OID_APDU_INDATA_LEN + PpsDigest->wLen + PpsSignature->wLen;
        }
        if((wMaxCommsBuffer) < wCalApduLen)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }

        //Allocating Heap memory
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,wCalApduLen);

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
                sApduData.wResponseLength = wMaxCommsBuffer;
        //Set digest tag, length, data
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = TAG_DIGEST;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], PpsDigest->wLen);
        OCP_MEMCPY(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET],PpsDigest->prgbStream,PpsDigest->wLen);
        wWritePosition += TAG_VALUE_OFFSET+ PpsDigest->wLen;

        //Set signature tag, length, data
        sApduData.prgbAPDUBuffer[wWritePosition] = TAG_SIGNATURE;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], PpsSignature->wLen);
        OCP_MEMCPY(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET],PpsSignature->prgbStream,PpsSignature->wLen);
        wWritePosition += TAG_VALUE_OFFSET + PpsSignature->wLen;


        if(eDataStream == PpsVerifySign->eVerifyDataType)
        {
            //Set TLV values for external public key
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_ALGO_IDENTIFIER;

            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_ALGO_IDENTIFIER);
            sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET] = (uint8_t)PpsVerifySign->sPubKeyInput.eAlgId;

            sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET + BYTES_SEQ] = (uint8_t)TAG_PUB_KEY;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET + BYTES_OFFSET], PpsVerifySign->sPubKeyInput.sDataStream.wLen);
            wWritePosition += TAG_VALUE_OFFSET + BYTES_OFFSET + BYTES_OFFSET;
            OCP_MEMCPY(&sApduData.prgbAPDUBuffer[wWritePosition],PpsVerifySign->sPubKeyInput.sDataStream.prgbStream ,PpsVerifySign->sPubKeyInput.sDataStream.wLen);
            //Total payload length
            sApduData.wPayloadLength = (uint16_t)(wWritePosition + PpsVerifySign->sPubKeyInput.sDataStream.wLen - OFFSET_PAYLOAD);
        }

        if(eOIDData == PpsVerifySign->eVerifyDataType)
        {
            //Set TLV values for public key OID
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_PUB_KEY_OID;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_PUB_KEY);
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET], PpsVerifySign->wOIDPubKey);
            //Total payload length
            sApduData.wPayloadLength = (uint16_t)(wWritePosition + TAG_VALUE_OFFSET + BYTES_OFFSET - OFFSET_PAYLOAD);
        }

        //Form Command
        sApduData.bCmd = CMD_VERIFYSIGN;
        sApduData.bParam = (uint8_t)PpsVerifySign->eSignScheme;


        //Transmit data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }
    }while(FALSE);

    //Free the allocated memory for buffer
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);

/// @cond hidden
    #undef DATA_STREAM_APDU_INDATA_LEN
    #undef OID_APDU_INDATA_LEN
/// @endcond

    return i4Status;
}

/**
* Generates a key pair by using the Security Chip.<br>
*
* Input:
* - Provide the required option for exporting the generated keys. Use \ref sKeyPairOption_d.eKeyExport
*   - #eStorePrivKeyOnly indicates that only private key is stored in the OID and public key is exported.
*   - #eExportKeyPair indicates that both public and private keys are exported.
*
* Output:
* - Successful API execution,
*   - Public key is returned in \ref sOutKeyPair_d.sPublicKey.
*   - Private key is returned in \ref sOutKeyPair_d.sPrivateKey , if input is #eExportKeyPair.
*
* Notes:
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.
* - Values of #eKeyUsage_d can be logically 'ORed' and passed to \ref sKeyPairOption_d.eKeyUsage.
* - If the memory buffers in #sOutKeyPair_d is not sufficient to store the generated keys,#CMD_LIB_INSUFFICIENT_MEMORY is returned. Refer OPTIGA_Trust_X_SolutionReferenceManual_v1.x.pdf for more details.
*
* \param[in] PpsKeyPairOption Pointer to #sKeyPairOption_d to provide input for key pair generation
* \param[in,out] PpsOutKeyPair Pointer to #sOutKeyPair_d that contains generated key pair
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_NULL_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_EXEC_ERROR
* \retval  #CMD_DEV_ERROR
*/
int32_t CmdLib_GenerateKeyPair(const sKeyPairOption_d* PpsKeyPairOption,sOutKeyPair_d* PpsOutKeyPair)
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wWritePosition = LEN_APDUHEADER;
    uint16_t wCalApduLen;
    uint16_t wLen;
    uint16_t wParsLen;
    uint8_t bMultiplier;
    sApduData_d sApduData = {0};
    uint8_t wAlgoLen;
    sbBlob_d * psBlobKey = NULL;

/// @cond hidden
    ///Tag for public key
    #define TAG_PUBLIC_KEY                  0x02
    ///Minimum length of APDU InData in case of Private key store. [TLV Header(3) of OID + OID (2) + TLV Header(3) for key usage identifier  + Identifier (1)]
    #define PRIV_KEY_APDU_INDATA_LEN        9
    /// Encoding bytes for private and public key
    #define KEY_PAIR_INDATA_LEN             4
/// @endcond

    do
    {
        //NULL checks
        if((NULL == PpsKeyPairOption) || (NULL == PpsOutKeyPair) || (NULL == PpsOutKeyPair->sPublicKey.prgbStream))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        wCalApduLen = LEN_APDUHEADER + PRIV_KEY_APDU_INDATA_LEN;
        bMultiplier = 2;
        if(eExportKeyPair == PpsKeyPairOption->eKeyExport)
        {
            //NULL checks
            if(NULL == PpsOutKeyPair->sPrivateKey.prgbStream)
            {
                i4Status = (int32_t)CMD_LIB_NULL_PARAM;
                break;
            }
            wCalApduLen = LEN_APDUHEADER + KEY_PAIR_INDATA_LEN + TAG_VALUE_OFFSET + TAG_VALUE_OFFSET;
            bMultiplier = 3;
        }
        //Get key size using algorithm identifier
        i4Status = Get_KeySize(PpsKeyPairOption->eAlgId, &wAlgoLen);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        wCalApduLen += ((wAlgoLen * bMultiplier) + 2);
        //Allocating Heap memory
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,wCalApduLen);

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        sApduData.wResponseLength = wCalApduLen;
        if(eStorePrivKeyOnly == PpsKeyPairOption->eKeyExport)
        {
            //Set private key OID tag, length, data
            sApduData.prgbAPDUBuffer[LEN_APDUHEADER] = TAG_OID;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_PRI_KEY);
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET], PpsKeyPairOption->wOIDPrivKey);

            wWritePosition += TAG_VALUE_OFFSET+ BYTES_OFFSET;

            //Set key usage identifier tag, length, data
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_KEY_USAGE_IDENTIFIER;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_KEY_USAGE_IDENTIFIER);
            sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET] = (uint8_t)PpsKeyPairOption->eKeyUsage;
            wWritePosition += TAG_VALUE_OFFSET + BYTES_SEQ;
        }

        if(eExportKeyPair == PpsKeyPairOption->eKeyExport)
        {
            //Set TLV values for extract key pair
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_EXPORT_KEY_PAIR;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_EXPORT_KEY_PAIR);
            wWritePosition += TAG_VALUE_OFFSET;
        }
        sApduData.wPayloadLength = (uint16_t)(wWritePosition - LEN_APDUHEADER);
        //Form Command
        sApduData.bCmd = CMD_GENERATE_KEY_PAIR;
        sApduData.bParam = (uint8_t)PpsKeyPairOption->eAlgId;

        //Transmit data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        wParsLen = LEN_APDUHEADER;

        do
        {
            wLen = Utility_GetUint16(&sApduData.prgbRespBuffer[wParsLen+BYTES_SEQ]);
            psBlobKey = (TAG_PUBLIC_KEY == sApduData.prgbRespBuffer[wParsLen])?&(PpsOutKeyPair->sPublicKey):&(PpsOutKeyPair->sPrivateKey);
            if(wLen > psBlobKey->wLen)
            {
                i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
                break;
            }
            //Copy public key to output buffer
            OCP_MEMCPY(psBlobKey->prgbStream,&sApduData.prgbRespBuffer[TAG_VALUE_OFFSET + wParsLen] ,wLen);
            psBlobKey->wLen = wLen;
            wParsLen += (wLen + TAG_VALUE_OFFSET);

        }while(wParsLen != sApduData.wResponseLength);

    }while(FALSE);

    //Free the allocated memory for buffer
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);

/// @cond hidden
    #undef TAG_PUBLIC_KEY
    #undef PRIV_KEY_APDU_INDATA_LEN
    #undef KEY_PAIR_INDATA_LEN
/// @endcond

    return i4Status;
}

/**
* Calculates signature on a digest by using the Security Chip.<br>
*
* Input:
* - Provide the signature scheme. Use \ref sCalcSignOptions_d.eSignScheme.
* - Provide the digest to be signed. Use \ref sCalcSignOptions_d.sDigestToSign.
* - Provide the OID of the private key. Use \ref sCalcSignOptions_d.wOIDSignKey.
*
* Output:
* - Successful API execution,
*   - Signature is returned in PpsSignature.<br>
*
* Notes:
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.
* - If the the data to be sent to security chip is more than communication buffer,#CMD_LIB_INSUFFICIENT_MEMORY is returned. Refer OPTIGA_Trust_X_SolutionReferenceManual_v1.x.pdf for more details.
* - If the memory buffer in PpsSignature is not sufficient to store the generated signature,#CMD_LIB_INSUFFICIENT_MEMORY is returned.

*
* \param[in] PpsCalcSign Pointer to #sCalcSignOptions_d to provide input for signature generation
* \param[in,out] PpsSignature Pointer to #sbBlob_d that contains generated signature
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_NULL_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_EXEC_ERROR
* \retval  #CMD_DEV_ERROR
*/
int32_t CmdLib_CalculateSign(const sCalcSignOptions_d *PpsCalcSign,sbBlob_d *PpsSignature)
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wWritePosition = LEN_APDUHEADER;
    uint16_t wCalApduLen;
    sApduData_d sApduData = {0};


    do
    {
        //NULL checks
        if((NULL == PpsCalcSign) || (NULL == PpsSignature->prgbStream) || (NULL == PpsCalcSign->sDigestToSign.prgbStream))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }


/// @cond hidden
///Minimum length of APDU InData in case of calculate sign. [TLV Header(3) of OID  + OID (2) + TLV Header(3) for digest ]
#define CALSIGN_APDU_LEN        8
///Tag for Signature length
#define SIGNATURE_LEN           0x77
///Total value required while sending the command
#define TX_LEN                  (CALSIGN_APDU_LEN + PpsCalcSign->sDigestToSign.wLen)
/// @endcond

        //Calculate the size of memory to be allocated
        wCalApduLen = LEN_APDUHEADER + (TX_LEN > SIGNATURE_LEN ? TX_LEN : SIGNATURE_LEN);
        if((wMaxCommsBuffer) < wCalApduLen)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }

        //Allocating Heap memory
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,wCalApduLen);

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        sApduData.wResponseLength = wCalApduLen;

        //Set digest tag, length, data
        sApduData.prgbAPDUBuffer[LEN_APDUHEADER] = TAG_DIGEST;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], PpsCalcSign->sDigestToSign.wLen);
        OCP_MEMCPY(&sApduData.prgbRespBuffer[TAG_VALUE_OFFSET + wWritePosition],PpsCalcSign->sDigestToSign.prgbStream,PpsCalcSign->sDigestToSign.wLen);
        wWritePosition += TAG_VALUE_OFFSET + PpsCalcSign->sDigestToSign.wLen;

        //Set OID of signature key tag, length, data
        sApduData.prgbAPDUBuffer[wWritePosition] = TAG_OID_SIG_KEY;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_OID_SIG_KEY);
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET], PpsCalcSign->wOIDSignKey);

        wWritePosition += TAG_VALUE_OFFSET + LEN_OID_SIG_KEY;

        sApduData.wPayloadLength = (uint16_t)(wWritePosition - LEN_APDUHEADER);
        //Form Command
        sApduData.bCmd = CMD_CALC_SIGN;
        sApduData.bParam = (uint8_t)PpsCalcSign->eSignScheme;

        //Transmit data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }
        sApduData.wResponseLength -= LEN_APDUHEADER;
        if(sApduData.wResponseLength > PpsSignature->wLen)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }
        //Copy signature to output buffer
        OCP_MEMCPY(PpsSignature->prgbStream,&sApduData.prgbRespBuffer[LEN_APDUHEADER],sApduData.wResponseLength);
        PpsSignature->wLen = sApduData.wResponseLength;

    }while(FALSE);

    //Free the allocated memory for buffer
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);


/// @cond hidden
#undef CALSIGN_APDU_LEN
#undef SIGNATURE_LEN
#undef TX_LEN
/// @endcond
    return i4Status;
}

/**
* Generates a shared secret by using the Security Chip.<br>
*
* Input:
* - Provide the key agreement algorithm for generating shared secret. Use \ref sCalcSSecOptions_d.eKeyAgreementType.
* - Provide the OID of private key. Use \ref sCalcSSecOptions_d.wOIDPrivKey.
* - Provide the algorithm identifier of the public key. Use \ref sCalcSSecOptions_d.ePubKeyAlgId.
* - Provide the public key. Use \ref sCalcSSecOptions_d.sPubKey.
* - Provide the OID to store the shared secret. Use \ref sCalcSSecOptions_d.wOIDSharedSecret.
*   - 0x0000 indicates that the shared secret is exported.
*
* Output:
* - Successful API execution,
*   - Calculated shared secret is returned in PpsSecret if \ref sCalcSSecOptions_d.wOIDSharedSecret is 0x0000.
*
* Notes:
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.
* - If the the data to be sent to security chip is more than communication buffer,#CMD_LIB_INSUFFICIENT_MEMORY is returned. Refer OPTIGA_Trust_X_SolutionReferenceManual_v1.x.pdf for more details.
* - If the memory buffer in PpsSecret is not sufficient to store the calculated secret,#CMD_LIB_INSUFFICIENT_MEMORY is returned.

*
* \param[in] PpsCalcSSec Pointer to #sCalcSSecOptions_d to provide input for shared secret calculation
* \param[in,out] PpsSecret Pointer to #sbBlob_d that contains calculated shared secret
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_NULL_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_EXEC_ERROR
* \retval  #CMD_DEV_ERROR
*/
int32_t CmdLib_CalculateSharedSecret(const sCalcSSecOptions_d *PpsCalcSSec,sbBlob_d *PpsSecret)
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wWritePosition = LEN_APDUHEADER;
    uint16_t wCalApduLen = 0;
    sApduData_d sApduData = {0};

    do
    {
        //NULL checks
        if((NULL == PpsCalcSSec) || (NULL == PpsCalcSSec->sPubKey.prgbStream))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        /// @cond hidden
        ///Minimum length of APDU InData in case of calculate shared secret.
        ///[TLV Header(3) of OID Private key + OID Private key (2) + TLV Header(3) for public key algoId + algoId (1) + TLV Header(3) for public key + TLV Header(3) for alternative ]
        #define CALCSSEC_APDU_LEN       15
        ///Tag for public key
        #define TAG_PUBLIC_KEY          0x06
        ///Len for privet key oid
        #define LEN_EXPORT_SHAR_SEC     0x0000
        ///Len for share sec oid
        #define LEN_OID_SHARE_SEC       0x0002
        ///Share sec OID zero value
        #define OID_SHARE_SEC_ZERO      0x0000
        ///Tag for export share secret
        #define TAG_EXPORT_SHARE_SEC    0x07
        ///Tag for share secret oid
        #define TAG_OID_SHARE_SEC       0x08
        ///Minimum length of APDU
        #define TX_LEN                  (CALCSSEC_APDU_LEN + PpsCalcSSec->sPubKey.wLen + 2)
        /// @endcond

        //Considering the size of Indata for allocating memory as this size is also sufficient for storing the response
        wCalApduLen = LEN_APDUHEADER + TX_LEN;
        //NULL checks
        if(OID_SHARE_SEC_ZERO == PpsCalcSSec->wOIDSharedSecret)
        {
            if(NULL == PpsSecret->prgbStream)
            {
                i4Status = (int32_t)CMD_LIB_NULL_PARAM;
                break;
            }
            wCalApduLen -=2;
        }

        //Check max comms buffer size
        if((wMaxCommsBuffer) < wCalApduLen)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }

        //Allocating Heap memory
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,wCalApduLen);

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        sApduData.wResponseLength = wCalApduLen;

        //Set privet key tag, length, data
        sApduData.prgbAPDUBuffer[LEN_APDUHEADER] = TAG_OID;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET],LEN_PRI_KEY);
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET],PpsCalcSSec->wOIDPrivKey);
        wWritePosition += TAG_VALUE_OFFSET + LEN_PRI_KEY;

        //Set public key algoId tag, length, data
        sApduData.prgbAPDUBuffer[wWritePosition] = TAG_ALGO_IDENTIFIER;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET],LEN_ALGO_IDENTIFIER);
        sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET] = (uint8_t)PpsCalcSSec->ePubKeyAlgId;
        wWritePosition += TAG_VALUE_OFFSET + LEN_ALGO_IDENTIFIER;

        //Set public key tag, length, data
        sApduData.prgbAPDUBuffer[wWritePosition] = TAG_PUBLIC_KEY;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET],PpsCalcSSec->sPubKey.wLen);
        OCP_MEMCPY(&sApduData.prgbRespBuffer[TAG_VALUE_OFFSET + wWritePosition],PpsCalcSSec->sPubKey.prgbStream,PpsCalcSSec->sPubKey.wLen);
        wWritePosition += TAG_VALUE_OFFSET + PpsCalcSSec->sPubKey.wLen;

        if(OID_SHARE_SEC_ZERO == PpsCalcSSec->wOIDSharedSecret)
        {
            //Set export share sec tag, length, data
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_EXPORT_SHARE_SEC;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_EXPORT_SHAR_SEC);
            wWritePosition += TAG_VALUE_OFFSET;
        }
        else
        {
            //Set OID of signature key tag, length, data
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_OID_SHARE_SEC;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_OID_SHARE_SEC);
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET], PpsCalcSSec->wOIDSharedSecret);
            wWritePosition += TAG_VALUE_OFFSET + LEN_OID_SHARE_SEC;
        }

        sApduData.wPayloadLength = (uint16_t)(wWritePosition - LEN_APDUHEADER);
        //Form Command
        sApduData.bCmd = CMD_CALC_SHARED_SEC;
        sApduData.bParam = (uint8_t)PpsCalcSSec->eKeyAgreementType;

        //Transmit data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        sApduData.wResponseLength -= LEN_APDUHEADER;
        if(OID_SHARE_SEC_ZERO == PpsCalcSSec->wOIDSharedSecret)
        {
            if(sApduData.wResponseLength > PpsSecret->wLen)
            {
                i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
                break;
            }
            //Copy signature to output buffer
            OCP_MEMCPY(PpsSecret->prgbStream,&sApduData.prgbRespBuffer[LEN_APDUHEADER],sApduData.wResponseLength);
            PpsSecret->wLen = sApduData.wResponseLength;
        }
    }while(FALSE);

    //Free the allocated memory for buffer
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);

    /// @cond hidden
    #undef CALCSSEC_APDU_LEN
    #undef TAG_PUBLIC_KEY
    #undef LEN_EXPORT_SHAR_SEC
    #undef LEN_OID_SHARE_SEC
    #undef OID_SHARE_SEC_ZERO
    #undef TAG_EXPORT_SHARE_SEC
    #undef TAG_OID_SHARE_SEC
    #undef TX_LEN
    /// @endcond
    return i4Status;
}

/**
* Derives a session key by using the Security Chip.<br>
*
* Input:
* - Provide the key derivation method. Use \ref sDeriveKeyOptions_d.eKDM.
* - Provide the OID of the shared secret. Use \ref sDeriveKeyOptions_d.wOIDSharedSecret.
* - Provide the input seed. Use \ref sDeriveKeyOptions_d.sSeed.
* - Provide the length for derived key. Use \ref sDeriveKeyOptions_d.wDerivedKeyLen.
* - Provide the OID to store the derived key. Use \ref sDeriveKeyOptions_d.wOIDDerivedKey.
*   - 0x0000 indicates that the derived key is exported.
*
* Output:
* - Successful API execution,
*   - Derived key is returned in PpsKey if \ref sDeriveKeyOptions_d.wOIDDerivedKey is 0x0000.
*
* Notes:
* - Application on security chip must be opened using #CmdLib_OpenApplication before using this API.
* - If the the data to be sent to security chip is more than communication buffer,#CMD_LIB_INSUFFICIENT_MEMORY is returned. Refer OPTIGA_Trust_X_SolutionReferenceManual_v1.x.pdf for more details.
* - If the memory buffer in PpsKey is not sufficient to store the derived key,#CMD_LIB_INSUFFICIENT_MEMORY is returned.

*
* \param[in] PpsDeriveKey   Pointer to #sDeriveKeyOptions_d to provide input for session key generation
* \param[in,out] PpsKey     Pointer to #sbBlob_d that contains the derived key
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_NULL_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_EXEC_ERROR
* \retval  #CMD_DEV_ERROR
*/
int32_t CmdLib_DeriveKey(const sDeriveKeyOptions_d *PpsDeriveKey,sbBlob_d *PpsKey)
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    uint16_t wWritePosition = LEN_APDUHEADER;
    uint16_t wCalApduLen = 0;
    sApduData_d sApduData = {0};

    do
    {
        //NULL checks
        if((NULL == PpsDeriveKey) || (NULL == PpsDeriveKey->sSeed.prgbStream))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        /// @cond hidden
        ///Minimum length of APDU InData in case of calculate shared secret.
        ///[TLV Header(3) of OID share secret + OID share secret (2) + TLV Header(3) for seed + TLV Header(3) for derive secret length + derive secret length(2) + TLV Header(3) for alternative ]
        #define DERIVEKEY_APDU_LEN      16
        ///Tag for derive key
        #define TAG_DERIVE_KEY          0x03
        ///Len for export derive key
        #define LEN_EXPORT_DERIVE_KEY   0x0000
        ///Len for share sec oid
        #define LEN_DERIVE_KEY          0x0002
        ///Derive key OID zero value
        #define OID_DERIVE_SEC_ZERO     0x0000
        ///Tag for export derive share secret
        #define TAG_EXPORT_DERIVE_KEY   0x07
        ///Tag for derive key oid
        #define TAG_OID_DERIVE_KEY      0x08
        ///Minimum length of APDU
        #define TX_LEN                  (DERIVEKEY_APDU_LEN + 2)
        /// @endcond

        //Considering the size of Indata for allocating memory as this size is also sufficient for storing the response
        wCalApduLen = LEN_APDUHEADER + TX_LEN + (PpsDeriveKey->sSeed.wLen > PpsDeriveKey->wDerivedKeyLen ? PpsDeriveKey->sSeed.wLen : PpsDeriveKey->wDerivedKeyLen);
        //NULL checks
        if(OID_DERIVE_SEC_ZERO == PpsDeriveKey->wOIDDerivedKey)
        {
            if(NULL == PpsKey->prgbStream)
            {
                i4Status = (int32_t)CMD_LIB_NULL_PARAM;
                break;
            }
            wCalApduLen -= 2;
        }

        //Check max comms buffer size
        if((wMaxCommsBuffer) < wCalApduLen)
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }

        //Allocating Heap memory
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,wCalApduLen);

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        sApduData.wResponseLength = wCalApduLen;

        //Set share secret key tag, length, data
        sApduData.prgbAPDUBuffer[LEN_APDUHEADER] = TAG_OID;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET],LEN_SHARED_SECRET_OID);
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET],PpsDeriveKey->wOIDSharedSecret);
        wWritePosition += TAG_VALUE_OFFSET + LEN_PRI_KEY;

        //Set public key algoId tag, length, data
        sApduData.prgbAPDUBuffer[wWritePosition] = TAG_SEED;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET],PpsDeriveKey->sSeed.wLen);
        OCP_MEMCPY(&sApduData.prgbRespBuffer[TAG_VALUE_OFFSET + wWritePosition],PpsDeriveKey->sSeed.prgbStream,PpsDeriveKey->sSeed.wLen);
        wWritePosition += TAG_VALUE_OFFSET + PpsDeriveKey->sSeed.wLen;

        //Set public key tag, length, data
        sApduData.prgbAPDUBuffer[wWritePosition] = TAG_DERIVE_KEY;
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET],LEN_DERIVE_KEY);
        Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET],PpsDeriveKey->wDerivedKeyLen);
        wWritePosition += TAG_VALUE_OFFSET + LEN_DERIVE_KEY;

        if(OID_DERIVE_SEC_ZERO == PpsDeriveKey->wOIDDerivedKey)
        {
            //Set export share sec tag, length, data
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_EXPORT_DERIVE_KEY;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_EXPORT_DERIVE_KEY);
            wWritePosition += TAG_VALUE_OFFSET;
        }
        else
        {
            //Set OID of signature key tag, length, data
            sApduData.prgbAPDUBuffer[wWritePosition] = TAG_OID_DERIVE_KEY;
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_LENGTH_OFFSET], LEN_DERIVE_KEY);
            Utility_SetUint16(&sApduData.prgbAPDUBuffer[wWritePosition + TAG_VALUE_OFFSET], PpsDeriveKey->wOIDDerivedKey);
            wWritePosition += TAG_VALUE_OFFSET + LEN_DERIVE_KEY;
        }

        sApduData.wPayloadLength = (uint16_t)(wWritePosition - LEN_APDUHEADER);
        //Form Command
        sApduData.bCmd = CMD_DERIVE_KEY;
        sApduData.bParam = (uint8_t)PpsDeriveKey->eKDM;

        //Transmit data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        sApduData.wResponseLength -= LEN_APDUHEADER;
        if(OID_DERIVE_SEC_ZERO == PpsDeriveKey->wOIDDerivedKey)
        {
            if(sApduData.wResponseLength > PpsKey->wLen)
            {
                i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
                break;
            }
            //Copy signature to output buffer
            OCP_MEMCPY(PpsKey->prgbStream,&sApduData.prgbRespBuffer[LEN_APDUHEADER],sApduData.wResponseLength);
            PpsKey->wLen = sApduData.wResponseLength;
        }
    }while(FALSE);

    //Free the allocated memory for buffer
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);

    /// @cond hidden
    #undef DERIVEKEY_APDU_LEN
    #undef TAG_DERIVE_KEY
    #undef LEN_EXPORT_DERIVE_KEY
    #undef LEN_DERIVE_KEY
    #undef OID_DERIVE_SEC_ZERO
    #undef TAG_EXPORT_DERIVE_KEY
    #undef TAG_OID_DERIVE_KEY
    #undef TX_LEN
    /// @endcond
    return i4Status;
}


#endif/*MODULE_ENABLE_TOOLBOX*/

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH
/**
* Gets Handshake message from Security Chip.<br>
*
*
* Notes: <br>
* - User should provide a callback through #sCallBack_d.
* - This callback allows the caller to allocate memory for the message and keep copying data into the memory in case of lengthy messages.
* - Allocated buffer is returned to user in #sCBGetMsg_d.
* - The callback should return #CMD_LIB_OK for successful allocation of memory else #CMD_LIB_ERROR in case of error.<br>
*
* - Any Message specific data must be provided by the user in the union puMsgParams.<br>
*   The union is defined as #uMsgParams_d.
*
* - The user must provide correct data in puMsgParams.This function does not <br>
*   validate the content of the message specific data.
*   E.g For sending gmt_unix_time for Client Hello message,uMsgParams_d.sMsgParamCH_d.dwUnixTime must be set.
*   If puMsgParams is set to NULL,then random dwUnixTime will be considered for Client Hello message <br>
*   and certificate will not be send for Client Certificate message.
*
*   The psBlobInBuffer pointer which is member of sProcMsgData_d should be set to NULL
*
* \param[in,out] PpsGMsgVector Pointer to DTLS Handshake Message parameters
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INVALID_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_GetMessage(const sProcMsgData_d *PpsGMsgVector)
{
///@cond hidden
#define STACK_ALLOC
///@endcond

    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;
    uint8_t bFragSeq ;
    uint16_t wRespLen;
    sbBlob_d sBlobMessage;

    do
    {
//To use stack memory for APDU buffer, define STACK_ALLOC locally else heap memory is used
#ifdef STACK_ALLOC
        INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer,MAX_APDU_BUFF_LEN);
#else
        INIT_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer,MAX_APDU_BUFF_LEN);
#endif

        //NULL checks
        if((NULL == PpsGMsgVector) || (NULL == PpsGMsgVector->psCallBack) ||
            (NULL == PpsGMsgVector->psCallBack->pfAcceptMessage) || (NULL == PpsGMsgVector->psCallBack->fvParams))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        //Verify the range of the param
        if((eClientHello != PpsGMsgVector->eParam) && (eClientHelloWithCookie != PpsGMsgVector->eParam) &&
        (eClientCertificate != PpsGMsgVector->eParam) && (eClientKeyExchange != PpsGMsgVector->eParam) &&
        (eCertificateVerify != PpsGMsgVector->eParam) && (eClientFinished != PpsGMsgVector->eParam))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_PARAM;
            break;
        }

        //Verify the Session OID reference
        if((SESSION_ID_LOWER_VALUE > PpsGMsgVector->wSessionKeyOID) ||
        (SESSION_ID_HIGHER_VALUE < PpsGMsgVector->wSessionKeyOID))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_SESSIONID;
            break;
        }

        //Set the fragment sequence to start
        bFragSeq = (uint8_t)eStart;

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;

        while((eFragSeq_d)bFragSeq != eFinal)
        {
            //Form data and assign to apdu structure
            //Assign cmd,param,length

            sApduData.bCmd = CMD_GETMSG;
            sApduData.bParam = (uint8_t)PpsGMsgVector->eParam;
            //Total payload length is Session ID Length
            sApduData.wPayloadLength = BYTES_SESSIONID;

            //Form the data in order in the buffer
            //Add the session ID to the buffer
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(PpsGMsgVector->wSessionKeyOID >> BITS_PER_BYTE);
            sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + 1] = (uint8_t)PpsGMsgVector->wSessionKeyOID;
            sApduData.wResponseLength = MAX_APDU_BUFF_LEN;

            if(((uint8_t)eClientHello == sApduData.bParam) && (NULL != PpsGMsgVector->puMsgParams))
            {
                sApduData.wPayloadLength += LEN_TAG_ENCODING + BYTES_GMT_TIME;
                sApduData.prgbAPDUBuffer[OFFSET_TAG] = TAG_GMTUNIX_TIME;
                sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN] = 0x00;
                sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN + 1] = BYTES_GMT_TIME;
                Utility_SetUint32 (&sApduData.prgbAPDUBuffer[OFFSET_TAG_DATA],PpsGMsgVector->puMsgParams->sMsgParamCH_d.dwUnixTime);
            }
            else if(((uint8_t)eClientCertificate == sApduData.bParam) && (NULL != PpsGMsgVector->puMsgParams))
            {
                sApduData.wPayloadLength += LEN_TAG_ENCODING + BYTES_OID;
                sApduData.prgbAPDUBuffer[OFFSET_TAG] = TAG_CERTIFICATE_OID;
                sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN] = 0x00;
                sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN + 1] = BYTES_OID;
                Utility_SetUint16 (&sApduData.prgbAPDUBuffer[OFFSET_TAG_DATA],PpsGMsgVector->puMsgParams->sMsgParamCert_d.wCertOID);
            }
            //Transmit data
            i4Status = TransceiveAPDU(&sApduData,TRUE);
            if(CMD_LIB_OK != i4Status)
            {
                break;
            }

            //Remove 4 byte apdu header + tag encoding
            sApduData.wResponseLength -= (LEN_APDUHEADER + LEN_TAG_ENCODING);

            //Verify the TLV encoding
            //Verify the Tag
            if(TAG_UNPROTECTED != (*(sApduData.prgbRespBuffer + LEN_APDUHEADER) & MASK_HIGHER_NIBBLE))
            {
                i4Status = (int32_t)CMD_LIB_INVALID_TAG;
                break;
            }

            //Extract the fragment sequence information
            bFragSeq = *(sApduData.prgbRespBuffer + LEN_APDUHEADER) & MASK_LOWER_NIBBLE;

            //extract the tag length field
            wRespLen = Utility_GetUint16(sApduData.prgbRespBuffer + LEN_APDUHEADER + 1);

            //Length validation for response length with the tag length
            if(sApduData.wResponseLength != wRespLen)
            {
                i4Status = (int32_t)CMD_LIB_INVALID_TAGLEN;
                break;
            }
            //Assign the handshake message pointer to the sblob
            sBlobMessage.prgbStream = sApduData.prgbRespBuffer + LEN_APDUHEADER + LEN_TAG_ENCODING;

            //Assign the response length(only Handshake message) excluding the tag encoding
            sBlobMessage.wLen = sApduData.wResponseLength;

            //Call back function to allocate the memory for handshake message based the response length
            i4Status = PpsGMsgVector->psCallBack->pfAcceptMessage(PpsGMsgVector->psCallBack->fvParams, &sBlobMessage);
            if(i4Status != CMD_LIB_OK)
            {
                i4Status = (int32_t)CMD_LIB_ERROR;
                break;
            }
        }
        //Note: If data is encoded with eContinue then loop to get complete data till eFinal is encoded
        //The response pointer should be updated for each loop and check for buffer overflow
    }while(FALSE);

    //Free the allocated memory for buffer
#ifndef STACK_ALLOC
    FREE_HEAP_APDUBUFFER(sApduData.prgbAPDUBuffer);
#else
    #undef STACK_ALLOC
#endif

    return i4Status;
}

/**
* Sends Handshake message to Security Chip for processing.<br>
*
*
* Notes: <br>
* - Input buffer must be provided by the user.<br>
* - Clearing of the buffers is the responsibility of the user.
*
* - The input pointer should contain sufficient memory to accommodate <br>
*   APDU header and data formatting.
*
* - The API will not recopy the Authentication message data but add the header and data<br>
*   formatting information before it, in the same input buffer.
*
*   The puMsgParams and psCallBack pointer which is member of sProcMsgData_d should be set to NULL
*
* \param[in] PpsPMsgVector Pointer to DTLS Handshake Message parameters
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INVALID_PARAM
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_PutMessage(const sProcMsgData_d *PpsPMsgVector)
{
    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;

    do
    {
        //NULL checks
        if((NULL == PpsPMsgVector) || (NULL == PpsPMsgVector->psBlobInBuffer) ||
        (NULL == PpsPMsgVector->psBlobInBuffer->prgbStream))
        {
            i4Status = (int32_t)CMD_LIB_NULL_PARAM;
            break;
        }

        //Zero length checks
        if(0x00 == PpsPMsgVector->psBlobInBuffer->wLen)
        {
            i4Status = (int32_t)CMD_LIB_LENZERO_ERROR;
            break;
        }

        //Verify the range of the param
        if(((eServerCertificate > PpsPMsgVector->eParam) || (eServerHelloDone < PpsPMsgVector->eParam)) &&
        ((eServerHello != PpsPMsgVector->eParam) && (eHelloVerifyRequest != PpsPMsgVector->eParam) &&
        (eServerFinished != PpsPMsgVector->eParam)))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_PARAM;
            break;
        }

        //Verify the Session OID reference
        if((SESSION_ID_LOWER_VALUE > PpsPMsgVector->wSessionKeyOID) ||
        (SESSION_ID_HIGHER_VALUE < PpsPMsgVector->wSessionKeyOID))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_SESSIONID;
            break;
        }

        //Length of data + OverHeadLen should not to be more than wMaxCommsBuffer
        //Currently, chaining is not supported by Command library and security chip.Hence, this length check is performed.
        if(PpsPMsgVector->psBlobInBuffer->wLen > (wMaxCommsBuffer) )
        {
            i4Status = (int32_t)CMD_LIB_INSUFFICIENT_MEMORY;
            break;
        }

        //Assign In memory pointer to the APDU Buffer in the Apdu structure
        sApduData.prgbAPDUBuffer = PpsPMsgVector->psBlobInBuffer->prgbStream;
        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        //Form data and assign to apdu structure
        //Assign cmd,param,length
        sApduData.bCmd = CMD_PUTMSG;
        sApduData.bParam = (uint8_t)PpsPMsgVector->eParam;
        //Total payload length is sum of length of Session ID , Tag, Tag length and the data
        sApduData.wPayloadLength = PpsPMsgVector->psBlobInBuffer->wLen - OFFSET_PAYLOAD;

        //Add the session ID to the buffer
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)(PpsPMsgVector->wSessionKeyOID >> BITS_PER_BYTE);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + 1] = (uint8_t)PpsPMsgVector->wSessionKeyOID;
        //Add the encoding tag to the buffer
        sApduData.prgbAPDUBuffer[OFFSET_TAG] = TAG_UNPROTECTED;

        sApduData.prgbAPDUBuffer[OFFSET_TAG] |= (uint8_t)eFinal;

        //Add the tag length to the buffer
        //lint --e{702} suppress "Acknowledging the shift. Reviewed it"
        sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN] = (uint8_t)(((PpsPMsgVector->psBlobInBuffer->wLen) - (OFFSET_PAYLOAD + BYTES_SESSIONID + LEN_TAG_ENCODING)) >> BITS_PER_BYTE);

        sApduData.prgbAPDUBuffer[OFFSET_TAG_LEN + 1] = (uint8_t)(PpsPMsgVector->psBlobInBuffer->wLen - (OFFSET_PAYLOAD + BYTES_SESSIONID + LEN_TAG_ENCODING));

        sApduData.wResponseLength = PpsPMsgVector->psBlobInBuffer->wLen;
        //Transmit data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }
    }while(FALSE);

    return i4Status;
}

/**
*
* Closes the DTLS session as indicated by the Session OID.<br>
*
*
* \param[in] PwSessionRefId session OID to be closed
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INVALID_SESSIONID
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
*/
int32_t CmdLib_CloseSession(uint16_t PwSessionRefId)
{
/// @cond hidden
#define CLOSE_SESSION_APDU_BUF_LEN    6
/// @endcond

    int32_t i4Status = (int32_t)CMD_LIB_ERROR;
    sApduData_d sApduData;

    do
    {
        INIT_STACK_APDUBUFFER(sApduData.prgbAPDUBuffer,CLOSE_SESSION_APDU_BUF_LEN);

        //Verify the session id
        if((SESSION_ID_LOWER_VALUE > PwSessionRefId) ||
        (SESSION_ID_HIGHER_VALUE < PwSessionRefId))
        {
            i4Status = (int32_t)CMD_LIB_INVALID_SESSIONID;
            break;
        }

        //Set the pointer to the response buffer
        sApduData.prgbRespBuffer = sApduData.prgbAPDUBuffer;
        //Form the command
        //Assign Cmd,param,payload length
        sApduData.bCmd = CMD_GETMSG;
        sApduData.bParam = (uint8_t)eCloseSession;

        //Total payload length is equal to session ID length
        sApduData.wPayloadLength = BYTES_SESSIONID;

        //Assign the session key
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD] = (uint8_t)( PwSessionRefId >> BITS_PER_BYTE);
        sApduData.prgbAPDUBuffer[OFFSET_PAYLOAD + 1] = (uint8_t)PwSessionRefId;

        sApduData.wResponseLength = CLOSE_SESSION_APDU_BUF_LEN;

        //Transmit the Data
        i4Status = TransceiveAPDU(&sApduData,TRUE);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }
    }while(FALSE);

/// @cond hidden
#undef CLOSE_SESSION_APDU_BUF_LEN
/// @endcond

    return i4Status;
}

/**
* Encrypts data by issuing ProcUpLink command to Security Chip.<br>
*
*
* Notes: <br>
* - Input and Output buffers must be provided by the user.
*Buffer deallocation is the responsibility of the user.<br>
*
* - The input data in #sbBlob_d sInData should contain sufficient memory to accommodate APDU header,
*data formatting,Plaintext.<br>
*The Plaintext and any specific data for encryption should start after an overhead of #OVERHEAD_UPDOWNLINK.<br>
*
* - wInDataLength in #sProcCryptoData_d should be greater than zero.<br>
*
* - Ciphertext is returned in #sCmdResponse_d* sOutData from zero offset.<br>
*
* - In addition to the Ciphertext, the length of buffer in sOutData
*should be sufficient to accommodate Response APDU header and data formatting. This is defined as #OVERHEAD_ENCDEC_RESPONSE<br>
*The total length of the Ciphertext is returned in wRespLength of #sCmdResponse_d.<br>
*
* - The current implementation of Security chip does not support command chaining.
*The maximum value of wInDataLength depends on the value supported by the security chip.<br>
*
* - Currently,the security chip supports only 0xE100 as session key OID.
*
* \param[in,out] PpsEncVector Pointer to structure containing Plaintext and Ciphertext
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_LIB_INVALID_SESSIONID
* \retval  #CMD_LIB_INVALID_LEN
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_Encrypt(sProcCryptoData_d *PpsEncVector)
{
    return CmdLib_EncDecHelper(PpsEncVector,CMD_ENCDATA,PARAM_ENC_DATA);
}

/**
* Decrypts data by issuing ProcDownLink command to Security Chip.<br>
*
*
* Notes: <br>
* - Input and Output buffers must be provided by the user.
*Buffer deallocation is the responsibility of the user.<br>
*
* - The input data in #sbBlob_d sInData should contain sufficient memory to accommodate APDU header,
*data formatting,Ciphertext.<br>
*The Ciphertext and any specific data for decryption should start after an overhead of #OVERHEAD_UPDOWNLINK.<br>
*
* - wInDataLength in #sProcCryptoData_d should be greater than zero.<br>
*
* - Plaintext is returned in #sCmdResponse_d* sOutData from zero offset.<br>
*
* - In addition to the Plaintext, the length of buffer in sOutData
*should be sufficient to accommodate Response APDU header and data formatting. This is defined as #OVERHEAD_ENCDEC_RESPONSE<br>
*The total length of the Plaintext is returned in wRespLength of #sCmdResponse_d.<br>
*
* - The current implementation of Security chip does not support command chaining.
*The maximum value of wInDataLength depends on the value supported by the security chip.<br>
*
* - Currently,the security chip supports only 0xE100 as session key OID.
*
* \param[in,out] PpsDecVector Pointer to structure containing Ciphertext and Plaintext
*
* \retval  #CMD_LIB_OK
* \retval  #CMD_LIB_ERROR
* \retval  #CMD_LIB_INSUFFICIENT_MEMORY
* \retval  #CMD_LIB_INVALID_SESSIONID
* \retval  #CMD_LIB_INVALID_LEN
* \retval  #CMD_DEV_ERROR
* \retval  #CMD_LIB_DECRYPT_FAILURE
* \retval  #CMD_LIB_NULL_PARAM
*/
int32_t CmdLib_Decrypt(sProcCryptoData_d *PpsDecVector)
{
    return CmdLib_EncDecHelper(PpsDecVector,CMD_DECDATA,PARAM_DEC_DATA);
}
#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH*/

/**
* @}
*/
