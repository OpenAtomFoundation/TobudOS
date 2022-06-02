#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_mmblk);

k_mmblk_pool_t test_mmblk_pool_00;
k_mmblk_pool_t test_mmblk_pool_01;
k_mmblk_pool_t test_mmblk_pool_02;
k_mmblk_pool_t *test_mmblk_pool_dyn_00;
k_mmblk_pool_t *test_mmblk_pool_dyn_01;
k_mmblk_pool_t *test_mmblk_pool_dyn_02;

uint8_t mmblk_pool_buffer_00[MMBLK_BLK_NUM * MMBLK_BLK_SIZE];
uint8_t mmblk_pool_buffer_01[MMBLK_BLK_NUM * MMBLK_BLK_SIZE];
uint8_t mmblk_pool_buffer_02[MMBLK_BLK_NUM * MMBLK_BLK_SIZE];

TEST test_tos_mmblk_pool_create(void)
{
    k_err_t err;

    err = tos_mmblk_pool_create(&test_mmblk_pool_00, mmblk_pool_buffer_00,
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_create(&test_mmblk_pool_01, mmblk_pool_buffer_01,
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_create(&test_mmblk_pool_02, mmblk_pool_buffer_02,
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_destroy(&test_mmblk_pool_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_destroy(&test_mmblk_pool_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_destroy(&test_mmblk_pool_02);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_create(&test_mmblk_pool_00, K_NULL,
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_mmblk_pool_create(&test_mmblk_pool_00, (void *)((cpu_addr_t)mmblk_pool_buffer_00 + 1),
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_MMBLK_INVALID_POOL_ADDR);

    err = tos_mmblk_pool_create(&test_mmblk_pool_00, mmblk_pool_buffer_00,
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE + 1);
    ASSERT_EQ(err, K_ERR_MMBLK_INVALID_BLK_SIZE);

    PASS();
}

TEST test_tos_mmblk_pool_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_mmblk_pool_destroy(&test_mmblk_pool_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_mmblk_pool_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

#if TOS_CFG_OBJ_DYNAMIC_CREATE_EN > 0u

TEST test_tos_mmblk_pool_create_dyn(void)
{
    k_err_t err;

    err = tos_mmblk_pool_create_dyn(&test_mmblk_pool_dyn_00, MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_create_dyn(&test_mmblk_pool_dyn_01, MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_create_dyn(&test_mmblk_pool_dyn_02, MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_destroy_dyn(test_mmblk_pool_dyn_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_destroy_dyn(test_mmblk_pool_dyn_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_destroy_dyn(test_mmblk_pool_dyn_02);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_pool_create_dyn(&test_mmblk_pool_dyn_00, MMBLK_BLK_NUM, MMBLK_BLK_SIZE + 1);
    ASSERT_EQ(err, K_ERR_MMBLK_INVALID_BLK_SIZE);

    PASS();
}

#endif

TEST test_tos_mmblk_alloc(void)
{
    int i = 0;
    k_err_t err;
    void *blk = K_NULL;

    err = tos_mmblk_pool_create(&test_mmblk_pool_00, mmblk_pool_buffer_00,
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < MMBLK_BLK_NUM; ++i) {
        err = tos_mmblk_alloc(&test_mmblk_pool_00, (void **)(&blk));
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT(blk != K_NULL);
    }

    err = tos_mmblk_alloc(&test_mmblk_pool_00, (void **)(&blk));
    ASSERT_EQ(err, K_ERR_MMBLK_POOL_EMPTY);

    err = tos_mmblk_pool_destroy(&test_mmblk_pool_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_mmblk_free(void)
{
    k_err_t err;
    void *blk;

    err = tos_mmblk_pool_create(&test_mmblk_pool_00, mmblk_pool_buffer_00,
                                    MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_alloc(&test_mmblk_pool_00, (void **)(&blk));
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT(blk != K_NULL);

    err = tos_mmblk_free(&test_mmblk_pool_00, blk);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmblk_free(&test_mmblk_pool_00, blk);
    ASSERT_EQ(err, K_ERR_MMBLK_POOL_FULL);

    err = tos_mmblk_pool_destroy(&test_mmblk_pool_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_mmblk)
{
    RUN_TEST(test_tos_mmblk_pool_create);
    RUN_TEST(test_tos_mmblk_pool_destroy);
#if TOS_CFG_OBJ_DYNAMIC_CREATE_EN > 0u
    RUN_TEST(test_tos_mmblk_pool_create_dyn);
#endif
    RUN_TEST(test_tos_mmblk_alloc);
    RUN_TEST(test_tos_mmblk_free);
}

