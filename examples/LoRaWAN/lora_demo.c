#include "lora_demo.h"

void lorawan_demo(void)
{
    int count = 1;
    extern int rhf76_lora_init(hal_uart_port_t uart_port);

    rhf76_lora_init(HAL_UART_PORT_1);
    tos_lora_module_join();

    while (1) {
        if (count % 20 == 0) {
            
            printf("############task1 count is %d \r\n",count++);
            tos_lora_module_send("test data",sizeof("test data"));
        }
        count++;
        osDelay(500);
    }
}


void application_entry(void *arg)
{
    lorawan_demo();
    while (1) {
        printf("This is a lorawan demo!\r\n");
        tos_task_delay(1000);
    }
}
