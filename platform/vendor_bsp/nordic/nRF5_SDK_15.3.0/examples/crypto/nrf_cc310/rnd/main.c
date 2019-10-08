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
#include "crys_rnd_error.h"
#include "integration_test_plat_defs.h"
#include "integration_test_ssi_data.h"
#include "integration_test_ssi_defs.h"

/*RND test data vector*/
extern rndDataStuct rndVectors[];

/*RND global variables*/
extern CRYS_RND_State_t*     rndState_ptr;
extern CRYS_RND_WorkBuff_t*  rndWorkBuff_ptr;


/*rnd_test - performs basic integration test for RND module*/
int rnd_tests(void)
{
	uint32_t                        ret = 0/*,MaxVectorSize = 0*/;
	int                             test_index = 0;

	/*Set additional input for rng seed*/
	ret = CRYS_RND_AddAdditionalInput(rndState_ptr,
		rndVectors[test_index].rndTest_AddInputData,
		rndVectors[test_index].rndTest_AddInputSize);


	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_AddAdditionalInput failed with 0x%x \n",ret);
		return ret;
	}
	INTEG_TEST_PRINT("\n CRYS_RND_AddAdditionalInput passed\n");

	/*Reseed rnd using added input (new seed will be generated using additional input)*/
	ret = CRYS_RND_Reseeding (rndState_ptr,
		rndWorkBuff_ptr);
	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_Reseeding failed with 0x%x \n",ret);
		return ret;
	}
	INTEG_TEST_PRINT("\n CRYS_RND_Reseeding passed\n");

	/*Generate random vector 1*/
	ret = CRYS_RND_GenerateVector(rndState_ptr,
		rndVectors[test_index].rndTest_RandomVectorSize,
		rndVectors[test_index].rndTest_RandomVectorData1);
	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector for vector 1 failed with 0x%x \n",ret);
		return ret;
	}

	INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector for first vector passed\n");
	/*Generate rnadom vector 2*/
	ret = CRYS_RND_GenerateVector(rndState_ptr,
		rndVectors[test_index].rndTest_RandomVectorSize,
		rndVectors[test_index].rndTest_RandomVectorData2);

	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector for vector 2 failed with 0x%x \n",ret);
		return ret;
	}

	INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector for second vector passed\n");

	/*Compare two generated vectors - should not be the same value*/
	ret = SaSi_PalMemCmp(rndVectors[test_index].rndTest_RandomVectorData1,
		rndVectors[test_index].rndTest_RandomVectorData2,
		rndVectors[test_index].rndTest_RandomVectorSize);

	if (ret == SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n Two random vectors should not are the same \n");
		return ret;
	}

	INTEG_TEST_PRINT("\n Two generated vectors are different as expected\n");

	/*Generate random vector in range when max value is NULL*/
	ret = CRYS_RND_GenerateVectorInRange(rndState_ptr, CRYS_RND_GenerateVector,
		rndVectors[test_index].rndTest_RandomVectorInRangeSize1,
		NULL,
		rndVectors[test_index].rndTest_RandomVectorInRangeData);


	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_GenerateVectorInRange for vector with fixed size failed with 0x%x \n",ret);
		return ret;
	}

	INTEG_TEST_PRINT("\n CRYS_RND_GenerateVectorInRange 1 passed\n");

	/*Generate random vector in range with max vector */
	ret = CRYS_RND_GenerateVectorInRange(rndState_ptr, CRYS_RND_GenerateVector,
		rndVectors[test_index].rndTest_RandomVectorInRangeSize2,
		rndVectors[test_index].rndTest_MaxVectorInRange2,
		rndVectors[test_index].rndTest_RandomVectorInRangeData2);

	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_GenerateVectorInRange failed with 0x%x \n",ret);
		return ret;
	}
	INTEG_TEST_PRINT("\n CRYS_RND_GenerateVectorInRange 2 passed\n");


	/*Perform UnInstantiation*/
	ret = CRYS_RND_UnInstantiation(rndState_ptr);

	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_UnInstantiation failed with 0x%x \n",ret);
		return ret;
	}

	INTEG_TEST_PRINT("\n CRYS_RND_UnInstantiation passed\n");

	/*Try to create random vector without instantiation - should fail*/
	ret = CRYS_RND_GenerateVector(rndState_ptr,
		rndVectors[test_index].rndTest_RandomVectorSize,
		rndVectors[test_index].rndTest_RandomVectorData2);

	if (ret != CRYS_RND_STATE_VALIDATION_TAG_ERROR){
		INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector returned wrong error 0x%x,\n CRYS_RND_STATE_VALIDATION_TAG_ERROR should be returned\n",ret);
		return ret;
	}

	INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector failed as expected\n");

	/*Set additional input for RND seed*/
	ret = CRYS_RND_AddAdditionalInput(rndState_ptr,
		rndVectors[test_index].rndTest_AddInputData,
		rndVectors[test_index].rndTest_AddInputSize);

	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_AddAdditionalInput failed with 0x%x \n",ret);
		return ret;
	}

	INTEG_TEST_PRINT("\n CRYS_RND_AddAdditionalInput passed\n");

	/*Perform instantiation for new seed*/
	ret = CRYS_RND_Instantiation(rndState_ptr,
		rndWorkBuff_ptr);

	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_Instantiation failed with 0x%x \n",ret);
		return ret;
	}
	INTEG_TEST_PRINT("\n CRYS_RND_Instantiation passed\n");

	/*Try to create two vectors and check that the vectors are different*/
	ret = CRYS_RND_GenerateVector(rndState_ptr,
		rndVectors[test_index].rndTest_RandomVectorSize,
		rndVectors[test_index].rndTest_RandomVectorData1);
	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector for vector 1 failed with 0x%x \n",ret);
		return ret;
	}
	INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector 1 passed\n");

	ret = CRYS_RND_GenerateVector(rndState_ptr,
		rndVectors[test_index].rndTest_RandomVectorSize,
		rndVectors[test_index].rndTest_RandomVectorData2);

	if (ret != SA_SILIB_RET_OK){
		INTEG_TEST_PRINT(" CRYS_RND_GenerateVector for vector 2 failed with 0x%x \n",ret);
		return ret;
	}
	INTEG_TEST_PRINT("\n CRYS_RND_GenerateVector 2 passed\n");

	ret = SaSi_PalMemCmp(rndVectors[test_index].rndTest_RandomVectorData1,
		rndVectors[test_index].rndTest_RandomVectorData2,
		rndVectors[test_index].rndTest_RandomVectorSize);

	if (ret == SA_SILIB_RET_OK){
		INTEG_TEST_PRINT(" Two random vectors should not are the same \n");
		return ret;
	}
	INTEG_TEST_PRINT("\n Compare passed\n");
	INTEG_TEST_PRINT("\n All RND tests passed\n");
	INTEG_TEST_PRINT("\n==========================\n");

	ret = SA_SILIB_RET_OK;

return ret;
}

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

        /*Call ecc test*/
	ret = rnd_tests();
	if (ret != SA_SILIB_RET_OK) {
	    INTEG_TEST_PRINT("Failure in rnd_test,ret = 0x%x\n", ret);
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
	/*Unmap memory*/
	integration_tests_clear();

	return ret;
}


