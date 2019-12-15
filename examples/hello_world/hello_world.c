#include "cmsis_os.h"

#define TASK1_STK_SIZE          512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void task1(void *arg)
{
    int count = 1;
    while (1) {
        printf("###This is task1, %d\r\n", count++);
        osDelay(2000);
    }
}

void task2(void *arg)
{
    int count = 1;
    while (1) {
#if TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN > 0u
        k_err_t rc;
        int depth;

        rc = tos_task_stack_draught_depth(K_NULL, &depth);
        printf("%d  %d\n", rc, depth);
#endif

        printf("***This is task2, %d\r\n", count++);
        osDelay(1000);
    }
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}
