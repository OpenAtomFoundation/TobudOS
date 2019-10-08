/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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
#include "ssi_pal_types.h"
#include "ssi_pal_mem.h"
#include "sns_silib.h"
#include "crys_rsa_kg.h"
#include "crys_rsa_build.h"
#include "crys_rsa_schemes.h"
#include "crys_rsa_prim.h"
#include "integration_test_plat_defs.h"
#include "integration_test_ssi_defs.h"
#include "integration_test_ssi_data.h"
#ifdef DX_LINUX_PLATFORM
#include <pthread.h>
#endif
/*RSA test data vectors*/
extern  rsaEncDecDataStuct rsaEncDecDataVectors[];
extern  rsaSignVerifyDataStuct rsaSignVerifyDataVectors[];

/*RNG Global variables*/
extern CRYS_RND_State_t*     rndState_ptr;
extern CRYS_RND_WorkBuff_t*  rndWorkBuff_ptr;


int rsa_SignVerify_tests(void);
int rsa_EncDec_tests(void);


int rsa_tests(void){
	uint32_t                        ret = 0;

        /*Init SaSi library*/
    ret = SaSi_LibInit();
	if (ret != SA_SILIB_RET_OK) {
        INTEG_TEST_PRINT("Failed SaSi_LibInit - ret = 0x%x\n", ret);
        goto exit_1;
    }

    ret = CRYS_RndInit(rndState_ptr, rndWorkBuff_ptr);
    if (ret != SA_SILIB_RET_OK) {
        INTEG_TEST_PRINT("Failed CRYS_RndInit - ret = 0x%x\n", ret);
	    goto exit_1;
	}

	ret = rsa_EncDec_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in encrypt/decrypt RSA tests,ret = 0x%x\n", ret);
	    goto endRSA;
	}
	/*Call to RSA Sign/Verify Tests*/
	ret = rsa_SignVerify_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in sign/verify tests,ret = 0x%x\n", ret);
	    goto endRSA;
	}

endRSA:
	/*Finish SaSi library*/
    SaSi_LibFini();

    ret = CRYS_RND_UnInstantiation(rndState_ptr);

    if (ret) {
        INTEG_TEST_PRINT("Failure in CRYS_RND_UnInstantiation,ret = 0x%x\n", ret);
    }

exit_1:
	return ret;
}

int rsa_SignVerify_tests(void){

	uint32_t                        ret = 0;
	int                             test_index;
	RSAType_enum                    RSA_mode;
	CRYS_RSAUserPrivKey_t           UserPrivKey;
	CRYS_RSAUserPubKey_t            UserPubKey;
	CRYS_RSAKGData_t                KeyGenData;
	CRYS_RSAKGFipsContext_t    		FipsCtx;
	CRYS_RSAPrivUserContext_t       ContextPrivate;
	CRYS_RSAPubUserContext_t        ContextPub;
	uint16_t                        ActualSignatureSize;
	SaSiRndGenerateVectWorkFunc_t rndGenerateVectFunc;

	rndGenerateVectFunc = CRYS_RND_GenerateVector;
	/*Run all RSA tests*/
	for (RSA_mode = NON_CRT_MODE ; RSA_mode <= CRT_MODE; RSA_mode++) /* Check both CRT and non CRT modes*/
	{
		for (test_index = 0; test_index < RSA_SIGN_VERIFY_OEP_TESTS_NUMBER ; test_index++) /*Run with all RSA data vectors*/
		{
			if (test_index == 0){ /*Check internal keys*/
				if (RSA_mode == NON_CRT_MODE ){ /*Check non CRT keys*/

					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPair  \n");

					ret = CRYS_RSA_KG_GenerateKeyPair(rndState_ptr, rndGenerateVectFunc,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_PublicExponent_E,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_PubExponentSize,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize*8,
						&UserPrivKey,
						&UserPubKey,
						&KeyGenData,
						&FipsCtx);

					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPair for key pair with 0x%x \n",ret);


					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPair for key pair failed with 0x%x \n",ret);
						goto endRSA;
					}
				} else {/*Check CRT keys*/

					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPairCRT  \n");

					ret = CRYS_RSA_KG_GenerateKeyPairCRT(rndState_ptr, rndGenerateVectFunc,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_PublicExponent_E,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_PubExponentSize,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize*8,
						&UserPrivKey,
						&UserPubKey,
						&KeyGenData,
						&FipsCtx);
					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPairCRT for key pair returned with 0x%x \n",ret);


					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPairCRT for key pair failed with 0x%x \n",ret);
						goto endRSA;

					}
				}

			} else { /*Check external keys*/

				/*Call CRYS_RSA_Build_PrivKey to create public key from external key buffer*/
				ret = CRYS_RSA_Build_PubKey(&UserPubKey,
					rsaSignVerifyDataVectors[test_index].rsaSignVerify_PublicExponent_E,
					rsaSignVerifyDataVectors[test_index].rsaSignVerify_PubExponentSize,
					rsaSignVerifyDataVectors[test_index].rsaSignVerify_Modulus_N,
					rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize);

					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT("CRYS_RSA_Build_PubKey for key failed with 0x%x \n",ret);
						goto endRSA;
					}

					INTEG_TEST_PRINT("CRYS_RSA_Build_PubKey for key pair with 0x%x \n",ret);

					if (RSA_mode == NON_CRT_MODE) {/*Check non CRT keys*/

						/*Call CRYS_RSA_Build_PrivKey to create private key from external key buffer*/
						ret =  CRYS_RSA_Build_PrivKey (&UserPrivKey,
							rsaSignVerifyDataVectors[test_index].rsaSignVerify_PrivetExponent_D,
							rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize,
							rsaSignVerifyDataVectors[test_index].rsaSignVerify_PublicExponent_E,
							rsaSignVerifyDataVectors[test_index].rsaSignVerify_PubExponentSize,
							rsaSignVerifyDataVectors[test_index].rsaSignVerify_Modulus_N,
							rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize);

						if (ret != SA_SILIB_RET_OK){
							INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKey for key failed with 0x%x \n",ret);
							goto endRSA;
						}

						INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKey for key pair returned with 0x%x \n",ret);
				} else { /*Check CRT keys*/

					/*Call CRYS_RSA_Build_PrivKey to create private key from external key buffer*/
					ret =  CRYS_RSA_Build_PrivKeyCRT (&UserPrivKey,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_P,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_DPSize,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_Q,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_DPSize,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_dP,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_DPSize,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_dQ,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_DPSize,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_Qinv,
						rsaSignVerifyDataVectors[test_index].rsaSignVerify_DPSize);


					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKeyCRT for key failed with 0x%x \n",ret);
						goto endRSA;
					}
					INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKeyCRT for key pair returned with 0x%x \n",ret);
				}

			}

			INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKeyCRT for key pair returned with 0x%x \n",ret);

			ActualSignatureSize = rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize;

			/*Call CRYS_RSA_PSS_Sign for PKCS#1 ver2.1 using SHA512*/
			ret =  CRYS_RSA_PSS_Sign(rndState_ptr, rndGenerateVectFunc,
				&ContextPrivate,
				&UserPrivKey,
				CRYS_RSA_HASH_SHA512_mode,
				CRYS_PKCS1_MGF1,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_SaltLength,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_data,//DataIn_ptr,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_dataSize,//DataInSize,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture,//Output_ptr,
				&ActualSignatureSize);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PSS_Sign failed with 0x%x \n",ret);
				goto endRSA;
			}


			INTEG_TEST_PRINT("CRYS_RSA_PSS_Sign passed \n");

			/*Verify the signature using using SHA512*/
			ret =  CRYS_RSA_PSS_Verify(
				&ContextPub,
				&UserPubKey,
				CRYS_RSA_HASH_SHA512_mode,
				CRYS_PKCS1_MGF1,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_SaltLength,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_data,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_dataSize,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PSS_Verify failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PSS_Verify passed \n");

			ActualSignatureSize = rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize;

			/*Call CRYS_RSA_PSS_SHA1_Sign PKCS#1 ver2.1 to sign on precalculated hash input using SHA1*/
			ret =  CRYS_RSA_PSS_SHA1_Sign(rndState_ptr, rndGenerateVectFunc,
				&ContextPrivate,
				&UserPrivKey,
				CRYS_PKCS1_MGF1,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_SaltLength,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_hash_SHA1,//rsaSignVerify_input_data,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture,
				&ActualSignatureSize);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PSS_SHA1_Sign failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PSS_SHA1_Sign passed \n");

			/*Verify signed hash using SHA1*/
			ret =  CRYS_RSA_PSS_SHA1_Verify(&ContextPub,
				&UserPubKey,
				CRYS_PKCS1_MGF1,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_SaltLength,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_hash_SHA1,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PSS_SHA1_Verify failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PSS_SHA1_Verify passed \n");


			ActualSignatureSize = rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize;

			/*Call CRYS_RSA_PKCS1v15_Sign PKCS#1 ver1.5 to sign on input data*/
			ret =  CRYS_RSA_PKCS1v15_Sign(rndState_ptr, rndGenerateVectFunc,
				&ContextPrivate,
				&UserPrivKey,
				CRYS_RSA_HASH_SHA256_mode,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_data,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_dataSize,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture,
				&ActualSignatureSize);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Sign failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Sign passed \n");

			/*Verify the signature*/
			ret =  CRYS_RSA_PKCS1v15_Verify(&ContextPub,
				&UserPubKey,
				CRYS_RSA_HASH_SHA256_mode,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_data,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_input_dataSize,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Verify failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Verify passed \n");

			ActualSignatureSize = rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize;

			/*Call CRYS_RSA_PKCS1v15_SHA256_Sign PKCS#1 ver1.5 to sign on precalculated hash input using SHA256*/
			ret =  CRYS_RSA_PKCS1v15_SHA256_Sign(rndState_ptr, rndGenerateVectFunc,
				&ContextPrivate,
				&UserPrivKey,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_hash_SHA256,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture,
				&ActualSignatureSize);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA256_Sign failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA256_Sign passed \n");


			/*Verify the signature*/
			ret =  CRYS_RSA_PKCS1v15_SHA256_Verify(&ContextPub,
				&UserPubKey,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_hash_SHA256,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA256_Verify failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA256_Verify passed \n");

			ActualSignatureSize = rsaSignVerifyDataVectors[test_index].rsaSignVerify_KeySize;

			/*Call CRYS_RSA_PKCS1v15_SHA256_Sign PKCS#1 ver1.5 to sign on precalculated hash input using SHA512*/
			ret =  CRYS_RSA_PKCS1v15_SHA512_Sign(rndState_ptr, rndGenerateVectFunc,
				&ContextPrivate,
				&UserPrivKey,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_hash_SHA256,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture,
				&ActualSignatureSize);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA512_Sign failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA512_Sign passed \n");


			/*Verify the signature*/
			ret =  CRYS_RSA_PKCS1v15_SHA512_Verify(&ContextPub,
				&UserPubKey,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_hash_SHA256,
				rsaSignVerifyDataVectors[test_index].rsaSignVerify_output_signiture);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA512_Verify failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_SHA512_Verify passed \n");
		}
	}
	INTEG_TEST_PRINT("All RSA SIGN/VERIFY tests passed \n=======================\n");

endRSA:
	return ret;
}



int rsa_EncDec_tests(void){
	uint32_t                        ret = 0;
	int                             test_index;
	uint16_t                        ActualDecDataSize;
	RSAType_enum                    RSA_mode;
	CRYS_RSAUserPrivKey_t           UserPrivKey;
	CRYS_RSAUserPubKey_t            UserPubKey;
	CRYS_RSAKGData_t                KeyGenData;
	CRYS_RSAKGFipsContext_t    		FipsCtx;
	CRYS_RSAPrimeData_t  	        PrimeData;
	CRYS_RSA_HASH_OpMode_t          HASH_OpMode;
	uint16_t                   	LessData_for_OAEP;
	SaSiRndGenerateVectWorkFunc_t rndGenerateVectFunc;

	rndGenerateVectFunc = CRYS_RND_GenerateVector;
	/*Run all RSA tests*/
	for (RSA_mode = NON_CRT_MODE ; RSA_mode <= CRT_MODE; RSA_mode++) /* Check both CRT and non CRT modes*/
	{
		for (test_index = 0; test_index < RSA_ENCDEC_OEP_TESTS_NUMBER ; test_index++) /*Run with all RSA data vectors*/
		{
			if (test_index == 0){ /*Check different hash modes*/
				HASH_OpMode = CRYS_RSA_HASH_SHA1_mode;
				LessData_for_OAEP = TST_LESS_DATA_FOR_OAEP_ENCRYPT_SHA1; //Select the size according to the hash mode*/

			}else {
				HASH_OpMode = CRYS_RSA_HASH_SHA256_mode;
				LessData_for_OAEP = TST_LESS_DATA_FOR_OAEP_ENCRYPT_SHA256;//Select the size according to the hash mode*/

			}
			/*Print test's parameters*/
			if (HASH_OpMode == CRYS_RSA_HASH_SHA1_mode ){
				INTEG_TEST_PRINT("\n\nRSA encrypt decrypt test number 0x%x Parameters : \n-----%s CRYS_RSA_HASH_SHA1_mode -----\n ",test_index,(uint32_t)rsaEncDecDataVectors[test_index].rsaEncDec_Name);
			} else {
				INTEG_TEST_PRINT("\n\nRSA encrypt decrypt test number 0x%x Parameters : \n-----%s CRYS_RSA_HASH_SHA256_mode -----\n ",test_index,(uint32_t)rsaEncDecDataVectors[test_index].rsaEncDec_Name);
			}
			/*Update ActualDecDataSize to output size*/
			ActualDecDataSize = rsaEncDecDataVectors[test_index].rsaEncDec_KeySize*8;


			if (test_index == 0){ /*Check internal keys*/
				if (RSA_mode == NON_CRT_MODE ){ /*Check non CRT keys*/

					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPair  \n");

					ret = CRYS_RSA_KG_GenerateKeyPair(rndState_ptr, rndGenerateVectFunc,
						rsaEncDecDataVectors[test_index].rsaEncDec_PublicExponent_E,
						rsaEncDecDataVectors[test_index].rsaEncDec_PubExponentSize,
						rsaEncDecDataVectors[test_index].rsaEncDec_KeySize*8,
						&UserPrivKey,
						&UserPubKey,
						&KeyGenData,
						&FipsCtx);

					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPair for key pair with 0x%x \n",ret);


					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT(" CRYS_RSA_KG_GenerateKeyPair for key pair failed with 0x%x \n",ret);
						goto endRSA;
					}
				} else {/*Check CRT keys*/

					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPairCRT  \n");

					ret = CRYS_RSA_KG_GenerateKeyPairCRT(rndState_ptr, rndGenerateVectFunc,
						rsaEncDecDataVectors[test_index].rsaEncDec_PublicExponent_E,
						rsaEncDecDataVectors[test_index].rsaEncDec_PubExponentSize,
						rsaEncDecDataVectors[test_index].rsaEncDec_KeySize*8,
						&UserPrivKey,
						&UserPubKey,
						&KeyGenData,
						&FipsCtx);
					INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPairCRT for key pair returned with 0x%x \n",ret);


					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT("CRYS_RSA_KG_GenerateKeyPairCRT for key pair failed with 0x%x \n",ret);
						goto endRSA;
					}
				}

			} else { /*Check external keys*/

				/*Call CRYS_RSA_Build_PrivKey to create public key from external key buffer*/
				ret = CRYS_RSA_Build_PubKey(&UserPubKey,
					rsaEncDecDataVectors[test_index].rsaEncDec_PublicExponent_E,
					rsaEncDecDataVectors[test_index].rsaEncDec_PubExponentSize,
					rsaEncDecDataVectors[test_index].rsaEncDec_Modulus_N,
					rsaEncDecDataVectors[test_index].rsaEncDec_KeySize);

					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT("CRYS_RSA_Build_PubKey for key failed with 0x%x \n",ret);
						goto endRSA;
					}

					INTEG_TEST_PRINT("CRYS_RSA_Build_PubKey for key pair with 0x%x \n",ret);

					if (RSA_mode == NON_CRT_MODE) {/*Check non CRT keys*/

						/*Call CRYS_RSA_Build_PrivKey to create private key from external key buffer*/
						ret =  CRYS_RSA_Build_PrivKey (&UserPrivKey,
							rsaEncDecDataVectors[test_index].rsaEncDec_PrivetExponent_D,
							rsaEncDecDataVectors[test_index].rsaEncDec_KeySize,
							rsaEncDecDataVectors[test_index].rsaEncDec_PublicExponent_E,
							rsaEncDecDataVectors[test_index].rsaEncDec_PubExponentSize,
							rsaEncDecDataVectors[test_index].rsaEncDec_Modulus_N,
							rsaEncDecDataVectors[test_index].rsaEncDec_KeySize);

						if (ret != SA_SILIB_RET_OK){
							INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKey for key failed with 0x%x \n",ret);
							goto endRSA;
						}

						INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKey for key pair returned with 0x%x \n",ret);
				} else { /*Check CRT keys*/

					/*Call CRYS_RSA_Build_PrivKey to create private key from external key buffer*/
					ret =  CRYS_RSA_Build_PrivKeyCRT (&UserPrivKey,
						rsaEncDecDataVectors[test_index].rsaEncDec_P,
						rsaEncDecDataVectors[test_index].rsaEncDec_DPSize,
						rsaEncDecDataVectors[test_index].rsaEncDec_Q,
						rsaEncDecDataVectors[test_index].rsaEncDec_DPSize,
						rsaEncDecDataVectors[test_index].rsaEncDec_dP,
						rsaEncDecDataVectors[test_index].rsaEncDec_DPSize,
						rsaEncDecDataVectors[test_index].rsaEncDec_dQ,
						rsaEncDecDataVectors[test_index].rsaEncDec_DPSize,
						rsaEncDecDataVectors[test_index].rsaEncDec_Qinv,
						rsaEncDecDataVectors[test_index].rsaEncDec_DPSize);

					if (ret != SA_SILIB_RET_OK){
						INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKeyCRT for key failed with 0x%x \n",ret);
						goto endRSA;
					}
					INTEG_TEST_PRINT("CRYS_RSA_Build_PrivKeyCRT for key pair returned with 0x%x \n",ret);
				}
			}

			/*Call CRYS_RSA_OAEP_Encrypt to ecrypt data buffer */
			ret =  CRYS_RSA_OAEP_Encrypt(rndState_ptr,
                rndGenerateVectFunc,
				&UserPubKey,
				&PrimeData,
				HASH_OpMode,
				NULL,
				0,
				CRYS_PKCS1_MGF1,
				rsaEncDecDataVectors[test_index].rsaEncDec_input_data,
				(uint16_t)(rsaEncDecDataVectors[test_index].rsaEncDec_KeySize-LessData_for_OAEP),
			        rsaEncDecDataVectors[test_index].rsaEncDec_output_data);

			if (ret != SA_SILIB_RET_OK){
					INTEG_TEST_PRINT("CRYS_RSA_OAEP_Encrypt for key failed with 0x%x \n",ret);
					goto endRSA;
				}

			INTEG_TEST_PRINT("CRYS_RSA_OAEP_Encrypt  returned with 0x%x \n",ret);

			/*Call CRYS_RSA_OAEP_Decrypt to decrypt data buffer */
			ret = CRYS_RSA_OAEP_Decrypt(&UserPrivKey,
				&PrimeData,
				HASH_OpMode,
				NULL,
				0,
				CRYS_PKCS1_MGF1,
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data,
				(uint16_t)rsaEncDecDataVectors[test_index].rsaEncDec_KeySize,
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data,//Inplace operation
				&ActualDecDataSize);

				if (ret != SA_SILIB_RET_OK){
					INTEG_TEST_PRINT("CRYS_RSA_OAEP_Decrypt failed with 0x%x \n",ret);
					goto endRSA;
				}

			INTEG_TEST_PRINT("CRYS_RSA_OAEP_Decrypt returned with 0x%x \n",ret);

			/*Compare decrypted buffer with input buffer*/
			ret = SaSi_PalMemCmp(rsaEncDecDataVectors[test_index].rsaEncDec_output_data ,
				rsaEncDecDataVectors[test_index].rsaEncDec_input_data,
				rsaEncDecDataVectors[test_index].rsaEncDec_KeySize-LessData_for_OAEP);

			if (ret != 0){
				INTEG_TEST_PRINT("Decrypted buffer for OAEP is wrong");
				goto endRSA;
			}
			/*Update ActualDecDataSize to output size*/
			ActualDecDataSize = rsaEncDecDataVectors[test_index].rsaEncDec_KeySize*8;

			/*CAll to CRYS_RSA_PKCS1v15_Encrypt to encrypt input buffer using PKCS#1 1.5*/
			ret = CRYS_RSA_PKCS1v15_Encrypt(rndState_ptr, rndGenerateVectFunc,
				&UserPubKey,
				&PrimeData,
				rsaEncDecDataVectors[test_index].rsaEncDec_input_data,
				(uint16_t)(rsaEncDecDataVectors[test_index].rsaEncDec_KeySize-TST_LESS_DATA_FOR_PKCS1V15_ENCRYPT),
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Encrypt  failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Encrypt returned with 0x%x \n",ret);

			/*DEcrypt oupu buffer*/
			ret = CRYS_RSA_PKCS1v15_Decrypt(&UserPrivKey,
				&PrimeData,
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data,
				rsaEncDecDataVectors[test_index].rsaEncDec_KeySize,
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data,//Inplace operation
				&ActualDecDataSize);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Decrypt failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PKCS1v15_Decrypt returned with 0x%x \n",ret);

			/*Compare decrypted buffer with input buffer*/
			ret = SaSi_PalMemCmp(rsaEncDecDataVectors[test_index].rsaEncDec_output_data ,
				rsaEncDecDataVectors[test_index].rsaEncDec_input_data,
				rsaEncDecDataVectors[test_index].rsaEncDec_KeySize-LessData_for_OAEP);

			if (ret != 0){
				INTEG_TEST_PRINT("Decrypted buffer for PKCS1v15 is wrong");
				goto endRSA;
			}


			/*Call to CRYS_RSA_PRIM_Encrypt to encrypt input buffer*/
			ret = CRYS_RSA_PRIM_Encrypt(&UserPubKey,
				&PrimeData,
				rsaEncDecDataVectors[test_index].rsaEncDec_input_data,
				rsaEncDecDataVectors[test_index].rsaEncDec_KeySize,
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT("CRYS_RSA_PRIM_Encrypt failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PRIM_Encrypt returned with 0x%x \n",ret);

			/*Decrypt oupu buffer*/
			ret = CRYS_RSA_PRIM_Decrypt(&UserPrivKey,
				&PrimeData,
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data,
				rsaEncDecDataVectors[test_index].rsaEncDec_KeySize,
				rsaEncDecDataVectors[test_index].rsaEncDec_output_data);//Inplace operation

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_RSA_PRIM_Decrypt failed with 0x%x \n",ret);
				goto endRSA;
			}

			INTEG_TEST_PRINT("CRYS_RSA_PRIM_Decrypt returned with 0x%x \n",ret);

			/*Compare decrypted buffer with input buffer*/
			ret = SaSi_PalMemCmp(rsaEncDecDataVectors[test_index].rsaEncDec_output_data ,
				rsaEncDecDataVectors[test_index].rsaEncDec_input_data,
				rsaEncDecDataVectors[test_index].rsaEncDec_KeySize);

			if (ret != 0){
				INTEG_TEST_PRINT("Decrypted buffer for PRIM is wrong");
				goto endRSA;
			}
		}
	}
	INTEG_TEST_PRINT("All RSA ENCRYPT/DECRYPT tests passed \n=======================\n");

endRSA:
	return ret;

}

#ifdef DX_LINUX_PLATFORM /*for linux platform only  -> we need to use contiguous memory for stack !!*/
/*rsa_wrap_tests creates thread with defined stack address to and calls to rsa test */
void* rsa_thread(int (*funcPtr)(void))
{
   uint32_t* threadReturnValue = SaSi_PalMemMalloc(sizeof(uint32_t));

   *threadReturnValue =funcPtr();
    if (*threadReturnValue != SA_SILIB_RET_OK) {
        INTEG_TEST_PRINT("Failure in funcPtr,ret = 0x%x\n", *threadReturnValue);
        goto exit;
    }


exit:
	pthread_exit(threadReturnValue);
}

int rsa_wrap_tests(int (*funcPtr)(void)){
	uint32_t rc = 0;

	pthread_t threadId;
	pthread_attr_t threadAttr;
	int threadRc;
	void *threadRet;
	threadRc = pthread_attr_init(&threadAttr);
	if (threadRc != 0) {
		INTEG_TEST_PRINT("pthread_attr_init failed\n");
		return -1;
	}

	threadRc = pthread_attr_setstack(&threadAttr, g_test_stack_base_addr, PTHREAD_STACK_SIZE);
	if (threadRc != 0) {
		INTEG_TEST_PRINT("pthread_attr_setstack failed\n");
		return -1;
	}

	/* Create independent thread which run with  */
	threadRc = pthread_create( &threadId, &threadAttr, (void*)rsa_thread, funcPtr);
	if (threadRc != 0) {
		INTEG_TEST_PRINT( "pthread_create failed\n");
		return -1;
	}

	/* Wait till thread is complete before main continues */
	threadRc = pthread_join( threadId, &threadRet);
	if (threadRc != 0) {
		INTEG_TEST_PRINT( "pthread_join failed\n");
		return -1;
	}

	rc =*((uint32_t *)*&threadRet);
    SaSi_PalMemFree(threadRet);
	threadRc = pthread_attr_destroy(&threadAttr);
	if (threadRc != 0) {
		INTEG_TEST_PRINT("pthread_attr_destroy failed\n");
	}

	return rc;
}
#endif

int main(void)
{
	int ret = 0;

	/*Perform memory mapping*/
	ret = integration_tests_setup();
	if (ret != 0)
	{
		INTEG_TEST_PRINT("integration_tests_setup failed\n");
		return ret;
	}

#ifdef DX_LINUX_PLATFORM
	ret = rsa_wrap_tests(rsa_tests);
#else
	ret = rsa_tests();
#endif
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in rsa_tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}

	INTEG_TEST_PRINT("All tests passed \n");

exit_0:

	integration_tests_clear();

	return ret;
}


