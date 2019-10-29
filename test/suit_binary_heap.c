#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_binary_heap);

k_bin_heap_t test_bin_heap_00;
k_bin_heap_t test_bin_heap_01;

#define TEST_BIN_HEAP_ITEM_CNT  16
uint8_t test_bin_heap_pool_00[sizeof(test_bin_heap_item_t) * TEST_BIN_HEAP_ITEM_CNT];
uint8_t test_bin_heap_pool_01[sizeof(test_bin_heap_item_t) * TEST_BIN_HEAP_ITEM_CNT];

uint8_t test_bin_heap_int_pool_00[sizeof(int) * TEST_BIN_HEAP_ITEM_CNT];
uint8_t test_bin_heap_int_pool_01[sizeof(int) * TEST_BIN_HEAP_ITEM_CNT];

int test_bin_heap_cmp_dummy(void *first, void *second)
{
    return 0;
}

int test_bin_heap_min_cmp_dummy(void *first, void *second)
{
    test_bin_heap_item_t *item_first, *item_second;

    item_first     = (test_bin_heap_item_t *)first;
    item_second    = (test_bin_heap_item_t *)second;

    if (item_first->a < item_second->a) {
        return K_TRUE;
    }
    return K_FALSE;
}

int test_bin_heap_max_cmp_dummy(void *first, void *second)
{
    test_bin_heap_item_t *item_first, *item_second;

    item_first     = (test_bin_heap_item_t *)first;
    item_second    = (test_bin_heap_item_t *)second;

    if (item_first->a > item_second->a) {
        return K_TRUE;
    }
    return K_FALSE;
}


int test_bin_heap_min_int_cmp_dummy(void *first, void *second)
{
    int *item_first, *item_second;

    item_first     = (int *)first;
    item_second    = (int *)second;

    if (*item_first < *item_second) {
        return K_TRUE;
    }
    return K_FALSE;
}

int test_bin_heap_max_int_cmp_dummy(void *first, void *second)
{
    int *item_first, *item_second;

    item_first     = (int *)first;
    item_second    = (int *)second;

    if (*item_first > *item_second) {
        return K_TRUE;
    }
    return K_FALSE;
}

TEST test_tos_binary_heap_create(void)
{
    k_err_t err;

    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_bin_heap_create(K_NULL, test_bin_heap_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_cmp_dummy);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_bin_heap_create(&test_bin_heap_01, test_bin_heap_pool_01, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_bin_heap_destroy(&test_bin_heap_01);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_heap_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_bin_heap_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_binary_heap_min_push(void)
{
    k_err_t err;
    int i = 0;
    test_bin_heap_item_t items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    test_bin_heap_item_t items_should[TEST_BIN_HEAP_ITEM_CNT] = {
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
    test_bin_heap_item_t item_popped;
    size_t item_size;

    // we gonna build a min heap
    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_min_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(test_bin_heap_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(test_bin_heap_item_t));
        ASSERT_EQ(item_popped.a, items_should[i].a);
        ASSERT_EQ(strcmp(item_popped.b, items_should[i].b), 0);
        ASSERT_EQ(item_popped.c, items_should[i].c);
    }

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_heap_max_push(void)
{
    k_err_t err;
    int i = 0;
    test_bin_heap_item_t items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    test_bin_heap_item_t items_should[TEST_BIN_HEAP_ITEM_CNT] = {
        { 12, "ccc", 'c' },
        { 11, "bbb", 'b' },
        { 10, "aaa", 'a' },
        { 9, "999", '9' },
        { 8, "888", '8' },
        { 7, "777", '7' },
        { 6, "666", '6' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 4, "444", '4' },
        { 3, "333", '3' },
        { 2, "222", '2' },
        { 2, "222", '2' },
        { 1, "111", '1' },
        { 0, "000", '0' },
    };
    test_bin_heap_item_t item_popped;
    size_t item_size;

    // we gonna build a max heap
    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_max_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(test_bin_heap_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(test_bin_heap_item_t));
        ASSERT_EQ(item_popped.a, items_should[i].a);
        ASSERT_EQ(strcmp(item_popped.b, items_should[i].b), 0);
        ASSERT_EQ(item_popped.c, items_should[i].c);
    }

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_heap_max_push_dyn(void)
{
    k_err_t err;
    int i = 0;
    test_bin_heap_item_t items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    test_bin_heap_item_t items_should[TEST_BIN_HEAP_ITEM_CNT] = {
        { 12, "ccc", 'c' },
        { 11, "bbb", 'b' },
        { 10, "aaa", 'a' },
        { 9, "999", '9' },
        { 8, "888", '8' },
        { 7, "777", '7' },
        { 6, "666", '6' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 4, "444", '4' },
        { 3, "333", '3' },
        { 2, "222", '2' },
        { 2, "222", '2' },
        { 1, "111", '1' },
        { 0, "000", '0' },
    };
    test_bin_heap_item_t item_popped;
    size_t item_size;

    // we gonna build a max heap
    err = tos_bin_heap_create_dyn(&test_bin_heap_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_max_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(test_bin_heap_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(test_bin_heap_item_t));
        ASSERT_EQ(item_popped.a, items_should[i].a);
        ASSERT_EQ(strcmp(item_popped.b, items_should[i].b), 0);
        ASSERT_EQ(item_popped.c, items_should[i].c);
    }

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_destroy_dyn(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_heap_min_int_push(void)
{
    k_err_t err;
    int i = 0;
    int items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    int items_should[TEST_BIN_HEAP_ITEM_CNT] = {
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
    int item_popped;
    size_t item_size;

    // we gonna build a min heap
    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_int_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(int), test_bin_heap_min_int_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(int));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(int));
        ASSERT_EQ(item_popped, items_should[i]);
    }

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_heap_max_int_push(void)
{
    k_err_t err;
    int i = 0;
    int items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    int items_should[TEST_BIN_HEAP_ITEM_CNT] = {
        12,
        11,
        10,
        9,
        8,
        7,
        6,
        5,
        5,
        5,
        4,
        3,
        2,
        2,
        1,
        0,
    };
    int item_popped;
    size_t item_size;

    // we gonna build a max heap
    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_int_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(int), test_bin_heap_max_int_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(int));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(int));
        ASSERT_EQ(item_popped, items_should[i]);
    }

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_heap_max_int_push_dyn(void)
{
    k_err_t err;
    int i = 0;
    int items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    int items_should[TEST_BIN_HEAP_ITEM_CNT] = {
        12,
        11,
        10,
        9,
        8,
        7,
        6,
        5,
        5,
        5,
        4,
        3,
        2,
        2,
        1,
        0,
    };
    int item_popped;
    size_t item_size;

    // we gonna build a max heap
    err = tos_bin_heap_create_dyn(&test_bin_heap_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(int), test_bin_heap_max_int_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(int));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(int));
        ASSERT_EQ(item_popped, items_should[i]);
    }

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_destroy_dyn(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_heap_push_limit(void)
{
    k_err_t err;
    int i = 0;
    test_bin_heap_item_t items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    test_bin_heap_item_t items_should[TEST_BIN_HEAP_ITEM_CNT] = {
        { 12, "ccc", 'c' },
        { 11, "bbb", 'b' },
        { 10, "aaa", 'a' },
        { 9, "999", '9' },
        { 8, "888", '8' },
        { 7, "777", '7' },
        { 6, "666", '6' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 5, "555", '5' },
        { 4, "444", '4' },
        { 3, "333", '3' },
        { 2, "222", '2' },
        { 2, "222", '2' },
        { 1, "111", '1' },
        { 0, "000", '0' },
    };
    test_bin_heap_item_t item_popped;
    size_t item_size;

    // we gonna build a max heap
    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_max_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(test_bin_heap_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    err = tos_bin_heap_push(&test_bin_heap_00, &items[0], sizeof(test_bin_heap_item_t));
    ASSERT_EQ(err, K_ERR_BIN_HEAP_FULL);

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
        ASSERT_EQ(err, K_ERR_NONE);
        ASSERT_EQ(item_size, sizeof(test_bin_heap_item_t));
        ASSERT_EQ(item_popped.a, items_should[i].a);
        ASSERT_EQ(strcmp(item_popped.b, items_should[i].b), 0);
        ASSERT_EQ(item_popped.c, items_should[i].c);
    }

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
    ASSERT_EQ(err, K_ERR_BIN_HEAP_EMPTY);

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_binary_flush(void)
{
    k_err_t err;
    int i = 0;
    test_bin_heap_item_t items[TEST_BIN_HEAP_ITEM_CNT] = {
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
    test_bin_heap_item_t item_popped;
    size_t item_size;

    // we gonna build a max heap
    err = tos_bin_heap_create(&test_bin_heap_00, test_bin_heap_pool_00, TEST_BIN_HEAP_ITEM_CNT, sizeof(test_bin_heap_item_t), test_bin_heap_max_cmp_dummy);
    ASSERT_EQ(err, K_ERR_NONE);

    for (i = 0; i < TEST_BIN_HEAP_ITEM_CNT; ++i) {
        err = tos_bin_heap_push(&test_bin_heap_00, &items[i], sizeof(test_bin_heap_item_t));
        ASSERT_EQ(err, K_ERR_NONE);
    }

    ASSERT(tos_bin_heap_is_full(&test_bin_heap_00));

    err = tos_bin_heap_flush(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT(tos_bin_heap_is_empty(&test_bin_heap_00));

    err = tos_bin_heap_pop(&test_bin_heap_00, &item_popped, &item_size);
    ASSERT_EQ(err, K_ERR_BIN_HEAP_EMPTY);

    err = tos_bin_heap_push(&test_bin_heap_00, &item_popped, sizeof(test_bin_heap_item_t));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_bin_heap_destroy(&test_bin_heap_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_binary_heap)
{
    RUN_TEST(test_tos_binary_heap_create);
    RUN_TEST(test_tos_binary_heap_destroy);
    RUN_TEST(test_tos_binary_heap_min_push);
    RUN_TEST(test_tos_binary_heap_max_push);
    RUN_TEST(test_tos_binary_heap_max_push_dyn);
    RUN_TEST(test_tos_binary_heap_min_int_push);
    RUN_TEST(test_tos_binary_heap_max_int_push);
    RUN_TEST(test_tos_binary_heap_max_int_push_dyn);
    RUN_TEST(test_tos_binary_heap_push_limit);
    RUN_TEST(test_tos_binary_flush);
}

