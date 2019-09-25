#include "gd32vf103.h"
#include "tos.h"

#define TASK_SIZE 512
k_task_t k_task_task1;
k_task_t k_task_task2;
uint8_t k_task1_stk[TASK_SIZE];
uint8_t k_task2_stk[TASK_SIZE];

int share = 0xCBA7F9;
k_sem_t sem;

void task1(void *pdata)
{
	int task_cnt1 = 0;
	while(1) {
	    task_cnt1++;
	    tos_sem_pend(&sem, ~0U);
	    gpio_bit_write(GPIOA, GPIO_PIN_7, share % 2);
	}
}

void task2(void *pdata)
{
	int task_cnt2 = 0;
	while(1) {
	    task_cnt2--;
	    share++;
		tos_task_delay(1000);
		tos_sem_post(&sem);
	}
}


void main(void) {
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    gpio_bit_reset(GPIOA, GPIO_PIN_7);

	tos_knl_init();

	tos_task_create(&k_task_task1, "task1", task1, NULL, 3, k_task1_stk, TASK_SIZE, 0);
	tos_task_create(&k_task_task2, "task2", task2, NULL, 3, k_task2_stk, TASK_SIZE, 0);

	k_err_t err = tos_sem_create(&sem, 1);
	if (err != K_ERR_NONE) {
	    goto die;
	}

	tos_knl_start();

die:
	while(1) {
		asm("wfi;");
	}
}
