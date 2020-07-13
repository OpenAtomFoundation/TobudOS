#include "mcu_init.h"

k_task_t task1;
k_stack_t task1_stack[1024];

k_task_t task2;
k_stack_t task2_stack[1024];

void task1_fun(void *arg)
{
    int count = 0;

    while (1) {
        printf("###I am task1,count is %d \r\n",count++);
        tos_task_delay(1000);
    }
}

void task2_fun(void *arg)
{
    int count = 0;

    while (1) {
        printf("***I am task2,count is %d \r\n",count++);
        tos_task_delay(2000);
    }
}

int main(void)
{
    k_err_t err;
    board_init();
    printf(" Welcome to TencentOS Tiny!\r\n");
    tos_knl_init(); // TencentOS Tiny kernel initialize    
    err = tos_task_create(&task1, 
                          "task1", 
                          task1_fun,
                          NULL, 
                          3, 
                          task1_stack,
                          1024,
                          20);
    if(err != K_ERR_NONE)
        printf("TencentOS Create task1 fail! code : %d \r\n",err);
    err = tos_task_create(&task2, 
                          "task2", 
                          task2_fun,
                          NULL, 
                          4, 
                          task2_stack,
                          1024,
                          20);
    if(err != K_ERR_NONE)
        printf("TencentOS Create task2 fail! code : %d \r\n",err);
    tos_knl_start(); // Start TencentOS Tiny
}
