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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_error.h"
#include "app_util.h"
#include "nrf_section.h"
#include "nrf_crypto.h"
#include "common_test.h"

/**@brief ECDSA test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Component-Testing
 */

/*lint -save -e91 */

#if NRF_CRYPTO_ECC_ENABLED && NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_OPTIGA)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R2)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R2)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192R1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256R1)

// ECDSA random - NIST P-256, SHA-256 - first test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_random_data, test_vector_ecdsa_random_t test_vector_ecdsa_random_secp256r1_sha256_1) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .p_test_vector_name         = "secp256r1 random sha256 1",
    .p_input                    = "44acf6b7e36c1342c2c5897204fe09504e1e2efb1a900377dbc4e7a6a133ec56",
    .sig_len                    = NRF_CRYPTO_ECDSA_SECP256R1_SIGNATURE_SIZE,
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP384R1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP384R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP521R1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP521R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256K1)

#endif // NRF_CRYPTO_ECC_ENABLED && !NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_CC310_BL)

/*lint -restore */
