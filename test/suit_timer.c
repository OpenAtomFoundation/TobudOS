#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE(suit_timer);

k_timer_t test_timer_00;
k_timer_t test_timer_01;
k_timer_t test_timer_02;

static uint32_t test_timer_oneshot_count = 0;
static uint32_t test_timer_periodic_count = 0;

static void test_timer_call_back_dummy(void *arg)
{
}

static void test_timer_oneshot_call_back(void *arg)
{
    ++test_timer_oneshot_count;
}

static void test_timer_periodic_call_back(void *arg)
{
    ++test_timer_periodic_count;
}

TEST test_tos_timer_create(void)
{
    k_err_t err;

    err = tos_timer_create(&test_timer_00, (k_tick_t)0u, (k_tick_t)0u,
                            test_timer_call_back_dummy, K_NULL,
                            TOS_OPT_TIMER_ONESHOT);
    ASSERT_EQ(err, K_ERR_TIMER_INVALID_DELAY);

    err = tos_timer_create(&test_timer_00, (k_tick_t)0u, (k_tick_t)200u,
                            test_timer_call_back_dummy, K_NULL,
                            TOS_OPT_TIMER_ONESHOT);
    ASSERT_EQ(err, K_ERR_TIMER_INVALID_DELAY);

    err = tos_timer_create(&test_timer_00, (k_tick_t)0u, (k_tick_t)0u,
                        test_timer_call_back_dummy, K_NULL,
                        TOS_OPT_TIMER_PERIODIC);
    ASSERT_EQ(err, K_ERR_TIMER_INVALID_PERIOD);

    err = tos_timer_create(&test_timer_00, (k_tick_t)200u, (k_tick_t)0u,
                        test_timer_call_back_dummy, K_NULL,
                        TOS_OPT_TIMER_PERIODIC);
    ASSERT_EQ(err, K_ERR_TIMER_INVALID_PERIOD);


    err = tos_timer_create(&test_timer_00, (k_tick_t)200u, (k_tick_t)0u,
                        K_NULL, K_NULL,
                        TOS_OPT_TIMER_ONESHOT);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    err = tos_timer_create(&test_timer_00, TOS_TIME_FOREVER, (k_tick_t)0u,
                        test_timer_call_back_dummy, K_NULL,
                        TOS_OPT_TIMER_ONESHOT);
    ASSERT_EQ(err, K_ERR_TIMER_DELAY_FOREVER);

    err = tos_timer_create(&test_timer_00, (k_tick_t)0u, TOS_TIME_FOREVER,
                        test_timer_call_back_dummy, K_NULL,
                        TOS_OPT_TIMER_PERIODIC);
    ASSERT_EQ(err, K_ERR_TIMER_PERIOD_FOREVER);

    err = tos_timer_create(&test_timer_00, (k_tick_t)200u, (k_tick_t)0u,
                        test_timer_call_back_dummy, K_NULL,
                        TOS_OPT_TIMER_ONESHOT);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_timer_create(&test_timer_01, (k_tick_t)0u, (k_tick_t)200u,
                    test_timer_call_back_dummy, K_NULL,
                    TOS_OPT_TIMER_PERIODIC);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_timer_destroy(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_timer_destroy(&test_timer_01);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

TEST test_tos_timer_destroy(void)
{
    k_err_t err;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_timer_destroy(&test_timer_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_timer_destroy(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

    PASS();
}

TEST test_tos_timer_stop(void)
{
    k_err_t err;

    err = tos_timer_stop(K_NULL);
    ASSERT_EQ(err, K_ERR_OBJ_PTR_NULL);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    err = tos_timer_stop(&test_timer_00);
    ASSERT_EQ(err, K_ERR_OBJ_INVALID);
#endif

    err = tos_timer_create(&test_timer_00, (k_tick_t)500u, (k_tick_t)0u,
                    test_timer_call_back_dummy, K_NULL,
                    TOS_OPT_TIMER_ONESHOT);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_timer_00.state, TIMER_STATE_STOPPED);

    err = tos_timer_stop(&test_timer_00);
    ASSERT_EQ(err, K_ERR_TIMER_STOPPED);

    err = tos_timer_start(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_timer_00.state, TIMER_STATE_RUNNING);

    err = tos_task_delay(500u + 10u);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_timer_00.state, TIMER_STATE_COMPLETED);

    err = tos_timer_stop(&test_timer_00);
    ASSERT_EQ(err, K_ERR_TIMER_STOPPED);

    err = tos_timer_start(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_timer_00.state, TIMER_STATE_RUNNING);

    err = tos_timer_stop(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);
    ASSERT_EQ(test_timer_00.state, TIMER_STATE_STOPPED);

    err = tos_timer_destroy(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);
    
    PASS();
}

TEST test_tos_timer_oneshot_functional(void)
{
    k_err_t err;

    err = tos_timer_create(&test_timer_00, (k_tick_t)500u, (k_tick_t)0u,
                        test_timer_oneshot_call_back, K_NULL,
                        TOS_OPT_TIMER_ONESHOT);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_timer_oneshot_count, 0);
    err = tos_timer_start(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_delay(500u + 10u);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_timer_00.state, TIMER_STATE_COMPLETED);
    ASSERT_EQ(test_timer_oneshot_count, 1);

    err = tos_timer_stop(&test_timer_00);
    ASSERT_EQ(err, K_ERR_TIMER_STOPPED);

    err = tos_timer_destroy(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);
    
    PASS();
}

TEST test_tos_timer_periodic_functional(void)
{
    int count = 0;
    k_err_t err;

    err = tos_timer_create(&test_timer_00, (k_tick_t)500u, (k_tick_t)400u,
                    test_timer_periodic_call_back, K_NULL,
                    TOS_OPT_TIMER_PERIODIC);
    ASSERT_EQ(err, K_ERR_NONE);

    
    ASSERT_EQ(test_timer_periodic_count, 0);
    err = tos_timer_start(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_delay(500u + 10u);
    ASSERT_EQ(err, K_ERR_NONE);

    ASSERT_EQ(test_timer_00.state, TIMER_STATE_RUNNING);
    ASSERT_EQ(test_timer_periodic_count, 1);

    while (count++ < 4) {
        err = tos_task_delay(400u + 10u);
        ASSERT_EQ(err, K_ERR_NONE);

        ASSERT_EQ(test_timer_00.state, TIMER_STATE_RUNNING);
        ASSERT_EQ(test_timer_periodic_count, 1 + count);
    }

    err = tos_timer_destroy(&test_timer_00);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_timer)
{
    RUN_TEST(test_tos_timer_create);
    RUN_TEST(test_tos_timer_destroy);
    RUN_TEST(test_tos_timer_stop);
    RUN_TEST(test_tos_timer_oneshot_functional);
    RUN_TEST(test_tos_timer_periodic_functional);
}

