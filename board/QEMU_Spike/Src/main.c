#include "tos_k.h"

#define TASK_SIZE 512
k_task_t k_task_task1;
k_task_t k_task_task2;
uint8_t k_task1_stk[TASK_SIZE];
uint8_t k_task2_stk[TASK_SIZE];

int share = 123;
void task1(void *pdata)
{
	int t1 = 0;
	while(1)
	{
		t1++;
		share++;
		tos_task_delay(2);
		//tos_task_yield();
	}
}

void task2(void *pdata)
{
	int t2 = 0;
	while(1)
	{
		t2--;
		share--;
		tos_task_delay(3);
		//tos_task_yield();
		//asm("wfi;");
	}
}

int main(void)
{
	tos_knl_init();

	tos_task_create(&k_task_task1, "task1", task1, NULL, 3, k_task1_stk, TASK_SIZE, 0);
	tos_task_create(&k_task_task2, "task2", task2, NULL, 3, k_task2_stk, TASK_SIZE, 0);

	tos_knl_start();


	int c = 0;
	while(1)
	{
		c++;
		asm("wfi;");
	}

}
