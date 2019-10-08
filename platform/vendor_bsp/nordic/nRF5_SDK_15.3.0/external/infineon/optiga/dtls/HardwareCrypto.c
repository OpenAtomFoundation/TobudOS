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
 * \file HardwareCrypto.c
 *
 * \brief This file provides APIs for hardware crypto layer.
 *
 * \addtogroup grMutualAuth
 * @{
 *
 */

#include "optiga/common/Util.h"
#include "optiga/dtls/HardwareCrypto.h"
#include "optiga/dtls/OcpCommon.h"
#include "optiga/cmd/CommandLib.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/**
 * Initialises the Hardware Crypto Layer.
 *
 * \param[in,out] PpsCL                 Pointer to #sHardwareCrypto_d structure.
 * \param[in]     PpParam               Pointer to the sessionKeyOID to be used for Encryption and Decryption.
 *
 * \retval    #OCP_CL_OK  Successful execution
 * \retval    #OCP_CL_ERROR    Failure in execution
 *
 */
int32_t HWCL_Init(sCL_d* PpsCL, const void* PpParam)
{
    int32_t i4Status = (int32_t)OCP_CL_ERROR;
    do
    {
        PpsCL->phCryptoHdl = (sHardwareCrypto_d*)OCP_MALLOC(sizeof(sHardwareCrypto_d));
        if(NULL == PpsCL->phCryptoHdl)
        {
            i4Status = (int32_t)OCP_CL_MALLOC_FAILURE;
            break;
        }
        memset(PpsCL->phCryptoHdl, 0x00, sizeof(sHardwareCrypto_d));

        ((sHardwareCrypto_d*)PpsCL->phCryptoHdl)->wSessionKeyOID = *((uint16_t*)PpParam);

        i4Status = OCP_CL_OK;
    }while(FALSE);
    return i4Status;
}

/**
 * Encrypts the input plain text using Security chip.
 * Under some erroneous conditions, error codes from Command Library can also be returned. <br>
 *
 * \param[in]       PpsCL                 Pointer to #sHardwareCrypto_d structure.
 * \param[in]       PpsBlobPlainText      Pointer to sbBlob_d containing plain text.
 * \param[in,out]   PpsBlobCipherText     Pointer to sbBlob_d containing cipher text.
 * \param[in]       PwLen                 Length of data to be encrypted.
 *
 * \retval    #OCP_CL_OK        Successful execution
 * \retval    #OCP_CL_ERROR     Failure in execution
 *
 */
int32_t HWCL_Encrypt(const sCL_d* PpsCL, const sbBlob_d* PpsBlobPlainText,sbBlob_d* PpsBlobCipherText,uint16_t PwLen)
{
    int32_t i4Status = (int32_t)OCP_CL_ERROR;
    sProcCryptoData_d sProcCryptoData;

    do
    {
        //Null Check
        if((NULL == PpsBlobPlainText)||(NULL == PpsBlobPlainText->prgbStream) ||
        (NULL == PpsBlobCipherText)|| (NULL == PpsBlobCipherText->prgbStream) || (NULL == PpsCL))
        {
            i4Status = (int32_t)OCP_CL_NULL_PARAM;
            break;
        }

        //Length check for input parameters
        if(0 == PwLen)
        {
            i4Status = (int32_t)OCP_CL_ZERO_LEN;
            break;
        }

        //Length check for input parameters
        if(PpsBlobPlainText->wLen < (PwLen + OVERHEAD_UPDOWNLINK) ||
        (PpsBlobCipherText->wLen < (PwLen + OVERHEAD_ENCDEC_RESPONSE + MAC_LENGTH + EXPLICIT_NOUNCE_LENGTH)))
        {
            i4Status = (int32_t)OCP_CL_INSUFFICIENT_MEMORY;
            break;
        }

        //Assign the required parameter(s) for the Encrypt Message command
        sProcCryptoData.sInData.prgbStream = PpsBlobPlainText->prgbStream;
        sProcCryptoData.sInData.wLen = PpsBlobPlainText->wLen;

        sProcCryptoData.wInDataLength = PwLen;
        sProcCryptoData.wSessionKeyOID = ((sHardwareCrypto_d*)PpsCL->phCryptoHdl)->wSessionKeyOID;

        sProcCryptoData.sOutData.prgbBuffer = PpsBlobCipherText->prgbStream;
        sProcCryptoData.sOutData.wBufferLength = PpsBlobCipherText->wLen;

        //Invoke the encrypt command API from the command library
        i4Status = CmdLib_Encrypt(&sProcCryptoData);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }

        PpsBlobCipherText->wLen = sProcCryptoData.sOutData.wRespLength;

        i4Status = (int32_t)OCP_CL_OK;
    }while(FALSE);
    return i4Status;

}

/**
 * Decrypts the input cipher text using Security chip.
 * Under some erroneous conditions, error codes from Command Library can also be returned. <br>
 *
 * \param[in] PpsCL                 Pointer to #sHardwareCrypto_d structure.
 * \param[in] PpsBlobCipherText     Pointer to sbBlob_d containing cipher text.
 * \param[in,out] PpsBlobPlainText  Pointer to sbBlob_d containing plain text.
 * \param[in] PwLen                 Length of data to be decrypted.
 *
 * \retval    #OCP_CL_OK  Successful execution
 * \retval    #OCP_CL_ERROR    Failure in execution
 *
 */
int32_t HWCL_Decrypt(const sCL_d* PpsCL,const sbBlob_d* PpsBlobCipherText,sbBlob_d* PpsBlobPlainText,uint16_t PwLen)
{
    int32_t i4Status = (int32_t)OCP_CL_ERROR;
    sProcCryptoData_d sProcCryptoData;

    do
    {
        //Null Check
        if((NULL == PpsBlobPlainText)||(NULL == PpsBlobPlainText->prgbStream) ||
        (NULL == PpsBlobCipherText)|| (NULL == PpsBlobCipherText->prgbStream) || (NULL == PpsCL))
        {
            i4Status = (int32_t)OCP_CL_NULL_PARAM;
            break;
        }

        //Length check for input parameters
        if(0 == PwLen)
        {
            i4Status = (int32_t)OCP_CL_ZERO_LEN;
            break;
        }

        //Length check for input parameters
        if((PpsBlobPlainText->wLen < (PwLen + OVERHEAD_ENCDEC_RESPONSE - (MAC_LENGTH + EXPLICIT_NOUNCE_LENGTH))) ||
        (PpsBlobCipherText->wLen < (PwLen + OVERHEAD_UPDOWNLINK)))
        {
            i4Status = (int32_t)OCP_CL_INSUFFICIENT_MEMORY;
            break;
        }

        //Assign the required parameter(s) for the Decrypt Message command
        sProcCryptoData.sInData.prgbStream = PpsBlobCipherText->prgbStream;
        sProcCryptoData.sInData.wLen = PpsBlobCipherText->wLen;

        sProcCryptoData.wInDataLength = PwLen;
        sProcCryptoData.wSessionKeyOID = ((sHardwareCrypto_d*)PpsCL->phCryptoHdl)->wSessionKeyOID;

        sProcCryptoData.sOutData.prgbBuffer = PpsBlobPlainText->prgbStream;
        sProcCryptoData.sOutData.wBufferLength = PpsBlobPlainText->wLen;

        LOG_TRANSPORTMSG("Encrypted Data sent to OPTIGA",eInfo);

        //Invoke the Decrypt command API from the command library
        i4Status = CmdLib_Decrypt(&sProcCryptoData);
        if(CMD_LIB_OK != i4Status)
        {
            LOG_TRANSPORTDBVAL(i4Status,eInfo);
            break;
        }

        PpsBlobPlainText->wLen = sProcCryptoData.sOutData.wRespLength;

        //To log the decrypted data
        LOG_TRANSPORTDBARY("Decrypted Data", sProcCryptoData.sOutData.prgbBuffer, PpsBlobPlainText->wLen, eInfo);
        i4Status = (int32_t) OCP_CL_OK;
    }while(FALSE);
    return i4Status;

}

/**
 * Closes the Crypto layer.
 *
 * \param[in,out] PpsCL       Pointer to #sHardwareCrypto_d structure.
 *
 */
void HWCL_Close(sCL_d* PpsCL)
{
    if((NULL != PpsCL) && (NULL != PpsCL->phCryptoHdl))
    {
        OCP_FREE(PpsCL->phCryptoHdl);
        PpsCL->phCryptoHdl = NULL;
    }
}

#endif //MODULE_ENABLE_DTLS_MUTUAL_AUTH
