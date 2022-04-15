#include "mcu_init.h"
#include "tos_shell.h"

#define APPLICATION_TASK_STK_SIZE       0x100
k_task_t application_task;
uint8_t application_task_stk[APPLICATION_TASK_STK_SIZE];
static char gs_cmd_buffer[32];

__attribute__ ((__weak__)) void application_entry(void *arg)
{
    while (1) {
        gpio_bit_toggle(LED_GPIO_Port, LED_Pin);
        tos_task_delay(1000);
    }
}

static void board_shell_output_t(const char ch)
{
    if (ch == '\n')
    {
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
        usart_data_transmit(USART0, '\r');
    }
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    usart_data_transmit(USART0, ch);
}

int main(void)
{
    board_init();
    printf("Welcome to TencentOS tiny(%s)\r\n", TOS_VERSION);
    tos_knl_init();
    tos_task_create(&application_task, "application_task", application_entry, NULL, 4, application_task_stk, APPLICATION_TASK_STK_SIZE, 0);
    tos_shell_init(gs_cmd_buffer, sizeof(gs_cmd_buffer), board_shell_output_t, NULL);
    tos_knl_start();
}

