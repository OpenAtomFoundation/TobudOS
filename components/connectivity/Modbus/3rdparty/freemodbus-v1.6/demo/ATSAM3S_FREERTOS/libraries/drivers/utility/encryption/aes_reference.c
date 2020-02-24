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
// Firmware encryption using AES reference implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "aes_reference.h"

#if defined(USE_ENCRYPTION) && defined(ENCRYPTION_AES_REF)
#include <stdio.h>
#include <string.h>
#include <board.h>
#include <utility/trace.h>
#include <utility/assert.h>
#include <pmc/pmc.h>

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
#define word8 static unsigned char
#define word32 static unsigned int

#include "boxes-ref.dat"

static unsigned char shifts[3][2][4] = {
    {{0,1,2,3}, {0,3,2,1}},
    {{0,1,2,3}, {0,5,4,3}},
    {{0,7,5,5}, {0,1,3,4}}
};

static unsigned char key[KC][4];
static unsigned char expandedKey[ROUNDS+1][BC][4];
static unsigned int T0[256], T1[256], T2[256], T3[256], TF[256];

#if defined(ENCRYPTION_CBC) || defined(ENCRYPTION_CTR)
static unsigned char IV[BC][4];
#endif

//------------------------------------------------------------------------------
// Inline functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Multiply two elements of GF(2^m) needed for MixColumn and InvMixColumn
/// \param a first element
/// \param b second element
/// \return  result of operation
//------------------------------------------------------------------------------
static unsigned char mul(unsigned char a, unsigned char b)
{
    if (a && b) {
        return Alogtable[(Logtable[a] + Logtable[b])%255];
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
/// Returns the minimum between two numbers
/// \param First number
/// \param Second number
/// \return Minimum between the two operands
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
static unsigned int min(unsigned int number1, unsigned int number2)
{
    if (number1 > number2) {
        return number2;
    }
    else {
        return number1;
    }
}
#endif

//------------------------------------------------------------------------------
/// XOR text and round key together
/// \param Plain text
/// \param Round key
//------------------------------------------------------------------------------
static inline void addRoundKey(unsigned char a[BC][4], const unsigned char rk[BC][4])
{
    unsigned int i;

    for (i=0; i < BC; i++) {
        ((int *) a)[i] ^= ((int *) rk)[i];
    }
}

//------------------------------------------------------------------------------
/// Performs the AES key schedule
/// \param Key to use
/// \param Buffer to store expanded key schedule
//------------------------------------------------------------------------------
static inline void keySchedule(unsigned char k[KC][4], unsigned char W[ROUNDS+1][BC][4])
{
    int t;
    int rconpointer = 0;
    unsigned int j;
    unsigned char tk[KC][4];

    for(j=0; j < KC; j++) {
        ((int *) tk)[j] = ((int *) k)[j];
    }
  
    t = 0;
    /* copy values into round key array */
    for(j=0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++) {
        ((int *) W[t / BC])[t%BC] = ((int *) tk)[j];
    }
  
    while (t < (ROUNDS+1)*BC) { 

        tk[0][0] ^= S[tk[KC-1][1]] ^ rcon[rconpointer++];
        tk[0][1] ^= S[tk[KC-1][2]];
        tk[0][2] ^= S[tk[KC-1][3]];
        tk[0][3] ^= S[tk[KC-1][0]];

        if (KC != 8) {
            for(j=1; j < KC; j++) {
                ((int *) tk)[j] ^= ((int *) tk)[j-1];
            }
        }
        else {
            for(j=1; j < KC/2; j++) {
                ((int *) tk)[j] ^= ((int *) tk)[j-1];
            }
            tk[KC/2][0] ^= S[tk[KC/2 - 1][0]];
            tk[KC/2][1] ^= S[tk[KC/2 - 1][1]];
            tk[KC/2][2] ^= S[tk[KC/2 - 1][2]];
            tk[KC/2][3] ^= S[tk[KC/2 - 1][3]];

            for(j=KC/2+1; j < KC; j++) {
                ((int *) tk)[j] ^= ((int *) tk)[j-1];
            }
        }

        // copy values into round key array
        for(j=0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++) {
            ((int *) W[t/BC])[t%BC] = ((int *) tk)[j];
        }
    }
}

//------------------------------------------------------------------------------
/// Performs the AES inverse key schedule
/// \param Key to use
/// \param Buffer to store expanded key schedule
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB) || defined(ENCRYPTION_CBC)
static inline void invKeySchedule(unsigned char k[KC][4], 
                                  unsigned char W[ROUNDS+1][BC][4]) 
{
    unsigned int r;
    unsigned int j;
    unsigned char tmp[4];
  
    // Expand key normally
    keySchedule(k, W);
  
    // Apply invMixColumns to all rounds except first and last one
    for (r=1; r < ROUNDS; r++) {
        for (j=0; j < BC; j++) {
            tmp[0] = mul(0x0E, W[r][j][0]) ^ mul(0x0B, W[r][j][1]) ^
                     mul(0x0D, W[r][j][2]) ^ mul(0x09, W[r][j][3]);
            tmp[1] = mul(0x0E, W[r][j][1]) ^ mul(0x0B, W[r][j][2]) ^
                     mul(0x0D, W[r][j][3]) ^ mul(0x09, W[r][j][0]);
            tmp[2] = mul(0x0E, W[r][j][2]) ^ mul(0x0B, W[r][j][3]) ^
                     mul(0x0D, W[r][j][0]) ^ mul(0x09, W[r][j][1]);
            tmp[3] = mul(0x0E, W[r][j][3]) ^ mul(0x0B, W[r][j][0]) ^
                     mul(0x0D, W[r][j][1]) ^ mul(0x09, W[r][j][2]);
            W[r][j][0] = tmp[0];
            W[r][j][1] = tmp[1];
            W[r][j][2] = tmp[2];
            W[r][j][3] = tmp[3];
        }
    }
}
#endif

//------------------------------------------------------------------------------
/// Perform the RotBytes operation needed by the AES cipher
/// \param input to rotate
/// \return Rotated word
//------------------------------------------------------------------------------
static inline unsigned int rotBytes(unsigned int input) 
{
    return ((input << 8) | (input >> 24));
}

//------------------------------------------------------------------------------
/// Generates the lookup tables needed for encryption
/// \param Pointer to t0
/// \param Pointer to t1
/// \param Pointer to t2
/// \param Pointer to t3
/// \param Pointer to tf
/// \param Box
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
static inline void generateEncryptionLUTs(unsigned int * t0,
                                          unsigned int * t1,
                                          unsigned int * t2,
                                          unsigned int * t3,
                                          unsigned int * tf,
                                          unsigned char box[256])
{
    unsigned int a;

    for (a=0; a <= 255; a++) {
        // Calc t0
        t0[a] = (mul(2, box[a])) |
                (box[a] << 8) |
                (box[a] << 16) |
                (mul(3, box[a]) << 24);
        
        // Calc t1, t2, t3
        t1[a] = rotBytes(t0[a]);
        t2[a] = rotBytes(t1[a]);
        t3[a] = rotBytes(t2[a]);
        
        // Calc tf
        tf[a] = box[a] | (box[a] << 8) | (box[a] << 16) | (box[a] << 24);
    }
}
#endif

//------------------------------------------------------------------------------
/// Generates the lookup tables needed for decryption
/// \param Pointer to t0
/// \param Pointer to t1
/// \param Pointer to t2
/// \param Pointer to t3
/// \param Pointer to tf
/// \param Box
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB) || defined(ENCRYPTION_CBC)
static inline void generateDecryptionLUTs(unsigned int * t0,
                                          unsigned int * t1,
                                          unsigned int * t2,
                                          unsigned int * t3,
                                          unsigned int * tf,
                                          unsigned char box[256])
{
    unsigned int a;

    for (a=0; a <= 255; a++) {
   
        // Calc t0
        t0[a] = (mul(0x0E, box[a])) |
                (mul(0x09, box[a]) << 8) |
                (mul(0x0D, box[a]) << 16) |
                (mul(0x0B, box[a]) << 24);
        
        // Calc t1, t2, t3
        t1[a] = rotBytes(t0[a]);
        t2[a] = rotBytes(t1[a]);
        t3[a] = rotBytes(t2[a]);
        
        // Calc tf
        tf[a] = box[a] | (box[a] << 8) | (box[a] << 16) | (box[a] << 24);
    }
}
#endif

//------------------------------------------------------------------------------
/// Copies a block to a buffer
/// \param Block to copy
/// \param Buffer to store copy
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
static void copyBlock(unsigned char input[BC][4], unsigned char output[BC][4])
{
    unsigned int j; 

    for (j=0; j < BC; j++) {
        ((int *) output)[j] = ((int *) input)[j];
    }
}
#endif

//------------------------------------------------------------------------------
///  Encrypts a block of plain text using precalculated LUTs
/// \param Block of plain text to encrypt
/// \param Expanded key
/// \param Pointer to t0
/// \param Pointer to t1
/// \param Pointer to t2
/// \param Pointer to t3
/// \param Pointer to tf
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
static inline void encrypt(unsigned char a[BC][4],
                           const unsigned char rk[ROUNDS+1][BC][4],
                           unsigned int * t0,
                           unsigned int * t1,
                           unsigned int * t2,
                           unsigned int * t3,
                           unsigned int * tf) 
{
    unsigned char b[BC][4];
    unsigned int r;
    unsigned int j;
                            
    // First key addition
    addRoundKey(a, rk[0]);

    // ROUNDS-1 ordinary rounds
    for(r=1; r < ROUNDS; r++) {
        for (j=0; j < BC; j++) {
     
            ((int *) b)[j] = t0[a[j][0]] ^
                           t1[a[(j+shifts[SC][0][1])%BC][1]] ^
                           t2[a[(j+shifts[SC][0][2])%BC][2]] ^
                           t3[a[(j+shifts[SC][0][3])%BC][3]] ^
                           ((int *) rk[r])[j];
        }
        if ((++r) == ROUNDS) {          
            break;
        }
        for (j=0; j < BC; j++) {
            ((int *) a)[j] = t0[b[j][0]] ^
                           t1[b[(j+shifts[SC][0][1])%BC][1]] ^
                           t2[b[(j+shifts[SC][0][2])%BC][2]] ^
                           t3[b[(j+shifts[SC][0][3])%BC][3]] ^
                           ((int *) rk[r])[j];
        }
    }
  
    // Last round (no MixColumns)
    for (j=0; j < BC; j++) {
        ((int *) a)[j] = (t0f[b[j][0]]) ^
                         (t1f[b[(j+shifts[SC][0][1])%BC][1]]) ^
                         (t2f[b[(j+shifts[SC][0][2])%BC][2]]) ^
                         (t3f[b[(j+shifts[SC][0][3])%BC][3]]) ^
                         ((int *) rk[ROUNDS])[j];
    }
}
#endif

//------------------------------------------------------------------------------
/// Decrypts a block of plain text using precalculated LUTs
/// \param Block of cipher text to decrypt
/// \param Expanded key
/// \param Pointer to t0
/// \param Pointer to t1
/// \param Pointer to t2
/// \param Pointer to t3
/// \param Pointer to tf
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB) || defined(ENCRYPTION_CBC)
static inline void decrypt(unsigned char a[BC][4],
                           const unsigned char rk[ROUNDS+1][BC][4],
                           unsigned int * t0,
                           unsigned int * t1,
                           unsigned int * t2,
                           unsigned int * t3,
                           unsigned int * tf)
{
    unsigned char b[BC][4];
    unsigned int r;
    unsigned int j;
                            
    // First key addition
    addRoundKey(a, rk[ROUNDS]);

    // ROUNDS-1 ordinary rounds
    for(r=ROUNDS-1; r > 0; r--) {
        for (j=0; j < BC; j++) {
            ((int *) b)[j] = t0[a[j][0]] ^
                           t1[a[(j+shifts[SC][1][1])%BC][1]] ^
                           t2[a[(j+shifts[SC][1][2])%BC][2]] ^
                           t3[a[(j+shifts[SC][1][3])%BC][3]] ^
                           ((int *) rk[r])[j];
        }
        if ((--r) == 0) {
            break;
        }
        for (j=0; j < BC; j++) {
            ((int *) a)[j] = t0[b[j][0]] ^
                           t1[b[(j+shifts[SC][1][1])%BC][1]] ^
                           t2[b[(j+shifts[SC][1][2])%BC][2]] ^
                           t3[b[(j+shifts[SC][1][3])%BC][3]] ^
                           ((int *) rk[r])[j];
        }
    }
    // Last round (no MixColumns)
    for (j=0; j < BC; j++) {
        ((int *) a)[j] = (t0f[b[j][0]]) ^
                         (t1f[b[(j+shifts[SC][1][1])%BC][1]]) ^
                         (t2f[b[(j+shifts[SC][1][2])%BC][2]]) ^
                         (t3f[b[(j+shifts[SC][1][3])%BC][3]]) ^
                         ((int *) rk[0])[j];
    }
}
#endif

//------------------------------------------------------------------------------
/// Converts an ASCII hexadecimal representation to a raw binary one
/// \param ASCII value
/// \param Buffer to store binary value
/// \param Size of value
//------------------------------------------------------------------------------
static void ASCII2RawHex(const unsigned char * ascii, 
                         unsigned char * binary, 
                         unsigned int length) 
{
    unsigned char * ptr;
    unsigned int i;

    ptr = (unsigned char *) binary;
    for (i=0; i < length; i++, ptr++, ascii++) {
        if (*ascii >= 'A') {
            *ptr = *ascii - 'A' + 10;
        }
        else {
            *ptr = *ascii - '0';
        }
        *ptr <<= 4;
        ascii++;
        if (*ascii >= 'A') {
            *ptr += *ascii - 'A' + 10;
        }
        else {
            *ptr += *ascii - '0';
        }
    }
}

//------------------------------------------------------------------------------
/// Decrypts a cipher text using ECB mode
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text
/// \param Expanded key to use
/// \return 0 if successful, 0 otherwise
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB)
static unsigned int ecb_decrypt(const unsigned char * cipherText,
                         unsigned char * plainText,
                         unsigned int length,
                         unsigned char expandedKey[ROUNDS+1][BC][4])
{
    unsigned char block[BC][4];
    unsigned int i;
    unsigned int l;

    // Check input parameters
    if ((cipherText == NULL) || (plainText == NULL) || (expandedKey == NULL)) {
        TRACE_DEBUG("AES/REF: NULL parameter(s).\n\r");
        return 0;
    }
    if (length%ENCRYPTION_BLOCK_LENGTH != 0) {
        TRACE_DEBUG("AES/REF: Data length must be a multiple of the cipher block size.\n\r");
        return 0;
    }
    // ECB decryption
    for (l=0; l < length;) {
        // Copy cipher text block, decrypt it and copy result
        for (i=0; i < ENCRYPTION_BLOCK_LENGTH; i++) {
            ((char *) block)[i] = cipherText[l+i];
        }
        decrypt(block, expandedKey, T0, T1, T2, T3, TF);
        for (i=0; i < ENCRYPTION_BLOCK_LENGTH; i++) {
            plainText[l+i] = ((char *) block)[i];
        }
        l += ENCRYPTION_BLOCK_LENGTH;
    }

    return 1;
}
#endif

//------------------------------------------------------------------------------
/// Decrypts a cipher text using CBC mode
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
/// \param Expanded key to use
/// \param Initialization vector to use
/// \return 1 if successful, 0 otherwise */
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CBC)
static unsigned int cbc_decrypt(const unsigned char * cipherText,
                                       unsigned char * plainText,
                                       unsigned int length,
                                       const unsigned char expandedKey[ROUNDS+1][BC][4],
                                       unsigned char IV[BC][4])
{             
    unsigned char block[BC][4];
    unsigned int i;
    unsigned int l;

    // Check input parameters
    if ((cipherText == NULL) || (plainText == NULL)) {
        TRACE_DEBUG("AES/REF: NULL parameter(s).\n\r");
        return 0;
    }
    if (length%ENCRYPTION_BLOCK_LENGTH != 0) {
        TRACE_DEBUG("AES/REF: Cipher text length must be a multiple of the cipher block length.\n\r");
        return 0;
    }
    // Decrypt data
    for (l=0; l < length;) {
        // Copy and decrypt a block of cipher text
        for (i=0; i < BC; i++) {
            ((int *) block)[i] = ((int *) &cipherText[l])[i];
        }
        decrypt(block, expandedKey, T0, T1, T2, T3, TF);
        // Xor decrypted text & IV, copy new IV
        for (i=0; i < BC; i++) {
            unsigned int tmp = ((int *) block)[i] ^ ((int *) IV)[i];
            ((int *) IV)[i] = ((int *) &cipherText[l])[i];
            ((int *) &plainText[l])[i] = tmp;
        }

        // Loop progression
        l += ENCRYPTION_BLOCK_LENGTH;
    }
    return 1;
}
#endif

//------------------------------------------------------------------------------
/// Decrypts a cipher text using CTR mode
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text
/// \param Expanded key to use
/// \param Initialization vector to use
/// \return 1 if successful, 0 otherwise
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
static unsigned int ctr_decrypt(const unsigned char * cipherText,
                                       unsigned char * plainText,
                                       unsigned int length,
                                       const unsigned char expandedKey[ROUNDS+1][BC][4],
                                       unsigned char IV[BC][4])
{
    unsigned char block[BC][4];
    unsigned int bytes;
    unsigned int i;
    unsigned int l;
    int k;

    // Check input parameters
    if ((cipherText == NULL) || (plainText == NULL)) {
        return 0;
    }
    for (l=0; l < length;) {
        // Copy counter and encrypt it
        copyBlock(IV, block);
        encrypt(block, expandedKey, T0, T1, T2, T3, TF);

        // XOR current plain text block with encrypted counter
        bytes = min(length - l, ENCRYPTION_BLOCK_LENGTH);

        for (i=0; i < bytes; i++) {
            plainText[l+i] = cipherText[l+i] ^ ((char *) block)[i];
        }
        // Increment counter (big-endian) and number of encrypted bytes
        for (k=ENCRYPTION_BLOCK_LENGTH-1; k >= 0; k--) {
            if (++((char *) IV)[k] != 0) {
                break;
            }
        }
        l += bytes;
    }
    return 1;
}
#endif

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the AES algorithm
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
void aes_ref_init(void)
{
    TRACE_DEBUG("AES/REF: Initializing ...\n\r");

    ASCII2RawHex((unsigned char*)ENCRYPTION_KEY, (unsigned char*)key, ENCRYPTION_KEY_LENGTH);

#if defined(ENCRYPTION_ECB) || defined(ENCRYPTION_CBC)

    // Initialize key schedule
    invKeySchedule(key, expandedKey);

    // Generate lookup tables
    generateDecryptionLUTs(T0, T1, T2, T3, TF, Si);

#elif defined(ENCRYPTION_CTR)

    // Initialize key schedule
    keySchedule(key, expandedKey);

    // Generate lookup tables
    generateEncryptionLUTs(T0, T1, T2, T3, TF, S);
#endif
  
#if defined(ENCRYPTION_CBC) || defined(ENCRYPTION_CTR)
    // Initialize counter
    ASCII2RawHex((unsigned char*)ENCRYPTION_IV, (unsigned char*)IV, ENCRYPTION_BLOCK_LENGTH);
#endif

    TRACE_DEBUG("AES/REF: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the AES algorithm mode CBC
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
#if defined(ENCRYPTION_CBC)
void aes_ref_init_CBC(void)
{
    TRACE_DEBUG("aes_ref_init_CBC\n\r");

    ASCII2RawHex((unsigned char*)ENCRYPTION_KEY, (unsigned char*)key, ENCRYPTION_KEY_LENGTH);

    // Initialize key schedule
    invKeySchedule(key, expandedKey);

    // Generate lookup tables
    generateDecryptionLUTs(T0, T1, T2, T3, TF, Si);

    // Initialize counter
    ASCII2RawHex((unsigned char*)ENCRYPTION_IV, (unsigned char*)IV, ENCRYPTION_BLOCK_LENGTH);

    TRACE_DEBUG("AES/REF: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the AES algorithm mode ECB
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB)
void aes_ref_init_ECB(void)
{
    TRACE_DEBUG("aes_ref_init_ECB\n\r");

    ASCII2RawHex((unsigned char*)ENCRYPTION_KEY, (unsigned char*)key, ENCRYPTION_KEY_LENGTH);

    // Initialize key schedule
    invKeySchedule(key, expandedKey);

    // Generate lookup tables
    generateDecryptionLUTs(T0, T1, T2, T3, TF, Si);

    TRACE_DEBUG("AES/REF: Initialization done.\n\r");
}
#endif

//------------------------------------------------------------------------------
/// Initializes the AES algorithm mode CTR
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
void aes_ref_init_CTR(void)
{
    TRACE_DEBUG("aes_ref_init_CTR\n\r");

    ASCII2RawHex((unsigned char*)ENCRYPTION_KEY, (unsigned char*)key, ENCRYPTION_KEY_LENGTH);

    // Initialize key schedule
    keySchedule(key, expandedKey);

    // Generate lookup tables
    generateEncryptionLUTs(T0, T1, T2, T3, TF, S);

    // Initialize counter
    ASCII2RawHex((unsigned char*)ENCRYPTION_IV, (unsigned char*)IV, ENCRYPTION_BLOCK_LENGTH);

    TRACE_DEBUG("AES/REF: Initialization done.\n\r");
}
#endif
#endif // ONLY_ONE_ENCRYPTION

//------------------------------------------------------------------------------
/// Cleanup the AES algorithm
//------------------------------------------------------------------------------
void aes_ref_cleanup(void)
{
    TRACE_DEBUG("AES/REF: Cleaning up ...\n\r");
    TRACE_DEBUG("AES/REF: Cleanup done.\n\r");
}

//------------------------------------------------------------------------------
/// Decrypt a cipher text of variable length
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
/// \return 1 if decryption was successful, 0 otherwise.
//------------------------------------------------------------------------------
#ifdef ONLY_ONE_ENCRYPTION
int aes_ref_decrypt(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length)
{
    TRACE_DEBUG("aes_ref_decrypt\n\r");
#if defined(ENCRYPTION_ECB)
    return ecb_decrypt(cipherText, plainText, length, expandedKey);
#elif defined(ENCRYPTION_CBC)
    return cbc_decrypt(cipherText, plainText, length, expandedKey, IV);;
#elif defined(ENCRYPTION_CTR)
    return ctr_decrypt(cipherText, plainText, length, expandedKey, IV);
#endif
}
#endif

//------------------------------------------------------------------------------
/// Decrypt a cipher text of variable length, mode CBC
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
/// \return 1 if decryption was successful, 0 otherwise.
//------------------------------------------------------------------------------
#ifndef ONLY_ONE_ENCRYPTION
#if defined(ENCRYPTION_CBC)
int aes_ref_decrypt_CBC(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length)
{
    TRACE_DEBUG("aes_ref_decrypt_CBC\n\r");
    return cbc_decrypt(cipherText, plainText, length, expandedKey, IV);;
}
#endif

//------------------------------------------------------------------------------
/// Decrypt a cipher text of variable length, mode ECB
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
/// \return 1 if decryption was successful, 0 otherwise.
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_ECB)
int aes_ref_decrypt_ECB(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length)
{
    TRACE_DEBUG("aes_ref_decrypt_ECB\n\r");
    return ecb_decrypt(cipherText, plainText, length, expandedKey);
}
#endif

//------------------------------------------------------------------------------
/// Decrypt a cipher text of variable length, mode CTR
/// \param Cipher text to decrypt
/// \param Buffer to store plain text
/// \param Length of cipher text (in bytes)
/// \return 1 if decryption was successful, 0 otherwise.
//------------------------------------------------------------------------------
#if defined(ENCRYPTION_CTR)
int aes_ref_decrypt_CTR(const unsigned char * cipherText,
                    unsigned char * plainText,
                    unsigned int length)
{
    TRACE_DEBUG("aes_ref_decrypt_CTR\n\r");
    return ctr_decrypt(cipherText, plainText, length, expandedKey, IV);
}
#endif

#endif // ONLY_ONE_ENCRYPTION

#endif // defined(USE_ENCRYPTION) && defined(ENCRYPTION_AES_REF)



