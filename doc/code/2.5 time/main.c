#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;

extern void entry_task_demo(void *arg);

void entry_task_demo(void *arg)
{
    k_time_t ms;
    k_tick_t systick, after_systick;

    systick = tos_millisec2tick(2000);
    printf("%d millisec equals to %lld ticks\n", 2000, systick);

    ms = tos_tick2millisec(1000);
    printf("%lld ticks equals to %d millisec\n", (k_tick_t)1000, ms);

    systick = tos_systick_get();
    printf("before sleep, systick is %lld\n", systick);

    tos_sleep_ms(2000);

    after_systick = tos_systick_get();
    printf("after sleep %d ms, systick is %lld\n", 2000, after_systick);

    printf("milliseconds sleep is about: %d\n", tos_tick2millisec(after_systick - systick));
}

int main(void)
{
    board_init();
    tos_knl_init();
    (void)tos_task_create(&task_demo, "receiver_higher_prio", entry_task_demo, NULL,
                            4, stack_task_demo, STK_SIZE_TASK_DEMO, 0);
    tos_knl_start();
}

