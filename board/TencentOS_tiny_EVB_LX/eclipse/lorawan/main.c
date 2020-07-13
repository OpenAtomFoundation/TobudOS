#include "mcu_init.h"
#include "tos_k.h"
#include "tos_hal.h"

#define LORA_TASK_SIZE 4096

k_task_t k_task_lora;
uint8_t k_lora_stk[LORA_TASK_SIZE];


void USART0_IRQHandler() {
    tos_knl_irq_enter();
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        uint8_t data = usart_data_receive(USART0);
        printf("%c\n", data);
    }
    tos_knl_irq_leave();
}

void USART1_IRQHandler() {
    tos_knl_irq_enter();
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)){
        uint8_t data = usart_data_receive(USART1);
        tos_at_uart_input_byte(data);
    }
    tos_knl_irq_leave();
}

void USART2_IRQHandler() {
    tos_knl_irq_enter();
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE)){
        uint8_t data = usart_data_receive(USART2);
    }
    tos_knl_irq_leave();
}

void UART3_IRQHandler() {
    tos_knl_irq_enter();
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)){
        uint8_t data = usart_data_receive(UART3);
        tos_shell_input_byte(data);
    }
    tos_knl_irq_leave();
}

void task_bled(void *pdata)
{
    int cnt = 0;
    while (1) {
        printf("blink led task cnt: %d\n", cnt++);
        gpio_bit_write(LED_GPIO_PORT, LED_PIN,cnt % 2 ? SET : RESET);
        tos_task_delay(1000);
    }
}

extern void application_entry(void);
void task_lora(void *pdata)
{
	 application_entry();
}


void main(void) {
    board_init();

    tos_knl_init();

    tos_task_create(&k_task_lora, "lora", task_lora, NULL, 4, k_lora_stk, LORA_TASK_SIZE, 0);

    tos_knl_start();

}
