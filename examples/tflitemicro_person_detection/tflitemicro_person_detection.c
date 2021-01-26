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

extern uint16_t camera_buffer[];
extern uint8_t frame_flag;
static uint8_t model_buffer[96*96];

#define TASK1_STK_SIZE          1024
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          1024
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

uint8_t rgb565_to_gray(uint16_t bg_color)
{
    uint8_t bg_r = 0;
    uint8_t bg_g = 0;
    uint8_t bg_b = 0;
    bg_r = ((bg_color>>11)&0xff)<<3;
    bg_g = ((bg_color>>5)&0x3f)<<2;
    bg_b = (bg_color&0x1f)<<2;
    uint8_t gray = (bg_r*299 + bg_g*587 + bg_b*114 + 500) / 1000;
    return gray;
}

void input_convert(uint16_t* camera_buffer , uint8_t* model_buffer)
{
	for(int i=0 ; i<OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT ; i++)
	{
		model_buffer[i] = rgb565_to_gray(camera_buffer[i]);
	}
}

void task1(void *arg)
{
    while (1) {
      if(frame_flag == 1){
				printf("*person_detect_task\r\n");
				if(HAL_DCMI_Stop(&hdcmi))Error_Handler(); //stop DCMI
				input_convert(camera_buffer,model_buffer);
				person_detect(model_buffer);
				LCD_2IN4_Display(camera_buffer,OV2640_PIXEL_WIDTH,OV2640_PIXEL_HEIGHT);
        
				frame_flag = 0;
				
				if(HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_CONTINUOUS,(uint32_t)camera_buffer ,\
					 (OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT)/2))Error_Handler(); //restart DCMI
			}
			osDelay(50);
    }
}

void task2(void *arg)
{
    while (1) {
        //printf("***task2\r\n");
        osDelay(200);
    }
}


void application_entry(void *arg)
{
    printf("***Start person detection task! \r\n");
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}

