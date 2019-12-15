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
* \file AlertProtocol.c
*
* \brief   This file implements the DTLS Alert Protocol.
*
* \addtogroup  grMutualAuth
* @{
*/

#include "optiga/dtls/DtlsRecordLayer.h"
#include "optiga/dtls/AlertProtocol.h"
#include "optiga/dtls/DtlsFlightHandler.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH


/// @cond hidden

/// Maximum size of Alert Message
#define LENGTH_ALERT_MSG 0x02

/// Offset for Alert Message
#define OFFSET_ALERT_MSG 0x01

//Device Error codes

///Invalid OID
#define INVALID_OID                             0x01
///Invalid param field in the command
#define INVALID_PARAM_FIELD                     0x03
///Invalid length field in the command
#define INVALID_LENGTH_FIELD                    0x04
///Invalid parameter in the data field
#define INVALID_PARAMETER_DATA_FIELD            0x05
///Device internal error
#define INTERNAL_PROCESS_ERROR                  0x06
///Invalid command field
#define INVALID_COMMAND_FIELD                   0x0A
///Command out of sequence
#define COMMAND_OUT_SEQUENCE                    0x0B
///Command not available
#define COMMAND_NOT_AVAILABLE                   0x0C
///Illegal parameter in the Handshake header
#define INVALID_HANDSHAKE_MESSAGE               0x21
///DTLS Protocol version mismatch
#define VERSION_MISMATCH                        0x22
///Cipher suite mismatch between client and server
#define INSUFFICIENT_UNSUPPORTED_CIPHERSUITE    0x23
///Unsupported extension
#define UNSUPPORTED_EXTENSION                   0x24
///Unsupported parameters
#define UNSUPPORTED_PARAMETERS                  0x25
///Invalid Trust Anchor
#define INVALID_TRUST_ANCHOR                    0x26
///Trust Anchor expired
#define TRUST_ANCHOR_EXPIRED                    0x27
///Unsupported Trust Anchor
#define UNSUPPORTED_TRUST_ANCHOR                0x28
///Invalid Certificate format
#define INVALID_CERTIFICATE_FORMAT              0x29
///Unsupported certificate/Unsupported Hash or Sign Algorithm
#define UNSUPPORTED_CERTIFICATE_HASHSIGN        0x2A
///Certificate expired
#define CERTIFICATE_EXPIRED                     0x2B
///Signature verification failed
#define SIGNATURE_VERIFICATION_FAILURE          0x2C


/**
 * \brief  DTLS Alert Level.
 */
typedef enum eAlertLevel_d
{
    ///Connection can continue
    eWARNING = 0x01,
    ///Terminate the connection
    eFATAL = 0x02
}eAlertLevel_d;

/**
 * \brief  DTLS Alert Types.
 */
 typedef enum eAlertMsg_d
 {
     /// Notifies the recipient that the sender will not send any more messages on this connection
     eCLOSE_NOTIFY = 0x00,
     /// Inappropriate message was received
     eUNEXPECTED_MESSAGE = 0x0A ,
     /// Notifies record is received with an incorrect MAC
     eBAD_RECORD_MAC = 0x14,
     ///Decryption Failure
     eDECRYPTION_FAILURE = 0x15,
     /// Notifies record received length is more than 2^14+2048
     eRECORD_OVERFLOW = 0x16,
     /// Notifies decompression function received improper input
     eDECOMPRESSION_FAILURE = 0x1E,
     /// Indicates sender was not able to negotiate with the security parameters
     eHANDSHAKE_FAILURE = 0x28,
     /// Notifies certificate was corrupt
     eBAD_CERTIFICATE = 0x2A,
     ///No certificate
     eNO_CERTIFICATE = 0x29,
     /// Notifies certificate was unsupported type
     eUNSUPPORTED_CERTIFICATE = 0x2B,
     /// Notifies the certificate was revoked by signer
     eCERTIFICATE_REVOKED = 0x2C,
     /// Indicates the certificate is Expired
     eCERTIFICATE_EXPIRED = 0x2D,
     /// Indicates unknown issue in processing the certificate
     eCERTIFICATE_UNKNOWN = 0x2E,
     /// Notifies field in handshake is out of range or inconsistent
     eILLEGAL_PARAMETER = 0x2F,
     /// Indicates CA certificate could not be found or not matched
     eUNKNOWN_CA = 0x30,
     /// Notifies the access denied
     eACCESS_DENIED = 0x31,
     /// Notifies message could not be decoded or some field is missing
     eDECODE_ERROR = 0x32,
     /// Notifies cryptographic operation failed
     eDECRYPT_ERROR = 0x33,
     ///Export restriction
     eEXPORT_RESTRICTION = 0x3C,
     /// Notifies protocol version attempted to negotiate is not supported
     ePROTOCOL_VERSION = 0x46,
     /// Notifies negotiation has failed specifically because the server requires ciphers more secure
     eINSUFFICIENT_SECURITY = 0x47,
     /// Notifies error is unrelated to peer or protocol
     eINTERNAL_ERROR = 0x50,
     /// Indicates that the handshake is canceled
     eUSER_CANCELLED = 0x5A,
     /// Notifies that the renegotiation is not initiated
     eNO_RENEGOTIATION = 0x64,
     /// Notifies unsupported extension was sent to server
     eUNSUPPORTED_EXTENSION = 0x6E
 }eAlertMsg_d;
/// @endcond

/**
 * \brief Maps the Alert types and level to error code.<br>
 */
_STATIC_H int32_t DtlsAlertErrorMapping(const sbBlob_d* PpsAlertMsg, int32_t* Ppi4ErrorCode);

//Alert protocol is defined by default. To disable define DISABLE_ALERT
#ifndef DISABLE_ALERT

 /**
 * \brief Maps the error code to Alert types and level.<br>
 */
_STATIC_H Void DtlsErrorAlertMapping(int32_t Pi4ErrorCode, sbBlob_d* PpsAlertMsg);

/**
 * \brief Forms the alert message based on the given internal error code.<br>
 */
_STATIC_H Void Alert_FormMsg(int32_t Pi4ErrorCode,sbBlob_d* PpsAlertMsg);

/**
 * Maps the error code to Alert types and level.<br>
 *
 * \param[in]       Pi4ErrorCode     DTLS Internal error code
 * \param[in,out]   PpsAlertMsg      Pointer to a blob containing Alert message as per DTLS Specification
 *
 */
_STATIC_H Void DtlsErrorAlertMapping(int32_t Pi4ErrorCode, sbBlob_d* PpsAlertMsg)
{
    do
    {
        if((int32_t)OCP_LIB_NO_RENEGOTIATE == Pi4ErrorCode)
        {
            *PpsAlertMsg->prgbStream = (uint8_t)eWARNING;
        }
        else
        {
            *PpsAlertMsg->prgbStream = (uint8_t)eFATAL;
        }
        //Set the Blob length to Alert message length
        PpsAlertMsg->wLen = LENGTH_ALERT_MSG;

        switch(Pi4ErrorCode)
        {
            case (int32_t)OCP_RL_ERROR:
            case (int32_t)OCP_FL_MSG_MAXCOUNT:
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eCLOSE_NOTIFY;
                break;
            case (int32_t)(CMD_DEV_ERROR | INVALID_HANDSHAKE_MESSAGE):
            case (int32_t)(CMD_DEV_ERROR | UNSUPPORTED_PARAMETERS):
            case (int32_t)(CMD_DEV_ERROR | VERSION_MISMATCH):
            case (int32_t) OCP_FL_HS_ERROR:
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eILLEGAL_PARAMETER;
                break;
            case (int32_t)OCP_LIB_NO_RENEGOTIATE:
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eNO_RENEGOTIATION;
                break;
            case (int32_t)(CMD_DEV_ERROR | INSUFFICIENT_UNSUPPORTED_CIPHERSUITE):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eINSUFFICIENT_SECURITY;
                break;
            case (int32_t)(CMD_DEV_ERROR | UNSUPPORTED_EXTENSION):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eUNSUPPORTED_EXTENSION;
                break;

            case (int32_t)(CMD_DEV_ERROR | INVALID_TRUST_ANCHOR):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eUNKNOWN_CA;
                break;
            case (int32_t)(CMD_DEV_ERROR | TRUST_ANCHOR_EXPIRED):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eCERTIFICATE_EXPIRED;
                break;
            case (int32_t)(CMD_DEV_ERROR | UNSUPPORTED_TRUST_ANCHOR):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eUNSUPPORTED_CERTIFICATE;
                break;
            case (int32_t)(CMD_DEV_ERROR | INVALID_CERTIFICATE_FORMAT):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eBAD_CERTIFICATE;
                break;
            case (int32_t)(CMD_DEV_ERROR | UNSUPPORTED_CERTIFICATE_HASHSIGN):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eUNSUPPORTED_CERTIFICATE;
                break;
            case (int32_t)(CMD_DEV_ERROR | CERTIFICATE_EXPIRED):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eCERTIFICATE_EXPIRED;
                break;
            case (int32_t)(CMD_DEV_ERROR | SIGNATURE_VERIFICATION_FAILURE):
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eDECRYPT_ERROR;
                break;
            default:
                //lint -e750 "The remaining errors returned by the security chip is mapped to Internal error Alert"
                //Prepare Alert Message
                *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG) = (uint8_t)eINTERNAL_ERROR;
            break;
        }
    }while(0);
}

/**
 * Forms the alert message based on the given internal error code.<br>
 *
 * \param[in]       Pi4ErrorCode     DTLS Internal error code
 * \param[in,out]   PpsAlertMsg      Pointer to a blob containing Alert message as per DTLS Specification
 *
 */
_STATIC_H Void Alert_FormMsg(int32_t Pi4ErrorCode,sbBlob_d* PpsAlertMsg)
{
    //Maps the internal error code to the Alert messages
    DtlsErrorAlertMapping(Pi4ErrorCode,PpsAlertMsg);
}

#endif //DISABLE_ALERT

/**
 * Maps the Alert types and level to error code.<br>
 *
 * \param[in]       PpsAlertMsg      Pointer to a blob containing Alert message as per DTLS Specification
 * \param[in,out]   Ppi4ErrorCode    Pointer to the DTLS Internal error code
 *
 * \retval    #OCP_AL_OK  Successful execution
 * \retval    #OCP_AL_ERROR    Failure in execution
 *
 */
_STATIC_H int32_t DtlsAlertErrorMapping(const sbBlob_d* PpsAlertMsg, int32_t* Ppi4ErrorCode)
{
    int32_t i4Status = (int32_t)OCP_AL_ERROR;

    do
    {
        //Check for the Alert level type
        if(eFATAL == (eAlertLevel_d)*PpsAlertMsg->prgbStream)
        {
            //Check for various fatal alert messages
            switch((eAlertMsg_d) *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG))
            {
                case eCLOSE_NOTIFY:
                case eUNEXPECTED_MESSAGE:
                case eBAD_RECORD_MAC:
                case eDECRYPTION_FAILURE:
                case eRECORD_OVERFLOW:
                case eDECOMPRESSION_FAILURE:
                case eHANDSHAKE_FAILURE:
                case eBAD_CERTIFICATE:
                case eUNSUPPORTED_CERTIFICATE:
                case eNO_CERTIFICATE:
                case eCERTIFICATE_REVOKED:
                case eCERTIFICATE_EXPIRED:
                case eCERTIFICATE_UNKNOWN:
                case eUSER_CANCELLED:
                case eNO_RENEGOTIATION:
                case eILLEGAL_PARAMETER:
                case eUNKNOWN_CA:
                case eACCESS_DENIED:
                case eDECODE_ERROR:
                case eDECRYPT_ERROR:
                case eEXPORT_RESTRICTION:
                case ePROTOCOL_VERSION:
                case eINSUFFICIENT_SECURITY:
                case eINTERNAL_ERROR:
                case eUNSUPPORTED_EXTENSION:
                {
                    *Ppi4ErrorCode = (int32_t)OCP_AL_FATAL_ERROR;
                    i4Status = (int32_t)OCP_AL_OK;
                }
                break;
                default:
                //Indicates the received Alert is not a valid Fatal Error

                break;
            }
        }
        //Check for Warning Alert level type
        else if (eWARNING == (eAlertLevel_d)*PpsAlertMsg->prgbStream)
        {
            //Check for various warning alert messages
            switch((eAlertMsg_d) *(PpsAlertMsg->prgbStream + OFFSET_ALERT_MSG))
            {
                case eBAD_CERTIFICATE:
                case eUNSUPPORTED_CERTIFICATE:
                case eCERTIFICATE_REVOKED:
                case eCERTIFICATE_EXPIRED:
                case eCERTIFICATE_UNKNOWN:
                case eUSER_CANCELLED:
                case eNO_RENEGOTIATION:
                {

                    *Ppi4ErrorCode = (int32_t)OCP_AL_WARNING_ERROR;
                    i4Status = (int32_t)OCP_AL_OK;
                    break;
                }
                default:
                //lint -e788 suppress "As the enum values are divided between Fatal and warning levels"
                //Indicates the received Alert is not a valid warning Error

                break;
            }
        }
    }while(0);
    return i4Status;
}


#ifndef DISABLE_ALERT
/**
 * Sends Alert based on the internal error code via the Record Layer.<br>
 *
 * \param[in]   PpsConfigRL     Pointer to structure containing Record Layer information.
 * \param[in]   Pi4ErrorCode    DTLS Internal error code
 *
 */
void Alert_Send(sConfigRL_d *PpsConfigRL,int32_t Pi4ErrorCode)
{
    int32_t i4Status  = (int32_t)OCP_AL_ERROR;
    sbBlob_d sAlertMsg;
    uint8_t bEncFlag = 0;
    uint8_t bFlagIncr = 0;
    uint8_t rgbAlertMsg[LENGTH_ALERT_MSG];

    //Null checks
    if((NULL != PpsConfigRL) && (NULL != PpsConfigRL->pfSend) && (NULL != PpsConfigRL->sRL.phRLHdl))
    {
        do
        {
/// @cond hidden
#define PS_RECORDLAYER ((sRecordLayer_d*)PpsConfigRL->sRL.phRLHdl)
/// @endcond

            sAlertMsg.prgbStream = rgbAlertMsg;
            sAlertMsg.wLen = LENGTH_ALERT_MSG;

            //Form the Alert message based on internal error code
            Alert_FormMsg(Pi4ErrorCode, &sAlertMsg);

            PpsConfigRL->sRL.bMemoryAllocated = FALSE;
            PpsConfigRL->sRL.bContentType = CONTENTTYPE_ALERT;

            //Until successful completion of Mutual Authentication Public Key Scheme (DTLS) the Client should use previous epoch and messages must not be encrypted
            if(((PS_RECORDLAYER->wServerEpoch != PS_RECORDLAYER->wClientNextEpoch) && (*PS_RECORDLAYER->pbDec != 0x01)) ||
                (Pi4ErrorCode == (int32_t)OCP_FL_INT_ERROR) || (Pi4ErrorCode == (int32_t)OCP_FL_HS_ERROR))
            {
                if((PS_RECORDLAYER->bEncDecFlag == ENC_DEC_ENABLED) && (PS_RECORDLAYER->wClientEpoch != PS_RECORDLAYER->wClientNextEpoch))
                {
                    bEncFlag = PS_RECORDLAYER->bEncDecFlag;
                    PS_RECORDLAYER->bEncDecFlag = ENC_DEC_DISABLED;
                    PS_RECORDLAYER->wClientNextEpoch--;
                    bFlagIncr = 0x01;
                }
            }
            //Send the Alert message via record layer
            i4Status = PpsConfigRL->pfSend(&PpsConfigRL->sRL, sAlertMsg.prgbStream, sAlertMsg.wLen);

            if(bFlagIncr == 0x01)
            {
                PS_RECORDLAYER->bEncDecFlag = bEncFlag;
                PS_RECORDLAYER->wClientNextEpoch++;
            }

            if(OCP_RL_OK != i4Status)
            {
                break;
            }

        }while(FALSE);

/// @cond hidden
#undef PS_RECORDLAYER
/// @endcond
    }
}
#endif //DISABLE_ALERT
/**
 * Processes the received Alert Message<br>
 * Returns the corresponding internal error code.<br>
 *
 * \param[in]       PpsAlertMsg      Pointer to a blob containing Alert message as per DTLS Specification
 * \param[in,out]   Ppi4ErrorCode    Pointer to the DTLS Internal error code
 *
 * \retval    #OCP_AL_OK                Successful execution
 * \retval    #OCP_AL_ERROR             Failure in execution
\if ENABLE_NULL_CHECKS
 * \retval    #OCP_AL_NULL_PARAM        Null parameter(s)
\endif
 * \retval    #OCP_AL_LENZERO_ERROR     Length of input parameter is zero
 *
 */
int32_t Alert_ProcessMsg(const sbBlob_d* PpsAlertMsg,int32_t* Ppi4ErrorCode)
{
    int32_t i4Status = (int32_t)OCP_AL_ERROR;

    do
    {
#ifdef ENABLE_NULL_CHECKS
        //NULL check for the input parameters
        if((NULL == PpsAlertMsg) || (NULL == Ppi4ErrorCode)|| (NULL == PpsAlertMsg->prgbStream))
        {
            i4Status  = (int32_t)OCP_AL_NULL_PARAM;
            break;
        }
#endif
        //Check for length is less than Alert message size
        if(LENGTH_ALERT_MSG != PpsAlertMsg->wLen)
        {
            break;
        }
        //Maps the received Alert messages to the internal error codes
        i4Status = DtlsAlertErrorMapping(PpsAlertMsg, Ppi4ErrorCode);
    }while(0);
    return i4Status;
}

/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH */
