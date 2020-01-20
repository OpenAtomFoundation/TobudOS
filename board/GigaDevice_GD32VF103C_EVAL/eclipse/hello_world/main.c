#include "mcu_init.h"
#include "tos_k.h"

#define TASK_SIZE 1024
k_task_t k_task_task1;
k_task_t k_task_task2;
uint8_t k_task1_stk[TASK_SIZE];
uint8_t k_task2_stk[TASK_SIZE];

void task1(void *pdata)
{
    int task_cnt1 = 0;
    while (1) {
        printf("hello world from %s cnt: %d\n", __func__, task_cnt1++);
        if (task_cnt1 % 2 == 0)
        {
            gpio_bit_write(GPIOC, GPIO_PIN_0,SET);
            gpio_bit_write(GPIOC, GPIO_PIN_2,SET);
        }
        else
        {
            gpio_bit_write(GPIOC, GPIO_PIN_0,RESET);
            gpio_bit_write(GPIOC, GPIO_PIN_2,RESET);
        }
    	tos_task_delay(1000);
    }
}

void task2(void *pdata)
{
    int task_cnt2 = 0;
    while (1) {
    	printf("hello world from %s cnt: %d\n", __func__, task_cnt2++);
    	if (task_cnt2 %2 == 0)
    	{
        	gpio_bit_write(GPIOE, GPIO_PIN_0,SET);
        	gpio_bit_write(GPIOE, GPIO_PIN_1,SET);
    	}
    	else
    	{
        	gpio_bit_write(GPIOE, GPIO_PIN_0,RESET);
        	gpio_bit_write(GPIOE, GPIO_PIN_1,RESET);
    	}
    	tos_task_delay(500);
    }
}


void main(void) {
    board_init();

    usart0_init(115200);

    tos_knl_init();

    tos_task_create(&k_task_task1, "task1", task1, NULL, 3, k_task1_stk, TASK_SIZE, 0);
    tos_task_create(&k_task_task2, "task2", task2, NULL, 3, k_task2_stk, TASK_SIZE, 0);

    tos_knl_start();

}


int _put_char(int ch)
{
    usart_data_transmit(USART0, (uint8_t) ch );
    while (usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
    }

    return ch;
}
