#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512
#define STK_SIZE_TASK_SAMPLE    512

#define PRIO_TASK_DEMO          4
#define PRIO_TASK_SAMPLE        (PRIO_TASK_DEMO - 1)

const k_timeslice_t timeslice_demo1 = 10;
const k_timeslice_t timeslice_demo2 = 20;

k_stack_t stack_task_demo1[STK_SIZE_TASK_DEMO];
k_stack_t stack_task_demo2[STK_SIZE_TASK_DEMO];
k_stack_t stack_task_sample[STK_SIZE_TASK_SAMPLE];

k_task_t task_demo1;
k_task_t task_demo2;
k_task_t task_sample;

extern void entry_task_demo1(void *arg);
extern void entry_task_demo2(void *arg);
extern void entry_task_sample(void *arg);

uint64_t demo1_counter = 0;
uint64_t demo2_counter = 0;

void entry_task_demo1(void *arg)
{
    while (K_TRUE) {
        ++demo1_counter;
    }
}

void entry_task_demo2(void *arg)
{
    while (K_TRUE) {
        ++demo2_counter;
    }
}

void entry_task_sample(void *arg)
{
    while (K_TRUE) {
        printf("demo1_counter: %lld\n", demo1_counter);
        printf("demo2_counter: %lld\n", demo2_counter);
        printf("demo2_counter / demo1_counter = %f\n",
            (double)demo2_counter / demo1_counter);
        printf("should almost equals to:\n");
        printf("timeslice_demo2 / timeslice_demo1 = %f\n\n", (double)timeslice_demo2 / timeslice_demo1);
        tos_task_delay(1000);
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_robin_default_timeslice_config((k_timeslice_t)500u);
    (void)tos_task_create(&task_demo1, "demo1", entry_task_demo1, NULL,
                            PRIO_TASK_DEMO, stack_task_demo1, STK_SIZE_TASK_DEMO,
                            timeslice_demo1);
    (void)tos_task_create(&task_demo2, "demo2", entry_task_demo2, NULL,
                            PRIO_TASK_DEMO, stack_task_demo2, STK_SIZE_TASK_DEMO,
                            timeslice_demo2);
    (void)tos_task_create(&task_sample, "sample", entry_task_sample, NULL,
                            PRIO_TASK_SAMPLE, stack_task_sample, STK_SIZE_TASK_SAMPLE,
                            0);
    tos_knl_start();
}

