#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;

extern void entry_task_demo(void *arg);

void oneshot_timer_cb(void *arg)
{
    printf("this is oneshot timer callback, current systick: %lld\n", tos_systick_get());
}

void periodic_timer_cb(void *arg)
{
    printf("this is periodic timer callback, current systick: %lld\n", tos_systick_get());
}

void entry_task_demo(void *arg)
{
    k_timer_t oneshot_tmr;
    k_timer_t periodic_tmr;

    tos_timer_create(&oneshot_tmr, 3000, 0, oneshot_timer_cb, K_NULL, TOS_OPT_TIMER_ONESHOT);
    tos_timer_create(&periodic_tmr, 2000, 3000, periodic_timer_cb, K_NULL, TOS_OPT_TIMER_PERIODIC);

    printf("current systick: %lld\n", tos_systick_get());
    tos_timer_start(&oneshot_tmr);
    tos_timer_start(&periodic_tmr);

    while (K_TRUE) {
        tos_task_delay(1000);
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    (void)tos_task_create(&task_demo, "receiver_higher_prio", entry_task_demo, NULL,
                            4, stack_task_demo, STK_SIZE_TASK_DEMO, 0);
    tos_knl_start();
}

