#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_priority_mail_queue);

k_prio_mail_q_t test_prio_mail_q_00;
k_prio_mail_q_t test_prio_mail_q_01;
k_prio_mail_q_t test_prio_mail_q_02;

#define TEST_PRIO_MAIL_Q_MAIL_CNT     5
uint8_t test_prio_mail_q_pool_00[sizeof(test_mail_t) * TEST_PRIO_MAIL_Q_MAIL_CNT];
uint8_t test_prio_mail_q_pool_01[sizeof(test_mail_t) * TEST_PRIO_MAIL_Q_MAIL_CNT];
uint8_t test_prio_mail_q_pool_02[sizeof(test_mail_t) * TEST_PRIO_MAIL_Q_MAIL_CNT];

static void test_prio_mail_queue_pend_task_entry(void *arg)
{
    k_err_t err;
    k_prio_mail_q_t *mail_q;
    test_mail_t mail;
    size_t mail_size;

    mail_q = arg ? (k_prio_mail_q_t *)arg : K_NULL;
    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_prio_mail_q_pend(mail_q, &mail, &mail_size, TOS_TIME_FOREVER);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);
        if (err == K_ERR_NONE) {
            test_mail_set(&mail, mail_size);
            tos_task_yield(); // yeild to test main
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

static void test_prio_mail_queue_pend_timed_task_entry(void *arg)
{
    k_err_t err;
    k_prio_mail_q_t *mail_q;
    test_mail_t mail;
    size_t mail_size;

    mail_q = arg ? (k_prio_mail_q_t *)arg : K_NULL;

    while (K_TRUE) {
        test_context_set(TEST_CONTEXT_00);
        err = tos_prio_mail_q_pend(mail_q, &mail, &mail_size, (k_tick_t)2000u);
        test_context_set(TEST_CONTEXT_01);
        test_err_set(err);
        if (err == K_ERR_NONE) {
            test_mail_set(&mail, mail_size);
        }

        if (err == K_ERR_PEND_DESTROY) {
            tos_task_delay(TOS_TIME_FOREVER - 1);
        }

        if (test_task_hook) {
            test_task_hook();
        }
    }
}

TEST test_tos_prio_mail_queue_create(void)
{
    k_err_t err;

    err = tos_prio_mail_q_create(&test_prio_mail_q_00, test_prio_mail_q_pool_00, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_create(&test_prio_mail_q_01, test_prio_mail_q_pool_01, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_create(&test_prio_mail_q_02, test_prio_mail_q_pool_02, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_destroy(&test_prio_mail_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_destroy(&test_prio_mail_q_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_destroy(&test_prio_mail_q_02);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_prio_mail_queue_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_prio_mail_q_destroy(&test_prio_mail_q_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_prio_mail_q_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_prio_mail_queue_pend(void)
{
    k_err_t err;
    test_mail_t mail0 = {
        0xDEAD0000,
        "test_mail0",
        'T',
    };
    test_mail_t mail1 = {
        0xDEAD1111,
        "test_mail1",
        'U',
    };
    test_mail_t mail2 = {
        0xDEAD2222,
        "test_mail2",
        'V',
    };

    test_mail_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_prio_mail_q_create(&test_prio_mail_q_00, test_prio_mail_q_pool_00, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with the same priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_prio_mail_queue_pend_task_entry,
                            (void *)(&test_prio_mail_q_00), k_curr_task->prio,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT(test_context != TEST_CONTEXT_00);

    // we post mail2, mail1, mail0 with the certain priority, the test_task should receive mail according the priority
    tos_prio_mail_q_post(&test_prio_mail_q_00, &mail2, sizeof(test_mail_t), 2);
    tos_prio_mail_q_post(&test_prio_mail_q_00, &mail1, sizeof(test_mail_t), 1);
    tos_prio_mail_q_post(&test_prio_mail_q_00, &mail0, sizeof(test_mail_t), 0);

    tos_task_yield(); // yeild to test_task

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    ASSERT_EQ(test_mail.a, mail0.a);
    ASSERT_EQ(strcmp(test_mail.b, mail0.b), 0);
    ASSERT_EQ(test_mail.c, mail0.c);
    tos_task_yield(); // yeild to test_task

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    ASSERT_EQ(test_mail.a, mail1.a);
    ASSERT_EQ(strcmp(test_mail.b, mail1.b), 0);
    ASSERT_EQ(test_mail.c, mail1.c);
    tos_task_yield(); // yeild to test_task

    ASSERT_EQ(test_mail.a, mail2.a);
    ASSERT_EQ(strcmp(test_mail.b, mail2.b), 0);
    ASSERT_EQ(test_mail.c, mail2.c);
    tos_task_yield(); // yeild to test_task

    tos_prio_mail_q_destroy(&test_prio_mail_q_00);
    tos_task_yield(); // yeild to test_task
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_prio_mail_queue_pend_dyn(void)
{
    k_err_t err;
    test_mail_t mail0 = {
        0xDEAD0000,
        "test_mail0",
        'T',
    };
    test_mail_t mail1 = {
        0xDEAD1111,
        "test_mail1",
        'U',
    };
    test_mail_t mail2 = {
        0xDEAD2222,
        "test_mail2",
        'V',
    };

    test_mail_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_prio_mail_q_create_dyn(&test_prio_mail_q_00, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with the same priority than current task
    err = tos_task_create(&test_task_00, "test_task", test_prio_mail_queue_pend_task_entry,
                            (void *)(&test_prio_mail_q_00), k_curr_task->prio,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);
    ASSERT(test_context != TEST_CONTEXT_00);

    // we post mail2, mail1, mail0 with the certain priority, the test_task should receive mail according the priority
    tos_prio_mail_q_post(&test_prio_mail_q_00, &mail2, sizeof(test_mail_t), 2);
    tos_prio_mail_q_post(&test_prio_mail_q_00, &mail1, sizeof(test_mail_t), 1);
    tos_prio_mail_q_post(&test_prio_mail_q_00, &mail0, sizeof(test_mail_t), 0);

    tos_task_yield(); // yeild to test_task

    ASSERT_EQ(test_count, 0);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    ASSERT_EQ(test_mail.a, mail0.a);
    ASSERT_EQ(strcmp(test_mail.b, mail0.b), 0);
    ASSERT_EQ(test_mail.c, mail0.c);
    tos_task_yield(); // yeild to test_task

    ASSERT_EQ(test_count, 1);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    ASSERT_EQ(test_mail.a, mail1.a);
    ASSERT_EQ(strcmp(test_mail.b, mail1.b), 0);
    ASSERT_EQ(test_mail.c, mail1.c);
    tos_task_yield(); // yeild to test_task

    ASSERT_EQ(test_mail.a, mail2.a);
    ASSERT_EQ(strcmp(test_mail.b, mail2.b), 0);
    ASSERT_EQ(test_mail.c, mail2.c);
    tos_task_yield(); // yeild to test_task

    tos_prio_mail_q_destroy_dyn(&test_prio_mail_q_00);
    tos_task_yield(); // yeild to test_task
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);
    ASSERT_EQ(test_context, TEST_CONTEXT_01);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_prio_mail_queue_pend_timed(void)
{
    k_err_t err;
    k_tick_t begin;
    test_mail_t mail = {
        0xDEADBEEF,
        "test_mail",
        'T',
    };

    test_mail_reset();
    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_prio_mail_q_create(&test_prio_mail_q_00, test_prio_mail_q_pool_00, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task", test_prio_mail_queue_pend_timed_task_entry,
                            (void *)(&test_prio_mail_q_00), k_curr_task->prio - 1,
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

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_count, 2);
    ASSERT_EQ(test_err, K_ERR_NONE);

    ASSERT_EQ(test_mail.a, mail.a);
    ASSERT_EQ(strcmp(test_mail.b, mail.b), 0);
    ASSERT_EQ(test_mail.c, mail.c);

    err = tos_prio_mail_q_destroy(&test_prio_mail_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_context, TEST_CONTEXT_01);
    ASSERT_EQ(test_err, K_ERR_PEND_DESTROY);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_prio_mail_queue_post_all(void)
{
    k_err_t err;
    test_mail_t mail = {
        0xDEADBEEF,
        "test_mail",
        'T',
    };

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_prio_mail_q_create(&test_prio_mail_q_00, test_prio_mail_q_pool_00, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_00, "test_task_00", test_prio_mail_queue_pend_task_entry,
                            (void *)(&test_prio_mail_q_00), k_curr_task->prio - 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_01, "test_task_01", test_prio_mail_queue_pend_task_entry,
                            (void *)(&test_prio_mail_q_00), k_curr_task->prio - 1,
                            test_task_stack_01, sizeof(test_task_stack_01),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a higher priority than current task(numerically smaller)
    err = tos_task_create(&test_task_02, "test_task_02", test_prio_mail_queue_pend_task_entry,
                            (void *)(&test_prio_mail_q_00), k_curr_task->prio - 1,
                            test_task_stack_02, sizeof(test_task_stack_02),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 0);

    err = tos_prio_mail_q_post_all(&test_prio_mail_q_00, &mail, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 3);
    ASSERT_EQ(test_mail.a, mail.a);
    ASSERT_EQ(strcmp(test_mail.b, mail.b), 0);
    ASSERT_EQ(test_mail.c, mail.c);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_count, 4);
    ASSERT_EQ(test_mail.a, mail.a);
    ASSERT_EQ(strcmp(test_mail.b, mail.b), 0);
    ASSERT_EQ(test_mail.c, mail.c);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_01);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_02);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_destroy(&test_prio_mail_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

TEST test_tos_prio_mail_queue_flush(void)
{
    k_err_t err;
    void *mail0 = (void *)0xDEADBEE0, *mail1 = (void *)0xDEADBEE1, *mail2 = (void *)0xDEADBEE2;

    test_err_reset();
    test_context_reset();
    test_count_reset();
    test_task_hook_set(test_count_inc);

    err = tos_prio_mail_q_create(&test_prio_mail_q_00, test_prio_mail_q_pool_00, TEST_PRIO_MAIL_Q_MAIL_CNT, sizeof(test_mail_t));
    ASSERT_EQ(err, K_ERR_NONE);

    // create a test task with a lower priority than current task(numerically bigger)
    err = tos_task_create(&test_task_00, "test_task_00", test_prio_mail_queue_pend_task_entry,
                            (void *)(&test_prio_mail_q_00), k_curr_task->prio + 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail0, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail1, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail2, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // we have post three mail, but we flush them, test_task_00 should not receive any message
    err = tos_prio_mail_q_flush(&test_prio_mail_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // yeild cpu to test_task_00
    tos_task_delay(500);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_count, 0);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail0, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail1, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // yeild cpu to test_task_00
    tos_task_delay(500);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 2);

    // test_task_00 is pending, this post will send mail0 to test_task_00
    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail0, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail1, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_post(&test_prio_mail_q_00, &mail2, sizeof(test_mail_t), 0);
    ASSERT_EQ(err, K_ERR_NONE);

    // mail1/mail2 has not been received yet, queue flush will discard these two message
    err = tos_prio_mail_q_flush(&test_prio_mail_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    // yeild cpu to test_task_00
    tos_task_delay(500);

    ASSERT_EQ(test_context, TEST_CONTEXT_00);
    ASSERT_EQ(test_err, K_ERR_NONE);
    ASSERT_EQ(test_count, 3);

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_prio_mail_q_destroy(&test_prio_mail_q_00);
    ASSERT_EQ(err, K_ERR_NONE);

    test_task_hook_reset();

    PASS();
}

SUITE(suit_priority_mail_queue)
{
    RUN_TEST(test_tos_prio_mail_queue_create);
    RUN_TEST(test_tos_prio_mail_queue_destroy);
    RUN_TEST(test_tos_prio_mail_queue_pend);
    RUN_TEST(test_tos_prio_mail_queue_pend_dyn);
    RUN_TEST(test_tos_prio_mail_queue_pend_timed);
    RUN_TEST(test_tos_prio_mail_queue_post_all);
    RUN_TEST(test_tos_prio_mail_queue_flush);
}

