/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "integration_test_plat_defs.h"
#include "ssi_pal_mutex_plat.h"
#include "nrf_cc310_bl_init.h"
#include "nrf_cc310_bl_ecdsa_verify_secp256r1.h"
#include "nrf_cc310_bl_ecdsa_verify_secp224r1.h"
#include "nrf_cc310_bl_hash_common.h"
#include "nrf_cc310_bl_hash_sha256.h"
#include <string.h>
#include "nrf.h"

#define NUM_ELEMENTS(x)  (sizeof(x) / sizeof((x)[0]))

// Simple implementation of HardFault handler in case of unaligned access or null
void HardFault_Handler(void)
{
    INTEG_TEST_PRINT("hardfault");
    while(1);
}

static nrf_cc310_bl_hash_context_sha256_t   hash_context;
static nrf_cc310_bl_hash_digest_sha256_t    hash_digest;
//
//  Input-data to the ecdsa verify function: Hash of the term "sample"
//  According to RFC6979, the input data "sample" is described as a
//  utf-8 string with 6 octets (which means disregarding the null termination).
//  Ref: rfc6979 A.1.2
//
static uint8_t hash_input[6] = "sample";

// Expected hash 
static nrf_cc310_bl_hash_digest_sha256_t hash_digest_expected = 
{
    0xaf, 0x2b, 0xdb, 0xe1, 0xaa, 0x9b, 0x6e, 0xc1, 0xe2, 0xad, 0xe1, 0xd6, 0x94, 0xf4, 0x1f, 0xc7, 
    0x1a, 0x83, 0x1d, 0x02, 0x68, 0xe9, 0x89, 0x15, 0x62, 0x11, 0x3d, 0x8a, 0x62, 0xad, 0xd1, 0xbf
};

// Invalid hash with some bytes in the middle set to zero
static nrf_cc310_bl_hash_digest_sha256_t hash_digest_faulty = 
{
    0xaf, 0x2b, 0xdb, 0xe1, 0xaa, 0x9b, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0x94, 0xf4, 0x1f, 0xc7, 
    0x1a, 0x83, 0x1d, 0x02, 0x68, 0xe9, 0x89, 0x15, 0x62, 0x11, 0x3d, 0x8a, 0x62, 0xad, 0xd1, 0xbf
};


//
//   For ECDSA verify using secp256r1 and SHA-256
//
static nrf_cc310_bl_ecdsa_verify_context_secp256r1_t    ecdsa_context_secp256r1;

// Public key according to rfc6979 for secp256r1 in A.2.5
// Ux = 60FED4BA255A9D31C961EB74C6356D68C049B8923B61FA6CE669622E60F29FB6
// Uy = 7903FE1008B8BC99A41AE9E95628BC64F2F1B20C2D7E9F5177A3C294D4462299
static nrf_cc310_bl_ecc_public_key_secp256r1_t ecc_public_key_secp256r1 =
{
    .x = { 0x60, 0xFE, 0xD4, 0xBA, 0x25, 0x5A, 0x9D, 0x31, 0xC9, 0x61, 0xEB, 0x74, 0xC6, 0x35, 0x6D, 0x68,
           0xC0, 0x49, 0xB8, 0x92, 0x3B, 0x61, 0xFA, 0x6C, 0xE6, 0x69, 0x62, 0x2E, 0x60, 0xF2, 0x9F, 0xB6 },
    .y = { 0x79, 0x03, 0xFE, 0x10, 0x08, 0xB8, 0xBC, 0x99, 0xA4, 0x1A, 0xE9, 0xE9, 0x56, 0x28, 0xBC, 0x64,
           0xF2, 0xF1, 0xB2, 0x0C, 0x2D, 0x7E, 0x9F, 0x51, 0x77, 0xA3, 0xC2, 0x94, 0xD4, 0x46, 0x22, 0x99 }
};

// Signature according to rfc6979 for secp256r1 in A.2.5
// r = EFD48B2AACB6A8FD1140DD9CD45E81D69D2C877B56AAF991C34D0EA84EAF3716
// s = F7CB1C942D657C41D436C7A1B6E29F65F3E900DBB9AFF4064DC4AB2F843ACDA8
static nrf_cc310_bl_ecc_signature_secp256r1_t ecdsa_signature_secp256r1 = 
{
    .r = { 0xEF, 0xD4, 0x8B, 0x2A, 0xAC, 0xB6, 0xA8, 0xFD, 0x11, 0x40, 0xDD, 0x9C, 0xD4, 0x5E, 0x81, 0xD6,
           0x9D, 0x2C, 0x87, 0x7B, 0x56, 0xAA, 0xF9, 0x91, 0xC3, 0x4D, 0x0E, 0xA8, 0x4E, 0xAF, 0x37, 0x16},
    .s = { 0xF7, 0xCB, 0x1C, 0x94, 0x2D, 0x65, 0x7C, 0x41, 0xD4, 0x36, 0xC7, 0xA1, 0xB6, 0xE2, 0x9F, 0x65,
           0xF3, 0xE9, 0x00, 0xDB, 0xB9, 0xAF, 0xF4, 0x06, 0x4D, 0xC4, 0xAB, 0x2F, 0x84, 0x3A, 0xCD, 0xA8}
};


// Invalid signature with some data set to zero.
static nrf_cc310_bl_ecc_signature_secp256r1_t ecdsa_signature_secp256r1_faulty = 
{
    .r = { 0xEF, 0xD4, 0x8B, 0x2A, 0xAC, 0xB6, 0xA8, 0xFD, 0x11, 0x40, 0xDD, 0x9C, 0xD4, 0x5E, 0x81, 0xD6,
           0x9D, 0x2C, 0x87, 0x00, 0x00, 0xAA, 0x00, 0x91, 0xC3, 0x4D, 0x0E, 0xA8, 0x4E, 0xAF, 0x37, 0x16},
    .s = { 0xF7, 0xCB, 0x1C, 0x94, 0x2D, 0x65, 0x7C, 0x00, 0xD4, 0x00, 0xC7, 0xA1, 0xB6, 0xE2, 0x9F, 0x65,
           0xF3, 0xE9, 0x00, 0xDB, 0xB9, 0xAF, 0xF4, 0x06, 0x4D, 0xC4, 0xAB, 0x2F, 0x84, 0x3A, 0xCD, 0xA8}
};


//
//   For ECDSA verify using secp224r1 and SHA-256
//
static nrf_cc310_bl_ecdsa_verify_context_secp224r1_t    ecdsa_context_secp224r1;

// Public key according to rfc6979 for secp224r1 in A.2.4
// Ux = 00CF08DA5AD719E42707FA431292DEA11244D64FC51610D94B130D6C
// Uy = EEAB6F3DEBE455E3DBF85416F7030CBD94F34F2D6F232C69F3C1385A
static nrf_cc310_bl_ecc_public_key_secp224r1_t ecc_public_key_secp224r1 =
{
    .x = { 0x00, 0xCF, 0x08, 0xDA, 0x5A, 0xD7, 0x19, 0xE4, 0x27, 0x07, 0xFA, 0x43, 0x12, 0x92,
           0xDE, 0xA1, 0x12, 0x44, 0xD6, 0x4F, 0xC5, 0x16, 0x10, 0xD9, 0x4B, 0x13, 0x0D, 0x6C },
    .y = { 0xEE, 0xAB, 0x6F, 0x3D, 0xEB, 0xE4, 0x55, 0xE3, 0xDB, 0xF8, 0x54, 0x16, 0xF7, 0x03,
           0x0C, 0xBD, 0x94, 0xF3, 0x4F, 0x2D, 0x6F, 0x23, 0x2C, 0x69, 0xF3, 0xC1, 0x38, 0x5A }
};

// Signature according to rfc6979 for secp224r1 in A.2.4
// r = 61AA3DA010E8E8406C656BC477A7A7189895E7E840CDFE8FF42307BA
// s = BC814050DAB5D23770879494F9E0A680DC1AF7161991BDE692B10101
static nrf_cc310_bl_ecc_signature_secp224r1_t ecdsa_signature_secp224r1 = 
{
    .r = { 0x61, 0xAA, 0x3D, 0xA0, 0x10, 0xE8, 0xE8, 0x40, 0x6C, 0x65, 0x6B, 0xC4, 0x77, 0xA7,
           0xA7, 0x18, 0x98, 0x95, 0xE7, 0xE8, 0x40, 0xCD, 0xFE, 0x8F, 0xF4, 0x23, 0x07, 0xBA },
    .s = { 0xBC, 0x81, 0x40, 0x50, 0xDA, 0xB5, 0xD2, 0x37, 0x70, 0x87, 0x94, 0x94, 0xF9, 0xE0,
           0xA6, 0x80, 0xDC, 0x1A, 0xF7, 0x16, 0x19, 0x91, 0xBD, 0xE6, 0x92, 0xB1, 0x01, 0x01 }
};


// Invalid signature with some data set to zero.
static nrf_cc310_bl_ecc_signature_secp224r1_t ecdsa_signature_secp224r1_faulty = 
{
    .r = { 0x61, 0xAA, 0x3D, 0xA0, 0x10, 0xE8, 0xE8, 0x40, 0x6C, 0x65, 0x6B, 0xC4, 0x77, 0xA7,
           0xA7, 0x18, 0x98, 0x95, 0xE7, 0xE8, 0x40, 0xCD, 0xFE, 0x8F, 0xF0, 0x23, 0x07, 0xBA },
    .s = { 0xBC, 0x81, 0x40, 0x50, 0xDA, 0xB5, 0xD2, 0x37, 0x70, 0x87, 0x94, 0x94, 0xF9, 0xE0,
           0xA6, 0x80, 0xDC, 0x1A, 0xF7, 0x16, 0x19, 0x91, 0xBD, 0xE6, 0x92, 0xB1, 0x01, 0x01 }
};


void print_array(char const * const name, uint8_t const * const p_src, size_t size)
{
    INTEG_TEST_PRINT("Array %s:\r\n", name);
    for(int i = 0; i < size - 1; i++)
        INTEG_TEST_PRINT("0x%x, ", p_src[i]);
    INTEG_TEST_PRINT("0x%x\r\n\r\n", p_src[size-1]);
}

void test_check(CRYSError_t error)
{
    if(error != SASI_SUCCESS)
    {
        INTEG_TEST_PRINT("Failure: 0x%08x\r\n", error);
        while(1);
    }
}
    

int main(void)
{
    int ret = 0;
    CRYSError_t err_code;
    
    // Enables log and sets CryptoCell HW to enabled.
    ret = integration_tests_setup();
    test_check(ret);
    

    // Initialize the thin version of the run-time library (no RNG support). 
    INTEG_TEST_PRINT("Initializing nrf_cc310_bl\r\n");
    err_code = nrf_cc310_bl_init();
    test_check(err_code);

    //
    // Hash test
    //
    
    INTEG_TEST_PRINT("\r\n\r\n===================\r\n");
    INTEG_TEST_PRINT("Testing SHA-256 hash\r\n\r\n");
    
    INTEG_TEST_PRINT("Initializing SHA-256 context\r\n");
    err_code = nrf_cc310_bl_hash_sha256_init(&hash_context);
    test_check(err_code);
    
    INTEG_TEST_PRINT("Running SHA-256 update\r\n");
    err_code = nrf_cc310_bl_hash_sha256_update(&hash_context, hash_input, 6);
    test_check(err_code);
    
    INTEG_TEST_PRINT("Running SHA-256 finalize\r\n");
    err_code = nrf_cc310_bl_hash_sha256_finalize(&hash_context, &hash_digest);
    test_check(err_code);
    
    print_array("expected", hash_digest, 32);
    print_array("calculated", hash_digest_expected, 32);
    
    INTEG_TEST_PRINT("Comparing result\r\n");
    if( memcmp(&hash_digest, &hash_digest_expected, sizeof(nrf_cc310_bl_hash_digest_sha256_t)) != 0)
    {
        APP_ERROR_CHECK(false);
    }
    INTEG_TEST_PRINT("Hash generation was successful!\r\n");
    
    
    //
    // Valid ECDSA secp256r1 verify test
    //
    
    INTEG_TEST_PRINT("\r\n\r\n====================================\r\n");
    INTEG_TEST_PRINT("Testing valid ECDSA secp256r1 verify\r\n\r\n");
    
    INTEG_TEST_PRINT("Initializing ecdsa verify context secp256r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_init_secp256r1(&ecdsa_context_secp256r1, &ecc_public_key_secp256r1);
    test_check(err_code);
    
    
    INTEG_TEST_PRINT("Executing ecdsa verify secp256r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_hash_secp256r1(&ecdsa_context_secp256r1, 
                                                        &ecdsa_signature_secp256r1, 
                                                        hash_digest, 
                                                        sizeof(nrf_cc310_bl_hash_digest_sha256_t));
    if (err_code == SASI_SUCCESS)
    {
        INTEG_TEST_PRINT("Verification was successful!\r\n");
    }
    else
    {
        INTEG_TEST_PRINT("Verification failed!\r\n");
        test_check(err_code);
    }
    
    //
    // Invalid ECDSA secp256r1 verify test (wrong signature)
    //
    
    INTEG_TEST_PRINT("\r\n\r\n=======================================================\r\n");
    INTEG_TEST_PRINT("Testing ECDSA secp256r1 verify failure scenario (wrong signature)\r\n\r\n");
    
    INTEG_TEST_PRINT("Initializing ecdsa verify context secp256r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_init_secp256r1(&ecdsa_context_secp256r1, &ecc_public_key_secp256r1);
    test_check(err_code);
    
    
    INTEG_TEST_PRINT("Executing ecdsa verify secp256r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_hash_secp256r1(&ecdsa_context_secp256r1, 
                                                        &ecdsa_signature_secp256r1_faulty, 
                                                        hash_digest, 
                                                        sizeof(nrf_cc310_bl_hash_digest_sha256_t));
    if (err_code == SASI_SUCCESS)
    {
        INTEG_TEST_PRINT("Verification not supposed to be successful!\r\n");
        test_check(err_code);
        
    }
    else
    {
        INTEG_TEST_PRINT("Verification failed as it should!\r\n");
    }
    
    
    //
    // Invalid ECDSA secp256r1 verify test (wrong hash)
    //
    
    INTEG_TEST_PRINT("\r\n\r\n==================================================\r\n");
    INTEG_TEST_PRINT("Testing ECDSA secp256r1 verify failure scenario (wrong hash)\r\n\r\n");
    
    INTEG_TEST_PRINT("Initializing ecdsa verify context secp256r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_init_secp256r1(&ecdsa_context_secp256r1, &ecc_public_key_secp256r1);
    test_check(err_code);
    
    
    INTEG_TEST_PRINT("Executing ecdsa verify secp256r1 with wrong hash\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_hash_secp256r1(&ecdsa_context_secp256r1, 
                                                        &ecdsa_signature_secp256r1, 
                                                        hash_digest_faulty, 
                                                        sizeof(nrf_cc310_bl_hash_digest_sha256_t));
    if (err_code == SASI_SUCCESS)
    {
        INTEG_TEST_PRINT("Verification not supposed to be successful!\r\n");
        test_check(err_code);
        
    }
    else
    {
        INTEG_TEST_PRINT("Verification failed as it should!\r\n");
    }
    
    
    //
    // Valid ECDSA secp224r1 verify test
    //
    
    INTEG_TEST_PRINT("\r\n\r\n====================================\r\n");
    INTEG_TEST_PRINT("Testing valid ECDSA secp224r1 verify\r\n\r\n");
    
    INTEG_TEST_PRINT("Initializing ecdsa verify context secp224r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_init_secp224r1(&ecdsa_context_secp224r1, &ecc_public_key_secp224r1);
    test_check(err_code);
    
    
    INTEG_TEST_PRINT("Executing ecdsa verify secp224r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_hash_secp224r1(&ecdsa_context_secp224r1, 
                                                        &ecdsa_signature_secp224r1, 
                                                        hash_digest, 
                                                        sizeof(nrf_cc310_bl_hash_digest_sha256_t));
    if (err_code == SASI_SUCCESS)
    {
        INTEG_TEST_PRINT("Verification was successful!\r\n");
    }
    else
    {
        INTEG_TEST_PRINT("Verification failed!\r\n");
        test_check(err_code);
    }
    
    //
    // Invalid ECDSA secp224r1 verify test (wrong signature)
    //
    
    INTEG_TEST_PRINT("\r\n\r\n=======================================================\r\n");
    INTEG_TEST_PRINT("Testing ECDSA secp224r1 verify failure scenario (wrong signature)\r\n\r\n");
    
    INTEG_TEST_PRINT("Initializing ecdsa verify context secp224r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_init_secp224r1(&ecdsa_context_secp224r1, &ecc_public_key_secp224r1);
    test_check(err_code);
    
    
    INTEG_TEST_PRINT("Executing ecdsa verify secp224r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_hash_secp224r1(&ecdsa_context_secp224r1, 
                                                        &ecdsa_signature_secp224r1_faulty, 
                                                        hash_digest, 
                                                        sizeof(nrf_cc310_bl_hash_digest_sha256_t));
    if (err_code == SASI_SUCCESS)
    {
        INTEG_TEST_PRINT("Verification not supposed to be successful!\r\n");
        test_check(err_code);
        
    }
    else
    {
        INTEG_TEST_PRINT("Verification failed as it should!\r\n");
    }
    
    
    //
    // Invalid ECDSA secp224r1 verify test (wrong hash)
    //
    
    INTEG_TEST_PRINT("\r\n\r\n==================================================\r\n");
    INTEG_TEST_PRINT("Testing ECDSA secp224r1 verify failure scenario (wrong hash)\r\n\r\n");
    
    INTEG_TEST_PRINT("Initializing ecdsa verify context secp224r1\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_init_secp224r1(&ecdsa_context_secp224r1, &ecc_public_key_secp224r1);
    test_check(err_code);
    
    
    INTEG_TEST_PRINT("Executing ecdsa verify secp224r1 with wrong hash\r\n");
    err_code = nrf_cc310_bl_ecdsa_verify_hash_secp224r1(&ecdsa_context_secp224r1, 
                                                        &ecdsa_signature_secp224r1, 
                                                        hash_digest_faulty, 
                                                        sizeof(nrf_cc310_bl_hash_digest_sha256_t));
    if (err_code == SASI_SUCCESS)
    {
        INTEG_TEST_PRINT("Verification not supposed to be successful!\r\n");
        test_check(err_code);
        
    }
    else
    {
        INTEG_TEST_PRINT("Verification failed as it should!\r\n");
    }
    
    INTEG_TEST_PRINT("\r\n\r\n=================================\r\n");
    INTEG_TEST_PRINT("All tests passed!\r\n");
    INTEG_TEST_PRINT("=================================\r\n");
    
    
    while(1);
}
