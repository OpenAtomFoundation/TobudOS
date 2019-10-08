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
#include "crys_chacha.h"
#include "crys_chacha_poly.h"
#include "crys_common.h"
#include "integration_test_plat_defs.h"
#include "integration_test_ssi_defs.h"
#include "integration_test_ssi_data.h"
#ifdef DX_LINUX_PLATFORM /*for linux platform only !!*/
#include <pthread.h>
#endif

/*RNG Global variables*/
extern CRYS_RND_State_t*   rndState_ptr;
extern CRYS_RND_WorkBuff_t*  rndWorkBuff_ptr;




/* Random data to be set in the beginning of the test */


extern CHACHA_DataStruct chachaTestTable [];
extern CHACHA_POLY_DataStruct_t chachaPolyTestTable [];




int chacha_tests(void)
{
        CRYSError_t err = CRYS_OK;
        uint32_t totalRc = 0;
        uint32_t test_index , j;

        uint8_t dataInBuff[MAX_TEST_DATA_SIZE];
        uint8_t crysDataOutBuff[MAX_TEST_DATA_SIZE];
        uint32_t dataLen;

        /* test integrated mode */
        for (test_index = 0; test_index < CHACHA_TESTS_NUMBER; test_index++)
        {

            dataLen = chachaTestTable[test_index].dataInSize;
            SaSi_PalMemSetZero(dataInBuff,MAX_TEST_DATA_SIZE);
            SaSi_PalMemSetZero(crysDataOutBuff,MAX_TEST_DATA_SIZE);
            SaSi_PalMemCopy(dataInBuff, chachaTestTable[test_index].pDataIn, dataLen);

            err = CRYS_CHACHA(
                        chachaTestTable[test_index].pIVCounter,
                        chachaTestTable[test_index].IVSize,
                        chachaTestTable[test_index].pKey,
                        chachaTestTable[test_index].initialCounter,
                        chachaTestTable[test_index].encryptDecryptFlag,
                        dataInBuff,
                        chachaTestTable[test_index].dataInSize,
                        crysDataOutBuff);

                if (err != chachaTestTable[test_index].expectedRc) {
                        INTEG_TEST_PRINT("%s CRYS_CHACHA failure: actual=0x%x, expected=0x%x", (uint32_t)chachaTestTable[test_index].testName, err, chachaTestTable[test_index].expectedRc);
                        totalRc += 1;
                }
                else if ((err == CRYS_OK) && (SaSi_PalMemCmp(crysDataOutBuff, chachaTestTable[test_index].pExpectedDataOut, chachaTestTable[test_index].dataInSize) != 0)) {
                        INTEG_TEST_PRINT("%s CRYS_CHACHA enc/dec cmp error", (uint32_t)chachaTestTable[test_index].testName);
                        totalRc += 1;
                }
                else {
                        INTEG_TEST_PRINT("%s (integrated) passed\n", (uint32_t)chachaTestTable[test_index].testName);
                }
        }

        /* test non-integrated mode */
         for (test_index = 0; test_index < CHACHA_TESTS_NUMBER; test_index++)
         {
                CRYS_CHACHAUserContext_t userContext;
                uint32_t blockSize = CRYS_CHACHA_BLOCK_SIZE_IN_BYTES;

                dataLen = chachaTestTable[test_index].dataInSize;

                SaSi_PalMemSetZero(dataInBuff,MAX_TEST_DATA_SIZE);
                SaSi_PalMemSetZero(crysDataOutBuff,MAX_TEST_DATA_SIZE);


                if (chachaTestTable[test_index].expectedRc != CRYS_OK) {
                        continue;
                }

                SaSi_PalMemCopy(dataInBuff, chachaTestTable[test_index].pDataIn, dataLen);
                SaSi_PalMemSetZero(&userContext, sizeof(userContext));

                err = CRYS_CHACHA_Init(&userContext,
                                        chachaTestTable[test_index].pIVCounter,
                                        chachaTestTable[test_index].IVSize,
                                        chachaTestTable[test_index].pKey,
                                        chachaTestTable[test_index].initialCounter,
                                        chachaTestTable[test_index].encryptDecryptFlag);
                if (err != CRYS_OK) {
                        INTEG_TEST_PRINT("%s CRYS_CHACHA_Init failure: err=0x%x\n", (uint32_t)chachaTestTable[test_index].testName, err);
                        totalRc += 1;
                        continue;
                }

                for (j = 0; j < (blockSize * (dataLen / blockSize)); j += blockSize) {
                        err = CRYS_CHACHA_Block(&userContext, dataInBuff + j, blockSize, crysDataOutBuff + j);
                        if (err != CRYS_OK) {
                                INTEG_TEST_PRINT("%s CRYS_CHACHA_Block (%d) failure: err=0x%x\n", (uint32_t)chachaTestTable[test_index].testName, j, err);
                                totalRc += 1;
                                break;
                        }
                }
                if (err != CRYS_OK) {
                        continue;
                }
                if ((dataLen % blockSize) > 0) {
                        blockSize = dataLen % blockSize;
                        err = CRYS_CHACHA_Finish(&userContext, dataInBuff + j, blockSize, crysDataOutBuff + j);
                        if (err != CRYS_OK) {
                                INTEG_TEST_PRINT("%s CRYS_CHACHA_Finish (%d) failure: err=0x%x\n", (uint32_t)chachaTestTable[test_index].testName, j, err);
                                totalRc += 1;
                                continue;
                        }
                }

                if (SaSi_PalMemCmp(crysDataOutBuff, chachaTestTable[test_index].pExpectedDataOut, chachaTestTable[test_index].dataInSize) != 0) {
                        INTEG_TEST_PRINT( "%s CRYS_CHACHA enc/dec cmp error:", (uint32_t)chachaTestTable[test_index].testName);
                        totalRc += 1;
                        continue;
                }

                err = CRYS_CHACHA_Free(&userContext);
                if (err != CRYS_OK) {
                        INTEG_TEST_PRINT( "%s CRYS_CHACHA_Free failure: err=0x%x\n", (uint32_t)chachaTestTable[test_index].testName, err);
                        totalRc += 1;
                        continue;
                }
                INTEG_TEST_PRINT( "%s (non-integrated) passed\n", (uint32_t)chachaTestTable[test_index].testName);
        }

        /* sum test results */
        /********************/

        INTEG_TEST_PRINT( "Chacha test finished with 0x%x!\n", totalRc);

	return totalRc;
}

int chacha_poly_tests(void)
{
        CRYSError_t err = CRYS_OK;
        uint32_t totalRc = 0;
        uint32_t test_index;
        uint8_t dataInBuff[TEST_CHACHA_POLY_DATA_SIZE_MAX];
        uint8_t addDataBuff[TEST_CHACHA_POLY_DATA_SIZE_MAX];
        uint8_t crysDataOutBuff[TEST_CHACHA_POLY_DATA_SIZE_MAX];
        uint32_t crysMacBuff[CRYS_POLY_MAC_SIZE_IN_WORDS];
        uint32_t refMacBuff[CRYS_POLY_MAC_SIZE_IN_WORDS];
        uint32_t *pMacBuff;
        uint32_t dataLen;
        uint32_t addDataLen;

        /* test integrated mode */
        for (test_index = 0; test_index < CHACHA_POLY_TESTS_NUMBER; test_index++)
        {
            CRYS_COMMON_ConvertLsbMsbBytesToLswMswWords(refMacBuff, chachaPolyTestTable[test_index].pExpectedMac, CRYS_POLY_MAC_SIZE_IN_BYTES);

            dataLen = chachaPolyTestTable[test_index].dataInSize;
            addDataLen = chachaPolyTestTable[test_index].addDataSize;

            SaSi_PalMemSetZero(dataInBuff,TEST_CHACHA_POLY_DATA_SIZE_MAX);
            SaSi_PalMemSetZero(addDataBuff,TEST_CHACHA_POLY_DATA_SIZE_MAX);
            SaSi_PalMemSetZero(addDataBuff,TEST_CHACHA_POLY_DATA_SIZE_MAX);
            SaSi_PalMemSetZero(crysDataOutBuff,TEST_CHACHA_POLY_DATA_SIZE_MAX);
            SaSi_PalMemCopy(dataInBuff, chachaPolyTestTable[test_index].pDataIn, dataLen);
            SaSi_PalMemCopy(addDataBuff, chachaPolyTestTable[test_index].pAddData, addDataLen);

	        if (chachaPolyTestTable[test_index].encryptDecryptFlag == CRYS_CHACHA_Encrypt) {
	        	pMacBuff = crysMacBuff;
	        } else {
	        	pMacBuff = refMacBuff;
	        }
                err = CRYS_CHACHA_POLY(
                        chachaPolyTestTable[test_index].pNonce,
                        chachaPolyTestTable[test_index].pKey,
                        chachaPolyTestTable[test_index].encryptDecryptFlag,
                        addDataBuff, addDataLen,
                        dataInBuff, dataLen,
                        crysDataOutBuff, pMacBuff);

                if (err != chachaPolyTestTable[test_index].expectedRc) {
                        INTEG_TEST_PRINT("%s CRYS_CHACHA_POLY failure: actual=0x%x, expected=0x%x",
				(uint32_t)chachaPolyTestTable[test_index].testName, err, chachaPolyTestTable[test_index].expectedRc);
                        totalRc += 1;
                } else if (err == CRYS_OK) {
			if((SaSi_PalMemCmp((uint8_t *)crysDataOutBuff, chachaPolyTestTable[test_index].pExpectedDataOut, dataLen)) != 0) {
				INTEG_TEST_PRINT("%s CRYS_CHACHA_POLY enc/dec cmp error\n", (uint32_t)chachaPolyTestTable[test_index].testName);
				totalRc += 1;
			} else {
				INTEG_TEST_PRINT("%s CRYS_CHACHA_POLY enc/dec cmp OK\n", (uint32_t)chachaPolyTestTable[test_index].testName);
			}
			if(chachaPolyTestTable[test_index].encryptDecryptFlag == CRYS_CHACHA_Encrypt) {
				if (SaSi_PalMemCmp((uint8_t *)pMacBuff, chachaPolyTestTable[test_index].pExpectedMac, CRYS_POLY_MAC_SIZE_IN_BYTES) != 0) {
					INTEG_TEST_PRINT("%s CRYS_CHACHA_POLY MAC cmp error\n", (uint32_t)chachaPolyTestTable[test_index].testName);
					totalRc += 1;
				} else {
					INTEG_TEST_PRINT("%s CRYS_CHACHA_POLY MAC cmp OK\n", (uint32_t)chachaPolyTestTable[test_index].testName);
				}
			}
                } else {
                        INTEG_TEST_PRINT("%s (integrated) passed\n", (uint32_t)chachaPolyTestTable[test_index].testName);
                }
        }
        /* sum test results */
        /********************/
        INTEG_TEST_PRINT("Chacha-poly test finished with 0x%x!\n", totalRc);

	return totalRc;
}

#ifdef DX_LINUX_PLATFORM /*for linux platform only  -> we need to use contiguous memory for stack !!*/
void* chacha_poly_thread(void)
{
   uint32_t* threadReturnValue = SaSi_PalMemMalloc(sizeof(uint32_t));

   *threadReturnValue =chacha_tests();
    if (*threadReturnValue != SA_SILIB_RET_OK) {
        INTEG_TEST_PRINT("Failure in chacha_tests,ret = 0x%x\n", *threadReturnValue);
        goto exit;
    }
	/*Call chacha poly test*/

	*threadReturnValue = chacha_poly_tests();
	if (*threadReturnValue != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in chacha_poly_tests,ret = 0x%x\n", *threadReturnValue);
	    goto exit;
	}

exit:
	pthread_exit(threadReturnValue);
}

/*aes_ccm_wrap_tests creates thread with defined stack address to and calls to AES CCM test */
int chacha_poly_wrap_tests(void)
{
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
	threadRc = pthread_create( &threadId, &threadAttr, (void *)chacha_poly_thread, &num);
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
    if(ret) {
        INTEG_TEST_PRINT("Failed SaSi_LibInit - ret = 0x%x\n", ret);
    }
    ret = CRYS_RndInit(rndState_ptr, rndWorkBuff_ptr);
    if (ret) {
        INTEG_TEST_PRINT("Failed CRYS_RndInit - ret = 0x%x\n", ret);
	    goto exit_1;
	}

     /*Call chacha test*/
#ifdef DX_LINUX_PLATFORM
	ret = chacha_poly_wrap_tests(); /*Call wrap function to create thread and to define stack's address to use contiguous memory*/
    if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}
#else
	ret = chacha_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in chacha_tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}
	ret = chacha_poly_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in chacha_poly_tests,ret = 0x%x\n", ret);
	    goto exit_0;
	}
#endif

	INTEG_TEST_PRINT("All tests passed \n");

exit_0:
	/*Finish SaSi library*/
    SaSi_LibFini();
    ret = CRYS_RND_UnInstantiation(rndState_ptr);
    if (ret)
    {
        INTEG_TEST_PRINT("Failure in CRYS_RND_UnInstantiation,ret = 0x%x\n", ret);
    }
exit_1:
	integration_tests_clear();

	return ret;
}



