#include "iom32.h"
#include "uart.h"
#include "tos_k.h"

#include "stdio.h"

#pragma vector = TIMER1_OVF_vect
__interrupt void TIMER1_OVF(void)
{
    if (tos_knl_is_running()) {
        tos_knl_irq_enter();
        tos_tick_handler();
        tos_knl_irq_leave();
    }
}

#pragma vector = USART_RXC_vect
__interrupt void UART0_rx(void)
{
    unsigned char rx_data;
    rx_data = UDR;
}

void interrupt_enable(void)
{
    SREG |= (1 << 7);
}

void task1_entry(void *arg)
{
    while (1) {
        uart_send_string("task1\r\n");
        tos_task_delay(2000);
    }
}

void task2_entry(void *arg)
{
    while (1) {
        uart_send_string("task2\r\n");
        tos_task_delay(3000);
    }
}

k_task_t task1;
k_stack_t task1_stack[128];

k_task_t task2;
k_stack_t task2_stack[128];

#define APPLICATION_TASK_STK_SIZE       256

k_task_t task_app;
k_stack_t task_app_stack[128];

void application_entry(void * arg)
{
    tos_task_create(&task1, "task1", task1_entry, NULL,
                        4,
                        task1_stack, sizeof(task1_stack),
                        0);

    tos_task_create(&task2, "task2", task2_entry, NULL,
                        4,
                        task2_stack, sizeof(task2_stack),
                        0);
}

int main(void)
{
    uart_init(9600);
    timer1();

    uart_send_string("welcome to TencentOS tiny!\r\n");
    printf("welcome to TencentOS tiny!\r\n");

    tos_knl_init();

    tos_task_create(&task_app, "app", application_entry, NULL,
                        4,
                        task_app_stack, sizeof(task_app_stack),
                        0);

    tos_knl_start();

    while (1);
}

