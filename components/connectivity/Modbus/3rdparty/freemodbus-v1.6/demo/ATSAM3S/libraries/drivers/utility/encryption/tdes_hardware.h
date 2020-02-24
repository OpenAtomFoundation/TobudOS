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
/// Firmware encryption using TDES hardware acceleration
/// 
/// !Usage
/// 
/// -# tdes_hard_init: Initialize TDES hardware 
/// -# tdes_hard_init_CBC: for the CBC mode
/// -# tdes_hard_init_ECB: for the CTR mode
/// -# tdes_hard_init_CTR: for the ECB mode
/// -# tdes_hard_cleanup: Cleans up DES
/// -# tdes_hard_decrypt: Decrypts a variable-length cipher text
/// -# tdes_hard_decrypt_CBC: for CBC mode
/// -# tdes_hard_decrypt_ECB: for ECB mode
/// -# tdes_hard_decrypt_CTR: for CTR mode
//------------------------------------------------------------------------------
#ifndef BOOTLOADER_TDES_HARDWARE_H
#define BOOTLOADER_TDES_HARDWARE_H

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "config.h"

#if defined(USE_ENCRYPTION) && defined(ENCRYPTION_3DES_HARD)

//------------------------------------------------------------------------------
// Check configuration
//------------------------------------------------------------------------------

#ifdef ONLY_ONE_ENCRYPTION
#if (ENCRYPTION_KEY_LENGTH != 16) && (ENCRYPTION_KEY_LENGTH != 24)
  #error Triple-DES hardware acceleration only supports 128 and 192 bits keys.
#endif

#if (ENCRYPTION_BLOCK_LENGTH != 8)
  #error Triple-DES hardware acceleration only supports 64 bits blocks.
#endif

#if !defined(ENCRYPTION_ECB) && \
    !defined(ENCRYPTION_CBC) && \
    !defined(ENCRYPTION_CTR)
  #error Only ECB, CBC & CTR mode are supported.
#endif

//------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------
#define ENCRYPTION_INIT     tdes_hard_init
#define ENCRYPTION_CLEANUP  tdes_hard_cleanup
#define ENCRYPTION_DECRYPT  tdes_hard_decrypt

#if defined(ENCRYPTION_ECB)
  #define TDES_MODE         AT91C_TDES_OPMOD_ECB
  #define TDES_CIPHER       0
#elif defined(ENCRYPTION_CBC)
  #define TDES_MODE         AT91C_TDES_OPMOD_CBC
  #define TDES_CIPHER       0
#elif defined(ENCRYPTION_CTR)
  #define TDES_MODE         AT91C_TDES_OPMOD_ECB
  #define TDES_CIPHER       AT91C_TDES_CIPHER
#endif

#endif // ONLY_ONE_ENCRYPTION

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
extern void tdes_hard_init(void);
#endif
extern void tdes_hard_init_CBC(void);
extern void tdes_hard_init_CTR(void);
extern void tdes_hard_init_ECB(void);
extern void tdes_hard_cleanup(void);
extern int tdes_hard_decrypt(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length);
extern int tdes_hard_decrypt_CBC(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length);
extern int tdes_hard_decrypt_CTR(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length);
extern int tdes_hard_decrypt_ECB(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length);


#endif // defined(USE_ENCRYPTION) && defined(ENCRYPTION_3DES_HARD)
#endif // BOOTLOADER_DES_HARDWARE_H

