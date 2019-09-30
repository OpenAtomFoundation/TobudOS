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
#include "integration_test_plat_defs.h"
#include "integration_test_ssi_data.h"
#include "integration_test_ssi_defs.h"
#ifdef DX_LINUX_PLATFORM
#include <pthread.h>
#endif

/*HMAC test data*/
extern hmacDataStuct hmacVectors[];

/*RND global variables*/
extern CRYS_RND_State_t*     rndState_ptr;
extern CRYS_RND_WorkBuff_t*  rndWorkBuff_ptr;




int hmac_tests(void)
{
	uint32_t                        ret = 0;
	int                             test_index,operation_index;
	uint8_t                         dataInBuff[MAX_TEST_DATA_SIZE];
    uint8_t                         keyptr[CRYS_HMAC_KEY_SIZE_IN_BYTES];
    CRYS_HASH_Result_t              hmacOutBuff = {0};
	CRYS_HMACUserContext_t           ContextID;

	/*Run all HMAC tests*/
	for(operation_index = INTEGRATED_OPERATION; operation_index <= NON_INTEGRATED_OPERATION; operation_index++){
		for (test_index = 0; test_index < HMAC_TESTS_NUMBER; test_index++)
		{

            SaSi_PalMemSetZero(dataInBuff,MAX_TEST_DATA_SIZE);
            SaSi_PalMemSetZero(keyptr,CRYS_HMAC_KEY_SIZE_IN_BYTES);
            /*Copy input text data to input buffer*/
			SaSi_PalMemCopy(dataInBuff, hmacVectors[test_index].hmacTest_InputData, hmacVectors[test_index].hmacTest_InputDataSize);
            SaSi_PalMemCopy(keyptr, hmacVectors[test_index].hmacTest_Key, hmacVectors[test_index].hmacTest_KeySize);

			INTEG_TEST_PRINT("\n HASH Test numer 0x%x Parameters : \n-----%s-----\n",test_index,(uint32_t)hmacVectors[test_index].hmacTest_Name);

			if (operation_index == INTEGRATED_OPERATION){ /*Perform Inegrated operation*/

				INTEG_TEST_PRINT("Integrated operation\n");

				ret = CRYS_HMAC(hmacVectors[test_index].hmacTest_TST_OperationMode,
					keyptr,
					hmacVectors[test_index].hmacTest_KeySize,
					dataInBuff,
					hmacVectors[test_index].hmacTest_InputDataSize,
					hmacOutBuff);

				if (ret != SA_SILIB_RET_OK){
					INTEG_TEST_PRINT(" CRYS_HMAC failed with 0x%x \n",ret);
					goto end;
				}
			} else { /*Perform NonInegrated operation*/

				INTEG_TEST_PRINT("Non integrated operation\n");


				ret = CRYS_HMAC_Init(&ContextID,
					hmacVectors[test_index].hmacTest_TST_OperationMode,
					keyptr,
					hmacVectors[test_index].hmacTest_KeySize);

				if (ret != SA_SILIB_RET_OK){
					INTEG_TEST_PRINT(" CRYS_HMAC_Init failed with 0x%x \n",ret);
					goto end;
				}

				ret = CRYS_HMAC_Update(&ContextID,
					dataInBuff,
					hmacVectors[test_index].hmacTest_InputDataSize);

				if (ret != SA_SILIB_RET_OK){
					INTEG_TEST_PRINT(" CRYS_HMAC_Update failed with 0x%x \n",ret);
					goto end;
				}

				ret = CRYS_HMAC_Finish(&ContextID,
					hmacOutBuff);

				if (ret != SA_SILIB_RET_OK){
					INTEG_TEST_PRINT(" CRYS_HMAC_Finish failed with 0x%x \n",ret);
					goto end;
				}


			}

			ret = SaSi_PalMemCmp(hmacOutBuff,hmacVectors[test_index].hmacTest_ExpOutData,hmacVectors[test_index].hmacTest_ExpOutDataSize);

			if (ret != SA_SILIB_RET_OK){
				INTEG_TEST_PRINT(" SaSi_PalMemCmp failed \n");
				goto end;
			}

			INTEG_TEST_PRINT("Passed\n");
			INTEG_TEST_PRINT("==========================\n");
		}
	}

end:

	return ret;
}

#ifdef DX_LINUX_PLATFORM /*for linux platform only  -> we need to use contiguous memory for stack !!*/
/*hmac_wrap_tests creates thread with defined stack address to and calls to AES HMAC test */
void* hash_thread(void)
{
   uint32_t* threadReturnValue = SaSi_PalMemMalloc(sizeof(uint32_t));

   *threadReturnValue = hmac_tests();
    if (*threadReturnValue != SA_SILIB_RET_OK) {
        INTEG_TEST_PRINT("Failure in hmac_tests,ret = 0x%x\n", *threadReturnValue);
        goto exit;
    }

exit:
	pthread_exit(threadReturnValue);
}

int hmac_wrap_tests(void){
	uint32_t rc = 0;

	pthread_t threadId;
	pthread_attr_t threadAttr;
	int threadRc;
	void *threadRet;
	//int num = 6;

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
	threadRc = pthread_create( &threadId, &threadAttr, (void *)hash_thread, NULL);
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

    /*Call hmac test*/
#ifdef DX_LINUX_PLATFORM
	ret = hmac_wrap_tests(); /*Call wrap function to create thread and to define stack's address to use contiguous memory*/
#else
	ret = hmac_tests();
#endif
   	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in ecc_sign_tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}

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


