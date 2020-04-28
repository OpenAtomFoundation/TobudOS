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
// Function:      Firmware encryption using libTomCrypt
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "libtomcrypt.h"
#include <stdio.h>
#include <string.h>
#include "config.h"

#if defined(USE_ENCRYPTION) && (defined(ENCRYPTION_AES_LTC) || defined(ENCRYPTION_3DES_LTC))
#include <board.h>
#include <utility/trace.h>
#include <utility/assert.h>
#include <pmc/pmc.h>
#include <src/headers/tomcrypt.h>

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

#if defined(ENCRYPTION_ECB)
    symmetric_ECB sECB;
#endif
#if defined(ENCRYPTION_CBC)
    symmetric_CBC sCBC;
#endif
#if defined(ENCRYPTION_CTR)
    symmetric_CTR sCTR;
#endif

//------------------------------------------------------------------------------
// Inline functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Converts an ASCII string to an hexadecimal value
/// \param ASCII string to convert
/// \param Buffer to store converted value
/// \param Length of buffer
//------------------------------------------------------------------------------
static inline void ASCII2Hex(const char * ascii, unsigned char * binary, unsigned int length) 
{
    unsigned int i;

    for (i=0; i < length; i++) {
        if (ascii[i*2] >= 'A') {
            binary[i] = ascii[i*2] - 'A' + 10;
        }
        else {
            binary[i] = ascii[i*2] - '0';
        }
        binary[i] <<= 4;
        if (ascii[i*2+1] >= 'A') {
            binary[i] += ascii[i*2+1] - 'A' + 10;
        }
        else {
            binary[i] += ascii[i*2+1] - '0';
        }
    }
}

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the decryption process
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
void ltc_init(void) 
{
    int cipherID;
    unsigned char key[ENCRYPTION_KEY_LENGTH];

#if defined(ENCRYPTION_CTR) || defined(ENCRYPTION_CBC)
    unsigned char IV[ENCRYPTION_BLOCK_LENGTH];
#endif

    TRACE_DEBUG("LTC: Initializing ...\n\r");

    // Register cipher
    register_cipher(&CIPHER_DESC);
    cipherID = find_cipher(CIPHER_NAME);

    // Load key
    ASCII2Hex(ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

#if defined(ENCRYPTION_CTR) || defined(ENCRYPTION_CBC)
    // Load IV
    ASCII2Hex(ENCRYPTION_IV, IV, ENCRYPTION_BLOCK_LENGTH);
#endif

    // Start decryption mode
#if defined(ENCRYPTION_ECB)
    ecb_start(cipherID, key, ENCRYPTION_KEY_LENGTH, 0, &sECB);
#elif defined(ENCRYPTION_CBC)
    cbc_start(cipherID, IV, key, ENCRYPTION_KEY_LENGTH, 0, &sCBC);
#elif defined(ENCRYPTION_CTR)
    ctr_start(cipherID, IV, key, ENCRYPTION_KEY_LENGTH, 0, CTR_COUNTER_BIG_ENDIAN, &sCTR);
#endif

    TRACE_DEBUG("LTC: Initialization done.\n\r");
}
#endif // ONLY_ONE_ENCRYPTION

//------------------------------------------------------------------------------
/// Initializes the decryption process for AES, mode CBC
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
#if defined(ENCRYPTION_CBC)
void ltc_init_AES_CBC(void) 
{
    int cipherID;
    unsigned char key[ENCRYPTION_KEY_LENGTH];
    unsigned char IV[ENCRYPTION_BLOCK_LENGTH];

    TRACE_DEBUG("LTC: Initializing CBC...\n\r");

    // Register cipher
    register_cipher(&rijndael_desc);
    cipherID = find_cipher("rijndael");

    // Load key
    ASCII2Hex(ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

    // Load IV
    ASCII2Hex(ENCRYPTION_IV, IV, ENCRYPTION_BLOCK_LENGTH);

    // Start decryption mode
    cbc_start(cipherID, IV, key, ENCRYPTION_KEY_LENGTH, 0, &sCBC);

    TRACE_DEBUG("LTC: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the decryption process for AES, mode CTR
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
void ltc_init_AES_CTR(void) 
{
    int cipherID;
    unsigned char key[ENCRYPTION_KEY_LENGTH];
    unsigned char IV[ENCRYPTION_BLOCK_LENGTH];

    TRACE_DEBUG("LTC: Initializing CTR...\n\r");

    // Register cipher
    register_cipher(&rijndael_desc);
    cipherID = find_cipher("rijndael");

    // Load key
    ASCII2Hex(ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

    // Load IV
    ASCII2Hex(ENCRYPTION_IV, IV, ENCRYPTION_BLOCK_LENGTH);

    // Start decryption mode
    ctr_start(cipherID, IV, key, ENCRYPTION_KEY_LENGTH, 0, CTR_COUNTER_BIG_ENDIAN, &sCTR);

    TRACE_DEBUG("LTC: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the decryption process for AES, mode ECB
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB)
void ltc_init_AES_ECB(void) 
{
    int cipherID;
    unsigned char key[ENCRYPTION_KEY_LENGTH];

    TRACE_DEBUG("LTC: Initializing ECB...\n\r");

    // Register cipher
    register_cipher(&rijndael_desc);
    cipherID = find_cipher("rijndael");

    // Load key
    ASCII2Hex(ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

    // Start decryption mode
    ecb_start(cipherID, key, ENCRYPTION_KEY_LENGTH, 0, &sECB);

    TRACE_DEBUG("LTC: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the decryption process for 3DES, mode CBC
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CBC)
void ltc_init_3DES_CBC(void) 
{
    int cipherID;
    unsigned char key[ENCRYPTION_KEY_LENGTH];
    unsigned char IV[ENCRYPTION_BLOCK_LENGTH];

    TRACE_DEBUG("LTC: Initializing CBC...\n\r");

    // Register cipher
    register_cipher(&des3_desc);
    cipherID = find_cipher("3des");

    // Load key
    ASCII2Hex(ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

    // Load IV
    ASCII2Hex(ENCRYPTION_IV, IV, ENCRYPTION_BLOCK_LENGTH);

    // Start decryption mode
    cbc_start(cipherID, IV, key, ENCRYPTION_KEY_LENGTH, 0, &sCBC);

    TRACE_DEBUG("LTC: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the decryption process for 3DES, mode CTR
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
void ltc_init_3DES_CTR(void) 
{
    int cipherID;
    unsigned char key[ENCRYPTION_KEY_LENGTH];
    unsigned char IV[ENCRYPTION_BLOCK_LENGTH];

    TRACE_DEBUG("LTC: Initializing CTR...\n\r");

    // Register cipher
    register_cipher(&des3_desc);
    cipherID = find_cipher("3des");

    // Load key
    ASCII2Hex(ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

    // Load IV
    ASCII2Hex(ENCRYPTION_IV, IV, ENCRYPTION_BLOCK_LENGTH);

    // Start decryption mode
    ctr_start(cipherID, IV, key, ENCRYPTION_KEY_LENGTH, 0, CTR_COUNTER_BIG_ENDIAN, &sCTR);

    TRACE_DEBUG("LTC: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the decryption process for 3DES, mode ECB
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB)
void ltc_init_3DES_ECB(void) 
{
    int cipherID;
    unsigned char key[ENCRYPTION_KEY_LENGTH];

    TRACE_DEBUG("LTC: Initializing ECB...\n\r");

    // Register cipher
    register_cipher(&des3_desc);
    cipherID = find_cipher("3des");

    // Load key
    ASCII2Hex(ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

    // Start decryption mode
    ecb_start(cipherID, key, ENCRYPTION_KEY_LENGTH, 0, &sECB);

    TRACE_DEBUG("LTC: Initialization done.\n\r");
}
#endif
#endif // ONLY_ONE_ENCRYPTION


//------------------------------------------------------------------------------
/// Terminates the decryption process
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
void ltc_cleanup(void)
{
    TRACE_DEBUG("LTC: Cleaning up ...\n\r");

#if defined(ENCRYPTION_ECB)
    ecb_done(&sECB);
#elif defined(ENCRYPTION_CBC)
    cbc_done(&sCBC);
#elif defined(ENCRYPTION_CTR)
    ctr_done(&sCTR);
#endif

    TRACE_DEBUG("LTC: Cleanup done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Terminates the decryption process for mode CBC
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
#if defined(ENCRYPTION_CBC)
void ltc_cleanup_CBC(void)
{
    TRACE_DEBUG("LTC: Cleaning up CBC...\n\r");
    cbc_done(&sCBC);
    TRACE_DEBUG("LTC: Cleanup done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Terminates the decryption process for mode CTR
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
void ltc_cleanup_CTR(void)
{
    TRACE_DEBUG("LTC: Cleaning up CTR...\n\r");
    ctr_done(&sCTR);
    TRACE_DEBUG("LTC: Cleanup done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Terminates the decryption process for mode ECB
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB)
void ltc_cleanup_ECB(void)
{
    TRACE_DEBUG("LTC: Cleaning up ECB...\n\r");
    ecb_done(&sECB);
    TRACE_DEBUG("LTC: Cleanup done.\n\r");
}
#endif
#endif // ONLY_ONE_ENCRYPTION

//------------------------------------------------------------------------------
/// Decrypts a block of data
/// \param Data to decrypt
/// \param Buffer to store decrypted data
/// \param Length of data
/// \return 1 if successful, 0 otherwise.
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
int ltc_decrypt(const unsigned char * cipherText, unsigned char * plainText, unsigned int length)
{
#if defined(ENCRYPTION_ECB)
    if (ecb_decrypt(cipherText, plainText, length, &sECB) != CRYPT_OK) {
#elif defined(ENCRYPTION_CBC)
    if (cbc_decrypt(cipherText, plainText, length, &sCBC) != CRYPT_OK) {
#elif defined(ENCRYPTION_CTR)
    if (ctr_decrypt(cipherText, plainText, length, &sCTR) != CRYPT_OK) {
#endif
        return 0;
    }
    else {
        return 1;
    }
}
#endif

//------------------------------------------------------------------------------
/// Decrypts a block of data in CBC mode
/// \param Data to decrypt
/// \param Buffer to store decrypted data
/// \param Length of data
/// \return 1 if successful, 0 otherwise.
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
int ltc_decrypt_CBC(const unsigned char * cipherText, unsigned char * plainText, unsigned int length)
{
    if (cbc_decrypt(cipherText, plainText, length, &sCBC) != CRYPT_OK) {
        return 0;
    }
    else {
        return 1;
    }
}

//------------------------------------------------------------------------------
/// Decrypts a block of data in CTR mode
/// \param Data to decrypt
/// \param Buffer to store decrypted data
/// \param Length of data
/// \return 1 if successful, 0 otherwise.
//------------------------------------------------------------------------------
int ltc_decrypt_CTR(const unsigned char * cipherText, unsigned char * plainText, unsigned int length)
{
    if (ctr_decrypt(cipherText, plainText, length, &sCTR) != CRYPT_OK) {
        return 0;
    }
    else {
        return 1;
    }
}

//------------------------------------------------------------------------------
/// Decrypts a block of data in ECB mode
/// \param Data to decrypt
/// \param Buffer to store decrypted data
/// \param Length of data
/// \return 1 if successful, 0 otherwise.
//------------------------------------------------------------------------------
int ltc_decrypt_ECB(const unsigned char * cipherText, unsigned char * plainText, unsigned int length)
{
    if (ecb_decrypt(cipherText, plainText, length, &sECB) != CRYPT_OK) {
        return 0;
    }
    else {
        return 1;
    }
}
#endif // ONLY_ONE_ENCRYPTION

#endif // defined(USE_ENCRYPTION) && (defined(ENCRYPTION_AES_LTC) || defined(ENCRYPTION_3DES_LTC))

