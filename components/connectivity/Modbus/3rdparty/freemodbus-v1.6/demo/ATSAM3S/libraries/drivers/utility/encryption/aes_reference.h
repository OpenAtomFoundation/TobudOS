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
/// Firmware encryption using AES reference implementation
/// 
/// !Usage
/// 
/// -# aes_ref_init: Initialize AES hardware 
/// -# aes_ref_init_CBC: for the CBC mode
/// -# aes_ref_init_ECB: for the CTR mode
/// -# aes_ref_init_CTR: for the ECB mode
/// -# aes_ref_cleanup: Cleans up AES
/// -# aes_ref_decrypt: Decrypts a variable-length cipher text
/// -# aes_ref_decrypt_CBC: for CBC mode
/// -# aes_ref_decrypt_ECB: for ECB mode
/// -# aes_ref_decrypt_CTR: for CTR mode
//------------------------------------------------------------------------------

#ifndef BOOTLOADER_AES_REFERENCE_H
#define BOOTLOADER_AES_REFERENCE_H

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "config.h"


#if defined(USE_ENCRYPTION) && defined(ENCRYPTION_AES_REF)

//------------------------------------------------------------------------------
// Check configuration
//------------------------------------------------------------------------------

// Supported modes
#if !defined(ENCRYPTION_ECB) && \
    !defined(ENCRYPTION_CBC) && \
    !defined(ENCRYPTION_CTR)
  #error No other mode than ECB, CBC & CTR are supported.
#endif

// Supported key length
#if (ENCRYPTION_KEY_LENGTH != 16) && \
    (ENCRYPTION_KEY_LENGTH != 24) && \
    (ENCRYPTION_KEY_LENGTH != 32) 
  #error Only key lengths of 128, 192 or 256 bits are supported.
#endif

// Supported block length
#if (ENCRYPTION_BLOCK_LENGTH != 16)
  #error Only block length of 128 bits is supported.
#endif

//------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------
#define BC     (ENCRYPTION_BLOCK_LENGTH / 4)
#define SC     ((BC - 4) >> 1)
#define KC     (ENCRYPTION_KEY_LENGTH / 4)
#define t0f    0x000000FF & tf
#define t1f    0x0000FF00 & tf
#define t2f    0x00FF0000 & tf
#define t3f    0xFF000000 & tf

#if (KC >= BC)
  #define ROUNDS            (KC + 6)
#else
  #define ROUNDS            (BC + 6)
#endif

#ifdef ONLY_ONE_ENCRYPTION
#define ENCRYPTION_INIT     aes_ref_init
#define ENCRYPTION_CLEANUP  aes_ref_cleanup
#define ENCRYPTION_DECRYPT  aes_ref_decrypt
#endif

//------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------
extern void aes_ref_init(void);
extern void aes_ref_init_CBC(void);
extern void aes_ref_init_ECB(void);
extern void aes_ref_init_CTR(void);
extern void aes_ref_cleanup(void);
extern int aes_ref_decrypt(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length);
extern int aes_ref_decrypt_CBC(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length);
extern int aes_ref_decrypt_ECB(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length);
extern int aes_ref_decrypt_CTR(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length);

#endif // defined(USE_ENCRYPTION) && defined(ENCRYPTION_AES_REF)

#endif // BOOTLOADER_AES_REFERENCE_H


