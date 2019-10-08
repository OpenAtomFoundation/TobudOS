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
#ifdef DX_LINUX_PLATFORM /*for linux platform only !!*/
#include <pthread.h>
#endif

#include "ssi_pal_types.h"
#include "ssi_pal_mem.h"
#include "sns_silib.h"
#include "ssi_aes.h"
#include "crys_aesccm.h"
#include "integration_test_plat_defs.h"
#include "integration_test_ssi_data.h"
#include "integration_test_ssi_defs.h"

/*AES test data vectors*/
extern aesDataStuct aesVectors[];
extern aesCCMDataStuct aesCCMVectors[];

/*Globals*/
extern CRYS_RND_State_t*     rndState_ptr;
extern CRYS_RND_WorkBuff_t*  rndWorkBuff_ptr;

int aes_ccm_tests(void)
{
	uint32_t     ret = 1;
	int test_index;
	uint8_t dataInBuff[MAX_AES_CCM_DATA_SIZE_VECTOR];
	uint8_t dataOutBuff[MAX_AES_CCM_DATA_SIZE_VECTOR];
	uint8_t aDataBuff[MAX_AES_CCM_DATA_SIZE_VECTOR];
	uint8_t macOutRes[MAX_AES_CCM_DATA_SIZE_VECTOR];
	uint8_t nonce[MAX_AES_CCM_DATA_SIZE_VECTOR];

	for (test_index = 0;test_index < AES_CCM_TESTS_NUMBER ;test_index++)
	{

        SaSi_PalMemSetZero(dataInBuff,MAX_AES_CCM_DATA_SIZE_VECTOR);
        SaSi_PalMemSetZero(dataOutBuff,MAX_AES_CCM_DATA_SIZE_VECTOR);
        SaSi_PalMemSetZero(aDataBuff,MAX_AES_CCM_DATA_SIZE_VECTOR);
        SaSi_PalMemSetZero(macOutRes,MAX_AES_CCM_DATA_SIZE_VECTOR);
        SaSi_PalMemSetZero(nonce,MAX_AES_CCM_DATA_SIZE_VECTOR);
		SaSi_PalMemCopy(dataInBuff, aesCCMVectors[test_index].aesCCMTest_TextData, aesCCMVectors[test_index].aesCCMTest_TextdataSize);
		/*Copy input additional data to input buffer*/
		SaSi_PalMemCopy(aDataBuff, aesCCMVectors[test_index].aesCCMTest_AddData, aesCCMVectors[test_index].aesCCMTest_AddDataSize);
		/*Copy nonce data to nonce buffer*/
		SaSi_PalMemCopy(nonce, aesCCMVectors[test_index].aesCCMTest_NData, aesCCMVectors[test_index].aesCCMTest_NDataSize /*7*/);



		INTEG_TEST_PRINT(" Test Parameters : %s \n",(uint32_t)aesCCMVectors[test_index].aesCCMTest_name);

		/*Call CRYS_AESCCM integrated function*/
		ret = CRYS_AESCCM(SASI_AES_ENCRYPT,
			aesCCMVectors[test_index].aesCCMTest_Key,
			aesCCMVectors[test_index].aesCCMTest_KeySize,
			nonce,//aesCCMVectors[test_index].aesCCMTest_NData,
			aesCCMVectors[test_index].aesCCMTest_NDataSize,
			aDataBuff,//aesCCMVectors[test_index].aesCCMTest_AddData,,
			aesCCMVectors[test_index].aesCCMTest_AddDataSize,
			dataInBuff,//aesCCMVectors[test_index].aesCCMTest_TextData,
			aesCCMVectors[test_index].aesCCMTest_TextdataSize,
			dataOutBuff,//aesCCMVectors[test_index]  uint8_t *TextDataOut_ptr,
			aesCCMVectors[test_index].aesCCMTest_Output_CCM_MACSize,
			macOutRes /*aesCCMVectors[test_index]  CRYS_AESCCM_Mac_Res_t MacRes*/);

		if (ret != SA_SILIB_RET_OK){
			INTEG_TEST_PRINT(" CRYS_AESCCM failed with 0x%x \n",ret);
			goto aesTester_end;
		}
		/*Check output text data with expected data*/
		ret = SaSi_PalMemCmp(dataOutBuff,aesCCMVectors[test_index].aesCCMTest_Expected_output_data,aesCCMVectors[test_index].aesCCMTest_TextdataSize);
		if (ret != 0){
			INTEG_TEST_PRINT("output text buffer of the test failed");
			goto aesTester_end;
		}
		/*Check mac output with expected data*/
		ret = SaSi_PalMemCmp(macOutRes,aesCCMVectors[test_index].aesCCMTest_Expected_MAC_output_data,aesCCMVectors[test_index].aesCCMTest_Output_CCM_MACSize);
		if (ret != 0){
			INTEG_TEST_PRINT("output mac buffer of the test failed");
			goto aesTester_end;
		}

	}
	INTEG_TEST_PRINT("Passed \n");
aesTester_end:
	return ret;
}





static bool should_set_iv(SaSiAesOperationMode_t  	operationMode)
{
    return (operationMode == SASI_AES_MODE_CBC) ||
	     (operationMode == SASI_AES_MODE_CTR) ||
	     (operationMode == SASI_AES_MODE_CBC_MAC);
}

int aes_tests(void){

	int ret = 0;
	uint32_t test_index,block_index;
	BufferType_t buffer_index;
	uint8_t dataInBuff[MAX_TEST_DATA_SIZE];
	uint8_t dataOutBuff[MAX_TEST_DATA_SIZE];
    uint8_t *dataOutBuff_ptr = dataOutBuff;
	SaSiAesUserContext_t    ContextID;
	SaSiAesUserKeyData_t    keyData;
    size_t  outSize = 0;
	/*Loop for aes test vectors*/
	for (buffer_index = NON_INPLACE_BUFFER ; buffer_index <= INPLACE_BUFFER; buffer_index++)
		for (test_index = 0; test_index < AES_TESTS_NUMBER; test_index++)
		{


            SaSi_PalMemSetZero(dataInBuff,MAX_TEST_DATA_SIZE);
            SaSi_PalMemSetZero(dataOutBuff,MAX_TEST_DATA_SIZE);
			SaSi_PalMemCopy(dataInBuff, aesVectors[test_index].aesTest_input_data, aesVectors[test_index].aesTest_DataSize);

			INTEG_TEST_PRINT(" Test Parameters : %s \n",(uint32_t)aesVectors[test_index].aesTest_name);

			/*If current iteration is for inplace buffer type, set the output data pointer to input data pointer*
			* to check inplace operation      						      */
			if (buffer_index == INPLACE_BUFFER ){
				/*Perform inplace operation*/
				dataOutBuff_ptr = dataInBuff;
				INTEG_TEST_PRINT(" Inplace buffer operaion \n");
			} else {
				INTEG_TEST_PRINT(" Non-Inplace buffer operaion \n");
			}



   		/*Call non-integrated APIs - first SaSi_AesInit*/
   		ret = SaSi_AesInit(&ContextID,
   			aesVectors[test_index].aesTest_EncDecMode,
   			aesVectors[test_index].aesTest_OperationMode,SASI_AES_PADDING_NONE);

        /*,
            aesVectors[test_index].aesTest_Key,   			aesVectors[test_index].aesTest_KeySize,*/

   		if (ret != SA_SILIB_RET_OK){
   			INTEG_TEST_PRINT("CRYS_AES_Init failed with error 0x%x\n",ret);
   			return ret;
   		}
        if ( should_set_iv(aesVectors[test_index].aesTest_OperationMode)){
            ret = SaSi_AesSetIv(&ContextID, aesVectors[test_index].aesTest_IvCounter);
            if (ret != SA_SILIB_RET_OK){
       			INTEG_TEST_PRINT("SaSi_AesSetIv failed with error 0x%x\n",ret);
       			return ret;
       		}
        }
        keyData.pKey = aesVectors[test_index].TST_KeyData;
        keyData.keySize = aesVectors[test_index].TST_KeySize;
        ret = SaSi_AesSetKey(&ContextID, SASI_AES_USER_KEY, &keyData, sizeof(keyData) );
        if (ret != SA_SILIB_RET_OK){
   			INTEG_TEST_PRINT("SaSi_AesSetKey failed with error 0x%x\n",ret);
   			return ret;
   		}
   		/*Call CRYS_AES_Block for each 16 byte block data of the input buffer*/
   		for (block_index = 0; block_index<aesVectors[test_index].aesTest_DataSize/SASI_AES_BLOCK_SIZE_IN_BYTES-1; block_index++)
   		{
   			ret = SaSi_AesBlock(&ContextID,
   				dataInBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
   				SASI_AES_BLOCK_SIZE_IN_BYTES,
   				dataOutBuff_ptr + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES));

   			if (ret != SA_SILIB_RET_OK){
   				INTEG_TEST_PRINT("CRYS_AES_Block  failed with error 0x%x\n",ret);
   				return ret;
   			}
   		}
        outSize = aesVectors[test_index].aesTest_DataSize - (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES);
        /*Call CRYS_AES_Finish for the last block (the size may be 0)*/
        if (aesVectors[test_index].aesTest_OperationMode == SASI_AES_MODE_XCBC_MAC ||
            aesVectors[test_index].aesTest_OperationMode == SASI_AES_MODE_CMAC){
			/*For XCBC_MAC and CMAC modes we should pass to SaSi_AesFinish the begining of the output buffer*/
					ret = SaSi_AesFinish(&ContextID,
						outSize,
						dataInBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
						outSize,
						dataOutBuff_ptr,
						&outSize);


				} else {
					/*For the rest of the modes we should pass to SaSi_AesFinish the relevant offset of output buffer*/
					ret = SaSi_AesFinish(&ContextID,
						outSize,
						dataInBuff + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
						outSize,
						dataOutBuff_ptr + (block_index*SASI_AES_BLOCK_SIZE_IN_BYTES),
						&outSize);
                }


   		if (ret != SA_SILIB_RET_OK){
   				INTEG_TEST_PRINT("CRYS_AES_Finish  failed with error 0x%x\n",ret);
   				return ret;
   		}


   	/*Check the output buffer with expected data */
   	ret = SaSi_PalMemCmp(dataOutBuff_ptr,aesVectors[test_index].aesTest_Expected_output_data,aesVectors[test_index].aesTest_OutputDataSize);
   	if (ret != 0){
   		INTEG_TEST_PRINT("output buffer of the test failed\n");
   		return ret;
   	}
   	INTEG_TEST_PRINT(" Passed \n");
	}
	return ret;
}

#ifdef DX_LINUX_PLATFORM /*for linux platform only  -> we need to use contiguous memory for stack !!*/
/*aes_ccm_wrap_tests creates thread with defined stack address to and calls to AES CCM test */
void* aes_thread(void)
{
   uint32_t* threadReturnValue = SaSi_PalMemMalloc(sizeof(uint32_t));
   *threadReturnValue =aes_tests();
    if (*threadReturnValue != SA_SILIB_RET_OK) {
        INTEG_TEST_PRINT("Failure in aes_tests,ret = 0x%x\n", *threadReturnValue);
        goto exit;
    }
	/*Call chacha poly test*/

	*threadReturnValue = aes_ccm_tests();
	if (*threadReturnValue != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in aes_ccm_tests,ret = 0x%x\n", *threadReturnValue);
	    goto exit;
	}

exit:
	pthread_exit(threadReturnValue);
}

int aes_wrap_tests(void){
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
	threadRc = pthread_create( &threadId, &threadAttr, (void *)aes_thread, &num);
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
INTEG_TEST_PRINT("****************MAIN***********************\n");
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

	/*Call aes test*/
#ifdef DX_LINUX_PLATFORM
	ret = aes_wrap_tests(); /*Call wrap function to create thread and to define stack's address to use contiguous memory*/
#else
        /*Call aes test*/
	ret = aes_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in aes test,ret = 0x%x\n", ret);
	    goto exit_0;
	}
	ret = aes_ccm_tests();
#endif
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in aes ccm test,ret = 0x%x\n", ret);
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


