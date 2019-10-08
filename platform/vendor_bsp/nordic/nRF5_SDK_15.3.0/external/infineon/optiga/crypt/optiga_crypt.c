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
* \brief   This file implements the OPTIGA Crypto toolbox APIs.
*
* \addtogroup  grOptigaCrypt
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga/pal/pal_os_lock.h"

optiga_lib_status_t optiga_crypt_random(optiga_rng_types_t rng_type,
                                        uint8_t * random_data,
                                        uint16_t random_data_length)
{
    optiga_lib_status_t return_value = OPTIGA_LIB_ERROR;

    sRngOptions_d rand_options;
    sCmdResponse_d rand_response;

    rand_options.eRngType       = (eRngType_d)rng_type;
    rand_options.wRandomDataLen = random_data_length;

    rand_response.prgbBuffer    = random_data;
    rand_response.wBufferLength = random_data_length;
    rand_response.wRespLength   = 0;

    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_GetRandom(&rand_options,&rand_response);
    pal_os_lock_release();

    if (CMD_LIB_OK != return_value)
    {
        return OPTIGA_LIB_ERROR;
    }
    return OPTIGA_LIB_SUCCESS;
}

optiga_lib_status_t optiga_crypt_hash_start(optiga_hash_context_t * hash_ctx)
{
    optiga_lib_status_t return_value;
    uint8_t rgbDataStream[1];
    sCalcHash_d hash_options;

    hash_options.eHashAlg      = (eHashAlg_d)(hash_ctx->hash_algo);
    hash_options.eHashDataType = eDataStream;
    hash_options.eHashSequence = eStartHash;

    hash_options.sDataStream.prgbStream = rgbDataStream;
    hash_options.sDataStream.wLen =0x00;  //No data

    hash_options.sContextInfo.pbContextData  = hash_ctx->context_buffer;
    hash_options.sContextInfo.dwContextLen   = hash_ctx->context_buffer_length;
    hash_options.sContextInfo.eContextAction = eExport;

    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_CalcHash(&hash_options);
    pal_os_lock_release();

    if (CMD_LIB_OK != return_value)
    {
        return OPTIGA_LIB_ERROR;
    }
    return OPTIGA_LIB_SUCCESS;
}

optiga_lib_status_t optiga_crypt_hash_update(optiga_hash_context_t * hash_ctx,
                                             uint8_t source_of_data_to_hash,
                                             void * data_to_hash)
{
    optiga_lib_status_t return_value;
    sCalcHash_d hash_options;
    //To read the maximum communication buffer size.
    uint16_t max_comms_buffer;
    //To hold the remaining communication buffer size to hold the data
    uint16_t remaining_comm_buffer_size = 0;

    //Hash the data
    //Available_Size = (wMaxCommsBuffer - #CALC_HASH_FIXED_OVERHEAD_SIZE)
    uint16_t remaining_comm_bfr_sz_basic;

    //Import context to security chip, calculate hash and export hash
    //Available_Size = (wMaxCommsBuffer - #CALC_HASH_FIXED_OVERHEAD_SIZE - #CALC_HASH_IMPORT_AND_EXPORT_OVERHEAD_SIZE - #CALC_HASH_SHA256_CONTEXT_SIZE)
    uint16_t remaining_comm_bfr_sz_with_import_export;

    //Import context to security chip and calculate hash
    //Available_Size = (wMaxCommsBuffer - #CALC_HASH_FIXED_OVERHEAD_SIZE - #CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE - #CALC_HASH_SHA256_CONTEXT_SIZE)
    uint16_t remaining_comm_bfr_sz_with_import;

    //Calulate hash and export context out of security chip
    //Available_Size = (wMaxCommsBuffer - CALC_HASH_FIXED_OVERHEAD_SIZE - CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE)
    uint16_t remaining_comm_bfr_sz_with_export;

    uint32_t size_of_data_to_hash = 0;
    uint8_t chaining = 0;

    hash_options.eHashAlg      = (eHashAlg_d)(hash_ctx->hash_algo);
    hash_options.eHashDataType = source_of_data_to_hash == OPTIGA_CRYPT_HOST_DATA?eDataStream:eOIDData;
    hash_options.eHashSequence = eContinueHash;

    //Hash context
    hash_options.sContextInfo.pbContextData  = hash_ctx->context_buffer;
    hash_options.sContextInfo.dwContextLen   = hash_ctx->context_buffer_length;
    hash_options.sContextInfo.eContextAction = eImportExport;

    max_comms_buffer = CmdLib_GetMaxCommsBufferSize();

    remaining_comm_bfr_sz_basic = max_comms_buffer - CALC_HASH_FIXED_OVERHEAD_SIZE;
    remaining_comm_bfr_sz_with_import_export = max_comms_buffer -(CALC_HASH_FIXED_OVERHEAD_SIZE +   \
                                               CALC_HASH_IMPORT_AND_EXPORT_OVERHEAD_SIZE +          \
                                               CALC_HASH_SHA256_CONTEXT_SIZE);

    remaining_comm_bfr_sz_with_import = max_comms_buffer -(CALC_HASH_FIXED_OVERHEAD_SIZE+   \
                                        CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE+           \
                                        CALC_HASH_SHA256_CONTEXT_SIZE);

    remaining_comm_bfr_sz_with_export = max_comms_buffer -(CALC_HASH_FIXED_OVERHEAD_SIZE +  \
                                        CALC_HASH_IMPORT_OR_EXPORT_OVERHEAD_SIZE);

    if ((eDataType_d)source_of_data_to_hash == OPTIGA_CRYPT_HOST_DATA)
    {
        hash_options.sDataStream.prgbStream = (uint8_t *)(((hash_data_from_host_t *)data_to_hash)->buffer);
        size_of_data_to_hash       = ((hash_data_from_host_t *)data_to_hash)->length;
        remaining_comm_buffer_size = size_of_data_to_hash;

        //Verify whether (Import & export) + data options can be set in one command or not
        if (remaining_comm_bfr_sz_with_import_export<size_of_data_to_hash)
        {
            chaining = 1;
            remaining_comm_buffer_size = remaining_comm_bfr_sz_with_import;
            if (remaining_comm_bfr_sz_with_import>size_of_data_to_hash)
            {
                remaining_comm_buffer_size = size_of_data_to_hash;
            }
            hash_options.sContextInfo.eContextAction = eImport;
        }
        hash_options.sDataStream.wLen = remaining_comm_buffer_size;
    }
    else
    {
        hash_options.sOIDData.wOID    = ((hash_data_in_optiga_t *)data_to_hash)->oid;
        hash_options.sOIDData.wOffset = ((hash_data_in_optiga_t *)data_to_hash)->offset;
        hash_options.sOIDData.wLength = ((hash_data_in_optiga_t *)data_to_hash)->length;
    }

    while (1)
    {
        while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
        return_value = CmdLib_CalcHash(&hash_options);
        pal_os_lock_release();

        if (CMD_LIB_OK != return_value)
        {
            return_value = OPTIGA_LIB_ERROR;
            break;
        }

        if (!chaining)
        {
            return_value = OPTIGA_LIB_SUCCESS;
            break;
        }
        else
        {
            hash_options.sDataStream.prgbStream += remaining_comm_buffer_size;
            size_of_data_to_hash -= remaining_comm_buffer_size;

            remaining_comm_buffer_size = size_of_data_to_hash;
            hash_options.sContextInfo.eContextAction = eUnused;
            //Verify whether export + data options can be set in one command or not
            if (remaining_comm_bfr_sz_with_export < size_of_data_to_hash)
            {
                remaining_comm_buffer_size = remaining_comm_bfr_sz_basic;
                if (remaining_comm_bfr_sz_basic > size_of_data_to_hash)
                {
                    remaining_comm_buffer_size = size_of_data_to_hash;
                }
            }
            else
            {
                hash_options.sContextInfo.eContextAction = eExport;
                chaining = 0;
            }
            hash_options.sDataStream.wLen = remaining_comm_buffer_size;
        }

        return_value = OPTIGA_LIB_SUCCESS;
    }

    return return_value;
}

optiga_lib_status_t optiga_crypt_hash_finalize(optiga_hash_context_t * hash_ctx,
                                               uint8_t * hash_output)
{
    optiga_lib_status_t return_value;
    uint8_t datastream[1];
    sCalcHash_d hash_options;

    hash_options.eHashAlg        = (eHashAlg_d)hash_ctx->hash_algo;
    hash_options.eHashDataType   =  eDataStream;
    hash_options.eHashSequence   =  eFinalizeHash;
    hash_options.sDataStream.prgbStream  = datastream;
    hash_options.sDataStream.wLen        = 0x00;    //No data

    hash_options.sContextInfo.pbContextData  = hash_ctx->context_buffer;
    hash_options.sContextInfo.dwContextLen   = hash_ctx->context_buffer_length;
    hash_options.sContextInfo.eContextAction = eImport;

    hash_options.sOutHash.prgbBuffer         = hash_output;
    if(hash_options.eHashAlg == eSHA256)
    {
        hash_options.sOutHash.wBufferLength  = 32;
    }

    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_CalcHash(&hash_options);
    pal_os_lock_release();

    if (CMD_LIB_OK != return_value)
    {
        return OPTIGA_LIB_ERROR;
    }
    return OPTIGA_LIB_SUCCESS;
}

optiga_lib_status_t optiga_crypt_ecc_generate_keypair(optiga_ecc_curve_t curve_id,
                                                      uint8_t key_usage,
                                                      bool_t export_private_key,
                                                      void * private_key,
                                                      uint8_t * public_key,
                                                      uint16_t * public_key_length)
{
    optiga_lib_status_t return_value;
    sKeyPairOption_d keypair_options;
    sOutKeyPair_d public_key_out;

    keypair_options.eAlgId      = (eAlgId_d)curve_id;
    keypair_options.eKeyUsage   = (eKeyUsage_d)key_usage;


    if (export_private_key)
    {
        keypair_options.eKeyExport  = eExportKeyPair;

        public_key_out.sPublicKey.prgbStream = public_key;
        public_key_out.sPublicKey.wLen       = *public_key_length;
        public_key_out.sPrivateKey.prgbStream = private_key;
        public_key_out.sPrivateKey.wLen      = 0xffff;
    }
    else
    {
        keypair_options.eKeyExport  = eStorePrivKeyOnly;
        keypair_options.wOIDPrivKey = *(uint16_t *)private_key;

        public_key_out.sPublicKey.prgbStream = public_key;
        public_key_out.sPublicKey.wLen       = *public_key_length;
    }



    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_GenerateKeyPair(&keypair_options,&public_key_out);
    pal_os_lock_release();

    if (CMD_LIB_OK != return_value)
    {
        return OPTIGA_LIB_ERROR;
    }
    //store updated public key length .
    *( public_key_length) = public_key_out.sPublicKey.wLen;
    return OPTIGA_LIB_SUCCESS;
}

optiga_lib_status_t optiga_crypt_ecdsa_sign (uint8_t * digest,
                                             uint8_t digest_length,
                                             optiga_key_id_t private_key,
                                             uint8_t * signature,
                                             uint16_t * signature_length)
{
    optiga_lib_status_t return_value;
    sbBlob_d sign;
    sCalcSignOptions_d sign_options;

    sign_options.eSignScheme = eECDSA_FIPS_186_3_WITHOUT_HASH;
    sign_options.wOIDSignKey = private_key;

    sign_options.sDigestToSign.prgbStream = digest;
    sign_options.sDigestToSign.wLen       = digest_length;

    sign.prgbStream = signature;
    sign.wLen       =  *signature_length;

    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_CalculateSign(&sign_options,&sign);
    pal_os_lock_release();

    if (CMD_LIB_OK != return_value)
    {
        return OPTIGA_LIB_ERROR;
    }
    *signature_length = sign.wLen;
    return OPTIGA_LIB_SUCCESS;
}

optiga_lib_status_t optiga_crypt_ecdsa_verify (uint8_t * digest,
                                               uint8_t digest_length,
                                               uint8_t * signature,
                                               uint16_t signature_length,
                                               uint8_t public_key_source_type,
                                               void * public_key)
{
    optiga_lib_status_t return_value;
    sVerifyOption_d verifysign_options;
    sbBlob_d sign, dgst;

    verifysign_options.eSignScheme         = eECDSA_FIPS_186_3_WITHOUT_HASH;
    verifysign_options.sPubKeyInput.eAlgId = (eAlgId_d )(((public_key_from_host_t *)public_key)->curve);

    if (public_key_source_type == OPTIGA_CRYPT_HOST_DATA)
    {
        verifysign_options.eVerifyDataType = eDataStream;
        verifysign_options.sPubKeyInput.sDataStream.prgbStream = (uint8_t *)((( public_key_from_host_t *)public_key)->public_key);
        verifysign_options.sPubKeyInput.sDataStream.wLen = (((public_key_from_host_t *)public_key)->length);
    }
    else if (public_key_source_type == OPTIGA_CRYPT_OID_DATA)
    {
        verifysign_options.eVerifyDataType = eOIDData;
        verifysign_options.wOIDPubKey      = *((uint16_t *)public_key);
    }

    dgst.prgbStream = digest;
    dgst.wLen       = digest_length;

    sign.prgbStream = signature;
    sign.wLen       = signature_length;

    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_VerifySign(&verifysign_options, &dgst, &sign);
    pal_os_lock_release();

    if(CMD_LIB_OK != return_value)
    {
        return OPTIGA_LIB_ERROR;
    }

    return OPTIGA_LIB_SUCCESS;
}

optiga_lib_status_t optiga_crypt_ecdh(optiga_key_id_t private_key,
                                      public_key_from_host_t * public_key,
                                      bool_t export_to_host,
                                      uint8_t * shared_secret)
{
    optiga_lib_status_t return_value = OPTIGA_LIB_ERROR;
    sCalcSSecOptions_d shared_secret_options;
    sbBlob_d sharedsecret;

    shared_secret_options.eKeyAgreementType  = eECDH_NISTSP80056A;
    shared_secret_options.wOIDPrivKey        = private_key;

    public_key->curve == OPTIGA_ECC_NIST_P_256 ? (sharedsecret.wLen = 32) : (sharedsecret.wLen = 64);

    shared_secret_options.ePubKeyAlgId       = (eAlgId_d)public_key->curve;
    shared_secret_options.sPubKey.prgbStream = public_key->public_key;
    shared_secret_options.sPubKey.wLen       = public_key->length;

    if (export_to_host == 1)
    {
        shared_secret_options.wOIDSharedSecret = 0x0000;
        sharedsecret.prgbStream = (uint8_t *)shared_secret;
    }
    else
    {
        //shared secret is stored in OID of OPTIGA
        shared_secret_options.wOIDSharedSecret = *((uint16_t *)shared_secret);
    }

    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_CalculateSharedSecret(&shared_secret_options, &sharedsecret);
    pal_os_lock_release();

    if(CMD_LIB_OK != return_value)
    {
        return_value = OPTIGA_LIB_ERROR;
    }

    return OPTIGA_LIB_SUCCESS;
}


optiga_lib_status_t optiga_crypt_tls_prf_sha256(uint16_t secret,
                                                uint8_t * label,
                                                uint16_t label_length,
                                                uint8_t * seed,
                                                uint16_t seed_length,
                                                uint16_t derived_key_length,
                                                bool_t export_to_host,
                                                uint8_t * derived_key)
{
    optiga_lib_status_t return_value = OPTIGA_LIB_ERROR;
    sDeriveKeyOptions_d derivekey_options;
    sbBlob_d derivekey_output_buffer;

    derivekey_options.eKDM             = eTLS_PRF_SHA256;
    derivekey_options.sSeed.prgbStream = seed;
    derivekey_options.sSeed.wLen       = seed_length;
    derivekey_options.wOIDSharedSecret = secret;

    if (derived_key_length < 16)
    {
        derived_key_length += (16 - derived_key_length);
    }

    derivekey_options.wDerivedKeyLen = derived_key_length;

    if (export_to_host == 1)
    {
        derivekey_options.wOIDDerivedKey   = 0x0000;
        derivekey_output_buffer.prgbStream = (uint8_t *)derived_key;
        derivekey_output_buffer.wLen       = derived_key_length;
    }
    else
    {
        derivekey_options.wOIDDerivedKey = *((uint16_t *)derived_key);
    }

    while (pal_os_lock_acquire() != OPTIGA_LIB_SUCCESS);
    return_value = CmdLib_DeriveKey(&derivekey_options, &derivekey_output_buffer);
    pal_os_lock_release();

    if(CMD_LIB_OK == return_value)
    {
        return_value = OPTIGA_LIB_SUCCESS;
    }

    return OPTIGA_LIB_ERROR;
}
