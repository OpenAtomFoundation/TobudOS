#include "mcu_init.h"
#include "tos_k.h"
#include "tos_hal.h"

#define BLED_TASK_SIZE 1024
#define WIFI_TASK_SIZE 4096
k_task_t k_task_bled;
k_task_t k_task_wifi;
uint8_t k_bled_stk[BLED_TASK_SIZE];
uint8_t k_wifi_stk[WIFI_TASK_SIZE];


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

void task_bled(void *pdata)
{
    int cnt = 0;
    while (1) {
        printf("blink led task cnt: %d\n", cnt++);
        gpio_bit_write(LED_GPIO_PORT, LED_PIN,cnt % 2 ? SET : RESET);
        tos_task_delay(1000);
    }
}

extern void mqtt_demo();
extern void mqtt_set_esp8266_port(hal_uart_port_t port);
void task_wifi(void *pdata)
{
    while(1) {
        mqtt_set_esp8266_port(HAL_UART_PORT_1);
        mqtt_demo();
        tos_task_delay(500);
    }
}


void main(void) {
    board_init();

    tos_knl_init();

    tos_task_create(&k_task_bled, "bled", task_bled, NULL, 5, k_bled_stk, BLED_TASK_SIZE, 0);
    tos_task_create(&k_task_wifi, "wifi", task_wifi, NULL, 4, k_wifi_stk, WIFI_TASK_SIZE, 0);

    tos_knl_start();

}
