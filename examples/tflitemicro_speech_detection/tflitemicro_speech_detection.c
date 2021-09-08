/**
 * @file tflitemicro_person_detection.c
 * @author Derekduke (dkeji627@gmail.com); QingChuanWS (bingshan45@163.com); yangqings (yangqingsheng12@outlook.com)
 * @brief  Person detection example based on TencentOS-Tiny and TFlite_Micro.
 * @version 0.1
 * @date 2021-01-05
 * @copyright Copyright (c) 2021
 * 
 */

#include "cmsis_os.h"
#include "mcu_init.h"
#include "micro_speech.h"

uint8_t tensor_flag = 0;

#define TASK1_STK_SIZE          1024
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          1024
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void task1(void *arg)
{
    while (1) {
			printf("***task1\r\n");
			osDelay(1000);
    }
}

void task2(void *arg)
{
		uint8_t* data = NULL;//you can input your own data
		extern void speech_run(uint8_t*); 
		speech_run(data); 
    while (1) {
        printf("***task2\r\n");
        osDelay(2000);
    }
}


void application_entry(void *arg)
{
		extern void speech_init(void);
		speech_init();
    printf("***Start speech detection task! \r\n");
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}

