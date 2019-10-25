#include "mcu_init.h"
#include "tos.h"

#define TASK_SIZE 1024
k_task_t k_task_task1;
k_task_t k_task_task2;
uint8_t k_task1_stk[TASK_SIZE];
uint8_t k_task2_stk[TASK_SIZE];

int share = 0xCBA7F9;
k_sem_t sem;

void task1(void *pdata)
{
    int task_cnt1 = 0;
    while (1) {
        printf("hello world from %s cnt: %d\n", __func__, task_cnt1++);
        tos_sem_pend(&sem, ~0U);
        gpio_bit_write(GPIOA, GPIO_PIN_7, share % 2);
    }
}

void task2(void *pdata)
{
    int task_cnt2 = 0;
    while (1) {
        share++;
        for(int i=0; i<5; i++) {
            printf("hello world from %s cnt: %08x\n", __func__, task_cnt2--);
            tos_task_delay(50);
        }
        tos_sem_post(&sem);
    }
}


void main(void) {
    board_init();

    usart0_init(115200);

    tos_knl_init();

    tos_task_create(&k_task_task1, "task1", task1, NULL, 3, k_task1_stk, TASK_SIZE, 0);
    tos_task_create(&k_task_task2, "task2", task2, NULL, 3, k_task2_stk, TASK_SIZE, 0);

    k_err_t err = tos_sem_create(&sem, 1);
    if (err != K_ERR_NONE) {
        goto die;
    }

    tos_knl_start();

die:
    while (1) {
        asm("wfi;");
    }
}


int _put_char(int ch)
{
    usart_data_transmit(USART0, (uint8_t) ch );
    while (usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
    }

    return ch;
}
