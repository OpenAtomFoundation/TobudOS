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
/// Firmware encryption using AES hardware acceleration
/// This code is based on the AES reference implementation published by Paulo
/// Barreto and Vincent Rijmen.
/// 
/// !Usage
/// 
/// -# aes_hard_init: Initialize AES hardware 
/// -# aes_hard_init_CBC: for the CBC mode
/// -# aes_hard_init_CTR: for the CTR mode
/// -# aes_hard_init_ECB: for the ECB mode
/// -# aes_hard_cleanup: Cleans up AES
/// -# aes_hard_decrypt: Decrypts a variable-length cipher text
//------------------------------------------------------------------------------

#ifndef BOOTLOADER_AES_HARDWARE_H
#define BOOTLOADER_AES_HARDWARE_H

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "config.h"

#if defined(USE_ENCRYPTION) && defined(ENCRYPTION_AES_HARD)

//------------------------------------------------------------------------------
// Check configuration
//------------------------------------------------------------------------------
#if (ENCRYPTION_KEY_LENGTH != 16)
  #error Hardware AES acceleration only supports 128 bits keys.
#endif

#if (ENCRYPTION_BLOCK_LENGTH !=  16)
  #error Hardware AES acceleration only supports 128 bits blocks.
#endif

#if !defined(ENCRYPTION_ECB) && \
    !defined(ENCRYPTION_CBC) && \
    !defined(ENCRYPTION_CTR)
  #error Only ECB, CBC & CTR modes are supported.
#endif

//------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
#define ENCRYPTION_INIT     aes_hard_init
#define ENCRYPTION_CLEANUP  aes_hard_cleanup
#define ENCRYPTION_DECRYPT  aes_hard_decrypt
#endif

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
extern void aes_hard_init(void);
#ifndef ONLY_ONE_ENCRYPTION
extern void aes_hard_init_CBC(void);
extern void aes_hard_init_CTR(void);
extern void aes_hard_init_ECB(void);
#endif
extern void aes_hard_cleanup(void);
extern int aes_hard_decrypt(const unsigned char *, unsigned char *, unsigned int);

#endif // defined(USE_ENCRYPTION) && defined(ENCRYPTION_AES_HARD)
#endif // BOOTLOADER_AES_HARDWARE_H


