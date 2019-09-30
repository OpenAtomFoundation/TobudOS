/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sdk_common.h"
#include "nrf_assert.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_crypto.h"
#include "nrf_crypto_ecc.h"
#include "nrf_crypto_error.h"
#include "nrf_crypto_ecdsa.h"
#include "nrf_crypto_rng.h"
#include "nrf_crypto_hash.h"
#include "mem_manager.h"
#include "optiga/optiga_util.h"
#include "optiga/optiga_crypt.h"
#include "nrfx_clock.h"
#include "optiga_backend_ecc.h"
#include "optiga_backend_utils.h"

#include "compiler_abstraction.h"

/* @brief Macro that checks for errors states and prints any nrf_crypto errors
 *        in hex format before stopping the application.
 */
#define DEMO_ERROR_CHECK(error)     \
do                                  \
{                                   \
    if (error != NRF_SUCCESS)       \
    {                               \
        NRF_LOG_ERROR("Error 0x%04X: %s", error, nrf_crypto_error_string_get(error));\
        NRF_LOG_FLUSH();            \
        APP_ERROR_CHECK(error);     \
    }                               \
} while(0)

/* @brief Macro that checks for errors states and prints any OPTIGA errors
 *        in hex format before stopping the application.
 */
#define DEMO_OPTIGA_ERROR_CHECK(status)  \
do                                  \
{                                   \
    if (status != OPTIGA_LIB_SUCCESS)\
    {                               \
        NRF_LOG_ERROR("OPTIGA error 0x%04X: %s", status, "");\
        NRF_LOG_FLUSH();            \
        APP_ERROR_HANDLER(0);       \
    }                               \
} while(0)



//========================================= Example entry =========================================
//


/** @brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void clock_event_handler(nrfx_clock_evt_type_t event)
{
}

const uint8_t test2_hash[] = {
    0x7F, 0x83, 0xB1, 0x65, 0x7F, 0xF1, 0xFC, 0x53, 0xB9, 0x2D, 0xC1, 0x81, 0x48, 0xA1, 0xD6, 0x5D,
    0xFC, 0x2D, 0x4B, 0x1F, 0xA3, 0xD6, 0x77, 0x28, 0x4A, 0xDD, 0xD2, 0x00, 0x12, 0x6D, 0x90, 0x69
};

const size_t test2_hash_len = 32;

static void pr_hex(const uint8_t* data, size_t len)
{
    NRF_LOG_INFO("HEX dump of %d bytes", len);
    NRF_LOG_RAW_HEXDUMP_INFO(data, len);
    NRF_LOG_RAW_INFO("\r\n");
    NRF_LOG_FLUSH();
}


static void uc_random_number_generation(void)
{
    ret_code_t err_code = NRF_SUCCESS;
    uint8_t buf[300] = {0};
    NRF_LOG_INFO("Random data 5 bytes:");
    err_code = nrf_crypto_rng_vector_generate(buf, 5);
    DEMO_ERROR_CHECK(err_code);
    pr_hex(buf, 5);

    NRF_LOG_INFO("Random data 32 bytes:");
    err_code = nrf_crypto_rng_vector_generate(buf, 32);
    DEMO_ERROR_CHECK(err_code);
    pr_hex(buf, 32);

    NRF_LOG_INFO("Random data 300 bytes:");
    err_code = nrf_crypto_rng_vector_generate(buf, 300);
    DEMO_ERROR_CHECK(err_code);
    pr_hex(buf, 300);
}


static void uc_print_coprocessor_information(void)
{
   optiga_lib_status_t optiga_lib_status;

    uint8_t  data[100] = { 0 };
    uint16_t data_len  = 100;

    NRF_LOG_INFO("Read coprocessor UID: version");

    // Read the version
    optiga_lib_status = optiga_util_read_data(0xE0C2, 0, data, &data_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    NRF_LOG_INFO("Trust X firmware version (from coprocessor UID): %2x%2x\r\n", (uint16_t) data[25], (uint16_t) data[26]);

    // Remove the comment to print the binary data of the coprocessor information
    //NRF_LOG_HEXDUMP_INFO(data, data_len);
}

static void uc_key_derivation(void)
{
    optiga_lib_status_t optiga_lib_status;
    uint8_t  info[100] = { 0 };
    uint16_t info_len  = 100;
    uint16_t oid               = 0xF1D0;
    uint8_t  shared_secret[64] = { 0 };

    // Check if key derivation is supported (OPTIGA Trust X after version 1.20.1048)
    optiga_lib_status = optiga_util_read_data(0xE0C2, 0, info, &info_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    if (info[25] == 0x10 && info[26] == 0x48)
    {
        NRF_LOG_INFO("Key derivation not supported!\r\n");
        NRF_LOG_FLUSH();
        return;
    }

    memset(shared_secret, 0x91, sizeof(shared_secret));

    NRF_LOG_INFO("Key derivation");

    NRF_LOG_INFO("Write shared secret");
    optiga_lib_status = optiga_util_write_data(oid, OPTIGA_UTIL_ERASE_AND_WRITE, 0, shared_secret, sizeof(shared_secret));
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    // Verify that shared secret was properly written
    uint8_t  data_object[128] = { 0x00 };
    memset(data_object, 0xAA, sizeof(data_object));
    uint16_t data_object_len = sizeof(data_object);
    optiga_lib_status = optiga_util_read_data(oid, 0, data_object, &data_object_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    NRF_LOG_HEXDUMP_INFO(data_object, data_object_len);

    NRF_LOG_INFO("Derive key:");

    uint8_t derived_key[32] = { 0 };
    uint8_t seed[32]        = { 0 };
    memset(seed, 0x33, sizeof(seed));

    optiga_lib_status = optiga_crypt_tls_prf_sha256(
      oid,                    // OID which contains shared secret,
      NULL,                   // Not needed
      0,                      // Not needed
      seed,                   // Pointer to the buffer which contain seed
      sizeof(seed),           // Length of the seed
      sizeof(derived_key),    // Length of the derived key
      true,                   // Indicate the storage type for the derive key
      derived_key             // Pointer to the buffer or OID to hold the derived key
    );
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    NRF_LOG_HEXDUMP_INFO(derived_key, sizeof(derived_key));
}

/** @brief If Trust X slot E0E0 contains a TLS certificate chain
           instead of a sole certificate, it cannot be used for signature verification.
           This function checks the certificate in E0E0, strips the chain, and stores a pure
           certificate in E0E1.
 */
static void uc_read_write_certificate(void)
{
    // Maximum size of the certificate
    #define CHAIN_SIZE 1728
    optiga_lib_status_t optiga_lib_status;

    // print internal certificate
    // This is a TLS certificate chain and not suitable for optiga_verify
    uint8_t  chain[CHAIN_SIZE] = {0};
    uint8_t  chain2[CHAIN_SIZE] = {0};
    uint8_t  chain3[CHAIN_SIZE] = {0};
    uint16_t chain_len = CHAIN_SIZE;

    NRF_LOG_INFO("Reading/Writing certificate");
    NRF_LOG_FLUSH();

    optiga_lib_status = optiga_util_read_data(eDEVICE_PUBKEY_CERT_IFX, 0, chain, &chain_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);
    NRF_LOG_INFO("Certificate Chain [%d bytes]:", chain_len);
    pr_hex(chain, chain_len);

    optiga_lib_status = optiga_util_read_data(eDEVICE_PUBKEY_CERT_IFX, 0, chain2, &chain_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);
    NRF_LOG_INFO("Certificate Chain 2 [%d bytes]:", chain_len);
    pr_hex(chain2, chain_len);

        optiga_lib_status = optiga_util_read_data(eDEVICE_PUBKEY_CERT_IFX, 0, chain3, &chain_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);
    NRF_LOG_INFO("Certificate Chain 3 [%d bytes]:", chain_len);
    pr_hex(chain3, chain_len);


    uint8_t * p_cert_start = chain;

    if (chain[0] == 0xC0) // 0x30 = TLS identity tag
    {
        // Strip TLS certificate chain header from certificate chain
        // Please refer to the official documentation for detailed information on public-key certificate data structures).
        //   Example TLS certificate header
        //     C0 01 CA 00 01 C7 00 01 C4 30 82 01 C0 30 82 ... end
        //     <- t1 -> <- va -> <- v1 -> <- X.509 cert  --------->
        p_cert_start += 9;
    }
    else if (chain[0] == 0x30) // 0xC0 = one-way authentication identity
    {
        // Pure ASN.1-coded certificate, no need to strip anything
    }
    else
    {
        APP_ERROR_HANDLER(1); // Not supported case
    }

    size_t cert_len = chain_len - (p_cert_start - chain);

    // Print stripped certificate
    NRF_LOG_INFO("Certificate (stripped) [%d bytes]:", cert_len);
    pr_hex(p_cert_start, cert_len);

    // write stripped certificate into PRJSPC_1 slot for verification

    optiga_lib_status = optiga_util_write_data(eDEVICE_PUBKEY_CERT_PRJSPC_1, OPTIGA_UTIL_ERASE_AND_WRITE, 0, p_cert_start, cert_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    memset(chain, 0, sizeof(chain));
    chain_len = CHAIN_SIZE;

    optiga_lib_status = optiga_util_read_data(eDEVICE_PUBKEY_CERT_PRJSPC_1, 0, chain, &chain_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    NRF_LOG_INFO("Certificate in PRJSPC_1 [%d bytes]:", chain_len);
    pr_hex(chain, chain_len);

#undef CHAIN_SIZE
}


static void uc_sign(uint8_t * p_sig, size_t * sig_len)
{
    ret_code_t err_code = NRF_SUCCESS;

    NRF_LOG_INFO("Signing hash");
    NRF_LOG_FLUSH();

    // Pass OPTIGA key OID into nrf_cyrpto API
    nrf_crypto_ecc_private_key_t private_key = NRF_CRYPTO_INFINEON_SECP256R1_PRIVATE_KEY_FROM_OID(OPTIGA_KEY_STORE_ID_E0F0);

    // compute signature
    err_code = nrf_crypto_ecdsa_sign(NULL, &private_key, test2_hash, test2_hash_len, p_sig, sig_len);
    DEMO_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Signature:");
    pr_hex(p_sig, *sig_len);
}


static void uc_verify(uint8_t * p_sig, size_t sig_len)
{
    ret_code_t err_code = NRF_SUCCESS;

    // verify signature using the certificate from PRJSPC_1

    nrf_crypto_ecc_public_key_t pub_key =
        NRF_CRYPTO_INFINEON_SECP256R1_PUBLIC_KEY_FROM_OID(eDEVICE_PUBKEY_CERT_PRJSPC_1);

    // Pass OPTIGA public key certificate OID into nrf_crypto API
    err_code = nrf_crypto_ecdsa_verify(NULL, &pub_key, test2_hash, test2_hash_len, p_sig, sig_len);
    DEMO_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Verify: %s", err_code == NRF_SUCCESS ? "PASS" : "FAIL");
}

static void uc_verify_ext()
{
    ret_code_t err_code = NRF_SUCCESS;

    // verify signature using a host supplied public key

    // Signature verification key
    const uint8_t verify_test_key[] = {
        0xca, 0x7e, 0xaa, 0x6e, 0x89, 0x3c, 0xef, 0x3a, 0x0e, 0x63, 0x69, 0xa7, 0x6d, 0xbe, 0x35,
        0x84, 0x81, 0x0b, 0x58, 0x95, 0xbd, 0x27, 0x42, 0x28, 0x80, 0x0e, 0x24, 0xd6, 0xb8, 0xc9, 0xf7,
        0x72, 0xcd, 0x16, 0x2f, 0x29, 0x58, 0xb6, 0xad, 0xfd, 0x3b, 0xeb, 0xf7, 0x09, 0x26, 0xcc, 0xf7,
        0x80, 0x49, 0x3c, 0x54, 0xe5, 0x45, 0x97, 0xa8, 0xcd, 0x27, 0x6b, 0x1c, 0x86, 0x56, 0xd5, 0x07, 0x6f
    };

    const uint8_t hallo_hash[] = { // = SHA256("Hallo")
        0x75, 0x36, 0x92, 0xec, 0x36, 0xad, 0xb4, 0xc7, 0x94, 0xc9, 0x73, 0x94, 0x5e, 0xb2, 0xa9, 0x9c,
        0x16, 0x49, 0x70, 0x3e, 0xa6, 0xf7, 0x6b, 0xf2, 0x59, 0xab, 0xb4, 0xfb, 0x83, 0x8e, 0x01, 0x3e
    };

    // test signature over "Hallo"
    const uint8_t test_signature[] = {
        //0x30, 0x45, 0x02,
        // R
        //0x21, 0x00,
        0xD1, 0x4A, 0xC7, 0xB0, 0x40, 0x96, 0x9F, 0xB7, 0xB3, 0x20, 0x2F, 0x0A, 0x63, 0x2A, 0x91, 0xE9,
        0xDA, 0xB4, 0x66, 0x9C, 0xB5, 0x52, 0x44, 0x86, 0xBB, 0xDE, 0x5D, 0xCC, 0x91, 0x9B, 0x5A, 0x80,
        // S
        //0x02, 0x20,
        0x26, 0x7F, 0x21, 0xBE, 0xE2, 0x70, 0x7F, 0x1B, 0xB7, 0xF7, 0x73, 0x62, 0x26, 0x23, 0xF6, 0x97,
        0x5F, 0x98, 0x51, 0xE6, 0xC4, 0xE4, 0xC4, 0x24, 0xA3, 0xEF, 0x3F, 0xBE, 0xF1, 0x13, 0x91, 0xA4,
    };

    nrf_crypto_ecc_public_key_t pub_key = NRF_CRYPTO_INFINEON_SECP256R1_PUBLIC_KEY_RAW;
    // Pass OPTIGA public key certificate OID into nrf_cyrpto API

    err_code = nrf_crypto_ecc_public_key_from_raw(&g_nrf_crypto_ecc_secp256r1_curve_info, &pub_key,
                                                  verify_test_key, NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE);
    DEMO_ERROR_CHECK(err_code);

    err_code = nrf_crypto_ecdsa_verify(NULL, &pub_key, hallo_hash, 32,
                                       test_signature, NRF_CRYPTO_ECDSA_SECP256R1_SIGNATURE_SIZE);
    DEMO_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Verify: %s", err_code == NRF_SUCCESS ? "PASS" : "FAIL");
}


/** @brief Function that reads the Trust X certificate, computes its hash,
 *         and stores the hash in Trust X data object.
 */
static void uc_hash_cert(void)
{
    optiga_lib_status_t optiga_lib_status;
    ret_code_t          err = NRF_SUCCESS;

    NRF_LOG_INFO("Hashing Infineon certificate");
    NRF_LOG_FLUSH();

    uint8_t  cert[2048] = { 0 };
    uint16_t cert_len   = 2048;
    optiga_lib_status = optiga_util_read_data(eDEVICE_PUBKEY_CERT_IFX, 0, cert, &cert_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    // Hash the certificate using the nrf_crypto API
    uint8_t                   hash[NRF_CRYPTO_HASH_SIZE_SHA256] = {0};
    size_t                    hash_len                          = NRF_CRYPTO_HASH_SIZE_SHA256;
    nrf_crypto_hash_context_t hash_ctx;

    err = nrf_crypto_hash_calculate(&hash_ctx,
                                    &g_nrf_crypto_hash_sha256_info,
                                    cert, cert_len,
                                    hash, &hash_len);
    DEMO_ERROR_CHECK(err);
    NRF_LOG_INFO("SHA256 of the certificate:");
    pr_hex(hash, hash_len);

    // Write the certificate hash back into a data object
    optiga_lib_status = optiga_util_write_data(0xF1D0, OPTIGA_UTIL_ERASE_AND_WRITE, 0, hash, hash_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    NRF_LOG_INFO("Writing hash of certificate to arbitrary data object successfull");
    NRF_LOG_FLUSH();
}

// Read the metadata from a data object
static void uc_read_metadata(void)
{
    optiga_lib_status_t optiga_lib_status;

    NRF_LOG_INFO("Reading Metadata of INFINEON certificate:");
    NRF_LOG_FLUSH();

    uint8_t metadata_buf[100] = {0};
    uint16_t metadata_len = 100;
    optiga_lib_status = optiga_util_read_metadata(eDEVICE_PUBKEY_CERT_IFX, metadata_buf, &metadata_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    NRF_LOG_INFO("Metadata of INFINEON certificate:");
    NRF_LOG_FLUSH();
    pr_hex(metadata_buf, metadata_len);
}

// write the metadata of a data object
static void uc_write_metadata(void)
{
    optiga_lib_status_t optiga_lib_status;

    NRF_LOG_INFO("Writing metadata of PRJSPC_2 certificate");
    NRF_LOG_FLUSH();

    // for the full list of tags see:
    // OPTIGA Trust X1 Solution Reference Manual
    // Table 37 - Metadata associated with data and key objects

    // NOTE: since the lifecycle can only be advanced, for this example we set it to "Creation"
    // in order to avoid locking a data object
    uint8_t new_metadata_buf[] = {
        0x20, // metadata tag
        0x03, // number of bytes following
        0xC0, // lifecycle state tag
        0x01, // number of data bytes for this tag
        0x01  // Lifecycle state: Creation
    };
    uint16_t new_metadata_len = 5;

    optiga_lib_status = optiga_util_write_metadata(eDEVICE_PUBKEY_CERT_PRJSPC_2, new_metadata_buf, new_metadata_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);
}


static void uc_secure_dfu(void)
{
    optiga_lib_status_t optiga_lib_status;
    ret_code_t          nordic_err_code;

    uint8_t  data[2048] = { 0 };
    uint16_t data_len   = sizeof(data);
    uint16_t oid        = 0xE0EF;

    NRF_LOG_INFO("Secure DFU Example");

    /*
      This functions show the complete flow for a developer who authors firmware images.
      The developer will be abbreviated "dev".
      The commands were exectued using PowerShell on Windows 10 x64.

      **) Generate the developer's private key, using Nordic tools
          $ C:\Python27\Scripts\nrfutil.exe keys generate dev.private.pem

      **) (Optional) Create the developer' corresonding public key, using OpenSSL
          $ C:\OpenSSL-Win64\bin\openssl.exe ec -in .\dev.private.pem -pubout -outform DER -out .\dev.public.der

      **) (Optional) Output the public key as C-array formatted string
          $ C:\Python27\python.exe .\optiga_tools.py -f .\dev.public.der

      **) Create a self-signed developer certificate, using OpenSSL
          $ C:\OpenSSL-Win64\bin\openssl.exe req -x509 -key .\dev.private.pem -extensions v3_req -out dev.cert.der -outform DER -config C:\OpenSSL-Win64\bin\cnf\openssl.cnf -sha256 -subj '/CN=Developer' -days 3650

      **) Output the certificate as C-array formatted string, and put it into the variable 'public_key_cert'
          $ C:\Python27\python.exe .\optiga_tools.py -f .\dev.cert.der
    */

    uint8_t public_key_cert[] =
    {
        0x30, 0x82, 0x01, 0x38, 0x30, 0x81, 0xDF, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00, 0x83,
        0x79, 0xAD, 0xB5, 0xF1, 0x41, 0x3F, 0xC7, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
        0x04, 0x03, 0x02, 0x30, 0x14, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x09,
        0x44, 0x65, 0x76, 0x65, 0x6C, 0x6F, 0x70, 0x65, 0x72, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x38, 0x30,
        0x39, 0x32, 0x31, 0x30, 0x38, 0x32, 0x37, 0x32, 0x33, 0x5A, 0x17, 0x0D, 0x32, 0x38, 0x30, 0x39,
        0x31, 0x38, 0x30, 0x38, 0x32, 0x37, 0x32, 0x33, 0x5A, 0x30, 0x14, 0x31, 0x12, 0x30, 0x10, 0x06,
        0x03, 0x55, 0x04, 0x03, 0x0C, 0x09, 0x44, 0x65, 0x76, 0x65, 0x6C, 0x6F, 0x70, 0x65, 0x72, 0x30,
        0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86,
        0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xB8, 0xBA, 0xE2, 0x7E, 0x50, 0x62,
        0x77, 0xBE, 0xF3, 0xB4, 0xC8, 0x3C, 0x02, 0x23, 0x88, 0x62, 0x6C, 0x19, 0xA2, 0x5F, 0xC2, 0x05,
        0x6D, 0x14, 0xD0, 0x3D, 0x2E, 0x38, 0xBA, 0x79, 0xD2, 0x54, 0x8D, 0x19, 0x2E, 0xBB, 0x39, 0x95,
        0x61, 0x92, 0x4B, 0x39, 0x33, 0x12, 0x75, 0x63, 0x49, 0xE5, 0x49, 0xCF, 0xD5, 0xAC, 0xC6, 0x1F,
        0xF2, 0xD8, 0xFD, 0xA3, 0xF5, 0x58, 0xC9, 0x20, 0x98, 0x8A, 0xA3, 0x1A, 0x30, 0x18, 0x30, 0x09,
        0x06, 0x03, 0x55, 0x1D, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x1D, 0x0F,
        0x04, 0x04, 0x03, 0x02, 0x05, 0xE0, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04,
        0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x21, 0x00, 0xE3, 0xD0, 0xC0, 0x64, 0xC1, 0x6D,
        0x44, 0x34, 0xD2, 0x50, 0x8C, 0x0E, 0xB0, 0x6E, 0x05, 0x86, 0x4A, 0x86, 0x8C, 0x91, 0x66, 0xDB,
        0x80, 0xA2, 0x05, 0xE8, 0x26, 0x41, 0x74, 0x96, 0x6E, 0xB8, 0x02, 0x20, 0x68, 0xE6, 0xB7, 0x17,
        0xF8, 0xEE, 0x25, 0xE0, 0x5A, 0x25, 0x17, 0x60, 0xA7, 0x69, 0x03, 0x08, 0x4E, 0xC9, 0xB4, 0xB1,
        0x9A, 0x15, 0xF3, 0x6F, 0xFF, 0x83, 0x0E, 0xF0, 0xB0, 0x38, 0xA7, 0x4B,
    };

    /*
      The developer compiels a firmware binary, with the content "0xFIRMWARE":

      **) Generate the (fake) firwmare binary
          $ echo "0xFIRMWARE" | Out-File firmware.bin -Encoding ASCII

      **) Convert the file into C-array formatted string, and put it into the variable 'firmware_binary'
          $ C:\Python27\python.exe .\optiga_tools.py -f .\firmware.bin

      **) Sign the firmware, using OpenSSL, and the developer' private key
          $ C:\OpenSSL-Win64\bin\openssl.exe dgst -sha256 -sign .\dev.private.pem -out firmware.sig.der firmware.bin

      **) Convert the DER-coded file into C-array formatted string, and put it into the variable 'firmware_binary'
          $ C:\Python27\python.exe .\optiga_tools.py -f .\firmware.sig.der
    */

    uint8_t firmware_binary[] = { // == "0xFIRMWARE"
        0x30, 0x78, 0x46, 0x49, 0x52, 0x4D, 0x57, 0x41, 0x52, 0x45, 0x0D, 0x0A,
     };
    uint8_t sig_asn[] = {
        0x30, 0x45, 0x02, 0x20, 0x08, 0xE8, 0xCA, 0x0E, 0xF1, 0xA0, 0x31, 0x7F, 0x74, 0x07, 0xDA, 0x69,
        0xF7, 0xF6, 0x0F, 0xAB, 0x5C, 0x60, 0x95, 0xCF, 0x64, 0xE0, 0xAB, 0x97, 0x0D, 0x21, 0xF9, 0x7C,
        0x69, 0x83, 0xE3, 0xD6, 0x02, 0x21, 0x00, 0xB0, 0xF8, 0x1D, 0x69, 0x1F, 0x66, 0xF5, 0x50, 0xE5,
        0x54, 0xAC, 0x61, 0xD0, 0x4A, 0x73, 0xAD, 0xA5, 0x9B, 0x92, 0x85, 0xE0, 0xA5, 0x31, 0xFF, 0x45,
        0x28, 0x8C, 0x3C, 0xBB, 0xFC, 0x60, 0xF6,
    };

    // Convert signature format
    uint8_t   sig_rs[100] = { 0 };
    size_t    sig_rs_len  = sizeof(sig_rs);
    uint8_t * p_sig_asn   = &sig_asn[2];
    size_t    sig_asn_len = sizeof(sig_asn) - 2;
    //lint -save -e662 (Suspicious cast, Suspicious use of &)
    nordic_err_code = asn1_to_ecdsa_rs(p_sig_asn, sig_asn_len, sig_rs, &sig_rs_len);
    DEMO_ERROR_CHECK(nordic_err_code);

    optiga_lib_status = optiga_util_write_data(oid, OPTIGA_UTIL_ERASE_AND_WRITE, 0, public_key_cert, sizeof(public_key_cert));
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);

    optiga_lib_status = optiga_util_read_data(oid, 0, data, &data_len);
    DEMO_OPTIGA_ERROR_CHECK(optiga_lib_status);
    NRF_LOG_INFO("Current certificate in %4x:", oid);
    pr_hex(data, data_len);

    // Hash the msg data using the nrf_crypto API
    uint8_t                   hash[NRF_CRYPTO_HASH_SIZE_SHA256] = {0};
    size_t                    hash_len                          = NRF_CRYPTO_HASH_SIZE_SHA256;
    nrf_crypto_hash_context_t hash_ctx;
    nordic_err_code = nrf_crypto_hash_calculate(
        &hash_ctx,
        &g_nrf_crypto_hash_sha256_info,
        firmware_binary, sizeof(firmware_binary),
        hash, &hash_len);
    DEMO_ERROR_CHECK(nordic_err_code);
    NRF_LOG_INFO("SHA256 of firmware_binary:");
    pr_hex(hash, hash_len);

    // ### VERIFY VIA PUBLIC KEY CERTIFICATE IN OID ###
    // Verify using public contained in certificate in oid slot
    nrf_crypto_ecc_public_key_t pub_key = NRF_CRYPTO_INFINEON_SECP256R1_PUBLIC_KEY_FROM_OID(oid);
    nordic_err_code = nrf_crypto_ecdsa_verify(NULL, &pub_key, hash, hash_len, sig_rs, sig_rs_len);
    DEMO_ERROR_CHECK(nordic_err_code);
    NRF_LOG_INFO("Verify (OID): %s", nordic_err_code == NRF_SUCCESS ? "PASS" : "FAIL");


    // ### VERIFY VIA EXTERNAL PUBLIC KEY ###
    // Verify using public key supplied by host

    /*
      **) Create the developer' corresonding public key, using OpenSSL
          $ C:\OpenSSL-Win64\bin\openssl.exe ec -in .\dev.private.pem -pubout -outform DER -out .\dev.public.der

      **) Output the public key as C-array formatted string, put it into variable 'pub_key_raw'
          $ C:\Python27\python.exe .\optiga_tools.py -f .\dev.public.der
    */

    const uint8_t pub_key_raw[] = {
        0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
        0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04,
        // here come the relevant R and S components, 2*32B) (offset=27, hardcoded below)
        0xB8, 0xBA, 0xE2, 0x7E, 0x50, 0x62, 0x77, 0xBE, 0xF3, 0xB4, 0xC8, 0x3C, 0x02, 0x23, 0x88, 0x62,
        0x6C, 0x19, 0xA2, 0x5F, 0xC2, 0x05, 0x6D, 0x14, 0xD0, 0x3D, 0x2E, 0x38, 0xBA, 0x79, 0xD2, 0x54,
        0x8D, 0x19, 0x2E, 0xBB, 0x39, 0x95, 0x61, 0x92, 0x4B, 0x39, 0x33, 0x12, 0x75, 0x63, 0x49, 0xE5,
        0x49, 0xCF, 0xD5, 0xAC, 0xC6, 0x1F, 0xF2, 0xD8, 0xFD, 0xA3, 0xF5, 0x58, 0xC9, 0x20, 0x98, 0x8A,
    };

    nrf_crypto_ecc_public_key_t pub_key2 = NRF_CRYPTO_INFINEON_SECP256R1_PUBLIC_KEY_RAW;
    nordic_err_code = nrf_crypto_ecc_public_key_from_raw(&g_nrf_crypto_ecc_secp256r1_curve_info, &pub_key2,
                                                  &pub_key_raw[27], // hardcoded offset
                                                  NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE);
    DEMO_ERROR_CHECK(nordic_err_code);
    nordic_err_code = nrf_crypto_ecdsa_verify(NULL, &pub_key2, hash, hash_len, sig_rs, sig_rs_len);
    DEMO_ERROR_CHECK(nordic_err_code);
    NRF_LOG_INFO("Verify (EXT): %s", nordic_err_code == NRF_SUCCESS ? "PASS" : "FAIL");

    NRF_LOG_INFO("Secure DFU Example done.\r\n");
}

// generate a ECC key pair on the OPTIGA and export private and public key to the host
static void uc_gen_keypair_export_private()
{
    ret_code_t nordic_err_code;

    NRF_LOG_INFO("Generating ECC keypair");

    // export private key to host
    nrf_crypto_ecc_private_key_t priv_key = NRF_CRYPTO_INFINEON_SECP256R1_PRIVATE_KEY_FROM_OID(NRF_CRYPTO_INFINEON_PRIVKEY_HOST_OID);
    nrf_crypto_ecc_public_key_t pub_key;
    nordic_err_code = nrf_crypto_ecc_key_pair_generate(NULL, &g_nrf_crypto_ecc_secp256r1_curve_info, &priv_key, &pub_key);
    DEMO_ERROR_CHECK(nordic_err_code);

    // print private key
    uint8_t priv_raw[NRF_CRYPTO_ECC_SECP256R1_RAW_PRIVATE_KEY_SIZE] = {0};
    size_t priv_raw_len = NRF_CRYPTO_ECC_SECP256R1_RAW_PRIVATE_KEY_SIZE;
    nordic_err_code = nrf_crypto_ecc_private_key_to_raw(&priv_key, priv_raw, &priv_raw_len);
    DEMO_ERROR_CHECK(nordic_err_code);

    NRF_LOG_INFO("Private Key:");
    NRF_LOG_HEXDUMP_INFO(priv_raw, priv_raw_len);

    // print public key
    uint8_t publ_raw[NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE] = {0};
    size_t publ_raw_len = NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE;
    nordic_err_code = nrf_crypto_ecc_public_key_to_raw(&pub_key, publ_raw, &publ_raw_len);
    DEMO_ERROR_CHECK(nordic_err_code);

    NRF_LOG_INFO("Public Key:");
    NRF_LOG_HEXDUMP_INFO(publ_raw, publ_raw_len);
}

// generate a ECC key pair on the OPTIGA and export private and public key to the host
static void uc_gen_keypair_export_public()
{
    ret_code_t nordic_err_code;

    NRF_LOG_INFO("Generating ECC keypair");

    // export private key to host
    nrf_crypto_ecc_private_key_t priv_key = { 0 };
    nrf_crypto_ecc_public_key_t pub_key;
    nordic_err_code = nrf_crypto_ecc_key_pair_generate(NULL, &g_nrf_crypto_ecc_secp256r1_curve_info, &priv_key, &pub_key);
    DEMO_ERROR_CHECK(nordic_err_code);

    // print public key
    uint8_t publ_raw[NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE] = {0};
    size_t publ_raw_len = NRF_CRYPTO_ECC_SECP256R1_RAW_PUBLIC_KEY_SIZE;
    nordic_err_code = nrf_crypto_ecc_public_key_to_raw(&pub_key, publ_raw, &publ_raw_len);
    DEMO_ERROR_CHECK(nordic_err_code);

    NRF_LOG_INFO("Public Key:");
    NRF_LOG_HEXDUMP_INFO(publ_raw, publ_raw_len);
}

// generate two ECC key pairs and perform a ECDH operation between them
static void uc_ecdh()
{
#define SESSION_CTX1_OID 0xE100
#define SESSION_CTX2_OID 0xE101

    ret_code_t nordic_err_code;

    NRF_LOG_INFO("Generating shared secrets using OPTIGA");

    // generate keypair 1 in session context 1
    nrf_crypto_ecc_private_key_t priv_key1 = NRF_CRYPTO_INFINEON_SECP256R1_PRIVATE_KEY_FROM_OID(SESSION_CTX1_OID);
    nrf_crypto_ecc_public_key_t pub_key1;
    nordic_err_code = nrf_crypto_ecc_key_pair_generate(NULL, &g_nrf_crypto_ecc_secp256r1_curve_info, &priv_key1, &pub_key1);
    DEMO_ERROR_CHECK(nordic_err_code);

    // generate keypair 2 in session context 2
    nrf_crypto_ecc_private_key_t priv_key2 = NRF_CRYPTO_INFINEON_SECP256R1_PRIVATE_KEY_FROM_OID(SESSION_CTX2_OID);
    nrf_crypto_ecc_public_key_t pub_key2;
    nordic_err_code = nrf_crypto_ecc_key_pair_generate(NULL, &g_nrf_crypto_ecc_secp256r1_curve_info, &priv_key2, &pub_key2);
    DEMO_ERROR_CHECK(nordic_err_code);

    // do ECDH with pub key 1 and priv key 2
    uint8_t shared_secret1[NRF_CRYPTO_ECDH_SECP256R1_SHARED_SECRET_SIZE] = {0};
    size_t shared_secret1_len = NRF_CRYPTO_ECDH_SECP256R1_SHARED_SECRET_SIZE;

    nordic_err_code = nrf_crypto_ecdh_compute(NULL, &priv_key2, &pub_key1, shared_secret1, &shared_secret1_len);
    DEMO_ERROR_CHECK(nordic_err_code);

    // do ECDH with pub key 2 and priv key 1
    uint8_t shared_secret2[NRF_CRYPTO_ECDH_SECP256R1_SHARED_SECRET_SIZE] = {0};
    size_t shared_secret2_len = NRF_CRYPTO_ECDH_SECP256R1_SHARED_SECRET_SIZE;

    nordic_err_code = nrf_crypto_ecdh_compute(NULL, &priv_key1, &pub_key2, shared_secret2, &shared_secret2_len);
    DEMO_ERROR_CHECK(nordic_err_code);

    NRF_LOG_INFO("Shared secret 1:");
    NRF_LOG_HEXDUMP_INFO(shared_secret1, shared_secret1_len);

    if(shared_secret1_len == shared_secret2_len &&
       memcmp(shared_secret1, shared_secret2, shared_secret1_len) == 0) {
        NRF_LOG_INFO("Shared secrets match");
    } else {
        NRF_LOG_INFO("Shared secret 2:");
        NRF_LOG_HEXDUMP_INFO(shared_secret2, shared_secret2_len);
        NRF_LOG_INFO("Error: shared secrets don't match");
        DEMO_ERROR_CHECK(NRF_ERROR_INTERNAL);
    }
#undef SESSION_CTX1_OID
#undef SESSION_CTX2_OID
}


int main(void)
{
    ret_code_t err_code = NRF_SUCCESS;

    log_init();

    NRF_LOG_INFO("Infineon OPTIGA(TM) Trust X example started\r\n");

    // Start internal LFCLK XTAL oscillator, needed for RTC and OPTIGA
    err_code = nrfx_clock_init(clock_event_handler);
    APP_ERROR_CHECK(err_code);
    nrfx_clock_lfclk_start();

    NRF_LOG_INFO("Clock init finish\r\n");

    NRF_LOG_FLUSH();

    // Init nrf_crypto, which initializes the OPTIGA backend, and the OPTIGA host library/stack
    err_code = nrf_crypto_init();
    DEMO_ERROR_CHECK(err_code);

    uc_print_coprocessor_information();

    NRF_LOG_INFO("Printing coprocessor information finished\r\n");

    uc_gen_keypair_export_public();

    NRF_LOG_INFO("Keypair export public finished\r\n");

    uc_gen_keypair_export_private();

    NRF_LOG_INFO("Keypair export private finished\r\n");

    uc_ecdh();

    NRF_LOG_INFO("ECDH finished\r\n");

    uc_read_metadata();

    NRF_LOG_INFO("Read metadata finished\r\n");
    NRF_LOG_FLUSH();

    uc_write_metadata();

    NRF_LOG_INFO("Write metadata finished\r\n");
    NRF_LOG_FLUSH();

    uc_hash_cert();

    NRF_LOG_INFO("Hash certificate finished\r\n");
    NRF_LOG_FLUSH();

    uc_random_number_generation();

    NRF_LOG_INFO("RNG finished\r\n");
    NRF_LOG_FLUSH();

    uc_read_write_certificate();

    NRF_LOG_INFO("Certificate write finished\r\n");
    NRF_LOG_FLUSH();

    size_t sig_len = NRF_CRYPTO_ECDSA_SECP256R1_SIGNATURE_SIZE;
    uint8_t sig[NRF_CRYPTO_ECDSA_SECP256R1_SIGNATURE_SIZE] = {0};

    uc_sign(sig, &sig_len);

    NRF_LOG_INFO("Sign finished\r\n");
    NRF_LOG_FLUSH();

    uc_verify(sig, sig_len);

    NRF_LOG_INFO("Verify finished");
    NRF_LOG_FLUSH();

    uc_verify_ext();

    NRF_LOG_INFO("Verify ext finished\r\n");
    NRF_LOG_FLUSH();

    uc_secure_dfu();
    NRF_LOG_INFO("Secure DFU finished\r\n");
    NRF_LOG_FLUSH();

    uc_key_derivation();
    NRF_LOG_INFO("Key derivation finished\r\n");
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("Infineon OPTIGA(TM) Trust X example executed successfully.");
    NRF_LOG_FLUSH();

    for (;;)
    {
    }
}
