#include "tos_k.h"

#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_uart.h"
#include "bsp_exit.h"
#include "bsp_epittimer.h"

void task1_entry(void *arg)
{
    while (1) {
        printf("task1\r\n");
        tos_task_delay(2000);
    }
}

void task2_entry(void *arg)
{
    while (1) {
        printf("task2\r\n");
        tos_task_delay(3000);
    }
}

k_task_t task1;
k_stack_t task1_stack[512];

k_task_t task2;
k_stack_t task2_stack[512];

int main(void)
{
    uart_init();
    led_init();
    beep_init();
    key_init();

    tos_knl_init();

    tos_task_create(&task1, "task1", task1_entry, NULL,
                        4,
                        task1_stack, sizeof(task1_stack),
                        0);
    tos_task_create(&task2, "task2", task2_entry, NULL,
                        4,
                        task2_stack, sizeof(task2_stack),
                        0);

    tos_knl_start();

    return 0;
}

