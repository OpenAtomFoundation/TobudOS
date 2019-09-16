#include "board.h"
#include "cmsis_os.h"

#define APPLICATION_TASK_STK_SIZE       512
extern void application_entry(void *arg);
osThreadDef(application_entry, osPriorityNormal, 1, APPLICATION_TASK_STK_SIZE);

#define TASK1_STK_SIZE		256
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		256
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void task1(void *arg)
{
    int count = 1;
    while (1) {
        LED_On(0);    
        LED_On(1);   
        PRINTF("###This is task1, %d\r\n", count++);
        osDelay(1000);
        LED_Off(0);
        LED_Off(1);
        osDelay(1000);
    }
}

void task2(void *arg)
{
    int count = 1;
    while (1) {
        LED_On(2);  
        osDelay(500);        
        PRINTF("***This is task2, %d\r\n", count++);
        osDelay(500);
        LED_Off(2);
    }
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}


int main(void)
{
    board_init();
    PRINTF("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(application_entry), NULL);
    osKernelStart();
}
