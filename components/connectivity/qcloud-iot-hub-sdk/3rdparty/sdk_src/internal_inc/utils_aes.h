/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef QCLOUD_IOT_UTILS_AES_H_
#define QCLOUD_IOT_UTILS_AES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

//========Platform================================//
#define UTILS_AES_C
#define UTILS_CIPHER_MODE_CBC
//#define UTILS_SELF_TEST

#define UTILS_ERR_PLATFORM_HW_ACCEL_FAILED     -0x0070 /**< Hardware accelerator failed */
#define UTILS_ERR_PLATFORM_FEATURE_UNSUPPORTED -0x0072 /**< The requested feature is not supported by the platform */

/* Internal macros meant to be called only from within the library. */
#define UTILS_INTERNAL_VALIDATE_RET(cond, ret) \
    do {                                       \
    } while (0)
#define UTILS_INTERNAL_VALIDATE(cond) \
    do {                              \
    } while (0)
//==============================================//

/* padlock.c and aesni.c rely on these values! */
#define UTILS_AES_ENCRYPT 1 /**< AES encryption. */
#define UTILS_AES_DECRYPT 0 /**< AES decryption. */

#define UTILS_AES_BLOCK_LEN 16
#define AES_KEY_BITS_128    128
#define AES_KEY_BITS_192    192
#define AES_KEY_BITS_256    256

/* Error codes in range 0x0020-0x0022 */
#define UTILS_ERR_AES_INVALID_KEY_LENGTH   -0x0020 /**< Invalid key length. */
#define UTILS_ERR_AES_INVALID_INPUT_LENGTH -0x0022 /**< Invalid data input length. */

/* Error codes in range 0x0021-0x0025 */
#define UTILS_ERR_AES_BAD_INPUT_DATA -0x0021 /**< Invalid input data. */

/* UTILS_ERR_AES_FEATURE_UNAVAILABLE is deprecated and should not be used. */
#define UTILS_ERR_AES_FEATURE_UNAVAILABLE \
    -0x0023 /**< Feature not available. For example, an unsupported AES key size. */

/* UTILS_ERR_AES_HW_ACCEL_FAILED is deprecated and should not be used. */
#define UTILS_ERR_AES_HW_ACCEL_FAILED -0x0025 /**< AES hardware accelerator failed. */

#if !defined(UTILS_AES_ALT)
// Regular implementation
//

/**
 * \brief The AES context-type definition.
 */
typedef struct utils_aes_context {
    int       nr;      /*!< The number of rounds. */
    uint32_t *rk;      /*!< AES round keys. */
    uint32_t  buf[68]; /*!< Unaligned data buffer. This buffer can
                            hold 32 extra Bytes, which can be used for
                            one of the following purposes:
                            <ul><li>Alignment if VIA padlock is
                                    used.</li>
                            <li>Simplifying key expansion in the 256-bit
                                case by generating an extra round key.
                                </li></ul> */
} utils_aes_context;

#else /* UTILS_AES_ALT */
#include "aes_alt.h"
#endif /* UTILS_AES_ALT */

/**
 * \brief          This function initializes the specified AES context.
 *
 *                 It must be the first API called before using
 *                 the context.
 *
 * \param ctx      The AES context to initialize. This must not be \c NULL.
 */
void utils_aes_init(utils_aes_context *ctx);

/**
 * \brief          This function releases and clears the specified AES context.
 *
 * \param ctx      The AES context to clear.
 *                 If this is \c NULL, this function does nothing.
 *                 Otherwise, the context must have been at least initialized.
 */
void utils_aes_free(utils_aes_context *ctx);

/**
 * \brief          This function sets the encryption key.
 *
 * \param ctx      The AES context to which the key should be bound.
 *                 It must be initialized.
 * \param key      The encryption key.
 *                 This must be a readable buffer of size \p keybits bits.
 * \param keybits  The size of data passed in bits. Valid options are:
 *                 <ul><li>128 bits</li>
 *                 <li>192 bits</li>
 *                 <li>256 bits</li></ul>
 *
 * \return         \c 0 on success.
 * \return         #UTILS_ERR_AES_INVALID_KEY_LENGTH on failure.
 */
int utils_aes_setkey_enc(utils_aes_context *ctx, const unsigned char *key, unsigned int keybits);

/**
 * \brief          This function sets the decryption key.
 *
 * \param ctx      The AES context to which the key should be bound.
 *                 It must be initialized.
 * \param key      The decryption key.
 *                 This must be a readable buffer of size \p keybits bits.
 * \param keybits  The size of data passed. Valid options are:
 *                 <ul><li>128 bits</li>
 *                 <li>192 bits</li>
 *                 <li>256 bits</li></ul>
 *
 * \return         \c 0 on success.
 * \return         #UTILS_ERR_AES_INVALID_KEY_LENGTH on failure.
 */
int utils_aes_setkey_dec(utils_aes_context *ctx, const unsigned char *key, unsigned int keybits);

/**
 * \brief          This function performs an AES single-block encryption or
 *                 decryption operation.
 *
 *                 It performs the operation defined in the \p mode parameter
 *                 (encrypt or decrypt), on the input data buffer defined in
 *                 the \p input parameter.
 *
 *                 utils_aes_init(), and either utils_aes_setkey_enc() or
 *                 utils_aes_setkey_dec() must be called before the first
 *                 call to this API with the same context.
 *
 * \param ctx      The AES context to use for encryption or decryption.
 *                 It must be initialized and bound to a key.
 * \param mode     The AES operation: #UTILS_AES_ENCRYPT or
 *                 #UTILS_AES_DECRYPT.
 * \param input    The buffer holding the input data.
 *                 It must be readable and at least \c 16 Bytes long.
 * \param output   The buffer where the output data will be written.
 *                 It must be writeable and at least \c 16 Bytes long.

 * \return         \c 0 on success.
 */
int utils_aes_crypt_ecb(utils_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16]);

#if defined(UTILS_CIPHER_MODE_CBC)
/**
 * \brief  This function performs an AES-CBC encryption or decryption operation
 *         on full blocks.
 *
 *         It performs the operation defined in the \p mode
 *         parameter (encrypt/decrypt), on the input data buffer defined in
 *         the \p input parameter.
 *
 *         It can be called as many times as needed, until all the input
 *         data is processed. utils_aes_init(), and either
 *         utils_aes_setkey_enc() or utils_aes_setkey_dec() must be called
 *         before the first call to this API with the same context.
 *
 * \note   This function operates on full blocks, that is, the input size
 *         must be a multiple of the AES block size of \c 16 Bytes.
 *
 * \note   Upon exit, the content of the IV is updated so that you can
 *         call the same function again on the next
 *         block(s) of data and get the same result as if it was
 *         encrypted in one call. This allows a "streaming" usage.
 *         If you need to retain the contents of the IV, you should
 *         either save it manually or use the cipher module instead.
 *
 *
 * \param ctx      The AES context to use for encryption or decryption.
 *                 It must be initialized and bound to a key.
 * \param mode     The AES operation: #UTILS_AES_ENCRYPT or
 *                 #UTILS_AES_DECRYPT.
 * \param length   The length of the input data in Bytes. This must be a
 *                 multiple of the block size (\c 16 Bytes).
 * \param iv       Initialization vector (updated after use).
 *                 It must be a readable and writeable buffer of \c 16 Bytes.
 * \param input    The buffer holding the input data.
 *                 It must be readable and of size \p length Bytes.
 * \param output   The buffer holding the output data.
 *                 It must be writeable and of size \p length Bytes.
 *
 * \return         \c 0 on success.
 * \return         #UTILS_ERR_AES_INVALID_INPUT_LENGTH
 *                 on failure.
 */
int utils_aes_crypt_cbc(utils_aes_context *ctx, int mode, size_t length, unsigned char iv[16],
                        const unsigned char *input, unsigned char *output);
#endif /* UTILS_CIPHER_MODE_CBC */

/**
 * \brief           Internal AES block encryption function. This is only
 *                  exposed to allow overriding it using
 *                  \c UTILS_AES_ENCRYPT_ALT.
 *
 * \param ctx       The AES context to use for encryption.
 * \param input     The plaintext block.
 * \param output    The output (ciphertext) block.
 *
 * \return          \c 0 on success.
 */
int utils_internal_aes_encrypt(utils_aes_context *ctx, const unsigned char input[16], unsigned char output[16]);

/**
 * \brief           Internal AES block decryption function. This is only
 *                  exposed to allow overriding it using see
 *                  \c UTILS_AES_DECRYPT_ALT.
 *
 * \param ctx       The AES context to use for decryption.
 * \param input     The ciphertext block.
 * \param output    The output (plaintext) block.
 *
 * \return          \c 0 on success.
 */
int utils_internal_aes_decrypt(utils_aes_context *ctx, const unsigned char input[16], unsigned char output[16]);

#if !defined(UTILS_DEPRECATED_REMOVED)
#if defined(UTILS_DEPRECATED_WARNING)
#define UTILS_DEPRECATED __attribute__((deprecated))
#else
#define UTILS_DEPRECATED
#endif
/**
 * \brief           Deprecated internal AES block encryption function
 *                  without return value.
 *
 * \deprecated      Superseded by utils_internal_aes_encrypt()
 *
 * \param ctx       The AES context to use for encryption.
 * \param input     Plaintext block.
 * \param output    Output (ciphertext) block.
 */
UTILS_DEPRECATED void utils_aes_encrypt(utils_aes_context *ctx, const unsigned char input[16],
                                        unsigned char output[16]);

/**
 * \brief           Deprecated internal AES block decryption function
 *                  without return value.
 *
 * \deprecated      Superseded by utils_internal_aes_decrypt()
 *
 * \param ctx       The AES context to use for decryption.
 * \param input     Ciphertext block.
 * \param output    Output (plaintext) block.
 */
UTILS_DEPRECATED void utils_aes_decrypt(utils_aes_context *ctx, const unsigned char input[16],
                                        unsigned char output[16]);

#undef UTILS_DEPRECATED
#endif /* !UTILS_DEPRECATED_REMOVED */

#if defined(UTILS_SELF_TEST)
/**
 * \brief          Checkup routine.
 *
 * \return         \c 0 on success.
 * \return         \c 1 on failure.
 */
int utils_aes_self_test(int verbose);

#endif /* UTILS_SELF_TEST */

int aes_sample(int verbose);

int utils_aes_cbc(uint8_t *pInData, uint32_t datalen, uint8_t *pOutData, uint32_t outBuffLen, uint8_t mode,
                  uint8_t *pKey, uint16_t keybits, uint8_t *iv);

#ifdef __cplusplus
}
#endif
#endif
