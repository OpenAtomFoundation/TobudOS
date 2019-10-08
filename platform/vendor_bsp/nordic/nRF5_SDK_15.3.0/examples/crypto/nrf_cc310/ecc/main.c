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
#include "crys_ecpki_build.h"
#include "crys_ecpki_ecdsa.h"
#include "crys_ecpki_dh.h"
#include "crys_ecpki_kg.h"
#include "crys_ecpki_domain.h"
#include "integration_test_plat_defs.h"
#include "integration_test_ssi_defs.h"
#include "integration_test_ssi_data.h"

#ifdef DX_LINUX_PLATFORM /*for linux platform only !!*/
#include <pthread.h>
#endif

/*ECC test data vectors*/
extern eccSignDataStuct eccSignVectors[];
extern eccDHDataStuct eccDHVectors[];

/*RNG Global variables*/
extern CRYS_RND_State_t*     rndState_ptr;
extern CRYS_RND_WorkBuff_t*  rndWorkBuff_ptr;



 int ecc_DH_tests(void)
{
	uint32_t                        ret = 0;
	int                             test_index;
	uint8_t                         pubKeyBuff1[ECC_KEY_MAX_LENGHT];
	uint8_t                         privKeyBuff1[ECC_KEY_MAX_LENGHT];
	uint8_t                         pubKeyBuff2[ECC_KEY_MAX_LENGHT];
	uint8_t                         privKeyBuff2[ECC_KEY_MAX_LENGHT];
	CRYS_ECPKI_UserPrivKey_t        UserPrivKey1;
	CRYS_ECPKI_UserPublKey_t        UserPublKey1;
	CRYS_ECPKI_UserPrivKey_t        UserPrivKey2;
	CRYS_ECPKI_UserPublKey_t        UserPublKey2;
 	CRYS_ECPKI_BUILD_TempData_t     TempBuff;
 	CRYS_ECDH_TempData_t            *TempDHBuffptr;
	CRYS_ECPKI_KG_TempData_t        *TempECCKGBuffptr;
	CRYS_ECPKI_KG_FipsContext_t   	FipsBuff;
	uint8_t                         sharedSecret1ptr[SHARED_SECRET_MAX_LENGHT];
	uint8_t                         sharedSecret2ptr[SHARED_SECRET_MAX_LENGHT];
	const CRYS_ECPKI_Domain_t       *pDomain;
	CRYS_ECDH_TempData_t            TempDHBuff;
	CRYS_ECPKI_KG_TempData_t        TempECCKGBuff;
	uint32_t                        sharedSecret1Size = SHARED_SECRET_MAX_LENGHT;
	uint32_t                        sharedSecret2Size = SHARED_SECRET_MAX_LENGHT;
	SaSiRndGenerateVectWorkFunc_t 	rndGenerateVectFunc = CRYS_RND_GenerateVector;

	/*Run all ECC tests*/
	for (test_index = 0; test_index < ECC_DH_TESTS_NUMBER; test_index++)
	{

        SaSi_PalMemSetZero(pubKeyBuff1,ECC_KEY_MAX_LENGHT);
        SaSi_PalMemSetZero(privKeyBuff1,ECC_KEY_MAX_LENGHT);
        SaSi_PalMemSetZero(pubKeyBuff2,ECC_KEY_MAX_LENGHT);
        SaSi_PalMemSetZero(privKeyBuff2,ECC_KEY_MAX_LENGHT);
        SaSi_PalMemSetZero(sharedSecret1ptr,SHARED_SECRET_MAX_LENGHT);
        SaSi_PalMemSetZero(sharedSecret2ptr,SHARED_SECRET_MAX_LENGHT);
		/*Copy public key1 data to pubKeyBuff buffer*/
		SaSi_PalMemCopy(pubKeyBuff1, eccDHVectors[test_index].eccDHTest_PubKey1, eccDHVectors[test_index].eccDHTest_PubKeySize1);
		/*Copy private key1 data to pubKeyBuff buffer*/
		SaSi_PalMemCopy(privKeyBuff1, eccDHVectors[test_index].eccDHTest_PrivKey1, eccDHVectors[test_index].eccDHTest_PrivKeySize1);
		/*Copy public key2 data to pubKeyBuff buffer*/
		SaSi_PalMemCopy(pubKeyBuff2, eccDHVectors[test_index].eccDHTest_PubKey2, eccDHVectors[test_index].eccDHTest_PubKeySize2);
		/*Copy private key2 data to pubKeyBuff buffer*/
		SaSi_PalMemCopy(privKeyBuff2, eccDHVectors[test_index].eccDHTest_PrivKey2, eccDHVectors[test_index].eccDHTest_PrivKeySize2);

		TempDHBuffptr = (CRYS_ECDH_TempData_t*)&TempDHBuff;
		TempECCKGBuffptr = (CRYS_ECPKI_KG_TempData_t*)&TempECCKGBuff;

        INTEG_TEST_PRINT("ECC DH Test number 0x%x Parameters : \n-----%s-----\n \n",test_index,(uint32_t)eccDHVectors[test_index].eccDHTest_name);
        pDomain = CRYS_ECPKI_GetEcDomain(eccDHVectors[test_index].eccDHTest_DomainID);

		if (test_index > 0){
			/*Generate first key pair*/
			ret = CRYS_ECPKI_GenKeyPair (rndState_ptr, rndGenerateVectFunc,
				pDomain,
				&UserPrivKey1,
				&UserPublKey1,
				TempECCKGBuffptr,&FipsBuff);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_GenKeyPair for key pair 1 failed with 0x%x \n",ret);
				return ret;
			}

			/*Generate second key pair*/
			ret = CRYS_ECPKI_GenKeyPair (rndState_ptr, rndGenerateVectFunc,
				pDomain,
				&UserPrivKey2,
				&UserPublKey2,
				TempECCKGBuffptr,&FipsBuff);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_GenKeyPair key pair 1 failed with 0x%x \n",ret);
				return ret;
			}


		} else {
			/*Call CRYS_ECPKI_BuildPrivKey to create private key1 from external key buffer*/
			ret = CRYS_ECPKI_BuildPrivKey(pDomain,
				privKeyBuff1,
				eccDHVectors[test_index].eccDHTest_PrivKeySize1,
				&UserPrivKey1);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_BuildPrivKey for key 1 failed with 0x%x \n",ret);
				return ret;
			}

			/*Call CRYS_ECPKI_BuildPublKeyFullCheck to create public key1 from external key buffer*/
			ret = CRYS_ECPKI_BuildPublKeyFullCheck(pDomain,
				pubKeyBuff1,
				eccDHVectors[test_index].eccDHTest_PubKeySize1,
				&UserPublKey1,
				&TempBuff);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_BuildPublKeyFullCheck for key 1 failed with 0x%x \n",ret);
				return ret;
			}

			/*Call CRYS_ECPKI_BuildPrivKey to create private key2 from external key buffer*/
			ret = CRYS_ECPKI_BuildPrivKey(pDomain,
				privKeyBuff2,
				eccDHVectors[test_index].eccDHTest_PrivKeySize2,
				&UserPrivKey2);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_BuildPrivKey for key 2 failed with 0x%x \n",ret);
				return ret;
			}

			/*Call CRYS_ECPKI_BuildPublKeyFullCheck to create public key2 from external key buffer*/
			ret = CRYS_ECPKI_BuildPublKeyFullCheck(pDomain,
				pubKeyBuff2,
				eccDHVectors[test_index].eccDHTest_PubKeySize2,
				&UserPublKey2,
				&TempBuff);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_BuildPublKeyFullCheck for key 2 failed with 0x%x \n",ret);
				return ret;
			}
		}

		/* Generating the Secret for user 1*/
		/*---------------------------------*/
		ret = CRYS_ECDH_SVDP_DH(&UserPublKey2,
			&UserPrivKey1,
			sharedSecret1ptr,
			&sharedSecret1Size,
			TempDHBuffptr);

		if (ret != SA_SILIB_RET_OK){
			INTEG_TEST_PRINT(" CRYS_ECDH_SVDP_DH for secret 1 failed with 0x%x \n",ret);
			return ret;
		}

		/* Generating the Secret for user 2*/
		/*---------------------------------*/
		ret = CRYS_ECDH_SVDP_DH(&UserPublKey1,
			&UserPrivKey2,
			sharedSecret2ptr,
			&sharedSecret2Size,
			TempDHBuffptr);

		if (ret != SA_SILIB_RET_OK){
			INTEG_TEST_PRINT(" CRYS_ECDH_SVDP_DH for secret 2 failed with 0x%x \n",ret);
			return ret;
		}

		/* Verifying we have the same Secret Key both for user1 & user2   */
		/*----------------------------------------------------------------*/
		ret = (CRYSError_t)SaSi_PalMemCmp(sharedSecret1ptr ,
			sharedSecret2ptr,
			sharedSecret2Size);

		if (ret != SA_SILIB_RET_OK){
			INTEG_TEST_PRINT(" SaSi_PalMemCmp of shared secrets failed");
			return ret;
		}

	    INTEG_TEST_PRINT("Test 0x%x passed \n",test_index);
	}
	INTEG_TEST_PRINT("All DH tests passed \n=======================\n");

return ret;
}

int ecc_sign_tests(void)
{
	uint32_t                        ret = 0;
	int                             test_index;
	uint8_t                         dataInBuff[MAX_TEST_DATA_SIZE];
	CRYS_ECDH_TempData_t            signOutBuff;
	uint8_t                         pubKeyBuff[MAX_TEST_DATA_SIZE];
	uint8_t                         privKeyBuff[MAX_TEST_DATA_SIZE];
	CRYS_ECPKI_UserPrivKey_t        UserPrivKey;
	CRYS_ECPKI_UserPublKey_t        UserPublKey;
 	CRYS_ECPKI_BUILD_TempData_t     TempBuff;
	CRYS_ECDSA_SignUserContext_t    SignUserContext;
	CRYS_ECDSA_VerifyUserContext_t  VerifyUserContext;
	CRYS_ECPKI_KG_TempData_t        TempECCKGBuff;
	CRYS_ECPKI_KG_FipsContext_t   	FipsBuff;
	const CRYS_ECPKI_Domain_t       *pDomain;
	SaSiRndGenerateVectWorkFunc_t 	rndGenerateVectFunc = CRYS_RND_GenerateVector;


	/*Run all ECC tests*/
	for (test_index = 0; test_index < ECC_SIGN_TESTS_NUMBER; test_index++)
	{

        SaSi_PalMemSetZero(dataInBuff,MAX_TEST_DATA_SIZE);
        SaSi_PalMemSetZero(pubKeyBuff,MAX_TEST_DATA_SIZE);
        SaSi_PalMemSetZero(privKeyBuff,MAX_TEST_DATA_SIZE);
        SaSi_PalMemSetZero(&signOutBuff,sizeof(CRYS_ECDH_TempData_t));
        SaSi_PalMemSetZero(&TempECCKGBuff,sizeof(TempECCKGBuff));

		/*Copy input text data to input buffer*/
		SaSi_PalMemCopy(dataInBuff, eccSignVectors[test_index].eccTest_InputData, eccSignVectors[test_index].eccTest_InputDataSize);
		/*Copy public key data to pubKeyBuff buffer*/
		SaSi_PalMemCopy(pubKeyBuff, eccSignVectors[test_index].eccTest_PubKey, eccSignVectors[test_index].eccTest_PubKeySize);
		/*Copy private key data to pubKeyBuff buffer*/
		SaSi_PalMemCopy(privKeyBuff, eccSignVectors[test_index].eccTest_PrivKey, eccSignVectors[test_index].eccTest_PrivKeySize);

		INTEG_TEST_PRINT("ECC Sign Test numer 0x%x Parameters : \n-----%s-----\n \n",test_index,(uint32_t)eccSignVectors[test_index].eccTest_name);
        pDomain = CRYS_ECPKI_GetEcDomain(eccDHVectors[test_index].eccDHTest_DomainID);

		if (test_index > 0){
			/*Generate first key pair*/
			ret = CRYS_ECPKI_GenKeyPair (rndState_ptr, rndGenerateVectFunc,
				pDomain,
				&UserPrivKey,
				&UserPublKey,
				&TempECCKGBuff,&FipsBuff);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_GenKeyPair for key pair 1 failed with 0x%x \n",ret);
				return ret;
			}
		} else {
			/*Call CRYS_ECPKI_BuildPrivKey to create private key from external key buffer*/
			ret = CRYS_ECPKI_BuildPrivKey(pDomain,
				privKeyBuff,
				eccSignVectors[test_index].eccTest_PrivKeySize,
				&UserPrivKey);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_BuildPrivKey failed with 0x%x \n",ret);
				return ret;
			}

			/*Call CRYS_ECPKI_BuildPublKeyFullCheck to create public key from external key buffer*/
			ret = CRYS_ECPKI_BuildPublKeyFullCheck(pDomain,
				pubKeyBuff,
				eccSignVectors[test_index].eccTest_PubKeySize,
				&UserPublKey,
				&TempBuff);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" CRYS_ECPKI_BuildPublKeyFullCheck failed with 0x%x \n",ret);
				return ret;
			}
		}

		/*Call CRYS_ECDSA_Sign to create signature from input buffer using created private key*/
		ret = CRYS_ECDSA_Sign (rndState_ptr, rndGenerateVectFunc,
			&SignUserContext,
			&UserPrivKey,
			eccSignVectors[test_index].eccTest_HashMode,
			dataInBuff,
			eccSignVectors[test_index].eccTest_InputDataSize,
			(uint8_t*)&signOutBuff,
			&eccSignVectors[test_index].eccTest_SignitureSize);

		if (ret != SA_SILIB_RET_OK){
			INTEG_TEST_PRINT(" CRYS_ECDSA_Sign failed with 0x%x \n",ret);
			return ret;
		}

		/*Call CRYS_ECDSA_Verify to verify the signature using created public key*/
		ret =  CRYS_ECDSA_Verify (&VerifyUserContext,
			&UserPublKey,
			eccSignVectors[test_index].eccTest_HashMode,
			(uint8_t*)&signOutBuff,
			eccSignVectors[test_index].eccTest_SignitureSize,
			dataInBuff,
			eccSignVectors[test_index].eccTest_InputDataSize);

		if (ret != SA_SILIB_RET_OK){
			INTEG_TEST_PRINT(" CRYS_ECDSA_Verify failed with 0x%x \n",ret);
			return ret;
		}
		INTEG_TEST_PRINT(" Test 0x%x passed \n",test_index);
	}
	INTEG_TEST_PRINT(" All sign tests Passed \n=======================\n");

return ret;
}


#ifdef DX_LINUX_PLATFORM /*for linux platform only  -> we need to use contiguous memory for stack !!*/
/*ecc_wrap_tests creates thread with defined stack address to and calls to ecc tests */
void* ecc_thread(void)
{
   uint32_t* threadReturnValue = SaSi_PalMemMalloc(sizeof(uint32_t));

   *threadReturnValue =ecc_sign_tests();
    if (*threadReturnValue != SA_SILIB_RET_OK) {
        INTEG_TEST_PRINT("Failure in ecc_sign_tests,ret = 0x%x\n", *threadReturnValue);
        goto exit;
    }
	/*Call chacha poly test*/

	*threadReturnValue = ecc_DH_tests();
	if (*threadReturnValue != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in ecc_DH_tests,ret = 0x%x\n", *threadReturnValue);
	    goto exit;
	}

exit:
	pthread_exit(threadReturnValue);
}

int ecc_wrap_tests(void){
	uint32_t rc = 0;

	pthread_t threadId;
	pthread_attr_t threadAttr;
	int threadRc;
	void *threadRet;
	int num = 6;

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
	threadRc = pthread_create( &threadId, &threadAttr, (void *)ecc_thread, &num);
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

#ifdef DX_LINUX_PLATFORM
    ret = ecc_wrap_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in ecc tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}
#else
        /*Call ecc test*/
	ret = ecc_sign_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in ecc_sign_tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}

	ret = ecc_DH_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in ecc_DH_tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}
#endif

	INTEG_TEST_PRINT("All tests passed \n");

exit_0:
	/*Finish SaSi library*/
    SaSi_LibFini();

    ret = CRYS_RND_UnInstantiation(rndState_ptr);

    if (ret) {
        INTEG_TEST_PRINT("Failure in CRYS_RND_UnInstantiation,ret = 0x%x\n", ret);
    }

exit_1:
	integration_tests_clear();

	return ret;
}


