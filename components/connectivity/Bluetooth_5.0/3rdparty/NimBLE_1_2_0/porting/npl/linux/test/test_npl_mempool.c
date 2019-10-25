/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "test_util.h"
#include "nimble/nimble_npl.h"

#define    TEST_MEMPOOL_BLOCKS       4
#define    TEST_MEMPOOL_BLOCK_SIZE   128

static struct ble_npl_mempool s_mempool;

static os_membuf_t s_mempool_mem[OS_MEMPOOL_SIZE(TEST_MEMPOOL_BLOCKS,
						 TEST_MEMPOOL_BLOCK_SIZE)];

static void *s_memblock[TEST_MEMPOOL_BLOCKS];

/**
 * Unit test for initializing a mempool.
 *
 * ble_npl_error_t ble_npl_mempool_init(struct ble_npl_mempool *mp, int blocks,
 *                            int block_size, void *membuf, char *name);
 *
 */
int test_init()
{
    int err;
    err = ble_npl_mempool_init(NULL,
			  TEST_MEMPOOL_BLOCKS,
			  TEST_MEMPOOL_BLOCK_SIZE,
			  NULL,
			  "Null mempool");
    VerifyOrQuit(err, "ble_npl_mempool_init accepted NULL parameters.");

    err = ble_npl_mempool_init(&s_mempool,
			  TEST_MEMPOOL_BLOCKS,
			  TEST_MEMPOOL_BLOCK_SIZE,
			  s_mempool_mem,
			  "s_mempool");
    return err;
}

/**
 * Test integrity check of a mempool.
 *
 * bool ble_npl_mempool_is_sane(const struct ble_npl_mempool *mp);
 */
int test_is_sane()
{
    return (ble_npl_mempool_is_sane(&s_mempool)) ? PASS : FAIL;
}

/**
 * Test getting a memory block from the pool, putting it back,
 * and checking if it is still valid.
 *
 * void *ble_npl_memblock_get(struct ble_npl_mempool *mp);
 *
 * ble_npl_error_t ble_npl_memblock_put(struct ble_npl_mempool *mp, void *block_addr);
 *
 * int ble_npl_memblock_from(const struct ble_npl_mempool *mp, const void *block_addr);
 */
int test_stress()
{
    int loops = 3;
    while(loops--)
    {
        for (int i = 0; i < 4; i++)
	{
	    s_memblock[i] = ble_npl_memblock_get(&s_mempool);
	    VerifyOrQuit(ble_npl_memblock_from(&s_mempool, s_memblock[i]),
			 "ble_npl_memblock_get return invalid block.");
	}


        for (int i = 0; i < 4; i++)
	{
 	    SuccessOrQuit(ble_npl_memblock_put(&s_mempool, s_memblock[i]),
			"ble_npl_memblock_put refused to take valid block.");
	    //VerifyOrQuit(!ble_npl_memblock_from(&s_mempool, s_memblock[i]),
	    //		 "Block still valid after ble_npl_memblock_put.");
	}

    }
    return PASS;
}

int main(void)
{
    SuccessOrQuit(test_init(),    "Failed: ble_npl_mempool_init");
    SuccessOrQuit(test_is_sane(), "Failed: ble_npl_mempool_is_sane");
    SuccessOrQuit(test_stress(),  "Failed: ble_npl_mempool stree test");
    SuccessOrQuit(test_is_sane(), "Failed: ble_npl_mempool_is_sane");
    printf("All tests passed\n");
    return PASS;
}
