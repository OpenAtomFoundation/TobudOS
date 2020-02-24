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
/// Firmware encryption
/// 
/// !Usage
///
/// This file must include the header of every encryption module implemented for
/// the bootloader. It is also responsible for defining mock functions if no
/// encryption module is selected, as well as checking that parameters are
/// correctly set.
//
/// An encryption module must define the following functions:
/// -# void encryption_init(void);
/// -# void encryption_cleanup(void);
/// -# int encryption_decrypt(const unsigned char *, unsigned char *, unsigned int)
//------------------------------------------------------------------------------

#ifndef BOOTLOADER_ENCRYPTION_H
#define BOOTLOADER_ENCRYPTION_H

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "config.h"


//------------------------------------------------------------------------------
// Check configuration
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
#if !defined(ENCRYPTION_INIT) && !defined(USE_ENCRYPTION)
  #define ENCRYPTION_INIT();
  #define ENCRYPTION_CLEANUP();
  #define ENCRYPTION_DECRYPT(...) 1

#elif !defined(ENCRYPTION_INIT) && defined(USE_ENCRYPTION)
  #error USE_ENCRYPTION defined but no encryption method selected.

#elif !defined(ENCRYPTION_AES_LTC) && \
      !defined(ENCRYPTION_AES_REF) && \
      !defined(ENCRYPTION_AES_HARD) && \
      !defined(ENCRYPTION_3DES_LTC) && \
      !defined(ENCRYPTION_3DES_HARD)
  #error No algorithm selected.

#elif !defined(ENCRYPTION_ECB) && \
      !defined(ENCRYPTION_CBC) && \
      !defined(ENCRYPTION_CTR)
  #error No encryption mode selected.

#elif !defined(ENCRYPTION_KEY)
  #error No key defined.

#elif !defined(ENCRYPTION_KEY_LENGTH)
  #error No key length selected.

#elif !defined(ENCRYPTION_IV)
  #error No initialization vector defined.

#elif !defined(ENCRYPTION_BLOCK_LENGTH)
  #error No block length selected.

#endif
#endif // ONLY_ONE_ENCRYPTION
#endif // BOOTLOADER_ENCRYPTION_H

