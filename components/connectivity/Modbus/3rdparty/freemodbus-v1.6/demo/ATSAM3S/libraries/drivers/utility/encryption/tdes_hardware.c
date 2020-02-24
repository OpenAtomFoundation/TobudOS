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
// Function:      Firmware encryption using TDES hardware acceleration
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "tdes_hardware.h"

#if defined(USE_ENCRYPTION) && defined(ENCRYPTION_3DES_HARD)
#include <stdio.h>
#include <string.h>
#include <board.h>
#include <utility/trace.h>
#include <utility/assert.h>
#include <pmc/pmc.h>

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

#ifdef ENCRYPTION_CTR
  unsigned char CTR[ENCRYPTION_BLOCK_LENGTH];
#endif

//------------------------------------------------------------------------------
// Inline functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Converts an ASCII value to an hexadecimal one
/// \param ASCII string
/// \param Buffer to store integer value
/// \param Length of string
//------------------------------------------------------------------------------
static inline void ASCII2Hex(const unsigned char * ascii, unsigned char * binary, unsigned int length)
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
/// Initializes the TDES peripheral
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
void tdes_hard_init(void) 
{
    unsigned char key[ENCRYPTION_KEY_LENGTH];

    TRACE_DEBUG("TDES/HARD: Initializing ...\n\r");

    // Activate peripheral clock
#ifdef AT91C_ID_AES
    PMC_EnablePeripheral( AT91C_ID_AES );
#elif AT91C_ID_AESTDES
    PMC_EnablePeripheral( AT91C_ID_AESTDES );
#elif AT91C_ID_TDES
    PMC_EnablePeripheral( AT91C_ID_TDES );
#else
#error AES/TDES undefined
#endif
  
    // Load mode
#if (ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | TDES_MODE
                             | AT91C_TDES_TDESMOD     | TDES_CIPHER;
#else
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | TDES_MODE | AT91C_TDES_KEYMOD
                             | AT91C_TDES_TDESMOD     | TDES_CIPHER;
#endif
  
    // Convert and load key
    ASCII2Hex((unsigned char*)ENCRYPTION_KEY, key, ENCRYPTION_KEY_LENGTH);

    AT91C_BASE_TDES->TDES_KEY1WxR[0] = ((int *) key)[0];
    AT91C_BASE_TDES->TDES_KEY1WxR[1] = ((int *) key)[1];
    AT91C_BASE_TDES->TDES_KEY2WxR[0] = ((int *) key)[2];
    AT91C_BASE_TDES->TDES_KEY2WxR[1] = ((int *) key)[3];

#if (ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_KEY3WxR[0] = ((int *) key)[4];
    AT91C_BASE_TDES->TDES_KEY3WxR[1] = ((int *) key)[5];
#endif

#if defined(ENCRYPTION_CBC)
    unsigned char IV[8];
    ASCII2Hex((unsigned char*)ENCRYPTION_IV, IV, ENCRYPTION_BLOCK_LENGTH);

    AT91C_BASE_TDES->TDES_IVxR[0] = ((int *) IV)[0];
    AT91C_BASE_TDES->TDES_IVxR[1] = ((int *) IV)[1];

#elif defined(ENCRYPTION_CTR)
    // Convert IV
    ASCII2Hex((unsigned char*)ENCRYPTION_IV, CTR, ENCRYPTION_BLOCK_LENGTH);
#endif

    TRACE_DEBUG("TDES/HARD: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the DES peripheral for CBC mode
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
void tdes_hard_init_CBC(void)
{
    unsigned char key[TDES_ENCRYPTION_KEY_LENGTH];

    TRACE_DEBUG("tdes_hard_init_CBC\n\r");

    // Activate peripheral clock
#ifdef AT91C_ID_AES
    PMC_EnablePeripheral( AT91C_ID_AES );
#elif AT91C_ID_AESTDES
    PMC_EnablePeripheral( AT91C_ID_AESTDES );
#elif AT91C_ID_TDES
    PMC_EnablePeripheral( AT91C_ID_TDES );
#else
#error AES undefined
#endif
  
    // Load mode
#if (TDES_ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | AT91C_TDES_OPMOD_CBC
                             | AT91C_TDES_TDESMOD     | 0;
#else
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | AT91C_TDES_OPMOD_CBC | AT91C_TDES_KEYMOD
                             | AT91C_TDES_TDESMOD     | 0;
#endif
  
    // Convert and load key
    ASCII2Hex((unsigned char*)TDES_ENCRYPTION_KEY, key, TDES_ENCRYPTION_KEY_LENGTH);

    AT91C_BASE_TDES->TDES_KEY1WxR[0] = ((int *) key)[0];
    AT91C_BASE_TDES->TDES_KEY1WxR[1] = ((int *) key)[1];
    AT91C_BASE_TDES->TDES_KEY2WxR[0] = ((int *) key)[2];
    AT91C_BASE_TDES->TDES_KEY2WxR[1] = ((int *) key)[3];

#if (TDES_ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_KEY3WxR[0] = ((int *) key)[4];
    AT91C_BASE_TDES->TDES_KEY3WxR[1] = ((int *) key)[5];
#endif

    unsigned char IV[8];
    ASCII2Hex((unsigned char*)ENCRYPTION_IV, IV, ENCRYPTION_BLOCK_LENGTH);

    AT91C_BASE_TDES->TDES_IVxR[0] = ((int *) IV)[0];
    AT91C_BASE_TDES->TDES_IVxR[1] = ((int *) IV)[1];
}
#endif

//------------------------------------------------------------------------------
/// Initializes the DES peripheral for CTR mode
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
void tdes_hard_init_CTR(void) 
{
    unsigned char key[TDES_ENCRYPTION_KEY_LENGTH];

    TRACE_DEBUG("tdes_hard_init_CTR\n\r");

    // Activate peripheral clock
#ifdef AT91C_ID_AES
    PMC_EnablePeripheral( AT91C_ID_AES );
#elif AT91C_ID_AESTDES
    PMC_EnablePeripheral( AT91C_ID_AESTDES );
#elif AT91C_ID_TDES
    PMC_EnablePeripheral( AT91C_ID_TDES );
#else
#error AES undefined
#endif
  
    // Load mode
#if (TDES_ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | AT91C_TDES_OPMOD_ECB
                             | AT91C_TDES_TDESMOD     | AT91C_TDES_CIPHER;
#else
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | AT91C_TDES_OPMOD_ECB | AT91C_TDES_KEYMOD
                             | AT91C_TDES_TDESMOD     | AT91C_TDES_CIPHER;
#endif
  
    // Convert and load key
    ASCII2Hex((unsigned char*)TDES_ENCRYPTION_KEY, key, TDES_ENCRYPTION_KEY_LENGTH);

    AT91C_BASE_TDES->TDES_KEY1WxR[0] = ((int *) key)[0];
    AT91C_BASE_TDES->TDES_KEY1WxR[1] = ((int *) key)[1];
    AT91C_BASE_TDES->TDES_KEY2WxR[0] = ((int *) key)[2];
    AT91C_BASE_TDES->TDES_KEY2WxR[1] = ((int *) key)[3];

#if (TDES_ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_KEY3WxR[0] = ((int *) key)[4];
    AT91C_BASE_TDES->TDES_KEY3WxR[1] = ((int *) key)[5];
#endif

    // Convert IV
    ASCII2Hex((unsigned char*)ENCRYPTION_IV, CTR, ENCRYPTION_BLOCK_LENGTH);
}
#endif

//------------------------------------------------------------------------------
/// Initializes the DES peripheral for ECB mode
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
void tdes_hard_init_ECB(void)
{
    unsigned char key[TDES_ENCRYPTION_KEY_LENGTH];

    TRACE_DEBUG("tdes_hard_init_ECB\n\r");

    // Activate peripheral clock
#ifdef AT91C_ID_AES
    PMC_EnablePeripheral( AT91C_ID_AES );
#elif AT91C_ID_AESTDES
    PMC_EnablePeripheral( AT91C_ID_AESTDES );
#elif AT91C_ID_TDES
    PMC_EnablePeripheral( AT91C_ID_TDES );
#else
#error AES undefined
#endif
  
    // Load mode
#if (TDES_ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | AT91C_TDES_OPMOD_ECB
                             | AT91C_TDES_TDESMOD     | 0;
#else
    AT91C_BASE_TDES->TDES_MR = AT91C_TDES_SMOD_MANUAL | AT91C_TDES_OPMOD_ECB | AT91C_TDES_KEYMOD
                             | AT91C_TDES_TDESMOD     | 0;
#endif
  
    // Convert and load key
    ASCII2Hex((unsigned char*)TDES_ENCRYPTION_KEY, key, TDES_ENCRYPTION_KEY_LENGTH);

    AT91C_BASE_TDES->TDES_KEY1WxR[0] = ((int *) key)[0];
    AT91C_BASE_TDES->TDES_KEY1WxR[1] = ((int *) key)[1];
    AT91C_BASE_TDES->TDES_KEY2WxR[0] = ((int *) key)[2];
    AT91C_BASE_TDES->TDES_KEY2WxR[1] = ((int *) key)[3];

#if (TDES_ENCRYPTION_KEY_LENGTH == 24)
    AT91C_BASE_TDES->TDES_KEY3WxR[0] = ((int *) key)[4];
    AT91C_BASE_TDES->TDES_KEY3WxR[1] = ((int *) key)[5];
#endif
}
#endif

//------------------------------------------------------------------------------
/// Cleans up the DES peripheral
//------------------------------------------------------------------------------
void tdes_hard_cleanup(void) 
{
    TRACE_DEBUG("TDES/HARD: Cleaning up ...\n\r");
    AT91C_BASE_TDES->TDES_MR = 0;
#ifdef AT91C_ID_AES
    PMC_DisablePeripheral( AT91C_ID_AES );
#elif AT91C_ID_AESTDES
    PMC_DisablePeripheral( AT91C_ID_AESTDES );
#elif AT91C_ID_TDES
    PMC_DisablePeripheral( AT91C_ID_TDES );
#else
#error AES undefined
#endif
    TRACE_DEBUG("TDES/HARD: Cleanup done.\n\r");
}

//------------------------------------------------------------------------------
/// Decrypts a variable-length cipher text
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
int tdes_hard_decrypt(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length)
{
    unsigned int l;

    TRACE_DEBUG("tdes_hard_decrypt\n\r");

#if defined(ENCRYPTION_ECB) || defined(ENCRYPTION_CBC)
    for (l=0; l < length;) {

        // Load counter and encrypt it
        AT91C_BASE_TDES->TDES_IDATAxR[0] = ((int *) &cipherText[l])[0];
        AT91C_BASE_TDES->TDES_IDATAxR[1] = ((int *) &cipherText[l])[1];

        // Start processing
        AT91C_BASE_TDES->TDES_CR = AT91C_TDES_START;

        while (!((AT91C_BASE_TDES->TDES_ISR) & AT91C_TDES_DATRDY));

        ((int *) &plainText[l])[0] = AT91C_BASE_TDES->TDES_ODATAxR[0];
        ((int *) &plainText[l])[1] = AT91C_BASE_TDES->TDES_ODATAxR[1];

        l += ENCRYPTION_BLOCK_LENGTH;
    }
  
#elif defined(ENCRYPTION_CTR)
    unsigned int bytes;
    unsigned char block[ENCRYPTION_BLOCK_LENGTH];
    unsigned int e;
    unsigned int i;
    int k;
  
    // Decrypt
    for (e=0; e < length;) {
   
        // Load counter and encrypt it
        AT91C_BASE_TDES->TDES_IDATAxR[0] = ((int *) CTR)[0];
        AT91C_BASE_TDES->TDES_IDATAxR[1] = ((int *) CTR)[1];

        AT91C_BASE_TDES->TDES_CR = AT91C_TDES_START;
        while (!(AT91C_BASE_TDES->TDES_ISR & AT91C_TDES_DATRDY));

        ((int *) block)[0] = AT91C_BASE_TDES->TDES_ODATAxR[0];
        ((int *) block)[1] = AT91C_BASE_TDES->TDES_ODATAxR[1];

        // XOR current plain text block with encrypted counter
        if ((length-e) < ENCRYPTION_BLOCK_LENGTH) {
            bytes = length - e;
        }
        else {
            bytes = ENCRYPTION_BLOCK_LENGTH;
        }
        
        for (i=0; i < bytes; i++) {
            plainText[e+i] = cipherText[e+i] ^ ((char *) block)[i];
        }

        // Increment counter (big-endian) and number of encrypted bytes
        for (k=ENCRYPTION_BLOCK_LENGTH-1; k >= 0; k--) {
            if (++((char *) CTR)[k] != 0) {
                break;
            }
        }
        e += bytes;
    }
#endif // ENCRYPTION_CTR
  
    return 1;
}
#endif

//------------------------------------------------------------------------------
/// Decrypts a variable-length cipher text for CBC mode
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
int tdes_hard_decrypt_CBC(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length)
{
    unsigned int l;

    TRACE_DEBUG("tdes_hard_decrypt_CBC\n\r");

    for (l=0; l < length;) {
        // Load counter and encrypt it
        AT91C_BASE_TDES->TDES_IDATAxR[0] = ((int *) &cipherText[l])[0];
        AT91C_BASE_TDES->TDES_IDATAxR[1] = ((int *) &cipherText[l])[1];

        // Start processing
        AT91C_BASE_TDES->TDES_CR = AT91C_TDES_START;

        while (!((AT91C_BASE_TDES->TDES_ISR) & AT91C_TDES_DATRDY));

        ((int *) &plainText[l])[0] = AT91C_BASE_TDES->TDES_ODATAxR[0];
        ((int *) &plainText[l])[1] = AT91C_BASE_TDES->TDES_ODATAxR[1];

        l += ENCRYPTION_BLOCK_LENGTH;
    }
    return 1;
}
#endif

//------------------------------------------------------------------------------
/// Decrypts a variable-length cipher text for CTR mode
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
int tdes_hard_decrypt_CTR(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length)
{
    unsigned int bytes;
    unsigned char block[ENCRYPTION_BLOCK_LENGTH];
    unsigned int e;
    unsigned int i;
    int k;

    TRACE_DEBUG("tdes_hard_decrypt_CTR\n\r");
  
    // Decrypt
    for (e=0; e < length;) {
   
        // Load counter and encrypt it
        AT91C_BASE_TDES->TDES_IDATAxR[0] = ((int *) CTR)[0];
        AT91C_BASE_TDES->TDES_IDATAxR[1] = ((int *) CTR)[1];

        AT91C_BASE_TDES->TDES_CR = AT91C_TDES_START;
        while (!(AT91C_BASE_TDES->TDES_ISR & AT91C_TDES_DATRDY));

        ((int *) block)[0] = AT91C_BASE_TDES->TDES_ODATAxR[0];
        ((int *) block)[1] = AT91C_BASE_TDES->TDES_ODATAxR[1];

        // XOR current plain text block with encrypted counter
        if ((length-e) < ENCRYPTION_BLOCK_LENGTH) {
            bytes = length - e;
        }
        else {
            bytes = ENCRYPTION_BLOCK_LENGTH;
        }
        
        for (i=0; i < bytes; i++) {
            plainText[e+i] = cipherText[e+i] ^ ((char *) block)[i];
        }

        // Increment counter (big-endian) and number of encrypted bytes
        for (k=ENCRYPTION_BLOCK_LENGTH-1; k >= 0; k--) {
            if (++((char *) CTR)[k] != 0) {
                break;
            }
        }
        e += bytes;
    }
  
    return 1;
}
#endif

//------------------------------------------------------------------------------
/// Decrypts a variable-length cipher text for ECB mode
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
int tdes_hard_decrypt_ECB(const unsigned char * cipherText,
                     unsigned char * plainText,
                     unsigned int length)
{
    unsigned int l;

    TRACE_DEBUG("tdes_hard_decrypt_ECB\n\r");

    for (l=0; l < length;) {

        // Load counter and encrypt it
        AT91C_BASE_TDES->TDES_IDATAxR[0] = ((int *) &cipherText[l])[0];
        AT91C_BASE_TDES->TDES_IDATAxR[1] = ((int *) &cipherText[l])[1];

        // Start processing
        AT91C_BASE_TDES->TDES_CR = AT91C_TDES_START;

        while (!((AT91C_BASE_TDES->TDES_ISR) & AT91C_TDES_DATRDY));

        ((int *) &plainText[l])[0] = AT91C_BASE_TDES->TDES_ODATAxR[0];
        ((int *) &plainText[l])[1] = AT91C_BASE_TDES->TDES_ODATAxR[1];

        l += ENCRYPTION_BLOCK_LENGTH;
    }
    return 1;
}
#endif

#endif // defined(USE_ENCRYPTION) && defined(ENCRYPTION_3DES_HARD)



