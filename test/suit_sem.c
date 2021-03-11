#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_sem);

k_sem_t test_sem_00;
k_sem_t test_sem_01;
k_sem_t test_sem_02;
k_sem_t *test_sem_dyn_00;
k_sem_t *test_sem_dyn_01;
k_sem_t *test_sem_dyn_02;
static void test_sem_pend_task_entry(void *arg)
{
    k_err_t err;
    k_sem_t *sem;

    sem = arg ? (k_sem_t *)arg : K_NULL;
    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_sem_pend(sem, TOS_TIME_FOREVER);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

static void test_sem_pend_timed_task_entry(void *arg)
{
    k_err_t err;
    k_sem_t *sem;

    sem = arg ? (k_sem_t *)arg : K_NULL;

    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_sem_pend(sem, (k_tick_t)2000u);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

TEST test_tos_sem_create(void)
{
    k_err_t err;

    err = tos_sem_create(&test_sem_00, (k_sem_cnt_t)0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_create(&test_sem_01, (k_sem_cnt_t)0xFF);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_create(&test_sem_02, (k_sem_cnt_t)0xFFFF);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_destroy(&test_sem_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_destroy(&test_sem_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_destroy(&test_sem_02);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_sem_create_dyn(void)
{
    k_err_t err;

    err = tos_sem_create_dyn(&test_sem_dyn_00, (k_sem_cnt_t)0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_create_dyn(&test_sem_dyn_01, (k_sem_cnt_t)0xFF);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_create_dyn(&test_sem_dyn_02, (k_sem_cnt_t)0xFFFF);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_destroy(test_sem_dyn_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_destroy(test_sem_dyn_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_destroy(test_sem_dyn_02);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_sem_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_sem_destroy(&test_sem_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_sem_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_sem_pend(void)
{
    k_err_t err;

    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_sem_create(&test_sem_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_sem_pend_task_entry,
                            (void *)(&test_sem_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_sem_post(&test_sem_00);

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_sem_destroy(&test_sem_00);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_sem_pend_timed(void)
{
    k_err_t err;
    k_tick_t begin;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_sem_create(&test_sem_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task", test_sem_pend_timed_task_entry,
                            (void *)(&test_sem_00), k_curr_task->prio - 1,
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

    err = tos_sem_post(&test_sem_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_count, 2);
    ASSERT_EQ(test_err, K_ERR_NONE);

    err = tos_sem_destroy(&test_sem_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_01);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_sem_post_all(void)
{
    k_err_t err;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_sem_create(&test_sem_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task_00", test_sem_pend_task_entry,
                            (void *)(&test_sem_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_01, "test_task_01", test_sem_pend_task_entry,
                            (void *)(&test_sem_00), k_curr_task->prio - 1,
                            test_task_stack_01, sizeof(test_task_stack_01),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_02, "test_task_02", test_sem_pend_task_entry,
                            (void *)(&test_sem_00), k_curr_task->prio - 1,
                            test_task_stack_02, sizeof(test_task_stack_02),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);

    err = tos_sem_post_all(&test_sem_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 3);

    err = tos_sem_post(&test_sem_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 4);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_02);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_sem_destroy(&test_sem_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

SUITE(suit_sem)
{
    RUN_TEST(test_tos_sem_create);
    RUN_TEST(test_tos_sem_create_dyn);
    RUN_TEST(test_tos_sem_destroy);
    RUN_TEST(test_tos_sem_pend);
    RUN_TEST(test_tos_sem_pend_timed);
    RUN_TEST(test_tos_sem_post_all);
}

