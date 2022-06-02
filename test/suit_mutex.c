#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_mutex);

k_mutex_t test_mutex_00;
k_mutex_t test_mutex_01;
k_mutex_t test_mutex_02;
k_mutex_t *test_mutex_dyn_00;

static void test_mutex_pend_task_entry(void *arg)
{
    k_err_t err;
    k_mutex_t *mutex;

    mutex = arg ? (k_mutex_t *)arg : K_NULL;

    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_mutex_pend(mutex);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (test_task_hook) {
            test_task_hook();
        }

        test_context_set(TEST_CONTEXT_02);
        tos_task_yield();
        test_context_set(TEST_CONTEXT_03);

        err = tos_mutex_post(mutex);
        test_err_set(err);

        test_context_set(TEST_CONTEXT_04);
        tos_task_yield();
        test_context_set(TEST_CONTEXT_05);
    }
}

static void test_mutex_pend_timed_task_entry(void *arg)
{
    k_err_t err;
    k_mutex_t *mutex;

    mutex = arg ? (k_mutex_t *)arg : K_NULL;

    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_mutex_pend_timed(mutex, (k_tick_t)2000u);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

static void test_mutex_pend_destroy_entry(void *arg)
{
    k_err_t err;
    k_mutex_t *mutex;

    mutex = arg ? (k_mutex_t *)arg : K_NULL;

    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_mutex_pend(mutex);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);
        if (err == K_ERR_PEND_DESTROY) {
            // give the main thread a chance to run
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }

        test_context_set(TEST_CONTEXT_02);
        tos_task_yield();
        test_context_set(TEST_CONTEXT_03);
    }
}

static void test_mutex_holder_entry(void *arg)
{
    k_mutex_t *mutex;

    mutex = arg ? (k_mutex_t *)arg : K_NULL;

    while (K_TRUE) {
        // the holder hold the mutex first
        tos_mutex_pend(mutex);
        // then go to a long sleep
        tos_task_delay(TOS_TIME_FOREVER - 1);
    }
}

static void test_mutex_killer_entry(void *arg)
{
    k_task_t *task;

    task = arg ? (k_task_t *)arg : K_NULL;

    while (K_TRUE) {
        // after 1000, kill the holder
        tos_task_delay(1000);
        tos_task_destroy(task);
    }
}

TEST test_tos_mutex_create(void)
{
    k_err_t err;

    err = tos_mutex_create(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mutex_destroy(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

#if TOS_CFG_OBJ_DYNAMIC_CREATE_EN > 0u

TEST test_tos_mutex_create_dyn(void)
{
    k_err_t err;

    err = tos_mutex_create_dyn(&test_mutex_dyn_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mutex_destroy_dyn(test_mutex_dyn_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

#endif

TEST test_tos_mutex_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_mutex_destroy(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_mutex_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_mutex_pend(void)
{
    k_err_t err;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_mutex_create(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // hold the mutex first
    err = tos_mutex_pend(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with the same priority as current task
    err = tos_task_create(&test_task_00, "test_task", test_mutex_pend_task_entry,
                            (void *)(&test_mutex_00), k_curr_task->prio,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_context, 0);

    // yeild cpu to test_task_00
    tos_task_yield();

    // test_task_00 cannot get the mutex, so test_count won't be changed
    // test_task00's context now is TEST_CONTEXT_00, blocking for mutex pending
    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 0);

    // open the Pandora's box
    err = tos_mutex_post(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    tos_task_yield();

    /* yeild cpu to test_task_00, the mutex is post, test_task_00 will get a chance
       to run(from TEST_CONTEXT_00 to TEST_CONTEXT_02), test_count will be changed to 1
     */
    ASSERT_EQ(test_context, TEST_CONTEXT_02);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 1);

    /* we try to post the mutex, but we are not the owner(mutex is now hold by test_task_00)
       test_task_00's context now is TEST_CONTEXT_02
     */
    err = tos_mutex_post(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_MUTEX_NOT_OWNER);

    // yeild cpu to test_task_00, test_task_00 will do the mutex post(from TEST_CONTEXT_02 to TEST_CONTEXT_04)
    tos_task_yield();

    // test_task_00's context now is TEST_CONTEXT_04
    ASSERT_EQ(test_context, TEST_CONTEXT_04);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 1);

    // yeild cpu to test_task_00
    // test_task_00's context will run from TEST_CONTEXT_05 to TEST_CONTEXT_02
    tos_task_yield();

    // test_task_00' context now is TEST_CONTEXT_02
    // we destroy the mutex here
    ASSERT_EQ(test_context, TEST_CONTEXT_02);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 2);

    // test_task_00 context will run from TEST_CONTEXT_02 to TEST_CONTEXT_04
    tos_task_yield();

    ASSERT_EQ(test_context, TEST_CONTEXT_04);

    err = tos_mutex_destroy(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_mutex_pend_timed(void)
{
    k_err_t err;
    k_tick_t begin;
    k_prio_t original_prio;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_mutex_create(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // hold the mutex first
    err = tos_mutex_pend(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    original_prio = k_curr_task->prio;
    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task", test_mutex_pend_timed_task_entry,
                            (void *)(&test_mutex_00), k_curr_task->prio - 1,
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

    /*
        current task hold the mutex, and a higher task(test_task_00) is pending on it, our priority should
        rise up to test_task_00's priority
     */
    ASSERT_EQ(k_curr_task->prio, original_prio - 1);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mutex_destroy(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    /*
        after the mutex is destroyed, our priority should go back to original.
     */
    ASSERT_EQ(k_curr_task->prio, original_prio);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_mutex_post(void)
{
    k_err_t err;

    err = tos_mutex_create(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // hold the mutex first
    err = tos_mutex_pend(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mutex_post(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mutex_post(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_MUTEX_NOT_OWNER);

    err = tos_mutex_post(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_MUTEX_NOT_OWNER);

    err = tos_mutex_destroy(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_mutex_pend_destroy(void)
{
    k_err_t err;
    k_prio_t original_prio;

    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_mutex_create(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // hold the mutex first
    err = tos_mutex_pend(&test_mutex_00);
    // we are the owner
    ASSERT_EQ(err, K_ERR_NONE);

    original_prio = k_curr_task->prio;
    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task", test_mutex_pend_destroy_entry,
                            (void *)(&test_mutex_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // test_task_00 has a higher priority, so it will get running after the task create immediately
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    /*
        ATTENTION:
        PRIORITY INVERSION, test_task_00 are pending the mutex hold by us(current task), but current
        task's priority is lower than test_task_00, so our priority will be rised up to test_task_00's
        priority
     */
    ASSERT_EQ(k_curr_task->prio, original_prio - 1);

    /*
        after we destroy the mutex, our priority should go back to original priority, and the
        test_task_00(with a higher priority) get to run immediately after the tos_mutex_destroy.
        we count on the tos_task_delay in test_mutex_pend_destroy_entry to obtain the cpu again.
     */
    err = tos_mutex_destroy(&test_mutex_00);
    ASSERT_EQ(k_curr_task->prio, original_prio);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_mutex_pend_owner_die(void)
{
    k_err_t err;

    err = tos_mutex_create(&test_mutex_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task", test_mutex_holder_entry,
                            (void *)(&test_mutex_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a killer task with a lower priority than current task(numerically bigger)
    // we want the killer to do the murder for us when we are blocking on the tos_mutex_pend
    err = tos_task_create(&test_task_01, "test_task", test_mutex_killer_entry,
                            (void *)(&test_task_00), k_curr_task->prio + 1,
                            test_task_stack_01, sizeof(test_task_stack_01),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mutex_pend(&test_mutex_00);
    /*
        the killer(test_task_01) should kill the holder(test_task_00), so we shall return,
        and get a K_ERR_PEND_OWNER_DIE.
     */
    ASSERT_EQ(err, K_ERR_PEND_OWNER_DIE);

    // kill the killer
    err = tos_task_destroy(&test_task_01);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_mutex)
{
    RUN_TEST(test_tos_mutex_create);
    RUN_TEST(test_tos_mutex_destroy);
#if TOS_CFG_OBJ_DYNAMIC_CREATE_EN > 0u
    RUN_TEST(test_tos_mutex_create_dyn);
#endif
    RUN_TEST(test_tos_mutex_pend);
    RUN_TEST(test_tos_mutex_pend_timed);
    RUN_TEST(test_tos_mutex_post);
    RUN_TEST(test_mutex_pend_destroy);
    RUN_TEST(test_mutex_pend_owner_die);
}

