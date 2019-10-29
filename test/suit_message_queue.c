#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_message_queue);

k_msg_q_t test_msg_q_00;
k_msg_q_t test_msg_q_01;
k_msg_q_t test_msg_q_02;

#define TEST_MESSAGE_Q_MSG_CNT     5
uint8_t test_msg_q_pool_00[sizeof(void *) * TEST_MESSAGE_Q_MSG_CNT];
uint8_t test_msg_q_pool_01[sizeof(void *) * TEST_MESSAGE_Q_MSG_CNT];
uint8_t test_msg_q_pool_02[sizeof(void *) * TEST_MESSAGE_Q_MSG_CNT];

static void test_message_queue_pend_task_entry(void *arg)
{
    k_err_t err;
    k_msg_q_t *msg_q;
    void *msg;

    msg_q = arg ? (k_msg_q_t *)arg : K_NULL;
    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_msg_q_pend(msg_q, &msg, TOS_TIME_FOREVER);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);
        if (err == K_ERR_NONE) {
            test_msg_set(msg);
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

static void test_message_queue_pend_timed_task_entry(void *arg)
{
    k_err_t err;
    k_msg_q_t *msg_q;
    void *msg;

    msg_q = arg ? (k_msg_q_t *)arg : K_NULL;

    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_msg_q_pend(msg_q, &msg, (k_tick_t)2000u);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);
        if (err == K_ERR_NONE) {
            test_msg_set(msg);
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

TEST test_tos_message_queue_create(void)
{
    k_err_t err;

    err = tos_msg_q_create(&test_msg_q_00, test_msg_q_pool_00, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_create(&test_msg_q_01, test_msg_q_pool_01, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_create(&test_msg_q_02, test_msg_q_pool_02, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_destroy(&test_msg_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_destroy(&test_msg_q_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_destroy(&test_msg_q_02);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_message_queue_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_msg_q_destroy(&test_msg_q_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_msg_q_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_message_queue_pend(void)
{
    k_err_t err;
    void *msg = (void *)0xDEADBEEF;

    test_msg_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_msg_q_create(&test_msg_q_00, test_msg_q_pool_00, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_message_queue_pend_task_entry,
                            (void *)(&test_msg_q_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_msg_q_post(&test_msg_q_00, msg);

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    ASSERT_EQ(test_msg, msg);

    tos_msg_q_destroy(&test_msg_q_00);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_message_queue_pend_dyn(void)
{
    k_err_t err;
    void *msg = (void *)0xDEADBEEF;

    test_msg_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_msg_q_create_dyn(&test_msg_q_00, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_message_queue_pend_task_entry,
                            (void *)(&test_msg_q_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    tos_msg_q_post(&test_msg_q_00, msg);

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_context, TEST_CONTEXT_00);

    ASSERT_EQ(test_msg, msg);

    tos_msg_q_destroy_dyn(&test_msg_q_00);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_message_queue_pend_timed(void)
{
    k_err_t err;
    k_tick_t begin;
    void *msg = (void *)0xDEADBEEF;

    test_msg_reset();
    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_msg_q_create(&test_msg_q_00, test_msg_q_pool_00, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task", test_message_queue_pend_timed_task_entry,
                            (void *)(&test_msg_q_00), k_curr_task->prio - 1,
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

    ASSERT_EQ(test_msg, K_NULL);

    err = tos_msg_q_post(&test_msg_q_00, msg);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_count, 2);
    ASSERT_EQ(test_err, K_ERR_NONE);

    ASSERT_EQ(test_msg, msg);

    err = tos_msg_q_destroy(&test_msg_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_01);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_message_queue_post_all(void)
{
    k_err_t err;
    void *msg = (void *)0xDEADBEEF;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_msg_q_create(&test_msg_q_00, test_msg_q_pool_00, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task_00", test_message_queue_pend_task_entry,
                            (void *)(&test_msg_q_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_01, "test_task_01", test_message_queue_pend_task_entry,
                            (void *)(&test_msg_q_00), k_curr_task->prio - 1,
                            test_task_stack_01, sizeof(test_task_stack_01),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_02, "test_task_02", test_message_queue_pend_task_entry,
                            (void *)(&test_msg_q_00), k_curr_task->prio - 1,
                            test_task_stack_02, sizeof(test_task_stack_02),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);

    err = tos_msg_q_post_all(&test_msg_q_00, msg);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 3);
    ASSERT_EQ(test_msg, msg);

    err = tos_msg_q_post(&test_msg_q_00, msg);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 4);
    ASSERT_EQ(test_msg, msg);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_02);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_destroy(&test_msg_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_message_queue_flush(void)
{
    k_err_t err;
    void *msg0 = (void *)0xDEADBEE0, *msg1 = (void *)0xDEADBEE1, *msg2 = (void *)0xDEADBEE2;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_msg_q_create(&test_msg_q_00, test_msg_q_pool_00, TEST_MESSAGE_Q_MSG_CNT);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a lower priority than current task(numerically bigger)
    err = tos_task_create(&test_task_00, "test_task_00", test_message_queue_pend_task_entry,
                            (void *)(&test_msg_q_00), k_curr_task->prio + 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_post(&test_msg_q_00, msg0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_post(&test_msg_q_00, msg1);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_post(&test_msg_q_00, msg2);
    ASSERT_EQ(err, K_ERR_NONE);

    // we have post three msg, but we flush them, test_task_00 should not receive any message
    err = tos_msg_q_flush(&test_msg_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // yeild cpu to test_task_00
    tos_task_delay(500);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_count, 0);

    err = tos_msg_q_post(&test_msg_q_00, msg0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_post(&test_msg_q_00, msg1);
    ASSERT_EQ(err, K_ERR_NONE);

    // yeild cpu to test_task_00
    tos_task_delay(500);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 2);

    // test_task_00 is pending, this post will send msg0 to test_task_00
    err = tos_msg_q_post(&test_msg_q_00, msg0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_post(&test_msg_q_00, msg1);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_post(&test_msg_q_00, msg2);
    ASSERT_EQ(err, K_ERR_NONE);

    // msg1/msg2 has not been received yet, queue flush will discard these two message
    err = tos_msg_q_flush(&test_msg_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // yeild cpu to test_task_00
    tos_task_delay(500);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 3);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_msg_q_destroy(&test_msg_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

SUITE(suit_message_queue)
{
    RUN_TEST(test_tos_message_queue_create);
    RUN_TEST(test_tos_message_queue_destroy);
    RUN_TEST(test_tos_message_queue_pend);
    RUN_TEST(test_tos_message_queue_pend_dyn);
    RUN_TEST(test_tos_message_queue_pend_timed);
    RUN_TEST(test_tos_message_queue_post_all);
    RUN_TEST(test_tos_message_queue_flush);
}

