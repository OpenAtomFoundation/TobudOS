#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_ring_queue);

k_ring_q_t test_ring_q_00;

#define TEST_RING_Q_U8_ITEM_CNT     5
#define TEST_RING_Q_U8_ITEM_SIZE    sizeof(uint8_t)
uint8_t test_ring_q_u8_pool_00[TEST_RING_Q_U8_ITEM_CNT * TEST_RING_Q_U8_ITEM_SIZE];

typedef struct test_ring_q_item_st {
    int     a;
    char   *b;
    char    c;
} ring_q_item_t;

#define TEST_RING_Q_STRUCT_ITEM_CNT         5
#define TEST_RING_Q_STRUCT_ITEM_SIZE      sizeof(ring_q_item_t)
uint8_t test_ring_q_struct_pool_00[TEST_RING_Q_STRUCT_ITEM_CNT * TEST_RING_Q_STRUCT_ITEM_SIZE];

TEST test_tos_ring_queue_create(void)
{
    k_err_t err;

    err = tos_ring_q_create(&test_ring_q_00, test_ring_q_u8_pool_00, TEST_RING_Q_U8_ITEM_CNT, TEST_RING_Q_U8_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_ring_q_create(K_NULL, test_ring_q_u8_pool_00, TEST_RING_Q_U8_ITEM_CNT, TEST_RING_Q_U8_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_ring_q_create(&test_ring_q_00, test_ring_q_u8_pool_00, TEST_RING_Q_U8_ITEM_CNT, TEST_RING_Q_U8_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_ring_q_destroy(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_ring_queue_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_ring_q_destroy(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_ring_q_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_ring_queue_u8_enqueue(void)
{
    k_err_t err;
    int i = 0;
    uint8_t items[TEST_RING_Q_U8_ITEM_CNT] = { 1, 2, 3, 4, 5 };
    uint8_t item_dequeued;
    size_t item_size;

    err = tos_ring_q_create(&test_ring_q_00, test_ring_q_u8_pool_00, TEST_RING_Q_U8_ITEM_CNT, TEST_RING_Q_U8_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_RING_Q_U8_ITEM_CNT; ++i) {
        err = tos_ring_q_enqueue(&test_ring_q_00, &items[i], sizeof(uint8_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    for (i = 0; i < TEST_RING_Q_U8_ITEM_CNT; ++i) {
        err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(uint8_t));
        ASSERT_EQ(item_dequeued, items[i]);
    }

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_destroy(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_ring_queue_u8_enqueue_dyn(void)
{
    k_err_t err;
    int i = 0;
    uint8_t items[TEST_RING_Q_U8_ITEM_CNT] = { 1, 2, 3, 4, 5 };
    uint8_t item_dequeued;
    size_t item_size;

    err = tos_ring_q_create_dyn(&test_ring_q_00, TEST_RING_Q_U8_ITEM_CNT, TEST_RING_Q_U8_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_RING_Q_U8_ITEM_CNT; ++i) {
        err = tos_ring_q_enqueue(&test_ring_q_00, &items[i], sizeof(uint8_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    for (i = 0; i < TEST_RING_Q_U8_ITEM_CNT; ++i) {
        err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(uint8_t));
        ASSERT_EQ(item_dequeued, items[i]);
    }

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_destroy_dyn(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_ring_queue_u8_enqueue_limit(void)
{
    k_err_t err;
    int i = 0;
    uint8_t items[TEST_RING_Q_U8_ITEM_CNT] = { 1, 2, 3, 4, 5 };
    uint8_t item_dequeued;
    size_t item_size;

    err = tos_ring_q_create(&test_ring_q_00, test_ring_q_u8_pool_00, TEST_RING_Q_U8_ITEM_CNT, TEST_RING_Q_U8_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_RING_Q_U8_ITEM_CNT; ++i) {
        err = tos_ring_q_enqueue(&test_ring_q_00, &items[i], sizeof(uint8_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    err = tos_ring_q_enqueue(&test_ring_q_00, &items[0], sizeof(uint8_t));
    ASSERT_EQ(err, K_ERR_RING_Q_FULL);

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    for (i = 0; i < TEST_RING_Q_U8_ITEM_CNT; ++i) {
        err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(uint8_t));
        ASSERT_EQ(item_dequeued, items[i]);
    }

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
    ASSERT_EQ(err, K_ERR_RING_Q_EMPTY);

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_destroy(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_ring_queue_struct_enqueue(void)
{
    k_err_t err;
    int i = 0;
    ring_q_item_t items[TEST_RING_Q_STRUCT_ITEM_CNT] = {
            { 1, "111", '1' },
            { 2, "222", '2' },
            { 3, "333", '3' },
            { 4, "444", '4' },
            { 5, "555", '5' },
    };
    ring_q_item_t item_dequeued;
    size_t item_size;

    err = tos_ring_q_create(&test_ring_q_00, test_ring_q_struct_pool_00, TEST_RING_Q_STRUCT_ITEM_CNT, TEST_RING_Q_STRUCT_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_RING_Q_STRUCT_ITEM_CNT; ++i) {
        err = tos_ring_q_enqueue(&test_ring_q_00, &items[i], sizeof(ring_q_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    for (i = 0; i < TEST_RING_Q_STRUCT_ITEM_CNT; ++i) {
        err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(ring_q_item_t));
        ASSERT_EQ(item_dequeued.a, items[i].a);
        ASSERT_EQ(strcmp(item_dequeued.b, items[i].b), 0);
        ASSERT_EQ(item_dequeued.c, items[i].c);
    }

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_destroy(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_ring_queue_struct_enqueue_dyn(void)
{
    k_err_t err;
    int i = 0;
    ring_q_item_t items[TEST_RING_Q_STRUCT_ITEM_CNT] = {
            { 1, "111", '1' },
            { 2, "222", '2' },
            { 3, "333", '3' },
            { 4, "444", '4' },
            { 5, "555", '5' },
    };
    ring_q_item_t item_dequeued;
    size_t item_size;

    err = tos_ring_q_create_dyn(&test_ring_q_00, TEST_RING_Q_STRUCT_ITEM_CNT, TEST_RING_Q_STRUCT_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_RING_Q_STRUCT_ITEM_CNT; ++i) {
        err = tos_ring_q_enqueue(&test_ring_q_00, &items[i], sizeof(ring_q_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    for (i = 0; i < TEST_RING_Q_STRUCT_ITEM_CNT; ++i) {
        err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(ring_q_item_t));
        ASSERT_EQ(item_dequeued.a, items[i].a);
        ASSERT_EQ(strcmp(item_dequeued.b, items[i].b), 0);
        ASSERT_EQ(item_dequeued.c, items[i].c);
    }

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_destroy_dyn(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}


TEST test_tos_ring_queue_struct_enqueue_limit(void)
{
    k_err_t err;
    int i = 0;
    ring_q_item_t items[TEST_RING_Q_STRUCT_ITEM_CNT] = {
            { 1, "111", '1' },
            { 2, "222", '2' },
            { 3, "333", '3' },
            { 4, "444", '4' },
            { 5, "555", '5' },
    };
    ring_q_item_t item_dequeued;
    size_t item_size;

    err = tos_ring_q_create(&test_ring_q_00, test_ring_q_struct_pool_00, TEST_RING_Q_STRUCT_ITEM_CNT, TEST_RING_Q_STRUCT_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_RING_Q_STRUCT_ITEM_CNT; ++i) {
        err = tos_ring_q_enqueue(&test_ring_q_00, &items[i], sizeof(ring_q_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    err = tos_ring_q_enqueue(&test_ring_q_00, &items[0], sizeof(ring_q_item_t));
    ASSERT_EQ(err, K_ERR_RING_Q_FULL);

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    for (i = 0; i < TEST_RING_Q_STRUCT_ITEM_CNT; ++i) {
        err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(ring_q_item_t));
        ASSERT_EQ(item_dequeued.a, items[i].a);
        ASSERT_EQ(strcmp(item_dequeued.b, items[i].b), 0);
        ASSERT_EQ(item_dequeued.c, items[i].c);
    }

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
    ASSERT_EQ(err, K_ERR_RING_Q_EMPTY);

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_destroy(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_ring_queue_flush(void)
{
    k_err_t err;
    int i = 0;
    uint8_t items[TEST_RING_Q_U8_ITEM_CNT] = { 1, 2, 3, 4, 5 };
    uint8_t item_dequeued;
    size_t item_size;

    err = tos_ring_q_create(&test_ring_q_00, test_ring_q_u8_pool_00, TEST_RING_Q_U8_ITEM_CNT, TEST_RING_Q_U8_ITEM_SIZE);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_RING_Q_U8_ITEM_CNT; ++i) {
        err = tos_ring_q_enqueue(&test_ring_q_00, &items[0], sizeof(uint8_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_ring_q_is_full(&test_ring_q_00));

    err = tos_ring_q_flush(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT(tos_ring_q_is_empty(&test_ring_q_00));

    err = tos_ring_q_dequeue(&test_ring_q_00, &item_dequeued, &item_size);
    ASSERT_EQ(err, K_ERR_RING_Q_EMPTY);

    err = tos_ring_q_enqueue(&test_ring_q_00, &items[0], sizeof(uint8_t));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_ring_q_destroy(&test_ring_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_ring_queue)
{
    RUN_TEST(test_tos_ring_queue_create);
    RUN_TEST(test_tos_ring_queue_destroy);
    RUN_TEST(test_tos_ring_queue_u8_enqueue);
    RUN_TEST(test_tos_ring_queue_u8_enqueue_dyn);
    RUN_TEST(test_tos_ring_queue_u8_enqueue_limit);
    RUN_TEST(test_tos_ring_queue_struct_enqueue);
    RUN_TEST(test_tos_ring_queue_struct_enqueue_dyn);
    RUN_TEST(test_tos_ring_queue_struct_enqueue_limit);
    RUN_TEST(test_tos_ring_queue_flush);
}

