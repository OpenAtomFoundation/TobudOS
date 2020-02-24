/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
/// \unit
///
/// !Purpose
/// 
/// Firmware encryption using libTomCrypt
/// 
/// !Usage
/// 
/// -# ltc_init: Initialize LibTomCrypt
/// -# ltc_init_AES_CBC
/// -# ltc_init_AES_CTR
/// -# ltc_init_AES_ECB
/// -# ltc_init_3DES_CBC
/// -# ltc_init_3DES_CTR
/// -# ltc_init_3DES_ECB
/// -# ltc_cleanup
/// -# ltc_cleanup_CBC
/// -# ltc_cleanup_CTR
/// -# ltc_cleanup_ECB
/// -# ltc_decrypt
/// -# ltc_decrypt_CBC
/// -# ltc_decrypt_CTR
/// -# ltc_decrypt_ECB
//------------------------------------------------------------------------------

#ifndef BOOTLOADER_LIBTOMCRYPT_H
#define BOOTLOADER_LIBTOMCRYPT_H

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "config.h"

#if defined(USE_ENCRYPTION) && (defined(ENCRYPTION_AES_LTC) || defined(ENCRYPTION_3DES_LTC))

//------------------------------------------------------------------------------
// Check configuration
//------------------------------------------------------------------------------

// Supported modes
#if !defined(ENCRYPTION_CTR) && \
    !defined(ENCRYPTION_CBC) && \
    !defined(ENCRYPTION_ECB)
  #error No other mode than ECB, CBC & CTR are supported.
#endif

// Supported key length
#if defined(ENCRYPTION_AES_LTC)
  #if (ENCRYPTION_KEY_LENGTH != 16) && \
      (ENCRYPTION_KEY_LENGTH != 24) && \
      (ENCRYPTION_KEY_LENGTH != 32) 
    #error Only a key length of 128, 192 or 256 bits are supported with AES.
  #endif
#elif defined(ENCRYPTION_3DES_LTC)
  #if (ENCRYPTION_KEY_LENGTH != 16) && \
      (ENCRYPTION_KEY_LENGTH != 24)
    #error Only a key length of 128 or 192 bits are supported with Triple-DES.
  #endif
#endif

// Supported block length
#if defined(ENCRYPTION_AES_LTC)
  #if (ENCRYPTION_BLOCK_LENGTH != 16)
    #error Only a block length of 128 bits is supported with AES.
  #endif
#elif defined(ENCRYPTION_3DES_LTC)
  #if (ENCRYPTION_BLOCK_LENGTH != 8)
    #error Only a block length of 64 bits is supported with Triple-DES.
  #endif
#endif

//------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------

// Functions
#ifdef ONLY_ONE_ENCRYPTION
#define ENCRYPTION_INIT     ltc_init
#define ENCRYPTION_CLEANUP  ltc_cleanup
#define ENCRYPTION_DECRYPT  ltc_decrypt

#if defined(ENCRYPTION_AES_LTC)
  #define CIPHER_NAME       "rijndael"
  #define CIPHER_DESC       rijndael_desc
#elif defined(ENCRYPTION_3DES_LTC)
  #define CIPHER_NAME       "3des"
  #define CIPHER_DESC       des3_desc
#endif
#endif

//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------

#ifdef ONLY_ONE_ENCRYPTION
extern void ltc_init(void);
#endif
extern void ltc_init_AES_CBC(void);
extern void ltc_init_AES_CTR(void);
extern void ltc_init_AES_ECB(void);
extern void ltc_init_3DES_CBC(void);
extern void ltc_init_3DES_CTR(void);
extern void ltc_init_3DES_ECB(void);

extern void ltc_cleanup(void);
extern void ltc_cleanup_CBC(void);
extern void ltc_cleanup_CTR(void);
extern void ltc_cleanup_ECB(void);

extern int ltc_decrypt(const unsigned char * cipherText, unsigned char * plainText, unsigned int length);
extern int ltc_decrypt_CBC(const unsigned char * cipherText, unsigned char * plainText, unsigned int length);
extern int ltc_decrypt_CTR(const unsigned char * cipherText, unsigned char * plainText, unsigned int length);
extern int ltc_decrypt_ECB(const unsigned char * cipherText, unsigned char * plainText, unsigned int length);

#endif // defined(USE_ENCRYPTION) && (defined(ENCRYPTION_AES_LTC) || defined(ENCRYPTION_3DES_LTC))
#endif // BOOTLOADER_AES_LIBTOMCRYPT_H


