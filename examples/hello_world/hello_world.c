#include "tos_k.h"

#define TASK1_STK_SIZE       1024
k_task_t task1;
uint8_t task1_stk[TASK1_STK_SIZE];


#define TASK2_STK_SIZE       1024
k_task_t task2;
uint8_t task2_stk[TASK2_STK_SIZE];

void task1_entry(void *arg)
{
    while (1) {
        printf("###I am task1\r\n");
        tos_task_delay(2000);
    }
}

void task2_entry(void *arg)
{
    while (1) {
        printf("***I am task2\r\n");
        tos_task_delay(1000);
    }
}


void application_entry(void *arg)
{
    tos_task_create(&task1, "task1", task1_entry, NULL, 3, task1_stk, TASK1_STK_SIZE, 0); // Create task1
    tos_task_create(&task2, "task2", task2_entry, NULL, 3, task2_stk, TASK2_STK_SIZE, 0);// Create task2
}

