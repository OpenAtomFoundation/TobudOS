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
 * \file HardwareCrypto.h
 *
 * \brief This file contains structures and prototypes of hardware crypto layer.
 *
 * \addtogroup grMutualAuth
 * @{
 *
 */


#ifndef __HWCRYPTO_H__
#define __HWCRYPTO_H__

#include "optiga/dtls/OcpCryptoLayer.h"
#include "optiga/common/MemoryMgmt.h"

/**
 * \brief  Structure for Hardware Crypto.
 */
typedef struct sHardwareCrypto_d
{
    /// Session OID
    uint16_t wSessionKeyOID;
}sHardwareCrypto_d;


/**
 * \brief Initialises the Hardware Crypto Layer.
 */
int32_t HWCL_Init(sCL_d* PpsCL, const void* PpParam);

/**
 * \brief Encrypts the input plain text using Security chip.
 */
int32_t HWCL_Encrypt(const sCL_d* PpsCL, const sbBlob_d* PpsBlobPlainText,sbBlob_d* PpsBlobCipherText,uint16_t PwLen);

/**
 * \brief Decrypts the input cipher text using Security chip.
 */
int32_t HWCL_Decrypt(const sCL_d* PpsCL,const sbBlob_d* PpsBlobCipherText,sbBlob_d* PpsBlobPlainText,uint16_t PwLen);

/**
 * \brief Closes the Crypto layer.
 */
void HWCL_Close(sCL_d* PpsCL);

#endif //__HWCRYPTO_H__
/**
* @}
*/
