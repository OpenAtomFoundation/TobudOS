#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_char_fifo);

k_chr_fifo_t test_fifo_00;
k_chr_fifo_t test_fifo_01;
k_chr_fifo_t test_fifo_02;

uint8_t fifo_buffer_00[FIFO_BUFFER_SIZE];
uint8_t fifo_buffer_01[FIFO_BUFFER_SIZE];
uint8_t fifo_buffer_02[FIFO_BUFFER_SIZE];

TEST test_tos_fifo_create(void)
{
    k_err_t err;

    err = tos_chr_fifo_create(&test_fifo_00, fifo_buffer_00, sizeof(fifo_buffer_00));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_chr_fifo_create(&test_fifo_01, fifo_buffer_01, sizeof(fifo_buffer_01));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_chr_fifo_create(&test_fifo_02, fifo_buffer_02, sizeof(fifo_buffer_02));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_chr_fifo_destroy(&test_fifo_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_chr_fifo_destroy(&test_fifo_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_chr_fifo_destroy(&test_fifo_02);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_fifo_destory(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_chr_fifo_destroy(&test_fifo_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_chr_fifo_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_fifo_char_push(void)
{
    k_err_t err;
    int i = 0;
    uint8_t data;

    err = tos_chr_fifo_create(&test_fifo_00, fifo_buffer_00, sizeof(fifo_buffer_00));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        err = tos_chr_fifo_push(&test_fifo_00, 'a' + i);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    err = tos_chr_fifo_push(&test_fifo_00, 'z');
    ASSERT_EQ(err, K_ERR_RING_Q_FULL);

    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        err = tos_chr_fifo_pop(&test_fifo_00, &data);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(data, 'a' + i);
    }

    err = tos_chr_fifo_pop(&test_fifo_00, &data);
    ASSERT_EQ(err, K_ERR_RING_Q_EMPTY);

    err = tos_chr_fifo_destroy(&test_fifo_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_fifo_char_push_dyn(void)
{
    k_err_t err;
    int i = 0;
    uint8_t data;

    err = tos_chr_fifo_create_dyn(&test_fifo_00, sizeof(fifo_buffer_00));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        err = tos_chr_fifo_push(&test_fifo_00, 'a' + i);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    err = tos_chr_fifo_push(&test_fifo_00, 'z');
    ASSERT_EQ(err, K_ERR_RING_Q_FULL);

    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        err = tos_chr_fifo_pop(&test_fifo_00, &data);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(data, 'a' + i);
    }

    err = tos_chr_fifo_pop(&test_fifo_00, &data);
    ASSERT_EQ(err, K_ERR_RING_Q_EMPTY);

    err = tos_chr_fifo_destroy_dyn(&test_fifo_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_fifo_stream_push(void)
{
    k_err_t err;
    int count = 0;
    uint8_t stream[FIFO_BUFFER_SIZE] = { 'a', 'b', 'c', 'd', 'e' };
    uint8_t stream_dummy[1] = { 'z' };
    uint8_t stream_pop[FIFO_BUFFER_SIZE];

    err = tos_chr_fifo_create(&test_fifo_00, fifo_buffer_00, sizeof(fifo_buffer_00));
    ASSERT_EQ(err, K_ERR_NONE);

    count = tos_chr_fifo_push_stream(&test_fifo_00, &stream[0], FIFO_BUFFER_SIZE);
    ASSERT_EQ(count, FIFO_BUFFER_SIZE);

    count = tos_chr_fifo_push_stream(&test_fifo_00, &stream_dummy[0], 1);
    ASSERT_EQ(count, 0);
    ASSERT_EQ(tos_chr_fifo_is_full(&test_fifo_00), K_TRUE);

    count = tos_chr_fifo_pop_stream(&test_fifo_00, &stream_pop[0], FIFO_BUFFER_SIZE);
    ASSERT_EQ(count, FIFO_BUFFER_SIZE);

    count = tos_chr_fifo_pop_stream(&test_fifo_00, &stream_pop[0], 1);
    ASSERT_EQ(count, 0);
    ASSERT_EQ(tos_chr_fifo_is_empty(&test_fifo_00), K_TRUE);

    PASS();
}

SUITE(suit_char_fifo)
{
    RUN_TEST(test_tos_fifo_create);
    RUN_TEST(test_tos_fifo_destory);
    RUN_TEST(test_tos_fifo_char_push);
    RUN_TEST(test_tos_fifo_char_push_dyn);
    RUN_TEST(test_tos_fifo_stream_push);
}

