#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_priority_queue);

k_prio_q_t test_prio_q_00;
k_prio_q_t test_prio_q_01;

#define TEST_PRIO_Q_ITEM_CNT  16
uint8_t test_prio_q_pool_00[sizeof(test_prio_q_item_t) * TEST_PRIO_Q_ITEM_CNT];
uint8_t test_prio_q_pool_01[sizeof(test_prio_q_item_t) * TEST_PRIO_Q_ITEM_CNT];
uint8_t test_prio_q_int_pool_00[sizeof(int) * TEST_PRIO_Q_ITEM_CNT];

uint8_t test_prio_q_mgr_array_00[TOS_PRIO_Q_MGR_ARRAY_SIZE(TEST_PRIO_Q_ITEM_CNT)];
uint8_t test_prio_q_mgr_array_01[TOS_PRIO_Q_MGR_ARRAY_SIZE(TEST_PRIO_Q_ITEM_CNT)];

TEST test_tos_priority_queue_create(void)
{
    k_err_t err;

    err = tos_prio_q_create(&test_prio_q_00, K_NULL, test_prio_q_pool_00, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_prio_q_create(K_NULL, test_prio_q_mgr_array_00, test_prio_q_pool_00, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_prio_q_create(&test_prio_q_00, test_prio_q_mgr_array_00, test_prio_q_pool_00, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_q_create(&test_prio_q_01, test_prio_q_mgr_array_01, test_prio_q_pool_01, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_q_destroy(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_q_destroy(&test_prio_q_01);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_priority_queue_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_prio_q_destroy(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_prio_q_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_priority_queue_enqueue(void)
{
    k_err_t err;
    int i = 0;
    k_prio_t prio;
    test_prio_q_item_t items[TEST_PRIO_Q_ITEM_CNT] = {
        { 2, "222", '2' },
        { 4, "444", '4' },
        { 1, "111", '1' },
        { 7, "777", '7' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 2, "222", '2' },
        { 0, "000", '0' },
        { 9, "999", '9' },
        { 10, "aaa", 'a' },
        { 5, "555", '5' },
        { 12, "ccc", 'c' },
        { 11, "bbb", 'b' },
        { 3, "333", '3' },
        { 6, "666", '6' },
        { 8, "888", '8' },
    };
    test_prio_q_item_t items_should[TEST_PRIO_Q_ITEM_CNT] = {
        { 0, "000", '0' },
        { 1, "111", '1' },
        { 2, "222", '2' },
        { 2, "222", '2' },
        { 3, "333", '3' },
        { 4, "444", '4' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 6, "666", '6' },
        { 7, "777", '7' },
        { 8, "888", '8' },
        { 9, "999", '9' },
        { 10, "aaa", 'a' },
        { 11, "bbb", 'b' },
        { 12, "ccc", 'c' },
    };
    test_prio_q_item_t item_dequeued;
    size_t item_size;

    // yes, our priority queue has a min heap inside(priority, numerically bigger, actually smaller)
    err = tos_prio_q_create(&test_prio_q_00, test_prio_q_mgr_array_00, test_prio_q_pool_00, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        // we enqueue the item with the domain "a" as the priority
        err = tos_prio_q_enqueue(&test_prio_q_00, &items[i], sizeof(test_prio_q_item_t), items[i].a);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_prio_q_is_full(&test_prio_q_00));

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        err = tos_prio_q_dequeue(&test_prio_q_00, &item_dequeued, &item_size, &prio);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(test_prio_q_item_t));
        ASSERT_EQ(prio, item_dequeued.a);
        ASSERT_EQ(item_dequeued.a, items_should[i].a);
        ASSERT_EQ(strcmp(item_dequeued.b, items_should[i].b), 0);
        ASSERT_EQ(item_dequeued.c, items_should[i].c);
    }

    ASSERT(tos_prio_q_is_empty(&test_prio_q_00));

    err = tos_prio_q_destroy(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_priority_queue_enqueue_dyn(void)
{
    k_err_t err;
    int i = 0;
    k_prio_t prio;
    test_prio_q_item_t items[TEST_PRIO_Q_ITEM_CNT] = {
        { 2, "222", '2' },
        { 4, "444", '4' },
        { 1, "111", '1' },
        { 7, "777", '7' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 2, "222", '2' },
        { 0, "000", '0' },
        { 9, "999", '9' },
        { 10, "aaa", 'a' },
        { 5, "555", '5' },
        { 12, "ccc", 'c' },
        { 11, "bbb", 'b' },
        { 3, "333", '3' },
        { 6, "666", '6' },
        { 8, "888", '8' },
    };
    test_prio_q_item_t items_should[TEST_PRIO_Q_ITEM_CNT] = {
        { 0, "000", '0' },
        { 1, "111", '1' },
        { 2, "222", '2' },
        { 2, "222", '2' },
        { 3, "333", '3' },
        { 4, "444", '4' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 6, "666", '6' },
        { 7, "777", '7' },
        { 8, "888", '8' },
        { 9, "999", '9' },
        { 10, "aaa", 'a' },
        { 11, "bbb", 'b' },
        { 12, "ccc", 'c' },
    };
    test_prio_q_item_t item_dequeued;
    size_t item_size;

    // yes, our priority queue has a min heap inside(priority, numerically bigger, actually smaller)
    err = tos_prio_q_create_dyn(&test_prio_q_00, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        // we enqueue the item with the domain "a" as the priority
        err = tos_prio_q_enqueue(&test_prio_q_00, &items[i], sizeof(test_prio_q_item_t), items[i].a);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_prio_q_is_full(&test_prio_q_00));

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        err = tos_prio_q_dequeue(&test_prio_q_00, &item_dequeued, &item_size, &prio);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(test_prio_q_item_t));
        ASSERT_EQ(prio, item_dequeued.a);
        ASSERT_EQ(item_dequeued.a, items_should[i].a);
        ASSERT_EQ(strcmp(item_dequeued.b, items_should[i].b), 0);
        ASSERT_EQ(item_dequeued.c, items_should[i].c);
    }

    ASSERT(tos_prio_q_is_empty(&test_prio_q_00));

    err = tos_prio_q_destroy_dyn(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_priority_queue_int_enqueue(void)
{
    k_err_t err;
    int i = 0;
    k_prio_t prio;
    int items[TEST_PRIO_Q_ITEM_CNT] = {
        2,
        4,
        1,
        7,
        5,
        5,
        2,
        0,
        9,
        10,
        5,
        12,
        11,
        3,
        6,
        8,
    };
    int items_should[TEST_PRIO_Q_ITEM_CNT] = {
        0,
        1,
        2,
        2,
        3,
        4,
        5,
        5,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
    };
    int item_dequeued;
    size_t item_size;

    // yes, our priority queue has a min heap inside(priority, numerically bigger, actually smaller)
    err = tos_prio_q_create(&test_prio_q_00, test_prio_q_mgr_array_00, test_prio_q_int_pool_00, TEST_PRIO_Q_ITEM_CNT, sizeof(int));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        // we enqueue the item with the value of item as the priority
        err = tos_prio_q_enqueue(&test_prio_q_00, &items[i], sizeof(int), items[i]);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_prio_q_is_full(&test_prio_q_00));

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        err = tos_prio_q_dequeue(&test_prio_q_00, &item_dequeued, &item_size, &prio);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(int));
        ASSERT_EQ(prio, item_dequeued);
        ASSERT_EQ(item_dequeued, items_should[i]);
    }

    ASSERT(tos_prio_q_is_empty(&test_prio_q_00));

    err = tos_prio_q_destroy(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_priority_queue_int_enqueue_dyn(void)
{
    k_err_t err;
    int i = 0;
    k_prio_t prio;
    int items[TEST_PRIO_Q_ITEM_CNT] = {
        2,
        4,
        1,
        7,
        5,
        5,
        2,
        0,
        9,
        10,
        5,
        12,
        11,
        3,
        6,
        8,
    };
    int items_should[TEST_PRIO_Q_ITEM_CNT] = {
        0,
        1,
        2,
        2,
        3,
        4,
        5,
        5,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
    };
    int item_dequeued;
    size_t item_size;

    // yes, our priority queue has a min heap inside(priority, numerically bigger, actually smaller)
    err = tos_prio_q_create_dyn(&test_prio_q_00, TEST_PRIO_Q_ITEM_CNT, sizeof(int));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        // we enqueue the item with the value of item as the priority
        err = tos_prio_q_enqueue(&test_prio_q_00, &items[i], sizeof(int), items[i]);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_prio_q_is_full(&test_prio_q_00));

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        err = tos_prio_q_dequeue(&test_prio_q_00, &item_dequeued, &item_size, &prio);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(int));
        ASSERT_EQ(prio, item_dequeued);
        ASSERT_EQ(item_dequeued, items_should[i]);
    }

    ASSERT(tos_prio_q_is_empty(&test_prio_q_00));

    err = tos_prio_q_destroy_dyn(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_priority_queue_enqueue_limit(void)
{
    k_err_t err;
    int i = 0;
    k_prio_t prio;
    test_prio_q_item_t items[TEST_PRIO_Q_ITEM_CNT] = {
        { 2, "222", '2' },
        { 4, "444", '4' },
        { 1, "111", '1' },
        { 7, "777", '7' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 2, "222", '2' },
        { 0, "000", '0' },
        { 9, "999", '9' },
        { 10, "aaa", 'a' },
        { 5, "555", '5' },
        { 12, "ccc", 'c' },
        { 11, "bbb", 'b' },
        { 3, "333", '3' },
        { 6, "666", '6' },
        { 8, "888", '8' },
    };
    test_prio_q_item_t items_should[TEST_PRIO_Q_ITEM_CNT] = {
        { 0, "000", '0' },
        { 1, "111", '1' },
        { 2, "222", '2' },
        { 2, "222", '2' },
        { 3, "333", '3' },
        { 4, "444", '4' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 6, "666", '6' },
        { 7, "777", '7' },
        { 8, "888", '8' },
        { 9, "999", '9' },
        { 10, "aaa", 'a' },
        { 11, "bbb", 'b' },
        { 12, "ccc", 'c' },
    };
    test_prio_q_item_t item_dequeued;
    size_t item_size;

    err = tos_prio_q_create(&test_prio_q_00, test_prio_q_mgr_array_00, test_prio_q_pool_00, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        err = tos_prio_q_enqueue(&test_prio_q_00, &items[i], sizeof(test_prio_q_item_t), items[i].a);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_prio_q_is_full(&test_prio_q_00));

    err = tos_prio_q_enqueue(&test_prio_q_00, &items[0], sizeof(test_prio_q_item_t), items[0].a);
    ASSERT_EQ(err, K_ERR_PRIO_Q_FULL);

    ASSERT(tos_prio_q_is_full(&test_prio_q_00));

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        err = tos_prio_q_dequeue(&test_prio_q_00, &item_dequeued, &item_size, &prio);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(test_prio_q_item_t));
        ASSERT_EQ(item_dequeued.a, items_should[i].a);
        ASSERT_EQ(strcmp(item_dequeued.b, items_should[i].b), 0);
        ASSERT_EQ(item_dequeued.c, items_should[i].c);
    }

    ASSERT(tos_prio_q_is_empty(&test_prio_q_00));

    err = tos_prio_q_dequeue(&test_prio_q_00, &item_dequeued, &item_size, &prio);
    ASSERT_EQ(err, K_ERR_PRIO_Q_EMPTY);

    ASSERT(tos_prio_q_is_empty(&test_prio_q_00));

    err = tos_prio_q_destroy(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_priority_queue_flush(void)
{
    k_err_t err;
    int i = 0;
    k_prio_t prio;
    test_prio_q_item_t items[TEST_PRIO_Q_ITEM_CNT] = {
        { 2, "222", '2' },
        { 4, "444", '4' },
        { 1, "111", '1' },
        { 7, "777", '7' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 2, "222", '2' },
        { 0, "000", '0' },
        { 9, "999", '9' },
        { 10, "aaa", 'a' },
        { 5, "555", '5' },
        { 12, "ccc", 'c' },
        { 11, "bbb", 'b' },
        { 3, "333", '3' },
        { 6, "666", '6' },
        { 8, "888", '8' },
    };
    test_prio_q_item_t item_dequeued;
    size_t item_size;

    err = tos_prio_q_create(&test_prio_q_00, test_prio_q_mgr_array_00, test_prio_q_pool_00, TEST_PRIO_Q_ITEM_CNT, sizeof(test_prio_q_item_t));
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_PRIO_Q_ITEM_CNT; ++i) {
        err = tos_prio_q_enqueue(&test_prio_q_00, &items[i], sizeof(test_prio_q_item_t), items[i].a);
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_prio_q_is_full(&test_prio_q_00));

    err = tos_prio_q_flush(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT(tos_prio_q_is_empty(&test_prio_q_00));

    err = tos_prio_q_dequeue(&test_prio_q_00, &item_dequeued, &item_size, &prio);
    ASSERT_EQ(err, K_ERR_PRIO_Q_EMPTY);

    err = tos_prio_q_enqueue(&test_prio_q_00, &items[0], sizeof(test_prio_q_item_t), items[0].a);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_q_destroy(&test_prio_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_priority_queue)
{
    RUN_TEST(test_tos_priority_queue_create);
    RUN_TEST(test_tos_priority_queue_destroy);
    RUN_TEST(test_tos_priority_queue_enqueue);
    RUN_TEST(test_tos_priority_queue_enqueue_dyn);
    RUN_TEST(test_tos_priority_queue_int_enqueue);
    RUN_TEST(test_tos_priority_queue_int_enqueue_dyn);
    RUN_TEST(test_tos_priority_queue_enqueue_limit);
    RUN_TEST(test_tos_priority_queue_flush);
}

