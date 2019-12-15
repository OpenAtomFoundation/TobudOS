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
/**
 * @defgroup nrf_crypto_ecdh_example
 * @{
 * @ingroup nrf_crypto_ecdh
 * @brief ECDH Example Application main file.
 *
 * This file contains the source code for a sample application that demonstrates using the
 * nrf_crypto library to do ECHD computation. Different backends can be used by adjusting
 * @ref sdk_config.h accordingly.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "sdk_common.h"
#include "nrf_assert.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_crypto.h"
#include "nrf_crypto_ecc.h"
#include "nrf_crypto_ecdh.h"
#include "nrf_crypto_error.h"
#include "mem_manager.h"


//======================================== Print functions ========================================
//
// This part of an example contains helper functions to print results generated in this examples.
//


static void print_array(uint8_t const * p_string, size_t size)
{
    #if NRF_LOG_ENABLED
    size_t i;
    NRF_LOG_RAW_INFO("    ");
    for(i = 0; i < size; i++)
    {
        NRF_LOG_RAW_INFO("%02x", p_string[i]);
    }
    #endif // NRF_LOG_ENABLED
}


static void print_hex(char const * p_msg, uint8_t const * p_data, size_t size)
{
    NRF_LOG_INFO(p_msg);
    print_array(p_data, size);
    NRF_LOG_RAW_INFO("\r\n");
}


#define DEMO_ERROR_CHECK(error)     \
do                                  \
{                                   \
    if (error != NRF_SUCCESS)       \
    {                               \
        NRF_LOG_ERROR("Error 0x%04X: %s", error, nrf_crypto_error_string_get(error));\
        APP_ERROR_CHECK(error);     \
    }                               \
} while(0)


//==================================== Communication functions ====================================
//
// This part of an example contains helper functions to demonstrate communication between
// Alice and Bob.
//


static uint8_t m_message_for_alice[256]; ///< @brief Message buffer from Bob to Alice
static size_t  m_message_for_alice_size; ///< @brief Message buffer size from Bob to Alice
static uint8_t m_message_for_bob[256];   ///< @brief Message buffer from Alice to Bob
static size_t  m_message_for_bob_size;   ///< @brief Message buffer size from Alice to Bob


/** @brief Function to demonstrate message send to Alice.
 */
static void send_to_alice(const uint8_t * p_data, size_t size)
{
    print_hex("Message with public key from Bob to Alice: ", p_data, size);
    memcpy(m_message_for_alice, p_data, size);
    m_message_for_alice_size = size;
}


/** @brief Function to demonstrate message send to Bob.
 */
static void send_to_bob(const uint8_t * p_data, size_t size)
{
    print_hex("Message with public key from Alice to Bob: ", p_data, size);
    memcpy(m_message_for_bob, p_data, size);
    m_message_for_bob_size = size;
}


/** @brief Function to demonstrate message receive from Alice.
 */
static size_t recv_from_alice(uint8_t * p_data)
{
    memcpy(p_data, m_message_for_bob, m_message_for_bob_size);
    return m_message_for_bob_size;
}


/** @brief Function to demonstrate message receive from Bob.
 */
static size_t recv_from_bob(uint8_t * p_data)
{
    memcpy(p_data, m_message_for_alice, m_message_for_alice_size);
    return m_message_for_alice_size;
}


//========================================= Alice's site =========================================
//
// This part of an example contains implementation of Alice's site. Alice have public and private
// key predefined. She is using always the same key pair for every secure connection. Code is
// divided into two steps - before and after public key exchange with Bob.
//


/** @brief Predefined example private key
 *
 *  This private key contains some dummy data just to show the functionality. Is should never be
 *  placed in any practical usage. Is is not secure, because it is filled with ones (in HEX).
 */
static const uint8_t m_alice_raw_private_key[] =
{
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,  // This is an example. DO NOT USE THIS KEY!
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,  // This is an example. DO NOT USE THIS KEY!
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,  // This is an example. DO NOT USE THIS KEY!
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,  // This is an example. DO NOT USE THIS KEY!
};


/** @brief Predefined example public key that is associated with example private key
 *  @ref alice_raw_private_key.
 */
static const uint8_t m_alice_raw_public_key[] =
{
    0x02, 0x17, 0xE6, 0x17, 0xF0, 0xB6, 0x44, 0x39,
    0x28, 0x27, 0x8F, 0x96, 0x99, 0x9E, 0x69, 0xA2,
    0x3A, 0x4F, 0x2C, 0x15, 0x2B, 0xDF, 0x6D, 0x6C,
    0xDF, 0x66, 0xE5, 0xB8, 0x02, 0x82, 0xD4, 0xED,
    0x19, 0x4A, 0x7D, 0xEB, 0xCB, 0x97, 0x71, 0x2D,
    0x2D, 0xDA, 0x3C, 0xA8, 0x5A, 0xA8, 0x76, 0x5A,
    0x56, 0xF4, 0x5F, 0xC7, 0x58, 0x59, 0x96, 0x52,
    0xF2, 0x89, 0x7C, 0x65, 0x30, 0x6E, 0x57, 0x94,
};

static nrf_crypto_ecdh_secp256r1_shared_secret_t m_shared_secret_bob;
static nrf_crypto_ecdh_secp256r1_shared_secret_t m_shared_secret_alice;

/** @brief First step of Alice's work. Ended with send public key to Bob.
 */
static void alice_step_1()
{
    NRF_LOG_INFO("Alice's prepare step");

    // Alice have predefined public key, so she just sends it to Bob
    send_to_bob(m_alice_raw_public_key, sizeof(m_alice_raw_public_key));
}


/** @brief Second step of Alice's work. At the end Alice will have shared secret the same as
 *  Bob has.
 */
static void alice_step_2()
{
    static nrf_crypto_ecc_private_key_t              alice_private_key;
    static nrf_crypto_ecc_public_key_t               bob_public_key;
    static nrf_crypto_ecc_secp256r1_raw_public_key_t raw_key_buffer;
    ret_code_t                                       err_code = NRF_SUCCESS;
    size_t                                           size;

    NRF_LOG_INFO("Alice's compute step");

    // Alice receives Bob's raw public key
    size = recv_from_bob(raw_key_buffer);

    // Alice converts Bob's raw public key to internal representation
    err_code = nrf_crypto_ecc_public_key_from_raw(&g_nrf_crypto_ecc_secp256r1_curve_info,
                                                  &bob_public_key,
                                                  raw_key_buffer, size);
    DEMO_ERROR_CHECK(err_code);

    // Alice converts her raw private key to internal representation
    err_code = nrf_crypto_ecc_private_key_from_raw(&g_nrf_crypto_ecc_secp256r1_curve_info,
                                                   &alice_private_key,
                                                   m_alice_raw_private_key,
                                                   sizeof(m_alice_raw_private_key));
    DEMO_ERROR_CHECK(err_code);

    // Alice computes shared secret using ECDH
    size = sizeof(m_shared_secret_alice);
    err_code = nrf_crypto_ecdh_compute(NULL,
                                       &alice_private_key,
                                       &bob_public_key,
                                       m_shared_secret_alice,
                                       &size);
    DEMO_ERROR_CHECK(err_code);

    // Alice can now use shared secret
    print_hex("Alice's shared secret: ", m_shared_secret_alice, size);

    // Key deallocation
    err_code = nrf_crypto_ecc_private_key_free(&alice_private_key);
    DEMO_ERROR_CHECK(err_code);
    err_code = nrf_crypto_ecc_public_key_free(&bob_public_key);
    DEMO_ERROR_CHECK(err_code);
}


//========================================== Bob's site ==========================================
//
// This part of an example contains implementation of Bobs's site. Bob do not have public and
// private key predefined. He always generates a new key pair for each connection. Code is divided
// into two steps - before and after public key exchange with Alice.
//


/** @brief Bob must keep his private key between step 1 and step 2.
 */
static nrf_crypto_ecc_private_key_t m_bob_private_key;


/** @brief First step of Bob's work. Ended with send public key to Alice.
 */
void bob_step_1()
{
    static nrf_crypto_ecc_public_key_t               bob_public_key;
    static nrf_crypto_ecc_secp256r1_raw_public_key_t raw_key_buffer;
    ret_code_t                                       err_code = NRF_SUCCESS;
    size_t                                           size;

    NRF_LOG_INFO("Bob's prepare step");

    // Bob creates new key pair each time
    err_code = nrf_crypto_ecc_key_pair_generate(NULL,
                                                &g_nrf_crypto_ecc_secp256r1_curve_info,
                                                &m_bob_private_key,
                                                &bob_public_key);
    DEMO_ERROR_CHECK(err_code);

    // Bob converts public key to raw representation
    size = sizeof(raw_key_buffer);
    err_code = nrf_crypto_ecc_public_key_to_raw(&bob_public_key,
                                                raw_key_buffer,
                                                &size);
    DEMO_ERROR_CHECK(err_code);

    // Bob sends raw public key to Alice
    send_to_alice(raw_key_buffer, size);

    // Key deallocation
    err_code = nrf_crypto_ecc_public_key_free(&bob_public_key);
    DEMO_ERROR_CHECK(err_code);
}


/** @brief Second step of Bob's work. At the end Bob will have shared secret the same as Alice has.
 */
void bob_step_2()
{
    static nrf_crypto_ecc_public_key_t               alice_public_key;
    static nrf_crypto_ecc_secp256r1_raw_public_key_t raw_key_buffer;

    ret_code_t                                       err_code = NRF_SUCCESS;
    size_t                                           size;

    NRF_LOG_INFO("Bob's compute step");

    // Bob receives Alice's raw public key
    size = recv_from_alice(raw_key_buffer);

    // Bob converts Alice's raw public key to internal representation
    err_code = nrf_crypto_ecc_public_key_from_raw(&g_nrf_crypto_ecc_secp256r1_curve_info,
                                                  &alice_public_key,
                                                  raw_key_buffer,
                                                  size);
    DEMO_ERROR_CHECK(err_code);

    // Bob computes shared secret using ECDH
    size = sizeof(m_shared_secret_bob);
    err_code = nrf_crypto_ecdh_compute(NULL,
                                       &m_bob_private_key,
                                       &alice_public_key,
                                       m_shared_secret_bob,
                                       &size);
    DEMO_ERROR_CHECK(err_code);

    // Bob can now use shared secret
    print_hex("Bob's shared secret:   ", m_shared_secret_bob, size);

    // Key deallocation
    err_code = nrf_crypto_ecc_private_key_free(&m_bob_private_key);
    DEMO_ERROR_CHECK(err_code);
    err_code = nrf_crypto_ecc_public_key_free(&alice_public_key);
    DEMO_ERROR_CHECK(err_code);
}


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


/** @brief Function for application main entry.
 */
int main(void)
{
    ret_code_t err_code = NRF_SUCCESS;

    log_init();

    NRF_LOG_INFO("ECDH example started.\r\n");

    err_code = nrf_mem_init();
    DEMO_ERROR_CHECK(err_code);

    err_code = nrf_crypto_init();
    DEMO_ERROR_CHECK(err_code);

    alice_step_1();  // Alice sends her public key to Bob
    bob_step_1();    // Bob generates new key pair and sends his public key to Alice
    alice_step_2();  // Alice receives Bob's public key and computes shared secret
    bob_step_2();    // Bob receives Alice's public key and computes shared secret

    if (memcmp(m_shared_secret_alice,
               m_shared_secret_bob,
               sizeof(nrf_crypto_ecdh_secp256r1_shared_secret_t)) == 0)
    {
        NRF_LOG_INFO("ECDH example executed successfully.");
    }
    else
    {
        NRF_LOG_ERROR("ECDH example failed!!!");
    }

    for (;;)
    {
    }
}


/** @}
 */
