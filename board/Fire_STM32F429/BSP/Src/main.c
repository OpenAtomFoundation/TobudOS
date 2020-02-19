#include "mcu_init.h"
#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE       4096
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 6, APPLICATION_TASK_STK_SIZE);

__weak void application_entry(void *arg)
{
    while (1) {
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

int main(void)
{
    board_init();
    printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize(); //TOS Tiny kernel initialize
    osThreadCreate(osThread(application_entry), NULL);// Create TOS Tiny task
    osKernelStart();//Start TOS Tiny
}

