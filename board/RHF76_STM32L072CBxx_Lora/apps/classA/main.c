#include <stdio.h>
#include <string.h>
#include "tos_k.h"

k_task_t task1;
k_stack_t task1_stack[4096];

extern int lora_main(void);
void task1_fun(void *arg)
{
    lora_main();
}

int main(void)
{
	k_err_t err;
	TOS_CPU_CPSR_ALLOC();
	printf("Welcome to TencentOS Tiny! \r\n");
	tos_knl_init();
	TOS_CPU_INT_DISABLE();
    err = tos_task_create(&task1, 
                          "task1", 
                          task1_fun,
                          NULL, 
                          3, 
                          task1_stack,
                          4096,
                          20);
    if(err != K_ERR_NONE)
        printf("TencentOS Tiny Create task1 fail! code : %d \r\n",err);

	TOS_CPU_INT_ENABLE();
	tos_knl_start();
}

