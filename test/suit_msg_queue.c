#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_msg_queue);

k_msg_queue_t test_msg_queue_00;
k_msg_queue_t test_msg_queue_01;
k_msg_queue_t test_msg_queue_02;

TEST test_tos_msg_queue_create(void)
{
    k_err_t err;

    err = tos_msg_queue_create(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_queue_create(&test_msg_queue_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_queue_create(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_msg_queue_destroy(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_queue_destroy(&test_msg_queue_01);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_msg_queue_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_msg_queue_destroy(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_msg_queue_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_msg_queue_fifo(void)
{
    k_err_t err;
    int i = 0;
    uint32_t msgs[3] = { 1, 2, 3 };
    void *msg_received = K_NULL;
    size_t msg_size = 0;

    err = tos_msg_queue_create(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT(TOS_CFG_MSG_POOL_SIZE >= 3);

    for (i = 0; i < 3; ++i) {
        err = tos_msg_queue_put(&test_msg_queue_00, (void *)(&msgs[i]), sizeof(uint32_t), TOS_OPT_MSG_PUT_FIFO);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    for (i = 0; i < 3; ++i) {
        err = tos_msg_queue_get(&test_msg_queue_00, (void **)&msg_received, &msg_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(msg_size, sizeof(uint32_t));
        ASSERT_EQ(msg_received, &msgs[i]);
    }

    PASS();
}

TEST test_tos_msg_queue_lifo(void)
{
    k_err_t err;
    int i = 0;
    uint32_t msgs[3] = { 1, 2, 3 };
    void *msg_received = K_NULL;
    size_t msg_size = 0;

    err = tos_msg_queue_create(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT(TOS_CFG_MSG_POOL_SIZE >= 3);

    for (i = 0; i < 3; ++i) {
        err = tos_msg_queue_put(&test_msg_queue_00, (void *)(&msgs[i]), sizeof(uint32_t), TOS_OPT_MSG_PUT_LIFO);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    for (i = 0; i < 3; ++i) {
        err = tos_msg_queue_get(&test_msg_queue_00, (void **)&msg_received, &msg_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(msg_size, sizeof(uint32_t));
        ASSERT_EQ(msg_received, &msgs[2 - i]);
    }

    PASS();
}

TEST test_tos_msg_queue_fifo_limit(void)
{
    k_err_t err;
    int i = 0;
    uint32_t msg_send;
    void *msg_received = K_NULL;
    size_t msg_size = 0;

    err = tos_msg_queue_create(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    for (; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        err = tos_msg_queue_put(&test_msg_queue_00, (void *)(&msg_send), sizeof(uint32_t), TOS_OPT_MSG_PUT_FIFO);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    err = tos_msg_queue_put(&test_msg_queue_00, (void *)(&msg_send), sizeof(uint32_t), TOS_OPT_MSG_PUT_FIFO);
    ASSERT_EQ(err, K_ERR_MSG_QUEUE_FULL);

    for (i = 0; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        err = tos_msg_queue_get(&test_msg_queue_00, (void **)&msg_received, &msg_size);
        ASSERT_EQ(err, K_ERR_NONE);
    }
    err = tos_msg_queue_get(&test_msg_queue_00, (void **)&msg_received, &msg_size);
    ASSERT_EQ(err, K_ERR_MSG_QUEUE_EMPTY);

    err = tos_msg_queue_destroy(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_msg_queue_lifo_limit(void)
{
    k_err_t err;
    int i = 0;
    uint32_t msg_send;
    void *msg_received = K_NULL;
    size_t msg_size = 0;

    err = tos_msg_queue_create(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    for (; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        err = tos_msg_queue_put(&test_msg_queue_00, (void *)(&msg_send), sizeof(uint32_t), TOS_OPT_MSG_PUT_LIFO);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    err = tos_msg_queue_put(&test_msg_queue_00, (void *)(&msg_send), sizeof(uint32_t), TOS_OPT_MSG_PUT_LIFO);
    ASSERT_EQ(err, K_ERR_MSG_QUEUE_FULL);

    for (i = 0; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        err = tos_msg_queue_get(&test_msg_queue_00, (void **)&msg_received, &msg_size);
        ASSERT_EQ(err, K_ERR_NONE);
    }
    err = tos_msg_queue_get(&test_msg_queue_00, (void **)&msg_received, &msg_size);
    ASSERT_EQ(err, K_ERR_MSG_QUEUE_EMPTY);

    err = tos_msg_queue_destroy(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_msg_queue_flush(void)
{
    k_err_t err;
    int i = 0;
    uint32_t msgs[3] = { 1, 2, 3 };
    void *msg_received = K_NULL;
    size_t msg_size = 0;

    err = tos_msg_queue_create(&test_msg_queue_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT(TOS_CFG_MSG_POOL_SIZE >= 3);

    for (i = 0; i < 3; ++i) {
        err = tos_msg_queue_put(&test_msg_queue_00, (void *)(&msgs[i]), sizeof(uint32_t), TOS_OPT_MSG_PUT_FIFO);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    tos_msg_queue_flush(&test_msg_queue_00);

    err = tos_msg_queue_get(&test_msg_queue_00, (void **)&msg_received, &msg_size);
    ASSERT_EQ(err, K_ERR_MSG_QUEUE_EMPTY);

    PASS();
}

SUITE(suit_msg_queue)
{
    RUN_TEST(test_tos_msg_queue_create);
    RUN_TEST(test_tos_msg_queue_destroy);
    RUN_TEST(test_tos_msg_queue_fifo);
    RUN_TEST(test_tos_msg_queue_lifo);
    RUN_TEST(test_tos_msg_queue_fifo_limit);
    RUN_TEST(test_tos_msg_queue_lifo_limit);
    RUN_TEST(test_tos_msg_queue_flush);
}

