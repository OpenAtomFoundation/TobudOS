#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

#if TOS_CFG_ROUND_ROBIN_EN > 0u

SUITE(suit_robin);

static const k_timeslice_t test_robin_timeslice_00 = 10;
static const k_timeslice_t test_robin_timeslice_01 = 20;

static uint64_t test_robin_counter_00 = 1;
static uint64_t test_robin_counter_01 = 1;

static void test_robin_task_00_entry(void *arg)
{
    while (K_TRUE) {
        ++test_robin_counter_00;
    }
}

static void test_robin_task_01_entry(void *arg)
{
    while (K_TRUE) {
        ++test_robin_counter_01;
    }
}

TEST test_robin(void)
{
    k_err_t err;
    double r_c, r_t;
    int test_count = 0;
    const double deviation = 0.1;

    tos_robin_default_timeslice_config((k_timeslice_t)500u);
    err = tos_task_create(&test_task_00, "test_task_00", test_robin_task_00_entry,
                            NULL, k_curr_task->prio + 1,
                            test_task_stack_00, sizeof(test_task_stack_00),
                            test_robin_timeslice_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_create(&test_task_01, "test_task_01", test_robin_task_01_entry,
                            NULL, k_curr_task->prio + 1,
                            test_task_stack_01, sizeof(test_task_stack_01),
                            test_robin_timeslice_01);
    ASSERT_EQ(err, K_ERR_NONE);

    r_t = test_robin_timeslice_01 / test_robin_timeslice_00;
    while (test_count++ < 5) {
        tos_task_delay(1000);
        r_c = (double)test_robin_counter_01 / test_robin_counter_00;
        if (r_t > r_c) {
            ASSERT(r_t - r_c <= deviation);
        } else {
            ASSERT(r_c - r_t <= deviation);
        }
    }

    err = tos_task_destroy(&test_task_00);
    ASSERT_EQ(err, K_ERR_NONE);

    err = tos_task_destroy(&test_task_01);
    ASSERT_EQ(err, K_ERR_NONE);

    PASS();
}

SUITE(suit_robin)
{
    RUN_TEST(test_robin);
}

#endif

