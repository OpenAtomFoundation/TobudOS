/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \file
*
* \brief This file implements the prototype declarations of OPTIGA Crypt.
*
* \defgroup  grOptigaCrypt OPTIGA Crypt API
* \ingroup infineon_api
* @{
*/
#ifndef _OPTIGA_CRYPT_H_
#define _OPTIGA_CRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/cmd/CommandLib.h"

/**
 * \brief  Typedef for Key IDs
 *         The KEY_STORE_ID_xxx holds only private key
 *
 *         The SESSION_ID_xxx can hold private key, premaster secret(ECDH) and master secret(PRF).
 *         But only one at a time.
 */
typedef enum optiga_key_id
{
    /// Key from key store (non-volatile)
    OPTIGA_KEY_STORE_ID_E0F0 = 0xE0F0,
    /// Key from key store (non-volatile)
    OPTIGA_KEY_STORE_ID_E0F1 = 0xE0F1,
    /// Key from key store (non-volatile)
    OPTIGA_KEY_STORE_ID_E0F2 = 0xE0F2,
    /// Key from key store (non-volatile)
    OPTIGA_KEY_STORE_ID_E0F3 = 0xE0F3,

    /// Key from Session context id 1 (volatile)
    OPTIGA_SESSION_ID_E100 = 0xE100,
    /// Key from Session context id 2 (volatile)
    OPTIGA_SESSION_ID_E101 = 0xE101,
    /// Key from Session context id 3 (volatile)
    OPTIGA_SESSION_ID_E102 = 0xE102,
    /// Key from Session context id 4 (volatile)
    OPTIGA_SESSION_ID_E103 = 0xE103,
} optiga_key_id_t;


/**
 * \brief OPTIGA Random Generation types
 */
typedef enum optiga_rng_types
{
    /// Generate Random data using TRNG
    OPTIGA_RNG_TYPE_TRNG  = 0x00,
    /// Generate Random data using DRNG
    OPTIGA_RNG_TYPE_DRNG  = 0x01,
} optiga_rng_types_t;

/**
 * \brief enumeration of OPTIGA(TM) ECC curve type
 */
typedef enum optiga_ecc_curve
{
    OPTIGA_ECC_NIST_P_256 = 0x03,
    OPTIGA_ECC_NIST_P_384 = 0x04,
} optiga_ecc_curve_t;

/**
 * \brief enumeration of OPTIGA(TM) key usage
 */
typedef enum optiga_key_usage
{
    /// This enables the private key for the signature generation as part of authentication commands
    OPTIGA_KEY_USAGE_AUTHENTICATION = 0x01,
    /// This enables the private key for the signature generation
    OPTIGA_KEY_USAGE_SIGN = 0x10,
    /// This enables the private key for key agreement (e.g. ecdh operations)
    OPTIGA_KEY_USAGE_KEY_AGREEMENT = 0x20,
} optiga_key_usage_t;

/**
 * \brief To specify the hash context information.
 */
typedef struct optiga_hash_context
{
    ///buffer to hold the context data
    uint8_t * context_buffer;
    ///context length
    uint16_t context_buffer_length;
    ///hashing algorithm
    uint8_t hash_algo;
} optiga_hash_context_t;

/**
 * \brief Specifies the hashing algorithm type in OPTIGA.
 */
typedef enum optiga_hash_type
{
    /// Hash algorithm type SHA256
    OPTIGA_HASH_TYPE_SHA_256 = 0xE2
} optiga_hash_type_t;

/** @brief Data is provided by host*/
#define OPTIGA_CRYPT_HOST_DATA        (0x01)
/** @brief Data in internal to optiga OID */
#define OPTIGA_CRYPT_OID_DATA         (0x00)

/**
 * \brief To specify the data coming from the host for hashing.
 */
typedef struct hash_data_from_host
{
    /// data for hashing
    const uint8_t * buffer;
    /// data length for hashing
    uint32_t length;
} hash_data_from_host_t;

/**
 * \brief To specify the data object for hashing.
 */
typedef struct hash_data_in_optiga
{
    ///OID of data object
    uint16_t oid;
    ///Offset within the data object
    uint16_t offset;
    ///Number of data bytes starting from the offset
    uint16_t length;
} hash_data_in_optiga_t;

/**
 * \brief To specifiy the Public Key details (key, size and algorithm)
 */
typedef struct public_key_from_host
{
    /// Public Key
    uint8_t * public_key;
    /// Length of public key
    uint16_t length;
    /// Public key algorithm
    uint8_t curve;
} public_key_from_host_t;

/**
 * \brief To specify the OID which holds the shared secret.
 */
typedef struct key_from_optiga
{
    ///
    uint16_t key_oid;
} optiga_shared_secret_t;

/**
 * @brief Generates a random number.
 *
 * Generates the requested random stream of data for the user provided length.<br>
 *
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *<b>API Details:</b>
 * - Invokes #CmdLib_GetRandom API, based on the input arguments to retrieve random data .<br>
 *<br>
 *
 *<b>Notes:</b>
 * - The maximum value of the <b>random_data_length</b> parameter is size of buffer <b>random_data</b>.
 *   In case the value is greater than buffer size, memory corruption can occur.<br>
 *
 * \param[in]      rng_type               Type of random data generator.
 *                                        - The input must be from #optiga_rng_types.
 *                                        - Argument check for rng_type is not done since OPTIGA will provide an error for invalid rng_type.
 * \param[in,out]  random_data            Pointer to the buffer into which random data is stored, must not be NULL.
 * \param[in]      random_data_length     Length of random data to be generated.
 *                                        - Range should be 8 - 256 bytes.
 *                                        - Length validation is not done, since OPTIGA will provide an error for invalid random_data_length.
 *
 * \retval  #OPTIGA_LIB_SUCCESS           Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR             The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_random(optiga_rng_types_t rng_type,
                                        uint8_t * random_data,
                                        uint16_t random_data_length);



 /**
 *
 * @brief Initializes a hash context.
 *
 * Sets up a hash context and exports it.<br>
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *<b>API Details:</b><br>
 * - Initializes a new hash context.<br>
 * - Exports the hash context to caller.<br>
 *
 *<b>Notes:</b><br>
 *  - User must save the output hash context for further usage as OPTIGA does not store it internally.<br>
 *
 *<br>
 * \param[inout]   hash_ctx     Pointer to #optiga_hash_context_t to store the hash context from OPTIGA
 *                              - The input <b>hash_algo</b> in  <b>hash_ctx</b> must be from #optiga_hash_type.
 *
 * \retval  #OPTIGA_LIB_SUCCESS           Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR             The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_hash_start(optiga_hash_context_t * hash_ctx);


 /**
 *
 * @brief Updates a hash context with the input data.
 *
 * Updates hashing for the given data and hash context then export the updated hash context.<br>
 *
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 * - #optiga_hash_context_t from #optiga_crypt_hash_start or #optiga_crypt_hash_update must be available.
 *
 *<b>API Details:</b><br>
 * - Update the input hash context.<br>
 * - Exports the hash context to caller.<br>
 *
 *<b>Notes:</b><br>
 *  - User must save the output hash context for further usage as OPTIGA does not store it internally.<br>
 *
 *<br>
 * \param[in]   hash_ctx                  Pointer to #optiga_hash_context_t containing hash context from OPTIGA, must not be NULL
 * \param[in]   source_of_data_to_hash    Data from host / Data in optiga. Must be one of the below
 *                                        - #OPTIGA_CRYPT_HOST_DATA,if source of data is from Host.
 *                                        - #OPTIGA_CRYPT_OID_DATA,if the source of data is from OPITGA.
 * \param[in]   data_to_hash              Data for hashing either in #hash_data_from_host or in #hash_data_in_optiga
 *
 * \retval  #OPTIGA_LIB_SUCCESS           Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR             The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_hash_update(optiga_hash_context_t * hash_ctx,
                                             uint8_t source_of_data_to_hash,
                                             void * data_to_hash);

 /**
 *
 * @brief Finalizes and exports the hash output.
 *
 * Finalizes the hash context and returns hash as output.<br>
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 * - #optiga_hash_context_t from #optiga_crypt_hash_start or #optiga_crypt_hash_update must be available.
 *
 *<b>API Details:</b><br>
 * - Finalize the hash from the input hash context
 * - Exports the finalized hash.
 *
 *<b>Notes:</b><br>
 *  - hash context is not updated by this API. This can be used later to fulfill intermediate hash use-cases<br>
 *  - User must save the output hash context for further usage as OPTIGA does not store it internally.<br>
 *
 *<br>
 * \param[in]   hash_ctx         Pointer to #optiga_hash_context_t containing hash context from OPTIGA, must not be NULL
 * \param[inout]   hash_output   Output Hash
 *
 * \retval  #OPTIGA_LIB_SUCCESS  Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR    The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_hash_finalize(optiga_hash_context_t * hash_ctx,
                                               uint8_t * hash_output);



/**
 *
 * @brief Generates an key pair based on ECC curves.
 *
 * Generates an ECC key-pair based on the type of the key.<br>
 *
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *<b>API Details:</b>
 * - Generate an ECC key pair using OPTIGA.<br>
 * - If export is requested, Exports the private key else stores it in the input private key OID.<br>
 * - Exports the public key always.<br>
 *<br>
 *
 * \param[in]   curve_id                 ECC curve id.
 * \param[in]   key_usage                Key usage defined by #optiga_key_usage_t.
 *                                       - Values from #optiga_key_usage can be logically ORed and passed.<br>
 *                                       - It is ignored if export_private_key is FALSE (0).
 * \param[in]   export_private_key       TRUE (1) - Exports both private key and public key to the host.<br>
 *                                       FALSE (0) - Exports only public key to the host. The input key_usage is ignored.
 * \param[in]    private_key             Buffer to store private key or private key OID of OPTIGA, must not be NULL.
 *                                       - If export_private_key is TRUE, assign pointer to a buffer to store private key.
 *                                       - The size of the buffer must be sufficient enough to accommodate the key type and additional DER encoding formats.
 *                                       - If export_private_key is FALSE, assign pointer to variable of type #optiga_key_id_t.
 * \param[in,out]    public_key          Buffer to store public key, must not be NULL.
 * \param[in]    public_key_length       Initially set as length of public_key, later updated as actual length of public_key.
 *
 * \retval  #OPTIGA_LIB_SUCCESS          Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR            The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_ecc_generate_keypair(optiga_ecc_curve_t curve_id,
                                                      uint8_t key_usage,
                                                      bool_t export_private_key,
                                                      void * private_key,
                                                      uint8_t * public_key,
                                                      uint16_t * public_key_length);


 /**
 *
 * @brief Generates a signature for the given digest.
 *
 * Generates a signature for the given digest using private key stored in OPTIGA.<br>
 *
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>.
 *
 *<b>API Details:</b>
 * - Generated signature for the input digest.<br>
 * - Exports the generated signature.<br>
 *<br>
 *
 * \param[in]   digest               Digest on which signature is generated.
 * \param[in]   digest_length        Length of the input digest.
 * \param[in]   private_key          Private key OID to generate signature.
 * \param[in,out]   signature            Generated signature, must not be NULL.
 *                                   - The size of the buffer must be sufficient enough to accommodate the additional DER encoding formatting for R and S components of signature.
 * \param[in]   signature_length     Length of signature.Intial value set as length of buffer, later updated as the actual length of generated signature.
 *
 * \retval  #OPTIGA_LIB_SUCCESS      Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR        The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_ecdsa_sign(uint8_t * digest,
                                            uint8_t digest_length,
                                            optiga_key_id_t private_key,
                                            uint8_t * signature,
                                            uint16_t * signature_length);

/**
 *
 * @brief Verifies the signature over the given digest.
 *
 * Verifies the signature over a given digest provided with the input data.<br>
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 *
 *<b>API Details:</b>
 * - Verifies the signature over the given provided with the input data using public key.
 *
 * \param[in]   digest                 Pointer to a given digest buffer, must not be NULL.
 * \param[in]   digest_length          Length of digest
 * \param[in]   signature              Pointer to a given signature buffer, must not be NULL.
 * \param[in]   signature_length       Length of signature
 * \param[in]   public_key_source_type Public key from host / public key of certificate OID from OPTIGA. Value must be one of the below
 *                                     - #OPTIGA_CRYPT_OID_DATA, if the public key is to used from the certificate data object from OPTIGA.
 *                                     - #OPTIGA_CRYPT_HOST_DATA, if the public key is provided by host.
 * \param[in]   public_key             Public key from host / public key of certificate OID. Value must be one of the below
 *                                     - For certificate OID, pointer OID value must be passed.
 *                                     - For Public key from host, pointer to #public_key_from_host_t instance.
 *
 * \retval  #OPTIGA_LIB_SUCCESS        Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR          The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_ecdsa_verify(uint8_t * digest,
                                              uint8_t digest_length,
                                              uint8_t * signature,
                                              uint16_t signature_length,
                                              uint8_t public_key_source_type,
                                              void * public_key);

 /**
 * @brief Calculates the shared secret using ECDH algorithm.
 *
 * Calculates the shared secret using ECDH algorithm.<br>
 *
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - There must be a secret available in the "session context / data object OID" provided as input parameter.<br>
 *
 *<b>API Details:</b>
 * - Calculates the shared secret based on input private key object ID and public key.<br>
 * - Based on user request(export_to_host), the shared secret can either be exported to the host or be stored in the acquired session object ID.<br>
 *<br>
 *
 *<b>Notes:</b>
 * - The buffer size for shared secret should be appropriately provided by the user
 * - If the user provides <b>private_key</b> as session based and <b>export_to_host</b> as FALSE,<br>
 *   then the shared secret generated will overwrite the private key stored in the session object ID
 *
 * \param[in]      private_key            Object ID of the private key stored in OPTIGA.<br>
 *                                        - Possible values are from the #optiga_key_id_t <br>
 *                                        - Argument check for private_key is not done since OPTIGA will provide an error for invalid private_key.
 * \param[in]      public_key             Pointer to the public key structure for shared secret generation with its properties, must not be NULL.<br>
 *                                        - Provide the inputs according to the structure type #public_key_from_host_t
 * \param[in]      export_to_host         TRUE (1) - Exports the generated shared secret to Host. <br>
 *                                        FALSE (0) - Stores the generated shared secret into the session object ID acquired by the instance.
 * \param[in,out]   shared_secret          Pointer to the shared secret buffer, only if <b>export_to_host</b> is TRUE. <br>
 *                                        Otherwise supply NULL as input.
 *
 * \retval  #OPTIGA_LIB_SUCCESS           Successful invocation of optiga cmd module Reference Manual)
 * \retval  #OPTIGA_LIB_ERROR             The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_ecdh(optiga_key_id_t private_key,
                                      public_key_from_host_t * public_key,
                                      bool_t export_to_host,
                                      uint8_t * shared_secret);

/**
 * @brief Derives a key.
 *
 * Derives a key using the secret stored in OPTIGA.<br>
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application.<br>
 * - There must be a secret available in the "session context / data object OID" provided as input parameter.<br>
 *
 *<b>API Details:</b>
 * - Derives a key using the secret stored in OPTIGA.
 * - Provides the options to store the derived key into OPTIGA session context or export to the host.
 * - It invokes the callback handler of the instance, when it is asynchronously completed.
 *
 *<b>Notes:</b>
 * - At present, the minimum length of the output derived key is 16.
 *
 * \param[in]      secret                 Object ID of the secret stored in OPTIGA.
 *                                        - Any OPTIGA data object ID(16 bit OID) which holds the secret.
 * \param[in]      label                  Pointer to the label, can be NULL if not applicable.
 * \param[in]      label_length           Length of the label.
 * \param[in]      seed                   Valid pointer to the seed, must not be NULL.
 * \param[in]      seed_length            Length of the seed.
 * \param[in]      derived_key_length     Length of derived key.
 * \param[in]      export_to_host         TRUE (1) - Exports the derived key to Host. <br>
 *                                        FALSE (0) - Stores the derived key  into the session object ID acquired by the instance.
 * \param[in,out]     derived_key          Pointer to the valid buffer with a minimum size of derived_key_length,
 *                                        in case of exporting the key to host(<b>export_to_host= TRUE</b>). Otherwise set to NULL.
 *
 * \retval  #OPTIGA_LIB_SUCCESS           Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR             The execution failed because of an internal error
 */
optiga_lib_status_t optiga_crypt_tls_prf_sha256(uint16_t secret,
                                                uint8_t * label,
                                                uint16_t label_length,
                                                uint8_t * seed,
                                                uint16_t seed_length,
                                                uint16_t derived_key_length,
                                                bool_t export_to_host,
                                                uint8_t * derived_key);


#ifdef __cplusplus
}
#endif

#endif //_OPTIGA_CRYPT_H_

/**
* @}
*/
