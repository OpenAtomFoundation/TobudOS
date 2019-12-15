#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f450i_eval.h"
#include "cmsis_os.h"

#define TASK1_STK_SIZE		512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 2, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void board_init(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    systick_config();
}

void task1(void *arg)
{
    while (1) {
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
        osDelay(2000);
    }
}

void task2(void *arg)
{
    while (1) {      
        gd_eval_led_toggle(LED3);
        osDelay(1000);
    }
}

int main(void)
{
    board_init();
    //printf("Welcome to TencentOS tiny\r\n");
    osKernelInitialize();
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
    osKernelStart();
}
