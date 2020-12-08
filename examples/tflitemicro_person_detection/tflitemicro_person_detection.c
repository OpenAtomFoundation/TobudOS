#include "cmsis_os.h"
#include "mcu_init.h"

extern uint16_t camBuffer[];
extern uint8_t frame_flag;

#define TASK1_STK_SIZE          1024
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          1024
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void task1(void *arg)
{
    while (1) {
      if(frame_flag == 1){
				
				if(HAL_DCMI_Stop(&hdcmi))Error_Handler(); //stop DCMI
				
				LCD_2IN4_Display(camBuffer,OV2640_PIXEL_WIDTH,OV2640_PIXEL_HEIGHT);
        
				frame_flag = 0;
				
				if(HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_CONTINUOUS,(uint32_t)camBuffer ,\
					 (OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT)/2))Error_Handler(); //restart DCMI
			}
			osDelay(50);
    }
}

void task2(void *arg)
{
    while (1) {
        printf("***person detection task\r\n");
        osDelay(50);
    }
}


void application_entry(void *arg)
{
    printf("***Start person detection task! \r\n");
    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}

