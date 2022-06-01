#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_task);

k_stack_t test_task_stack_00[TEST_TASK_STACK_SIZE_00];
k_stack_t test_task_stack_01[TEST_TASK_STACK_SIZE_01];
k_stack_t test_task_stack_02[TEST_TASK_STACK_SIZE_02];

k_task_t test_task_00;
k_task_t test_task_01;
k_task_t test_task_02;

static void test_task_entry(void *arg)
{
    k_tick_t delay;

    // if arg == K_NULL, delay 2000 ticks(default)
    delay = arg ? *((k_tick_t *)arg) : (k_tick_t)2000u;

    while (K_TRUE) {
        tos_task_delay(delay);
        if (test_task_hook) {
            test_task_hook();
        }
    }
}

TEST test_tos_task_create(void)
{
    k_err_t err;

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            K_NULL, K_TASK_PRIO_IDLE,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_TASK_PRIO_INVALID);

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            K_NULL, K_TASK_PRIO_IDLE + 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_TASK_PRIO_INVALID);

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            K_NULL, TEST_TASK_PRIO_00,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_task_destroy(void)
{
    k_err_t err;

    err = tos_task_destroy(&k_idle_task);
    ASSERT_EQ(err, K_ERR_TASK_DESTROY_IDLE);

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            K_NULL, TEST_TASK_PRIO_00,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_task_create_destroy_dyn(void)
{
    k_err_t err;
    k_task_t *task;
    k_mmheap_info_t mm_info_before, mm_info_after;

    err = tos_mmheap_check(&mm_info_before);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_create_dyn(&task, "test_dyn_task", test_task_entry,
                            K_NULL, TEST_TASK_PRIO_00, TEST_TASK_STACK_SIZE_00, 0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(task);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID_ALLOC_TYPE);

    err = tos_task_destroy_dyn(task);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_mmheap_check(&mm_info_after);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(mm_info_before.free, mm_info_after.free);

    PASS();
}

TEST test_tos_task_delay(void)
{
    int try = 0;
    k_err_t err;
    k_tick_t begin, end, delay, deviation = (k_tick_t)10u;

    while (try++ < 5) {
        delay = try * 300 + 200;

        begin = tos_systick_get();
        err = tos_task_delay(delay);
        ASSERT_EQ(err, K_ERR_NONE);
        end = tos_systick_get();
        ASSERT(end - begin <= delay + deviation);
    }

    PASS();
}

TEST test_tos_task_delay_abort(void)
{
    k_err_t err;
    k_tick_t delay = TOS_TIME_FOREVER - 1;

    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            (void *)(&delay), TEST_TASK_PRIO_00,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);

    // yield cpu to test_task_00
    err = tos_task_delay(100);
    ASSERT_EQ(err, K_ERR_NONE);

    // test_task_00 has a huge delay and will not increase test_count
    ASSERT_EQ(test_count, 0);

    // abort test_task_00's huge delay
    err = tos_task_delay_abort(&test_task_00);
    // yield cpu to test_task_00
    tos_task_delay(100);

    // test_count should be increased, and test_task_00 will be trapped into the huge delay again
    // so the test_count should be 1 and only 1
    ASSERT_EQ(test_count, 1);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_task_suspend_resume(void)
{
    k_err_t err;
    uint32_t test_count_cache;
    k_tick_t delay = (k_tick_t)20u;

    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            (void *)(&delay), TEST_TASK_PRIO_00,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // yield cpu to test_task_00
    tos_task_delay(500);

    // suspend test_task_00
    err = tos_task_suspend(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // cache the current test_count
    test_count_cache = test_count;

    // yield cpu, test_task_00 is suspended, so test_count should no more increasing
    tos_task_delay(500);

    ASSERT_EQ(test_count_cache, test_count);

    // resume test_task_00 now
    err = tos_task_resume(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // yield cpu
    tos_task_delay(500);

    // test_task_00 obtain cpu, test_count get increased.
    ASSERT(test_count > test_count_cache);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_task_prio_change(void)
{
    k_err_t err;

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            K_NULL, TEST_TASK_PRIO_00,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_prio_change(&test_task_00, K_TASK_PRIO_IDLE);
    ASSERT_EQ(err, K_ERR_TASK_PRIO_INVALID);

    err = tos_task_prio_change(&test_task_00, TEST_TASK_PRIO_00 + 1);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_task_00.prio, TEST_TASK_PRIO_00 + 1);

    err = tos_task_prio_change(&test_task_00, TEST_TASK_PRIO_00 - 1);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_task_00.prio, TEST_TASK_PRIO_00 - 1);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_task_yeild(void)
{
    k_err_t err;
    uint32_t test_count_cache;
    k_tick_t delay = (k_tick_t)20u, begin;

    // reset test count
    test_count_reset();
    test_task_hook_set(test_count_inc);

    // create a task with a lower priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            (void *)(&delay), k_curr_task->prio,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    begin = tos_systick_get();

    test_count_cache = test_count;

    /* we obtain cpu now, if we don't do yield, test_task_00 will never get a
       chance to run, so test_count will never be changed.
     */
    // in 2000 ticks, test_count will never be changed.
    while (tos_systick_get() - begin < (k_tick_t)1000u) {
        ASSERT_EQ(test_count, test_count_cache);
    }

    while (test_count == test_count_cache) {
        /* we keep trying to yield, test_task_00 will get a chance to increase test_count at last.
         */
        tos_task_yield();
    }

    ASSERT(test_count > test_count_cache);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_task_find(void)
{
    k_err_t err;
    k_task_t *task;

    err = tos_task_create(&test_task_00, "test_task", test_task_entry,
                            NULL, TEST_TASK_PRIO_00,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    task = tos_task_find(NULL);
    ASSERT_EQ(task, NULL);

    task = tos_task_find("not exist");
    ASSERT_EQ(task, NULL);

    task = tos_task_find("test_task");
    ASSERT_EQ(task, &test_task_00);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_task)
{
    RUN_TEST(test_tos_task_create);
    RUN_TEST(test_tos_task_destroy);
    RUN_TEST(test_tos_task_create_destroy_dyn);
    RUN_TEST(test_tos_task_delay);
    RUN_TEST(test_tos_task_delay_abort);
    RUN_TEST(test_tos_task_suspend_resume);
    RUN_TEST(test_tos_task_prio_change);
    RUN_TEST(test_tos_task_yeild);
    RUN_TEST(test_tos_task_find);
}

