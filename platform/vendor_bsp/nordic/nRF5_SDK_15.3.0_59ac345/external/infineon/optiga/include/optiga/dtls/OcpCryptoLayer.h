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
*          OCP Crypto Layer.
*
* \addtogroup  grMutualAuth
* @{
*/

#ifndef __OCPCRYPTOLAYER_H__
#define __OCPCRYPTOLAYER_H__

#include "optiga/common/Datatypes.h"
#include "optiga/common/Logger.h"
#include "optiga/dtls/OcpCommonIncludes.h"

/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/

/// Failure in execution
#define OCP_CL_ERROR                    0x51667F82

///Successful Execution
#define OCP_CL_OK                       0x4C36F247

///Null paramter
#define OCP_CL_NULL_PARAM               BASE_ERROR_CRYPTO_LAYER

///Malloc Failure in Crypto
#define OCP_CL_MALLOC_FAILURE           (BASE_ERROR_CRYPTO_LAYER + 1)

///Zero Length Failure
#define OCP_CL_ZERO_LEN                 (BASE_ERROR_CRYPTO_LAYER + 2)

///Insufficient memory
#define OCP_CL_INSUFFICIENT_MEMORY      (BASE_ERROR_CRYPTO_LAYER + 3)

/**
 * \brief Structure containing Crypto Layer information.
 */
typedef struct sCL_d
{
    ///Handle to Crypto
    hdl_t phCryptoHdl;

    //Pointer to Logger
    sLogger_d sLogger;
}sCL_d;


///Function pointer to Initialize Record Layer.
typedef int32_t (*fCLInit)(sCL_d* psCL, const void*);

///Function pointer for Record Layer Send
typedef int32_t (*fCLEncrypt)(const sCL_d* psCL, const sbBlob_d* PpsBlobPlainText,sbBlob_d* PpsBlobCipherText,uint16_t wLen);

///Function pointer for Record Layer Receive
typedef int32_t (*fCLDecrypt)(const sCL_d* psCL, const sbBlob_d* PpsBlobCipherText,sbBlob_d* PpsBlobPlainText,uint16_t wLen);

///Function pointer to close Record Layer
typedef void (*fCLClose)(sCL_d* psCL);

/**
 * \brief Structure to configure Crypto Layer.
 */
typedef struct sConfigCL_d
{
    ///Function pointer to Init Crypto layer
    fCLInit pfInit;

    ///Function pointer to Close Crypto layer
    fCLClose pfClose;

    ///Function pointer to Encrypt data via Crypto layer
    fCLEncrypt pfEncrypt;

    ///Function pointer to Decrypt data via Crypto layer
    fCLDecrypt pfDecrypt;

    ///Crypto Handle
    sCL_d sCL;
}sConfigCL_d;

#endif //__OCPCRYPTOLAYER_H__
/**
* @}
*/
