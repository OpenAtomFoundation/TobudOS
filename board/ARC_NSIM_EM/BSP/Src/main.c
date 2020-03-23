#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE       1024*64

extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

EMBARC_WEAK void application_entry(void *arg)
{
    while (1) {
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

int main(void)
{
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize(); // TOS Tiny kernel initialize
    osThreadCreate(osThread(application_entry), NULL); // Create TOS Tiny task
    osKernelStart(); // Start TOS Tiny
}
