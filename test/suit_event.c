#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_event);

k_event_t test_event_00;
k_event_t test_event_01;
k_event_t test_event_02;

static const k_event_flag_t event_expect_00 = (k_event_flag_t)(1 << 0);
static const k_event_flag_t event_expect_01 = (k_event_flag_t)(1 << 1);
static const k_event_flag_t event_expect_02 = (k_event_flag_t)(1 << 2);
static const k_event_flag_t event_expect_dummy = (k_event_flag_t)(1 << 3);

static void test_event_pend_all_task_entry(void *arg)
{
    k_err_t err;
    k_event_t *event;
    k_event_flag_t flag_match;

    event = arg ? (k_event_t *)arg : K_NULL;
    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_event_pend(event, (event_expect_00 | event_expect_01 | event_expect_02),
                                &flag_match, TOS_TIME_FOREVER,
                                TOS_OPT_EVENT_PEND_ALL | TOS_OPT_EVENT_PEND_CLR);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (err == K_ERR_NONE) {
            test_event_set(flag_match);
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

static void test_event_pend_any_task_entry(void *arg)
{
    k_err_t err;
    k_event_t *event;
    k_event_flag_t flag_match;

    event = arg ? (k_event_t *)arg : K_NULL;
    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_event_pend(event, (event_expect_00 | event_expect_01 | event_expect_02),
                                &flag_match, TOS_TIME_FOREVER,
                                TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (err == K_ERR_NONE) {
            test_event_set(flag_match);
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

static void test_event_pend_timed_task_entry(void *arg)
{
    k_err_t err;
    k_event_t *event;
    k_event_flag_t flag_match;

    event = arg ? (k_event_t *)arg : K_NULL;

    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_event_pend(event, (event_expect_00 | event_expect_01 | event_expect_02),
                                &flag_match, (k_tick_t)2000u,
                                TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (err == K_ERR_NONE) {
            test_event_set(flag_match);
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

static void test_event_post_keep_task_entry(void *arg)
{
    k_err_t err;
    k_event_t *event;
    k_event_flag_t flag_match;

    event = arg ? (k_event_t *)arg : K_NULL;
    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_event_pend(event, (event_expect_00 | event_expect_01 | event_expect_02),
                                &flag_match, TOS_TIME_FOREVER,
                                TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (err == K_ERR_NONE) {
            test_event_set(flag_match);
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }

        test_context_set(TEST_CONTEXT_02);
        tos_task_delay(100);
        test_context_set(TEST_CONTEXT_03);
    }
}

TEST test_tos_event_create(void)
{
    k_err_t err;

    err = tos_event_create(&test_event_00, (k_event_flag_t)0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_event_create(&test_event_01, (k_event_flag_t)0xFF);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_event_create(&test_event_02, (k_event_flag_t)0xFFFF);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_event_destroy(&test_event_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_event_destroy(&test_event_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_event_destroy(&test_event_02);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_event_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_event_destroy(&test_event_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_event_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

#if TOS_CFG_OBJ_DYNAMIC_CREATE_EN > 0u

TEST test_tos_event_create_dyn(void)
{
    k_err_t err;
    k_event_t *event;

    err = tos_event_create_dyn(&event, (k_event_flag_t)0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_event_destroy(event);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID_ALLOC_TYPE);

    err = tos_event_destroy_dyn(event);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

#endif

TEST test_tos_event_pend_all(void)
{
    k_err_t err;

    test_event_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_event_create(&test_event_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_event_pend_all_task_entry,
                            (void *)(&test_event_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_dummy);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_event_match, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_00);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_event_match, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_00 | event_expect_01);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_event_match, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_00 | event_expect_01 | event_expect_02);

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_event_match, event_expect_00 | event_expect_01 | event_expect_02);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_destroy(&test_event_00);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_event_pend_any(void)
{
    k_err_t err;

    test_event_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_event_create(&test_event_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_event_pend_any_task_entry,
                            (void *)(&test_event_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_dummy);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_event_match, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_00);

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_event_match, event_expect_00);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_00 | event_expect_01);

    ASSERT_EQ(test_count, 2);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_event_match, event_expect_00 | event_expect_01);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_00 | event_expect_01 | event_expect_02);

    ASSERT_EQ(test_count, 3);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_event_match, event_expect_00 | event_expect_01 | event_expect_02);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_destroy(&test_event_00);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_event_pend_timed(void)
{
    k_err_t err;
    k_tick_t begin;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_event_create(&test_event_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task", test_event_pend_timed_task_entry,
                            (void *)(&test_event_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // test_task_00 has a higher priority, so it will get running after the task create immediately
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    begin = tos_systick_get();
    while (tos_systick_get() - begin < (k_tick_t)(2000u + 20u)) {
        tos_task_yield();
    }

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_PEND_TIMEOUT);

    err = tos_event_post(&test_event_00, event_expect_00 | event_expect_01);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_count, 2);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_event_match, event_expect_00 | event_expect_01);

    err = tos_event_destroy(&test_event_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_01);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_event_post_keep(void)
{
    k_err_t err;

    test_event_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_event_create(&test_event_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a lower priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_event_post_keep_task_entry,
                            (void *)(&test_event_00), k_curr_task->prio + 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // yeild cpu to test_task_00
    err = tos_task_delay(100);
    ASSERT_EQ(err, K_ERR_NONE);    

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post(&test_event_00, event_expect_dummy);

    // yeild cpu to test_task_00
    err = tos_task_delay(100);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_event_match, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_event_post_keep(&test_event_00, event_expect_00);

    // yeild cpu to test_task_00
    err = tos_task_delay(100);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_event_match, event_expect_00);
    ASSERT_EQ(test_context, TEST_CONTEXT_02);

    tos_event_post_keep(&test_event_00, event_expect_01);
    tos_event_post_keep(&test_event_00, event_expect_02);

    // yeild cpu to test_task_00
    err = tos_task_delay(100);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 2);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_event_match, event_expect_01 | event_expect_02);
    ASSERT_EQ(test_context, TEST_CONTEXT_02);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_event_destroy(&test_event_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

SUITE(suit_event)
{
    RUN_TEST(test_tos_event_create);
    RUN_TEST(test_tos_event_destroy);
#if TOS_CFG_OBJ_DYNAMIC_CREATE_EN > 0u
    RUN_TEST(test_tos_event_create_dyn);
#endif
    RUN_TEST(test_tos_event_pend_all);
    RUN_TEST(test_tos_event_pend_any);
    RUN_TEST(test_tos_event_pend_timed);
    RUN_TEST(test_tos_event_post_keep);
}

