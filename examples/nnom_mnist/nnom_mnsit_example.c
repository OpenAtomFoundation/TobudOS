/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  **************
	****************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dcmi.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "spi.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "nnom.h"
#include "image.h"
#include "weights.h"

//task1
#define TASK1_STK_SIZE		256
void task1(void *pdata);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);
   
//task2
#define TASK2_STK_SIZE		256
void task2(void *pdata);
	 
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

#ifdef NNOM_USING_STATIC_MEMORY
    uint8_t static_buf[1024*20];
#endif

nnom_model_t *model;
const char codeLib[] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'.   ";
void print_img_nnom(int8_t * buf)
{
    for(int y = 0; y < 28; y++) 
	{
        for (int x = 0; x < 28; x++) 
		{
            int index =  69 / 127.0 * (127 - buf[y*28+x]); 
			if(index > 69) index =69;
			if(index < 0) index = 0;
            printf("%c",codeLib[index]);
			printf("%c",codeLib[index]);
        }
        printf("\n");
    }
}

void mnist_nnom()
{
	uint32_t tick, time;
	uint32_t predic_label;
	float prob;
	int32_t index = 0;
	
	printf("\nprediction start.. \n");
	tick = tos_systick_get();
	
	// copy data and do prediction
	memcpy(nnom_input_data, (int8_t*)&img[index][0], 784);
	nnom_predict(model, &predic_label, &prob);
	time = tos_systick_get() - tick;
	
	//print original image to console
	print_img_nnom((int8_t*)&img[index][0]);
	
	printf("Time: %d tick\n", time);
	printf("Truth label: %d\n", label[index]);
	printf("Predicted label: %d\n", predic_label);
	printf("Probability: %d%%\n", (int)(prob*100));
}
		


void task1(void *pdata)
{
	nnom_set_static_buf(static_buf, sizeof(static_buf));
	model = nnom_model_create();
	model_run(model);
	mnist_nnom();
  while(1)
  {
		osDelay(1000);
  }
}

void task2(void *pdata)
{		
    while(1)
    {
        osDelay(1000);
    }
}


void application_entry(void *arg)
{
    printf("***nnom mnist example\r\n");
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}

