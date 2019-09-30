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
*          Command Library implementation.
*
* \addtogroup  grCmdLib
* @{
*/
#ifndef _CMD_LIB_H_
#define _CMD_LIB_H_

#include <stdint.h>
#include "optiga/common/Datatypes.h"
#include "optiga/common/ErrorCodes.h"
#include "optiga/common/AuthLibSettings.h"
#include "optiga/comms/optiga_comms.h"

/****************************************************************************
 *
 * API Error Codes
 *
 ****************************************************************************/
///Requested operation completed without any error
#define CMD_LIB_OK                              0x75E96B01

///Device error, the device error(last error code) is available in the least significant byte e.g. for Invalid OID 0x80010001 is returned.
#define CMD_DEV_ERROR                           0x80010000

///An error to indicate that command execution failed.It is possibly that the application on security chip is not open or communication failure has occurred.
#define CMD_DEV_EXEC_ERROR                      0x80020000

///Null parameter(s)
#define CMD_LIB_NULL_PARAM                      0x80001001

///Invalid or unsupported parameter(s)
#define CMD_LIB_INVALID_PARAM                   (CMD_LIB_NULL_PARAM + 1)

///Insufficient memory
#define CMD_LIB_INSUFFICIENT_MEMORY             (CMD_LIB_NULL_PARAM + 2)

///Length of Input is zero
#define CMD_LIB_LENZERO_ERROR                   (CMD_LIB_NULL_PARAM + 3)

///Invalid Session ID
#define CMD_LIB_INVALID_SESSIONID               (CMD_LIB_NULL_PARAM + 4)

///Invalid Tag
#define CMD_LIB_INVALID_TAG                     (CMD_LIB_NULL_PARAM + 5)

///Invalid Tag Length
#define CMD_LIB_INVALID_TAGLEN                  (CMD_LIB_NULL_PARAM + 6)

///Invalid Length
#define CMD_LIB_INVALID_LEN                     (CMD_LIB_NULL_PARAM + 7)

///Decryption failed
#define CMD_LIB_DECRYPT_FAILURE                 (CMD_LIB_NULL_PARAM + 8)

///Invalid OID
#define CMD_LIB_INVALID_OID                     (CMD_LIB_NULL_PARAM + 9)

///Generic error condition
#define CMD_LIB_ERROR                            0xF87ECF01

///Length of the hash context information for SHA 256
#define CALC_HASH_SHA256_CONTEXT_SIZE           (130)

///Overhead for all hash calculation operations (Tag and Length size (3 bytes) + APDU header (4 bytes))
#define CALC_HASH_FIXED_OVERHEAD_SIZE           (0x03 + 0x04)

///Overhead for import or export hash context (Tag and Length size (3 bytes))
#define CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE  (0x03)

///Overhead for import and export hash context
#define CALC_HASH_IMPORT_AND_EXPORT_OVERHEAD_SIZE  (0x06)

/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/


/**
 * \brief Structure to receive the response of the device for a command.
 */
typedef struct sCmdResponse_d
{
    ///The length of the buffer
    uint16_t    wBufferLength;

    ///Pointer to the buffer to copy the response
    uint8_t    *prgbBuffer;

    ///Actual length of the response
    uint16_t    wRespLength;
}sCmdResponse_d;

/**
 * \brief Function to send a command and receive response for the command.
 */
typedef int32_t (*pFTransceive)(const void* ctx,const uint8_t *PprgbWriteBuffer, const uint16_t *PpwWriteBufferLen, uint8_t *PprgbReadBuffer, uint16_t *PpwReadBufferLen);


/****************************************************************************
 *
 * Definitions related to OpenApplication and CloseApplication commands.
 *
 ****************************************************************************/
/**
 * \brief Enumerations to open the application on security chip.
 */
typedef enum eOpenType_d
{
    ///Initialise a clean application context
    eInit = 0x00
}eOpenType_d;

/**
 * \brief Structure to specify open application command parameters.
 */
typedef struct sOpenApp_d
{
    ///Type of option for Open application
    eOpenType_d eOpenType;
}sOpenApp_d;

/**
 * \brief Opens the Security Chip Application.

 */
LIBRARY_EXPORTS int32_t CmdLib_OpenApplication(const sOpenApp_d* PpsOpenApp);

/// @cond hidden
LIBRARY_EXPORTS void CmdLib_SetOptigaCommsContext(const optiga_comms_t *p_input_optiga_comms);
/// @endcond
/****************************************************************************
 *
 * Definitions related to GetDataObject and SetDataObject commands.
 *
 ****************************************************************************/

/**
 * \brief Enumeration to specify data or metadata.
 */
typedef enum eDataOrMedata_d
{
    ///Read or write data
    eDATA,

    ///Read or write metadata
    eMETA_DATA
}eDataOrMedata_d;


/**
 * \brief Enumeration to specify write option.
 */
typedef enum eWriteOption_d
{
    ///Perform write operation
    eWRITE,

    ///Perform erase and write operation
    eERASE_AND_WRITE
}eWriteOption_d;

/**
 * \brief Structure to specify GetDataObject command parameters.
 */
typedef struct sGetData_d
{
    ///OID of data object
    uint16_t wOID;

    ///Offset within the data object
    uint16_t wOffset;

    ///Number of data bytes to read
    uint16_t wLength;

    ///To read data or metadata
    eDataOrMedata_d  eDataOrMdata;
}sGetData_d;

/**
 * \brief Structure to specify SetDataObject command parameters.
 */
typedef struct sSetData_d
{
    ///OID of data object
    uint16_t wOID;

    ///Offset within the data object
    uint16_t wOffset;

    ///Number of data bytes to write
    uint16_t wLength;

    ///Data bytes to be written
    uint8_t *prgbData;

    ///To write data or metadata
    eDataOrMedata_d  eDataOrMdata;

    ///Write option
    eWriteOption_d   eWriteOption;
}sSetData_d;


#ifdef MODULE_ENABLE_READ_WRITE
/**
 * \brief Reads the specified data object by issuing GetDataObject command.
 */
LIBRARY_EXPORTS int32_t CmdLib_GetDataObject(const sGetData_d *PpsGDVector, sCmdResponse_d *PpsResponse);

/**
 * \brief Writes to the specified data object by issuing SetDataObject command.
 */
LIBRARY_EXPORTS int32_t CmdLib_SetDataObject(const sSetData_d *PpsSDVector);

/**
 * \brief Reads maximum communication buffer size supported by the security chip.
 */
LIBRARY_EXPORTS uint16_t CmdLib_GetMaxCommsBufferSize(Void);

#endif
/****************************************************************************
 *
 * Definitions related to authentication commands.
 *
 ****************************************************************************/

///Challenge minimum length
#define CHALLENGE_MIN_LEN               0x08

///Challenge maximum length
#define CHALLENGE_MAX_LEN               0x100

/**
 * \brief Enumeration to specify an authentication scheme.
 */
typedef enum eAuthScheme_d
{
    ///The ECDSA signature scheme as specified within FIPS 186-3 (SHA256)
    eECDSA = 0x91,
    ///DTLS Client scheme with cipher suite as DTLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
    eDTLSClient = 0x99
}eAuthScheme_d;

/**
 * \brief Structure to specify authentication command parameters.
 */
typedef struct sAuthMsg_d
{
    ///Challenge length
    uint16_t wRndLength;

    ///Challenge
    uint8_t *prgbRnd;

    ///Authentication scheme
    eAuthScheme_d eAuthScheme;

    ///Private key
    uint16_t wOIDDevPrivKey;
}sAuthMsg_d;

/**
 * \brief Structure to specify authentication Scheme parameters.
 */
typedef struct sAuthScheme_d
{
    ///OID of Device Private key
    uint16_t wDevicePrivKey;

    ///OID of Session Key
    uint16_t wSessionKeyId;

    ///Authentication scheme
    eAuthScheme_d eAuthScheme;

}sAuthScheme_d;

/**
 * \brief Enumeration to specify the type of the random number
 */
typedef enum eRngType_d
{
    ///True random number
    eTRNG = 0x00,
    ///Deterministic random number
    eDRNG = 0x01,
}eRngType_d;


/**
* \brief Structure to specify option to generate random number
*/
typedef struct sRngOptions_d
{
    ///Type of Random number
    eRngType_d eRngType;
    ///Length of the random number to be generated
    uint16_t wRandomDataLen;
}sRngOptions_d;

/**
 * \brief  Gets the signature generated by Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_GetSignature(const sAuthMsg_d *PpsAuthMsg, sCmdResponse_d *PpsResponse);

/**
 * \brief Gets the true random bytes generated by Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_GetRandom(const sRngOptions_d *PpsRng, sCmdResponse_d *PpsResponse);

/**
 * \brief Sets the Authentication Scheme by issuing SetAuthScheme command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_SetAuthScheme(const sAuthScheme_d *PpsAuthVector);

/**
 * \brief Enumeration to specify Hashing algorithm.
 */
typedef enum eHashAlg_d
{
    ///SHA256 for Hashing
    eSHA256 = 0xE2
}eHashAlg_d;

/**
 * \brief Enumeration to specify Hashing Sequence.
 */
typedef enum eHashSequence_d
{
    ///Start hashing
    eStartHash = 0x00,

    ///Start and Finalize hashing
    eStartFinalizeHash = 0x01,

    ///Continue hashing
    eContinueHash = 0x02,

    ///Finalize hashing
    eFinalizeHash = 0x03,

    ///Terminate hashing
    eTerminateHash = 0x04,

    ///Get Intermediate hash
    eIntermediateHash = 0x05

}eHashSequence_d;

/**
 * \brief Enumeration to specify Hashing data type.
 */
typedef enum eDataType_d
{
    ///Data stream
    eDataStream = 0x00,

    ///OID contents
    eOIDData = 0x01
}eDataType_d;

/**
 * \brief Structure to specify OID information.
 */
typedef struct sOIDInfo_d
{
    ///OID of data object
    uint16_t wOID;

    ///Offset within the data object
    uint16_t wOffset;

    ///Number of data bytes starting from the offset
    uint16_t wLength;

}sOIDInfo_d;

/**
 * \brief Enumeration to specify the action to be performed with the context.
 */
typedef enum eContextAction_d
{
    ///Context data is not used
    eUnused = 0x00,

    ///Import hash context to perform the hash
    eImport = 0x06,

    ///Export the current active hash context
    eExport = 0x07,

    ///Import the provided hash context and export back the hash context after hashing
    eImportExport = 0x67
}eContextAction_d;

/**
 * \brief Structure to specify the information for context information.
 */
typedef struct sContextInfo_d
{
    ///Action on the input context
    eContextAction_d eContextAction;

    ///Pointer to memory , to copy to/from context data
    uint8_t* pbContextData;

    ///Length of the context while importing/ Length of memory while exporting context
    uint16_t dwContextLen;
}sContextInfo_d;

/**
 * \brief Structure to specify parameters for Hash generation.
 */
typedef struct sCalcHash_d
{
    ///Hashing Algorithm
    eHashAlg_d eHashAlg;

    ///Hashing sequence
    eHashSequence_d eHashSequence;

    ///Type of data for hashing
    eDataType_d eHashDataType;

    ///Data stream blob for hashing
    sbBlob_d sDataStream;

    ///Object data for hashing
    sOIDInfo_d sOIDData;

    ///Output Hash
    sCmdResponse_d sOutHash;

    ///Context Information
    sContextInfo_d sContextInfo;
}sCalcHash_d;

/**
 * \brief Structure to specify hash information.
 */
typedef struct sHashinfo
{
    ///Hash Algorithm type
    eHashAlg_d eHashAlgo;

    ///Hash Length
    uint8_t bHashLen;

    ///Hash context data
    uint16_t wHashCntx;
}sHashinfo_d;

/**
 * \brief Enumeration for supported Signing scheme.
 */
typedef enum eSignScheme_d
{
    ///ECDSA FIPS 186-3 w/o hash
    eECDSA_FIPS_186_3_WITHOUT_HASH = 0x11
}eSignScheme_d;

/**
 * \brief Enumeration for supported algorithm identifier.
 */
typedef enum eAlgId_d
{
    ///Elliptic curve key on NIST P256 curve
    eECC_NIST_P256 = 0x03,

    ///Elliptic curve key on NIST P384 curve
    eECC_NIST_P384 = 0x04
}eAlgId_d;

/**
 * \brief Structure to specify public key for verify hash
 */
typedef struct sPubKeyInput_d
{
    ///Algorithm identifier
    eAlgId_d eAlgId;

    ///Data stream containing public key
    sbBlob_d sDataStream;
}sPubKeyInput_d;

/**
 * \brief Structure to specify parameters for Signature verification.
 */
typedef struct sVerifyOption_d
{
    ///Signing scheme
    eSignScheme_d eSignScheme;

    ///Source of the public key
    eDataType_d eVerifyDataType;

    ///Data stream blob for hashing
    sPubKeyInput_d sPubKeyInput;

    ///OID of data object
    uint16_t wOIDPubKey;
}sVerifyOption_d;

/**
 * \brief Structure to return key pair as output.
 */
typedef struct sOutKeyPair_d
{
    ///Blob pointing to memory to store public key
    sbBlob_d sPublicKey;

    ///Blob pointing to memory to store private key
    sbBlob_d sPrivateKey;
}sOutKeyPair_d;

/**
 * \brief Enumeration to specify the action on key pair.
 */
typedef enum eKeyExport_d
{
    ///Stores only the private key in the OID and exports the public key
    eStorePrivKeyOnly = 0x01,

    ///Exports both public and private key as the output
    eExportKeyPair = 0x07
}eKeyExport_d;

/**
 * \brief Enumeration to specify supported key usage.
 */
typedef enum eKeyUsage_d
{
    ///The key is used for authentication
    eAuthentication = 0x01,

    ///The key is used for encryption
    eEncryption = 0x02,

    ///The key is used for host firmware update
    eHostFwUpdate = 0x04,

    ///The key is used for device management
    eDevMgmt = 0x08,

    ///The key is used for signature
    eSign = 0x10,

    ///The key agreement
    eKeyAgreement = 0x20
}eKeyUsage_d;

/**
 * \brief Structure to specify parameters for Key Pair Generation.
 */
typedef struct sKeyPairOption_d
{
    ///Algorithm identifier of the key to be generated
    eAlgId_d eAlgId;

    ///Option to export/store keys
    eKeyExport_d eKeyExport;

    ///OID of the private key/session context to be generated
    uint16_t wOIDPrivKey;

    ///Key Usage
    eKeyUsage_d eKeyUsage;
}sKeyPairOption_d;

/**
 * \brief Structure to specify parameters for signature generation.
 */
typedef struct sCalcSignOptions_d
{
    ///Signing scheme
    eSignScheme_d eSignScheme;

    ///OID of the signature key
    uint16_t wOIDSignKey;

    ///Digest to be signed
    sbBlob_d sDigestToSign;
}sCalcSignOptions_d;

/**
 * \brief Enumeration to specify supported key agreement primitives
 */
typedef enum eKeyAgreementType_d
{
    ///Elliptic Curve Diffie-Hellman shared secret agreement according to NIST SP-800 56A
    eECDH_NISTSP80056A = 0x01
}eKeyAgreementType_d;

/**
 * \brief Structure to specify parameters for shared secret calculation.
 */
typedef struct sCalcSSecOptions_d
{
    ///Key agreement algorithm
    eKeyAgreementType_d eKeyAgreementType;

    ///Algorithm identifier of the public key
    eAlgId_d ePubKeyAlgId;

    ///OID of the private key/session context for shared secret calculation
    uint16_t wOIDPrivKey;

    ///OID of the session context to store the calculated secret
    uint16_t wOIDSharedSecret;

    ///Public key for shared secret calculation
    sbBlob_d sPubKey;
}sCalcSSecOptions_d;

/**
 * \brief Enumeration to specify supported key derivation method
 */
typedef enum eKeyDerivationMethod_d
{
    ///IETF 5246 TLS PRF SHA256
    eTLS_PRF_SHA256 = 0x01
}eKeyDerivationMethod_d;

/**
 * \brief Structure to specify parameters for derive key generation.
 */
typedef struct sDeriveKeyOptions_d
{
    ///Key derivation method
    eKeyDerivationMethod_d eKDM;

    ///OID of Shared Secret to derive the new secret from
    uint16_t wOIDSharedSecret;

    ///The length of the key to be derived
    uint16_t wDerivedKeyLen;

    ///OID to store the Derived key
    uint16_t wOIDDerivedKey;

    ///Seed
    sbBlob_d sSeed;
}sDeriveKeyOptions_d;

#ifdef MODULE_ENABLE_TOOLBOX
/**
 * \brief Calculates the hash on input data by issuing CalcHash command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_CalcHash(sCalcHash_d* PpsCalcHash);

/**
 * \brief Verify the signature on digest by issuing VerifySign command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_VerifySign(const sVerifyOption_d* PpsVerifySign,const sbBlob_d * PpsDigest,const sbBlob_d * PpsSignature);

/**
 * \brief Generate a key pair by issuing GenKeyPair command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_GenerateKeyPair(const sKeyPairOption_d* PpsKeyPairOption,sOutKeyPair_d* PpsOutKeyPair);

/**
 * \brief  Calculate signature on a digest by issuing CalcSign command to the Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_CalculateSign(const sCalcSignOptions_d *PpsCalcSign,sbBlob_d *PpsSignature);

/**
 * \brief  Calculate shared secret by issuing CalcSSec command to the Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_CalculateSharedSecret(const sCalcSSecOptions_d *PpsCalcSSec,sbBlob_d *PpsSecret);

/**
 * \brief  Derive session key by issuing DeriveKey command to the Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_DeriveKey(const sDeriveKeyOptions_d *PpsDeriveKey,sbBlob_d *PpsKey);
#endif/*MODULE_ENABLE_TOOLBOX*/

/****************************************************************************
 *
 * Definitions related to (D)TLS commands.
 *
 ****************************************************************************/

///Length of APDU header
#define LEN_APDUHEADER              0x04

///Number of bytes in Session ID field
#define BYTES_SESSIONID             0x02

///bits per byte
#define BITS_PER_BYTE               0x08

///Tag Encoding length
#define LEN_TAG_ENCODING            0x03

///Additional bytes for Encryption/Decryption
#define ADDITIONALBYTES_ENCDEC      10

///Overhead for all uplink/downlink messages
#define OVERHEAD_UPDOWNLINK        (LEN_APDUHEADER + BYTES_SESSIONID + LEN_TAG_ENCODING + ADDITIONALBYTES_ENCDEC)

//Overhead for Encrypt/Decrypt response
#define OVERHEAD_ENCDEC_RESPONSE        (LEN_APDUHEADER + LEN_TAG_ENCODING)
/**
 * \brief Enumeration to specify the type of message.
 */
typedef enum eMsgType_d
{
    //Client Side Message type
    ///Client Hello
    eClientHello = 0x01,
    ///Client Hello with Cookie
    eClientHelloWithCookie = 0x03,
    ///Client Certificate
    eClientCertificate = 0x0B,
    ///Client Key Exchange
    eClientKeyExchange = 0x10,
    ///Certificate Verify
    eCertificateVerify = 0x0F,
    ///Client Finished
    eClientFinished = 0x14,
    //Server Side Message type
    ///Hello Verify Request
    eHelloVerifyRequest = 0x03,
    ///Server Hello
    eServerHello = 0x02,
    ///Server Certificate
    eServerCertificate = 0x0B,
    ///Server Key Exchange
    eServerKeyExchange = 0x0C,
    ///Certificate Request
    eCertificateRequest = 0x0D,
    ///Server Hello Done
    eServerHelloDone = 0x0E,
    ///Server Finished
    eServerFinished = 0x14,
    ///Change cipher spec
    eChangeCipherSpec = 0x13,
    ///Close session
    eCloseSession = 0x71
}eMsgType_d;

/**
 * \brief Union to specify message specific parameters
 */
typedef union uMsgParams_d
{
    ///Client Hello Params
    struct sMsgParamCH_d
    {
        ///Unix Time
        uint32_t dwUnixTime;
    }sMsgParamCH_d;

    ///Certificate Params
    struct sMsgParamCert_d
    {
        ///Certificate OID
        uint16_t wCertOID;
    }sMsgParamCert_d;

}uMsgParams_d;

///Callback to Caller to accept the message
typedef int32_t (*fAcceptMessage)(Void*,const sbBlob_d*);

/**
 * \brief Structure to specify Call back parameters for Get Message.
 */
typedef struct sCallBack_d
{
    ///Callback to Caller to accept the message
    fAcceptMessage pfAcceptMessage;
    ///Params for Call back
    Void* fvParams;
}sCallBack_d;

/**
 * \brief Structure to specify parameters for (D)TLS handshake messages.
 */
typedef struct sProcMsgData_d
{
    ///Pointer to Input Buffer memory
    sbBlob_d* psBlobInBuffer;

    ///Type of Message
    eMsgType_d eParam;

    ///Session Key OID
    uint16_t wSessionKeyOID;

    ///Message related parameters
    uMsgParams_d* puMsgParams;

    ///Pointer to a structure containing Callback function and parameter pointer to Caller to accept the message
    sCallBack_d *psCallBack;

}sProcMsgData_d;


/**
 * \brief Structure to specify ProcUpLink/ProcDownLink command parameters.
 */
typedef struct sProcCryptoData_d
{
    ///Contains data to be encrypted/decrypted.This should contain enough memory to accommodate APDU header, data formatting and the data for encryption/decryption
    sbBlob_d sInData;

    ///Actual length of data to be encrypted/decrypted
    uint16_t wInDataLength;

    ///Session Key OID
    uint16_t wSessionKeyOID;

    ///Holds the output data after encryption/decryption
    sCmdResponse_d sOutData;

}sProcCryptoData_d;


#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH
/**
 * \brief Generates Uplink message by issuing ProcUpLink command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_GetMessage(const sProcMsgData_d *PpsGMsgVector);

/**
 * \brief Process Authentication message by issuing ProcDownLink command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_PutMessage(const sProcMsgData_d *PpsPMsgVector);

/**
 * \brief Encrypts data by issuing ProcUpLink command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_Encrypt(sProcCryptoData_d *PpsEncVector);

/**
 * \brief Decrypts data by issuing ProcDownLink command to Security Chip.
 */
LIBRARY_EXPORTS int32_t CmdLib_Decrypt(sProcCryptoData_d *PpsDecVector);

/**
 * \brief Closes the Security Chip session as indicated by the Session Reference Id.
 */
LIBRARY_EXPORTS int32_t CmdLib_CloseSession(uint16_t PwSessionRefId);
#endif /* MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
#endif //_CMD_LIB_H_

/**
* @}
*/
